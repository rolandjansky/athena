/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LArCollisionTimeDecorator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_LARCOLLISIONTIMEDECORATOR_H
#define DERIVATIONFRAMEWORK_LARCOLLISIONTIMEDECORATOR_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthLinks/ElementLink.h"

#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteDecorHandleKey.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "LArRecEvent/LArCollisionTime.h"

namespace DerivationFramework {

  class LArCollisionTimeDecorator : public AthAlgTool, public IAugmentationTool {
    public:
      LArCollisionTimeDecorator(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:

      Gaudi::Property<std::string> m_sgName
        { this, "DecorationPrefix", "", ""};
      SG::ReadHandleKey<xAOD::EventInfo> m_eventInfoKey
        { this, "ContainerName", "EventInfo", "" };
      SG::ReadHandleKey<LArCollisionTime> m_larCollisionTimeKey
        { this, "LArCollisionTimeKey", "LArCollisionTime", ""};

      enum EIntDecor   {kncellA, kncellC, kNIntDecor };
      enum EFloatDecor {kenergyA,kenergyC,ktimeA,ktimeC, kNFloatDecor };

      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_intDecorKeys;
      std::vector<SG::WriteDecorHandleKey<xAOD::EventInfo> > m_floatDecorKeys;

  };
}

#endif // DERIVATIONFRAMEWORK_LARCOLLISIONTIMEDECORATOR_H
