/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOTOOL_H
#define ATHEXHELLOWORLD_HELLOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExHelloWorld/IHelloTool.h"

#include <string>

class HelloTool : virtual public IHelloTool, virtual public AthAlgTool {
public:
   HelloTool( const std::string&, const std::string&, const IInterface* );

// to allow access to the IHelloTool interface
   StatusCode queryInterface( const InterfaceID& riid, void** ppvIf );

// the magic method this tool provides
   virtual StatusCode saySomething();

private:
   std::string m_myMessage;
};

#endif
