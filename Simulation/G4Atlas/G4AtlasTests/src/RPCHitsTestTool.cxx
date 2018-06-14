/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCHitsTestTool.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "Identifier/Identifier.h"
#include "GeoAdaptors/GeoMuonHits.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"

#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/RPCSimHit.h"

#include "MuonSimEvent/RpcHitIdHelper.h"
#include "MuonIdHelpers/RpcIdHelper.h"

#include "GeneratorObjects/McEventCollection.h"
#include "CLHEP/Vector/LorentzVector.h"

#include <TH2D.h>
#include "TTree.h"

Identifier RPCHitsTestTool::getIdentifier(HitID rpchit) {
  RpcHitIdHelper* rpchelper = RpcHitIdHelper::GetHelper();
  std::string rpc_stname = rpchelper->GetStationName(rpchit);
  int         rpc_steta  = rpchelper->GetZSector(rpchit);
  int         rpc_stphi  = rpchelper->GetPhiSector(rpchit);
  int         rpc_dbr    = rpchelper->GetDoubletR(rpchit);
  int         rpc_dbz    = rpchelper->GetDoubletZ(rpchit);
  int         rpc_dbp    = rpchelper->GetDoubletPhi(rpchit);
  int         rpc_gg     = rpchelper->GetGasGapLayer(rpchit);
  int         rpc_mfi    = rpchelper->GetMeasuresPhi(rpchit);
  int         rpc_strip  = 1;//rpchelper->GetStrip(rpchit);

  // Create the offline identifiers, fill them with hit info. Ready to be
  // accessed and retrieve info. Currently not used in this code except for
  // some checks.
  Identifier offid = m_pRpcIdHelper->channelID(rpc_stname, rpc_steta, rpc_stphi, rpc_dbr, rpc_dbz, rpc_dbp, rpc_gg, rpc_mfi, rpc_strip);
  if (offid == 0) {
    ATH_MSG_FATAL("RPC : Cannot build a valid Identifier; skip ");
  }

  return offid;
}

StatusCode RPCHitsTestTool::checkIdentifier(Identifier offid) {

  const MuonGM::RpcReadoutElement* descriptor = m_pMuonMgr->getRpcReadoutElement(offid);
  if (!descriptor) {
    ATH_MSG_FATAL("RPC readout element not found for Id = " << m_pRpcIdHelper->show_to_string(offid));
    return StatusCode::FAILURE;
  }

  // Testing Sim to Offline ID conversion
  Amg::Vector3D rpc_gPos   = descriptor->stripPos(offid);
  double rpc_strip_phi   = rpc_gPos.phi();
  if (rpc_strip_phi < 0.) rpc_strip_phi += 2.*M_PI;

  return StatusCode::SUCCESS;
}


StatusCode RPCHitsTestTool::processEvent() {
  CHECK(executeCheckEventInfo());

  // MuonSpectrometer
  // Get the generic MuonSpectrometer (MDT,RPC,CSC,TGC) histograms
  // Short description:
  //   MuonSpectrometer cross section
  //   MuonSpectrometer longitudinal
  //   Number of events processed from truth
  //   Event specific run number
  //   Eta, Theta, Phi, z residual (...), phi residual (...) of hits in MuonSpectrometer

  if (m_DoRPCTest) {

    const DataHandle<RPCSimHitCollection> p_collection;
    CHECK(evtStore()->retrieve(p_collection,"RPC_Hits"));
    for (RPCSimHitCollection::const_iterator i_hit = p_collection->begin(); i_hit != p_collection->end(); ++i_hit) {
      // Check the Hits identifiers, access the functions that give:
      // Station name, station eta, station phi, doublet Z, doublet phi, doublet R, GasGap, Measures Phi.
      HitID rpchit= (*i_hit).RPCid();
      Identifier offid= getIdentifier(rpchit);
      CHECK(checkIdentifier(offid));
     
      // Check Hits
      // For every hit within the event, get the global position Amg::Vector3D u and then retrieve all releveant info
      // either from the Amg::Vector3D or from the MC vector (direction)
      GeoRPCHit ghit(*i_hit);

      if (!ghit) continue;
      Amg::Vector3D u = ghit.getGlobalPosition();
      CHECK(executeFillHistos(u));
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode RPCHitsTestTool::initialize() {
  CHECK(MuonHitTestToolBase::initialize());
  m_pRpcIdHelper = m_pMuonMgr->rpcIdHelper();
  return StatusCode::SUCCESS;
}
