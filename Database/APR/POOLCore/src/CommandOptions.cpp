/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "POOLCore/CommandOptions.h"
#include <sstream>
#include <iostream>

const char& pool::CommandOptions::optionMarker(){
  static char s_optionMarker('-');
  return s_optionMarker;
}

const pool::CommandOptions::Option& pool::CommandOptions::helpOption(){
  static Option s_helpOpt;
  s_helpOpt.flag = "-h";
  s_helpOpt.type = Option::BOOLEAN;
  s_helpOpt.property = "help";
  s_helpOpt.helpEntry = "display the help";
  return s_helpOpt;
}

pool::CommandOptions::Option::Option():type(BOOLEAN),
                               flag(""),
                               property(""),
                               helpEntry(""),
                               defaultValue(""),
                               required(false){}

pool::CommandOptions::Option::Option(const Option& anOpt):type(anOpt.type),
                               flag(anOpt.flag),
                               property(anOpt.property),
                               helpEntry(anOpt.helpEntry),
                               defaultValue(anOpt.defaultValue),
                               required(anOpt.required){}

pool::CommandOptions::Option& 
pool::CommandOptions::Option::operator=(const Option& anOpt){
  if (&anOpt != this) {
    type = anOpt.type;
    flag = anOpt.flag;
    property = anOpt.property;
    helpEntry = anOpt.helpEntry;
    defaultValue = anOpt.defaultValue;
    required = anOpt.required;

  }
  return *this;
}

bool 
pool::CommandOptions::Option::isEqual(const std::string& s) const {
  std::string shSin = flag;
  std::string lnSin = property;
  if(shSin[0]!=optionMarker()){
    shSin.insert(0,1,optionMarker());
  }
  if(lnSin[0]!=optionMarker()){
    lnSin.insert(0,2,optionMarker());
  }
  if(s==shSin){
    return true;
  } 
  if(s==lnSin){
    return true;
  }
  return false;
}

pool::CommandOptions::OptionParseException::OptionParseException( const std::string& msg ):
  pool::Exception( msg, "CommandOptions::parse", "POOLCore" ){}
 
pool::CommandOptions::CommandOptions(const std::vector<Option>& supportedOptions):
                                     m_supportedOptions(supportedOptions),m_options(),m_commandName(""){
  std::vector<Option>::const_iterator iOpt;
  bool helpFound = false;
  for(iOpt = m_supportedOptions.begin();iOpt!=m_supportedOptions.end();iOpt++){
    if(iOpt->required){
      m_requiredOptions.push_back(iOpt->property);
    }
    if(iOpt->property == helpOption().property) helpFound = true;
  }
  if(!helpFound) m_supportedOptions.push_back(helpOption());
}

std::vector<pool::CommandOptions::Option>::const_iterator 
pool::CommandOptions::searchOption(const std::string& s) const {
  std::vector<Option>::const_iterator ret = m_supportedOptions.end();
  bool found = false;
  for(std::vector<Option>::const_iterator iter=m_supportedOptions.begin();!found && iter!=m_supportedOptions.end();iter++){
    const Option& opt = (*iter);
    if(opt.isEqual(s)){
      found = true;
      ret= iter;
    }
  }
  return ret;
}

bool 
pool::CommandOptions::parse(int argc,char** argv){
  if(argc>=1){
    m_commandName = argv[0];
  }
  m_options.clear();
  std::stringstream sout;
  std::string currentOpt;
  std::map<std::string,std::string> cmdOptions;
  for(int i=1;i<argc;i++){
    std::string singleArg(argv[i]);
    if(argv[i][0]==optionMarker()){
      if(singleArg.size()==1){
        throw OptionParseException("Parsing error: Unbound marker '-' found.");
      }
      if(!currentOpt.empty()){
        cmdOptions.insert(std::make_pair(currentOpt,sout.str()));
      }
      currentOpt = singleArg;
      sout.str("");
    } else {
      if(currentOpt.empty()){
	std::stringstream msg;
        msg << "Parsing error: Unbound parameter " << singleArg << " found.";  
        throw OptionParseException(msg.str());
      }
      if(!sout.str().empty()) sout << " ";
      sout << singleArg;
    }
    if(i+1==argc && !currentOpt.empty()){
      cmdOptions.insert(std::make_pair(currentOpt,sout.str()));
    }
  }
  // check collected options
  std::map<std::string,std::string>::iterator map_iter;
  for(map_iter = cmdOptions.begin();map_iter!=cmdOptions.end();map_iter++){
    std::string theOption =  (*map_iter).first;
    std::string theParameterValue = (*map_iter).second;
    std::vector<Option>::const_iterator opt = searchOption(theOption);
    if(opt==m_supportedOptions.end()){
      std::stringstream msg;
      msg << "Unsupported option " << theOption << " found.";  
      throw OptionParseException(msg.str());
    } 
    if((*opt).type==Option::BOOLEAN){ 
      if(!theParameterValue.empty()){
	std::stringstream msg;
        msg << "Parameter not expected for option " << theOption << ".";  
        throw OptionParseException(msg.str());
      } else {
        theParameterValue = "true";
      }
    } 
    if((*opt).type==Option::STRING && theParameterValue.empty()){
      std::stringstream msg;
      msg << "Parameter required for option " << theOption << " is missing.";  
      throw OptionParseException(msg.str());
    }
    m_options.insert(std::make_pair((*opt).property,theParameterValue));
  }      
  if(m_options.find(helpOption().property)!=m_options.end() || m_options.empty()){
    return true;
  } 
  for(std::vector<std::string>::const_iterator reqOptIter=m_requiredOptions.begin();reqOptIter!=m_requiredOptions.end();reqOptIter++){
    std::map<std::string,std::string>::const_iterator optIter = m_options.find(*reqOptIter);
    if(optIter==m_options.end()){
      std::stringstream msg;
      msg << "Required option " << *reqOptIter << " is missing.";  
      throw OptionParseException(msg.str());      
    }
  }
  return true;
}

std::string 
pool::CommandOptions::commandName() const {
  return m_commandName;
}

const std::map<std::string,std::string>& 
pool::CommandOptions::userOptions() const {
  return m_options;
}

void 
pool::CommandOptions::help() const {
  if(m_commandName==""){
    std::cout << "No command provided." << std::endl;
  } else {
    std::cout << "Usage:" << std::endl;
    std::cout << "        "<< m_commandName << " [opt0] [opt1] ... [optN] (*=mandatory) "<< std::endl;
    std::cout << std::endl;
    for(std::vector<Option>::const_iterator iter=m_supportedOptions.begin();iter!=m_supportedOptions.end();iter++){
      std::cout << "                ";
      std::cout << (*iter).flag << " ( --" << (*iter).property << " ) : " << (*iter).helpEntry;
      if(iter->required) std::cout << " (*)";
      std::cout << std::endl;
    }
  }
}

void 
pool::CommandOptions::dumpUserOptions() const {
  std::map<std::string,std::string>::const_iterator iter;
  std::cout << "Options stored = " << m_options.size() << std::endl;
  if(m_options.size()==0){
    std::cout << "No option specified." << std::endl;
  }
  for(iter = m_options.begin();iter!=m_options.end();iter++){
    std::cout << "option [" << (*iter).first << "] - value=" << (*iter).second << std::endl;
  }
}

