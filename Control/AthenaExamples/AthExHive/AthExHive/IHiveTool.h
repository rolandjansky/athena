/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHEXHIVE_IHIVETOOL
#define ATHEXHIVE_IHIVETOOL

#include "GaudiKernel/IAlgTool.h"

class GAUDI_API IHiveTool : virtual public IAlgTool {
public:
   virtual StatusCode doSomething() const = 0;
   virtual ~IHiveTool() {};

  DeclareInterfaceID(IHiveTool,1,0);

};
#endif // !ATHEXHIVE_IHIVETOOL
