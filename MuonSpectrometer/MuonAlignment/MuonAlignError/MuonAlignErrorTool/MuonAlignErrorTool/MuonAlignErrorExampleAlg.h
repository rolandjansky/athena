/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONALIGNERRORTOOL_MUONALIGNERROREXAMPLEALG_H
#define MUONALIGNERRORTOOL_MUONALIGNERROREXAMPLEALG_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrkToolInterfaces/ITrkAlignmentDeviationTool.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "MuonCalibITools/IIdToFixedIdTool.h"

#include "TFile.h"
#include "TH1F.h"

namespace Trk {
  class Track;
}

namespace MuonAlign {
  class MuonAlignErrorExampleAlg : public AthAlgorithm {
    public:
      MuonAlignErrorExampleAlg(const std::string& name, ISvcLocator* pSvcLocator);
      StatusCode initialize();
      StatusCode execute();
      StatusCode finalize();
    private:
      void muonTrack(const Trk::Track* track) const;

      ToolHandle<Trk::ITrkAlignmentDeviationTool> m_alignErrorTool{this,"alignErrorTool","MuonAlign::AlignmentErrorTool"};

      // FOR DEBUGGING
      TFile* m_debug;
      TH1F* m_cham_per_dev;
      TH1F* m_dev_per_track;
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
      ToolHandle<MuonCalib::IIdToFixedIdTool> m_idTool{this,"idTool","MuonCalib::IdToFixedIdTool/IdToFixedIdTool"};
      std::string hardwareName(MuonCalib::MuonFixedId calibId) const;
      std::string side(MuonCalib::MuonFixedId calibId) const;
      std::string sectorString(MuonCalib::MuonFixedId calibId) const;
      int sector(MuonCalib::MuonFixedId calibId) const;
      int hardwareEta(MuonCalib::MuonFixedId calibId) const;
      bool isSmallSector(MuonCalib::MuonFixedId calibId) const;

  };
}

#endif

