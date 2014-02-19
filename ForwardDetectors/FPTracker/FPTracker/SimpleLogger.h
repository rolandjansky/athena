/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FPTRACKER_SIMPLELOGGER_H
#define FPTRACKER_SIMPLELOGGER_H

// Singleton class for collecting log messages

#include <vector>
#include <string>
#include <sstream>

namespace FPTracker
{
  class SimpleLogger
  {
  public:

    static SimpleLogger* getLogger();

    void debug   ( const std::string& );
    void info    ( const std::string& );
    void warning ( const std::string& );
    void error   ( const std::string& );

    void debug   ( const std::ostringstream& );
    void info    ( const std::ostringstream& );
    void warning ( const std::ostringstream& );
    void error   ( const std::ostringstream& );


    std::vector< std::pair< unsigned int, std::string > > debugMsgs   () const;
    std::vector< std::pair< unsigned int, std::string > > infoMsgs    () const;
    std::vector< std::pair< unsigned int, std::string > > warningMsgs () const;
    std::vector< std::pair< unsigned int, std::string > > errorMsgs   () const;

    std::string debug   () const;
    std::string info    () const;
    std::string warning () const;
    std::string error   () const;


    void reset();

  protected:
    SimpleLogger();

  private:

    static SimpleLogger* m_instance;

    unsigned int         m_index;

    std::vector< std::pair< unsigned int, std::string > > m_debugMsg;
    std::vector< std::pair< unsigned int, std::string > > m_infoMsg;
    std::vector< std::pair< unsigned int, std::string > > m_warningMsg;
    std::vector< std::pair< unsigned int, std::string > > m_errorMsg;

    
  };
}
#endif
