/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TGCHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/TgcReadoutElement.h"

#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHit.h"

#include "MuonSimEvent/TgcHitIdHelper.h"
#include "MuonIdHelpers/TgcIdHelper.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <TH2D.h>

Identifier TGCHitsTestTool::getIdentifier(HitID tgchit) {
  TgcHitIdHelper* tgchelper = TgcHitIdHelper::GetHelper();
  std::string tgc_stname = tgchelper->GetStationName(tgchit);
  int         tgc_steta  = tgchelper->GetStationEta(tgchit);
  int         tgc_stphi  = tgchelper->GetStationPhi(tgchit);
  int         tgc_gg     = tgchelper->GetGasGap(tgchit);
  int         tgc_strip  = 1;//tgchelper->GetIsStrip(tgchit);
  int         tgc_channel = 1;//tgchelper->GetChannel(tgchit);

  /**Create the offline identifiers, fill them with hit info.Ready to be accessed and retrieve info.
     Currently not used in this code except for some checks.
  */

  Identifier offid = m_pTgcIdHelper->channelID(tgc_stname, tgc_steta, tgc_stphi, tgc_gg, tgc_strip, tgc_channel);
  if (offid == 0) {
    ATH_MSG_FATAL("TGC: Cannot build a valid Identifier; skip ");
  }
  ATH_MSG_VERBOSE(" TGC: Offline Id "<<m_pTgcIdHelper->show_to_string(offid));

  return offid;
}

StatusCode TGCHitsTestTool::checkIdentifier(Identifier offid) {
  const MuonGM::TgcReadoutElement* descriptor = m_pMuonMgr->getTgcReadoutElement(offid);
  if (!descriptor) {
    ATH_MSG_FATAL("TGC readout element not found for Id = " << m_pTgcIdHelper->show_to_string(offid));
    return StatusCode::FAILURE;
  }

  // -------- Testing Sim to Offline ID conversion ---------
  Amg::Vector3D tgc_gPos = descriptor->channelPos(offid);
  double tgc_strip_phi = tgc_gPos.phi();
  if (tgc_strip_phi < 0.) tgc_strip_phi += 2.*M_PI;
  /*
    double tgc_strip_perp  = tgc_gPos.perp();
    double tgc_strip_z   = tgc_gPos.z();
    double tgc_strip_cot   = 1./tan(tgc_gPos.theta());
  */

  return StatusCode::SUCCESS;
}


StatusCode TGCHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  //MuonSpectrometer
  /**Get the generic MuonSpectrometer (MDT,RPC,CSC,TGC) histograms
     Short description:
     MuonSpectrometer cross section
     MuonSpectrometer longitudinal
     Number of events processed from truth
     Event specific run number
     Eta, Theta, Phi, z residual (...), phi residual (...) of hits in MuonSpectrometer
  */

  // TGC
  // Get the TGC histograms
  // Short description:
  //   MuonSpectrometer TGC cross section
  //   MuonSpectrometer TGC longitudinal
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in TGCs

  // Enter the main algorithm loop
  if (m_DoTGCTest) {

    const DataHandle<TGCSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection, "TGC_Hits"));
    for (TGCSimHitCollection::const_iterator i_hit=p_collection->begin(); i_hit!=p_collection->end(); ++i_hit) {
      /** Check the Hits identifiers, access the functions that give:
          Station name, station eta, station phi, doublet Z, doublet phi, doublet R, GasGap, Measures Phi.
          The values of these variables are written out to the AANtuple (variable content and range explained in the code section where AANTuple variables are
          registered)
      */
      HitID tgchit= (*i_hit).TGCid();
      Identifier offid= getIdentifier(tgchit);
      CHECK(checkIdentifier(offid));

       //Check Hits
      /**For every hit within the event, get the global position Amg::Vector3D u and then retrieve all releveant info
         either from the Amg::Vector3D or from the MC vector (direction)
      */
      GeoTGCHit ghit(*i_hit);
      if (!ghit) continue;
      Amg::Vector3D u = ghit.getGlobalPosition();
      CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TGCHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  m_pTgcIdHelper = m_pMuonMgr->tgcIdHelper();
  return StatusCode::SUCCESS;
}
