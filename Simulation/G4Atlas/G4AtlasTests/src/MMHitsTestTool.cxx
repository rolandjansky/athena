/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MMHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonSimEvent/MicromegasHitIdHelper.h"

#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/GenericMuonSimHit.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/INTupleSvc.h"

#include "TH2D.h"
#include "TTree.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TF1.h"
#include "TH1F.h"

using namespace MuonGM;

 
 
StatusCode MMHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  if (m_DoMMTest) {
    const DataHandle<GenericMuonSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"MicromegasSensitiveDetector"));
    for (GenericMuonSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) {
      Amg::Vector3D u = (*i_hit).globalPosition();
      CHECK(executeFillHistos(u));
      // GeoMMHit ghit(*i_hit);
//       if (!ghit) continue;
//       Amg::Vector3D u = ghit.getGlobalPosition();
//       CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode MMHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  return StatusCode::SUCCESS;
}
