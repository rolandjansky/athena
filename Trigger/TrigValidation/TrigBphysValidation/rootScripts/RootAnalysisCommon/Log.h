/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///===================================================
/// Log class
///---------------------------------------------------
/// Logging tool
///
/// Author: Daniel Scheirich <scheiric@cern.ch>
/// Part of B2JpsiV0RootAnalysis
///===================================================

#include <iostream>
#include <sstream>
#include <iomanip>

#ifndef Log_H
#define Log_H

typedef int OutputLevel;

const int ERROR   = 0;
const int WARNING = 1;
const int INFO    = 2;
const int DEBUG   = 3;

enum StatusCode {SUCCESS, ACCEPT, FAILURE, REJECT };

class Log {
  public:
    Log(std::string inMethod, OutputLevel ol);
    ~Log();
    void print(OutputLevel outputLevel, std::string text);

    void error(std::string text) { print(ERROR, text); }
    void warning(std::string text) { print(WARNING, text); }
    void info(std::string text) { print(INFO, text); }
    void debug(std::string text) { print(DEBUG, text); }

  private:

    std::string m_inMethod;
    OutputLevel m_outputLevel;

};
//************************************************************

OutputLevel strToOl(std::string str);
std::string iToStr(int value);
std::string fToStr(double value);

Log::Log(std::string inMethod, OutputLevel ol) : m_inMethod(inMethod), m_outputLevel(ol) {
  print(DEBUG, "In method " + m_inMethod + "...");
}

Log::~Log() {
  print(DEBUG, "Leaving " + m_inMethod);
}

void Log::print(OutputLevel outputLevel, std::string text) {
  if(outputLevel<=m_outputLevel) {
    std::string level;
    switch(outputLevel) {
      case ERROR: level = "ERROR"; break;
      case WARNING: level = "WARNING"; break;
      case INFO: level = "INFO"; break;
      default: level = "DEBUG"; break;
    }
    std::string tmp1 = "["+m_inMethod+"] ";
    std::string tmp2 = level+": ";
    std::cout << std::setw(45) << std::left << tmp1 << std::setw(10) <<std::right<< tmp2 << text<<std::endl;
  }
}
//************************************************************
OutputLevel strToOl(std::string str) {
  if(str=="DEBUG") return DEBUG;
  if(str=="INFO") return INFO;
  if(str=="WARNING") return WARNING;
  if(str=="ERROR") return ERROR;

  return INFO;

}
//************************************************************
std::string iToStr(int value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}
//************************************************************
std::string fToStr(double value) {
  std::stringstream ss;
  ss << value;
  return ss.str();
}
//************************************************************

#endif
