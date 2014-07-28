/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigmuFast/MuMessageInterface.h" 


static pthread_mutex_t MuMessageMutex = PTHREAD_MUTEX_INITIALIZER;


#ifdef LVL2_STANDALONE
MuMessageInterface::MuMessageInterface(unsigned short int debug_printout,
                                       unsigned short int info_printout,
				       int max_events,std::string test) :
    m_debug_printout(debug_printout),m_info_printout(info_printout),
    m_max_events(max_events),m_printed_events(0),m_test(test)
{
}
#endif

#ifndef LVL2_STANDALONE
MuMessageInterface::MuMessageInterface(MsgStream& stream, 
                                       unsigned short int debug_printout,
                                       unsigned short int info_printout,
				       int max_events,std::string test) :
    m_stream(stream),m_debug_printout(debug_printout),
    m_info_printout(info_printout),
    m_max_events(max_events),m_printed_events(0),m_test(test)
{
}
#endif

MuMessageInterface::~MuMessageInterface() {}


void
MuMessageInterface::next()
{
    if (m_max_events <0 || (m_debug_printout && m_info_printout)) return;
    m_printed_events++;
    if (m_printed_events > m_max_events) 
    {
        m_debug_printout = 0; 
        m_info_printout  = 0;
    }
}

std::string
MuMessageInterface::testString(void)
{
    return m_test;
}

#ifdef LVL2_STANDALONE
void
MuMessageInterface::display(char* msg) const
{
    cout << msg;
}

void
MuMessageInterface::display(std::string msg) const
{
    cout << msg;
}
#else
void
MuMessageInterface::display(MSG::Level level,char* msg) 
{
    m_stream << level << msg << endreq;
}

void
MuMessageInterface::display(MSG::Level level,std::string msg) 
{
    m_stream << level << msg << endreq;  
}

void
MuMessageInterface::lock() const
{
    pthread_mutex_lock(&MuMessageMutex);
}
void
MuMessageInterface::unlock() const
{
    pthread_mutex_unlock(&MuMessageMutex);
}
#endif


#ifndef LVL2_STANDALONE

// interfaces for gcc 2.95 messaging system
void 
MuMessageInterface::displayDebug(char* msg)   {display (MSG::DEBUG,msg);}

void
MuMessageInterface::displayInfo(char* msg)    {display (MSG::INFO,msg);}
    
void 
MuMessageInterface::displayWarning(char* msg) {display (MSG::WARNING,msg);}
    
void 
MuMessageInterface::displayError(char* msg)   {display (MSG::ERROR,msg);}
    
void 
MuMessageInterface::displayFatal(char* msg)   {display (MSG::FATAL,msg);}



// interfaces for gcc 3.2 messaging system
void 
MuMessageInterface::displayDebug(std::string msg)   {display (MSG::DEBUG,msg);}

void
MuMessageInterface::displayInfo(std::string msg)    {display (MSG::INFO,msg);}
    
void 
MuMessageInterface::displayWarning(std::string msg) {display (MSG::WARNING,msg);}
    
void 
MuMessageInterface::displayError(std::string msg)   {display (MSG::ERROR,msg);}
    
void 
MuMessageInterface::displayFatal(std::string msg)   {display (MSG::FATAL,msg);}

#endif
