// umlrttcpprotocol.hh

#ifndef UMLRTTCPPROTOCOL_HH
#define UMLRTTCPPROTOCOL_HH

#include "umlrtprotocol.hh"
#include "umlrtsignal.hh"
#include "umlrtobjectclass.hh"
#include "umlrttcpservice.hh"

struct UMLRTCommsPort;

class UMLRTTCPProtocol
{
public:
    enum SignalId { signal_received = UMLRTSignal::FIRST_PROTOCOL_SIGNAL_ID,
    	signal_connected,
		signal_disconnected,
		signal_error };

    struct params_received
    {
        char * message;
        int size;
    };

    static UMLRTObject_field fields_received[];
    static UMLRTObject payload_received;

    struct params_error
    {
        int errno;
    };

    static UMLRTObject_field fields_error[];
    static UMLRTObject payload_error;

    static UMLRTObject_field fields_connected[];
    static UMLRTObject payload_connected;

    static UMLRTObject_field fields_disconnected[];
    static UMLRTObject payload_disconnected;

    class InSignals {  };
    class OutSignals {
    public:
        
        const void connect( const char * address ) const;
        const void disconnect( ) const;
        const void send( const char* message, int size ) const;
    };

    typedef OutSignals Base;
    typedef InSignals Conjugate;
};

class UMLRTTCPProtocol_baserole : protected UMLRTProtocol, private UMLRTTCPProtocol::Base
{
public:
    UMLRTTCPProtocol_baserole( const UMLRTCommsPort *& srcPort ) : UMLRTProtocol( srcPort ) { }

    const void connect( const char * host, int port ) const
    {
    	UMLRTTCPService::connekt(srcPort, host, port);
    }

    const void disconnect( ) const
    {
    	UMLRTTCPService::disconnect(srcPort);
    }

    const void send( const char* message, int size ) const
    {
    	UMLRTTCPService::send(srcPort, message, size);
    }
};

#endif // UMLRTTCPPROTOCOL_HH
