/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "RPCSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/RpcHitIdHelper.h"

#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "AtlasHepMC/GenParticle.h"

#include "TTree.h"

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode RPCSimHitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

  ATH_MSG_DEBUG("do fillRPCSimHitVariables()");

  CHECK( this->clearVariables() );

  const RPCSimHitCollection *rpcContainer = nullptr;
  CHECK( m_evtStore->retrieve(rpcContainer, m_ContainerName.c_str() ) );
  
  // Get the RPC Id hit helper
  RpcHitIdHelper* rpchhelper = RpcHitIdHelper::GetHelper();

  if(!rpcContainer->size()) ATH_MSG_DEBUG(m_ContainerName<<" container empty");
  for( auto it : *rpcContainer ) {
    const RPCSimHit hit = it;

    HitID hitid = hit.RPCid();

    std::string stname = rpchhelper->GetStationName(hitid);
    int         steta  = rpchhelper->GetZSector(hitid);
    int         stphi  = rpchhelper->GetPhiSector(hitid);
    int         dbr    = rpchhelper->GetDoubletR(hitid);
    int         dbz    = rpchhelper->GetDoubletZ(hitid);
    int         dbp    = rpchhelper->GetDoubletPhi(hitid);
    int         gg     = rpchhelper->GetGasGapLayer(hitid);
    int         mfi    = rpchhelper->GetMeasuresPhi(hitid);

    bool isValid=false;
    // the last 2 arguments are:
    // bool check -> set to true for validity check of identifier
    // &bool isValid -> reference to boolean, which will be false in case of invalid identifier
    // the channelID() function of the RpcIdHelper needs also a stripNumber, but the RpcHitIdHelper 
    // does not seem to return this, so we just give stripNumber=1 for now
    Identifier offid = m_RpcIdHelper->channelID(stname, steta, stphi,dbr,dbz,dbp,gg,mfi,1,true,&isValid);
    if (!isValid) {
       ATH_MSG_WARNING("Cannot build a valid Identifier for RPC stationName="<<stname<<", eta="<<steta<<", phi="<<stphi<<", doubletR="<<dbr<<", doubletZ="<<dbz<<", doubletPhi="<<dbp<<", gasGap="<<gg<<", measuresPhi="<<mfi<<"; skipping...");
       continue;
    }

    const MuonGM::RpcReadoutElement* rpcdet = MuonDetMgr->getRpcReadoutElement(offid);
    if (!rpcdet) {
      ATH_MSG_ERROR("RPCSimHitVariables::fillVariables() - Failed to retrieve RpcReadoutElement for "<<m_RpcIdHelper->print_to_string(offid).c_str());
      return StatusCode::FAILURE;
    }

    m_RPC_Sim_stationName   .push_back(stname);
    m_RPC_stationName   .push_back(m_RpcIdHelper->stationName(offid));
    m_RPC_stationEta   .push_back(m_RpcIdHelper->stationEta(offid));
    m_RPC_stationPhi   .push_back(m_RpcIdHelper->stationPhi(offid));
    m_RPC_isEndcap   .push_back(m_RpcIdHelper->isEndcap(offid));
    m_RPC_strip   .push_back(m_RpcIdHelper->strip(offid)); // vector<int>
    m_RPC_Sim_stationEta    .push_back(steta);
    m_RPC_Sim_stationPhi    .push_back(stphi);
    m_RPC_Sim_doubletR    .push_back(dbr);
    m_RPC_Sim_doubletZ    .push_back(dbz);
    m_RPC_Sim_doubletPhi    .push_back(dbp);
    m_RPC_Sim_gasGapLayer    .push_back(gg);
    m_RPC_Sim_measuresPhi    .push_back(mfi);
    
    m_RPC_globalTime.push_back(hit.globalTime());

    const Amg::Vector3D localPosition = hit.localPosition();
    m_RPC_hitLocalPositionX.push_back(localPosition.x());
    m_RPC_hitLocalPositionY.push_back(localPosition.y());
    m_RPC_hitLocalPositionZ.push_back(localPosition.z());

    Amg::Vector3D simHitPosLoc(localPosition.x(), localPosition.y(), localPosition.z());
    Amg::Vector3D simHitPosGlo = rpcdet->localToGlobalCoords(simHitPosLoc , offid );

    m_RPC_hitGlobalPositionX.push_back(simHitPosGlo.x());
    m_RPC_hitGlobalPositionY.push_back(simHitPosGlo.y());
    m_RPC_hitGlobalPositionZ.push_back(simHitPosGlo.z());
    m_RPC_hitGlobalPositionR.push_back(simHitPosGlo.perp());
    m_RPC_hitGlobalPositionP.push_back(simHitPosGlo.phi());

    Amg::Vector3D detpos = rpcdet->globalPosition();

    m_RPC_detector_globalPositionX.push_back( detpos.x() );
    m_RPC_detector_globalPositionY.push_back( detpos.y() );
    m_RPC_detector_globalPositionZ.push_back( detpos.z() );
    m_RPC_detector_globalPositionR.push_back( detpos.perp() );
    m_RPC_detector_globalPositionP.push_back( detpos.phi() );


    m_RPC_particleEncoding.push_back(hit.particleEncoding());
    m_RPC_kineticEnergy.push_back(hit.kineticEnergy());
    m_RPC_depositEnergy.push_back(hit.energyDeposit());
    m_RPC_StepLength.push_back(hit.stepLength());

    int pdgId=-999;
    int barcode=-999;
    const HepMcParticleLink& pLink = hit.particleLink();
    if (pLink.isValid()) {
      const HepMC::GenParticle* genP = pLink.cptr();
      if (genP) {
        pdgId=genP->pdg_id();
        barcode=HepMC::barcode(genP);
      } else {
        ATH_MSG_WARNING("GenParticle is nullptr for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", doubletR="<<
        dbr<<", doubletZ="<<dbz<<", doubletPhi="<<dbp<<", gasgaplayer="<<gg<<", measuresPhi="<<mfi<<")");
      }
    } else {
      ATH_MSG_WARNING("HepMcParticleLink is not valid for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", doubletR="<<
      dbr<<", doubletZ="<<dbz<<", doubletPhi="<<dbp<<", gasgaplayer="<<gg<<", measuresPhi="<<mfi<<")");
    }
    m_RPC_trackId.push_back(pdgId);
    m_RPC_truthEl.push_back(barcode);

    ++m_RPC_nSimHits;
  }

  ATH_MSG_DEBUG("processed " << m_RPC_nSimHits << " Rpc hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode RPCSimHitVariables::clearVariables()
{
    m_RPC_Sim_stationName.clear();
    m_RPC_stationName.clear();
    m_RPC_stationEta.clear();
    m_RPC_stationPhi.clear();
    m_RPC_isEndcap.clear();
    m_RPC_strip.clear();
    m_RPC_Sim_stationEta.clear();
    m_RPC_Sim_stationPhi.clear();
    m_RPC_Sim_doubletR.clear();
    m_RPC_Sim_doubletZ.clear();
    m_RPC_Sim_doubletPhi.clear();
    m_RPC_Sim_gasGapLayer.clear();
    m_RPC_Sim_measuresPhi.clear();
    m_RPC_globalTime.clear();
    m_RPC_hitLocalPositionX.clear();
    m_RPC_hitLocalPositionY.clear();
    m_RPC_hitLocalPositionZ.clear();
    m_RPC_hitGlobalPositionX.clear();
    m_RPC_hitGlobalPositionY.clear();
    m_RPC_hitGlobalPositionZ.clear();
    m_RPC_hitGlobalPositionR.clear();
    m_RPC_hitGlobalPositionP.clear();
    m_RPC_detector_globalPositionX.clear();
    m_RPC_detector_globalPositionY.clear();
    m_RPC_detector_globalPositionZ.clear();
    m_RPC_detector_globalPositionR.clear();
    m_RPC_detector_globalPositionP.clear();
    m_RPC_particleEncoding.clear();
    m_RPC_kineticEnergy.clear();
    m_RPC_depositEnergy.clear();
    m_RPC_StepLength.clear();
    m_RPC_trackId.clear();
    m_RPC_truthEl.clear();
    m_RPC_nSimHits=0;
    return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode RPCSimHitVariables::initializeVariables()
{
    if(m_tree){
      m_tree->Branch("RPC_Sim_stationName", &m_RPC_Sim_stationName);   
      m_tree->Branch("RPC_stationName", &m_RPC_stationName);
      m_tree->Branch("RPC_stationEta", &m_RPC_stationEta);
      m_tree->Branch("RPC_stationPhi", &m_RPC_stationPhi);
      m_tree->Branch("RPC_isEndcap", &m_RPC_isEndcap);
      m_tree->Branch("RPC_strip", &m_RPC_strip);
      m_tree->Branch("RPC_Sim_stationEta", &m_RPC_Sim_stationEta);
      m_tree->Branch("RPC_Sim_stationPhi", &m_RPC_Sim_stationPhi);                
      m_tree->Branch("RPC_Sim_doubletR", &m_RPC_Sim_doubletR);
      m_tree->Branch("RPC_Sim_doubletZ", &m_RPC_Sim_doubletZ);
      m_tree->Branch("RPC_Sim_doubletPhi", &m_RPC_Sim_doubletPhi);
      m_tree->Branch("RPC_Sim_gasGapLayer", &m_RPC_Sim_gasGapLayer);
      m_tree->Branch("RPC_Sim_measuresPhi", &m_RPC_Sim_measuresPhi);
      m_tree->Branch("RPC_globalTime", &m_RPC_globalTime);
      m_tree->Branch("RPC_hitLocalPositionX", &m_RPC_hitLocalPositionX);
      m_tree->Branch("RPC_hitLocalPositionY", &m_RPC_hitLocalPositionY);
      m_tree->Branch("RPC_hitLocalPositionZ", &m_RPC_hitLocalPositionZ);
      m_tree->Branch("RPC_hitGlobalPositionX", &m_RPC_hitGlobalPositionX);
      m_tree->Branch("RPC_hitGlobalPositionY", &m_RPC_hitGlobalPositionY);
      m_tree->Branch("RPC_hitGlobalPositionZ", &m_RPC_hitGlobalPositionZ);
      m_tree->Branch("RPC_hitGlobalPositionR", &m_RPC_hitGlobalPositionR);
      m_tree->Branch("RPC_hitGlobalPositionP", &m_RPC_hitGlobalPositionP);
      m_tree->Branch("RPC_detector_globalPositionX", &m_RPC_detector_globalPositionX);
      m_tree->Branch("RPC_detector_globalPositionY", &m_RPC_detector_globalPositionY);
      m_tree->Branch("RPC_detector_globalPositionZ", &m_RPC_detector_globalPositionZ);
      m_tree->Branch("RPC_detector_globalPositionR", &m_RPC_detector_globalPositionR);
      m_tree->Branch("RPC_detector_globalPositionP", &m_RPC_detector_globalPositionP);
      m_tree->Branch("RPC_particleEncoding", &m_RPC_particleEncoding);
      m_tree->Branch("RPC_kineticEnergy", &m_RPC_kineticEnergy);
      m_tree->Branch("RPC_depositEnergy", &m_RPC_depositEnergy);
      m_tree->Branch("RPC_StepLength", &m_RPC_StepLength);
      m_tree->Branch("RPC_trackId", &m_RPC_trackId);
      m_tree->Branch("RPC_truthEl", &m_RPC_truthEl);
      m_tree->Branch("RPC_nSimHits", &m_RPC_nSimHits);
  }
  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called in the destructor */
void RPCSimHitVariables::deleteVariables()
{
  return;
}
