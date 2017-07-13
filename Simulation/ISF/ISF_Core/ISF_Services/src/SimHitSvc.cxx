/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SimHitSvc.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

// class header include
#include "SimHitSvc.h"

// Athena includes
#include "GeneratorObjects/HepMcParticleLink.h"

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
  m_bcmHits("BCMHits"),
  m_blmHits("BLMHits"),
  m_pixHits("PixelHits"),
  m_sctHits("SCT_Hits"),
  m_trtHits("TRTUncompressedHits"),
  m_pixPileupHits("PileupPixelHits"),
  m_sctPileupHits("PileupSCT_Hits"),
  m_trtPileupHits("PileupTRTUncompressedHits"),
  m_separateInDetPileupHits(false),
  m_mdtHits("MDT_Hits"),
  m_rpcHits("RPC_Hits"),
  m_tgcHits("TGC_Hits"),
  m_cscHits("CSC_Hits"),
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

  declareProperty("BCM_HitCollection",  m_bcmHits );
  declareProperty("BLM_HitCollection",  m_blmHits );
  declareProperty("PixelHitCollection", m_pixHits );
  declareProperty("SCT_HitCollection",  m_sctHits );
  declareProperty("TRT_HitCollection",  m_trtHits );
  declareProperty("PileupPixelHitCollection", m_pixPileupHits );
  declareProperty("PileupSCT_HitCollection",  m_sctPileupHits );
  declareProperty("PileupTRT_HitCollection",  m_trtPileupHits );
  declareProperty("SeparateInDetPileupHits", m_separateInDetPileupHits);

  declareProperty("MDT_HitCollection",  m_mdtHits );
  declareProperty("RPC_HitCollection",  m_rpcHits );
  declareProperty("TGC_HitCollection",  m_tgcHits );
  declareProperty("CSC_HitCollection",  m_cscHits );

  declareProperty("SensitiveDetectorMasterTool", m_senDetTool );
  declareProperty("FastSimulationMasterTool", m_fastSimTool );
}


ISF::SimHitSvc::~SimHitSvc()
{
}

/** framework methods */
StatusCode ISF::SimHitSvc::initialize()
{
  // setup for validation mode
  if ( m_validationOutput)
    {
      ATH_CHECK(this->createSimHitsTree());
    }

  return StatusCode::SUCCESS;
}

/** framework methods */
StatusCode ISF::SimHitSvc::finalize()
{
  return StatusCode::SUCCESS;
}

/** Initialize event */
StatusCode ISF::SimHitSvc::initializeEvent()
{
  ATH_MSG_DEBUG("initializing hit collections");

  if(!m_senDetTool) {
    ATH_CHECK(m_senDetTool.retrieve());
  }
  ATH_CHECK(m_senDetTool->BeginOfAthenaEvent());

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
  if (m_mdtHits.isValid() && m_mdtHits->size()) {
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

  if (m_rpcHits.isValid() && m_rpcHits->size()) {
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
  if (m_tgcHits.isValid() && m_tgcHits->size()) {
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
  if (m_cscHits.isValid() && m_cscHits->size()) {
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
    SG::ReadHandle<SiHitCollection>& pixHits=(ipileup==0) ? m_pixHits : m_pixPileupHits;

    if (pixHits.isValid() && pixHits->size()) {
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
    SG::ReadHandle<SiHitCollection>& sctHits=(ipileup==0) ? m_sctHits : m_sctPileupHits;
    if (sctHits.isValid() && sctHits->size()) {
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
    SG::ReadHandle<TRTUncompressedHitCollection>& trtHits = (ipileup==0) ? m_trtHits : m_trtPileupHits;
    if (trtHits.isValid() && trtHits->size()) {
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
