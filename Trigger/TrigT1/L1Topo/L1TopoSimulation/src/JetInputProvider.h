/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef L1TopoSimulation_JetInputProvider
#define L1TopoSimulation_JetInputProvider


#include "L1TopoSimulation/IInputTOBConverter.h"

namespace LVL1 {

   class JetInputProvider : public extends1<AthAlgTool, IInputTOBConverter> {
   public:
      JetInputProvider(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~JetInputProvider();

      virtual StatusCode fillTopoInputEvent(TCS::TopoInputEvent& ) const; 

   private:

      StringProperty m_jetLocation;    //!<  Jet ROIs SG key

   };
}

#endif
