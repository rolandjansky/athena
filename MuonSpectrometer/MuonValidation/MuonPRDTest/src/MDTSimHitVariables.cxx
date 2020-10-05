/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MDTSimHitVariables.h"
#include "AthenaKernel/errorcheck.h"

#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/MdtHitIdHelper.h"

#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "AtlasHepMC/GenParticle.h"

#include "TTree.h"
#include <TString.h> // for Form

/** ---------- filling of variables */
/** ---------- to be called on each evt i.e. execute level of main alg */
StatusCode MDTSimHitVariables::fillVariables(const MuonGM::MuonDetectorManager* MuonDetMgr)
{

  ATH_MSG_DEBUG("do fillMDTSimHitVariables()");

  CHECK( this->clearVariables() );

  const MDTSimHitCollection *mdtContainer = nullptr;
  CHECK( m_evtStore->retrieve(mdtContainer, m_ContainerName.c_str() ) );
  
  // Get the MDT Id hit helper
  MdtHitIdHelper* mdthhelper = MdtHitIdHelper::GetHelper(m_MdtIdHelper->tubeMax());

  if(mdtContainer->size()==0) ATH_MSG_WARNING(" MdtSimHit empty ");
  for( auto it : *mdtContainer ) {
    const MDTSimHit hit = it;

    HitID hitid = hit.MDTid();

    std::string stname = mdthhelper->GetStationName(hitid);
    int         steta  = mdthhelper->GetZSector(hitid);
    int         stphi  = mdthhelper->GetPhiSector(hitid);
    int         ml     = mdthhelper->GetMultiLayer(hitid);
    int         tl     = mdthhelper->GetLayer(hitid);
    int         tube   = mdthhelper->GetTube(hitid);

    bool isValid=false;
    // the last 2 arguments are:
    // bool check -> set to true for validity check of identifier
    // &bool isValid -> reference to boolean, which will be false in case of invalid identifier
    Identifier offid = m_MdtIdHelper->channelID(stname, steta, stphi,ml,tl,tube,true,&isValid);
    if (!isValid) {
       ATH_MSG_WARNING(" Cannot build a valid Identifier; skip ");
       continue;
    }

    const MuonGM::MdtReadoutElement* mdtdet = MuonDetMgr->getMdtReadoutElement(offid);
    if (!mdtdet) throw std::runtime_error(Form("File: %s, Line: %d\nMDTSimHitVariables::fillVariables() - Failed to retrieve MdtReadoutElement for %s", __FILE__, __LINE__, m_MdtIdHelper->print_to_string(offid).c_str()));

    m_MDT_Sim_stationName   ->push_back(stname);
    m_MDT_stationName   ->push_back(m_MdtIdHelper->stationName(offid));
    m_MDT_stationEta   ->push_back(m_MdtIdHelper->stationEta(offid));
    m_MDT_stationPhi   ->push_back(m_MdtIdHelper->stationPhi(offid));
    m_MDT_isEndcap   ->push_back(m_MdtIdHelper->isEndcap(offid));
    m_MDT_Sim_stationEta    ->push_back(steta);
    m_MDT_Sim_stationPhi    ->push_back(stphi);
    m_MDT_Sim_multilayer     ->push_back(ml);
    m_MDT_Sim_tubelayer       ->push_back(tl);
    m_MDT_Sim_tube       ->push_back(tube);
    
    m_MDT_globalTime->push_back(hit.globalTime());

    const Amg::Vector3D localPosition = hit.localPosition();
    m_MDT_hitLocalPositionX->push_back(localPosition.x());
    m_MDT_hitLocalPositionY->push_back(localPosition.y());
    m_MDT_hitLocalPositionZ->push_back(localPosition.z());

    Amg::Vector3D simHitPosLoc(localPosition.x(), localPosition.y(), localPosition.z());
    Amg::Vector3D simHitPosGlo = mdtdet->localToGlobalCoords(simHitPosLoc , offid );

    m_MDT_hitGlobalPositionX->push_back(simHitPosGlo.x());
    m_MDT_hitGlobalPositionY->push_back(simHitPosGlo.y());
    m_MDT_hitGlobalPositionZ->push_back(simHitPosGlo.z());
    m_MDT_hitGlobalPositionR->push_back(simHitPosGlo.perp());
    m_MDT_hitGlobalPositionP->push_back(simHitPosGlo.phi());

    Amg::Vector3D detpos = mdtdet->globalPosition();

    m_MDT_detector_globalPositionX->push_back( detpos.x() );
    m_MDT_detector_globalPositionY->push_back( detpos.y() );
    m_MDT_detector_globalPositionZ->push_back( detpos.z() );
    m_MDT_detector_globalPositionR->push_back( detpos.perp() );
    m_MDT_detector_globalPositionP->push_back( detpos.phi() );

    m_MDT_driftRadius->push_back( hit.driftRadius());


    m_MDT_particleEncoding->push_back(hit.particleEncoding());
    m_MDT_kineticEnergy->push_back(hit.kineticEnergy());
    m_MDT_depositEnergy->push_back(hit.energyDeposit());
    m_MDT_StepLength->push_back(hit.stepLength());

    int pdgId=-999;
    int barcode=-999;
    const HepMcParticleLink& pLink = hit.particleLink();
    if (pLink.isValid()) {
      const HepMC::GenParticle* genP = pLink.cptr();
      if (genP) {
        pdgId=genP->pdg_id();
        barcode=genP->barcode();
      } else {
        ATH_MSG_WARNING("GenParticle is nullptr for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", ml="<<
        ml<<", tubelayer="<<tl<<", tube="<<tube<<")");
      }
    } else {
      ATH_MSG_WARNING("HepMcParticleLink is not valid for hit in "<<stname<<" (eta="<<steta<<", phi="<<stphi<<", ml="<<
      ml<<", tubelayer="<<tl<<", tube="<<tube<<")");
    }
    m_MDT_trackId->push_back(pdgId);
    m_MDT_truthEl->push_back(barcode);

    ++m_MDT_nSimHits;
  }

  ATH_MSG_DEBUG("processed " << m_MDT_nSimHits << " Mdt hits");
  return StatusCode::SUCCESS;
}


/** ---------- clearing of variables */
/** ---------- to be called inside filling method before filling starts */
StatusCode MDTSimHitVariables::clearVariables()
{

    m_MDT_Sim_stationName->clear();
    m_MDT_stationName->clear();
    m_MDT_stationEta->clear();
    m_MDT_stationPhi->clear();
    m_MDT_isEndcap->clear();
    m_MDT_Sim_stationEta->clear();
    m_MDT_Sim_stationPhi->clear();
    m_MDT_Sim_multilayer->clear();
    m_MDT_Sim_tubelayer->clear();
    m_MDT_Sim_tube->clear();
    m_MDT_globalTime->clear();
    m_MDT_hitLocalPositionX->clear();
    m_MDT_hitLocalPositionY->clear();
    m_MDT_hitLocalPositionZ->clear();
    m_MDT_hitGlobalPositionX->clear();
    m_MDT_hitGlobalPositionY->clear();
    m_MDT_hitGlobalPositionZ->clear();
    m_MDT_hitGlobalPositionR->clear();
    m_MDT_hitGlobalPositionP->clear();
    m_MDT_detector_globalPositionX->clear();
    m_MDT_detector_globalPositionY->clear();
    m_MDT_detector_globalPositionZ->clear();
    m_MDT_detector_globalPositionR->clear();
    m_MDT_detector_globalPositionP->clear();
    m_MDT_driftRadius->clear();
    m_MDT_particleEncoding->clear();
    m_MDT_kineticEnergy->clear();
    m_MDT_depositEnergy->clear();
    m_MDT_StepLength->clear();
    m_MDT_trackId->clear();
    m_MDT_truthEl->clear();
    m_MDT_nSimHits=0;
 

  return StatusCode::SUCCESS;
}


/** ---------- creating variables and associate them to branches */
/** ---------- to be called on initialization level of main alg */
StatusCode MDTSimHitVariables::initializeVariables()
{
    m_MDT_Sim_stationName = new std::vector<std::string>;
    m_MDT_stationName = new std::vector<int>;
    m_MDT_stationEta = new std::vector<int>;
    m_MDT_stationPhi = new std::vector<int>;
    m_MDT_isEndcap = new std::vector<bool>;
    m_MDT_Sim_stationEta = new std::vector<int>;
    m_MDT_Sim_stationPhi = new std::vector<int>;
    m_MDT_Sim_multilayer = new std::vector<int>;
    m_MDT_Sim_tubelayer = new std::vector<int>;
    m_MDT_Sim_tube = new std::vector<int>;
    m_MDT_globalTime = new std::vector<double>;
    m_MDT_hitLocalPositionX = new std::vector<double>;
    m_MDT_hitLocalPositionY = new std::vector<double>;
    m_MDT_hitLocalPositionZ = new std::vector<double>;
    m_MDT_hitGlobalPositionX = new std::vector<double>;
    m_MDT_hitGlobalPositionY = new std::vector<double>;
    m_MDT_hitGlobalPositionZ = new std::vector<double>;
    m_MDT_hitGlobalPositionR = new std::vector<double>;
    m_MDT_hitGlobalPositionP = new std::vector<double>;
    m_MDT_detector_globalPositionX = new std::vector<double>;
    m_MDT_detector_globalPositionY = new std::vector<double>;
    m_MDT_detector_globalPositionZ = new std::vector<double>;
    m_MDT_detector_globalPositionR = new std::vector<double>;
    m_MDT_detector_globalPositionP = new std::vector<double>;
    m_MDT_driftRadius = new std::vector<double>;

    m_MDT_particleEncoding = new std::vector<int>;
    m_MDT_kineticEnergy = new std::vector<double>;
    m_MDT_depositEnergy = new std::vector<double>;
    m_MDT_StepLength = new std::vector<double>;
    m_MDT_trackId = new std::vector<int>;
    m_MDT_truthEl = new std::vector<int>;
    m_MDT_nSimHits = 0;
 
    if(m_tree){
      m_tree->Branch("MDT_Sim_stationName", &m_MDT_Sim_stationName);   
      m_tree->Branch("MDT_stationName", &m_MDT_stationName);
      m_tree->Branch("MDT_stationEta", &m_MDT_stationEta);
      m_tree->Branch("MDT_stationPhi", &m_MDT_stationPhi);
      m_tree->Branch("MDT_isEndcap", &m_MDT_isEndcap);
      m_tree->Branch("MDT_Sim_stationEta", &m_MDT_Sim_stationEta);
      m_tree->Branch("MDT_Sim_stationPhi", &m_MDT_Sim_stationPhi);
      m_tree->Branch("MDT_Sim_multilayer", &m_MDT_Sim_multilayer);
      m_tree->Branch("MDT_Sim_tubelayer", &m_MDT_Sim_tubelayer);
      m_tree->Branch("MDT_Sim_tube", &m_MDT_Sim_tube);
      m_tree->Branch("MDT_globalTime", &m_MDT_globalTime);
      m_tree->Branch("MDT_hitLocalPositionX", &m_MDT_hitLocalPositionX);
      m_tree->Branch("MDT_hitLocalPositionY", &m_MDT_hitLocalPositionY);
      m_tree->Branch("MDT_hitLocalPositionZ", &m_MDT_hitLocalPositionZ);
      m_tree->Branch("MDT_hitGlobalPositionX", &m_MDT_hitGlobalPositionX);
      m_tree->Branch("MDT_hitGlobalPositionY", &m_MDT_hitGlobalPositionY);
      m_tree->Branch("MDT_hitGlobalPositionZ", &m_MDT_hitGlobalPositionZ);
      m_tree->Branch("MDT_hitGlobalPositionR", &m_MDT_hitGlobalPositionR);
      m_tree->Branch("MDT_hitGlobalPositionP", &m_MDT_hitGlobalPositionP);
      m_tree->Branch("MDT_detector_globalPositionX", &m_MDT_detector_globalPositionX);
      m_tree->Branch("MDT_detector_globalPositionY", &m_MDT_detector_globalPositionY);
      m_tree->Branch("MDT_detector_globalPositionZ", &m_MDT_detector_globalPositionZ);
      m_tree->Branch("MDT_detector_globalPositionR", &m_MDT_detector_globalPositionR);
      m_tree->Branch("MDT_detector_globalPositionP", &m_MDT_detector_globalPositionP);
      m_tree->Branch("MDT_driftRadius", &m_MDT_driftRadius);

      m_tree->Branch("MDT_particleEncoding", &m_MDT_particleEncoding);
      m_tree->Branch("MDT_kineticEnergy", &m_MDT_kineticEnergy);
      m_tree->Branch("MDT_depositEnergy", &m_MDT_depositEnergy);
      m_tree->Branch("MDT_StepLength", &m_MDT_StepLength);
      m_tree->Branch("MDT_trackId", &m_MDT_trackId);
      m_tree->Branch("MDT_truthEl", &m_MDT_truthEl);
      m_tree->Branch("MDT_nSimHits", &m_MDT_nSimHits);
  }

  return StatusCode::SUCCESS;
}


/** ---------- freeing resources and resetting pointers */
/** ---------- to be called in the destructor */
void MDTSimHitVariables::deleteVariables()
{
    delete m_MDT_Sim_stationName;
    delete m_MDT_stationName;
    delete m_MDT_stationEta;
    delete m_MDT_stationPhi;
    delete m_MDT_isEndcap;
    delete m_MDT_Sim_stationEta;
    delete m_MDT_Sim_stationPhi;
    delete m_MDT_Sim_multilayer;
    delete m_MDT_Sim_tubelayer;
    delete m_MDT_Sim_tube;
    delete m_MDT_globalTime;
    delete m_MDT_hitLocalPositionX;
    delete m_MDT_hitLocalPositionY;
    delete m_MDT_hitLocalPositionZ;
    delete m_MDT_hitGlobalPositionX;
    delete m_MDT_hitGlobalPositionY;
    delete m_MDT_hitGlobalPositionZ;
    delete m_MDT_hitGlobalPositionR;
    delete m_MDT_hitGlobalPositionP;
    delete m_MDT_detector_globalPositionX;
    delete m_MDT_detector_globalPositionY;
    delete m_MDT_detector_globalPositionZ;
    delete m_MDT_detector_globalPositionR;
    delete m_MDT_detector_globalPositionP;
    delete m_MDT_driftRadius;
    delete m_MDT_particleEncoding;
    delete m_MDT_kineticEnergy;
    delete m_MDT_depositEnergy;
    delete m_MDT_StepLength;
    delete m_MDT_trackId;
    delete m_MDT_truthEl;
    m_MDT_nSimHits=0;

    m_MDT_Sim_stationName = nullptr;
    m_MDT_stationName = nullptr;
    m_MDT_stationEta = nullptr;
    m_MDT_stationPhi = nullptr;
    m_MDT_isEndcap = nullptr;
    m_MDT_Sim_stationEta = nullptr;
    m_MDT_Sim_stationPhi = nullptr;
    m_MDT_Sim_multilayer = nullptr;
    m_MDT_Sim_tubelayer = nullptr;
    m_MDT_Sim_tube = nullptr;
    m_MDT_globalTime = nullptr;
    m_MDT_hitLocalPositionX = nullptr;
    m_MDT_hitLocalPositionY = nullptr;
    m_MDT_hitLocalPositionZ = nullptr;
    m_MDT_hitGlobalPositionX = nullptr;
    m_MDT_hitGlobalPositionY = nullptr;
    m_MDT_hitGlobalPositionZ = nullptr;
    m_MDT_hitGlobalPositionR = nullptr;
    m_MDT_hitGlobalPositionP = nullptr;
    m_MDT_detector_globalPositionX = nullptr;
    m_MDT_detector_globalPositionY = nullptr;
    m_MDT_detector_globalPositionZ = nullptr;
    m_MDT_detector_globalPositionR = nullptr;
    m_MDT_detector_globalPositionP = nullptr;
    m_MDT_driftRadius = nullptr;
    m_MDT_particleEncoding = nullptr;
    m_MDT_kineticEnergy = nullptr;
    m_MDT_depositEnergy = nullptr;
    m_MDT_StepLength = nullptr;
    m_MDT_trackId = nullptr;
    m_MDT_truthEl = nullptr;

  return;
}
