#include "Network.h"

#define PORT 6789

int main() {
	try
	{
		WSASession Session;
		UDPSocket Socket;
		char buffer[100];

		Socket._bind(PORT);
		while (1) {
			// receive 무한반복
			sockaddr_in add = Socket._recvfrom(buffer, sizeof(buffer));
			std::cout << buffer << std::endl;
			std::string input(buffer);

			// 문자열 역순정렬. 의미없음.
			std::reverse(std::begin(input), std::end(input));
			Socket._sendto(add, input.c_str(), input.size());
		}
	}
	catch (std::system_error& e) {
		std::cout << e.what();
	}

	return 0;
}