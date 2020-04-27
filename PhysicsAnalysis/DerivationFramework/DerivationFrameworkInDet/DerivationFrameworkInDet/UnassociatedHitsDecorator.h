/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// UnassociatedHitsDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_UNASSOCIATEDHITSDECORATOR_H
#define DERIVATIONFRAMEWORK_UNASSOCIATEDHITSDECORATOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthLinks/ElementLink.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

#include "DerivationFrameworkInDet/MinBiasPRDAssociation.h"
#include "DerivationFrameworkInDet/IUnassociatedHitsGetterTool.h"

#include "StoreGate/WriteDecorHandleKey.h"
#include "StoreGate/ReadHandleKey.h"

namespace DerivationFramework {

  class UnassociatedHitsDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      UnassociatedHitsDecorator(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      Gaudi::Property<std::string>  m_sgName
         { this, "DecorationPrefix", "", ""};

      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
         { this, "ContainerName", "EventInfo", ""};

      ToolHandle<IUnassociatedHitsGetterTool> m_UnassociatedHitsGetterTool
         { this, "UnassociatedHitsGetter", "" , ""};

      enum EIntDecor {knPixelUADecor,
                      knBlayerUADecor,
                      knPixelBarrelUADecor,
                      knPixelEndCapAUADecor,
                      knPixelEndCapCUADecor,
                      knSCTUADecor,
                      knSCTBarrelUADecor,
                      knSCTEndCapAUADecor,
                      knSCTEndCapCUADecor,
                      knTRTUADecor,
                      knTRTBarrelUADecor,
                      knTRTEndCapAUADecor,
                      knTRTEndCapCUADecor,
                      kNIntDecor};
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_intDecorKeys;

  }; 
}

#endif // DERIVATIONFRAMEWORK_UNASSOCIATEDHITSDECORATOR_H
