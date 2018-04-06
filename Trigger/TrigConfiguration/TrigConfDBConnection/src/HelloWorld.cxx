/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfDBConnection/HelloWorld.h"

#include <iostream>

HelloWorld::HelloWorld() :
   m_message("")
{}


HelloWorld::HelloWorld(const std::string& s) :
   m_message(s)
{}


HelloWorld::~HelloWorld()
{}


void
HelloWorld::greetings()
{
   std::cout << m_message << std::endl;
}


void
HelloWorld::setGreetings(const std::string& s) {
   m_message = s;
}


void
HelloWorld::setStrPtr(const std::string* p) {
   m_message = *p;
}


