/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
#ifndef DerivationFrameworkInterfaces_IForwardParentThinningTool_H
#define DerivationFrameworkInterfaces_IForwardParentThinningTool_H

// athena includes
#include "GaudiKernel/IAlgTool.h"

// EDM includes
#include "xAODBase/IParticleContainer.h"

/**
 * @class IForwardParentThinningTool
 * @brief Interface class for tools that thin low-level objects based on a
 *        selection on associated high-level objects.
 * 
 * @author Jon Burr
 */
class IForwardParentThinningTool : virtual public IAlgTool {
  public:
    /// AlgTool interface ID
    static const InterfaceID& interfaceID() {
      static const InterfaceID thisID("IForwardParentThinningTool", 1, 0);
      return thisID;
    }

    /// Virtual destructor
    virtual ~IForwardParentThinningTool() {}

    /// Forward the thinning from the parent particles
    virtual StatusCode forwardParentThinning(
        const xAOD::IParticleContainer* parent,
        const std::vector<bool>& mask) const = 0;
};

#endif //> !DerivationFrameworkInterfaces_IForwardParentThinningTool_H
