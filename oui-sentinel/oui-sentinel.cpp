#define WIN32_LEAN_AND_MEAN   
#include <winsock2.h>         
#include <iphlpapi.h>         
#include <iostream>
#include <iomanip>
#include <bitset>

int main() {
    // ── 1. Buffer inicial ──────────────────────────────────────────────────
    // sizeof(IP_ADAPTER_INFO) é o tamanho de UM adaptador.
    // Se houver mais de um, GetAdaptersInfo retorna ERROR_BUFFER_OVERFLOW
    // e coloca em bufLen o tamanho real necessário → realloc + segunda chamada.
    ULONG bufLen = sizeof(IP_ADAPTER_INFO);
    IP_ADAPTER_INFO* pAdapterInfo = (IP_ADAPTER_INFO*)malloc(bufLen);

    // ── 2. Primeira chamada: detecta se o buffer é suficiente ─────────────
    DWORD ret = GetAdaptersInfo(pAdapterInfo, &bufLen);

    if (ret == ERROR_BUFFER_OVERFLOW) {
        // bufLen foi atualizado com o tamanho correto → realoca
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(bufLen);
        ret = GetAdaptersInfo(pAdapterInfo, &bufLen);
    }

    if (ret != NO_ERROR) {
        std::cerr << "Erro GetAdaptersInfo: " << ret << std::endl;
        free(pAdapterInfo);
        return 1;
    }

    // ── 3. Percorre a lista encadeada de adaptadores ──────────────────────
    IP_ADAPTER_INFO* pAdapter = pAdapterInfo;
    while (pAdapter) {
        std::cout << "\nAdaptador : " << pAdapter->Description << "\n";
        std::cout << "MAC (hex) : ";

        // AddressLength informa quantos bytes do campo Address[] são válidos
        // Para Ethernet/Wi-Fi = 6 bytes  |  Para loopback pode ser 0
        for (UINT i = 0; i < pAdapter->AddressLength; i++) {
            BYTE b = pAdapter->Address[i];

            // ── exibe em hex ──
            std::cout << std::hex << std::uppercase
                      << std::setw(2) << std::setfill('0')
                      << (int)b;
            if (i < pAdapter->AddressLength - 1) std::cout << ":";
        }

        // ── 4. Representação em bitset (pentest / análise de bit flags) ───
        std::cout << "\nMAC (bits): ";
        for (UINT i = 0; i < pAdapter->AddressLength; i++) {
            // std::bitset<8> converte o byte em 8 bits, do MSB ao LSB
            std::bitset<8> bits(pAdapter->Address[i]);
            std::cout << bits;
            if (i < pAdapter->AddressLength - 1) std::cout << " ";
        }

        // ── 5. Bit 0 do primeiro byte: identifica MAC unicast ou multicast ─
        // Em pentest/análise de rede, esse bit é crucial:
        // bit0 = 0 → MAC unicast (endereço de um único dispositivo)
        // bit0 = 1 → MAC multicast/broadcast
        if (pAdapter->AddressLength > 0) {
            BYTE firstByte = pAdapter->Address[0];
            std::cout << "\nTipo      : "
                      << ((firstByte & 0x01) ? "Multicast/Broadcast" : "Unicast");

            // bit1 do primeiro byte: OUI administrado localmente ou globalmente
            // bit1 = 0 → universally administered (fabricante)
            // bit1 = 1 → locally administered (MAC foi alterado — relevante em pentest)
            std::cout << " | Administração: "
                      << ((firstByte & 0x02) ? "Local (possível spoofing)" : "Global (OUI)");
        }

        std::cout << std::dec << "\n";
        pAdapter = pAdapter->Next;  // próximo adaptador na lista
    }

    free(pAdapterInfo);
    return 0;
}