/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTKSim/FTKLogging.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>

/* class FTKLogging
 *
 * message logger
 * classes derived from FTKLogging may use methods Error() etc
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

int FTKLogging::s_printLevel=4;

int FTKLogging::s_abortLevel=2;

void FTKLogging::SetPrintLevel(int level) {
   s_printLevel=level;
}

void FTKLogging::SetAbortLevel(int level) {
   s_abortLevel=level;
}

std::string FTKLogging::GetHeader(const char *where) const {
   return m_name+"::"+where+" ";
}

std::ostream &FTKLogging::GetStream(const char *name,int level) const {
   m_out->rdbuf(FTKLogger::GetLogger(level,s_printLevel,s_abortLevel));
   if(name) (*m_out)<<GetHeader(name);
   return *m_out;
}


std::ostream &FTKLogging::Fatal(const char *where) const {
   return GetStream(where,0);
}

std::ostream &FTKLogging::Error(const char *where) const {
   return GetStream(where,1);
}

std::ostream &FTKLogging::Warning(const char *where) const {
   return GetStream(where,2);
}

std::ostream &FTKLogging::Info(const char *where) const {
   return GetStream(where,3);
}

std::ostream &FTKLogging::Debug(const char *where) const {
   return GetStream(where,4);
}

void FTKLogging::ShowProgress(const char *text) {
   GetStream(0,4)<<text<<"\n";
}

FTKLogging::~FTKLogging() {
   if (m_out) delete m_out;
}

/* class FTKLogger
 *
 * message logger
 * classes derived from FTKLogging may use methods Error() etc
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */

FTKLogger *FTKLogger::s_logger=0;

FTKLogger::FTKLogger(void) {
   m_buffer=0;
   m_type=0;
   m_abortLevel = 0;
   m_printLevel = 0;
}

FTKLogger::~FTKLogger() {
   if(s_logger==this) SetLogger(0);
}

FTKLogger *FTKLogger::GetLogger(int type,int printLevel,int abortLevel) {
   if(!s_logger) SetLogger(new FTKLogger());
   s_logger->SetType(type,printLevel,abortLevel);
   return s_logger;
}

void FTKLogger::SetLogger(FTKLogger *ftkLogger) {
   s_logger=ftkLogger;
}

void FTKLogger::SetType(int type,int printLevel,int abortLevel) {
   m_type=type;
   m_printLevel=printLevel;
   m_abortLevel=abortLevel;
}

int FTKLogger::overflow (int c) {
   char s=c;
   if(xsputn(&s,1)!=1) return EOF;
   return std::char_traits<char>::to_int_type(c);
}

std::streamsize FTKLogger::xsputn(const char * s, std::streamsize n) {
   for(int i=0;i<n;i++) {
      if(!m_buffer) m_buffer=new std::ostringstream();
      if(s[i]=='\n') {
         PostMessage();
	 delete m_buffer;
	 m_buffer=0;
      } else {
	 (*m_buffer)<<s[i];
      }
   }
   return n;
}

void FTKLogger::PostMessage(void) {
   static char const *text[]={"FATAL","ERROR","WARNING","INFO","DEBUG"};
   if(m_type<m_printLevel) {
      std::cout<<text[m_type]<<"\t"<<m_buffer->str()<<"\n";
   }
   if(m_type<m_abortLevel) {
      exit(1+m_type);
   }
}
