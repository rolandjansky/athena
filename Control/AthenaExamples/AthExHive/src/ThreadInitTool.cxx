#include "unistd.h"
#include "sys/syscall.h"

#include "ThreadInitTool.h"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

ThreadInitTool::ThreadInitTool( const std::string& type, const std::string& name,
                        const IInterface* parent ) 
  : base_class(type, name, parent),
    m_nInitThreads(0)

{
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void
ThreadInitTool::initThread()
{
  ATH_MSG_INFO ("initThread in thread 0x" << std::hex << pthread_self() 
		<< " at " << this << std::dec );


  // Thread Local initializations would go here.


  m_nInitThreads++;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

void
ThreadInitTool::terminateThread()
{
  ATH_MSG_INFO ("terminateThread in thread 0x"
		<< std::hex << pthread_self() << std::dec );


  m_nInitThreads--;

}


