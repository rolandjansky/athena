/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigConfBase/TrigConfMessaging.h"

using namespace TrigConf;

// Sub-classing
class MyClass : public TrigConfMessaging {
public:
  MyClass();
  void print();
};

// Direct use of TrigConf::MsgStream
class MyData {
public:
  void print();
};

MyClass::MyClass() :
  TrigConfMessaging("MyClass")
{ 
}

void MyClass::print()
{
  if (msg(MSGTC::INFO))
    msg() << MSGTC::INFO << "This is a INFO message" << endmsgtc;
  TRG_MSG_ERROR("ERROR message via message stream macros");
  TRG_MSG(WARNING) << "WARNING message" << endmsgtc;
}

void MyData::print()
{
  MsgStreamTC msg("MyData");
  msg << MSGTC::INFO << "INFO message via MsgStream" << endmsgtc;
}

int main() {
  MyClass c;
  c.print();
  MyData d;
  d.print();
}
