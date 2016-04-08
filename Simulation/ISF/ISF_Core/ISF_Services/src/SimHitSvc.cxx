/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "SimHitSvc.h"

// Athena includes
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArMiniFCAL_ID.h"
#include "CaloSimEvent/CaloCalibrationHitContainer.h"
#include "GeneratorObjects/HepMcParticleLink.h"
#include "SimHelpers/AthenaHitsCollectionHelper.h"
#include "StoreGate/StoreGateSvc.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// ROOT includes
#include "TTree.h"

/** Constructor **/
ISF::SimHitSvc::SimHitSvc(const std::string& name,ISvcLocator* svc) :
  AthService(name,svc),
  m_validationOutput(false),
  m_thistSvc("THistSvc",name),
  m_validationStream("ISFSimHit"),
  m_t_simHits(nullptr),
  m_mdt_n(-1),
  m_rpc_n(-1),
  m_tgc_n(-1),
  m_csc_n(-1),
  m_pileup(-1),
  m_type(-1),
  m_id(-1),
  m_mother(-1),
  m_barcode(-1),
  m_time(-1.),
  m_drift(-1.),
  m_edeposit(-1.),
  m_momentum(-1.),
  m_theta(-1.),
  m_phi(-1.),
  m_eta(-1.),
  m_hitCollectionHelper(new AthenaHitsCollectionHelper),
  m_simulateID(true),
  m_bcmHits(nullptr),
  m_bcmHitCollectionName("BCMHits"),
  m_blmHits(nullptr),
  m_blmHitCollectionName("BLMHits"),
  m_pixHits(nullptr),
  m_pixHitCollectionName("PixelHits"),
  m_sctHits(nullptr),
  m_sctHitCollectionName("SCT_Hits"),
  m_trtHits(nullptr),
  m_trtHitCollectionName("TRTUncompressedHits"),
  m_pixPileupHits(0),
  m_pixPileupHitCollectionName("PileupPixelHits"),
  m_sctPileupHits(0),
  m_sctPileupHitCollectionName("PileupSCT_Hits"),
  m_trtPileupHits(0),
  m_trtPileupHitCollectionName("PileupTRTUncompressedHits"),
  m_separateInDetPileupHits(false),
  m_simulateCalo(true),
  m_embHitCollectionName("LArHitEMB"),
  m_emecHitCollectionName("LArHitEMEC"),
  m_fcalHitCollectionName("LArHitFCAL"),
  m_hecHitCollectionName("LArHitHEC"),
  m_miniFcalHitCollectionName("LArHitMiniFCAL"),
  m_storedCalibContainers(nullptr),
  m_activeLArHitCollectionName("LArCalibrationHitActive"),
  m_inactiveLArHitCollectionName("LArCalibrationHitInactive"),
  m_deadLArHitCollectionName("LArCalibrationHitDeadMaterial"),
  m_tileHitVecHits(nullptr),
  m_tileHitVecHitCollectionName("TileHitVec"),
  m_mbtsHits(nullptr),
  m_mbtsHitCollectionName("MBTSHits"),
  m_tileActiveCellCalibHits(nullptr),
  m_tileActiveCellCalibHitCollectionName("TileCalibHitActiveCell"),
  m_tileInactiveCellCalibHits(nullptr),
  m_tileInactiveCellCalibHitCollectionName("TileCalibHitInactiveCell"),
  m_tileDeadMaterialCalibHits(nullptr),
  m_tileDeadMaterialCalibHitCollectionName("TileCalibHitDeadMaterial"),
  m_doMiniFcal(false),
  m_doTileCalibHits(false),
  m_simulateMS(true),
  m_mdtHits(nullptr),
  m_mdtHitCollectionName("MDT_Hits"),
  m_rpcHits(nullptr),
  m_rpcHitCollectionName("RPC_Hits"),
  m_tgcHits(nullptr),
  m_tgcHitCollectionName("TGC_Hits"),
  m_cscHits(nullptr),
  m_cscHitCollectionName("CSC_Hits"),
  m_muonHits(nullptr),
  m_muonHitCollectionName("GenericMuon_Hits"),
  m_caloEntryLayerTracks(nullptr),
  m_caloEntryLayerTrackCollectionName("CaloEntryLayer"),
  m_muonEntryLayerTracks(nullptr),
  m_muonEntryLayerTrackCollectionName("MuonEntryLayer"),
  m_muonExitLayerTracks(nullptr),
  m_muonExitLayerTrackCollectionName("MuonExitLayer"),
  m_cosmicPerigeeTracks(nullptr),
  m_cosmicPerigeeTrackCollectionName("CosmicPerigee"),
  m_senDetTool("SensitiveDetectorMasterTool"),
  m_fastSimTool("FastSimulationMasterTool")
{
  // validation output section
  declareProperty( "ValidationOutput",
                   m_validationOutput = false,
                   "If turned on, write out a ROOT tree.");
  declareProperty("ValidationStreamName",
                  m_validationStream = "ISFSimHit",
                  "Name of the output stream" );
  declareProperty("THistService",
                  m_thistSvc,
                  "The THistSvc" );

  declareProperty("SimulateID",         m_simulateID );
  declareProperty("BCM_HitCollection",  m_bcmHitCollectionName );
  declareProperty("BLM_HitCollection",  m_blmHitCollectionName );
  declareProperty("PixelHitCollection", m_pixHitCollectionName );
  declareProperty("SCT_HitCollection",  m_sctHitCollectionName );
  declareProperty("TRT_HitCollection",  m_trtHitCollectionName );
  declareProperty("PileupPixelHitCollection", m_pixPileupHitCollectionName );
  declareProperty("PileupSCT_HitCollection",  m_sctPileupHitCollectionName );
  declareProperty("PileupTRT_HitCollection",  m_trtPileupHitCollectionName );
  declareProperty("SeparateInDetPileupHits", m_separateInDetPileupHits);

  declareProperty("SimulateCalo",              m_simulateCalo );
  declareProperty("EMBLAr_HitCollection",      m_embHitCollectionName );
  declareProperty("EMECLAr_HitCollection",     m_emecHitCollectionName );
  declareProperty("FCAL_HitCollection",        m_fcalHitCollectionName );
  declareProperty("HEC_HitCollection",         m_hecHitCollectionName );
  declareProperty("MiniFCAL_HitCollection",    m_miniFcalHitCollectionName );
  declareProperty("ActiveLAr_HitCollection",   m_activeLArHitCollectionName );
  declareProperty("InactiveLAr_HitCollection", m_inactiveLArHitCollectionName );
  declareProperty("DeadMatlLAr_HitCollection", m_deadLArHitCollectionName );
  declareProperty("TileHitVec_HitCollection",  m_tileHitVecHitCollectionName );
  declareProperty("MBTS_HitCollection",        m_mbtsHitCollectionName );
  declareProperty("TileActiveCellCalibHitCollection",   m_tileActiveCellCalibHitCollectionName );
  declareProperty("TileInactiveCellCalibHitCollection", m_tileInactiveCellCalibHitCollectionName );
  declareProperty("TileDeadMaterialCalibHitCollection", m_tileDeadMaterialCalibHitCollectionName );
  declareProperty("DoTileCalibHits", m_doTileCalibHits );

  declareProperty("SimulateMS",         m_simulateMS );
  declareProperty("MDT_HitCollection",  m_mdtHitCollectionName );
  declareProperty("RPC_HitCollection",  m_rpcHitCollectionName );
  declareProperty("TGC_HitCollection",  m_tgcHitCollectionName );
  declareProperty("CSC_HitCollection",  m_cscHitCollectionName );
  declareProperty("GenericMuon_HitCollection",  m_muonHitCollectionName );

  declareProperty("CaloEntryLayer_TrackCollection",  m_caloEntryLayerTrackCollectionName );
  declareProperty("MuonEntryLayer_TrackCollection",  m_muonEntryLayerTrackCollectionName );
  declareProperty("MuonExitLayer_TrackCollection",   m_muonExitLayerTrackCollectionName );

  declareProperty("CosmicPerigee_TrackCollection",   m_cosmicPerigeeTrackCollectionName );

  declareProperty("SensitiveDetectorMasterTool", m_senDetTool );
  declareProperty("FastSimulationMasterTool", m_fastSimTool );
}


ISF::SimHitSvc::~SimHitSvc()
{
  delete m_hitCollectionHelper;

}

/** framework methods */
StatusCode ISF::SimHitSvc::initialize()
{
  if(m_simulateCalo)
    {
      ISvcLocator* svcLocator = Gaudi::svcLocator();
      StoreGateSvc* detStore;
      CHECK(svcLocator->service("DetectorStore", detStore));
      // Check whether or not we need to write out Mini FCAL hits
      const CaloIdManager* caloIdManager;
      CHECK(detStore->retrieve(caloIdManager));
      const LArMiniFCAL_ID* larMiniFcalID = caloIdManager->getMiniFCAL_ID();
      m_doMiniFcal = (larMiniFcalID && larMiniFcalID->is_initialized());
    }

  // Global containers
  m_storedCalibContainers = new StoredLArCalibHitContainers();

  // setup for validation mode
  if ( m_validationOutput)
    {
      CHECK(this->createSimHitsTree());
    }

  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::SimHitSvc::finalize()
{
  //FIXME really?!
  m_storedCalibContainers = new StoredLArCalibHitContainers();

  return StatusCode::SUCCESS;
}

/** Initialize event */
StatusCode ISF::SimHitSvc::initializeEvent()
{
  ATH_MSG_DEBUG("initializing hit collections");

  if(!m_senDetTool)
    {
      CHECK(m_senDetTool.retrieve());
    }
  CHECK(m_senDetTool->BeginOfAthenaEvent());


  // Inner Detector
  m_bcmHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_bcmHitCollectionName);
  m_blmHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_blmHitCollectionName);
  m_pixHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_pixHitCollectionName);
  m_sctHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_sctHitCollectionName);
  m_trtHits = m_hitCollectionHelper->RetrieveNonconstCollection<TRTUncompressedHitCollection>(m_trtHitCollectionName);
  if(m_separateInDetPileupHits)
    {
      m_pixPileupHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_pixPileupHitCollectionName);
      m_sctPileupHits = m_hitCollectionHelper->RetrieveNonconstCollection<SiHitCollection>(m_sctPileupHitCollectionName);
      m_trtPileupHits = m_hitCollectionHelper->RetrieveNonconstCollection<TRTUncompressedHitCollection>(m_trtPileupHitCollectionName);
    }

  // LAr CaloCalibarationHit Containers
  m_storedCalibContainers->activeHitCollection   = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_activeLArHitCollectionName);
  m_storedCalibContainers->inactiveHitCollection = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_inactiveLArHitCollectionName);
  m_storedCalibContainers->deadHitCollection     = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_deadLArHitCollectionName);

  // Tile Calorimeter
  m_tileHitVecHits = m_hitCollectionHelper->RetrieveNonconstCollection<TileHitVector>(m_tileHitVecHitCollectionName);
  m_mbtsHits       = m_hitCollectionHelper->RetrieveNonconstCollection<TileHitVector>(m_mbtsHitCollectionName);

  // Tile CaloCalibrationHit Containers
  if (m_doTileCalibHits)
    {
      m_tileActiveCellCalibHits   = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_tileActiveCellCalibHitCollectionName);
      m_tileInactiveCellCalibHits = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_tileInactiveCellCalibHitCollectionName);
      m_tileDeadMaterialCalibHits = m_hitCollectionHelper->RetrieveNonconstCollection<CaloCalibrationHitContainer>(m_tileDeadMaterialCalibHitCollectionName);
    }

  // Muon Spectrometer
  m_mdtHits  = m_hitCollectionHelper->RetrieveNonconstCollection<MDTSimHitCollection>(m_mdtHitCollectionName);
  m_rpcHits  = m_hitCollectionHelper->RetrieveNonconstCollection<RPCSimHitCollection>(m_rpcHitCollectionName);
  m_tgcHits  = m_hitCollectionHelper->RetrieveNonconstCollection<TGCSimHitCollection>(m_tgcHitCollectionName);
  m_cscHits  = m_hitCollectionHelper->RetrieveNonconstCollection<CSCSimHitCollection>(m_cscHitCollectionName);
  m_muonHits = m_hitCollectionHelper->RetrieveNonconstCollection<GenericMuonSimHitCollection>(m_muonHitCollectionName);

  // TrackRecordCollections
  m_caloEntryLayerTracks = m_hitCollectionHelper->RetrieveNonconstCollection<TrackRecordCollection>(m_caloEntryLayerTrackCollectionName);
  m_muonEntryLayerTracks = m_hitCollectionHelper->RetrieveNonconstCollection<TrackRecordCollection>(m_muonEntryLayerTrackCollectionName);
  m_muonExitLayerTracks  = m_hitCollectionHelper->RetrieveNonconstCollection<TrackRecordCollection>(m_muonExitLayerTrackCollectionName);

  m_cosmicPerigeeTracks  = m_hitCollectionHelper->RetrieveNonconstCollection<TrackRecordCollection>(m_cosmicPerigeeTrackCollectionName);

  // Forward Detectors
  // FIXME add Forward Detectors hit collections here

  return StatusCode::SUCCESS;
}

/** Simulation Call --- simply kills the particle */
StatusCode ISF::SimHitSvc::releaseEvent()
{

  if(!m_senDetTool)
    {
      CHECK(m_senDetTool.retrieve());
    }
  CHECK(m_senDetTool->EndOfAthenaEvent());
  if(!m_fastSimTool)
    {
      CHECK(m_fastSimTool.retrieve());
    }
  CHECK(m_fastSimTool->EndOfAthenaEvent());

  if (m_validationOutput)
    {
      this->fillSimHitsTree();
    }

  // set all collections to const
  // Inner Detector
  m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_bcmHits);
  m_bcmHits=0;
  m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_blmHits);
  m_blmHits=0;
  m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_pixHits);
  m_pixHits=0;
  m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_sctHits);
  m_sctHits=0;
  m_hitCollectionHelper->SetConstCollection<TRTUncompressedHitCollection>(m_trtHits);
  m_trtHits=0;
  if(m_separateInDetPileupHits)
    {
      m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_pixPileupHits);
      m_pixPileupHits=0;
      m_hitCollectionHelper->SetConstCollection<SiHitCollection>(m_sctPileupHits);
      m_sctPileupHits=0;
      m_hitCollectionHelper->SetConstCollection<TRTUncompressedHitCollection>(m_trtPileupHits);
      m_trtPileupHits=0;
    }


  // LAr CaloCalibarationHit Containers
  m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->activeHitCollection);
  m_storedCalibContainers->activeHitCollection=0;
  m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->inactiveHitCollection);
  m_storedCalibContainers->inactiveHitCollection=0;
  m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_storedCalibContainers->deadHitCollection);
  m_storedCalibContainers->deadHitCollection=0;

  // Tile Calorimeter
  m_hitCollectionHelper->SetConstCollection< TileHitVector >(m_tileHitVecHits);
  m_tileHitVecHits=0;
  m_hitCollectionHelper->SetConstCollection< TileHitVector >(m_mbtsHits);
  m_mbtsHits=0;

  // Tile CaloCalibrationHit Containers
  if (m_doTileCalibHits)
    {
      m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_tileActiveCellCalibHits);
      m_tileActiveCellCalibHits=0;
      m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_tileInactiveCellCalibHits);
      m_tileInactiveCellCalibHits=0;
      m_hitCollectionHelper->SetConstCollection< CaloCalibrationHitContainer >(m_tileDeadMaterialCalibHits);
      m_tileDeadMaterialCalibHits=0;
    }

  // Muon Spectrometer
  m_hitCollectionHelper->SetConstCollection< MDTSimHitCollection >(m_mdtHits);
  m_mdtHits=0;
  m_hitCollectionHelper->SetConstCollection< RPCSimHitCollection >(m_rpcHits);
  m_rpcHits=0;
  m_hitCollectionHelper->SetConstCollection< TGCSimHitCollection >(m_tgcHits);
  m_tgcHits=0;
  m_hitCollectionHelper->SetConstCollection< CSCSimHitCollection >(m_cscHits);
  m_cscHits=0;
  m_hitCollectionHelper->SetConstCollection< GenericMuonSimHitCollection >(m_muonHits);
  m_muonHits=0;

  // TrackRecordCollections
  m_hitCollectionHelper->SetConstCollection<TrackRecordCollection>(m_caloEntryLayerTracks);
  m_caloEntryLayerTracks = 0;
  m_hitCollectionHelper->SetConstCollection<TrackRecordCollection>(m_muonEntryLayerTracks);
  m_muonEntryLayerTracks = 0;
  m_hitCollectionHelper->SetConstCollection<TrackRecordCollection>(m_muonExitLayerTracks);
  m_muonExitLayerTracks = 0;

  m_hitCollectionHelper->SetConstCollection<TrackRecordCollection>(m_cosmicPerigeeTracks);
  m_cosmicPerigeeTracks = 0;

  // Forward Detectors
  // FIXME add Forward Detectors hit collections here

  return StatusCode::SUCCESS;
}

/** Create the simhits tree - validation mode only */
StatusCode ISF::SimHitSvc::createSimHitsTree()
{
  // retrieve the histogram service
  CHECK( m_thistSvc.retrieve().isSuccess() );

  // Create the prefix of histogram names for the THistSvc
  //std::string prefix = "/" + m_validationStream + "/";
  const char *treeName="simhits";
  const std::string prefix = "/" + m_validationStream + "/"+ treeName;
  m_t_simHits = new TTree( treeName, treeName );
  //m_t_simHits->Branch("mdtHits"    , &m_mdt_n    , "mdtHits/I"  );
  //m_t_simHits->Branch("rpcHits"    , &m_rpc_n    , "rpcHits/I"  );
  //m_t_simHits->Branch("tgcHits"    , &m_tgc_n    , "tgcHits/I"  );
  //m_t_simHits->Branch("cscHits"    , &m_csc_n    , "cscHits/I"  );
  m_t_simHits->Branch("pileup"     , &m_pileup   , "pileup/I"   );
  m_t_simHits->Branch("type"       , &m_type     , "type/I"     );
  m_t_simHits->Branch("id"         , &m_id       , "id/I"       );
  m_t_simHits->Branch("mother"     , &m_mother   , "mother/I"   );
  m_t_simHits->Branch("barcode"    , &m_barcode  , "barcode/I"  );
  m_t_simHits->Branch("momentum"   , &m_momentum , "momentum/F" );
  m_t_simHits->Branch("time"       , &m_time     , "time/F"     );
  m_t_simHits->Branch("drift"      , &m_drift    , "dist/F"     );
  m_t_simHits->Branch("edeposit"   , &m_edeposit , "edeposit/F" );
  m_t_simHits->Branch("theta"      , &m_theta    , "theta/F"    );
  m_t_simHits->Branch("phi"        , &m_phi      , "phi/F"      );
  m_t_simHits->Branch("eta"        , &m_eta      , "eta/F"      );

  // register the Tree to the THistSvc and return it's StatusCodes
  CHECK( m_thistSvc->regTree( prefix, m_t_simHits) );

  return StatusCode::SUCCESS;
}

/** Fill the simhit TTree - validation mode only */
void ISF::SimHitSvc::fillSimHitsTree()
{

  // loop over collections
  if (m_mdtHits->size()) {
    MDTSimHitCollection::const_iterator ih=m_mdtHits->begin();
    while ( ih!=m_mdtHits->end()) {
      m_type = 1;
      m_id = (*ih).MDTid();
      m_mother = (*ih).particleEncoding();
      m_time = (*ih).globalTime();
      m_drift = (*ih).driftRadius();
      m_edeposit = (*ih).energyDeposit();
      m_barcode = (*ih).trackNumber();
      HepMcParticleLink HMPL = (*ih).particleLink();
      this->addHepMcParticleLinkInfoToTree(HMPL);

      ih++;
      while (ih!=m_mdtHits->end() && m_id==(*ih).MDTid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }

  if (m_rpcHits->size()) {
    RPCSimHitCollection::const_iterator ih=m_rpcHits->begin();
    while (ih!=m_rpcHits->end()) {
      m_type = 2;
      m_id = (*ih).RPCid();
      m_mother = (*ih).particleEncoding();
      m_time = (*ih).globalTime();
      m_drift = 0.;
      m_edeposit = (*ih).energyDeposit();
      m_barcode = (*ih).trackNumber();
      HepMcParticleLink HMPL = (*ih).particleLink();
      this->addHepMcParticleLinkInfoToTree(HMPL);

      ih++;
      while (ih!=m_rpcHits->end() && m_id==(*ih).RPCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }
  if (m_tgcHits->size()) {
    TGCSimHitCollection::const_iterator ih=m_tgcHits->begin();
    while ( ih!=m_tgcHits->end()) {
      m_type = 3;
      m_id = (*ih).TGCid();
      m_mother = (*ih).particleEncoding();
      m_time = (*ih).globalTime();
      m_drift = 0.;
      m_edeposit = (*ih).energyDeposit();
      m_barcode = (*ih).trackNumber();
      HepMcParticleLink HMPL = (*ih).particleLink();
      this->addHepMcParticleLinkInfoToTree(HMPL);

      ih++;
      while (ih!=m_tgcHits->end() && m_id==(*ih).TGCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }
  if (m_cscHits->size()) {
    CSCSimHitCollection::const_iterator ih=m_cscHits->begin();
    while ( ih!=m_cscHits->end()) {
      m_type = 4;
      m_id = (*ih).CSCid();
      m_mother = (*ih).particleID();
      m_time = (*ih).globalTime();
      m_drift = 0.;
      m_edeposit = (*ih).energyDeposit();
      m_barcode = (*ih).trackNumber();
      HepMcParticleLink HMPL = (*ih).particleLink();
      this->addHepMcParticleLinkInfoToTree(HMPL);

      ih++;
      while (ih!=m_cscHits->end() && m_id==(*ih).CSCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }
  for (int ipileup=0;ipileup<2;ipileup++) {
    m_pileup = ipileup;
    // pixel hits
    SiHitCollection* pixHits=(ipileup==0) ? m_pixHits : m_pixPileupHits;

    if (pixHits->size()) {
      SiHitCollection::const_iterator ih=pixHits->begin();
      while (ih!=pixHits->end()) {
        m_type = 5;
        m_id = (*ih).identify();
        HepMcParticleLink HMPL = (*ih).particleLink();
        if (HMPL.isValid()) m_mother = (HMPL.cptr())->pdg_id();
        else m_mother=0;
        m_time = (*ih).meanTime();
        m_drift = 0.;
        m_edeposit = (*ih).energyLoss();
        m_barcode = (*ih).trackNumber();
        this->addHepMcParticleLinkInfoToTree(HMPL);

        ih++;
        while (ih!=pixHits->end() && ((unsigned int)m_id)==(*ih).identify() && m_barcode==(*ih).trackNumber() ) {
          // merge energy deposits and move on
          m_edeposit += (*ih).energyLoss();
          ih++;
        }
        m_t_simHits->Fill();
      }
    }
    SiHitCollection* sctHits=(ipileup==0) ? m_sctHits : m_sctPileupHits;
    if (sctHits->size()) {
      SiHitCollection::const_iterator ih=sctHits->begin();
      while (ih!=sctHits->end()) {
        m_type = 6;
        m_id = (*ih).identify();
        HepMcParticleLink HMPL = (*ih).particleLink();
        if (HMPL.isValid()) m_mother = (HMPL.cptr())->pdg_id();
        else m_mother=0;
        m_time = (*ih).meanTime();
        m_drift = 0.;
        m_edeposit = (*ih).energyLoss();
        m_barcode = (*ih).trackNumber();
        this->addHepMcParticleLinkInfoToTree(HMPL);

        ih++;
        while (ih!=sctHits->end() && ((unsigned int)m_id)==(*ih).identify() && m_barcode==(*ih).trackNumber() ) {
          // merge energy deposits and move on
          m_edeposit += (*ih).energyLoss();
          ih++;
        }
        m_t_simHits->Fill();
      }
    }
    TRTUncompressedHitCollection* trtHits = (ipileup==0) ? m_trtHits : m_trtPileupHits;
    if (trtHits->size()) {
      TRTUncompressedHitCollection::const_iterator ih=trtHits->begin();
      while ( ih!=trtHits->end()) {
        m_type = 7;
        m_id = (*ih).GetHitID();
        m_mother = (*ih).GetParticleEncoding();
        HepMcParticleLink HMPL = (*ih).particleLink();
        m_time = (*ih).GetGlobalTime();
        m_drift = 0.;
        m_edeposit = (*ih).GetEnergyDeposit();
        m_barcode = (*ih).GetTrackID();
        this->addHepMcParticleLinkInfoToTree(HMPL);

        ih++;
        while (ih!=trtHits->end() && m_id==(*ih).GetHitID() && m_barcode==(*ih).GetTrackID() ) {
          // merge energy deposits and move on
          m_edeposit += (*ih).GetEnergyDeposit();
          ih++;
        }
        m_t_simHits->Fill();
      }
    }
  }
  return;
}

//** Add information from HepMcParticleLink to TTree - common for all SimHit types */
void ISF::SimHitSvc::addHepMcParticleLinkInfoToTree(HepMcParticleLink &HMPL)
{
  if (HMPL.isValid()) {
    m_momentum = (HMPL.cptr())->momentum().rho();
    m_eta= (HMPL.cptr())->momentum().eta();
    m_theta= (HMPL.cptr())->momentum().theta();
    m_phi  = (HMPL.cptr())->momentum().phi();
  } else {
    m_momentum=-1.;
    m_theta=-1.;
    m_eta=-10.;
    m_phi = -10.;
  }
  return;
}

/** Query the interfaces. */
StatusCode ISF::SimHitSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){

  if ( IID_ISimHitSvc == riid )
    *ppvInterface = (ISimHitSvc*)this;
  else  {
    // Interface is not directly available: try out a base class
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void ISF::SimHitSvc::insert(const SiHit& )
{
  ATH_MSG_ERROR("NOT INSERTING THIS SIHIT");
  //do nothing
  throw;
}

void ISF::SimHitSvc::insert(const TRTHit& )
{
  ATH_MSG_ERROR("NOT INSERTING THIS TRT HIT");
  //do nothing
  throw;
}

void ISF::SimHitSvc::insert(const TRTUncompressedHit& )
{
  ATH_MSG_ERROR("NOT INSERTING THIS TRT HIT");
  //do nothing
  throw;
}
