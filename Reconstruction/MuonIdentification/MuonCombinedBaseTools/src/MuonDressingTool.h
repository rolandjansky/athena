/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONDRESSINGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONDRESSINGTOOL_H

#include "xAODMuonCnv/IMuonDressingTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

namespace Trk {
  class TrackSummary;
}

namespace MuonCombined {

  class MuonDressingTool: public AthAlgTool, virtual public xAOD::IMuonDressingTool {
  public:
    MuonDressingTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonDressingTool()=default;

    StatusCode initialize();

    void addMuonHitSummary( xAOD::Muon& muon, const Trk::TrackSummary* summary = 0 ) const ;

  private:
    ToolHandle<Muon::IMuonHitSummaryTool> m_hitSummaryTool {this, "MuonHitSummaryTool", "Muon::MuonHitSummaryTool/MuonHitSummaryTool"};
    ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  };
  
}	// end of namespace

#endif


