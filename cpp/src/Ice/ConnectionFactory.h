// **********************************************************************
//
// Copyright (c) 2002
// MutableRealms, Inc.
// Huntsville, AL, USA
//
// All Rights Reserved
//
// **********************************************************************

#ifndef ICE_CONNECTION_FACTORY_H
#define ICE_CONNECTION_FACTORY_H

#include <IceUtil/Mutex.h>
#include <Ice/ConnectionFactoryF.h>
#include <Ice/ConnectionF.h>
#include <Ice/InstanceF.h>
#include <Ice/ObjectAdapterF.h>
#include <Ice/EndpointF.h>
#include <Ice/AcceptorF.h>
#include <Ice/TransceiverF.h>
#include <Ice/EventHandler.h>
#include <list>

namespace Ice
{

class LocalException;

}

namespace IceInternal
{

class OutgoingConnectionFactory : public ::IceUtil::Shared, public ::IceUtil::Mutex
{
public:

    OutgoingConnectionFactory(const InstancePtr&);
    virtual ~OutgoingConnectionFactory();

    void destroy();
    ConnectionPtr create(const std::vector<EndpointPtr>&);

private:

    InstancePtr _instance;
    std::map<EndpointPtr, ConnectionPtr> _connections;
};

class IncomingConnectionFactory : public EventHandler, public ::IceUtil::Mutex
{
public:

    IncomingConnectionFactory(const InstancePtr&, const EndpointPtr&, const ::Ice::ObjectAdapterPtr&);
    virtual ~IncomingConnectionFactory();

    void destroy();
    void hold();
    void activate();

    EndpointPtr endpoint() const;
    bool equivalent(const EndpointPtr&) const;

    //
    // Operations from EventHandler
    //
    virtual bool server() const;
    virtual bool readable() const;
    virtual void read(BasicStream&);
    virtual void message(BasicStream&);
    virtual void exception(const ::Ice::LocalException&);
    virtual void finished();
    virtual bool tryDestroy();
    
private:

    enum State
    {
	StateActive,
	StateHolding,
	StateClosed
    };

    void setState(State);
    void clearBacklog();
    void warning(const ::Ice::LocalException&) const;

    EndpointPtr _endpoint;
    ::Ice::ObjectAdapterPtr _adapter;
    ThreadPoolPtr _threadPool;
    AcceptorPtr _acceptor;
    TransceiverPtr _transceiver;
    std::list<ConnectionPtr> _connections;
    State _state;
    bool _warn;
};

}

#endif
