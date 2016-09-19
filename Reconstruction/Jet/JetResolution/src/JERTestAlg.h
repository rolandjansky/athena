/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETRESOLUTION_JERTESTALG_H
#define JETRESOLUTION_JERTESTALG_H

// Framework includes
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

// Local includes
#include "JetResolution/IJERSmearingTool.h"

/// A testing algorithm for the dual-use JER tools in Athena
///
/// @author Steve Farrell <steven.farrell@cern.ch>
///
class JERTestAlg : public AthAlgorithm
{

  public:

    /// Standard algorithm constructor
    JERTestAlg(const std::string& name, ISvcLocator* svcLoc);

    /// Initialize the algorithm
    virtual StatusCode initialize();
    /// Execute the algorithm
    virtual StatusCode execute();

  private:

    /// Handles to the tools
    ToolHandle<IJERSmearingTool> m_jerSmearingTool;

    // Systematics to test
    bool m_applySystematics;
    std::vector<CP::SystematicSet> m_sysList;

};

#endif
