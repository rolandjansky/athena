/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventAuxInfo.h"

namespace DerivationFramework {

  class LArCollisionTimeDecorator : public AthAlgTool, public IAugmentationTool {
    public: 
      LArCollisionTimeDecorator(const std::string& type, const std::string& name, const IInterface* parent);

      StatusCode initialize();
      StatusCode finalize();
      virtual StatusCode addBranches() const;

    private:
    
      std::string m_sgName;
      std::string m_containerName;
    
  }; 
}

#endif // DERIVATIONFRAMEWORK_LARCOLLISIONTIMEDECORATOR_H
