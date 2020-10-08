#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#include <thread>
#include <list>
#include <set>

using namespace std;

constexpr auto BUFF_SIZE = 1024;

typedef void (*AcceptFunc)(const int &);
typedef void (*CloseFunc)(const int &);
typedef void (*RecvFunc)(const int &, const char *, const int &);

class Server {
public:
	Server(int port);

	void init();
	void close();

	void loop();
	void accept(int clientSocket);
	void recv(int clientSocket);

	inline set<int> getClientSocketList() {
		return clientSocketList;
	}
	// for udp
	inline set<int> getUdpClients() {
		return udpClientList;
	}

	inline void setOnAccept(AcceptFunc onAccept) {
		this->onAccept = onAccept;
	}
	inline void setOnClose(CloseFunc onClose) {
		this->onClose = onClose;
	}
	inline void setOnRecv(RecvFunc onRecv) {
		this->onRecv = onRecv;
	}

private:
	int port;

	int listenSocket = -1;
	set<int> clientSocketList;

	// for udp
	int listen_socketU = -1
	set<int> updClientList;

	thread *loopThread = nullptr;
	bool bThreadRun;
	fd_set targetReads;
	int fdMax;

    list<thread *> eventThreadList;

	AcceptFunc onAccept = nullptr;
	CloseFunc onClose = nullptr;
	RecvFunc onRecv = nullptr;
};
