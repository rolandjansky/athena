/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// FinalizeMuonContainers.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef REC_FINALIZEMUONCONTAINERS_H
#define REC_FINALIZEMUONCONTAINERS_H

#include "AthenaBaseComps/AthAlgorithm.h"
// #include "GaudiKernel/ToolHandle.h"
#include <vector>
#include <string>


namespace Rec {

  class IMuonPrintingTool;
  class MuonTypeComparisonFunction;

  /** @class FinalizeMuonContainers
      @brief Locking of the Muon containers. Done in a separate algorithm so that
             overlap removal, muon dressing and cross-collection linking can be split
             across dedicated algorithms.

      @author Wolfgang Liebig
  */

  class FinalizeMuonContainers : public AthAlgorithm {
  public:
    FinalizeMuonContainers(const std::string &name, ISvcLocator* pSvcLocator);
    virtual StatusCode initialize();
    virtual StatusCode execute();
    virtual StatusCode finalize();

  private:
    FinalizeMuonContainers();
    FinalizeMuonContainers(const FinalizeMuonContainers&);
    FinalizeMuonContainers &operator=(const FinalizeMuonContainers&);

    /** vector of collections, each still unlocked */
    std::vector<std::string> m_muonContainerNames;
  };
}
#endif
