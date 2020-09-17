/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// CscClusterUtilTool.h

#ifndef CscClusterUtilTool_H
#define CscClusterUtilTool_H

// Woochun Park
// April 2009
//
// Strip fitter using the parabolic fit fron the CSC calibration tool.

#include <vector>

#include "AthenaBaseComps/AthAlgTool.h"
#include "CscClusterization/ICscClusterUtilTool.h"
#include "GaudiKernel/ToolHandle.h"
class CscIdHelper;
namespace Muon {
class CscClusterOnTrack;
class CscPrepData;
class CscStripPrepData;
}
namespace Trk {
class RIO_OnTrack;
}

class CscClusterUtilTool : virtual public ICscClusterUtilTool, public AthAlgTool {

  public:  // Ctors and dtor.
    // Constructor.
    CscClusterUtilTool(const std::string&, const std::string&, const IInterface*);

    // Destructor.
    ~CscClusterUtilTool()=default;

    StatusCode initialize();

    ICscClusterFitter::Results getRefitCluster(const Muon::CscPrepData* MClus, double tantheta) const;

    void getStripFits(const Trk::RIO_OnTrack* rot, ICscClusterFitter::StripFitList& sfits) const;
    void getStripFits(const Muon::CscClusterOnTrack* pclu, ICscClusterFitter::StripFitList& sfits) const;
    void getStripFits(const Muon::CscPrepData*, ICscClusterFitter::StripFitList&) const;

    std::vector<const Muon::CscStripPrepData*> getStrips(const Muon::CscPrepData* MClus) const;

  private:
    const CscIdHelper* m_phelper;

    // Strip fitter.
    ToolHandle<ICscStripFitter> m_stripFitter{
        this,
        "strip_fitter",
        "CalibCscStripFitter/CalibCscStripFitter",
    };
    ToolHandle<ICscClusterFitter> m_precClusterFitter{
        this,
        "precision_fitter",
        "QratCscClusterFitter/QratCscClusterFitter",
    };

    SG::ReadHandleKey<Muon::CscStripPrepDataContainer> m_cscStripLocation;
};

#endif
