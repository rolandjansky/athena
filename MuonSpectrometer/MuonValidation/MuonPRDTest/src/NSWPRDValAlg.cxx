/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <stdlib.h>
#include "MuonPRDTest/NSWPRDValAlg.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/sTgcReadoutElement.h"
#include "MuonReadoutGeometry/MMReadoutElement.h"
#include "MuonReadoutGeometry/NSWenumeration.h"
#include "MuonReadoutGeometry/NSWgeometry.h"

#include "MuonIdHelpers/MmIdHelper.h"
#include "MuonIdHelpers/sTgcIdHelper.h"

#include "xAODEventInfo/EventInfo.h"

#include "TrackRecord/TrackRecordCollection.h"

#include "GeneratorObjects/McEventCollection.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenRanges.h"
#include "HepMC/GenVertex.h"

#include "CLHEP/Vector/ThreeVector.h"

#include "MuonSimEvent/GenericMuonSimHitCollection.h"
#include "MuonSimEvent/sTgcHitIdHelper.h"
#include "MuonSimEvent/MM_SimIdToOfflineId.h"
#include "MuonSimEvent/sTgcSimIdToOfflineId.h"

#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonDigitContainer/MmDigit.h"

#include "MuonPrepRawData/MMPrepDataContainer.h"
#include "MuonPrepRawData/MMPrepData.h"

#include "MuonPrepRawData/sTgcPrepDataContainer.h"
#include "MuonPrepRawData/sTgcPrepData.h"

#include "MuonSimData/MuonSimDataCollection.h"
#include "MuonSimData/MuonSimData.h"


//#include <strstream>
//#include <iomanip>


NSWPRDValAlg::NSWPRDValAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator), 
    m_thistSvc(0),
    m_tree(0), 
    m_detManager(0),
    m_MmIdHelper(0),
    m_sTgcIdHelper(0),
    m_runNumber(0),
    m_eventNumber(0),
    m_Truth_nVertices(0),
    m_Truth_vertexX(0),
    m_Truth_vertexY(0),
    m_Truth_vertexZ(0),
    m_Truth_vertexT(0),
    m_Truth_vertexId(0),
    m_Truth_nParticles(0), 
    m_Truth_particlePt(0), 
    m_Truth_particleEta(0), 
    m_Truth_particlePhi(0), 
    m_Truth_particleE(0), 
    m_Truth_particleM(0), 
    m_Truth_particlePdg_id(0), 
    m_Truth_particleStatus(0), 
    m_Truth_particleBarcode(0),
    m_Truth_particleProduction_vertex_id(0), 
    m_Truth_particleEnd_vertex_id(0), 

    m_MuEntry_nParticles(0), 
    m_MuEntry_particlePt(0), 
    m_MuEntry_particleEta(0), 
    m_MuEntry_particlePhi(0), 
    m_MuEntry_particlePdg_id(0), 
    m_MuEntry_particleBarcode(0),
    m_MuEntry_positionEta(0), 
    m_MuEntry_positionPhi(0), 
    m_MuEntry_positionX(0),
    m_MuEntry_positionY(0),
    m_MuEntry_positionZ(0),
  
    m_NSWsTGC_nSimHits(0), 
    m_NSWsTGC_trackId(0),
    m_NSWsTGC_truthEl(0),
    m_NSWsTGC_globalTime(0), 
    m_NSWsTGC_hitGlobalPositionX(0), 
    m_NSWsTGC_hitGlobalPositionY(0), 
    m_NSWsTGC_hitGlobalPositionZ(0), 
    m_NSWsTGC_hitGlobalPositionR(0),
    m_NSWsTGC_hitGlobalPositionP(0),
    m_NSWsTGC_hitGlobalDirectionX(0), 
    m_NSWsTGC_hitGlobalDirectionY(0), 
    m_NSWsTGC_hitGlobalDirectionZ(0), 

    m_NSWsTGC_hitLocalPositionX(0), 
    m_NSWsTGC_hitLocalPositionY(0), 
    m_NSWsTGC_hitLocalPositionZ(0), 

    m_NSWsTGC_detector_globalPositionX(0),
    m_NSWsTGC_detector_globalPositionY(0),
    m_NSWsTGC_detector_globalPositionZ(0),
    m_NSWsTGC_detector_globalPositionR(0),
    m_NSWsTGC_detector_globalPositionP(0),

    m_NSWsTGC_hitToDsurfacePositionX(0),
    m_NSWsTGC_hitToDsurfacePositionY(0),
    m_NSWsTGC_hitToDsurfacePositionZ(0),

    m_NSWsTGC_hitToRsurfacePositionX(0),
    m_NSWsTGC_hitToRsurfacePositionY(0),
    m_NSWsTGC_hitToRsurfacePositionZ(0),

    m_NSWsTGC_FastDigitRsurfacePositionX(0),
    m_NSWsTGC_FastDigitRsurfacePositionY(0),

    m_NSWsTGC_particleEncoding(0), 
    m_NSWsTGC_kineticEnergy(0), 
    m_NSWsTGC_depositEnergy(0), 
    m_NSWsTGC_StepLength(0),

    m_NSWsTGC_sim_stationName(0),
    m_NSWsTGC_wedgeId(0),
    m_NSWsTGC_wedgeType(0),
    m_NSWsTGC_detectorNumber(0),
    m_NSWsTGC_sim_stationEta(0),
    m_NSWsTGC_sim_stationPhi(0),
    m_NSWsTGC_sim_multilayer(0),
    m_NSWsTGC_sim_layer(0),
    m_NSWsTGC_sim_side(0),
    m_NSWsTGC_stripNumber(0),
    m_NSWsTGC_wireNumber(0),

    m_NSWsTGC_off_stationName(0),
    m_NSWsTGC_off_stationEta(0),                                         
    m_NSWsTGC_off_stationPhi(0),
    m_NSWsTGC_off_multiplet(0),
    m_NSWsTGC_off_gas_gap(0),
    m_NSWsTGC_off_channel_type(0),
    m_NSWsTGC_off_channel(0),

    m_NSWsTGC_nFastDigits(0),
    m_NSWsTGC_fdg_stationName(0),
    m_NSWsTGC_fdg_stationEta(0),
    m_NSWsTGC_fdg_stationPhi(0),
    m_NSWsTGC_fdg_multiplet(0),
    m_NSWsTGC_fdg_gas_gap(0),
    m_NSWsTGC_fdg_channel_type(0),
    m_NSWsTGC_fdg_channel(0),

    m_NSWsTGC_fdg_globalPosX(0),
    m_NSWsTGC_fdg_globalPosY(0),
    m_NSWsTGC_fdg_globalPosZ(0),

    m_NSWsTGC_fdg_localPosX(0),
    m_NSWsTGC_fdg_localPosY(0),
    m_NSWsTGC_fdg_covMatrix_1_1(0),
    m_NSWsTGC_fdg_covMatrix_2_2(0),

    m_NSWsTGC_fdg_truth_barcode(0),
    m_NSWsTGC_fdg_truth_hitOnSurfaceX(0),
    m_NSWsTGC_fdg_truth_hitOnSurfaceY(0),

    m_NSWsTGC_nDigits(0),
    m_NSWsTGC_dig_stationName(0),
    m_NSWsTGC_dig_stationEta(0),
    m_NSWsTGC_dig_stationPhi(0),
    m_NSWsTGC_dig_multiplet(0),
    m_NSWsTGC_dig_gas_gap(0),
    m_NSWsTGC_dig_channel_type(0),
    m_NSWsTGC_dig_channel(0),

    m_NSWMM_nSimHits(0), 
    m_NSWMM_trackId(0),
    m_NSWMM_truthEl(0),
    m_NSWMM_globalTime(0), 
    m_NSWMM_hitGlobalPositionX(0), 
    m_NSWMM_hitGlobalPositionY(0), 
    m_NSWMM_hitGlobalPositionZ(0), 
    m_NSWMM_hitGlobalPositionR(0),
    m_NSWMM_hitGlobalPositionP(0),
    m_NSWMM_hitGlobalDirectionX(0), 
    m_NSWMM_hitGlobalDirectionY(0), 
    m_NSWMM_hitGlobalDirectionZ(0), 

    m_NSWMM_hitLocalPositionX(0), 
    m_NSWMM_hitLocalPositionY(0), 
    m_NSWMM_hitLocalPositionZ(0), 

    m_NSWMM_detector_globalPositionX(0),
    m_NSWMM_detector_globalPositionY(0),
    m_NSWMM_detector_globalPositionZ(0),
    m_NSWMM_detector_globalPositionR(0),
    m_NSWMM_detector_globalPositionP(0),

    m_NSWMM_hitToDsurfacePositionX(0),
    m_NSWMM_hitToDsurfacePositionY(0),
    m_NSWMM_hitToDsurfacePositionZ(0),

    m_NSWMM_hitToRsurfacePositionX(0),
    m_NSWMM_hitToRsurfacePositionY(0),
    m_NSWMM_hitToRsurfacePositionZ(0),

    m_NSWMM_FastDigitRsurfacePositionX(0),
    m_NSWMM_FastDigitRsurfacePositionY(0),

    m_NSWMM_particleEncoding(0), 
    m_NSWMM_kineticEnergy(0), 
    m_NSWMM_depositEnergy(0), 
    m_NSWMM_StepLength(0),

    m_NSWMM_sim_stationName(0),
    m_NSWMM_sim_stationEta(0),
    m_NSWMM_sim_stationPhi(0),
    m_NSWMM_sim_multilayer(0),
    m_NSWMM_sim_layer(0),
    m_NSWMM_sim_side(0),

    m_NSWMM_off_stationName(0),
    m_NSWMM_off_stationEta(0),
    m_NSWMM_off_stationPhi(0),
    m_NSWMM_off_multiplet(0),
    m_NSWMM_off_gas_gap(0),
    m_NSWMM_off_channel(0),

    m_NSWMM_nFastDigits(0), 
    m_NSWMM_fdg_stationName(0),
    m_NSWMM_fdg_stationEta(0),
    m_NSWMM_fdg_stationPhi(0),
    m_NSWMM_fdg_multiplet(0),
    m_NSWMM_fdg_gas_gap(0),
    m_NSWMM_fdg_channel(0),

    m_NSWMM_fdg_globalPosX(0),
    m_NSWMM_fdg_globalPosY(0),
    m_NSWMM_fdg_globalPosZ(0),

    m_NSWMM_fdg_localPosX(0),
    m_NSWMM_fdg_localPosY(0),
    m_NSWMM_fdg_covMatrix_1_1(0),

    m_NSWMM_fdg_truth_barcode(0),
    m_NSWMM_fdg_truth_hitOnSurfaceX(0),
    m_NSWMM_fdg_truth_hitOnSurfaceY(0),

    m_NSWMM_nDigits(0),
    m_NSWMM_dig_stationName(0),
    m_NSWMM_dig_stationEta(0),
    m_NSWMM_dig_stationPhi(0),
    m_NSWMM_dig_multiplet(0),
    m_NSWMM_dig_gas_gap(0),
    m_NSWMM_dig_channel(0),
    m_NSWMM_dig_time(0),
    m_NSWMM_dig_charge(0),
    m_NSWMM_dig_stripPosition(0),
    m_NSWMM_dig_stripLposX(0),
    m_NSWMM_dig_stripLposY(0),
    m_NSWMM_dig_stripGposX(0),
    m_NSWMM_dig_stripGposY(0),
    m_NSWMM_dig_stripGposZ(0),
    m_NSWMM_dig_sr_time(0),
    m_NSWMM_dig_sr_charge(0),
    m_NSWMM_dig_sr_stripPosition(0),
    m_NSWMM_dig_sr_stripLposX(0),
    m_NSWMM_dig_sr_stripLposY(0),
    m_NSWMM_dig_sr_stripGposX(0),
    m_NSWMM_dig_sr_stripGposY(0),
    m_NSWMM_dig_sr_stripGposZ(0),
    m_NSWMM_dig_truth_barcode(0),
    m_NSWMM_dig_truth_localPosX(0),
    m_NSWMM_dig_truth_localPosY(0),
    m_NSWMM_dig_truth_globalPosX(0),
    m_NSWMM_dig_truth_globalPosY(0),
    m_NSWMM_dig_truth_globalPosZ(0),
    m_NSWMM_dig_truth_XZ_angle(0),
    m_NSWMM_dig_stripForTrigger(0),
    m_NSWMM_dig_stripTimeForTrigger(0)
{
  declareProperty("Truth_ContainerName", m_Truth_ContainerName="TruthEvent");
  declareProperty("MuonEntryLayer_ContainerName", m_MuEntry_ContainerName="MuonEntryLayer");
  declareProperty("NSWsTGC_ContainerName", m_NSWsTGC_ContainerName="sTGCSensitiveDetector");
  declareProperty("NSWsTGC_FastDigitContainerName", m_NSWsTGC_FastDigitContainerName="STGC_Measurements");
  declareProperty("NSWsTGC_DigitContainerName", m_NSWsTGC_DigitContainerName="");
  declareProperty("NSWMM_ContainerName", m_NSWMM_ContainerName="MicromegasSensitiveDetector");
  declareProperty("NSWMM_FastDigitContainerName", m_NSWMM_FastDigitContainerName="MM_Measurements");
  declareProperty("NSWMM_DigitContainerName", m_NSWMM_DigitContainerName="MM_DIGITS");

  declareProperty("doTruth",         m_doTruth=true);
  declareProperty("doMuEntry",       m_doMuEntry=true);
  declareProperty("doSTGCHit",       m_doSTGCHit=true);
  declareProperty("doSTGCFastDigit", m_doSTGCFastDigit=true);
  declareProperty("doSTGCDigit",     m_doSTGCDigit=false);
  declareProperty("doMMHit",         m_doMMHit=true);
  declareProperty("doMMFastDigit",   m_doMMFastDigit=true);
  declareProperty("doMMDigit",       m_doMMDigit=true);
  declareProperty("EnergyThreshold", m_energyThreshold = 50., "Minimal energy to produce a PRD"  );
}

StatusCode NSWPRDValAlg::initialize() {
  ATH_MSG_INFO("initialize()");
  
  StatusCode sc = service("THistSvc", m_thistSvc);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Unable to retrieve THistSvc");
    return sc;
  }

  m_tree = new TTree("NSWHitsTree", "Ntuple of NSWHits");
  m_tree->Branch("runNumber", &m_runNumber, "runNumber/i");
  m_tree->Branch("eventNumber", &m_eventNumber, "eventNumber/i");

  if (m_doTruth)         initializeTruthVariables();
  if (m_doMuEntry)       initializeMuEntryVariables();
  if (m_doSTGCHit)       initializeNSWsTGCHitVariables();
  if (m_doSTGCFastDigit) initializeNSWsTGCFastDigitVariables();
  if (m_doSTGCDigit)     initializeNSWsTGCDigitVariables();
  if (m_doMMHit)         initializeNSWMMHitVariables();
  if (m_doMMFastDigit)   initializeNSWMMFastDigitVariables();
  if (m_doMMDigit)       initializeNSWMMDigitVariables();

  sc = m_thistSvc->regTree("/NSWPRDValAlg/NSWHitsTree", m_tree);
  if(sc.isFailure()) {
    ATH_MSG_FATAL("Unable to register " << m_tree->GetName());
    return sc;
  }

  // retrieve the detector Manager
  if ( detStore()->retrieve( m_detManager ).isFailure()) {
    ATH_MSG_WARNING("Failed to retrieve MuonDetectorManager");
    return StatusCode::FAILURE;
  }

  //  retrieve the MM offline Id helper
  if( detStore()->retrieve( m_MmIdHelper ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve MmIdHelper");
    return StatusCode::FAILURE;
  }

  //  retrieve the sTGC offline Id helper
  if( detStore()->retrieve( m_sTgcIdHelper ).isFailure() ){
    ATH_MSG_WARNING("Failed to retrieve sTgcIdHelper");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::finalize() 
{
  ATH_MSG_INFO("finalize()");

  if (m_doTruth)         deleteTruthVariables();
  if (m_doMuEntry)       deleteMuEntryVariables();
  if (m_doSTGCHit)       deleteNSWsTGCHitVariables();
  if (m_doSTGCFastDigit) deleteNSWsTGCFastDigitVariables();
  if (m_doSTGCDigit)     deleteNSWsTGCDigitVariables();
  if (m_doMMHit)         deleteNSWMMHitVariables();
  if (m_doMMFastDigit)   deleteNSWMMFastDigitVariables();
  if (m_doMMDigit)       deleteNSWMMDigitVariables();
  return StatusCode::SUCCESS;
}
 
StatusCode NSWPRDValAlg::execute() 
{
  ATH_MSG_INFO("execute()");

  // Event information
  const xAOD::EventInfo *pevt = 0;
  StatusCode sc = evtStore()->retrieve(pevt);
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve event info");
    return StatusCode::SUCCESS;
  }
  m_runNumber = pevt->runNumber();
  m_eventNumber = pevt->eventNumber();

  // Truth information
  if (m_doTruth)  {
      sc = fillTruthVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillTruthVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }

  // Truth information
  if (m_doMuEntry)  {
      sc = fillMuEntryVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillMuEntryVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }
  
  // Fill sTGC hit variables
  if (m_doSTGCHit) {
      sc = fillNSWsTGCHitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWsTGCHitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }

  // Fill sTGC fast digitization variables
  if (m_doSTGCFastDigit) {
      sc = fillNSWsTGCFastDigitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWsTGCFastDigitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }

  // Fill sTGC fast digitization variables
  if (m_doSTGCDigit) {
      sc = fillNSWsTGCDigitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWsTGCDigitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }


  // Fill MicroMegas hit variables
  if (m_doMMHit) {
      sc = fillNSWMMHitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWMMHitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }

  // Fill MicroMegas fast digit variables
  if (m_doMMFastDigit) {
      sc = fillNSWMMFastDigitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWMMFastDigitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }

  // Fill MicroMegas digit variables
  if (m_doMMDigit) {
      sc = fillNSWMMDigitVariables();
      if(sc.isFailure()) {
          ATH_MSG_FATAL("fillNSWMMDigitVariables() returned StatusCode::FAILURE.");
          return sc;
      }
  }



  m_tree->Fill();

  return StatusCode::SUCCESS;
}

void NSWPRDValAlg::initializeTruthVariables() 
{
  m_Truth_nVertices = 0;
  m_Truth_vertexX = new std::vector<double>;
  m_Truth_vertexY = new std::vector<double>;
  m_Truth_vertexZ = new std::vector<double>;
  m_Truth_vertexT = new std::vector<double>;
  m_Truth_vertexId = new std::vector<int>;
  m_Truth_nParticles = 0;
  m_Truth_particlePt = new std::vector<double>;
  m_Truth_particleEta = new std::vector<double>;
  m_Truth_particlePhi = new std::vector<double>;
  m_Truth_particleE = new std::vector<double>;
  m_Truth_particleM = new std::vector<double>;
  m_Truth_particlePdg_id = new std::vector<int>;
  m_Truth_particleStatus = new std::vector<int>;
  m_Truth_particleBarcode = new std::vector<int>;
  m_Truth_particleProduction_vertex_id = new std::vector<int>;
  m_Truth_particleEnd_vertex_id = new std::vector<int>;
  if(m_tree) {
    m_tree->Branch("TruthVertex_n", &m_Truth_nVertices, "TruthVertex_n/i");
    m_tree->Branch("TruthVertex_X", &m_Truth_vertexX);
    m_tree->Branch("TruthVertex_Y", &m_Truth_vertexY);
    m_tree->Branch("TruthVertex_Z", &m_Truth_vertexZ);    
    m_tree->Branch("TruthVertex_T", &m_Truth_vertexT);    
    m_tree->Branch("TruthVertex_Id", &m_Truth_vertexId);    
    m_tree->Branch("TruthParticle_n", &m_Truth_nParticles, "TruthParticle_n/i");
    m_tree->Branch("TruthParticle_Pt", &m_Truth_particlePt);
    m_tree->Branch("TruthParticle_Eta", &m_Truth_particleEta);
    m_tree->Branch("TruthParticle_Phi", &m_Truth_particlePhi);
    m_tree->Branch("TruthParticle_E", &m_Truth_particleE);
    m_tree->Branch("TruthParticle_M", &m_Truth_particleM);
    m_tree->Branch("TruthParticle_Pdg", &m_Truth_particlePdg_id);
    m_tree->Branch("TruthParticle_Status", &m_Truth_particleStatus);
    m_tree->Branch("TruthParticle_Barcode", &m_Truth_particleBarcode);
    m_tree->Branch("TruthParticle_Production_vertex_id", &m_Truth_particleProduction_vertex_id);
    m_tree->Branch("TruthParticle_End_vertex_id", &m_Truth_particleEnd_vertex_id);
  }
}

void NSWPRDValAlg::clearTruthVariables() {
  m_Truth_nVertices = 0;
  m_Truth_vertexX->clear();
  m_Truth_vertexY->clear();
  m_Truth_vertexZ->clear();
  m_Truth_vertexT->clear();
  m_Truth_vertexId->clear();
  m_Truth_nParticles = 0;
  m_Truth_particlePt->clear();
  m_Truth_particleEta->clear();
  m_Truth_particlePhi->clear();
  m_Truth_particleE->clear();
  m_Truth_particleM->clear();
  m_Truth_particlePdg_id->clear();
  m_Truth_particleStatus->clear();
  m_Truth_particleBarcode->clear();
  m_Truth_particleProduction_vertex_id->clear();
  m_Truth_particleEnd_vertex_id->clear();
}

StatusCode NSWPRDValAlg::fillTruthVariables() {
  clearTruthVariables();

  const McEventCollection *truthContainer = 0;
  StatusCode sc = evtStore()->retrieve(truthContainer, m_Truth_ContainerName.c_str());
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve " << m_Truth_ContainerName.c_str());
    return StatusCode::SUCCESS;
  }

  unsigned int nSubEvents = 0;
  McEventCollection::const_iterator it   = truthContainer->begin();
  McEventCollection::const_iterator it_e = truthContainer->end();
  for(; it!=it_e; ++it) {
    const HepMC::GenEvent *subEvent = *it;

    // Vertex 
    HepMC::ConstGenEventVertexRange vertex_range = subEvent->vertex_range();
    HepMC::GenEvent::vertex_const_iterator vit   = vertex_range.begin();
    HepMC::GenEvent::vertex_const_iterator vit_e = vertex_range.end();
    for(; vit!=vit_e; ++vit) {
      const HepMC::GenVertex *vertex = *vit;
      const HepMC::FourVector position = vertex->position();
      m_Truth_vertexX->push_back(position.x());
      m_Truth_vertexY->push_back(position.y());
      m_Truth_vertexZ->push_back(position.z());
      m_Truth_vertexT->push_back(position.t());
      m_Truth_vertexId->push_back(vertex->id());
      m_Truth_nVertices++;
    }

    // Particle 
    HepMC::ConstGenEventParticleRange particle_range = subEvent->particle_range();
    HepMC::GenEvent::particle_const_iterator pit   = particle_range.begin();
    HepMC::GenEvent::particle_const_iterator pit_e = particle_range.end();
    for(; pit!=pit_e; ++pit) {
      const HepMC::GenParticle *particle = *pit;
      const HepMC::FourVector momentum = particle->momentum();
      m_Truth_particlePt->push_back(momentum.perp());
      m_Truth_particleEta->push_back(momentum.eta());
      m_Truth_particlePhi->push_back(momentum.phi());
      m_Truth_particleE->push_back(momentum.e());
      m_Truth_particleM->push_back(momentum.m());
      m_Truth_particlePdg_id->push_back(particle->pdg_id());
      m_Truth_particleStatus->push_back(particle->status());
      m_Truth_particleBarcode->push_back(particle->barcode());
      HepMC::GenVertex* production_vertex = particle->production_vertex();
      m_Truth_particleProduction_vertex_id->push_back(production_vertex ? production_vertex->id() : -1);
      HepMC::GenVertex* end_vertex = particle->end_vertex();
      m_Truth_particleEnd_vertex_id->push_back(end_vertex ? end_vertex->id() : -1);
      m_Truth_nParticles++;
    }

    nSubEvents++;
  }

  return StatusCode::SUCCESS;
}

void NSWPRDValAlg::deleteTruthVariables() 
{
  m_Truth_nVertices = 0;

  delete m_Truth_vertexX;
  delete m_Truth_vertexY;
  delete m_Truth_vertexZ;
  delete m_Truth_vertexT;
  delete m_Truth_vertexId;

  m_Truth_vertexX = 0;
  m_Truth_vertexY = 0;
  m_Truth_vertexZ = 0;
  m_Truth_vertexT = 0;
  m_Truth_vertexId = 0;

  m_Truth_nParticles = 0;

  delete m_Truth_particlePt;
  delete m_Truth_particleEta;
  delete m_Truth_particlePhi;
  delete m_Truth_particleE;
  delete m_Truth_particleM;
  delete m_Truth_particlePdg_id;
  delete m_Truth_particleStatus;
  delete m_Truth_particleBarcode;
  delete m_Truth_particleProduction_vertex_id;
  delete m_Truth_particleEnd_vertex_id;
  
  m_Truth_particlePt = 0;
  m_Truth_particleEta = 0;
  m_Truth_particlePhi = 0;
  m_Truth_particleE = 0;
  m_Truth_particleM = 0;
  m_Truth_particlePdg_id = 0;
  m_Truth_particleStatus = 0;
  m_Truth_particleBarcode = 0;
  m_Truth_particleProduction_vertex_id = 0;
  m_Truth_particleEnd_vertex_id = 0;
}

void NSWPRDValAlg::initializeMuEntryVariables() 
{
  m_MuEntry_nParticles = 0;
  m_MuEntry_particlePt = new std::vector<double>;
  m_MuEntry_particleEta = new std::vector<double>;
  m_MuEntry_particlePhi = new std::vector<double>;
  m_MuEntry_particlePdg_id = new std::vector<int>;
  m_MuEntry_particleBarcode = new std::vector<int>;
  m_MuEntry_positionEta = new std::vector<double>;
  m_MuEntry_positionPhi = new std::vector<double>;
  m_MuEntry_positionX = new std::vector<double>;
  m_MuEntry_positionY = new std::vector<double>;
  m_MuEntry_positionZ = new std::vector<double>;
  if(m_tree) {
    m_tree->Branch("MuEntry_Particle_n", &m_MuEntry_nParticles, "MuEntryParticle_n/i");
    m_tree->Branch("MuEntry_Particle_Pt", &m_MuEntry_particlePt);
    m_tree->Branch("MuEntry_Particle_Eta", &m_MuEntry_particleEta);
    m_tree->Branch("MuEntry_Particle_Phi", &m_MuEntry_particlePhi);
    m_tree->Branch("MuEntry_Particle_Pdg", &m_MuEntry_particlePdg_id);
    m_tree->Branch("MuEntry_Particle_Barcode", &m_MuEntry_particleBarcode);
    m_tree->Branch("MuEntry_Position_Eta", &m_MuEntry_positionEta);
    m_tree->Branch("MuEntry_Position_Phi", &m_MuEntry_positionPhi);
    m_tree->Branch("MuEntry_Position_X", &m_MuEntry_positionX);
    m_tree->Branch("MuEntry_Position_Y", &m_MuEntry_positionY);
    m_tree->Branch("MuEntry_Position_Z", &m_MuEntry_positionZ);
  }
}

void NSWPRDValAlg::clearMuEntryVariables() {
  m_MuEntry_nParticles = 0;
  m_MuEntry_particlePt->clear();
  m_MuEntry_particleEta->clear();
  m_MuEntry_particlePhi->clear();
  m_MuEntry_particlePdg_id->clear();
  m_MuEntry_particleBarcode->clear();
  m_MuEntry_positionEta->clear();
  m_MuEntry_positionPhi->clear();
  m_MuEntry_positionX->clear();
  m_MuEntry_positionY->clear();
  m_MuEntry_positionZ->clear();
}

StatusCode NSWPRDValAlg::fillMuEntryVariables() {
  clearMuEntryVariables();

  const TrackRecordCollection* trackRecordCollection = 0;
  StatusCode sc = evtStore()->retrieve(trackRecordCollection, m_MuEntry_ContainerName.c_str());
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve " << m_MuEntry_ContainerName.c_str());
    return StatusCode::SUCCESS;
  }

  TrackRecordCollection::const_iterator it   = trackRecordCollection->begin();
  TrackRecordCollection::const_iterator it_e = trackRecordCollection->end();
  for(; it!=it_e; ++it) {

    const CLHEP::Hep3Vector momentum = (*it)->GetMomentum();
    const CLHEP::Hep3Vector position = (*it)->GetPosition();
    m_MuEntry_particlePt->push_back(momentum.perp());
    m_MuEntry_particleEta->push_back(momentum.getEta());
    m_MuEntry_particlePhi->push_back(momentum.getPhi());
    m_MuEntry_particlePdg_id->push_back((*it)->GetPDGCode());
    m_MuEntry_particleBarcode->push_back((*it)->GetBarCode());
    m_MuEntry_positionEta->push_back(position.getEta());
    m_MuEntry_positionPhi->push_back(position.getPhi());
    m_MuEntry_positionX->push_back(position.x());
    m_MuEntry_positionY->push_back(position.y());
    m_MuEntry_positionZ->push_back(position.z());
    m_MuEntry_nParticles++;

  }

  return StatusCode::SUCCESS;
}

void NSWPRDValAlg::deleteMuEntryVariables() 
{
  m_MuEntry_nParticles = 0;

  delete m_MuEntry_particlePt;
  delete m_MuEntry_particleEta;
  delete m_MuEntry_particlePhi;
  delete m_MuEntry_particlePdg_id;
  delete m_MuEntry_particleBarcode;
  delete m_MuEntry_positionEta;
  delete m_MuEntry_positionPhi;
  delete m_MuEntry_positionX;
  delete m_MuEntry_positionY;
  delete m_MuEntry_positionZ;
}

void NSWPRDValAlg::initializeNSWsTGCHitVariables() 
{
  m_NSWsTGC_nSimHits = 0;
  m_NSWsTGC_trackId  = new std::vector<int>();
  m_NSWsTGC_truthEl  = new std::vector<int>();

  m_NSWsTGC_globalTime = new std::vector<double>;
  m_NSWsTGC_hitGlobalPositionX = new std::vector<double>;
  m_NSWsTGC_hitGlobalPositionY = new std::vector<double>;
  m_NSWsTGC_hitGlobalPositionZ = new std::vector<double>;
  m_NSWsTGC_hitGlobalPositionR = new std::vector<double>;
  m_NSWsTGC_hitGlobalPositionP = new std::vector<double>;
  m_NSWsTGC_hitGlobalDirectionX = new std::vector<double>;
  m_NSWsTGC_hitGlobalDirectionY = new std::vector<double>;
  m_NSWsTGC_hitGlobalDirectionZ = new std::vector<double>;

  m_NSWsTGC_hitLocalPositionX = new std::vector<double>;
  m_NSWsTGC_hitLocalPositionY = new std::vector<double>;
  m_NSWsTGC_hitLocalPositionZ = new std::vector<double>;

  m_NSWsTGC_detector_globalPositionX = new std::vector<double>;
  m_NSWsTGC_detector_globalPositionY = new std::vector<double>;
  m_NSWsTGC_detector_globalPositionZ = new std::vector<double>;
  m_NSWsTGC_detector_globalPositionR = new std::vector<double>;
  m_NSWsTGC_detector_globalPositionP = new std::vector<double>;

  m_NSWsTGC_hitToDsurfacePositionX = new std::vector<double>;
  m_NSWsTGC_hitToDsurfacePositionY = new std::vector<double>;
  m_NSWsTGC_hitToDsurfacePositionZ = new std::vector<double>;

  m_NSWsTGC_hitToRsurfacePositionX = new std::vector<double>;
  m_NSWsTGC_hitToRsurfacePositionY = new std::vector<double>;
  m_NSWsTGC_hitToRsurfacePositionZ = new std::vector<double>;

  m_NSWsTGC_FastDigitRsurfacePositionX = new std::vector<double>;
  m_NSWsTGC_FastDigitRsurfacePositionY = new std::vector<double>;


  m_NSWsTGC_particleEncoding = new std::vector<int>;
  m_NSWsTGC_kineticEnergy = new std::vector<double>;
  m_NSWsTGC_depositEnergy = new std::vector<double>;
  m_NSWsTGC_StepLength = new std::vector<double>;

  m_NSWsTGC_sim_stationName = new std::vector<std::string>;
  m_NSWsTGC_wedgeId         = new std::vector<int>;
  m_NSWsTGC_wedgeType       = new std::vector<int>;
  m_NSWsTGC_detectorNumber  = new std::vector<int>;
  m_NSWsTGC_sim_stationEta  = new std::vector<int>;
  m_NSWsTGC_sim_stationPhi  = new std::vector<int>;
  m_NSWsTGC_sim_multilayer  = new std::vector<int>;
  m_NSWsTGC_sim_layer       = new std::vector<int>;
  m_NSWsTGC_sim_side        = new std::vector<int>;
  m_NSWsTGC_stripNumber     = new std::vector<int>;
  m_NSWsTGC_wireNumber      = new std::vector<int>;

  m_NSWsTGC_off_stationName  = new std::vector<std::string>;
  m_NSWsTGC_off_stationEta   = new std::vector<int>;                                         
  m_NSWsTGC_off_stationPhi   = new std::vector<int>;
  m_NSWsTGC_off_multiplet    = new std::vector<int>;
  m_NSWsTGC_off_gas_gap      = new std::vector<int>;
  m_NSWsTGC_off_channel_type = new std::vector<int>;
  m_NSWsTGC_off_channel      = new std::vector<int>;

  if(m_tree) {
    m_tree->Branch("Hits_sTGC_n", &m_NSWsTGC_nSimHits, "Hits_sTGC_nSimHits/i");
    m_tree->Branch("Hits_sTGC_trackId", &m_NSWsTGC_trackId);
    m_tree->Branch("Hits_sTGC_truthEl", &m_NSWsTGC_truthEl);

    m_tree->Branch("Hits_sTGC_globalTime", &m_NSWsTGC_globalTime);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionX", &m_NSWsTGC_hitGlobalPositionX);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionY", &m_NSWsTGC_hitGlobalPositionY);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionZ", &m_NSWsTGC_hitGlobalPositionZ);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionR", &m_NSWsTGC_hitGlobalPositionR);
    m_tree->Branch("Hits_sTGC_hitGlobalPositionP", &m_NSWsTGC_hitGlobalPositionP);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionX", &m_NSWsTGC_hitGlobalDirectionX);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionY", &m_NSWsTGC_hitGlobalDirectionY);
    m_tree->Branch("Hits_sTGC_hitGlobalDirectionZ", &m_NSWsTGC_hitGlobalDirectionZ);

    m_tree->Branch("Hits_sTGC_hitLocalPositionX", &m_NSWsTGC_hitLocalPositionX);
    m_tree->Branch("Hits_sTGC_hitLocalPositionY", &m_NSWsTGC_hitLocalPositionY);
    m_tree->Branch("Hits_sTGC_hitLocalPositionZ", &m_NSWsTGC_hitLocalPositionZ);

    m_tree->Branch("Hits_sTGC_detector_globalPositionX", &m_NSWsTGC_detector_globalPositionX);
    m_tree->Branch("Hits_sTGC_detector_globalPositionY", &m_NSWsTGC_detector_globalPositionY);
    m_tree->Branch("Hits_sTGC_detector_globalPositionZ", &m_NSWsTGC_detector_globalPositionZ);
    m_tree->Branch("Hits_sTGC_detector_globalPositionR", &m_NSWsTGC_detector_globalPositionR);
    m_tree->Branch("Hits_sTGC_detector_globalPositionP", &m_NSWsTGC_detector_globalPositionP);

    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionX", &m_NSWsTGC_hitToDsurfacePositionX);
    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionY", &m_NSWsTGC_hitToDsurfacePositionY);
    m_tree->Branch("Hits_sTGC_hitToDsurfacePositionZ", &m_NSWsTGC_hitToDsurfacePositionZ);

    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionX", &m_NSWsTGC_hitToRsurfacePositionX);
    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionY", &m_NSWsTGC_hitToRsurfacePositionY);
    m_tree->Branch("Hits_sTGC_hitToRsurfacePositionZ", &m_NSWsTGC_hitToRsurfacePositionZ);

    m_tree->Branch("Hits_sTGC_FastDigitRsurfacePositionX", &m_NSWsTGC_FastDigitRsurfacePositionX);
    m_tree->Branch("Hits_sTGC_FastDigitRsurfacePositionY", &m_NSWsTGC_FastDigitRsurfacePositionY);


    m_tree->Branch("Hits_sTGC_particleEncoding", &m_NSWsTGC_particleEncoding);
    m_tree->Branch("Hits_sTGC_kineticEnergy", &m_NSWsTGC_kineticEnergy);
    m_tree->Branch("Hits_sTGC_depositEnergy", &m_NSWsTGC_depositEnergy);
    m_tree->Branch("Hits_sTGC_StepLength", &m_NSWsTGC_StepLength);

    m_tree->Branch("Hits_sTGC_sim_stationName", &m_NSWsTGC_sim_stationName);
    m_tree->Branch("Hits_sTGC_wedgeId", &m_NSWsTGC_wedgeId);
    m_tree->Branch("Hits_sTGC_wedgeType", &m_NSWsTGC_wedgeType);
    m_tree->Branch("Hits_sTGC_detectorNumber", &m_NSWsTGC_detectorNumber);
    m_tree->Branch("Hits_sTGC_sim_stationEta", &m_NSWsTGC_sim_stationEta);
    m_tree->Branch("Hits_sTGC_sim_stationPhi", &m_NSWsTGC_sim_stationPhi);
    m_tree->Branch("Hits_sTGC_sim_multilayer", &m_NSWsTGC_sim_multilayer);
    m_tree->Branch("Hits_sTGC_sim_layer", &m_NSWsTGC_sim_layer);
    m_tree->Branch("Hits_sTGC_sim_side", &m_NSWsTGC_sim_side);
    m_tree->Branch("Hits_sTGC_stripNumber", &m_NSWsTGC_stripNumber);
    m_tree->Branch("Hits_sTGC_wireNumber", &m_NSWsTGC_wireNumber);

    m_tree->Branch("Hits_sTGC_off_stationName", &m_NSWsTGC_off_stationName);
    m_tree->Branch("Hits_sTGC_off_stationEta", &m_NSWsTGC_off_stationEta);                  
    m_tree->Branch("Hits_sTGC_off_stationPhi", &m_NSWsTGC_off_stationPhi);
    m_tree->Branch("Hits_sTGC_off_multiplet", &m_NSWsTGC_off_multiplet);
    m_tree->Branch("Hits_sTGC_off_gas_gap", &m_NSWsTGC_off_gas_gap);
    m_tree->Branch("Hits_sTGC_off_channel_type", &m_NSWsTGC_off_channel_type);
    m_tree->Branch("Hits_sTGC_off_channel", &m_NSWsTGC_off_channel);  
  }
}

void NSWPRDValAlg::initializeNSWsTGCFastDigitVariables()
{
    m_NSWsTGC_nFastDigits = 0;
    m_NSWsTGC_fdg_stationName   = new std::vector<std::string>();
    m_NSWsTGC_fdg_stationEta    = new std::vector<int>();
    m_NSWsTGC_fdg_stationPhi    = new std::vector<int>();
    m_NSWsTGC_fdg_multiplet     = new std::vector<int>();
    m_NSWsTGC_fdg_gas_gap       = new std::vector<int>();
    m_NSWsTGC_fdg_channel_type  = new std::vector<int>();
    m_NSWsTGC_fdg_channel       = new std::vector<int>();

    m_NSWsTGC_fdg_globalPosX    = new std::vector<double>();
    m_NSWsTGC_fdg_globalPosY    = new std::vector<double>();
    m_NSWsTGC_fdg_globalPosZ    = new std::vector<double>();

    m_NSWsTGC_fdg_localPosX     = new std::vector<double>();
    m_NSWsTGC_fdg_localPosY     = new std::vector<double>();
    m_NSWsTGC_fdg_covMatrix_1_1 = new std::vector<double>();
    m_NSWsTGC_fdg_covMatrix_2_2 = new std::vector<double>();

    m_NSWsTGC_fdg_truth_barcode       = new std::vector<int>();
    m_NSWsTGC_fdg_truth_hitOnSurfaceX = new std::vector<double>();
    m_NSWsTGC_fdg_truth_hitOnSurfaceY = new std::vector<double>();



    m_tree->Branch("FastDigit_sTGC", &m_NSWsTGC_nFastDigits, "FastDigits_sTGC_n/i");
    m_tree->Branch("FastDigit_sTGC_stationName", &m_NSWsTGC_fdg_stationName);
    m_tree->Branch("FastDigit_sTGC_stationEta",  &m_NSWsTGC_fdg_stationEta);
    m_tree->Branch("FastDigit_sTGC_stationPhi",  &m_NSWsTGC_fdg_stationPhi);
    m_tree->Branch("FastDigit_sTGC_multiplet",   &m_NSWsTGC_fdg_multiplet);
    m_tree->Branch("FastDigit_sTGC_gas_gap",     &m_NSWsTGC_fdg_gas_gap);
    m_tree->Branch("FastDigit_sTGC_channel_type",&m_NSWsTGC_fdg_channel_type);
    m_tree->Branch("FastDigit_sTGC_channel",     &m_NSWsTGC_fdg_channel);

    m_tree->Branch("FastDigit_sTGC_globalPosX",  &m_NSWsTGC_fdg_globalPosX);
    m_tree->Branch("FastDigit_sTGC_globalPosY",  &m_NSWsTGC_fdg_globalPosY);
    m_tree->Branch("FastDigit_sTGC_globalPosZ",  &m_NSWsTGC_fdg_globalPosZ);

    m_tree->Branch("FastDigit_sTGC_localPosX",   &m_NSWsTGC_fdg_localPosX);
    m_tree->Branch("FastDigit_sTGC_localPosY",   &m_NSWsTGC_fdg_localPosY);
    m_tree->Branch("FastDigit_sTGC_covMatrix_1_1", &m_NSWsTGC_fdg_covMatrix_1_1);
    m_tree->Branch("FastDigit_sTGC_covMatrix_2_2", &m_NSWsTGC_fdg_covMatrix_2_2);


    m_tree->Branch("FastDigit_sTGC_truth_barcode", &m_NSWsTGC_fdg_truth_barcode);
    m_tree->Branch("FastDigit_sTGC_truth_hitOnSurfaceX", &m_NSWsTGC_fdg_truth_hitOnSurfaceX);
    m_tree->Branch("FastDigit_sTGC_truth_hitOnSurfaceY", &m_NSWsTGC_fdg_truth_hitOnSurfaceY);
}

void NSWPRDValAlg::initializeNSWsTGCDigitVariables()
{
}


void NSWPRDValAlg::initializeNSWMMHitVariables() 
{
  m_NSWMM_nSimHits = 0;
  m_NSWMM_trackId  = new std::vector<int>;
  m_NSWMM_truthEl  = new std::vector<int>;
  m_NSWMM_globalTime = new std::vector<double>;
  m_NSWMM_hitGlobalPositionX = new std::vector<double>;
  m_NSWMM_hitGlobalPositionY = new std::vector<double>;
  m_NSWMM_hitGlobalPositionZ = new std::vector<double>;
  m_NSWMM_hitGlobalPositionR = new std::vector<double>;
  m_NSWMM_hitGlobalPositionP = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionX = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionY = new std::vector<double>;
  m_NSWMM_hitGlobalDirectionZ = new std::vector<double>;

  m_NSWMM_hitLocalPositionX = new std::vector<double>;
  m_NSWMM_hitLocalPositionY = new std::vector<double>;
  m_NSWMM_hitLocalPositionZ = new std::vector<double>;

  m_NSWMM_detector_globalPositionX = new std::vector<double>;
  m_NSWMM_detector_globalPositionY = new std::vector<double>;
  m_NSWMM_detector_globalPositionZ = new std::vector<double>;
  m_NSWMM_detector_globalPositionR = new std::vector<double>;
  m_NSWMM_detector_globalPositionP = new std::vector<double>;

  m_NSWMM_hitToDsurfacePositionX = new std::vector<double>;
  m_NSWMM_hitToDsurfacePositionY = new std::vector<double>;
  m_NSWMM_hitToDsurfacePositionZ = new std::vector<double>;

  m_NSWMM_hitToRsurfacePositionX = new std::vector<double>;
  m_NSWMM_hitToRsurfacePositionY = new std::vector<double>;
  m_NSWMM_hitToRsurfacePositionZ = new std::vector<double>;

  m_NSWMM_FastDigitRsurfacePositionX = new std::vector<double>;
  m_NSWMM_FastDigitRsurfacePositionY = new std::vector<double>;


  m_NSWMM_particleEncoding = new std::vector<int>;
  m_NSWMM_kineticEnergy = new std::vector<double>;
  m_NSWMM_depositEnergy = new std::vector<double>;
  m_NSWMM_StepLength = new std::vector<double>;


  m_NSWMM_sim_stationName = new std::vector<std::string>;
  m_NSWMM_sim_stationEta  = new std::vector<int>;
  m_NSWMM_sim_stationPhi  = new std::vector<int>;
  m_NSWMM_sim_multilayer  = new std::vector<int>;
  m_NSWMM_sim_layer       = new std::vector<int>;
  m_NSWMM_sim_side        = new std::vector<int>;

  m_NSWMM_off_stationName = new std::vector<std::string>;
  m_NSWMM_off_stationEta  = new std::vector<int>;
  m_NSWMM_off_stationPhi  = new std::vector<int>;
  m_NSWMM_off_multiplet   = new std::vector<int>;
  m_NSWMM_off_gas_gap     = new std::vector<int>;
  m_NSWMM_off_channel     = new std::vector<int>;



  if(m_tree) {
    m_tree->Branch("Hits_MM_n", &m_NSWMM_nSimHits, "Hits_MM_n/i");
    m_tree->Branch("Hits_MM_trackId", &m_NSWMM_trackId);
    m_tree->Branch("Hits_MM_truthEl", &m_NSWMM_truthEl);
    m_tree->Branch("Hits_MM_globalTime", &m_NSWMM_globalTime);
    m_tree->Branch("Hits_MM_hitGlobalPositionX", &m_NSWMM_hitGlobalPositionX);
    m_tree->Branch("Hits_MM_hitGlobalPositionY", &m_NSWMM_hitGlobalPositionY);
    m_tree->Branch("Hits_MM_hitGlobalPositionZ", &m_NSWMM_hitGlobalPositionZ);
    m_tree->Branch("Hits_MM_hitGlobalPositionR", &m_NSWMM_hitGlobalPositionR);
    m_tree->Branch("Hits_MM_hitGlobalPositionP", &m_NSWMM_hitGlobalPositionP);
    m_tree->Branch("Hits_MM_hitGlobalDirectionX", &m_NSWMM_hitGlobalDirectionX);
    m_tree->Branch("Hits_MM_hitGlobalDirectionY", &m_NSWMM_hitGlobalDirectionY);
    m_tree->Branch("Hits_MM_hitGlobalDirectionZ", &m_NSWMM_hitGlobalDirectionZ);

    m_tree->Branch("Hits_MM_hitLocalPositionX", &m_NSWMM_hitLocalPositionX);
    m_tree->Branch("Hits_MM_hitLocalPositionY", &m_NSWMM_hitLocalPositionY);
    m_tree->Branch("Hits_MM_hitLocalPositionZ", &m_NSWMM_hitLocalPositionZ);

    m_tree->Branch("Hits_MM_detector_globalPositionX", &m_NSWMM_detector_globalPositionX);
    m_tree->Branch("Hits_MM_detector_globalPositionY", &m_NSWMM_detector_globalPositionY);
    m_tree->Branch("Hits_MM_detector_globalPositionZ", &m_NSWMM_detector_globalPositionZ);
    m_tree->Branch("Hits_MM_detector_globalPositionR", &m_NSWMM_detector_globalPositionR);
    m_tree->Branch("Hits_MM_detector_globalPositionP", &m_NSWMM_detector_globalPositionP);

    m_tree->Branch("Hits_MM_hitToDsurfacePositionX", &m_NSWMM_hitToDsurfacePositionX);
    m_tree->Branch("Hits_MM_hitToDsurfacePositionY", &m_NSWMM_hitToDsurfacePositionY);
    m_tree->Branch("Hits_MM_hitToDsurfacePositionZ", &m_NSWMM_hitToDsurfacePositionZ);

    m_tree->Branch("Hits_MM_hitToRsurfacePositionX", &m_NSWMM_hitToRsurfacePositionX);
    m_tree->Branch("Hits_MM_hitToRsurfacePositionY", &m_NSWMM_hitToRsurfacePositionY);
    m_tree->Branch("Hits_MM_hitToRsurfacePositionZ", &m_NSWMM_hitToRsurfacePositionZ);

    m_tree->Branch("Hits_MM_FastDigitRsurfacePositionX", &m_NSWMM_FastDigitRsurfacePositionX);
    m_tree->Branch("Hits_MM_FastDigitRsurfacePositionY", &m_NSWMM_FastDigitRsurfacePositionY);
     

    m_tree->Branch("Hits_MM_particleEncoding", &m_NSWMM_particleEncoding);
    m_tree->Branch("Hits_MM_kineticEnergy", &m_NSWMM_kineticEnergy);
    m_tree->Branch("Hits_MM_depositEnergy", &m_NSWMM_depositEnergy);
    m_tree->Branch("Hits_MM_StepLength", &m_NSWMM_StepLength);


    m_tree->Branch("Hits_MM_sim_stationName", &m_NSWMM_sim_stationName);
    m_tree->Branch("Hits_MM_sim_stationEta", &m_NSWMM_sim_stationEta);
    m_tree->Branch("Hits_MM_sim_stationPhi", &m_NSWMM_sim_stationPhi);
    m_tree->Branch("Hits_MM_sim_multilayer", &m_NSWMM_sim_multilayer);
    m_tree->Branch("Hits_MM_sim_layer", &m_NSWMM_sim_layer);
    m_tree->Branch("Hits_MM_sim_side", &m_NSWMM_sim_side);

    m_tree->Branch("Hits_MM_off_stationName", &m_NSWMM_off_stationName);
    m_tree->Branch("Hits_MM_off_stationEta", &m_NSWMM_off_stationEta);
    m_tree->Branch("Hits_MM_off_stationPhi", &m_NSWMM_off_stationPhi);
    m_tree->Branch("Hits_MM_off_multiplet", &m_NSWMM_off_multiplet);
    m_tree->Branch("Hits_MM_off_gas_gap", &m_NSWMM_off_gas_gap);
    m_tree->Branch("Hits_MM_off_channel", &m_NSWMM_off_channel);

  }
}

void NSWPRDValAlg::initializeNSWMMFastDigitVariables()
{
  m_NSWMM_nFastDigits = 0;
  m_NSWMM_fdg_stationName = new std::vector<std::string>();
  m_NSWMM_fdg_stationEta = new std::vector<int>();
  m_NSWMM_fdg_stationPhi = new std::vector<int>();
  m_NSWMM_fdg_multiplet  = new std::vector<int>();
  m_NSWMM_fdg_gas_gap    = new std::vector<int>();
  m_NSWMM_fdg_channel    = new std::vector<int>();

  m_NSWMM_fdg_globalPosX = new std::vector<double>();
  m_NSWMM_fdg_globalPosY = new std::vector<double>();
  m_NSWMM_fdg_globalPosZ = new std::vector<double>();

  m_NSWMM_fdg_localPosX = new std::vector<double>();
  m_NSWMM_fdg_localPosY = new std::vector<double>();
  m_NSWMM_fdg_covMatrix_1_1 = new std::vector<double>();

  m_NSWMM_fdg_truth_barcode        = new std::vector<int>();
  m_NSWMM_fdg_truth_hitOnSurfaceX  = new std::vector<double>();
  m_NSWMM_fdg_truth_hitOnSurfaceY  = new std::vector<double>();


  m_tree->Branch("FastDigit_MM", &m_NSWMM_nFastDigits, "FastDigits_MM_n/i");
  m_tree->Branch("FastDigit_MM_stationName",   &m_NSWMM_fdg_stationName);
  m_tree->Branch("FastDigit_MM_stationEta",    &m_NSWMM_fdg_stationEta); 
  m_tree->Branch("FastDigit_MM_stationPhi",    &m_NSWMM_fdg_stationPhi);
  m_tree->Branch("FastDigit_MM_multiplet",     &m_NSWMM_fdg_multiplet);
  m_tree->Branch("FastDigit_MM_gas_gap",       &m_NSWMM_fdg_gas_gap);
  m_tree->Branch("FastDigit_MM_channel",       &m_NSWMM_fdg_channel);

  m_tree->Branch("FastDigit_MM_globalPosX",    &m_NSWMM_fdg_globalPosX);
  m_tree->Branch("FastDigit_MM_globalPosY",    &m_NSWMM_fdg_globalPosY);
  m_tree->Branch("FastDigit_MM_globalPosZ",    &m_NSWMM_fdg_globalPosZ);

  m_tree->Branch("FastDigit_MM_localPosX",     &m_NSWMM_fdg_localPosX);
  m_tree->Branch("FastDigit_MM_localPosY",     &m_NSWMM_fdg_localPosY);
  m_tree->Branch("FastDigit_MM_covMatrix_1_1", &m_NSWMM_fdg_covMatrix_1_1);

  m_tree->Branch("FastDigit_MM_truth_barcode",       &m_NSWMM_fdg_truth_barcode);
  m_tree->Branch("FastDigit_MM_truth_hitOnSurfaceX", &m_NSWMM_fdg_truth_hitOnSurfaceX);
  m_tree->Branch("FastDigit_MM_truth_hitOnSurfaceY", &m_NSWMM_fdg_truth_hitOnSurfaceY);
}

void NSWPRDValAlg::initializeNSWMMDigitVariables()
{
  m_NSWMM_nDigits = 0;
  m_NSWMM_dig_stationName = new std::vector<std::string>();
  m_NSWMM_dig_stationEta  = new std::vector<int>();
  m_NSWMM_dig_stationPhi  = new std::vector<int>();
  m_NSWMM_dig_multiplet   = new std::vector<int>();
  m_NSWMM_dig_gas_gap     = new std::vector<int>();
  m_NSWMM_dig_channel     = new std::vector<int>();

  m_NSWMM_dig_time          = new std::vector< std::vector<float> >();
  m_NSWMM_dig_charge        = new std::vector< std::vector<float> >();
  m_NSWMM_dig_stripPosition = new std::vector< std::vector<int> >();
  m_NSWMM_dig_stripLposX    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_stripLposY    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_stripGposX    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_stripGposY    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_stripGposZ    = new std::vector< std::vector<double> >();

  m_NSWMM_dig_sr_time          = new std::vector< std::vector<float> >();
  m_NSWMM_dig_sr_charge        = new std::vector< std::vector<float> >();
  m_NSWMM_dig_sr_stripPosition = new std::vector< std::vector<int> >();
  m_NSWMM_dig_sr_stripLposX    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_sr_stripLposY    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_sr_stripGposX    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_sr_stripGposY    = new std::vector< std::vector<double> >();
  m_NSWMM_dig_sr_stripGposZ    = new std::vector< std::vector<double> >();


  m_NSWMM_dig_truth_barcode    = new std::vector<int>();
  m_NSWMM_dig_truth_localPosX  = new std::vector<double>();
  m_NSWMM_dig_truth_localPosY  = new std::vector<double>();
  m_NSWMM_dig_truth_XZ_angle   = new std::vector<float>();
  m_NSWMM_dig_truth_globalPosX = new std::vector<double>();
  m_NSWMM_dig_truth_globalPosY = new std::vector<double>();
  m_NSWMM_dig_truth_globalPosZ = new std::vector<double>();


  m_NSWMM_dig_stripForTrigger     = new std::vector<int>();
  m_NSWMM_dig_stripTimeForTrigger = new std::vector<float>();

  m_tree->Branch("Digits_MM",    &m_NSWMM_nDigits, "Digits_MM_n/i");
  m_tree->Branch("Digits_MM_stationName", &m_NSWMM_dig_stationName);
  m_tree->Branch("Digits_MM_stationEta",  &m_NSWMM_dig_stationEta);
  m_tree->Branch("Digits_MM_stationPhi",  &m_NSWMM_dig_stationPhi);
  m_tree->Branch("Digits_MM_multiplet",   &m_NSWMM_dig_multiplet);
  m_tree->Branch("Digits_MM_gas_gap",     &m_NSWMM_dig_gas_gap);
  m_tree->Branch("Digits_MM_channel",     &m_NSWMM_dig_channel);

  m_tree->Branch("Digits_MM_time",                        "std::vector< std::vector<float> >",  &m_NSWMM_dig_time);
  m_tree->Branch("Digits_MM_charge",                      "std::vector< std::vector<float> >",  &m_NSWMM_dig_charge);
  m_tree->Branch("Digits_MM_stripPosition",               "std::vector< std::vector<int> >",    &m_NSWMM_dig_stripPosition);
  m_tree->Branch("Digits_MM_stripLposX",                  "std::vector< std::vector<double> >", &m_NSWMM_dig_stripLposX);
  m_tree->Branch("Digits_MM_stripLposY",                  "std::vector< std::vector<double> >", &m_NSWMM_dig_stripLposY);
  m_tree->Branch("Digits_MM_stripGposX",                  "std::vector< std::vector<double> >", &m_NSWMM_dig_stripGposX);
  m_tree->Branch("Digits_MM_stripGposY",                  "std::vector< std::vector<double> >", &m_NSWMM_dig_stripGposY);
  m_tree->Branch("Digits_MM_stripGposZ",                  "std::vector< std::vector<double> >", &m_NSWMM_dig_stripGposZ);

  m_tree->Branch("Digits_MM_stripResponse_time",          "std::vector< std::vector<float> >",  &m_NSWMM_dig_sr_time);
  m_tree->Branch("Digits_MM_stripResponse_charge",        "std::vector< std::vector<float> >",  &m_NSWMM_dig_sr_charge);
  m_tree->Branch("Digits_MM_stripResponse_stripPosition", "std::vector< std::vector<int> >",    &m_NSWMM_dig_sr_stripPosition);
  m_tree->Branch("Digits_MM_stripResponse_stripLposX",    "std::vector< std::vector<double> >", &m_NSWMM_dig_sr_stripLposX);
  m_tree->Branch("Digits_MM_stripResponse_stripLposY",    "std::vector< std::vector<double> >", &m_NSWMM_dig_sr_stripLposY);
  m_tree->Branch("Digits_MM_stripresponse_stripGposX",    "std::vector< std::vector<double> >", &m_NSWMM_dig_sr_stripGposX);
  m_tree->Branch("Digits_MM_stripResponse_stripGposY",    "std::vector< std::vector<double> >", &m_NSWMM_dig_sr_stripGposY);
  m_tree->Branch("Digits_MM_stripResponse_stripGposZ",    "std::vector< std::vector<double> >", &m_NSWMM_dig_sr_stripGposZ);

  m_tree->Branch("Digits_MM_truth_barcode",    "std::vector< int >",    &m_NSWMM_dig_truth_barcode);
  m_tree->Branch("Digits_MM_truth_localPosX",  "std::vector< double >", &m_NSWMM_dig_truth_localPosX);
  m_tree->Branch("Digits_MM_truth_localPosY",  "std::vector< double >", &m_NSWMM_dig_truth_localPosY);
  m_tree->Branch("Digits_MM_truth_XZ_angle",   "std::vector< float >",  &m_NSWMM_dig_truth_XZ_angle);
  m_tree->Branch("Digits_MM_truth_globalPosX", "std::vector< double >", &m_NSWMM_dig_truth_globalPosX);
  m_tree->Branch("Digits_MM_truth_globalPosY", "std::vector< double >", &m_NSWMM_dig_truth_globalPosY);
  m_tree->Branch("Digits_MM_truth_globalPosZ", "std::vector< double >", &m_NSWMM_dig_truth_globalPosZ);


  m_tree->Branch("Digits_MM_stripForTrigger",     &m_NSWMM_dig_stripForTrigger);
  m_tree->Branch("Digits_MM_stripTimeForTrigger", &m_NSWMM_dig_stripTimeForTrigger);

}


void NSWPRDValAlg::clearNSWsTGCHitVariables() 
{
  m_NSWsTGC_nSimHits = 0;
  m_NSWsTGC_trackId->clear();
  m_NSWsTGC_truthEl->clear();

  m_NSWsTGC_globalTime->clear();
  m_NSWsTGC_hitGlobalPositionX->clear();
  m_NSWsTGC_hitGlobalPositionY->clear();
  m_NSWsTGC_hitGlobalPositionZ->clear();
  m_NSWsTGC_hitGlobalPositionR->clear();
  m_NSWsTGC_hitGlobalPositionP->clear();
  m_NSWsTGC_hitGlobalDirectionX->clear();
  m_NSWsTGC_hitGlobalDirectionY->clear();
  m_NSWsTGC_hitGlobalDirectionZ->clear();

  m_NSWsTGC_hitLocalPositionX->clear();
  m_NSWsTGC_hitLocalPositionY->clear();
  m_NSWsTGC_hitLocalPositionZ->clear();

  m_NSWsTGC_detector_globalPositionX->clear();
  m_NSWsTGC_detector_globalPositionY->clear();
  m_NSWsTGC_detector_globalPositionZ->clear();
  m_NSWsTGC_detector_globalPositionR->clear();
  m_NSWsTGC_detector_globalPositionP->clear();

  m_NSWsTGC_hitToDsurfacePositionX->clear();
  m_NSWsTGC_hitToDsurfacePositionY->clear();
  m_NSWsTGC_hitToDsurfacePositionZ->clear();

  m_NSWsTGC_hitToRsurfacePositionX->clear();
  m_NSWsTGC_hitToRsurfacePositionY->clear();
  m_NSWsTGC_hitToRsurfacePositionZ->clear();

  m_NSWsTGC_FastDigitRsurfacePositionX->clear();
  m_NSWsTGC_FastDigitRsurfacePositionY->clear();


  m_NSWsTGC_particleEncoding->clear();
  m_NSWsTGC_kineticEnergy->clear();
  m_NSWsTGC_depositEnergy->clear();
  m_NSWsTGC_StepLength->clear();


  m_NSWsTGC_sim_stationName->clear();
  m_NSWsTGC_wedgeId->clear();
  m_NSWsTGC_wedgeType->clear();
  m_NSWsTGC_detectorNumber->clear();
  m_NSWsTGC_sim_stationEta->clear();
  m_NSWsTGC_sim_stationPhi->clear();
  m_NSWsTGC_sim_multilayer->clear();
  m_NSWsTGC_sim_layer->clear();
  m_NSWsTGC_sim_side->clear();
  m_NSWsTGC_stripNumber->clear();
  m_NSWsTGC_wireNumber->clear();

  m_NSWsTGC_off_stationName->clear();
  m_NSWsTGC_off_stationEta->clear();
  m_NSWsTGC_off_stationPhi->clear();
  m_NSWsTGC_off_multiplet->clear();
  m_NSWsTGC_off_gas_gap->clear();
  m_NSWsTGC_off_channel_type->clear();
  m_NSWsTGC_off_channel->clear();
}

void NSWPRDValAlg::clearNSWsTGCFastDigitVariables()  
{
  m_NSWsTGC_nFastDigits = 0;
  m_NSWsTGC_fdg_stationName->clear();
  m_NSWsTGC_fdg_stationEta ->clear();
  m_NSWsTGC_fdg_stationPhi ->clear();
  m_NSWsTGC_fdg_multiplet  ->clear();
  m_NSWsTGC_fdg_gas_gap    ->clear();
  m_NSWsTGC_fdg_channel_type->clear();
  m_NSWsTGC_fdg_channel    ->clear();

  m_NSWsTGC_fdg_globalPosX ->clear();
  m_NSWsTGC_fdg_globalPosY ->clear();
  m_NSWsTGC_fdg_globalPosZ ->clear();

  m_NSWsTGC_fdg_localPosX  ->clear();
  m_NSWsTGC_fdg_localPosY  ->clear();
  m_NSWsTGC_fdg_covMatrix_1_1 ->clear();
  m_NSWsTGC_fdg_covMatrix_2_2 ->clear();

  m_NSWsTGC_fdg_truth_barcode->clear();
  m_NSWsTGC_fdg_truth_hitOnSurfaceX->clear();
  m_NSWsTGC_fdg_truth_hitOnSurfaceY->clear();
}

void NSWPRDValAlg::clearNSWsTGCDigitVariables()  
{
}

void NSWPRDValAlg::clearNSWMMHitVariables() 
{
  m_NSWMM_nSimHits = 0;
  m_NSWMM_trackId->clear();
  m_NSWMM_truthEl->clear();
  m_NSWMM_globalTime->clear();
  m_NSWMM_hitGlobalPositionX->clear();
  m_NSWMM_hitGlobalPositionY->clear();
  m_NSWMM_hitGlobalPositionZ->clear();
  m_NSWMM_hitGlobalPositionR->clear();
  m_NSWMM_hitGlobalPositionP->clear();
  m_NSWMM_hitGlobalDirectionX->clear();
  m_NSWMM_hitGlobalDirectionY->clear();
  m_NSWMM_hitGlobalDirectionZ->clear();

  m_NSWMM_hitLocalPositionX->clear();
  m_NSWMM_hitLocalPositionY->clear();
  m_NSWMM_hitLocalPositionZ->clear();

  m_NSWMM_detector_globalPositionX->clear();
  m_NSWMM_detector_globalPositionY->clear();
  m_NSWMM_detector_globalPositionZ->clear();
  m_NSWMM_detector_globalPositionR->clear();
  m_NSWMM_detector_globalPositionP->clear();

  m_NSWMM_hitToDsurfacePositionX->clear();
  m_NSWMM_hitToDsurfacePositionY->clear();
  m_NSWMM_hitToDsurfacePositionZ->clear();

  m_NSWMM_hitToRsurfacePositionX->clear();
  m_NSWMM_hitToRsurfacePositionY->clear();
  m_NSWMM_hitToRsurfacePositionZ->clear();

  m_NSWMM_FastDigitRsurfacePositionX->clear();
  m_NSWMM_FastDigitRsurfacePositionY->clear();


  m_NSWMM_particleEncoding->clear();
  m_NSWMM_kineticEnergy->clear();
  m_NSWMM_depositEnergy->clear();
  m_NSWMM_StepLength->clear();


  m_NSWMM_sim_stationName->clear();
  m_NSWMM_sim_stationEta->clear();
  m_NSWMM_sim_stationPhi->clear();
  m_NSWMM_sim_multilayer->clear();
  m_NSWMM_sim_layer->clear();
  m_NSWMM_sim_side->clear();

  m_NSWMM_off_stationName->clear();
  m_NSWMM_off_stationEta->clear();
  m_NSWMM_off_stationPhi->clear();
  m_NSWMM_off_multiplet->clear();
  m_NSWMM_off_gas_gap->clear();
  m_NSWMM_off_channel->clear();
}

void NSWPRDValAlg::clearNSWMMFastDigitVariables()
{
  m_NSWMM_nFastDigits = 0;
  m_NSWMM_fdg_stationName->clear();
  m_NSWMM_fdg_stationEta ->clear();
  m_NSWMM_fdg_stationPhi ->clear();
  m_NSWMM_fdg_multiplet  ->clear();
  m_NSWMM_fdg_gas_gap    ->clear();
  m_NSWMM_fdg_channel    ->clear();

  m_NSWMM_fdg_globalPosX ->clear();
  m_NSWMM_fdg_globalPosY ->clear();
  m_NSWMM_fdg_globalPosZ ->clear();

  m_NSWMM_fdg_localPosX  ->clear();
  m_NSWMM_fdg_localPosY  ->clear();
  m_NSWMM_fdg_covMatrix_1_1 ->clear();

  m_NSWMM_fdg_truth_barcode->clear();
  m_NSWMM_fdg_truth_hitOnSurfaceX->clear();
  m_NSWMM_fdg_truth_hitOnSurfaceY->clear();

}

void NSWPRDValAlg::clearNSWMMDigitVariables()
{
  m_NSWMM_nDigits = 0;
  m_NSWMM_dig_stationName->clear();
  m_NSWMM_dig_stationEta->clear();
  m_NSWMM_dig_stationPhi->clear();
  m_NSWMM_dig_multiplet->clear();
  m_NSWMM_dig_gas_gap->clear();
  m_NSWMM_dig_channel->clear();
  
  m_NSWMM_dig_time->clear();
  m_NSWMM_dig_charge->clear();
  m_NSWMM_dig_stripPosition->clear();
  m_NSWMM_dig_stripLposX->clear();
  m_NSWMM_dig_stripLposY->clear();
  m_NSWMM_dig_stripGposX->clear();
  m_NSWMM_dig_stripGposY->clear();
  m_NSWMM_dig_stripGposZ->clear();

  m_NSWMM_dig_sr_time->clear();
  m_NSWMM_dig_sr_charge->clear();
  m_NSWMM_dig_sr_stripPosition->clear();
  m_NSWMM_dig_sr_stripLposX->clear();
  m_NSWMM_dig_sr_stripLposY->clear();
  m_NSWMM_dig_sr_stripGposX->clear();
  m_NSWMM_dig_sr_stripGposY->clear();
  m_NSWMM_dig_sr_stripGposZ->clear();


  m_NSWMM_dig_truth_barcode->clear();
  m_NSWMM_dig_truth_localPosX->clear();
  m_NSWMM_dig_truth_localPosY->clear();
  m_NSWMM_dig_truth_XZ_angle->clear();
  m_NSWMM_dig_truth_globalPosX->clear();
  m_NSWMM_dig_truth_globalPosY->clear();
  m_NSWMM_dig_truth_globalPosZ->clear();


  m_NSWMM_dig_stripForTrigger->clear();
  m_NSWMM_dig_stripTimeForTrigger->clear();
}


StatusCode NSWPRDValAlg::fillNSWsTGCHitVariables() 
{
  ATH_MSG_INFO("do fillNSWsTGCHitVariables()");

  clearNSWsTGCHitVariables();

  const GenericMuonSimHitCollection *nswContainer = 0;
  StatusCode sc = evtStore()->retrieve(nswContainer, m_NSWsTGC_ContainerName.c_str());
  if(!sc.isSuccess()) 
    {
      ATH_MSG_WARNING("Could not retrieve " << m_NSWsTGC_ContainerName.c_str());
      return StatusCode::SUCCESS;
    }

  ATH_MSG_DEBUG("ReadMuonSimHits: Retrieved " << nswContainer->size() << " sTGC hits!" ); 

  // Get sTGC Helper
  sTgcHitIdHelper* hitHelper = sTgcHitIdHelper::GetHelper();
  sTgcSimIdToOfflineId simToOffline (*m_sTgcIdHelper);
  
  GenericMuonSimHitCollection::const_iterator it   = nswContainer->begin();
  GenericMuonSimHitCollection::const_iterator it_e = nswContainer->end();
  if(nswContainer->size()==0) msg(MSG::WARNING) <<" GenericMuonSimHit empty " << endreq;
  for(; it!=it_e; ++it) {
    const GenericMuonSimHit hit = *it;
    if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 

    // connect the hit with the MC truth
    int barcode = hit.particleLink().barcode();
    m_NSWsTGC_trackId->push_back(barcode);
    for (unsigned int tr=0;tr<m_Truth_particleBarcode->size();tr++) {
        if (barcode==m_Truth_particleBarcode->at(tr)) {
            m_NSWsTGC_truthEl->push_back(tr);
        }
    }


    m_NSWsTGC_globalTime->push_back(hit.globalTime());
    const  Amg::Vector3D globalPosition = hit.globalPosition();
    m_NSWsTGC_hitGlobalPositionX->push_back(globalPosition.x());
    m_NSWsTGC_hitGlobalPositionY->push_back(globalPosition.y());
    m_NSWsTGC_hitGlobalPositionZ->push_back(globalPosition.z());
    m_NSWsTGC_hitGlobalPositionR->push_back(globalPosition.perp());
    m_NSWsTGC_hitGlobalPositionP->push_back(globalPosition.phi());
    const  Amg::Vector3D globalDirection = hit.globalDirection();
    m_NSWsTGC_hitGlobalDirectionX->push_back(globalDirection.x());
    m_NSWsTGC_hitGlobalDirectionY->push_back(globalDirection.y());
    m_NSWsTGC_hitGlobalDirectionZ->push_back(globalDirection.z());

    const  Amg::Vector3D localPosition = hit.localPosition();
    m_NSWsTGC_hitLocalPositionX->push_back(localPosition.x());
    m_NSWsTGC_hitLocalPositionY->push_back(localPosition.y());
    m_NSWsTGC_hitLocalPositionZ->push_back(localPosition.z());

    m_NSWsTGC_particleEncoding->push_back(hit.particleEncoding());
    m_NSWsTGC_kineticEnergy->push_back(hit.kineticEnergy());
    m_NSWsTGC_depositEnergy->push_back(hit.depositEnergy());
    m_NSWsTGC_StepLength->push_back(hit.StepLength());

    // Read information about the sTGC hits; make sanity checks and printout
    int simId = hit.GenericId(); 
    std::string sim_stationName = hitHelper->GetStationName(simId);
    int sim_stationEta          = hitHelper->GetZSector(simId);
    int sim_stationPhi          = hitHelper->GetPhiSector(simId);
    int sim_multilayer          = hitHelper->GetMultiLayer(simId);
    int sim_layer               = hitHelper->GetLayer(simId);
    int sim_side                = hitHelper->GetSide(simId);

    ATH_MSG_DEBUG(     "sTGC SimHit  id:  Station Name [" << sim_stationName << " ]"
                    << " Station Eta ["  << sim_stationEta               << "]"
                    << " Station Phi ["  << sim_stationPhi               << "]"
                    << " MultiLayer ["   << sim_multilayer               << "]"
                    << " Layer  ["       << sim_layer                    << "]"
                    << " Side ["         << sim_side                     << "]" );

    if ( sim_stationPhi==0 ) {
        ATH_MSG_ERROR("unexpected phi range: " << sim_stationPhi);
        return StatusCode::FAILURE;
    }
    // Current [7/12/12] implementation of the Station Name is: T[0-3][L/S][P/C] 
    int detNumber = -999, wedgeId = -999, wedgeType = -999;
    if(sim_stationName.length()!=4) {
      ATH_MSG_WARNING("sTGC validation: station Name exceeds 4 charactes, filling dummy information for detNumber, wedgeId and wedgeType");
    } 
    else {
      detNumber = atoi(sim_stationName.substr(1,1).c_str());
      wedgeId   = (sim_stationName.substr(2,1).compare("L")) ? 0 : 1;
      wedgeType = (sim_stationName.substr(3,1).compare("P")) ? 0 : 1;
    }

    // Fill Ntuple with SimId data
    m_NSWsTGC_sim_stationName   ->push_back(sim_stationName);
    m_NSWsTGC_sim_stationEta    ->push_back(sim_stationEta);
    m_NSWsTGC_sim_stationPhi    ->push_back(sim_stationPhi);
    m_NSWsTGC_wedgeId           ->push_back(wedgeId);
    m_NSWsTGC_wedgeType         ->push_back(wedgeType);
    m_NSWsTGC_detectorNumber    ->push_back(detNumber);
    m_NSWsTGC_sim_multilayer    ->push_back(sim_multilayer);
    m_NSWsTGC_sim_layer         ->push_back(sim_layer);
    m_NSWsTGC_sim_side          ->push_back(sim_side);

    //  convert simHit id to offline id; make sanity checks; retrieve the associated detector element.
    Identifier offId = simToOffline.convert(hit.GenericId());
    if( !m_sTgcIdHelper->is_stgc(offId) ){
        ATH_MSG_WARNING("sTgc id is not a stgc id! " << m_sTgcIdHelper->print_to_string(offId));
    }
    if( !m_sTgcIdHelper->is_muon(offId) ){
        ATH_MSG_WARNING("sTgc id is not a muon id! " << m_sTgcIdHelper->print_to_string(offId));
    }
    if( m_sTgcIdHelper->is_mdt(offId)||m_sTgcIdHelper->is_rpc(offId)||m_sTgcIdHelper->is_tgc(offId)||m_sTgcIdHelper->is_csc(offId)||m_sTgcIdHelper->is_mm(offId) ){
        ATH_MSG_WARNING("sTgc id has wrong technology type! " << m_sTgcIdHelper->is_mdt(offId) << " " << m_sTgcIdHelper->is_rpc(offId)
                        << " " << m_sTgcIdHelper->is_tgc(offId) << " " << m_sTgcIdHelper->is_csc(offId) << " " << m_sTgcIdHelper->is_mm(offId) );
    }
    if( m_sTgcIdHelper->gasGap(offId) != sim_layer ) {
        ATH_MSG_WARNING("sTgc id has bad layer field! " << m_sTgcIdHelper->print_to_string(offId)  );
    }

    std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(offId));
    int off_stationEta   = m_sTgcIdHelper->stationEta(offId); 
    int off_stationPhi   = m_sTgcIdHelper->stationPhi(offId);
    int off_multiplet    = m_sTgcIdHelper->multilayer(offId);
    int off_gas_gap      = m_sTgcIdHelper->gasGap(offId);
    int off_channel_type = m_sTgcIdHelper->channelType(offId);
    int off_channel      = m_sTgcIdHelper->channel(offId);
    
    ATH_MSG_DEBUG(     "sTGC Offline id:  Station Name [" << stName << "]"
                    << " Station Eta ["  << off_stationEta      << "]"
                    << " Station Phi ["  << off_stationPhi      << "]"
                    << " Multiplet  ["   << off_multiplet       << "]"
                    << " GasGap ["       << off_gas_gap         << "]"
                    << " Type ["         << off_channel_type    << "]"
                    << " ChNr ["         << off_channel         << "]" );

    int isSmall = stName[2] == 'S';

    ATH_MSG_DEBUG("sTGC geometry, retrieving detector element for: isSmall " << isSmall << " eta " << m_sTgcIdHelper->stationEta(offId)
                  << " phi " << m_sTgcIdHelper->stationPhi(offId) << " ml " << m_sTgcIdHelper->multilayer(offId) );

    int phiCor = m_sTgcIdHelper->stationPhi(offId);

    if( sim_stationPhi == 15 && hit.globalPosition().phi() > -0.4 ) phiCor = 1;

    int mlCor = m_sTgcIdHelper->multilayer(offId);

    if( !isSmall ){
      if( mlCor == 1 ) mlCor = 2;
      else             mlCor = 1;
    }

    const MuonGM::sTgcReadoutElement* detEl = m_detManager->getsTgcRElement_fromIdFields(isSmall,m_sTgcIdHelper->stationEta(offId),phiCor,mlCor );
    if( !detEl ){
      ATH_MSG_WARNING("sTGC geometry, failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_sTgcIdHelper->stationEta(offId)
                      << " phi " << m_sTgcIdHelper->stationPhi(offId) << " ml " << m_sTgcIdHelper->multilayer(offId) );
      continue;
    }

    // compute hit position within the detector element/surfaces
    const Trk::PlaneSurface& surf = detEl->surface(offId);
    Amg::Transform3D     gToL = detEl->absTransform().inverse();
    Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
    Amg::Vector3D dSurface_pos = gToL*hpos;

    // compute the hit position on the readout plane (same as in MuonFastDigitization)
    Amg::Vector3D rSurface_pos = surf.transform().inverse()*hpos;
    Amg::Vector3D ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(),hit.globalDirection().y(),hit.globalDirection().z());

    double scale = -rSurface_pos.z()/ldir.z();
    Amg::Vector3D hitOnSurface = rSurface_pos + scale*ldir;

    // hitOnSurface.x() will be susequent smeared to simulate the detector resolution, here we do not apply any smearing
    Amg::Vector2D  posOnSurf(hitOnSurface.x(), rSurface_pos.y());

    int stripNumber = detEl->stripNumber(posOnSurf,offId);
    if( stripNumber == -1 ){
      ATH_MSG_WARNING("sTGC validation: failed to obtain strip number " << m_sTgcIdHelper->print_to_string(offId) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
      stripNumber = 1;
    }
    Identifier oldId = offId;
    offId = m_sTgcIdHelper->channelID(offId, m_sTgcIdHelper->multilayer(offId), m_sTgcIdHelper->gasGap(offId),1,stripNumber);
    if( m_sTgcIdHelper->gasGap(offId) != sim_layer ) {
      ATH_MSG_WARNING("sTGC validation: sTgc id has bad layer field(2)! " << std::endl << " " << m_sTgcIdHelper->print_to_string(offId) << std::endl
                      << " " << m_sTgcIdHelper->print_to_string(oldId) << " stripN " << stripNumber );
    }

    Amg::Vector2D fastDigitPos(0,0);
    if( !detEl->stripPosition(offId,fastDigitPos) ){
      ATH_MSG_WARNING("sTGC validation: failed to obtain local position for identifier " << m_sTgcIdHelper->print_to_string(offId) );
      continue;
    }

    Amg::Vector3D detpos = detEl->globalPosition();
    ATH_MSG_DEBUG("sTGC Global hit: r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                    << "; detEl: r " << detpos.perp()     << ", phi " << detpos.phi()   << ", z " << detpos.z()
                    << "; surf z "   << surf.center().z() << ", ml "  << sim_multilayer << ", l " << sim_layer );

    ATH_MSG_DEBUG("sTGC Local hit : x " << hit.localPosition().x() << " y " << hit.localPosition().y() << " z " << hit.localPosition().z()
                    << " detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
    ATH_MSG_DEBUG("sTGC Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                    << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );



    // Fill Ntuple with offline ID data
    m_NSWsTGC_off_stationName   ->push_back(stName);
    m_NSWsTGC_off_stationEta    ->push_back(off_stationEta);                                         
    m_NSWsTGC_off_stationPhi    ->push_back(off_stationPhi);
    m_NSWsTGC_off_multiplet     ->push_back(off_multiplet);
    m_NSWsTGC_off_gas_gap       ->push_back(off_gas_gap);
    m_NSWsTGC_off_channel_type  ->push_back(off_channel_type);
    m_NSWsTGC_off_channel       ->push_back(off_channel);

    // Fill ntuple with the hit/surface/digit positions
    m_NSWsTGC_detector_globalPositionX->push_back( detpos.x() );
    m_NSWsTGC_detector_globalPositionY->push_back( detpos.y() );
    m_NSWsTGC_detector_globalPositionZ->push_back( detpos.z() );
    m_NSWsTGC_detector_globalPositionR->push_back( detpos.perp() );
    m_NSWsTGC_detector_globalPositionP->push_back( detpos.phi() );

    m_NSWsTGC_hitToDsurfacePositionX->push_back( dSurface_pos.x() );
    m_NSWsTGC_hitToDsurfacePositionY->push_back( dSurface_pos.y() );
    m_NSWsTGC_hitToDsurfacePositionZ->push_back( dSurface_pos.z() );

    m_NSWsTGC_hitToRsurfacePositionX->push_back( rSurface_pos.x() );
    m_NSWsTGC_hitToRsurfacePositionY->push_back( rSurface_pos.y() );
    m_NSWsTGC_hitToRsurfacePositionZ->push_back( rSurface_pos.z() );

    m_NSWsTGC_FastDigitRsurfacePositionX->push_back( posOnSurf.x() );
    m_NSWsTGC_FastDigitRsurfacePositionY->push_back( posOnSurf.y() );

    

    m_NSWsTGC_stripNumber->push_back(stripNumber);
    m_NSWsTGC_wireNumber->push_back(-999);

    ATH_MSG_DEBUG("---------- Hit processing ends!");
/*
    ATH_MSG_DEBUG(     " NSW Hits E = "      << hit.depositEnergy()
		    << ", Global X sTGC = "  << globalPosition.x()
		    << ", Global Y sTGC = "  << globalPosition.y()
		    << ", Global Z sTGC = "  << globalPosition.z()
		    << ", Local X sTGC = "   << localPosition.x()
		    << ", Local Y sTGC = "   << localPosition.y()
		    << ", Local Z sTGC = "   << localPosition.z()
		    << ", time = "           << hit.globalTime()
		    << ", step length = "    << hit.StepLength() );
*/    
    m_NSWsTGC_nSimHits++;
  }

  ATH_MSG_INFO("processed " << m_NSWsTGC_nSimHits << " sTGC sim hits");
  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::fillNSWsTGCFastDigitVariables() 
{   
  ATH_MSG_INFO("do fillNSWsTGCFastDigitVariables()");
  clearNSWsTGCFastDigitVariables();

  const MuonSimDataCollection* nsw_sTgcSdoContainer = 0;
  StatusCode sc = evtStore()->retrieve(nsw_sTgcSdoContainer, "STGC_SDO");
  if(!sc.isSuccess()) {  
    ATH_MSG_WARNING("Could not retrieve MM_SDO; MM fast digit branches will not have the mc truth associated.");
  }

  const Muon::sTgcPrepDataContainer *nsw_sTgcPrepDataContainer = 0; 
  sc = evtStore()->retrieve(nsw_sTgcPrepDataContainer, m_NSWsTGC_FastDigitContainerName.c_str());
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve " << m_NSWsTGC_FastDigitContainerName.c_str());
    return StatusCode::SUCCESS;
  }

  Muon::sTgcPrepDataContainer::const_iterator it   = nsw_sTgcPrepDataContainer->begin();
  Muon::sTgcPrepDataContainer::const_iterator it_e = nsw_sTgcPrepDataContainer->end();

  if(nsw_sTgcPrepDataContainer->size()==0) msg(MSG::WARNING) <<" sTgc Fast DigitContainer empty " << endreq;

  for(; it!=it_e; ++it) {
    const Muon::sTgcPrepDataCollection* coll = *it;
    
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::sTgcPrepData* fast_digit = coll->at(item);
      Identifier Id = fast_digit->identify();

      std::string stName   = m_sTgcIdHelper->stationNameString(m_sTgcIdHelper->stationName(Id));
      int stationEta       = m_sTgcIdHelper->stationEta(Id);
      int stationPhi       = m_sTgcIdHelper->stationPhi(Id);
      int multiplet        = m_sTgcIdHelper->multilayer(Id);
      int gas_gap          = m_sTgcIdHelper->gasGap(Id);
      int channel_type     = m_sTgcIdHelper->channelType(Id);
      int channel          = m_sTgcIdHelper->channel(Id);

      ATH_MSG_DEBUG(     "sTGC Fast Digit Offline id:  Station Name [" << stName << "]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " Type ["         << channel_type    << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWsTGC_fdg_stationName->push_back(stName);
      m_NSWsTGC_fdg_stationEta->push_back(stationEta);
      m_NSWsTGC_fdg_stationPhi->push_back(stationPhi);
      m_NSWsTGC_fdg_multiplet->push_back(multiplet);
      m_NSWsTGC_fdg_gas_gap->push_back(gas_gap);
      m_NSWsTGC_fdg_channel_type->push_back(channel_type);
      m_NSWsTGC_fdg_channel->push_back(channel);

      const MuonGM::sTgcReadoutElement* det = fast_digit->detectorElement();
      Amg::Vector3D pos = fast_digit->globalPosition();
      Amg::Vector2D loc_pos(0., 0.);
       det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      double err_x = fast_digit->localCovariance()(0,0);
      double err_y = ( fast_digit->localCovariance().rows()==2)? fast_digit->localCovariance()(1,1) : 0.;

      ATH_MSG_DEBUG(     "sTgc Fast Digit local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                << ", ex=" << std::setw(6) << std::setprecision(2) << err_x
                                                << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] 
                                                << ", ey=" << std::setw(6) << std::setprecision(2) << err_y );

      m_NSWsTGC_fdg_globalPosX->push_back(pos.x());
      m_NSWsTGC_fdg_globalPosY->push_back(pos.y());
      m_NSWsTGC_fdg_globalPosZ->push_back(pos.z());

      m_NSWsTGC_fdg_localPosX->push_back(loc_pos[0]);
      m_NSWsTGC_fdg_localPosY->push_back(loc_pos[1]);
      m_NSWsTGC_fdg_covMatrix_1_1->push_back(err_x);
      m_NSWsTGC_fdg_covMatrix_2_2->push_back(err_y);


      if (nsw_sTgcSdoContainer) {
        const MuonSimData stgc_sdo = (nsw_sTgcSdoContainer->find(Id))->second;
        std::vector<MuonSimData::Deposit> deposits;
        stgc_sdo.deposits(deposits);

        int    truth_barcode       = deposits[0].first.barcode();
        double truth_hitOnSurfaceX = deposits[0].second.firstEntry(); 
        double truth_hitOnSurfaceY = deposits[0].second.secondEntry();

        ATH_MSG_DEBUG("sTGC Fast Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("sTGC Fast Digit, truth hitOnSurfaceX=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceX
                                   << ", truth hitOnSurfaceY=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceY);

        m_NSWsTGC_fdg_truth_barcode->push_back( truth_barcode );
        m_NSWsTGC_fdg_truth_hitOnSurfaceX->push_back( truth_hitOnSurfaceX );
        m_NSWsTGC_fdg_truth_hitOnSurfaceY->push_back( truth_hitOnSurfaceY );
      }


      m_NSWsTGC_nFastDigits++;
    }
  }

  ATH_MSG_INFO("processed " << m_NSWsTGC_nFastDigits << " sTGC fast digitis");

  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::fillNSWsTGCDigitVariables() 
{   
  clearNSWsTGCDigitVariables();
  return StatusCode::SUCCESS;
}



StatusCode NSWPRDValAlg::fillNSWMMHitVariables() 
{
  ATH_MSG_INFO("do fillNSWMMHitVariables()");

  clearNSWMMHitVariables();

  const GenericMuonSimHitCollection *nswContainer = 0;
  StatusCode sc = evtStore()->retrieve(nswContainer, m_NSWMM_ContainerName.c_str());
  if(!sc.isSuccess()) 
    {
      ATH_MSG_WARNING("Could not retrieve " << m_NSWMM_ContainerName.c_str());
      return StatusCode::SUCCESS;
    }
  
  // Get the MicroMegas Id hit helper
  MicromegasHitIdHelper* hitHelper = MicromegasHitIdHelper::GetHelper();
  MM_SimIdToOfflineId simToOffline(*m_MmIdHelper);

  GenericMuonSimHitCollection::const_iterator it   = nswContainer->begin();
  GenericMuonSimHitCollection::const_iterator it_e = nswContainer->end();
  if(nswContainer->size()==0) msg(MSG::WARNING) <<" GenericMuonSimHit empty " << endreq;
  for(; it!=it_e; ++it) {
    const GenericMuonSimHit hit = *it;

    if(hit.depositEnergy()==0.) continue; // SimHits without energy loss are not recorded. 
    //    if( hit.kineticEnergy()<m_energyThreshold && abs(hit.particleEncoding())==11) continue ; 
       
    // connect the hit with the MC truth
    int barcode = hit.particleLink().barcode();
    m_NSWMM_trackId->push_back(barcode);
    for (unsigned int tr=0;tr<m_Truth_particleBarcode->size();tr++) {
        if (barcode==m_Truth_particleBarcode->at(tr)) {
            m_NSWMM_truthEl->push_back(tr);
        }
    }


    m_NSWMM_globalTime->push_back(hit.globalTime());
  
    const Amg::Vector3D globalPosition = hit.globalPosition();
    m_NSWMM_hitGlobalPositionX->push_back(globalPosition.x());
    m_NSWMM_hitGlobalPositionY->push_back(globalPosition.y());
    m_NSWMM_hitGlobalPositionZ->push_back(globalPosition.z());
    m_NSWMM_hitGlobalPositionR->push_back(globalPosition.perp());
    m_NSWMM_hitGlobalPositionP->push_back(globalPosition.phi());
    const Amg::Vector3D globalDirection = hit.globalDirection();
    m_NSWMM_hitGlobalDirectionX->push_back(globalDirection.x());
    m_NSWMM_hitGlobalDirectionY->push_back(globalDirection.y());
    m_NSWMM_hitGlobalDirectionZ->push_back(globalDirection.z());

    const Amg::Vector3D localPosition = hit.localPosition();
    m_NSWMM_hitLocalPositionX->push_back(localPosition.x());
    m_NSWMM_hitLocalPositionY->push_back(localPosition.y());
    m_NSWMM_hitLocalPositionZ->push_back(localPosition.z());

    m_NSWMM_particleEncoding->push_back(hit.particleEncoding());
    m_NSWMM_kineticEnergy->push_back(hit.kineticEnergy());
    m_NSWMM_depositEnergy->push_back(hit.depositEnergy());
    m_NSWMM_StepLength->push_back(hit.StepLength());


    // Read the information about the Micro Megas hit
    int simId = hit.GenericId();
    std::string sim_stationName = hitHelper->GetStationName(simId);
    int sim_stationEta  = hitHelper->GetZSector(simId);
    int sim_stationPhi  = hitHelper->GetPhiSector(simId);
    int sim_multilayer  = hitHelper->GetMultiLayer(simId);
    int sim_layer       = hitHelper->GetLayer(simId);
    int sim_side        = hitHelper->GetSide(simId);

    ATH_MSG_DEBUG(     "MicroMegas SimHit  id:  Station Name [" << sim_stationName << "]"
                    << " Station Eta ["  << sim_stationEta  << "]"
                    << " Station Phi ["  << sim_stationPhi  << "]"
                    << " MultiLayer ["   << sim_multilayer  << "]"
                    << " Layer  ["        << sim_layer       << "]"
                    << " Side ["         << sim_side        << "]" );

    if( sim_stationPhi == 0 ){
      ATH_MSG_ERROR("MicroMegas validation: unexpected phi range " << sim_stationPhi);
      return StatusCode::FAILURE;
    }

    // Fill Ntuple with SimId data
    m_NSWMM_sim_stationName ->push_back(sim_stationName);
    m_NSWMM_sim_stationEta  ->push_back(sim_stationEta);
    m_NSWMM_sim_stationPhi  ->push_back(sim_stationPhi);
    m_NSWMM_sim_multilayer  ->push_back(sim_multilayer);
    m_NSWMM_sim_layer       ->push_back(sim_layer);
    m_NSWMM_sim_side        ->push_back(sim_side);


    //  convert simHit id to offline id; make sanity checks; retrieve the associated detector element.
    Identifier offId = simToOffline.convert(hit.GenericId());
    
    // sanity checks
    if( !m_MmIdHelper->is_mm(offId) ){
      ATH_MSG_WARNING("MM id is not a mm id! " << m_MmIdHelper->print_to_string(offId));
    }
    if( !m_MmIdHelper->is_muon(offId) ){
      ATH_MSG_WARNING("MM id is not a muon id! " << m_MmIdHelper->print_to_string(offId));
    }
    if( m_MmIdHelper->is_mdt(offId)||m_MmIdHelper->is_rpc(offId)||m_MmIdHelper->is_tgc(offId)||m_MmIdHelper->is_csc(offId)||m_MmIdHelper->is_stgc(offId) ){
      ATH_MSG_WARNING("MM id has wrong technology type! " << m_MmIdHelper->is_mdt(offId) << " " << m_MmIdHelper->is_rpc(offId)
                      << " " << m_MmIdHelper->is_tgc(offId) << " " << m_MmIdHelper->is_csc(offId) << " " << m_MmIdHelper->is_stgc(offId) );
    }
    if( m_MmIdHelper->gasGap(offId) != sim_layer ) {
      ATH_MSG_WARNING("MM id has bad layer field! " << m_MmIdHelper->print_to_string(offId)  );
    }


    std::string stName = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(offId));
    int off_stationEta   = m_MmIdHelper->stationEta(offId);
    int off_stationPhi   = m_MmIdHelper->stationPhi(offId);
    int off_multiplet    = m_MmIdHelper->multilayer(offId);
    int off_gas_gap      = m_MmIdHelper->gasGap(offId);
    int off_channel      = m_MmIdHelper->channel(offId);


    ATH_MSG_DEBUG(     "MicroMegas Offline id:  Station Name [" << stName << " ]"
                    << " Station Eta ["  << off_stationEta      << "]"
                    << " Station Phi ["  << off_stationPhi      << "]"
                    << " Multiplet  ["   << off_multiplet       << "]"
                    << " GasGap ["       << off_gas_gap         << "]"
                    << " ChNr ["         << off_channel         << "]" );


    int isSmall = stName[2] == 'S';
    ATH_MSG_DEBUG("MicroMegas geometry, retrieving detector element for: isSmall " << isSmall << " eta " << m_MmIdHelper->stationEta(offId)
                  << " phi " << m_MmIdHelper->stationPhi(offId) << " ml " << m_MmIdHelper->multilayer(offId) );

    //   int phiCor = m_MmIdHelper->stationPhi(offId);
    //   int mlCor  = m_MmIdHelper->multiplet(offId);

    // const MuonGM::MMReadoutElement* detEl = m_detManager->getMMRElement_fromIdFields(isSmall, m_MmIdHelper->stationEta(offId), phiCor, mlCor );  
    const MuonGM::MMReadoutElement* detEl = m_detManager->getMMReadoutElement(offId);

    if( !detEl ){
      /*
	ATH_MSG_WARNING("MicroMegas geometry, failed to retrieve detector element for: isSmall " << isSmall << " eta " << m_MmIdHelper->stationEta(offId)
	<< " phi " << m_MmIdHelper->stationPhi(offId) << " ml " << m_MmIdHelper->multiplet(offId) );
      */		      
      ATH_MSG_WARNING("MicroMegas geometry, failed to retrieve detector element for: " << m_MmIdHelper->print_to_string(offId) );
      continue;
    }

    // surface
    const Trk::PlaneSurface& surf = detEl->surface(offId);
    // compute hit position within the detector element/surfaces
    Amg::Transform3D     gToL = detEl->absTransform().inverse();
    Amg::Vector3D hpos(hit.globalPosition().x(),hit.globalPosition().y(),hit.globalPosition().z());
    Amg::Vector3D dSurface_pos = gToL*hpos;

    // compute the hit position on the readout plane (same as in MuonFastDigitization)
    Amg::Vector3D rSurface_pos = surf.transform().inverse()*hpos;
     
    Amg::Vector2D  posOnSurfUnProjected(rSurface_pos.x(),rSurface_pos.y());
//    double gasGapThickness = detEl->getDesign(offId)->gasGapThickness();

    // check where the readout plane is located and compute the local direction accordingly 
    Amg::Vector3D ldir(0., 0., 0.);
    if (MM_READOUT[m_MmIdHelper->gasGap(offId)-1]==1)
      ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), hit.globalDirection().z());
    else
      ldir = surf.transform().inverse().linear()*Amg::Vector3D(hit.globalDirection().x(), hit.globalDirection().y(), -hit.globalDirection().z());
  
    double scale, scaletop;
    double gasgap = 5.;
   
    scale = -rSurface_pos.z()/ldir.z();
    scaletop = (gasgap+rSurface_pos.z())/ldir.z();
    // scaletop = (fabs(gasGapThickness) + slpos.z())/locdir.z();
      	
    Amg::Vector3D hitOnSurface = rSurface_pos + scale*ldir;
    Amg::Vector3D hitOnTopSurface = rSurface_pos + scaletop*ldir;
    Amg::Vector2D posOnSurf (hitOnSurface.x(), hitOnSurface.y());
    Amg::Vector2D posOnTopSurf (hitOnTopSurface.x(),hitOnTopSurface.y());
        
  
    int stripNumber = detEl->stripNumber(posOnSurf,offId);
//    int LastStripNumber = detEl->stripNumber(posOnTopSurf, offId);
     
    // perform bound check
    if( !surf.insideBounds(posOnSurf) ) continue;
        
    if( stripNumber == -1 ){
      ATH_MSG_WARNING("MicroMegas validation: failed to obtain strip number " << m_MmIdHelper->print_to_string(offId) );
      ATH_MSG_WARNING(" pos " << posOnSurf << " z " << rSurface_pos.z() );
      stripNumber = 1;
    }
    
    Identifier oldId = offId;
    offId = m_MmIdHelper->channelID(offId, m_MmIdHelper->multilayer(offId), m_MmIdHelper->gasGap(offId),stripNumber);
    if( m_MmIdHelper->gasGap(offId) != sim_layer ) {
      ATH_MSG_WARNING("MicroMegas validation: MM id has bad layer field(2)! " << std::endl << " " << m_MmIdHelper->print_to_string(offId) << std::endl
                      << " " << m_MmIdHelper->print_to_string(oldId) << " stripN " << stripNumber );
    }

    Amg::Vector2D fastDigitPos(0.,0.);
    if( !detEl->stripPosition(offId,fastDigitPos ) ){
      ATH_MSG_WARNING("MicroMegas validation: failed to obtain local position for identifier " << m_MmIdHelper->print_to_string(offId) );
      continue;
    }

    Amg::Vector3D detpos = detEl->globalPosition();
    ATH_MSG_DEBUG("Global hit   : r " << hit.globalPosition().perp() << ", phi " << hit.globalPosition().phi() << ", z " << hit.globalPosition().z()
                    << "; detEl: r " << detpos.perp() << ", phi " << detpos.phi() << ", z " << detpos.z()
                    << "; surf z "   << surf.center().z() << ", ml "  << sim_multilayer << ", l " << sim_layer );
    ATH_MSG_DEBUG("Local hit    : x " << hit.localPosition().x() << " y " << hit.localPosition().y() << " z " << hit.localPosition().z()
                    << " detEl: x " << dSurface_pos.x() << " y " << dSurface_pos.y() << " z " << dSurface_pos.z());
    ATH_MSG_DEBUG("MM Fast digit: x " << fastDigitPos.x() << " y " << fastDigitPos.y()
                    << ", gToL: x " << rSurface_pos.x() << " y " << rSurface_pos.y() << " z " << rSurface_pos.z() );



    // Fill Ntuple with offline ID data
    m_NSWMM_off_stationName   ->push_back(stName);
    m_NSWMM_off_stationEta    ->push_back(off_stationEta);
    m_NSWMM_off_stationPhi    ->push_back(off_stationPhi);
    m_NSWMM_off_multiplet     ->push_back(off_multiplet);
    m_NSWMM_off_gas_gap       ->push_back(off_gas_gap);
    m_NSWMM_off_channel       ->push_back(off_channel);


    // Fill ntuple with the hit/surface/digit positions
    m_NSWMM_detector_globalPositionX->push_back( detpos.x() );
    m_NSWMM_detector_globalPositionY->push_back( detpos.y() );
    m_NSWMM_detector_globalPositionZ->push_back( detpos.z() );
    m_NSWMM_detector_globalPositionR->push_back( detpos.perp() );
    m_NSWMM_detector_globalPositionP->push_back( detpos.phi() );

    m_NSWMM_hitToDsurfacePositionX->push_back( dSurface_pos.x() );
    m_NSWMM_hitToDsurfacePositionY->push_back( dSurface_pos.y() );
    m_NSWMM_hitToDsurfacePositionZ->push_back( dSurface_pos.z() );

    m_NSWMM_hitToRsurfacePositionX->push_back( rSurface_pos.x() );
    m_NSWMM_hitToRsurfacePositionY->push_back( rSurface_pos.y() );
    m_NSWMM_hitToRsurfacePositionZ->push_back( rSurface_pos.z() );

    m_NSWMM_FastDigitRsurfacePositionX->push_back( posOnSurf.x() );
    m_NSWMM_FastDigitRsurfacePositionY->push_back( posOnSurf.y() );



    ATH_MSG_DEBUG("---------- Hit processing ends!");

    m_NSWMM_nSimHits++;
  }

  ATH_MSG_INFO("processed " << m_NSWMM_nSimHits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::fillNSWMMFastDigitVariables()
{
  ATH_MSG_INFO(" do fillNSWMMFastDigitVariables()");
  clearNSWMMFastDigitVariables();

  const MuonSimDataCollection* nsw_MmSdoContainer = 0;
  StatusCode sc = evtStore()->retrieve(nsw_MmSdoContainer, "MM_SDO");
  if(!sc.isSuccess()) {  
    ATH_MSG_WARNING("Could not retrieve MM_SDO; MM fast digit branches will not have the mc truth associated.");
  }


  const Muon::MMPrepDataContainer *nsw_MMPrepDataContainer = 0;
  sc = evtStore()->retrieve(nsw_MMPrepDataContainer, m_NSWMM_FastDigitContainerName.c_str());
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve " << m_NSWMM_FastDigitContainerName.c_str());
    return StatusCode::SUCCESS;
  }

  Muon::MMPrepDataContainer::const_iterator it   = nsw_MMPrepDataContainer->begin();
  Muon::MMPrepDataContainer::const_iterator it_e = nsw_MMPrepDataContainer->end();

  if(nsw_MMPrepDataContainer->size()==0) msg(MSG::WARNING) <<" MM Fast DigitContainer empty " << endreq;

  for(; it!=it_e; ++it) {
    const Muon::MMPrepDataCollection* coll = *it;
    
    for (unsigned int item=0; item<coll->size(); item++) {
      const Muon::MMPrepData* fast_digit = coll->at(item);
      Identifier Id = fast_digit->identify();

      std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
      int stationEta       = m_MmIdHelper->stationEta(Id);
      int stationPhi       = m_MmIdHelper->stationPhi(Id);
      int multiplet        = m_MmIdHelper->multilayer(Id);
      int gas_gap          = m_MmIdHelper->gasGap(Id);
      int channel          = m_MmIdHelper->channel(Id);


      ATH_MSG_DEBUG(     "MicroMegas Fast Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWMM_fdg_stationName->push_back(stName);
      m_NSWMM_fdg_stationEta->push_back(stationEta);
      m_NSWMM_fdg_stationPhi->push_back(stationPhi);
      m_NSWMM_fdg_multiplet->push_back(multiplet);
      m_NSWMM_fdg_gas_gap->push_back(gas_gap);
      m_NSWMM_fdg_channel->push_back(channel);

      const MuonGM::MMReadoutElement* det = fast_digit->detectorElement();
      Amg::Vector3D pos    = fast_digit->globalPosition();
      const Amg::MatrixX & cov = fast_digit->localCovariance();
      Amg::Vector2D loc_pos(0., 0.);
      det->surface(Id).globalToLocal(pos, Amg::Vector3D(0., 0., 0.), loc_pos);

      ATH_MSG_DEBUG(     "MicroMegas Fast Digit local pos.:  x=" << std::setw(6) << std::setprecision(2) << loc_pos[0]
                                                      << ", ex=" << std::setw(6) << std::setprecision(2) << cov(0,0)
                                                      << ",  y=" << std::setw(6) << std::setprecision(2) << loc_pos[1] );

      m_NSWMM_fdg_globalPosX->push_back(pos.x());
      m_NSWMM_fdg_globalPosY->push_back(pos.y());
      m_NSWMM_fdg_globalPosZ->push_back(pos.z());

      m_NSWMM_fdg_localPosX->push_back(loc_pos[0]);
      m_NSWMM_fdg_localPosY->push_back(loc_pos[1]);
      m_NSWMM_fdg_covMatrix_1_1->push_back(cov(0,0));

      if (nsw_MmSdoContainer) {
        const MuonSimData mm_sdo = (nsw_MmSdoContainer->find(Id))->second;
        std::vector<MuonSimData::Deposit> deposits;
        mm_sdo.deposits(deposits);

        int    truth_barcode       = deposits[0].first.barcode();
        double truth_hitOnSurfaceX = deposits[0].second.firstEntry(); 
        double truth_hitOnSurfaceY = deposits[0].second.secondEntry();

        ATH_MSG_DEBUG("MicroMegas Fast Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("MicroMegas Fast Digit, truth hitOnSurfaceX=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceX
                                   << ", truth hitOnSurfaceY=" << std::setw(9) << std::setprecision(2) << truth_hitOnSurfaceY);

        m_NSWMM_fdg_truth_barcode->push_back( truth_barcode );
        m_NSWMM_fdg_truth_hitOnSurfaceX->push_back( truth_hitOnSurfaceX );
        m_NSWMM_fdg_truth_hitOnSurfaceY->push_back( truth_hitOnSurfaceY );
      }

      m_NSWMM_nFastDigits++;
    }
  }

  ATH_MSG_INFO("processed " << m_NSWMM_nFastDigits << " MicroMegas fast digitis");
  return StatusCode::SUCCESS;
}

StatusCode NSWPRDValAlg::fillNSWMMDigitVariables()
{
  ATH_MSG_INFO("do fillNSWMMDigitVariables()");
  clearNSWMMDigitVariables();

  const MuonSimDataCollection* nsw_MmSdoContainer = 0;
  StatusCode sc = evtStore()->retrieve(nsw_MmSdoContainer, "MM_SDO");
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve MM_SDO; MM digit branches will not have the mc truth associated.");
  }

  const MmDigitContainer *nsw_MmDigitContainer = 0;
  sc = evtStore()->retrieve(nsw_MmDigitContainer, m_NSWMM_DigitContainerName.c_str());
  if(!sc.isSuccess()) {
    ATH_MSG_WARNING("Could not retrieve " << m_NSWMM_DigitContainerName.c_str());
    return StatusCode::SUCCESS;
  }

  MmDigitContainer::const_iterator it   = nsw_MmDigitContainer->begin();
  MmDigitContainer::const_iterator it_e = nsw_MmDigitContainer->end();

  if(nsw_MmDigitContainer->size()==0) msg(MSG::WARNING) <<" MM DigitContainer empty " << endreq;


  for(; it!=it_e; ++it) {
    const MmDigitCollection* coll = *it;

    for (unsigned int item=0; item<coll->size(); item++) {
      const MmDigit* digit = coll->at(item);
      Identifier Id = digit->identify();

      std::string stName   = m_MmIdHelper->stationNameString(m_MmIdHelper->stationName(Id));
      int stationEta       = m_MmIdHelper->stationEta(Id);
      int stationPhi       = m_MmIdHelper->stationPhi(Id);
      int multiplet        = m_MmIdHelper->multilayer(Id);
      int gas_gap          = m_MmIdHelper->gasGap(Id);
      int channel          = m_MmIdHelper->channel(Id);
    

      ATH_MSG_DEBUG(     "MicroMegas Digit Offline id:  Station Name [" << stName << " ]"
                      << " Station Eta ["  << stationEta      << "]"
                      << " Station Phi ["  << stationPhi      << "]"
                      << " Multiplet  ["   << multiplet       << "]"
                      << " GasGap ["       << gas_gap         << "]"
                      << " ChNr ["         << channel         << "]" );

      m_NSWMM_dig_stationName->push_back(stName);
      m_NSWMM_dig_stationEta->push_back(stationEta);
      m_NSWMM_dig_stationPhi->push_back(stationPhi);
      m_NSWMM_dig_multiplet->push_back(multiplet);
      m_NSWMM_dig_gas_gap->push_back(gas_gap);
      m_NSWMM_dig_channel->push_back(channel);

      int isSmall = stName[2] == 'S';
      const MuonGM::MMReadoutElement* rdoEl = m_detManager->getMMRElement_fromIdFields(isSmall, stationEta, stationPhi, multiplet );


    
      std::vector<float>  time          = digit->chipResponseTime();
      std::vector<float>  charge        = digit->chipResponseCharge();
      std::vector<int>    stripPosition = digit->chipResponsePosition();
      std::vector<double> localPosX;
      std::vector<double> localPosY;
      std::vector<double> globalPosX;
      std::vector<double> globalPosY;
      std::vector<double> globalPosZ;


      std::vector<float>  sr_time          = digit->stripResponseTime();
      std::vector<float>  sr_charge        = digit->stripResponseCharge();
      std::vector<int>    sr_stripPosition = digit->stripResponsePosition();
      std::vector<double> sr_localPosX;
      std::vector<double> sr_localPosY;
      std::vector<double> sr_globalPosX;
      std::vector<double> sr_globalPosY;
      std::vector<double> sr_globalPosZ;


      if ( stripPosition.size() != sr_stripPosition.size() ) {
        ATH_MSG_DEBUG("MicroMegas digitization: number of strip out from the strip response different from that out from the chip response");
      }

      bool isValid;
      int stationName  = m_MmIdHelper->stationName(Id);

      for (unsigned int i=0;i<stripPosition.size();i++) {
        int cr_strip = stripPosition.at(i);

        localPosX.push_back(0.);
        localPosY.push_back(0.);
        globalPosX.push_back(0.);
        globalPosY.push_back(0.);
        globalPosZ.push_back(0.);

        //retrieve the position for the strip form the strip response
        Identifier cr_id = m_MmIdHelper->channelID(stationName, stationEta, stationPhi, multiplet, gas_gap, cr_strip, true, &isValid);
        if (!isValid) {
          ATH_MSG_WARNING("MicroMegas digitization: failed to create a valid ID for (chip response) strip n. " << cr_strip
                       << "; associated positions will be set to 0.0.");
        } else {
          Amg::Vector2D cr_strip_pos(0., 0.);
          if ( !rdoEl->stripPosition(cr_id,cr_strip_pos) ) {
            ATH_MSG_WARNING("MicroMegas digitization: failed to associate a valid local position for (chip response) strip n. " << cr_strip
                         << "; associated positions will be set to 0.0.");
          } else {
            localPosX.at(i) = cr_strip_pos.x();
            localPosY.at(i) = cr_strip_pos.y();
          }

          Amg::Vector3D cr_strip_gpos(0., 0., 0.);
          rdoEl->surface(cr_id).localToGlobal(cr_strip_pos, Amg::Vector3D(0., 0., 0.), cr_strip_gpos);
          globalPosX.at(i) = cr_strip_gpos[0];
          globalPosY.at(i) = cr_strip_gpos[1];
          globalPosZ.at(i) = cr_strip_gpos[2];

          // check if local and global position are congruent with the transform
          Amg::Vector3D lpos = rdoEl->transform(cr_id).inverse() * cr_strip_gpos;
          double dx = cr_strip_pos.x() - lpos.x();
          double dy = cr_strip_pos.y() - lpos.y();

          if ( fabs(dx)>0.1 || fabs(dy)>0.1 ) {
            ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
            ATH_MSG_WARNING("                         X from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.x()
                         << ", X from global*transform(id).inverse(): " << lpos.x() );            
            ATH_MSG_WARNING("                         Y from stripPosition: " << std::setw(10) << std::setprecision(3) << cr_strip_pos.y()
                         << ", Y from global*transform(Id).inverse(): " << lpos.y() );
          }
       }
     }


     for (unsigned int i=0;i<sr_stripPosition.size();i++) {
        int sr_strip = sr_stripPosition.at(i);

        sr_localPosX.push_back(0.);
        sr_localPosY.push_back(0.);
        sr_globalPosX.push_back(0.);
        sr_globalPosY.push_back(0.);
        sr_globalPosZ.push_back(0.);

        Identifier sr_id = m_MmIdHelper->channelID(stationName, stationEta, stationPhi, multiplet, gas_gap, sr_strip, true, &isValid);
        if (!isValid) {
          ATH_MSG_WARNING("MicroMegas digitization: failed to create a valid ID for (chip response) strip n. " << sr_strip
                       << "; associated positions will be set to 0.0.");
        } else {
          Amg::Vector2D sr_strip_pos(0., 0.);
          if ( !rdoEl->stripPosition(sr_id,sr_strip_pos) ) {
            ATH_MSG_WARNING("MicroMegas digitization: failed to associate a valid local position for (chip response) strip n. " << sr_strip
                         << "; associated positions will be set to 0.0.");
          } else {
            sr_localPosX.at(i) = sr_strip_pos.x();
            sr_localPosY.at(i) = sr_strip_pos.y();
          }

          Amg::Vector3D sr_strip_gpos(0., 0., 0.);
          rdoEl->surface(sr_id).localToGlobal(sr_strip_pos, Amg::Vector3D(0., 0., 0.), sr_strip_gpos);
          sr_globalPosX.at(i) = sr_strip_gpos[0];
          sr_globalPosY.at(i) = sr_strip_gpos[1];
          sr_globalPosZ.at(i) = sr_strip_gpos[2];

          // check if local and global position are congruent with the transform
          Amg::Vector3D lpos = rdoEl->transform(sr_id).inverse() * sr_strip_gpos;
          double dx = sr_strip_pos.x() - lpos.x();
          double dy = sr_strip_pos.y() - lpos.y();

          if ( fabs(dx)>0.1 || fabs(dy)>0.1 ) {
            ATH_MSG_WARNING("MicroMegas digitization: inconsistency between local/global position and transform:");
            ATH_MSG_WARNING("                         X from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.x()
                         << ", X from local*transform: " << lpos.x() );
            ATH_MSG_WARNING("                         Y from Center(): " << std::setw(10) << std::setprecision(3) << sr_strip_pos.y()
                         << ", Y from local*transform: " << lpos.y() );
          }

        }
      }

      unsigned int cl = (time.size()<=sr_time.size())? time.size() : sr_time.size();

      for (unsigned int i=0;i<cl;i++) {
        ATH_MSG_DEBUG("MicroMegas Digit, chip response time    :" << std::setw(6) << std::setprecision(2) << time.at(i)         
                                 << ",  strip response time    :" << std::setw(6) << std::setprecision(2) << sr_time.at(i));
        ATH_MSG_DEBUG("MicroMegas Digit, chip response charge  :" << std::setw(6) << std::setprecision(2) << charge.at(i)   
                                 << ",  strip response charge  :" << std::setw(6) << std::setprecision(2) << sr_charge.at(i));
        ATH_MSG_DEBUG("MicroMegas Digit, chip response position:" << std::setw(6) << std::setprecision(2) << stripPosition.at(i)
                                 << ",  strip response position:" << std::setw(6) << std::setprecision(2) << sr_stripPosition.at(i));
      }


      m_NSWMM_dig_time->push_back(time);
      m_NSWMM_dig_charge->push_back(charge);
      m_NSWMM_dig_stripPosition->push_back(stripPosition);
      m_NSWMM_dig_stripLposX->push_back(localPosX);
      m_NSWMM_dig_stripLposY->push_back(localPosY);
      m_NSWMM_dig_stripGposX->push_back(globalPosX);
      m_NSWMM_dig_stripGposY->push_back(globalPosY);
      m_NSWMM_dig_stripGposZ->push_back(globalPosZ);


      m_NSWMM_dig_sr_time->push_back(sr_time);
      m_NSWMM_dig_sr_charge->push_back(sr_charge);
      m_NSWMM_dig_sr_stripPosition->push_back(sr_stripPosition);
      m_NSWMM_dig_sr_stripLposX->push_back(sr_localPosX);
      m_NSWMM_dig_sr_stripLposY->push_back(sr_localPosY);
      m_NSWMM_dig_sr_stripGposX->push_back(sr_globalPosX);
      m_NSWMM_dig_sr_stripGposY->push_back(sr_globalPosY);
      m_NSWMM_dig_sr_stripGposZ->push_back(sr_globalPosZ);


      // retrieve the MC truth associated with the digit
      if (nsw_MmSdoContainer) {
        const MuonSimData mm_sdo = (nsw_MmSdoContainer->find(Id))->second;
        std::vector<MuonSimData::Deposit> deposits;
        mm_sdo.deposits(deposits);

        int    truth_barcode   = deposits[0].first.barcode();
        double truth_localPosX = deposits[0].second.firstEntry();
        double truth_localPosY = deposits[0].second.secondEntry();
        float  truth_angle     = mm_sdo.word()/1000.;

        Amg::Vector2D hit_on_surface(truth_localPosX, truth_localPosY);
        Amg::Vector3D hit_gpos(0., 0., 0.);
        rdoEl->surface(Id).localToGlobal(hit_on_surface, Amg::Vector3D(0., 0., 0.), hit_gpos);

        ATH_MSG_DEBUG("MicroMegas Digit, truth barcode=" << truth_barcode);
        ATH_MSG_DEBUG("MicroMegas Digit, truth localPosX=" << std::setw(9) << std::setprecision(2) << truth_localPosX
                                   << ", truth localPosY=" << std::setw(9) << std::setprecision(2) << truth_localPosY
                                    << ", truth XZ angle=" << std::setw(8) << std::setprecision(5) << truth_angle);

        m_NSWMM_dig_truth_barcode->push_back( truth_barcode );
        m_NSWMM_dig_truth_localPosX->push_back( truth_localPosX );
        m_NSWMM_dig_truth_localPosY->push_back( truth_localPosY );
        m_NSWMM_dig_truth_XZ_angle->push_back( truth_angle );

        m_NSWMM_dig_truth_globalPosX->push_back( hit_gpos[0] );
        m_NSWMM_dig_truth_globalPosY->push_back( hit_gpos[1] );
        m_NSWMM_dig_truth_globalPosZ->push_back( hit_gpos[2] );

      }

      m_NSWMM_dig_stripForTrigger->push_back(digit->stripForTrigger());
      m_NSWMM_dig_stripTimeForTrigger->push_back(digit->stripTimeForTrigger());

      m_NSWMM_nDigits++;
    }
  }

  ATH_MSG_INFO("processed " << m_NSWMM_nDigits << " MicroMegas hits");
  return StatusCode::SUCCESS;
}

void NSWPRDValAlg::deleteNSWsTGCHitVariables() 
{ 
  delete m_NSWsTGC_trackId;
  delete m_NSWsTGC_truthEl;
  delete m_NSWsTGC_globalTime;
  delete m_NSWsTGC_hitGlobalPositionX;
  delete m_NSWsTGC_hitGlobalPositionY;
  delete m_NSWsTGC_hitGlobalPositionZ;
  delete m_NSWsTGC_hitGlobalPositionR;
  delete m_NSWsTGC_hitGlobalPositionP;
  delete m_NSWsTGC_hitGlobalDirectionX;
  delete m_NSWsTGC_hitGlobalDirectionY;
  delete m_NSWsTGC_hitGlobalDirectionZ;

  delete m_NSWsTGC_hitLocalPositionX;
  delete m_NSWsTGC_hitLocalPositionY;
  delete m_NSWsTGC_hitLocalPositionZ;

  delete m_NSWsTGC_detector_globalPositionX;
  delete m_NSWsTGC_detector_globalPositionY;
  delete m_NSWsTGC_detector_globalPositionZ;
  delete m_NSWsTGC_detector_globalPositionR;
  delete m_NSWsTGC_detector_globalPositionP;

  delete m_NSWsTGC_hitToDsurfacePositionX;
  delete m_NSWsTGC_hitToDsurfacePositionY;
  delete m_NSWsTGC_hitToDsurfacePositionZ;

  delete m_NSWsTGC_hitToRsurfacePositionX;
  delete m_NSWsTGC_hitToRsurfacePositionY;
  delete m_NSWsTGC_hitToRsurfacePositionZ;

  delete m_NSWsTGC_FastDigitRsurfacePositionX;
  delete m_NSWsTGC_FastDigitRsurfacePositionY;


  delete m_NSWsTGC_particleEncoding;
  delete m_NSWsTGC_kineticEnergy;
  delete m_NSWsTGC_depositEnergy;
  delete m_NSWsTGC_StepLength;


  delete m_NSWsTGC_sim_stationName;
  delete m_NSWsTGC_sim_stationEta;
  delete m_NSWsTGC_sim_stationPhi;
  delete m_NSWsTGC_wedgeId;
  delete m_NSWsTGC_wedgeType;
  delete m_NSWsTGC_detectorNumber;
  delete m_NSWsTGC_sim_multilayer;
  delete m_NSWsTGC_sim_layer;
  delete m_NSWsTGC_sim_side;
  delete m_NSWsTGC_stripNumber;
  delete m_NSWsTGC_wireNumber;

  delete m_NSWsTGC_off_stationName;
  delete m_NSWsTGC_off_stationEta;
  delete m_NSWsTGC_off_stationPhi;
  delete m_NSWsTGC_off_multiplet;
  delete m_NSWsTGC_off_gas_gap;
  delete m_NSWsTGC_off_channel_type;
  delete m_NSWsTGC_off_channel;
 


  m_NSWsTGC_nSimHits = 0;
  m_NSWsTGC_trackId  = 0;
  m_NSWsTGC_truthEl  = 0;

  m_NSWsTGC_globalTime = 0;
  m_NSWsTGC_hitGlobalPositionX = 0;
  m_NSWsTGC_hitGlobalPositionY = 0;
  m_NSWsTGC_hitGlobalPositionZ = 0;
  m_NSWsTGC_hitGlobalPositionR = 0;
  m_NSWsTGC_hitGlobalPositionP = 0;
  m_NSWsTGC_hitGlobalDirectionX = 0;
  m_NSWsTGC_hitGlobalDirectionY = 0;
  m_NSWsTGC_hitGlobalDirectionZ = 0;

  m_NSWsTGC_hitLocalPositionX = 0;
  m_NSWsTGC_hitLocalPositionY = 0;
  m_NSWsTGC_hitLocalPositionZ = 0;

  m_NSWsTGC_detector_globalPositionX = 0;
  m_NSWsTGC_detector_globalPositionY = 0;
  m_NSWsTGC_detector_globalPositionZ = 0;
  m_NSWsTGC_detector_globalPositionR = 0;
  m_NSWsTGC_detector_globalPositionP = 0;

  m_NSWsTGC_hitToDsurfacePositionX = 0;
  m_NSWsTGC_hitToDsurfacePositionY = 0;
  m_NSWsTGC_hitToDsurfacePositionZ = 0;

  m_NSWsTGC_hitToRsurfacePositionX = 0;
  m_NSWsTGC_hitToRsurfacePositionY = 0;
  m_NSWsTGC_hitToRsurfacePositionZ = 0;

  m_NSWsTGC_FastDigitRsurfacePositionX = 0;
  m_NSWsTGC_FastDigitRsurfacePositionY = 0;

  m_NSWsTGC_particleEncoding = 0;
  m_NSWsTGC_kineticEnergy = 0;
  m_NSWsTGC_depositEnergy = 0;
  m_NSWsTGC_StepLength = 0;

  m_NSWsTGC_sim_stationName =0;
  m_NSWsTGC_sim_stationEta=0;
  m_NSWsTGC_sim_stationPhi=0;
  m_NSWsTGC_wedgeId=0;
  m_NSWsTGC_wedgeType=0;
  m_NSWsTGC_detectorNumber=0;
  m_NSWsTGC_sim_multilayer=0;
  m_NSWsTGC_sim_layer=0;
  m_NSWsTGC_sim_side=0;
  m_NSWsTGC_stripNumber=0;
  m_NSWsTGC_wireNumber=0;

  m_NSWsTGC_off_stationName  = 0;
  m_NSWsTGC_off_stationEta   = 0;
  m_NSWsTGC_off_stationPhi   = 0;
  m_NSWsTGC_off_multiplet    = 0;
  m_NSWsTGC_off_gas_gap      = 0;
  m_NSWsTGC_off_channel_type = 0;
  m_NSWsTGC_off_channel      = 0;

}

void NSWPRDValAlg::deleteNSWsTGCFastDigitVariables()
{
  delete m_NSWsTGC_fdg_stationName;
  delete m_NSWsTGC_fdg_stationEta;
  delete m_NSWsTGC_fdg_stationPhi;
  delete m_NSWsTGC_fdg_multiplet;
  delete m_NSWsTGC_fdg_gas_gap;
  delete m_NSWsTGC_fdg_channel_type;
  delete m_NSWsTGC_fdg_channel;

  delete m_NSWsTGC_fdg_globalPosX;
  delete m_NSWsTGC_fdg_globalPosY;
  delete m_NSWsTGC_fdg_globalPosZ;

  delete m_NSWsTGC_fdg_localPosX;
  delete m_NSWsTGC_fdg_localPosY;
  delete m_NSWsTGC_fdg_covMatrix_1_1;
  delete m_NSWsTGC_fdg_covMatrix_2_2;

  delete m_NSWsTGC_fdg_truth_barcode;
  delete m_NSWsTGC_fdg_truth_hitOnSurfaceX;
  delete m_NSWsTGC_fdg_truth_hitOnSurfaceY;


  m_NSWsTGC_nFastDigits = 0;
  m_NSWsTGC_fdg_stationName   = 0;
  m_NSWsTGC_fdg_stationEta    = 0;
  m_NSWsTGC_fdg_stationPhi    = 0;
  m_NSWsTGC_fdg_multiplet     = 0;
  m_NSWsTGC_fdg_gas_gap       = 0;
  m_NSWsTGC_fdg_channel_type  = 0;
  m_NSWsTGC_fdg_channel       = 0;

  m_NSWsTGC_fdg_globalPosX    = 0;
  m_NSWsTGC_fdg_globalPosY    = 0;
  m_NSWsTGC_fdg_globalPosZ    = 0;

  m_NSWsTGC_fdg_localPosX     = 0;
  m_NSWsTGC_fdg_localPosY     = 0;
  m_NSWsTGC_fdg_covMatrix_1_1 = 0;
  m_NSWsTGC_fdg_covMatrix_2_2 = 0;

  m_NSWsTGC_fdg_truth_barcode       = 0;
  m_NSWsTGC_fdg_truth_hitOnSurfaceX = 0 ;
  m_NSWsTGC_fdg_truth_hitOnSurfaceY = 0;

}

void NSWPRDValAlg::deleteNSWsTGCDigitVariables()
{
}


void NSWPRDValAlg::deleteNSWMMHitVariables() 
{ 
  delete m_NSWMM_globalTime;
  delete m_NSWMM_hitGlobalPositionX;
  delete m_NSWMM_hitGlobalPositionY;
  delete m_NSWMM_hitGlobalPositionZ;
  delete m_NSWMM_hitGlobalPositionR;
  delete m_NSWMM_hitGlobalPositionP;
  delete m_NSWMM_hitGlobalDirectionX;
  delete m_NSWMM_hitGlobalDirectionY;
  delete m_NSWMM_hitGlobalDirectionZ;

  delete m_NSWMM_hitLocalPositionX;
  delete m_NSWMM_hitLocalPositionY;
  delete m_NSWMM_hitLocalPositionZ;

  delete m_NSWMM_detector_globalPositionX;
  delete m_NSWMM_detector_globalPositionY;
  delete m_NSWMM_detector_globalPositionZ;
  delete m_NSWMM_detector_globalPositionR;
  delete m_NSWMM_detector_globalPositionP;

  delete m_NSWMM_hitToDsurfacePositionX;
  delete m_NSWMM_hitToDsurfacePositionY;
  delete m_NSWMM_hitToDsurfacePositionZ;

  delete m_NSWMM_hitToRsurfacePositionX;
  delete m_NSWMM_hitToRsurfacePositionY;
  delete m_NSWMM_hitToRsurfacePositionZ;

  delete m_NSWMM_FastDigitRsurfacePositionX;
  delete m_NSWMM_FastDigitRsurfacePositionY;


  delete m_NSWMM_particleEncoding;
  delete m_NSWMM_kineticEnergy;
  delete m_NSWMM_depositEnergy;
  delete m_NSWMM_StepLength;

  delete m_NSWMM_sim_stationName;
  delete m_NSWMM_sim_stationEta;
  delete m_NSWMM_sim_stationPhi;
  delete m_NSWMM_sim_multilayer;
  delete m_NSWMM_sim_layer;
  delete m_NSWMM_sim_side;

  delete m_NSWMM_off_stationName;
  delete m_NSWMM_off_stationEta;
  delete m_NSWMM_off_stationPhi;
  delete m_NSWMM_off_multiplet;
  delete m_NSWMM_off_gas_gap;
  delete m_NSWMM_off_channel;



  m_NSWMM_nSimHits = 0;
  m_NSWMM_globalTime = 0;
  m_NSWMM_hitGlobalPositionX = 0;
  m_NSWMM_hitGlobalPositionY = 0;
  m_NSWMM_hitGlobalPositionZ = 0;
  m_NSWMM_hitGlobalPositionR = 0;
  m_NSWMM_hitGlobalPositionP = 0;

  m_NSWMM_hitGlobalDirectionX = 0;
  m_NSWMM_hitGlobalDirectionY = 0;
  m_NSWMM_hitGlobalDirectionZ = 0;

  m_NSWMM_hitLocalPositionX = 0;
  m_NSWMM_hitLocalPositionY = 0;
  m_NSWMM_hitLocalPositionZ = 0;

  m_NSWMM_detector_globalPositionX = 0;
  m_NSWMM_detector_globalPositionY = 0;
  m_NSWMM_detector_globalPositionZ = 0;
  m_NSWMM_detector_globalPositionR = 0;
  m_NSWMM_detector_globalPositionP = 0;

  m_NSWMM_hitToDsurfacePositionX = 0;
  m_NSWMM_hitToDsurfacePositionY = 0;
  m_NSWMM_hitToDsurfacePositionZ = 0;

  m_NSWMM_hitToRsurfacePositionX = 0;
  m_NSWMM_hitToRsurfacePositionY = 0;
  m_NSWMM_hitToRsurfacePositionZ = 0;

  m_NSWMM_FastDigitRsurfacePositionX = 0;
  m_NSWMM_FastDigitRsurfacePositionY = 0;


  m_NSWMM_particleEncoding = 0;
  m_NSWMM_kineticEnergy = 0;
  m_NSWMM_depositEnergy = 0;
  m_NSWMM_StepLength = 0;


  m_NSWMM_sim_stationName = 0;
  m_NSWMM_sim_stationEta = 0;
  m_NSWMM_sim_stationPhi = 0;
  m_NSWMM_sim_multilayer = 0;
  m_NSWMM_sim_layer = 0;
  m_NSWMM_sim_side = 0;

  m_NSWMM_off_stationName = 0;
  m_NSWMM_off_stationEta = 0;
  m_NSWMM_off_stationPhi = 0;
  m_NSWMM_off_multiplet = 0;
  m_NSWMM_off_gas_gap = 0;
  m_NSWMM_off_channel = 0;
}

void NSWPRDValAlg::deleteNSWMMFastDigitVariables()
{
  delete m_NSWMM_fdg_stationName;
  delete m_NSWMM_fdg_stationEta;
  delete m_NSWMM_fdg_stationPhi;
  delete m_NSWMM_fdg_multiplet;
  delete m_NSWMM_fdg_gas_gap;
  delete m_NSWMM_fdg_channel;

  delete m_NSWMM_fdg_globalPosX;
  delete m_NSWMM_fdg_globalPosY;
  delete m_NSWMM_fdg_globalPosZ;

  delete m_NSWMM_fdg_localPosX;
  delete m_NSWMM_fdg_localPosY;
  delete m_NSWMM_fdg_covMatrix_1_1;

  delete m_NSWMM_fdg_truth_barcode;
  delete m_NSWMM_fdg_truth_hitOnSurfaceX;
  delete m_NSWMM_fdg_truth_hitOnSurfaceY;

  m_NSWMM_nFastDigits    = 0;
  m_NSWMM_fdg_stationName= 0;
  m_NSWMM_fdg_stationEta = 0;
  m_NSWMM_fdg_stationPhi = 0;
  m_NSWMM_fdg_multiplet  = 0;
  m_NSWMM_fdg_gas_gap    = 0;
  m_NSWMM_fdg_channel    = 0;

  m_NSWMM_fdg_globalPosX = 0;
  m_NSWMM_fdg_globalPosY = 0;
  m_NSWMM_fdg_globalPosZ = 0;

  m_NSWMM_fdg_localPosX  = 0;
  m_NSWMM_fdg_localPosY  = 0;
  m_NSWMM_fdg_covMatrix_1_1 = 0;

  m_NSWMM_fdg_truth_barcode       = 0;
  m_NSWMM_fdg_truth_hitOnSurfaceX = 0 ; 
  m_NSWMM_fdg_truth_hitOnSurfaceY = 0;;
}

void NSWPRDValAlg::deleteNSWMMDigitVariables()
{
  delete m_NSWMM_dig_stationName;
  delete m_NSWMM_dig_stationEta;
  delete m_NSWMM_dig_stationPhi;
  delete m_NSWMM_dig_multiplet;
  delete m_NSWMM_dig_gas_gap;
  delete m_NSWMM_dig_channel;
  
  delete m_NSWMM_dig_time;
  delete m_NSWMM_dig_charge;
  delete m_NSWMM_dig_stripPosition;
  delete m_NSWMM_dig_stripLposX;
  delete m_NSWMM_dig_stripLposY;
  delete m_NSWMM_dig_stripGposX;
  delete m_NSWMM_dig_stripGposY;
  delete m_NSWMM_dig_stripGposZ;

  delete m_NSWMM_dig_sr_time;
  delete m_NSWMM_dig_sr_charge;
  delete m_NSWMM_dig_sr_stripPosition;
  delete m_NSWMM_dig_sr_stripLposX;
  delete m_NSWMM_dig_sr_stripLposY;
  delete m_NSWMM_dig_sr_stripGposX;
  delete m_NSWMM_dig_sr_stripGposY;
  delete m_NSWMM_dig_sr_stripGposZ;


  delete m_NSWMM_dig_truth_barcode;
  delete m_NSWMM_dig_truth_localPosX;
  delete m_NSWMM_dig_truth_localPosY;
  delete m_NSWMM_dig_truth_XZ_angle;
  delete m_NSWMM_dig_truth_globalPosX;
  delete m_NSWMM_dig_truth_globalPosY;
  delete m_NSWMM_dig_truth_globalPosZ;


  delete m_NSWMM_dig_stripForTrigger;
  delete m_NSWMM_dig_stripTimeForTrigger;

  m_NSWMM_nDigits = 0;
  m_NSWMM_dig_stationName = 0;
  m_NSWMM_dig_stationEta = 0;
  m_NSWMM_dig_stationPhi = 0;
  m_NSWMM_dig_multiplet = 0;
  m_NSWMM_dig_gas_gap = 0;
  m_NSWMM_dig_channel = 0;
  
  m_NSWMM_dig_time = 0;
  m_NSWMM_dig_charge = 0;
  m_NSWMM_dig_stripPosition = 0;
  m_NSWMM_dig_stripLposX = 0;
  m_NSWMM_dig_stripLposY = 0;
  m_NSWMM_dig_stripGposX = 0;
  m_NSWMM_dig_stripGposY = 0;
  m_NSWMM_dig_stripGposZ = 0;

  m_NSWMM_dig_sr_time = 0;
  m_NSWMM_dig_sr_charge = 0;
  m_NSWMM_dig_sr_stripPosition = 0;
  m_NSWMM_dig_sr_stripLposX = 0;
  m_NSWMM_dig_sr_stripLposY = 0;
  m_NSWMM_dig_sr_stripGposX = 0;
  m_NSWMM_dig_sr_stripGposY = 0;
  m_NSWMM_dig_sr_stripGposZ = 0;


  m_NSWMM_dig_truth_barcode    = 0;
  m_NSWMM_dig_truth_localPosX  = 0;
  m_NSWMM_dig_truth_localPosY  = 0;
  m_NSWMM_dig_truth_XZ_angle   = 0;
  m_NSWMM_dig_truth_globalPosX = 0;
  m_NSWMM_dig_truth_globalPosY = 0;
  m_NSWMM_dig_truth_globalPosZ = 0;


  m_NSWMM_dig_stripForTrigger = 0;
  m_NSWMM_dig_stripTimeForTrigger = 0;
}
