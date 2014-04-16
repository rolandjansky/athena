/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTOTRACKCONVERTERALG_H
#define MUONTOTRACKCONVERTERALG_H

#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include <vector>
#include <string>

#include "TrkTrack/TrackCollection.h"


namespace Analysis {
  class Muon;
}

namespace Trk {
  class IPropagator;
  class Track;
}

namespace Muon {
  class MuonEDMPrinterTool;
  class MuonEDMHelperTool;
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
  ToolHandle<Muon::MuonEDMHelperTool>  m_edmHelper;
  ToolHandle<Trk::IPropagator>         m_propagator;
  ToolHandle<Muon::IMuonHoleRecoveryTool>   m_muonHoleRecoverTool ;    //<! tool to recover holes on track 
};

#endif // PDA_MUONBUILDER_H
