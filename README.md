# oui-sentinel
Exibe o MAC address de cada adaptador de rede no sistema em formato hexadecimal e binário (bitset), identifica o tipo de endereço (unicast/multicast) e detecta sinais de administração local (possível MAC spoofing).



equisitos

RequisitoVersão mínimaSistema operacionalWindows 10 ou superiorCompiladorMinGW-w64 (GCC 12+) ou MSVC 2019+Windows SDK10.0Bibliotecasiphlpapi.lib, ws2_32.lib (inclusas no SDK)


Como compilar

MinGW-w64 (recomendado para ambiente local)

Instalar o MinGW-w64:

Baixe em https://winlibs.com — escolha a versão Win64 · UCRT · sem LLVM.

Extraia em C:\mingw64 e adicione C:\mingw64\bin nas variáveis de ambiente (PATH).

Verifique:

bashg++ --version

Compilar:

bashcd caminho\para\o\projeto
g++ oui-sentinel.cpp -o oui-sentinel.exe -liphlpapi -lws2_32

Executar:

oui-sentinel.exe
