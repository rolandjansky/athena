/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CscSplitClusterFitter_H
#define CscSplitClusterFitter_H

// Woochun Park & David Adams
// March 2007
//
// Tool to fit a CSC cluster using adjacent charge ratios.

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscClusterFitter.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonPrepRawData/CscClusterStatus.h"

namespace Muon {
class CscPrepData;
}

class CscSplitClusterFitter : virtual public ICscClusterFitter, public AthAlgTool {

  public:
    CscSplitClusterFitter(std::string, std::string, const IInterface*);

    ~CscSplitClusterFitter() = default;

    StatusCode initialize();

    // Inherited methods.
    Results fit(const StripFitList& sfits) const;
    Results fit(const StripFitList& sfits, double dposdz) const;
    double  getCorrectedError(const Muon::CscPrepData* pclu, double slope) const;

  private:
    // Properties
    // Minimum distance between peaks and valley
    int m_min_dist;
    // Maximum charge ratio between peak strip and valley strip
    float m_max_qratio;

    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc{
        this,
        "MuonIdHelperSvc",
        "Muon::MuonIdHelperSvc/MuonIdHelperSvc",
    };

    // Cluster fitters.
    ToolHandle<ICscClusterFitter> m_pfitter_def;
    ToolHandle<ICscClusterFitter> m_pfitter_prec;
};

#endif
