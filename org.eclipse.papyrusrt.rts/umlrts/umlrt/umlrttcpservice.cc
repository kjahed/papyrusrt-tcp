// umlrttcpservice.cc

#include "basedebug.hh"
#include "basefatal.hh"
#include "umlrtoutsignal.hh"
#include "umlrttcpservice.hh"
#include "umlrttcpprotocol.hh"
#include "umlrtcontroller.hh"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>

UMLRTHashMap * UMLRTTCPService::portSocketMap = NULL;
UMLRTQueue UMLRTTCPService::outQueue;

UMLRTHashMap * UMLRTTCPService::getPortSocketMap ( )
{
    if (portSocketMap == NULL)
    {
    	portSocketMap = new UMLRTHashMap("portSocketMap", UMLRTHashMap::compareString, false/*objectIsString*/);
    }
    return portSocketMap;
}

int UMLRTTCPService::getSocket ( const UMLRTCommsPort * port )
{
	UMLRTHashMap * portSocketMap = getPortSocketMap();
	int * sockPtr = (int*) portSocketMap->getObject(port);
	if(sockPtr == NULL)
		return -1;
	return *sockPtr;
}

void UMLRTTCPService::connekt ( const UMLRTCommsPort * port, const char* host, int tcpPort )
{
	int sock = getSocket(port);
	if(sock == -1) {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		printf("Socket init: %d\n", sock);
		getPortSocketMap()->insert(port, (void *)new int(sock));
	}

	struct sockaddr_in addr;
	memset(&addr, '0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(tcpPort);

	if(inet_pton(AF_INET, host, &addr.sin_addr) <= 0)
	{
		UMLRTOutSignal signal;
		signal.initialize( "error", UMLRTTCPProtocol::signal_error, port, &UMLRTTCPProtocol::payload_error, new int(0));
		port->slot->controller->deliver( port, signal, port->roleIndex );
		return;
	}

    UMLRTOutSignal signal;
	int ret = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
	if( ret < 0 )
		signal.initialize( "error", UMLRTTCPProtocol::signal_error, port, &UMLRTTCPProtocol::payload_error, new int(0));
	else
		signal.initialize( "connected", UMLRTTCPProtocol::signal_connected, port, &UMLRTTCPProtocol::payload_connected);
	//signal.send();

	port->slot->controller->deliver( port, signal, port->roleIndex );

	printf("Connect return %d\n", ret);
}

void UMLRTTCPService::disconnect ( const UMLRTCommsPort * port )
{
	int socket = getSocket(port);
	if(socket != -1) {
		UMLRTOutSignal signal;

		if (close(socket) < 0) {
		    signal.initialize( "error", UMLRTTCPProtocol::signal_error, port, &UMLRTTCPProtocol::payload_error, new int(0));
		}
		else {
			signal.initialize( "disconnected", UMLRTTCPProtocol::signal_disconnected, port, &UMLRTTCPProtocol::payload_disconnected);
		}

		port->slot->controller->deliver( port, signal, port->roleIndex );
	}
}

void UMLRTTCPService::send ( const UMLRTCommsPort * port, const char* msg, int size )
{
	UMLRTTCPService::Message * message = new UMLRTTCPService::Message;
	message->port = port;
	message->msg = msg;
	message->size = size;
	outQueue.enqueue(message);
}

// Main loop
void * UMLRTTCPService::run ( void * args )
{
    printf("TCP service running.\n");

    while(1) {
    	UMLRTTCPService::Message* msg;
		while((msg = (UMLRTTCPService::Message*) outQueue.dequeue()) != NULL) {
			int socket = getSocket(msg->port);
			int ret = write(socket, msg->msg, msg->size);
			if (ret  < 0 )
			{
				UMLRTOutSignal signal;
				signal.initialize( "error", UMLRTTCPProtocol::signal_error, msg->port, &UMLRTTCPProtocol::payload_error, new int(0));
				msg->port->slot->controller->deliver( msg->port, signal, msg->port->roleIndex );
			}
		}

		getPortSocketMap()->lock();
		UMLRTHashMap::Iterator clIter = getPortSocketMap()->getIterator();

		while (clIter != clIter.end())
		{
			const UMLRTCommsPort * port = (UMLRTCommsPort *) clIter.getKey();
			int socket = *((int*) clIter.getObject());
			char* buffer = new char[TCP_BUFFER_SIZE];

			int bytes = recv(socket, buffer, TCP_BUFFER_SIZE, MSG_DONTWAIT);
			if(bytes > 0) {
				UMLRTOutSignal signal;
				signal.initialize( "received", UMLRTTCPProtocol::signal_received, port, &UMLRTTCPProtocol::payload_received, &buffer, &bytes);
				port->slot->controller->deliver( port, signal, port->roleIndex );
			} else {
				delete buffer;
			}

			clIter = clIter.next();
		}

		getPortSocketMap()->unlock();
    }
}
