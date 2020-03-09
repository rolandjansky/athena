/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CscSplitClusterFitter.h

#ifndef CscSplitClusterFitter_H
#define CscSplitClusterFitter_H

// Woochun Park & David Adams
// March 2007
//
// Tool to fit a CSC cluster using adjacent charge ratios.

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

namespace Muon {
  class CscPrepData;
}

namespace MuonGM {
  class MuonDetectorManager;
}

class CscSplitClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {
  
public:

  // Constructor.
  CscSplitClusterFitter(std::string, std::string, const IInterface*);
        
  // Destructor.
  ~CscSplitClusterFitter();
        
  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

  // Inherited methods.
  //  using ICscClusterFitter::fit;
  Results fit(const StripFitList& sfits) const;
  Results fit(const StripFitList& sfits, double dposdz) const;
  double getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;
  
private:

  // Properties
  // Minimum distance between peaks and valley               
  int m_min_dist;
  // Maximum charge ratio between peak strip and valley strip
  float m_max_qratio;

  ToolHandle<Muon::MuonIdHelperTool> m_muonIdHelperTool{this, "idHelper", 
    "Muon::MuonIdHelperTool/MuonIdHelperTool", "Handle to the MuonIdHelperTool"};
  // Cluster fitters.
  ToolHandle<ICscClusterFitter> m_pfitter_def;
  ToolHandle<ICscClusterFitter> m_pfitter_prec;
  
};

#endif
