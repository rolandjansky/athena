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

int FTKLogging::sPrintLevel=4;

int FTKLogging::sAbortLevel=2;

void FTKLogging::SetPrintLevel(int level) {
   sPrintLevel=level;
}

void FTKLogging::SetAbortLevel(int level) {
   sAbortLevel=level;
}

std::string FTKLogging::GetHeader(const char *where) const {
   return fName+"::"+where+" ";
}

std::ostream &FTKLogging::GetStream(const char *name,int level) const {
   fOut->rdbuf(FTKLogger::GetLogger(level,sPrintLevel,sAbortLevel));
   if(name) (*fOut)<<GetHeader(name);
   return *fOut;
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
   if (fOut) delete fOut;
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

FTKLogger *FTKLogger::logger=0;

FTKLogger::FTKLogger(void) {
   fBuffer=0;
   fType=0;
   fAbortLevel = 0;
   fPrintLevel = 0;
}

FTKLogger::~FTKLogger() {
   if(logger==this) SetLogger(0);
}

FTKLogger *FTKLogger::GetLogger(int type,int printLevel,int abortLevel) {
   if(!logger) SetLogger(new FTKLogger());
   logger->SetType(type,printLevel,abortLevel);
   return logger;
}

void FTKLogger::SetLogger(FTKLogger *ftkLogger) {
   logger=ftkLogger;
}

void FTKLogger::SetType(int type,int printLevel,int abortLevel) {
   fType=type;
   fPrintLevel=printLevel;
   fAbortLevel=abortLevel;
}

int FTKLogger::overflow (int c) {
   char s=c;
   if(xsputn(&s,1)!=1) return EOF;
   return std::char_traits<char>::to_int_type(c);
}

std::streamsize FTKLogger::xsputn(const char * s, std::streamsize n) {
   for(int i=0;i<n;i++) {
      if(!fBuffer) fBuffer=new std::ostringstream();
      if(s[i]=='\n') {
         PostMessage();
	 delete fBuffer;
	 fBuffer=0;
      } else {
	 (*fBuffer)<<s[i];
      }
   }
   return n;
}

void FTKLogger::PostMessage(void) {
   static char const *text[]={"FATAL","ERROR","WARNING","INFO","DEBUG"};
   if(fType<fPrintLevel) {
      std::cout<<text[fType]<<"\t"<<fBuffer->str()<<"\n";
   }
   if(fType<fAbortLevel) {
      exit(1+fType);
   }
}
