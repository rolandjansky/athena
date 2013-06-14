/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HelloWorld_h
#define HelloWorld_h

#include <string>

class HelloWorld {
public:
   HelloWorld();
   HelloWorld(const std::string& s);
   virtual ~HelloWorld();

   void greetings();
   void setGreetings(const std::string& s);
   void setStrPtr(const std::string* p);

private:

   std::string fMessage;

};

#endif
