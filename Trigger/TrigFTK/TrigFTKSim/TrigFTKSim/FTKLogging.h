/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FTKLogging_H
#define FTKLogging_H

/* class FTKLogging
 *
 * message logger
 * classes derived from FTKLogger may use methods Error() etc
 *
 * Author: Stefan Schmitt, DESY
 * Date: 2013/12/02
 *
 */
#include <iostream>
#include <string>
#include <sstream>

class FTKLogging {
public:
   FTKLogging(const std::string &name) : fName(name),fOut(new std::ostream(0)) { }
   virtual ~FTKLogging();
   std::ostream &Fatal(const char *where) const;
   std::ostream &Error(const char *where) const;
   std::ostream &Warning(const char *where) const;
   std::ostream &Info(const char *where) const;
   std::ostream &Debug(const char *where) const;
   void ShowProgress(const char *text);
   const std::string &GetName(void) const { return fName; }
   static void SetPrintLevel(int);
   static void SetAbortLevel(int);
private:
   static int sPrintLevel,sAbortLevel;
   std::string GetHeader(const char *where) const;
   std::ostream &GetStream(const char *name,int level) const;
   std::string fName;
   std::ostream *fOut;
};

class FTKLogger : public std::streambuf {
public:
   // Athena algorithms should derive from this class
   // and overwrite the "PostMessage" method
   //
   //   class MyAlgo : public AthAlgorithm , public FTKLogger {
   //     ...
   //    protected:
   //     virtual void PostMessage(void);
   //     ...
   //   };
   //
   //   void MyAlgo::PostMessage(void) {
   //        if(fType==0)  ATH_MSG_FATAL(fBuffer->str());
   //        else if(fType==1)  ATH_MSG_ERROR(fBuffer->str());
   //        else if(fType==2)  ATH_MSG_WARNING(fBuffer->str());
   //        else if(fType==3)  ATH_MSG_INFO(fBuffer->str());
   //        else if(fType==4)  ATH_MSG_DEBUG(fBuffer->str());
   //   }
   static FTKLogger *GetLogger(int type,int printLevel,int abortLevel);
protected:
   static FTKLogger *logger;

   FTKLogger(void);
   virtual ~FTKLogger();
   static void SetLogger(FTKLogger *ftkLogger);
   void SetType(int type,int printLevel,int abortLevel);
   //FTKLogger() : fLevel(0) { }
   virtual std::streamsize xsputn ( const char * s, std::streamsize n ); 
   virtual int overflow (int c);
   virtual void PostMessage(void);
   std::ostringstream *fBuffer;
   int fType,fAbortLevel,fPrintLevel;
};

#endif

