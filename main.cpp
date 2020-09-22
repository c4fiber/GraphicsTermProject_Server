#include <iostream>
#include <cstring>

#include "server.hpp"

using namespace std;

constexpr auto PORT = 45678;

class Game {
public:
	Game() : server(PORT) {
	}

	void run() {
		try {
			server.init();
		}
		catch (string error) {
			perror(error.c_str());
			return ;
		}

		char buff[256];
		while (true) {
			cin >> buff;
			if (!strcmp(buff, "end"))
				break;
		}

		server.close();
	}

	void onAccept(const int &clientSocket) {
		cout << clientSocket << "님이 입장!" << endl;
	}
	void onClose(const int &clientSocket) {
		cout << clientSocket << "님이 퇴장!" << endl;
	}
	void onRecv(const int &clientSocket, const char *buff, const int &recvLen) {
		auto clientSocketList = server.getClientSocketList();
		for (auto targetSocket : clientSocketList) {
			if (targetSocket == clientSocket)
				continue;
			
			send(targetSocket, buff, recvLen, 0);
		}
	}

	Server server;
};

Game game;

int main() {

	game.server.setOnAccept([](const int &clientSocket) {
		game.onAccept(clientSocket);
	});
	game.server.setOnClose([](const int &clientSocket) {
		game.onClose(clientSocket);
	});
	game.server.setOnRecv([](const int &clientSocket, const char *buff, const int &recvLen) {
		game.onRecv(clientSocket, buff, recvLen);
	});

	game.run();

	return 0;
}