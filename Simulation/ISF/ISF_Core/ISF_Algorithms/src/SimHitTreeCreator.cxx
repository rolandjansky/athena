/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// class header include
#include "SimHitTreeCreator.h"

// Athena includes
#include "StoreGate/ReadHandle.h"
#include "GeneratorObjects/HepMcParticleLink.h"

// HepMC includes
#include "HepMC/GenParticle.h"

// ROOT includes
#include "TTree.h"

ISF::SimHitTreeCreator::SimHitTreeCreator( const std::string& name, ISvcLocator* pSvcLocator )
  : ::AthAlgorithm( name, pSvcLocator )
  , m_thistSvc("THistSvc",name)
  , m_validationStream("ISFSimHit")
  , m_t_simHits(nullptr)
  , m_pileup(-1)
  , m_type(-1)
  , m_id(-1)
  , m_mother(-1)
  , m_barcode(-1)
  , m_time(-1.)
  , m_drift(-1.)
  , m_edeposit(-1.)
  , m_momentum(-1.)
  , m_theta(-1.)
  , m_phi(-1.)
  , m_eta(-1.)
  , m_bcmHits("BCMHits")
  , m_blmHits("BLMHits")
  , m_pixHits("PixelHits")
  , m_sctHits("SCT_Hits")
  , m_trtHits("TRTUncompressedHits")
  , m_pixPileupHits("PileupPixelHits")
  , m_sctPileupHits("PileupSCT_Hits")
  , m_trtPileupHits("PileupTRTUncompressedHits")
  , m_mdtHits("MDT_Hits")
  , m_rpcHits("RPC_Hits")
  , m_tgcHits("TGC_Hits")
  , m_cscHits("CSC_Hits")
{
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
  declareProperty("MDT_HitCollection",  m_mdtHits );
  declareProperty("RPC_HitCollection",  m_rpcHits );
  declareProperty("TGC_HitCollection",  m_tgcHits );
  declareProperty("CSC_HitCollection",  m_cscHits );

}

// Destructor
///////////////
ISF::SimHitTreeCreator::~SimHitTreeCreator()
{}

/** framework methods */
StatusCode ISF::SimHitTreeCreator::initialize() {
  // retrieve the histogram service
  ATH_CHECK( m_thistSvc.retrieve() );

  ATH_CHECK(this->createSimHitsTree());

  ATH_CHECK( m_bcmHits.initialize() );
  ATH_CHECK( m_blmHits.initialize() );
  ATH_CHECK( m_pixHits.initialize() );
  ATH_CHECK( m_sctHits.initialize() );
  ATH_CHECK( m_trtHits.initialize() );
  ATH_CHECK( m_pixPileupHits.initialize() );
  ATH_CHECK( m_sctPileupHits.initialize() );
  ATH_CHECK( m_trtPileupHits.initialize() );
  ATH_CHECK( m_mdtHits.initialize() );
  ATH_CHECK( m_rpcHits.initialize() );
  ATH_CHECK( m_tgcHits.initialize() );
  ATH_CHECK( m_cscHits.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode ISF::SimHitTreeCreator::execute() {
  ATH_CHECK(this->fillSimHitsTree());
  return StatusCode::SUCCESS;
}

/** Create the simhits tree - validation mode only */
StatusCode ISF::SimHitTreeCreator::createSimHitsTree() {
  // Create the prefix of histogram names for the THistSvc
  //std::string prefix = "/" + m_validationStream + "/";
  const char *treeName="simhits";
  const std::string prefix = "/" + m_validationStream + "/"+ treeName;
  m_t_simHits = new TTree( treeName, treeName );
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
  ATH_CHECK( m_thistSvc->regTree( prefix, m_t_simHits) );

  return StatusCode::SUCCESS;
}

/** Fill the simhit TTree - validation mode only */
StatusCode ISF::SimHitTreeCreator::fillSimHitsTree()
{
  // loop over collections

  SG::ReadHandle<MDTSimHitCollection> mdtHits(m_mdtHits);
  if (mdtHits.isValid() && mdtHits->size()) {
    MDTSimHitCollection::const_iterator ih=mdtHits->begin();
    while ( ih!=mdtHits->end()) {
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
      while (ih!=mdtHits->end() && m_id==(*ih).MDTid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }

  SG::ReadHandle<RPCSimHitCollection> rpcHits(m_rpcHits);
  if (rpcHits.isValid() && rpcHits->size()) {
    RPCSimHitCollection::const_iterator ih=rpcHits->begin();
    while (ih!=rpcHits->end()) {
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
      while (ih!=rpcHits->end() && m_id==(*ih).RPCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }

  SG::ReadHandle<TGCSimHitCollection> tgcHits(m_tgcHits);
  if (tgcHits.isValid() && tgcHits->size()) {
    TGCSimHitCollection::const_iterator ih=tgcHits->begin();
    while ( ih!=tgcHits->end()) {
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
      while (ih!=tgcHits->end() && m_id==(*ih).TGCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }

  SG::ReadHandle<CSCSimHitCollection> cscHits(m_cscHits);
  if (cscHits.isValid() && cscHits->size()) {
    CSCSimHitCollection::const_iterator ih=cscHits->begin();
    while ( ih!=cscHits->end()) {
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
      while (ih!=cscHits->end() && m_id==(*ih).CSCid() && m_barcode==(*ih).trackNumber() ) {
        // merge energy deposits and move on
        m_edeposit += (*ih).energyDeposit();
        ih++;
      }
      m_t_simHits->Fill();
    }
  }

  for (int ipileup=0;ipileup<2;ipileup++) {
    m_pileup = ipileup;

    SG::ReadHandle<SiHitCollection> pixHits((ipileup==0) ? m_pixHits : m_pixPileupHits);
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

    SG::ReadHandle<SiHitCollection> sctHits((ipileup==0) ? m_sctHits : m_sctPileupHits);
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

    SG::ReadHandle<TRTUncompressedHitCollection> trtHits((ipileup==0) ? m_trtHits : m_trtPileupHits);
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
  return StatusCode::SUCCESS;
}

//** Add information from HepMcParticleLink to TTree - common for all SimHit types */
void ISF::SimHitTreeCreator::addHepMcParticleLinkInfoToTree(HepMcParticleLink &HMPL) {
  if (HMPL.isValid()) {
    m_momentum = (HMPL.cptr())->momentum().rho();
    m_eta= (HMPL.cptr())->momentum().eta();
    m_theta= (HMPL.cptr())->momentum().theta();
    m_phi  = (HMPL.cptr())->momentum().phi();
  }
  else {
    m_momentum=-1.;
    m_theta=-1.;
    m_eta=-10.;
    m_phi = -10.;
  }
  return;
}
