/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_IReadTopoEDM
#define L1TopoSimulation_IReadTopoEDM

//#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"

namespace TCS {
   class TopoInputEvent;
}

namespace LVL1 {

   /**
   * \class IReadTopoEDM
   */
  class IReadTopoEDM : virtual public extend_interfaces1<IAlgTool> {
  public:
     DeclareInterfaceID(IReadTopoEDM, 0, 1);

     virtual StatusCode Read(bool isLegacy) const = 0;
  };

}


#endif
