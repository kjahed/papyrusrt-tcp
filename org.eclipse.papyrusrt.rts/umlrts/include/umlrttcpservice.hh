// umlrttcpservice.hh

#ifndef UMLRTTCPSERVICE_HH
#define UMLRTTCPSERVICE_HH

#include "umlrtbasicthread.hh"
#include "umlrtqueue.hh"
#include "umlrtqueueelement.hh"
#include "umlrtcommsport.hh"
#include "umlrthashmap.hh"

#ifndef TCP_BUFFER_SIZE
#define TCP_BUFFER_SIZE 1024
#endif

class UMLRTTCPService : UMLRTBasicThread
{
public:
	UMLRTTCPService ( ) : UMLRTBasicThread("UMLRTTCPService")
	{
		start(NULL);
	}
    
    class Message : public UMLRTQueueElement {
	public:
		const UMLRTCommsPort * port;
		const char* msg;
		int size;
    };

    static void connekt ( const UMLRTCommsPort * port, const char* host, int tcpPort );
    static void disconnect ( const UMLRTCommsPort * port );
    static void send ( const UMLRTCommsPort * port, const char* msg, int size );
    static void queueMessage ( UMLRTTCPService::Message * msg );

private:
    virtual void * run ( void * args );

    static UMLRTQueue outQueue;

    static int getSocket ( const UMLRTCommsPort * port );
    static UMLRTHashMap * getPortSocketMap ( );

    static UMLRTHashMap * portSocketMap;

};

#endif // UMLRTTCPSERVICE_HH
