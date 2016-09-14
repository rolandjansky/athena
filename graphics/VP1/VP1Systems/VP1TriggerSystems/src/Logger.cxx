/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***********************************************************************************
 * @Package : VP1TriggerSystems
 * @Class   : Logger
 *
 * @brief   : Trigger Messenger Service - common stdcout methods
 *
 * @author  : Manuel Proissl  <mproissl@cern.ch> - University of Edinburgh
 ***********************************************************************************/

//Local includes
#include "VP1TriggerSystems/Logger.h"

//Default Constructor (no class name specified)
//_____________________________________________________________________________________________
VP1Trig::Logger::Logger() 
  : m_name("VP1Trig::?"),m_lvl(0) {}


//Constructor call with class name
//_____________________________________________________________________________________________
VP1Trig::Logger::Logger(const std::string& name) 
  : m_name(name),m_lvl(1) {}


//Output methods without variables
//_____________________________________________________________________________________________
//--- output activated by user: level dependent --------------------->
void VP1Trig::Logger::log_info(const QString& msg) {
  if(m_lvl>=1)
    std::cout<<prefix(m_name)<<"INFO "<<msg.toStdString()<<std::endl;
}
void VP1Trig::Logger::log_warning(const QString& msg) {
  if(m_lvl>=2)
    std::cout<<prefix(m_name)<<"WARNING "<<msg.toStdString()<<std::endl;
}
void VP1Trig::Logger::log_verbose(const QString& msg) {
  if(m_lvl>=3)
    std::cout<<prefix(m_name)<<"VERBOSE "<<msg.toStdString()<<std::endl;
}

//--- output always active: independet of level --------------------->
void VP1Trig::Logger::log_error(const QString& msg) {
  std::cout<<prefix(m_name)<<"ERROR "<<msg.toStdString()<<std::endl;
}
void VP1Trig::Logger::log_fatal(const QString& msg) {
  std::cout<<prefix(m_name)<<"FATAL "<<msg.toStdString()<<std::endl;
}

//--- output can be only used by developers ------------------------->
void VP1Trig::Logger::log_debug(const QString& msg) {
  std::cout<<prefix(m_name)<<"DEBUG "<<msg.toStdString()<<std::endl;
}


//Applying character prefix standard
//_____________________________________________________________________________________________
std::string VP1Trig::Logger::prefix(std::string str)
{
  //15 character prefix standard
  if(int(str.size())>15) {
    str=str.substr(0,15);
    str=str+std::string("...   ");
    return str;
  }
  else if(int(str.size())==15) {
    str=str+std::string("      ");
    return str;
  }
  int q=21-int(str.size());
  for(int i=0;i<q;++i)
    str=str+std::string(" ");
  return str;
}


//VP1String addition for char variables
//_____________________________________________________________________________________________
QString VP1Trig::Logger::qstr(char c)
{
  std::stringstream ss; ss<<c; 
  std::string cStr=ss.str();
  ss.str(std::string()); ss.clear();
  return QString::fromStdString(cStr);
}


//VP1String addition for std::string variables
//_____________________________________________________________________________________________
QString VP1Trig::Logger::qstr(std::string str)
{
  return QString::fromStdString(str);
}
