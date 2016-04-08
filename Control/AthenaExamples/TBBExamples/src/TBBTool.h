/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOTOOL_H
#define ATHEXHELLOWORLD_HELLOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "TBBExamples/IHelloTool.h"

#include <string>

class TBBTool : virtual public IHelloTool, virtual public AthAlgTool {
public:
   TBBTool( const std::string&, const std::string&, const IInterface* );

// to allow access to the IHelloTool interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

// the magic method this tool provides
   virtual StatusCode saySomething(std::string& something);
   virtual int addToIt(int soFar);

private:
   std::string m_myMessage;
   int m_increment;
   int m_called;
};

#endif
