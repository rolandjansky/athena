/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonCablingTools/BaseObject.h"

#include <pthread.h>


static pthread_mutex_t StopDisplayStream = PTHREAD_MUTEX_INITIALIZER;

using namespace std;


void
MessageStream::init_message()
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    m_display = new __osstream;
#else
    // put your gcc 2.95 specific code here
    const int buffer_length=10000;
    m_buffer_display = new char[buffer_length];
    for(int i=0;i<buffer_length;++i) m_buffer_display[i]='\0';
    m_display = new __osstream(m_buffer_display,buffer_length);
#endif
}

void
MessageStream::delete_message()
{
#if (__GNUC__) && (__GNUC__ > 2) 
    // put your gcc 3.2 specific code here
    delete m_display;
#else
    // put your gcc 2.95 specific code here
    delete []m_buffer_display;
    delete m_display;
#endif
}



BaseObject::BaseObject(ObjectType tag,std::string name) : 
    m_tag(tag),m_name(name) 
{
    m_message = new MessageStream;
}

BaseObject::BaseObject(ObjectType tag,const char* name) : 
    m_tag(tag)
{
    int i=0;
    while(name[i]!='\0') ++i;
    m_name.resize(i);
    i=0;
    while(name[i]!='\0')
    {
        m_name[i] = name[i];
	++i;
    }
    m_message = new MessageStream;
}

BaseObject::BaseObject(const BaseObject& obj)
{
    m_tag  = obj.tag();
    m_name = obj.name();
    m_message = new MessageStream;
}

BaseObject::~BaseObject()
{
    delete m_message;
}


void
BaseObject::display_error(__osstream& display) const
{
#ifdef LVL1_STANDALONE
    cout << display.str() << endl;
#else
    StatusCode sc;    
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    sc = svcLoc->service( "MessageSvc", msgSvc );
    if(sc.isSuccess()) {
        MsgStream log(msgSvc, name() );
        log << MSG::ERROR << display.str() <<endreq; 
    }
#endif
}

void
BaseObject::display_debug(__osstream& display) const
{
#ifdef LVL1_STANDALONE
    cout << display.str() << endl;
#else
    StatusCode sc;
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    sc = svcLoc->service( "MessageSvc", msgSvc );
    if(sc.isSuccess()) {
        MsgStream log(msgSvc, name() );
        log << MSG::DEBUG << display.str() <<endreq; 
    }
#endif
}

void
BaseObject::display_warning(__osstream& display) const
{
#ifdef LVL1_STANDALONE
    cout << display.str() << endl;
#else
    StatusCode sc;    
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    sc = svcLoc->service( "MessageSvc", msgSvc );
    if(sc.isSuccess()) {
        MsgStream log(msgSvc, name() );
        log << MSG::WARNING << display.str() <<endreq; 
    }
#endif
}

void
BaseObject::display_info(__osstream& display) const
{
#ifdef LVL1_STANDALONE
    cout << display.str() << endl;
#else
    StatusCode sc;
    IMessageSvc*  msgSvc;
    ISvcLocator* svcLoc = Gaudi::svcLocator( );
    sc = svcLoc->service( "MessageSvc", msgSvc );
    if(sc.isSuccess()) {
        MsgStream log(msgSvc, name() );
        log << MSG::INFO << display.str() <<endreq; 
    }
#endif
}

void
BaseObject::lock() const
{
    pthread_mutex_lock(&StopDisplayStream);
}


void
BaseObject::unlock() const
{
    pthread_mutex_unlock(&StopDisplayStream);
}

BaseObject
BaseObject::operator=(const BaseObject& obj)
{
    m_tag  = obj.tag();
    m_name = obj.name();
    return*this;
}
