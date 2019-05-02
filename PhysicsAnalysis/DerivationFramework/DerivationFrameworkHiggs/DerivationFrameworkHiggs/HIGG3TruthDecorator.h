/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// Tool for decorating events with truth related quantities, in which the full truth record is assumed to still be available
// Ported originally from code in the HWWPhysicsxAODMaker by D.Shope (david.richard.shope@cern.ch):
// https://gitlab.cern.ch/atlas-physics/higgs/hww/HWWPhysicsxAODMaker

#ifndef DerivationFrameworkHiggs_HIGG3TruthDecorator_H
#define DerivationFrameworkHiggs_HIGG3TruthDecorator_H

#include <string>
#include <vector>
#include <algorithm>

// Gaudi & Athena basics
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

// DerivationFramework includes
#include "DerivationFrameworkInterfaces/IAugmentationTool.h"

namespace DerivationFramework {

  class HIGG3TruthDecorator : public AthAlgTool, public IAugmentationTool {

  public:
    /** Constructor with parameters */
    HIGG3TruthDecorator(const std::string& t, const std::string& n, const IInterface* p);

    /** Destructor */
    ~HIGG3TruthDecorator();

    // Athena algtool's Hooks
    StatusCode initialize();
    StatusCode finalize();

    virtual StatusCode addBranches() const;

  private:
    std::string m_inElContName;

  };

}

#endif