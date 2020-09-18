/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscThresholdClusterBuilderTool.h

#ifndef CscThresholdClusterBuilderTool_H
#define CscThresholdClusterBuilderTool_H

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

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"
#include "MuonPrepRawData/CscStripPrepDataContainer.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "CscClusterization/ICscClusterBuilder.h"
#include "MuonPrepRawData/CscPrepDataContainer.h"

class ICscCalibTool;
class ICscStripFitter;
class ICscClusterFitter;

namespace Muon {
class CscPrepData;
class CscStripPrepData;
}  // namespace Muon
typedef Muon::CscPrepData MyCscDigit;
class CscDigit;


class CscThresholdClusterBuilderTool : virtual public ICscClusterBuilder, public AthAlgTool {

  public:  // methods
    enum NoiseOption { rms = 0, sigma, f001 };


    // Constructor.
    CscThresholdClusterBuilderTool(const std::string& type, const std::string& aname, const IInterface*);

    // Destructor.
    ~CscThresholdClusterBuilderTool();

    /** AlgTool InterfaceID
     */
    //  static const InterfaceID& interfaceID( ) ;


    // Initialization.
    StatusCode initialize();

    // Event processing.
    StatusCode getClusters(std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& selectedIdVect, Muon::CscPrepDataContainer *object);

    // Finalization.
    StatusCode finalize();

  private:  // Private methods.
    // Cluster a cathode plane.
    //  int make_clusters(bool dump, int maxstrip, double pitch,
    //                    const std::vector<MyCscDigit*>& idstrip, const std::vector<double>& qstrip);
    //  int make_clusters(bool dump, int maxstrip, double pitch, const std::vector<Muon::CscStripPrepData*>& strips);
    int        make_clusters(bool measphi, const std::vector<const Muon::CscStripPrepData*>& strips,
                             Muon::CscPrepDataCollection*& collection);
    StatusCode getClusters(IdentifierHash idVect, std::vector<IdentifierHash>& selectedIdVect);
    StatusCode getClusters(IdentifierHash idVect, std::vector<IdentifierHash>& selectedIdVect,
                           Muon::CscPrepDataContainer* pclusters);
    StatusCode getClusters(std::vector<IdentifierHash>& selectedIdVect, Muon::CscPrepDataContainer* pclusters);

  private:  // data
    // Properties.
    double                                             m_threshold;  // Charge threshold to include strip in cluster
    float                                              m_kFactor;
    std::string                                        m_noiseOptionStr;
    NoiseOption                                        m_noiseOption;
    SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_digit_key;  // SG key for input digits

    // Calibration tool.
    ToolHandle<ICscCalibTool> m_cscCalibTool{
        this,
        "cscCalibTool",
        "CscCalibTool/CscCalibTool",
    };

    // Strip fitter.
    ToolHandle<ICscStripFitter> m_pstrip_fitter{
        this,
        "strip_fitter",
        "CalibCscStripFitter/CalibCscStripFitter",
    };

    // Cluster fitters.
    ToolHandle<ICscClusterFitter> m_pfitter_def{
        this,
        "default_fitter",
        "SimpleCscClusterFitter/SimpleCscClusterFitter",
    };
    ToolHandle<ICscClusterFitter> m_pfitter_prec{
        this,
        "precision_fitter",
        "QratCscClusterFitter/QratCscClusterFitter",
    };
    ToolHandle<ICscClusterFitter> m_pfitter_split{
        this,
        "split_fitter",
        "CscSplitClusterFitter/CscSplitClusterFitter",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    /** retrieve MuonDetectorManager from the conditions store */
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this,
        "DetectorManagerKey",
        "MuonDetectorManager",
        "Key of input MuonDetectorManager condition data",
    };

    bool m_makeNarrowClusterThreeStrips;
};

#endif
