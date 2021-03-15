/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCABLINGTOOLS_LVL1OBJECT_H
#define MUONCABLINGTOOLS_LVL1OBJECT_H


#define DISP this->lock(); \
             m_message->init_message();\
             m_message->message()

#define DISP_DEBUG display_debug(m_message->message()); \
                   m_message->delete_message(); \
		   this->unlock();   
#define DISP_ERROR display_error(m_message->message()); \
                   m_message->delete_message(); \
		   this->unlock();
#define DISP_WARNING display_warning(m_message->message()); \
                     m_message->delete_message(); \
		     this->unlock();
#define DISP_INFO display_info(m_message->message()); \
                  m_message->delete_message(); \
		  this->unlock();
#include <string>
#include <iostream>


#include "MuonCablingTools/OutputDef.h"



#ifndef LVL1_STANDALONE
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#endif



enum ObjectType {Logic,Data,Hardware,Monitoring};


class MessageStream
{
    private:
    __osstream* m_display;
    public:
    void init_message(void);
    void delete_message(void);  
    __osstream& message(void);
};

inline __osstream&
MessageStream::message(void)
{
    return *m_display;
}

class BaseObject
{
    private:
    ObjectType  m_tag;
    std::string m_name;

    protected:
    MessageStream* m_message;


    public:
    BaseObject (ObjectType,std::string);
    BaseObject (ObjectType,const char*);
    BaseObject (const BaseObject&);
    ~BaseObject();

    BaseObject& operator = (const BaseObject&);

    ObjectType  tag(void)  const {return m_tag;}
    std::string name(void) const {return m_name;}

    void Print(std::ostream&,bool) const {}
    void display_warning(__osstream&) const;
    void display_info(__osstream&) const;
    void display_error(__osstream&) const;
    void display_debug(__osstream&) const;
    
    void lock(void) const;
    void unlock(void) const;

#ifndef LVL1_STANDALONE
    //static IMessageSvc* s_message;
#endif
};

#endif
