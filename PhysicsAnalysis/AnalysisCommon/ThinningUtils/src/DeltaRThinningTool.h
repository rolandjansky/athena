/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef ThinningUtils_DeltaRThinningTool_H
#define ThinningUtils_DeltaRThinningTool_H

// athena includes
#include "DerivationFrameworkInterfaces/IForwardParentThinningTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IThinningSvc.h"

/**
 * @class DeltaRThinningTool
 * @brief Keep objects from a child container that lie within deltaR cones of
 * kept objects from a parent container.
 *
 * @author Jon Burr
 */

class DeltaRThinningTool 
  : virtual public IForwardParentThinningTool,
            public AthAlgTool
{
  public:
    DeltaRThinningTool(
        const std::string& type,
        const std::string& name,
        const IInterface* parent);

    ~DeltaRThinningTool() {}
    
    StatusCode initialize() override;

    /// For each parent particle that passes the thinning keep child particles
    /// within DR of it.
    StatusCode forwardParentThinning(
        const xAOD::IParticleContainer* parent,
        const std::vector<bool>& mask) const override;

  private:
    // Properties
    /// The thinning service.
    ServiceHandle<IThinningSvc> m_thinningSvc;

    /// The name of the child container to thin.
    std::string m_sgKey;

    /// The size of the dR cone to use.
    float m_coneSize{0.};

    /// Whether to use the 'And' operator.
    bool m_and{false};
}; //> end class DeltaRThinningTool

#endif //> !ThinningUtils_DeltaRThinningTool_H
