/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCOMBINEDBASETOOLS_MUONDRESSINGTOOL_H
#define MUONCOMBINEDBASETOOLS_MUONDRESSINGTOOL_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODMuonCnv/IMuonDressingTool.h"
#include "MuonRecToolInterfaces/IMuonHitSummaryTool.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"

namespace Trk {
  class TrackSummary;
}

namespace MuonCombined {

  class MuonDressingTool: public AthAlgTool, virtual public xAOD::IMuonDressingTool {
  public:
    MuonDressingTool(const std::string& type, const std::string& name, const IInterface* parent);
    ~MuonDressingTool(void); // destructor
  
    StatusCode initialize();
    StatusCode finalize();

    void addMuonHitSummary( xAOD::Muon& muon, const Trk::TrackSummary* summary = 0 ) const ;

  private:
    ToolHandle<Muon::IMuonHitSummaryTool>       m_hitSummaryTool;
    ToolHandle<Muon::MuonIdHelperTool>          m_idHelper;
  };
  
}	// end of namespace

#endif


