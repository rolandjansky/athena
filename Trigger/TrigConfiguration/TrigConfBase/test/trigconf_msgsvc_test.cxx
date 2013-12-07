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
  if (msg(MSG::INFO))
    msg() << MSG::INFO << "This is a INFO message" << endmsg;
  TRG_MSG_ERROR("ERROR message via message stream macros");
  TRG_MSG(WARNING) << "WARNING message" << endmsg;
}

void MyData::print()
{
  MsgStream msg("MyData");
  msg << MSG::INFO << "INFO message via MsgStream" << endmsg;
}

int main() {
  MyClass c;
  c.print();
  MyData d;
  d.print();
}
