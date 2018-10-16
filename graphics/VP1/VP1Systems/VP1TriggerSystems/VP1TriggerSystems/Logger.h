/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VP1TrigLogger_h
#define VP1TrigLogger_h

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @class   : Logger
 *
 * @brief   : Trigger Messenger Service (Header) - common stdcout methods
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Includes
#include "VP1Base/VP1String.h"
#include <QString>
#include <string>
#include <iostream>
#include <sstream>

namespace VP1Trig {
  class Logger {
  public:
    Logger(const std::string& name);
    Logger();
    virtual ~Logger() {}
    
    //User dependent: level setting
    void log_info(const QString& msg);
    void log_warning(const QString& msg);
    void log_verbose(const QString& msg);
        
    //Always active
    void log_error(const QString& msg);
    void log_fatal(const QString& msg);
    
    //Developer dependent
    void log_debug(const QString& msg);
    
    //Prefix
    std::string prefix(std::string str);
    
    //Addon to VP1String class
    QString qstr(char c);
    QString qstr(std::string str);
    
    //Sub-output prefix definition
    static const QString pfx1() {return " ";}
    static const QString pfx2() {return "  ";}
    static const QString pfx3() {return "   ";}
    
  private:
    std::string m_name;
    int m_lvl;
  };
}
#endif
