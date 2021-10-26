/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_IInputTOBConverter
#define L1TopoSimulation_IInputTOBConverter

//#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/IAlgTool.h"

namespace TCS {
   class TopoInputEvent;
}

namespace LVL1 {

   /**
   * \class IInputTOBConverter
   *
   * \brief Interface for tools to convert any input to TOBs for topo steering
   */
  class IInputTOBConverter : virtual public extend_interfaces1<IAlgTool> {
  public:
     DeclareInterfaceID(IInputTOBConverter, 0, 1);

     /** \brief partially fills inputEvent from information in SG
      *  \param inputEvent reference to the inputEvent
      */
     virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& inputEvent) const = 0;
  };

}


#endif
