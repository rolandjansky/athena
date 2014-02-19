/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "FPTracker/SimpleLogger.h"
#include <algorithm>

namespace FPTracker
{
  struct Consolidator
  {
    void operator()( const std::pair< unsigned int, std::string >& element)
    {
      m_msg += element.second;
    }
    std::string m_msg;
  };
  
  std::string consolidate(std::vector< std::pair< unsigned int, std::string > > msgs)
  {
    return std::for_each( msgs.begin(), msgs.end(), Consolidator() ).m_msg;
  }
  
  SimpleLogger* SimpleLogger::m_instance = 0;
  SimpleLogger*  SimpleLogger::getLogger()
    {
      if ( m_instance == 0 )
	{
	  m_instance = new SimpleLogger;
	}
      return m_instance;
    }

  void SimpleLogger::debug  (const std::string& msg)
  { 
    m_debugMsg.push_back  ( std::pair< unsigned int, std::string >( m_index, msg) ); 
    ++m_index;
  }
  
  
  void SimpleLogger::info   (const std::string& msg)
  {
    m_infoMsg.push_back ( std::pair< unsigned int, std::string >( m_index, msg) );
    ++m_index;
  }

  void SimpleLogger::warning(const std::string& msg)
  { 
    m_warningMsg.push_back( std::pair< unsigned int, std::string >( m_index, msg) );
    ++m_index;
  }

  void SimpleLogger::error  (const std::string& msg)
  { 
    m_errorMsg.push_back  ( std::pair< unsigned int, std::string >( m_index, msg) ); 
    ++m_index;
  }


  void SimpleLogger::debug  (const std::ostringstream& msg)
  {
    this->debug( msg.str() );
  }
  
  
  void SimpleLogger::info   (const std::ostringstream& msg)
  {
    this->info( msg.str() );
  }

  void SimpleLogger::warning(const std::ostringstream& msg)
  { 
    this->warning( msg.str() );
  }

  void SimpleLogger::error  (const std::ostringstream& msg)
  { 
    this->error( msg.str() );
  }



  std::vector< std::pair< unsigned int, std::string > >
  SimpleLogger::debugMsgs  () const { return m_debugMsg; }
  
  std::vector< std::pair< unsigned int, std::string > >
  SimpleLogger::infoMsgs   () const { return m_infoMsg; }

  std::vector< std::pair< unsigned int, std::string > >
  SimpleLogger::warningMsgs() const { return m_warningMsg; }

  std::vector< std::pair< unsigned int, std::string > >
  SimpleLogger::errorMsgs  () const { return m_errorMsg; }

  std::string
  SimpleLogger::debug  () const { return consolidate(m_debugMsg); }
  
  std::string
  SimpleLogger::info   () const { return consolidate(m_infoMsg); }

  std::string
  SimpleLogger::warning() const { return consolidate(m_warningMsg); }

  std::string
  SimpleLogger::error  () const { return consolidate(m_errorMsg); }

  void
  SimpleLogger::reset()
  {
    
    m_index = 0;
    
    m_debugMsg.erase  ( m_debugMsg.begin(),    m_debugMsg.end()   );
    m_infoMsg.erase   ( m_infoMsg.begin(),     m_infoMsg.end()    );
    m_warningMsg.erase( m_warningMsg.begin(),  m_warningMsg.end() );
    m_errorMsg.erase  ( m_errorMsg.begin(),    m_errorMsg.end()   );
  }

SimpleLogger::SimpleLogger():m_index(0){}
}
