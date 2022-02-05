// Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#ifndef L1TopoSimulation_TopoEDM
#define L1TopoSimulation_TopoEDM

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1TopoSimulation/IReadTopoEDM.h"

//EM/Tau EDMs
#include "xAODTrigger/L1TopoSimResultsContainer.h"


namespace LVL1 {

   class TopoEDM : public AthAlgTool, virtual public IReadTopoEDM {
   public:
      TopoEDM(const std::string& type, const std::string& name, 
                         const IInterface* parent);
      
      virtual ~TopoEDM();

      virtual StatusCode initialize() override final;

      virtual StatusCode Read(bool isLegacy) const override final; 

   private:

     SG::ReadHandleKey<xAOD::L1TopoSimResultsContainer> m_l1topoKey {this, "L1_TopoKey", "L1_TopoSimResults", "l1topo EDM"};
     SG::ReadHandleKey<xAOD::L1TopoSimResultsContainer> m_legacyL1topoKey {this, "L1_LegacyTopoKey", "L1_LegacyTopoSimResults", "l1topo EDM"};
   };
}

#endif
