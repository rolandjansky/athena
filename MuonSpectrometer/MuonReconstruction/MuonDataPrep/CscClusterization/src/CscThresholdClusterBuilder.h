/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscThresholdClusterBuilder.h

#ifndef CscThresholdClusterBuilder_H
#define CscThresholdClusterBuilder_H

// David Adams
// June 2006
//
// ATLAS algorithm to build CSC clusters from CSC digits.
// A simple threshold cluster algorithm is applied: any adjacent
// channels with charge above a user-specified threshold are
// merged into a cluster.
//
// Clusters are tagged as spoiled if any of the following apply:
//   1. Too narrow: only one strip.
//   2. Too wide: pitch*(nstrip-1) above a user-specified cutoff.
//   3. Multiple peaks.
//   4. Skewed: peak not in in the central channel for an odd cluster
//      (odd # strips) or central two for an even cluster.
//   5. Edge: cluster includes the first or last strip in the plane.
//   6. RMS: charge centroid RMS is too large
// The different algorithms used to evaluate the unspoiled position
// may transfer clusters into the spoiled state. E.g. the adjacent
// charge ratios method requires left and right to be consistent.
//
// Spoiled clusters are assigned position at the center of the cluster
// and a large error: width/sqrt(12) with possible correction for the
// intrinsic signal width.
//
// The positions for unspoiled clusters may evaluated in a number of
// ways selected through job options.
//
// The corrected charge centroid method starts with the charge
// centroid and then applies a strip position correction
// which is a function of the plane type (CSS or CSL, r or phi), the
// number of strips in the cluster, and the deviation of the charge
// centroid from the center of the cluster. Thes corrections are 4th
// order odd polynomials in that deviation, i.e.
//   A d + B d**3 + C D**5 + D d**7
// and the coefficients were determined by comparison with Monte Carlo
// track positions. For now these coefficients are hardwired into the
// code.
//
// The adjacent charge ratios method uses the ratios of the left and
// right charges to that of the central strip to estimate the position
// again using a polynomial calibration.
//
// Unspoiled clusters are assigned a position error of
// m_unspoiled_error.

// Algorithm to construct CSC clusters from digits.

#include "AthenaBaseComps/AthAlgorithm.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "CscClusterization/ICscStripFitter.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace MuonGM {
class MuonDetectorManager;
}
class CscIdHelper;
namespace Muon {
class CscPrepData;
class CscStripPrepData;
}  // namespace Muon
typedef Muon::CscPrepData MyCscDigit;
class CscDigit;

class CscThresholdClusterBuilder : public AthAlgorithm {

  public:  // methods
    // Constructor.
    CscThresholdClusterBuilder(const std::string& name, ISvcLocator* pSvcLocator);

    // Destructor.
    ~CscThresholdClusterBuilder();

    // Initialization.
    StatusCode initialize();

    // Event processing.
    StatusCode execute();


  private:  // data
    // Strip fitter.
    ToolHandle<ICscClusterBuilder> m_cluster_builder{
        this,
        "cluster_builder",
        "CscThresholdClusterBuilderTool/CscThresholdClusterBuilderTool",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    SG::WriteHandleKey<Muon::CscPrepDataContainer> m_pclusters{
        this, 
	"cluster_key", 
	"CSC_Clusters", 
	"Ouput CSC Cluster container"};

};

#endif
