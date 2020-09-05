/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// Oleg.Fedin@cern.ch, August 2010
//////////////////////////////////////////////////////////////////////////
#include "McEventCollectionFilter.h"
//
#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenVertex.h"
//
#include "InDetSimEvent/SiHit.h"
#include "MuonSimEvent/TGCSimHit.h"
#include "MuonSimEvent/CSCSimHit.h"
#include "MuonSimEvent/sTGCSimHit.h"
#include "MuonSimEvent/MMSimHit.h"
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
//
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"
#include <climits>

McEventCollectionFilter::McEventCollectionFilter(const std::string& name, ISvcLocator* pSvcLocator)
  : AthAlgorithm(name, pSvcLocator)
  , m_inputTruthCollection("StoreGateSvc+TruthEventOLD")
  , m_inputBCMHits("StoreGateSvc+BCMHitsOLD")
  , m_inputPixelHits("StoreGateSvc+PixelHitsOLD")
  , m_inputSCTHits("StoreGateSvc+SCT_HitsOLD")
  , m_inputTRTHits("StoreGateSvc+TRTUncompressedHitsOLD")
  , m_inputCSCHits("StoreGateSvc+CSC_HitsOLD")
  , m_inputMDTHits("StoreGateSvc+MDT_HitsOLD")
  , m_inputRPCHits("StoreGateSvc+RPC_HitsOLD")
  , m_inputTGCHits("StoreGateSvc+TGC_HitsOLD")
  , m_inputSTGCHits("StoreGateSvc+sTGCSensitiveDetectorOLD")
  , m_inputMMHits("StoreGateSvc+MicromegasSensitiveDetectorOLD")
  , m_outputTruthCollection("StoreGateSvc+TruthEvent")
  , m_outputBCMHits("StoreGateSvc+BCMHits")
  , m_outputPixelHits("StoreGateSvc+PixelHits")
  , m_outputSCTHits("StoreGateSvc+SCT_Hits")
  , m_outputTRTHits("StoreGateSvc+TRTUncompressedHits")
  , m_outputCSCHits("StoreGateSvc+CSC_Hits")
  , m_outputMDTHits("StoreGateSvc+MDT_Hits")
  , m_outputRPCHits("StoreGateSvc+RPC_Hits")
  , m_outputTGCHits("StoreGateSvc+TGC_Hits")
  , m_outputSTGCHits("StoreGateSvc+sTGCSensitiveDetector")
  , m_outputMMHits("StoreGateSvc+MicromegasSensitiveDetector")
  , m_IsKeepTRTElect(false)
  , m_PileupPartPDGID(999) //Geantino
  , m_UseTRTHits(true)
  , m_UseCSCHits(true) // On unless RUN3 symmetric layout
  , m_UseSTGCHits(false) // Off unless RUN3 layout
  , m_UseMMHits(false) // Off unless RUN3 layout
  , m_UseBCMHits(true) //On unless RUN4 layout
  , m_RefBarcode(0)
{
  declareProperty("TruthInput"        , m_inputTruthCollection);
  declareProperty("TruthOutput"       , m_outputTruthCollection);
  declareProperty("BCMHitsInput"      , m_inputBCMHits);
  declareProperty("BCMHitsOutput"     , m_outputBCMHits);
  declareProperty("PixelHitsInput"    , m_inputPixelHits);
  declareProperty("PixelHitsOutput"   , m_outputPixelHits);
  declareProperty("SCTHitsInput"      , m_inputSCTHits);
  declareProperty("SCTHitsOutput"     , m_outputSCTHits);
  declareProperty("TRTHitsInput"      , m_inputTRTHits);
  declareProperty("TRTHitsOutput"     , m_outputTRTHits);
  declareProperty("CSCHitsInput"      , m_inputCSCHits);
  declareProperty("CSCHitsOutput"     , m_outputCSCHits);
  declareProperty("MDTHitsInput"      , m_inputMDTHits);
  declareProperty("MDTHitsOutput"     , m_outputMDTHits);
  declareProperty("RPCHitsInput"      , m_inputRPCHits);
  declareProperty("RPCHitsOutput"     , m_outputRPCHits);
  declareProperty("TGCHitsInput"      , m_inputTGCHits);
  declareProperty("TGCHitsOutput"     , m_outputTGCHits);
  declareProperty("sTGCHitsInput"     , m_inputSTGCHits);
  declareProperty("sTGCHitsOutput"    , m_outputSTGCHits);
  declareProperty("MMHitsInput"       , m_inputMMHits);
  declareProperty("MMHitsOutput"      , m_outputMMHits);
  declareProperty("IsKeepTRTElect"    , m_IsKeepTRTElect);
  declareProperty("PileupPartPDGID"   , m_PileupPartPDGID);
  declareProperty("UseTRTHits"        , m_UseTRTHits);
  declareProperty("UseCSCHits"        , m_UseCSCHits);
  declareProperty("UseSTGCHits"       , m_UseSTGCHits);
  declareProperty("UseMMHits"         , m_UseMMHits);
  declareProperty("UseBCMHits"        , m_UseBCMHits);

}


//-----------------------------------------------------
McEventCollectionFilter::~McEventCollectionFilter(){
  //----------------------------------------------------
}

//----------------------------------------------------
StatusCode McEventCollectionFilter::initialize(){
  //----------------------------------------------------

  return StatusCode::SUCCESS;

}

//-------------------------------------------------
StatusCode McEventCollectionFilter::execute(){
  //-------------------------------------------------

  ATH_MSG_DEBUG( " execute..... " );

  //... to find  electron barcodes linked to TRT hists
  if(m_IsKeepTRTElect&&m_UseTRTHits) {
    ATH_CHECK( FindTRTElectronHits() );
  }

  //.......Reduce McEventCollection
  ATH_CHECK( ReduceMCEventCollection() );

  //.......to relink all Pixel/SCT hits to the new particle
  ATH_CHECK( SiliconHitsTruthRelink() );

  //.......to relink all TRT hits to the new particle
  if(m_UseTRTHits) {
    ATH_CHECK( TRTHitsTruthRelink() );
  }

  //.......to relink all MDT hits to the new particle
  ATH_CHECK( MDTHitsTruthRelink() );

  //.......to relink all CSC hits to the new particle
  if(m_UseCSCHits) {
    ATH_CHECK( CSCHitsTruthRelink() );
  }

  //.......to relink all RPC hits to the new particle
  ATH_CHECK( RPCHitsTruthRelink() );

  //.......to relink all TGC hits to the new particle
  ATH_CHECK( TGCHitsTruthRelink() );

  //.......to relink all sTGC hits to the new particle
  if(m_UseSTGCHits) {
  ATH_CHECK( STGC_HitsTruthRelink() );
  }

  //.......to relink all MM hits to the new particle
  if(m_UseMMHits) {
    ATH_CHECK( MM_HitsTruthRelink() );
  }
  
  //.......to relink all BCM hits to the new particle
  if(m_UseBCMHits) {
    ATH_CHECK( BCMHitsTruthRelink() );
  }

  ATH_MSG_DEBUG( "succeded McEventCollectionFilter ..... " );

  return StatusCode::SUCCESS;

}

//-------------------------------------------------
StatusCode McEventCollectionFilter::finalize(){
  //-------------------------------------------------
  //
  ATH_MSG_DEBUG( "McEventCollectionFilter:: finalize completed successfully" );
  return StatusCode::SUCCESS;

}
//----------------------------------------------------------------
StatusCode McEventCollectionFilter::ReduceMCEventCollection(){
  //----------------------------------------------------------------
  //.......to reduce McEventCollection for pileup  particles
  //----------------------------------------------------------------
  //
  if(!m_inputTruthCollection.isValid())
    {
      ATH_MSG_ERROR( "Could not find McEventCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found McEventCollection");

  if (!m_outputTruthCollection.isValid()) m_outputTruthCollection = std::make_unique<McEventCollection>();

  //.......Create new particle (geantino) to link  hits from pileup
  HepMC::GenParticlePtr genPart=HepMC::newGenParticlePtr();
  genPart->set_pdg_id(m_PileupPartPDGID); //Geantino
  genPart->set_status(1); //!< set decay status
  HepMC::suggest_barcode(genPart, std::numeric_limits<int32_t>::max() );

  HepMC::GenVertexPtr genVertex = HepMC::newGenVertexPtr();
  genVertex->add_particle_out(genPart);

  const HepMC::GenEvent* genEvt = *(m_inputTruthCollection->begin());

  //......copy GenEvent to the new one and remove all vertex
  HepMC::GenEvent* evt=new HepMC::GenEvent(*genEvt);


  //to set geantino vertex as a truth primary vertex
  HepMC::GenVertexPtr hScatVx = genEvt->barcode_to_vertex(-3);
  if(hScatVx!=nullptr) {
    const HepMC::FourVector& pmvxpos=hScatVx->position();
    genVertex->set_position(pmvxpos);
    //to set geantino kinematic phi=eta=0, E=p=E_hard_scat
    HepMC::GenVertex::particles_in_const_iterator itrp =hScatVx->particles_in_const_begin();
    if (hScatVx->particles_in_size()==2){
      HepMC::FourVector mom1=(*itrp)->momentum();
      HepMC::FourVector mom2=(*(++itrp))->momentum();
      HepMC::FourVector vxmom;
      vxmom.setPx(mom1.e()+mom2.e());
      vxmom.setPy(0.);
      vxmom.setPz(0.);
      vxmom.setE(mom1.e()+mom2.e());

      genPart->set_momentum(vxmom);
    }
  }

  if(!evt->vertices_empty()){
    HepMC::GenEvent::vertex_iterator itvtx = evt->vertices_begin();
    for (;itvtx != evt ->vertices_end(); ++itvtx ) {
      HepMC::GenVertexPtr vtx = *itvtx++;
      evt->remove_vertex(vtx);
      delete vtx;
    }
  }

  //--------------------------------------
  if(m_IsKeepTRTElect){
    for(int i=0;i<(int) m_elecBarcode.size();i++){
      HepMC::GenParticlePtr thePart=genEvt->barcode_to_particle(m_elecBarcode[i]);
      if (!thePart){
        ATH_MSG_DEBUG( "Could not find particle for barcode " << m_elecBarcode[i] );
        continue;
      }
      HepMC::ConstGenVertexPtr vx = thePart->production_vertex();
      HepMC::GenParticlePtr thePart_new = HepMC::newGenParticlePtr( thePart->momentum(),thePart->pdg_id(),
                                                                    thePart->status());
      HepMC::suggest_barcode(thePart_new, m_elecBarcode[i]);

      const HepMC::FourVector& pos= vx->position();
      HepMC::GenVertexPtr vx_new = HepMC::newGenVertexPtr(pos);
      vx_new->add_particle_out(thePart_new);
      evt->add_vertex(vx_new);
    }
  }

  //.....add new vertex with geantino
  evt->add_vertex(genVertex);
  m_RefBarcode=HepMC::barcode(*genPart);

  m_outputTruthCollection->push_back(evt);

  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::SiliconHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all Pixel/SCT hits to the new particle
  //--------------------------------------------------------
  //
  if(!m_inputPixelHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find Pixel SiHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found Pixel SiHitCollection");

  if (!m_outputPixelHits.isValid()) m_outputPixelHits = std::make_unique<SiHitCollection>();

  ATH_CHECK(this->SiHitsTruthRelink(m_inputPixelHits,m_outputPixelHits));

  if(!m_inputSCTHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find SCT SiHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found SCT SiHitCollection");

  if (!m_outputSCTHits.isValid()) m_outputSCTHits = std::make_unique<SiHitCollection>();

  ATH_CHECK(this->SiHitsTruthRelink(m_inputSCTHits,m_outputSCTHits));

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::BCMHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink BCM hits to the new particle
  //--------------------------------------------------------
  //

  if(!m_inputBCMHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find BCM SiHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found BCM SiHitCollection");

  if (!m_outputBCMHits.isValid()) m_outputBCMHits = std::make_unique<SiHitCollection>();

  ATH_CHECK(this->SiHitsTruthRelink(m_inputBCMHits,m_outputBCMHits));

  return StatusCode::SUCCESS;
}

StatusCode McEventCollectionFilter::SiHitsTruthRelink(SG::ReadHandle<SiHitCollection>& inputHits, SG::WriteHandle<SiHitCollection>& outputHits){
  for (SiHitCollection::const_iterator i = inputHits->begin(); i != inputHits->end(); ++i) {
    const HepMcParticleLink oldLink = (*i).particleLink();


    HepGeom::Point3D<double>   lP1  = (*i).localStartPosition();
    HepGeom::Point3D<double>   lP2  = (*i).localEndPosition();
    double       edep = (*i).energyLoss();
    double       mt   = (*i).meanTime();
    unsigned int id   = (*i).identify();

    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
    outputHits->Emplace(lP1,lP2, edep, mt,curBarcode , id);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TRTHitsTruthRelink()
{
  //--------------------------------------------------------
  //.......to relink all TRT hits to the new particle
  //--------------------------------------------------------
  //
  if(!m_inputTRTHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find TRTUncompressedHitsCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found TRTUncompressedHitsCollection");

  if (!m_outputTRTHits.isValid()) m_outputTRTHits = std::make_unique<TRTUncompressedHitCollection>();
  for (TRTUncompressedHitCollection::const_iterator i = m_inputTRTHits->begin(); i != m_inputTRTHits->end(); ++i)
    {

      const HepMcParticleLink oldLink = (*i).particleLink();

      int   pdgID = (*i).GetParticleEncoding();
      int curBarcode=oldLink.barcode();
      if(curBarcode!=0)
        {
          if(!(m_IsKeepTRTElect && std::abs(pdgID)==11))
            {
              curBarcode=m_RefBarcode;
            }
        }
      HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
      int   id         = (*i).GetHitID();
      float kinEnergy  = (*i).GetKineticEnergy();
      float eneDeposit = (*i).GetEnergyDeposit();
      float preX       = (*i).GetPreStepX();
      float preY       = (*i).GetPreStepY();
      float preZ       = (*i).GetPreStepZ();
      float postX      = (*i).GetPostStepX();
      float postY      = (*i).GetPostStepY() ;
      float postZ      = (*i).GetPostStepZ();
      float time       = (*i).GetGlobalTime();

      m_outputTRTHits->Emplace(id,partLink,pdgID,kinEnergy,eneDeposit,preX,preY,preZ,postX,postY,postZ,time);
    }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::MDTHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all MDT hits to the new particle
  //--------------------------------------------------------
  if(!m_inputMDTHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find MDTSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found MDTSimHitCollection");

  if (!m_outputMDTHits.isValid()) m_outputMDTHits = std::make_unique<MDTSimHitCollection>();
  for(MDTSimHitConstIterator i=m_inputMDTHits->begin();i!=m_inputMDTHits->end();++i){

    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
    int            id = (*i).MDTid();
    double       time = (*i).globalTime();
    double     radius = (*i).driftRadius();
    Amg::Vector3D  lP = (*i).localPosition();
    //int  trackNumber = (*i).trackNumber();
    double stepLength = (*i).stepLength();
    double eneDeposit = (*i).energyDeposit();
    int         pdgID = (*i).particleEncoding();
    double  kinEnergy = (*i).kineticEnergy();

    m_outputMDTHits->Emplace(id,time,radius,lP,partLink,stepLength,eneDeposit,pdgID,kinEnergy);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::CSCHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all CSC hits to the new particle
  //--------------------------------------------------------
  if(!m_inputCSCHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find CSCSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found CSCSimHitCollection");

  if (!m_outputCSCHits.isValid()) m_outputCSCHits = std::make_unique<CSCSimHitCollection>();
  for(CSCSimHitConstIterator i=m_inputCSCHits->begin();i!=m_inputCSCHits->end();++i){

    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
    int              id = (*i).CSCid();
    double         time = (*i).globalTime();
    double   eneDeposit = (*i).energyDeposit();
    Amg::Vector3D HitStart = (*i).getHitStart();
    Amg::Vector3D HitEnd = (*i).getHitEnd();
    int          pdgID  = (*i).particleID();
    double    kinEnergy = (*i).kineticEnergy();

    m_outputCSCHits->Emplace(id,time,eneDeposit,HitStart,HitEnd,pdgID,partLink,kinEnergy);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::RPCHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all RPC hits to the new particle
  //--------------------------------------------------------
  if(!m_inputRPCHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find RPCSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found RPCSimHitCollection");

  if (!m_outputRPCHits.isValid()) m_outputRPCHits = std::make_unique<RPCSimHitCollection>();
  for(RPCSimHitConstIterator i=m_inputRPCHits->begin();i!=m_inputRPCHits->end();++i){

    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
    int            id = (*i).RPCid();
    double       time = (*i).globalTime();
    Amg::Vector3D prepos = (*i).preLocalPosition();
    Amg::Vector3D   ppos = (*i).postLocalPosition();
    double eneDeposit = (*i).energyDeposit();
    int         pdgID = (*i).particleEncoding();
    double kinEnergy  = (*i).kineticEnergy();
    double stepLength = (*i).stepLength();

    m_outputRPCHits->Emplace(id,time,prepos,partLink,ppos,eneDeposit,stepLength,pdgID,kinEnergy);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TGCHitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all TGC hits to the new particle
  //--------------------------------------------------------
  if(!m_inputTGCHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find TGCSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found TGCSimHitCollection");

  if (!m_outputTGCHits.isValid()) m_outputTGCHits = std::make_unique<TGCSimHitCollection>();
  for(TGCSimHitConstIterator i=m_inputTGCHits->begin();i!=m_inputTGCHits->end();++i){

    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
    int             id = (*i).TGCid();
    double        time = (*i).globalTime();
    Amg::Vector3D  pos = (*i).localPosition();
    Amg::Vector3D  dir = (*i).localDireCos();
    double   enDeposit = (*i).energyDeposit();
    double      stpLen = (*i).stepLength();
    int          pdgID = (*i).particleEncoding();
    double  kinEnergy  = (*i).kineticEnergy();

    m_outputTGCHits->Emplace(id,time,pos,dir,partLink,enDeposit,stpLen,pdgID,kinEnergy);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::STGC_HitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all sTGC hits to the new particle
  //--------------------------------------------------------
  if(!m_inputSTGCHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find sTGCSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found sTGCSimHitCollection");

  if (!m_outputSTGCHits.isValid()) m_outputSTGCHits = std::make_unique<sTGCSimHitCollection>();
  for(sTGCSimHitConstIterator i=m_inputSTGCHits->begin();i!=m_inputSTGCHits->end();++i){
    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());

    int             id = (*i).sTGCId();
    double        time = (*i).globalTime();
    Amg::Vector3D  pos = (*i).globalPosition();
    int          pdgID = (*i).particleEncoding();
    Amg::Vector3D  dir = (*i).globalDirection();
    double   enDeposit = (*i).depositEnergy();

    m_outputSTGCHits->Emplace(id,time,pos,pdgID,dir,enDeposit,partLink);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::MM_HitsTruthRelink(){
  //--------------------------------------------------------
  //.......to relink all MM hits to the new particle
  //--------------------------------------------------------
  if(!m_inputMMHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find MMSimHitCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found MMSimHitCollection");

  if (!m_outputMMHits.isValid()) m_outputMMHits = std::make_unique<MMSimHitCollection>();
  for(MMSimHitConstIterator i=m_outputMMHits->begin();i!=m_outputMMHits->end();++i){
    const HepMcParticleLink oldLink = (*i).particleLink();
    int curBarcode=0;
    if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
    HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());

    int             id = (*i).MMId();
    double        time = (*i).globalTime();
    Amg::Vector3D  pos = (*i).globalPosition();
    int          pdgID = (*i).particleEncoding();
    double    kinEnergy = (*i).kineticEnergy();
    Amg::Vector3D  dir = (*i).globalDirection();
    double   enDeposit = (*i).depositEnergy();

    m_outputMMHits->Emplace(id,time,pos,pdgID,kinEnergy,dir,enDeposit,partLink);
  }

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::FindTRTElectronHits()
{
  //--------------------------------------------------------
  //.......retrive TRTUncompressedHitCollection collection
  if(!m_inputTRTHits.isValid())
    {
      ATH_MSG_ERROR( "Could not find TRTUncompressedHitsCollection");
      return StatusCode::FAILURE;
    }
  ATH_MSG_DEBUG( "Found TRTUncompressedHitsCollection");

  m_elecBarcode.clear();

  std::set<int> barcode_tmp;

  for (TRTUncompressedHitCollection::const_iterator i = m_inputTRTHits->begin(); i != m_inputTRTHits->end(); ++i)
    {
      const HepMcParticleLink McLink = (*i).particleLink();
      int  pdgID = (*i).GetParticleEncoding();
      if(std::abs(pdgID)==11&&McLink.barcode()!=0) barcode_tmp.insert(McLink.barcode());
    }

  m_elecBarcode.assign(barcode_tmp.begin(),barcode_tmp.end());

  return StatusCode::SUCCESS;
}
