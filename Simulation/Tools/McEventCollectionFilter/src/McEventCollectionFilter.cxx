/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// Oleg.Fedin@cern.ch, August 2010
//////////////////////////////////////////////////////////////////////////
#include "McEventCollectionFilter.h"
//
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "GeneratorObjects/McEventCollection.h"
//
#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEvent/SiHit.h"
#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "MuonSimEvent/MDTSimHitCollection.h"
#include "MuonSimEvent/RPCSimHitCollection.h"
#include "MuonSimEvent/TGCSimHitCollection.h"
#include "MuonSimEvent/CSCSimHitCollection.h"
#include "MuonSimEvent/sTGCSimHitCollection.h"
#include "MuonSimEvent/MMSimHitCollection.h"
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

McEventCollectionFilter::McEventCollectionFilter(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
 declareProperty("IsKeepTRTElect"    , m_IsKeepTRTElect   = false);  //
 declareProperty("McEventCollection" , m_mcEventCollection  = "TruthEvent");
 declareProperty("PileupPartPDGID"   , m_PileupPartPDGID    = 999);  //Geantino
 declareProperty("UseTRTHits"        , m_UseTRTHits   = true);  //
 declareProperty("UseCSCHits"        , m_UseCSCHits   = true);  // If symmetric RUN3, CSCs are turned off, otherwise default to on
 declareProperty("UseSTGCHits"       , m_UseSTGCHits   = false);  // Both NSW technologies are turned off by default. They are turned automatically if RUN3 geometry is used.
 declareProperty("UseMMHits"         , m_UseMMHits   = false);  //

 m_RefBarcode=0;
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

  //.......to relink all Si hits to the new particle
  ATH_CHECK( SiHitsTruthRelink() );

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

  //.......to relink all STGC hits to the new particle
  if(m_UseSTGCHits) {
    ATH_CHECK( STGC_HitsTruthRelink() );
  }

  //.......to relink all MM hits to the new particle
  if(m_UseMMHits) {
    ATH_CHECK( MM_HitsTruthRelink() );
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
  // ....... Retrieve MC truht collection
  const McEventCollection* pMcEvtColl=0;
  ATH_CHECK( evtStore()->retrieve(pMcEvtColl,m_mcEventCollection) );

  //.......Create new McEventCollection
  McEventCollection* pMcEvtCollNew= new McEventCollection();

  //.......Create new particle (geantino) to link  hits from pileup
  HepMC::GenParticle* genPart=new HepMC::GenParticle();
  genPart->set_pdg_id(m_PileupPartPDGID); //Geantino
  genPart->set_status(1); //!< set decay status
  genPart->suggest_barcode( std::numeric_limits<int32_t>::max() );

  HepMC::GenVertex* genVertex=new HepMC::GenVertex();
  genVertex->add_particle_out(genPart);

  const HepMC::GenEvent* genEvt = *(pMcEvtColl->begin());

  //......copy GenEvent to the new one and remove all vertex
  HepMC::GenEvent* evt=new HepMC::GenEvent(*genEvt);

  //to set geantino vertex as a truth primary vertex
  //HepMC::GenVertex* pmvx=*(evt->vertices_begin());
  //HepMC::FourVector pmvxpos=pmvx->position();
  //genVertex->set_position(pmvxpos);

  //to set geantino vertex as a truth primary vertex
  HepMC::GenVertex* hScatVx = genEvt->barcode_to_vertex(-3);
  if(hScatVx!=0) {
    HepMC::FourVector pmvxpos=hScatVx->position();
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
    std::vector<HepMC::GenVertex *> vtxvec;
    HepMC::GenEvent::vertex_iterator itvtx = evt->vertices_begin();
    for (;itvtx != evt ->vertices_end(); ++itvtx ) {
      evt->remove_vertex(*itvtx);
      vtxvec.push_back((*itvtx));
      //fix me: delete vertex pointer causes crash
      //delete (*itvtx);
    }
    for(unsigned int i=0;i<vtxvec.size();i++)  delete vtxvec[i];
  }

  //--------------------------------------
  if(m_IsKeepTRTElect){
    for(int i=0;i<(int) m_elecBarcode.size();i++){
      HepMC::GenParticle* thePart=genEvt->barcode_to_particle(m_elecBarcode[i]);
      if (!thePart){
        ATH_MSG_DEBUG( "Could not find particle for barcode " << m_elecBarcode[i] );
        continue;
      }
      const HepMC::GenVertex* vx = thePart->production_vertex();
      HepMC::GenParticle* thePart_new=new HepMC::GenParticle( thePart->momentum(),thePart->pdg_id(),
                                                              thePart->status(),thePart->flow(),
                                                              thePart->polarization() );
      thePart_new->suggest_barcode(m_elecBarcode[i]);

      HepMC::FourVector pos= vx->position();
      HepMC::GenVertex* vx_new=new HepMC::GenVertex(pos);
      vx_new->add_particle_out(thePart_new);
      evt->add_vertex(vx_new);

    }
  }

  //.....add new vetex with geantino
  evt->add_vertex(genVertex);
  m_RefBarcode=genPart->barcode();

  pMcEvtCollNew->push_back(evt);


  //......remove old McEventCollection
  ATH_CHECK( evtStore()->remove(pMcEvtColl) );

  //......write new McEventCollection to SG
  ATH_CHECK( evtStore()->record(pMcEvtCollNew, m_mcEventCollection) );

  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::SiHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all Si hits to the new particle
//--------------------------------------------------------
//
  std::vector <std::string> m_HitContainer;
  m_HitContainer.push_back("PixelHits");
  m_HitContainer.push_back("SCT_Hits");
  m_HitContainer.push_back("BCMHits");
  //??? m_HitContainer.push_back("BLMHits");

  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++){

    //.......retrive SiHit collection
    const DataHandle<SiHitCollection> pSiHitColl;
    if(evtStore()->contains<SiHitCollection>(m_HitContainer[iHitContainer])) {
      ATH_CHECK( evtStore()->retrieve(pSiHitColl,m_HitContainer[iHitContainer] ) );
    } else {
      ATH_MSG_ERROR( "Could not find SiHitCollection containing " <<  m_HitContainer[iHitContainer] );
      return StatusCode::FAILURE;
    }

    SiHitCollection * pSiHitC = const_cast<SiHitCollection *> (&*pSiHitColl);

    //.......create new SiHit collection and copy all hits with the new HepMcParticleLink
    SiHitCollection *pSiHitCollNew = new SiHitCollection();

    for (SiHitCollection::const_iterator i = pSiHitC->begin(); i != pSiHitC->end(); ++i) {
      const HepMcParticleLink oldLink = (*i).particleLink();


      HepGeom::Point3D<double>   lP1  = (*i).localStartPosition();
      HepGeom::Point3D<double>   lP2  = (*i).localEndPosition();
      double       edep = (*i).energyLoss();
      double       mt   = (*i).meanTime();
      unsigned int id   = (*i).identify();
      int curBarcode=0;
      if(oldLink.barcode()!=0)  curBarcode=m_RefBarcode;
      HepMcParticleLink partLink(curBarcode, oldLink.eventIndex(), oldLink.getEventCollection());
      pSiHitCollNew->Emplace(lP1,lP2, edep, mt, partLink, id);
    }

    //......remove old SiHitCollection
    ATH_CHECK( evtStore()->remove(pSiHitC) );

    //......write new SiHitCollection
    ATH_CHECK( evtStore()->record(pSiHitCollNew,m_HitContainer[iHitContainer]) );

  }

  return StatusCode::SUCCESS;

}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TRTHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all TRT hits to the new particle
//--------------------------------------------------------
//
  //.......retrive TRTUncompressedHitCollection collection
  m_HitName  = "TRTUncompressedHits";
  const DataHandle<TRTUncompressedHitCollection> pTRTHitColl;

  if(evtStore()->contains<TRTUncompressedHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pTRTHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find collection containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Found collection containing " << m_HitName );

  TRTUncompressedHitCollection*  pTRTHitC= const_cast<TRTUncompressedHitCollection*> (&*pTRTHitColl);

  //.......Create new TRTUncompressedHitCollection
  TRTUncompressedHitCollection *pTRTHitCollNew =  new TRTUncompressedHitCollection();
  for (TRTUncompressedHitCollection::const_iterator i = pTRTHitColl->begin(); i != pTRTHitColl->end(); ++i) {

    const HepMcParticleLink oldLink = (*i).particleLink();

    int   pdgID = (*i).GetParticleEncoding();
    int curBarcode=0;
    if(oldLink.barcode()!=0&&!m_IsKeepTRTElect)                      curBarcode=m_RefBarcode;
    else if(oldLink.barcode()!=0&&m_IsKeepTRTElect&&fabs(pdgID)!=11) curBarcode=m_RefBarcode;
    else if(oldLink.barcode()!=0&&m_IsKeepTRTElect&&fabs(pdgID)==11) curBarcode=oldLink.barcode();
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

    pTRTHitCollNew->Emplace(id,partLink,pdgID,kinEnergy,eneDeposit,preX,preY,preZ,postX,postY,postZ,time);

  }

  //.......remove old TRTUncompressedHitCollection
  ATH_CHECK( evtStore()->remove(pTRTHitC) );

  //.......write new  TRTUncompressedHitCollection
  ATH_CHECK( evtStore()->record(pTRTHitCollNew,m_HitName) );

  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::MDTHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all MDT hits to the new particle
//--------------------------------------------------------

  m_HitName="MDT_Hits";
  const DataHandle<MDTSimHitCollection> pMDTHitColl;

  if(evtStore()->contains<MDTSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pMDTHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find MDTSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }


  MDTSimHitCollection* pMDTHitC = const_cast<MDTSimHitCollection*> (&*pMDTHitColl);

  //.......Create new MDTSimHitCollection
  MDTSimHitCollection* pMDTHitCollNew = new MDTSimHitCollection();

  for(MDTSimHitConstIterator i=pMDTHitColl->begin();i!=pMDTHitColl->end();++i){

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


    pMDTHitCollNew->Emplace(id,time,radius,lP,partLink,stepLength,eneDeposit,pdgID,kinEnergy);
  }

  //.......remove old MDTSimHitCollection
  ATH_CHECK( evtStore()->remove(pMDTHitC) );

  //.......write new  MDTSimHitCollection
  ATH_CHECK( evtStore()->record(pMDTHitCollNew,m_HitName) );

  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::CSCHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all CSC hits to the new particle
//--------------------------------------------------------

  m_HitName="CSC_Hits";
  const DataHandle<CSCSimHitCollection> pCSCHitColl;

  if(evtStore()->contains<CSCSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pCSCHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find CSCSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }


  CSCSimHitCollection* pCSCHitC = const_cast<CSCSimHitCollection*> (&*pCSCHitColl);

  //.......Create new CSCSimHitCollection
  CSCSimHitCollection* pCSCHitCollNew = new CSCSimHitCollection();

  for(CSCSimHitConstIterator i=pCSCHitColl->begin();i!=pCSCHitColl->end();++i){

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

    pCSCHitCollNew->Emplace(id,time,eneDeposit,HitStart,HitEnd,pdgID,partLink,kinEnergy);
  }

  //.......remove old CSCSimHitCollection
  ATH_CHECK( evtStore()->remove(pCSCHitC) );

  //.......write new  CSCSimHitCollection
  ATH_CHECK( evtStore()->record(pCSCHitCollNew,m_HitName) );

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::RPCHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all RPC hits to the new particle
//--------------------------------------------------------

  m_HitName="RPC_Hits";
  const DataHandle<RPCSimHitCollection> pRPCHitColl;

  if(evtStore()->contains<RPCSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pRPCHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find RPCSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  RPCSimHitCollection* pRPCHitC = const_cast<RPCSimHitCollection*> (&*pRPCHitColl);

  //.......Create new RPCSimHitCollection
  RPCSimHitCollection* pRPCHitCollNew = new RPCSimHitCollection();

  for(RPCSimHitConstIterator i=pRPCHitColl->begin();i!=pRPCHitColl->end();++i){

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

    pRPCHitCollNew->Emplace(id,time,prepos,partLink,ppos,eneDeposit,stepLength,pdgID,kinEnergy);

  }

  //.......remove old RPCSimHitCollection
  ATH_CHECK( evtStore()->remove(pRPCHitC) );

  //.......write new  RPCSimHitCollection
  ATH_CHECK( evtStore()->record(pRPCHitCollNew,m_HitName) );

  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TGCHitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all TGC hits to the new particle
//--------------------------------------------------------
  m_HitName="TGC_Hits";
  const DataHandle<TGCSimHitCollection> pTGCHitColl;

  if(evtStore()->contains<TGCSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pTGCHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find TGCSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  TGCSimHitCollection* pTGCHitC = const_cast<TGCSimHitCollection*> (&*pTGCHitColl);

  //.......Create new TGCSimHitCollection
  TGCSimHitCollection* pTGCHitCollNew = new TGCSimHitCollection();

  for(TGCSimHitConstIterator i=pTGCHitColl->begin();i!=pTGCHitColl->end();++i){
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

    pTGCHitCollNew->Emplace(id,time,pos,dir,partLink,enDeposit,stpLen,pdgID,kinEnergy);

  }

  //.......remove old TGCSimHitCollection
  ATH_CHECK( evtStore()->remove(pTGCHitC) );

  //.......write new  TGCSimHitCollection
  ATH_CHECK( evtStore()->record(pTGCHitCollNew,m_HitName) );
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::STGC_HitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all STGC hits to the new particle
//--------------------------------------------------------
  m_HitName="sTGCSensitiveDetector";
  const DataHandle<sTGCSimHitCollection> pSTGCHitColl;

  if(evtStore()->contains<sTGCSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pSTGCHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find sTGCSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  sTGCSimHitCollection* pSTGCHitC = const_cast<sTGCSimHitCollection*> (&*pSTGCHitColl);

  //.......Create new sTGCSimHitCollection
  sTGCSimHitCollection* pSTGCHitCollNew = new sTGCSimHitCollection();

  for(sTGCSimHitConstIterator i=pSTGCHitColl->begin();i!=pSTGCHitColl->end();++i){
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

    pSTGCHitCollNew->Emplace(id,time,pos,pdgID,dir,enDeposit,partLink);

  }

  //.......remove old sTGCSimHitCollection
  ATH_CHECK( evtStore()->remove(pSTGCHitC) );

  //.......write new  sTGCSimHitCollection
  ATH_CHECK( evtStore()->record(pSTGCHitCollNew,m_HitName) );
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::MM_HitsTruthRelink(){
//--------------------------------------------------------
//.......to relink all MM hits to the new particle
//--------------------------------------------------------
  m_HitName="MicromegasSensitiveDetector";
  const DataHandle<MMSimHitCollection> pMMHitColl;

  if(evtStore()->contains<MMSimHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pMMHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find MMSimHitCollection  containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  MMSimHitCollection* pMMHitC = const_cast<MMSimHitCollection*> (&*pMMHitColl);

  //.......Create new MMSimHitCollection
  MMSimHitCollection* pMMHitCollNew = new MMSimHitCollection();

  for(MMSimHitConstIterator i=pMMHitColl->begin();i!=pMMHitColl->end();++i){
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

    pMMHitCollNew->Emplace(id,time,pos,pdgID,kinEnergy,dir,enDeposit,partLink);

  }

  //.......remove old MMSimHitCollection
  ATH_CHECK( evtStore()->remove(pMMHitC) );

  //.......write new  MMSimHitCollection
  ATH_CHECK( evtStore()->record(pMMHitCollNew,m_HitName) );
  return StatusCode::SUCCESS;
}


//--------------------------------------------------------
StatusCode McEventCollectionFilter::FindTRTElectronHits(){
//--------------------------------------------------------
  //.......retrive TRTUncompressedHitCollection collection
  m_HitName  = "TRTUncompressedHits";
  const DataHandle<TRTUncompressedHitCollection> pTRTHitColl;

  if(evtStore()->contains<TRTUncompressedHitCollection>(m_HitName)) {
    ATH_CHECK( evtStore()->retrieve(pTRTHitColl, m_HitName) );
  } else {
    ATH_MSG_ERROR( "Could not find collection containing " << m_HitName );
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG( "Found collection containing " << m_HitName );

  m_elecBarcode.clear();

  std::set<int> barcode_tmp;

  for (TRTUncompressedHitCollection::const_iterator i = pTRTHitColl->begin(); i != pTRTHitColl->end(); ++i) {
    const HepMcParticleLink McLink = (*i).particleLink();
    int  pdgID = (*i).GetParticleEncoding();
    if(fabs(pdgID)==11&&McLink.barcode()!=0) barcode_tmp.insert(McLink.barcode());
  }

  m_elecBarcode.assign(barcode_tmp.begin(),barcode_tmp.end());

  return StatusCode::SUCCESS;
}
