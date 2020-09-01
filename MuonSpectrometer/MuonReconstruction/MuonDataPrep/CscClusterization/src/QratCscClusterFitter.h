/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef QratCscClusterFitter_H
#define QratCscClusterFitter_H

// Woochun Park & David Adams
// March 2007
//
// Tool to fit a CSC cluster using adjacent charge ratios.

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

class QratCscClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {

  public:
    QratCscClusterFitter(std::string, std::string, const IInterface*);

    ~QratCscClusterFitter() = default;

    StatusCode initialize();

    // Inherited methods.
    const DataNames& dataNames() const;
    using ICscClusterFitter::fit;
    Results fit(const StripFitList& sfits) const;
    Results fit(const StripFitList& sfits, double dposdz) const;
    double  getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;

  private:
    // Properties
    std::vector<unsigned int> m_max_width;
    std::string               m_posopt_eta;
    std::string               m_posopt_phi;
    std::string               m_erropt_eta;
    std::string               m_erropt_phi;
    double                    m_error_eta;
    double                    m_error_phi;
    double                    m_precisionErrorScaler;
    double                    m_qrat_maxdiff;
    double                    m_qrat_maxsig;
    double                    m_error_tantheta;
    double                    m_xtan_css_eta_offset;
    double                    m_xtan_css_eta_slope;
    double                    m_xtan_csl_eta_offset;
    double                    m_xtan_csl_eta_slope;
    double                    m_qratmin_css_eta;
    double                    m_qratmin_csl_eta;
    std::vector<double>       m_qratcor_css_eta;
    std::vector<double>       m_qratcor_csl_eta;

    double m_atanh_a_css_eta;  // MS: atanh qrat parametrization
    double m_atanh_b_css_eta;
    double m_atanh_c_css_eta;
    double m_atanh_x0_css_eta;
    double m_atanh_a_csl_eta;
    double m_atanh_b_csl_eta;
    double m_atanh_c_csl_eta;
    double m_atanh_x0_csl_eta;

    double m_dposmin;  // MS: minimum position error in mm

    /** retrieve MuonDetectorManager from the conditions store */
    SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_DetectorManagerKey{
        this,
        "DetectorManagerKey",
        "MuonDetectorManager",
        "Key of input MuonDetectorManager condition data",
    };

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    ToolHandle<ICscAlignmentTool> m_alignmentTool;
};
#endif
