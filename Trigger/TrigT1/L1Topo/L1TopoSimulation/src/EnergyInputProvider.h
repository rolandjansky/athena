/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_EnergyInputProvider
#define L1TopoSimulation_EnergyInputProvider


#include "L1TopoSimulation/IInputTOBConverter.h"

namespace LVL1 {

   class EnergyInputProvider : public extends1<AthAlgTool, IInputTOBConverter> {
   public:
      EnergyInputProvider(const std::string& type, const std::string& name, 
                          const IInterface* parent);
      
      virtual ~EnergyInputProvider();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

   private:

      StringProperty m_energyLocation;    //!<  EnergyROI SG key

   };
}

#endif
