/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfDBConnection/HelloWorld.h"

#include <iostream>

HelloWorld::HelloWorld() :
   fMessage("")
{}


HelloWorld::HelloWorld(const std::string& s) :
   fMessage(s)
{}


HelloWorld::~HelloWorld()
{}


void
HelloWorld::greetings()
{
   std::cout << fMessage << std::endl;
}


void
HelloWorld::setGreetings(const std::string& s) {
   fMessage = s;
}


void
HelloWorld::setStrPtr(const std::string* p) {
   fMessage = *p;
}


