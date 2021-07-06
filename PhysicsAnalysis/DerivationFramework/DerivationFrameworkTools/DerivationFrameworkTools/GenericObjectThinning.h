/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// GenericObjectThinning.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H
#define DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H

#include <string>
#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "DerivationFrameworkInterfaces/IThinningTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODBase/IParticleContainer.h"
#include "StoreGate/ThinningHandleKey.h"

#include "xAODBase/IParticleContainer.h"
#include "ExpressionEvaluation/ExpressionParserUser.h"

namespace DerivationFramework {

  class GenericObjectThinning : public extends<ExpressionParserUser<AthAlgTool>, IThinningTool> {
    public: 
      GenericObjectThinning(const std::string& t, const std::string& n, const IInterface* p);
      ~GenericObjectThinning();
      virtual StatusCode initialize() override;
      virtual StatusCode finalize() override;
      virtual StatusCode doThinning() const override;

    private:
      StringProperty m_streamName
        { this, "StreamName", "", "Name of the stream being thinned" };
      SG::ThinningHandleKey<xAOD::IParticleContainer> m_SGKey
        { this, "ContainerName", "", "" };

      //Expression for object thinning selection
      std::string m_selectionString;

      //Counters
      mutable std::atomic<unsigned int> m_ntot, m_npass;
  }; 
}

#endif // DERIVATIONFRAMEWORK_GENERICOBJECTTHINNING_H
