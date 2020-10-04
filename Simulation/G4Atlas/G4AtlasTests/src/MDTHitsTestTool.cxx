/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/Identifier.h"

#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonSimEvent/MdtHitIdHelper.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MDTSimHit.h"

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

Identifier MDTHitsTestTool::getIdentifier(HitID mdthit) 
{
  MdtHitIdHelper* mdthelper = MdtHitIdHelper::GetHelper(m_pMdtIdHelper->tubeMax());
  std::string mdt_stname = mdthelper->GetStationName(mdthit);
  int         mdt_steta  = mdthelper->GetZSector(mdthit);
  int         mdt_stphi  = mdthelper->GetPhiSector(mdthit);
  int         mdt_ml     = mdthelper->GetMultiLayer(mdthit);
  int         mdt_tl     = mdthelper->GetLayer(mdthit);
  int         mdt_tube   = mdthelper->GetTube(mdthit);
  
  // Create the offline identifiers, fill them with hit info. Ready to be
  // accessed and retrieve info.  Currently not used in this code except
  // for some checks.
  Identifier offid = m_pMdtIdHelper->channelID(mdt_stname, mdt_steta, mdt_stphi,mdt_ml,mdt_tl,mdt_tube);
  if (offid == 0){
    ATH_MSG_FATAL("MDT: Cannot build a valid Identifier; skip ");
  }

  ATH_MSG_VERBOSE(" MDT: Offline Id "<<m_pMdtIdHelper->show_to_string(offid));
  return offid;
}

StatusCode MDTHitsTestTool::checkIdentifier(Identifier offid) {
  const MuonGM::MdtReadoutElement* descriptor = m_pMuonMgr->getMdtReadoutElement(offid);
  if (descriptor == NULL) {
    ATH_MSG_FATAL("MDT readout element not found for Id = " << m_pMdtIdHelper->show_to_string(offid));
    return StatusCode::FAILURE;
   }

  // Testing Sim to Offline ID conversion
  Amg::Vector3D mdt_gPos   = descriptor->tubePos(offid);
  // double mdt_tube_pos = sqrt(mdt_gPos.perp()*mdt_gPos.perp()+mdt_gPos.z()*mdt_gPos.z());
  double mdt_tube_phi   = mdt_gPos.phi();
  if (mdt_tube_phi < 0.) mdt_tube_phi += 2.*M_PI;
  // double mdt_tube_perp  = mdt_gPos.perp();
  // double mdt_tube_z   = mdt_gPos.z();
  // double mdt_tube_cot   = 1./tan(mdt_gPos.theta());
  //Amg::Transform3D xf  = descriptor->getMaterialGeom()->getAbsoluteTransform(); //FIXME in the future??
  //*AS* Amg::Vector3D xU = xf*Amg::Vector3D((*i_hit).localPosition().x(),-(*i_hit).localPosition().z(), (*i_hit).localPosition().y());
  //*AS*Amg::Vector3D gx = mdt_gPos + xU;

  return StatusCode::SUCCESS;
}

StatusCode MDTHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  // MuonSpectrometer
  // Get the generic MuonSpectrometer (MDT,RPC,CSC,TGC) histograms
  // Short description:
  //   MuonSpectrometer cross section
  //   MuonSpectrometer longitudinal
  //   Number of events processed from truth
  //   Event specific run number
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in MuonSpectrometer

  // MDT
  // Get the MDT histograms
  // Short description:
  //   MuonSpectrometer MDT cross section (affected by m_BarrelEtaCut)
  //   MuonSpectrometer MDT longitudinal
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in MDTs


  if (m_DoMDTTest) {
    const DataHandle<MDTSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"MDT_Hits"));
    for (MDTSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) {
      // Check the Hits identifiers, access the functions that give:
      // Station name, station eta, station phi, multilayer ID, layer ID, tube ID.
      HitID mdthit= (*i_hit).MDTid();
      Identifier offid= getIdentifier(mdthit);
      CHECK(checkIdentifier(offid));



      // Check Hits
      // For every hit within the event, get the global position Amg::Vector3D u and then retrieve all releveant info
      // either from the Amg::Vector3D or from the MC vector (direction)
      GeoMDTHit ghit(*i_hit);
      if (!ghit) continue;
      Amg::Vector3D u = ghit.getGlobalPosition();
      CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode MDTHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  //  CHECK(detStore()->retrieve(m_pMuonMgr));
  m_pMdtIdHelper = m_pMuonMgr->mdtIdHelper();
  return StatusCode::SUCCESS;
}
