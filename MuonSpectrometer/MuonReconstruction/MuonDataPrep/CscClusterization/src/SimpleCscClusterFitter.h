/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// SimpleCscClusterFitter.h

#ifndef SimpleCscClusterFitter_H
#define SimpleCscClusterFitter_H

// Woochun Park and David Adams
// March 2007
//
// Tool to select muons for physics analysis.

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "GaudiKernel/ToolHandle.h"

namespace Muon {
  class CscPrepData;
}
namespace MuonGM {
  class MuonDetectorManager;
}

class ICscAlignmentTool;
class CscIdHelper;
class SimpleCscClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {
  
public:

  // Constructor.
  SimpleCscClusterFitter(std::string, std::string, const IInterface*);
        
  // Destructor.
  ~SimpleCscClusterFitter();
        
  // Initialization.
  StatusCode initialize();

  // Finalization.
  StatusCode finalize();

  // Inherited methods.
  using ICscClusterFitter::fit;
  Results fit(const StripFitList& sfits) const;
  Results fit(const StripFitList& sfits, double dposdz) const;
  double getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;
private:

  // Properties
  std::string m_option;                // Fitting option: MEAN, PEAK, ...
  double m_intrinsic_cluster_width;    // Intrinsic widh for error calculation
  bool m_use_peakthreshold;
  double m_defaultErrorScaler_eta;
  double m_defaultErrorScaler_phi;

  const MuonGM::MuonDetectorManager* m_detMgr;
  const CscIdHelper* m_cscIdHelper;
  ToolHandle<ICscAlignmentTool>                 m_alignmentTool;
};
#endif
