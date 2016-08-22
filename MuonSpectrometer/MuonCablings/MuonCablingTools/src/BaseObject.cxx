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
    m_display = new __osstream;
}

void
MessageStream::delete_message()
{
    delete m_display;
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
        log << MSG::ERROR << display.str() <<endmsg; 
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
        log << MSG::DEBUG << display.str() <<endmsg; 
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
        log << MSG::WARNING << display.str() <<endmsg; 
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
        log << MSG::INFO << display.str() <<endmsg; 
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

BaseObject&
BaseObject::operator=(const BaseObject& obj)
{
    if(this!=&obj) {
      m_tag  = obj.m_tag;
      m_name = obj.m_name;
      delete m_message;
      m_message = new MessageStream;
    }
    return*this;
}
