/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHELLOWORLD_HELLOTOOL_H
#define ATHEXHELLOWORLD_HELLOTOOL_H 1

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthExHelloWorld/IHelloTool.h"

#include <string>

class HelloTool : virtual public extends<AthAlgTool, IHelloTool> {
public:
   HelloTool( const std::string&, const std::string&, const IInterface* );

// the magic method this tool provides
   virtual StatusCode saySomething();

private:
  Gaudi::Property<std::string> m_myMessage {this, "MyMessage", 
      "Default message set in HelloTool.h", "something to say"};
};

#endif
