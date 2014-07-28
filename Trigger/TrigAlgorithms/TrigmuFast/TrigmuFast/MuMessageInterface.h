/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PHYSICS_MU_MUMESSAGEINTERFACE_H
#define PHYSICS_MU_MUMESSAGEINTERFACE_H



#ifdef LVL2_STANDALONE

#include <iostream>

#else

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#endif

#include <pthread.h>


class MuMessageInterface
{
    public:
#ifdef LVL2_STANDALONE
    MuMessageInterface(unsigned short int debug_printout,
                       unsigned short int info_printout,
		       int max_events,std::string test);
    void display(char* msg) const;
    void display(std::string) const;
#else
    MuMessageInterface(MsgStream& stream, unsigned short int debug_printouts,
                       unsigned short int info_printout,int max_events,
		       std::string test);

    void displayDebug(char* msg);
    void displayInfo(char* msg);
    void displayWarning(char* msg);
    void displayError(char* msg);
    void displayFatal(char* msg);
    
    void displayDebug(std::string msg);
    void displayInfo(std::string msg);
    void displayWarning(std::string msg);
    void displayError(std::string msg);
    void displayFatal(std::string msg);

    void display(MSG::Level level,char* msg);
    void display(MSG::Level level,std::string msg);
    
    void lock(void) const;
    void unlock(void) const;
    
    void next(void);
    
    std::string testString(void);

#endif
    ~MuMessageInterface();

    private:
#ifndef LVL2_STANDALONE
    MsgStream m_stream;
#endif


    unsigned short int m_debug_printout;
    unsigned short int m_info_printout;
    
    int m_max_events;
    int m_printed_events;
    std::string m_test;

    public:
    unsigned short int  debug_printout(void) const {return m_debug_printout;}
    unsigned short int  info_printout(void)  const {return m_info_printout;}

};


#endif  // PHYSICS_MU_MUMESSAGEINTERFACE_H
