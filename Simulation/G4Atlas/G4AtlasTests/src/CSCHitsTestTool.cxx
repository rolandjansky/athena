/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CSCHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "GeoAdaptors/GeoMuonHits.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/CscReadoutElement.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonIdHelpers/CscIdHelper.h"
#include "MuonSimEvent/CscHitIdHelper.h"

#include <TH1D.h>
#include <TH2D.h>


Identifier CSCHitsTestTool::getIdentifier(HitID cschit) {
  CscHitIdHelper* cschelper = CscHitIdHelper::GetHelper();
  
  const std::string & csc_stname = cschelper->GetStationName(cschit);
  int csc_steta  = cschelper->GetZSector(cschit);
  int csc_stphi  = cschelper->GetPhiSector(cschit);
  int csc_cl     = cschelper->GetChamberLayer(cschit);
  int csc_wl     = cschelper->GetWireLayer(cschit);
  int csc_measphi = 1;
  int csc_strip =  1;

  // Create the offline identifiers, fill them with hit info. Ready to be
  // accessed and retrieve info.  Currently not used in this code except for
  // some checks.
  //  ======================================================================
  Identifier offid = m_pCscIdHelper->channelID(csc_stname, csc_steta, 
					       csc_stphi, csc_cl, csc_wl, 
					       csc_measphi, csc_strip);
  if (offid == 0) {
    ATH_MSG_FATAL("CSC: Cannot build a valid Identifier; skip ");
  }
  // *AS* really info level? perhaps debug or verbose?
  ATH_MSG_VERBOSE(" CSC: Offline Id "<<m_pCscIdHelper->show_to_string(offid));
  return offid;
}

StatusCode CSCHitsTestTool::checkIdentifier(Identifier offid) {
  const MuonGM::CscReadoutElement* descriptor = m_pMuonMgr->getCscReadoutElement(offid);
  if (!descriptor) {
    ATH_MSG_FATAL("CSC readout element not found for Id = " << m_pCscIdHelper->show_to_string(offid));
    return StatusCode::FAILURE;
  }

  // Testing Sim to Offline ID conversion
  Amg::Vector3D csc_gPos   = descriptor->stripPos(offid);
  double csc_strip_phi   = csc_gPos.phi();
  if (csc_strip_phi < 0.) csc_strip_phi += 2.*M_PI;
  double csc_strip_perp __attribute ((unused))  = csc_gPos.perp();
  double csc_strip_z  __attribute ((unused))  = csc_gPos.z();
  double csc_strip_cot  __attribute ((unused))  = 1./tan(csc_gPos.theta());

  return StatusCode::SUCCESS;
}

StatusCode CSCHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  // *AS* which direction is selected in the end?
  // should match with truth particle using barcode (particleID)

  // MuonSpectrometer
  // Get the generic MuonSpectrometer (MDT,RPC,CSC,TGC) histograms
  // Short description:
  //   MuonSpectrometer cross section
  //   MuonSpectrometer longitudinal
  //   Number of events processed from truth
  //   Event specific run number
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in MuonSpectrometer

  // CSC
  // Get the CSC histograms
  // Short description:
  //   MuonSpectrometer RPC cross section
  //   MuonSpectrometer RPC longitudinal
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in CSCs

  if (m_DoCSCTest) {

    // -- loop over CSC hit collection --
    const DataHandle<CSCSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"CSC_Hits"));
    for (CSCSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) {

      // Check the Hits identifiers, access the functions that give:
      // Station name, station eta, station phi, chamber layer, wire layer.
      HitID cschit= (*i_hit).CSCid();
      Identifier offid= getIdentifier(cschit);
      CHECK(checkIdentifier(offid));

      // Check Hits. For every hit within the event, get the global position
      // Amg::Vector3D u and then retrieve all releveant info either from the
      // Amg::Vector3D or from the MC vector (direction)
      GeoCSCHit ghit(*i_hit);
      if (!ghit) continue;
      Amg::Vector3D u = ghit.getGlobalPosition();
      CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode CSCHitsTestTool::initialize() {
  // initialise general histograms
  CHECK(MuonHitTestToolBase::initialize());

  m_pCscIdHelper = m_pMuonMgr->cscIdHelper();

  return StatusCode::SUCCESS;
}
