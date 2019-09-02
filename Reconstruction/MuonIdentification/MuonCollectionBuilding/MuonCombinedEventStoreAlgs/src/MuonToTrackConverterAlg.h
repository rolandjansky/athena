/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTOTRACKCONVERTERALG_H
#define MUONTOTRACKCONVERTERALG_H

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

#include "TrkTrack/TrackCollection.h"
#include "MuonRecHelperTools/IMuonEDMHelperSvc.h"

namespace Analysis {
  class Muon;
}

namespace Trk {
  class IPropagator;
  class Track;
}

namespace Muon {
  class MuonEDMPrinterTool;
}

namespace Muon {
   class IMuonHoleRecoveryTool;
}


class MuonToTrackConverterAlg : public AthAlgorithm {
 public:
  MuonToTrackConverterAlg(const std::string& name, ISvcLocator* pSvcLocator);
  ~MuonToTrackConverterAlg();
  
  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();

 private:

  void retrieveMuons( const std::vector<std::string>& keys, std::vector<const Analysis::Muon*>& muons ) const;
  
  TrackCollection* convertToTrack( const std::vector<const Analysis::Muon*>& muons ) const;

  Trk::Track* createTaggedMuonTrack( const Analysis::Muon& muon ) const;

  /** Muon containers */
  std::vector<std::string> m_muonContainerNames;
  std::string              m_muonTrackLocation;
  bool                     m_useMuonHitsOnly;

  ServiceHandle<StoreGateSvc>			m_storeGate;
  ToolHandle<Muon::MuonEDMPrinterTool> m_edmPrinter;
  ServiceHandle<Muon::IMuonEDMHelperSvc> m_edmHelperSvc {this, "edmHelper", 
    "Muon::MuonEDMHelperSvc/MuonEDMHelperSvc", 
    "Handle to the service providing the IMuonEDMHelperSvc interface" };
  ToolHandle<Trk::IPropagator>         m_propagator;
  ToolHandle<Muon::IMuonHoleRecoveryTool>   m_muonHoleRecoverTool ;    //<! tool to recover holes on track 
};

#endif // PDA_MUONBUILDER_H
