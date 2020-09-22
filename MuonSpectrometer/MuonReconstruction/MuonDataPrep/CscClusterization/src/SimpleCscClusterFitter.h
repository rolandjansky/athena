/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SimpleCscClusterFitter_H
#define SimpleCscClusterFitter_H

// Woochun Park and David Adams
// March 2007
//
// Tool to select muons for physics analysis.

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscAlignmentTool.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscClusterStatus.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"

namespace Muon {
class CscPrepData;
}

class SimpleCscClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {

  public:
    SimpleCscClusterFitter(std::string, std::string, const IInterface*);

    ~SimpleCscClusterFitter() = default;

    StatusCode initialize();

    // Inherited methods.
    using ICscClusterFitter::fit;
    Results fit(const StripFitList& sfits) const;
    Results fit(const StripFitList& sfits, double dposdz) const;
    double  getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;

  private:
    // Properties
    std::string m_option;                   // Fitting option: MEAN, PEAK, ...
    double      m_intrinsic_cluster_width;  // Intrinsic widh for error calculation
    bool        m_use_peakthreshold;
    double      m_defaultErrorScaler_eta;
    double      m_defaultErrorScaler_phi;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{this, "MuonIdHelperSvc",
                                                        "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

    /** retrieve MuonDetectorManager from the conditions store */
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this, "DetectorManagerKey", "MuonDetectorManager", "Key of input MuonDetectorManager condition data"};

    ToolHandle<ICscAlignmentTool> m_alignmentTool{
        this,
        "CscAlignmentTool",
        "CscAlignmentTool/CscAlignmentTool",
    };
};
#endif
