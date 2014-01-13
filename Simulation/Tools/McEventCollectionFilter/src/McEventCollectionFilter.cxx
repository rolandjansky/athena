/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////
// Oleg.Fedin@cern.ch, August 2010
//////////////////////////////////////////////////////////////////////////
#include "McEventCollectionFilter.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Units/SystemOfUnits.h"
//
#include "CLHEP/Geometry/Point3D.h"
#include "GeoPrimitives/GeoPrimitives.h"

McEventCollectionFilter::McEventCollectionFilter(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name, pSvcLocator)
{
 declareProperty("IsKeepTRTElect"    , m_IsKeepTRTElect   = false);  //
 declareProperty("McEventCollection" , m_mcEventCollection  = "TruthEvent");
 declareProperty("PileupPartPDGID"   , m_PileupPartPDGID    = 999);  //Geantino
 declareProperty("UseTRTHits"        , m_UseTRTHits   = true);  //

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

  StatusCode sc;

  msg(MSG::DEBUG) << " execute..... " << endreq;


  //... to find  electron barcodes linked to TRT hists
  if(m_IsKeepTRTElect&&m_UseTRTHits) {
     StatusCode  sc=FindTRTElectronHits();
    if(sc.isFailure()){
      msg(MSG::ERROR)  <<"FindTRTElectronHits  failed " << endreq;
      return sc;
    }
    msg(MSG::DEBUG)<< "FindTRTElectronHits succesful   "<< endreq;
  }

  //-------------------------------------
  //.......Reduce McEventCollection
  //-------------------------------------

  sc=ReduceMCEvenetCollection();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"ReduceMCEvenetCollection  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "ReduceMCEvenetCollection succesful   "<< endreq;

  //--------------------------------------------------
  //.......to relink all Si hits to the new particle
  //-------------------------------------------------

  sc=SiHistsTruthRelink();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"SiHistsTruthRelink:: Si hits truth particle relink  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "SiHistsTruthRelink:: Si hits truth particle relink succesful   "<< endreq;

  //--------------------------------------------------
  //.......to relink all TRT hits to the new particle
  //-------------------------------------------------

  if(m_UseTRTHits) {
   sc=TRTHistsTruthRelink();
   if(sc.isFailure()){
     msg(MSG::ERROR)  <<"TRTHistsTruthRelink:: TRT hits truth particle relink  failed " << endreq;
     return sc;
   }
   msg(MSG::DEBUG)<< "TRTHistsTruthRelink:: TRT hits truth particle relink succesful   "<< endreq;
  }

  //--------------------------------------------------
  //.......to relink all MDT hits to the new particle
  //-------------------------------------------------

  sc=MDTHistsTruthRelink();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"MDTHistsTruthRelink:: MDT hits truth particle relink  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "MDTHistsTruthRelink:: MDT hits truth particle relink succesful   "<< endreq;

  //--------------------------------------------------
  //.......to relink all CSC hits to the new particle
  //-------------------------------------------------

  sc=CSCHistsTruthRelink();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"CSCHistsTruthRelink:: CSC hits truth particle relink  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "CSCHistsTruthRelink:: CSC hits truth particle relink successful   "<< endreq;

  //--------------------------------------------------
  //.......to relink all RPC hits to the new particle
  //-------------------------------------------------

  sc=RPCHistsTruthRelink();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"RPCHistsTruthRelink:: RPC hits truth particle relink  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "RPCHistsTruthRelink:: RPC hits truth particle relink successful   "<< endreq;

  //--------------------------------------------------
  //.......to relink all TGC hits to the new particle
  //-------------------------------------------------

  sc=TGCHistsTruthRelink();
  if(sc.isFailure()){
    msg(MSG::ERROR)  <<"TGCHistsTruthRelink:: TGC hits truth particle relink  failed " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "TGCHistsTruthRelink:: TGC hits truth particle relink successful   "<< endreq;


  msg(MSG::DEBUG) << "succeded McEventCollectionFilter ..... " << endreq;

  return StatusCode::SUCCESS;

}

//-------------------------------------------------
StatusCode McEventCollectionFilter::finalize(){
//-------------------------------------------------
//
  msg(MSG::DEBUG)
    << "McEventCollectionFilter:: finalize completed successfully"
    << endreq;

   return StatusCode::SUCCESS;

}
//----------------------------------------------------------------
StatusCode McEventCollectionFilter::ReduceMCEvenetCollection(){
//----------------------------------------------------------------
//.......to reduce McEventCollection for pileup  particles
//----------------------------------------------------------------
//
  StatusCode sc;

  // ....... Retrieve MC truht collection
  const McEventCollection* pMcEvtColl=0;
  sc=evtStore()->retrieve(pMcEvtColl,m_mcEventCollection);
  if (sc.isFailure()||!pMcEvtColl){
    msg(MSG::ERROR)
      << "Could not retrieve container "
      << m_mcEventCollection
      << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::DEBUG) <<"McEventCollection succesfully retrived" << endreq;


  //.......Create new McEventCollection
  McEventCollection* pMcEvtCollNew= new McEventCollection();

  //.......Create new particle (geantino) to link  hits from pileup
  HepMC::GenParticle* genPart=new HepMC::GenParticle();
  genPart->set_pdg_id(m_PileupPartPDGID); //Geantino
  genPart->set_status(1); //!< set decay status

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
  sc=evtStore()->remove(pMcEvtColl);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can't remove McEventCollection" << endreq;
    return sc;
  }


  //......write new McEventCollection to SG
  sc=evtStore()->record(pMcEvtCollNew, m_mcEventCollection);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Couldn't record new McEventCollection" << endreq;
    return sc;
  }


  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::SiHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all Si hits to the new particle
//--------------------------------------------------------
//
  StatusCode sc;

  std::vector <std::string> m_HitContainer;
  m_HitContainer.push_back("PixelHits");
  m_HitContainer.push_back("SCT_Hits");
  m_HitContainer.push_back("BCMHits");
  //??? m_HitContainer.push_back("BLMHits");

  for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++){


    //.......retrive SiHit collection
    const DataHandle<SiHitCollection> pSiHitColl;
    if(evtStore()->contains<SiHitCollection>(m_HitContainer[iHitContainer])) {
      sc=evtStore()->retrieve(pSiHitColl,m_HitContainer[iHitContainer] );
      if (sc.isFailure() || !pSiHitColl) {
        msg(MSG::ERROR) << "Couldn't find the data object '" << m_HitContainer[iHitContainer] << "' !" << endreq;
        return sc;
     } else {
        msg(MSG::DEBUG) <<m_HitContainer[iHitContainer] << " container found " <<pSiHitColl->size()<< endreq;
       }
     } else {
       msg(MSG::ERROR) << "Could not find SiHitCollection containing " <<  m_HitContainer[iHitContainer] << endreq;
       sc = StatusCode::FAILURE;
       return sc;
     }

    SiHitCollection * pSiHitC = const_cast<SiHitCollection *> (&*pSiHitColl);

    //.......create new SiHit collection and copy all hits with the new HepMcParticleLink
    SiHitCollection *pSiHitCollNew = new SiHitCollection();

    for (SiHitCollection::const_iterator i = pSiHitC->begin(); i != pSiHitC->end(); ++i) {
      const HepMcParticleLink McLink = (*i).particleLink();


      HepGeom::Point3D<double>   lP1  = (*i).localStartPosition();
      HepGeom::Point3D<double>   lP2  = (*i).localEndPosition();
      double       edep = (*i).energyLoss();
      double       mt   = (*i).meanTime();
      unsigned int id   = (*i).identify();

      int CurBarcode=0;
      if(McLink.barcode()!=0)  CurBarcode=m_RefBarcode;


      SiHit newSiHit(lP1,lP2, edep, mt,CurBarcode , id);
      pSiHitCollNew->Insert(newSiHit);
    }

    //......remove old SiHitCollection
    sc=evtStore()->remove(pSiHitC);
    if (sc.isFailure()) {
      msg(MSG::ERROR)  << "Can not remove SiHitCollection containing " << m_HitContainer[iHitContainer]<<endreq;
      return sc;
    }
    msg(MSG::DEBUG)<< "Remove container "<<m_HitContainer[iHitContainer]<< endreq;

    //......write new SiHitCollection
    sc=evtStore()->record(pSiHitCollNew,m_HitContainer[iHitContainer]);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Can not record new container for " << m_HitContainer[iHitContainer] << endreq;
      return sc;
    }
    msg(MSG::DEBUG)<< "Record new SiHitCollection containing "<<m_HitContainer[iHitContainer]<< endreq;

  }

  return StatusCode::SUCCESS;

}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TRTHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all TRT hits to the new particle
//--------------------------------------------------------
//
  StatusCode sc;

  //.......retrive TRTUncompressedHitCollection collection
  m_HitName  = "TRTUncompressedHits";
  const DataHandle<TRTUncompressedHitCollection> pTRTHitColl;

  if(evtStore()->contains<TRTUncompressedHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pTRTHitColl, m_HitName);
    if (sc.isFailure() || !pTRTHitColl) {
      msg(MSG::ERROR) << "Could not find the data object '" << m_HitName << "' !" << endreq;
      return sc;
     } else {
      msg(MSG::DEBUG) << m_HitName << " container found" << endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find collection containing " << m_HitName << endreq;
     sc = StatusCode::FAILURE;
     return sc;
  }

  msg(MSG::DEBUG) << "Found collection containing " << m_HitName << endreq;

  TRTUncompressedHitCollection*  pTRTHitC= const_cast<TRTUncompressedHitCollection*> (&*pTRTHitColl);

  //.......Create new TRTUncompressedHitCollection
  TRTUncompressedHitCollection *pTRTHitCollNew =  new TRTUncompressedHitCollection();
  for (TRTUncompressedHitCollection::const_iterator i = pTRTHitColl->begin(); i != pTRTHitColl->end(); ++i) {

    const HepMcParticleLink McLink = (*i).particleLink();

    int   pdgID = (*i).GetParticleEncoding();
    int CurBarcode=0;
    if(McLink.barcode()!=0&&!m_IsKeepTRTElect)                      CurBarcode=m_RefBarcode;
    else if(McLink.barcode()!=0&&m_IsKeepTRTElect&&fabs(pdgID)!=11) CurBarcode=m_RefBarcode;
    else if(McLink.barcode()!=0&&m_IsKeepTRTElect&&fabs(pdgID)==11) CurBarcode=McLink.barcode();

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



    TRTUncompressedHit newTRTHit(id,CurBarcode,pdgID,kinEnergy,eneDeposit,preX,preY,preZ,postX,postY,postZ,time);
    pTRTHitCollNew->Insert(newTRTHit);


 }

  //.......remove old TRTUncompressedHitCollection
  sc=evtStore()->remove(pTRTHitC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can not remove collection " <<m_HitName<< endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Remove collection   "<< m_HitName<< endreq;

  //.......write new  TRTUncompressedHitCollection
  sc=evtStore()->record(pTRTHitCollNew,m_HitName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record new container for TRTUncompressedHits" << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Record new container TRTUncompressedHits"<< endreq;


  return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::MDTHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all MDT hits to the new particle
//--------------------------------------------------------

  StatusCode sc;

  m_HitName="MDT_Hits";
  const DataHandle<MDTSimHitCollection> pMDTHitColl;

  if(evtStore()->contains<MDTSimHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pMDTHitColl, m_HitName);
    if (sc.isFailure() || !pMDTHitColl) {
      msg(MSG::ERROR) << "Could not find MDTSimHitCollection" << endreq;
      return sc;
    } else {
      msg(MSG::DEBUG) << " MDTSimHitCollection  found " <<pMDTHitColl->size()<< endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find MDTSimHitCollection  containing " << m_HitName << endreq;
    sc = StatusCode::FAILURE;
    return sc;
  }


  MDTSimHitCollection* pMDTHitC = const_cast<MDTSimHitCollection*> (&*pMDTHitColl);

  //.......Create new MDTSimHitCollection
  MDTSimHitCollection* pMDTHitCollNew = new MDTSimHitCollection();

  for(MDTSimHitConstIterator i=pMDTHitColl->begin();i!=pMDTHitColl->end();++i){

    const HepMcParticleLink McLink = (*i).particleLink();
    int CurBarcode=0;
    if(McLink.barcode()!=0)  CurBarcode=m_RefBarcode;

    int            id = (*i).MDTid();
    double       time = (*i).globalTime();
    double     radius = (*i).driftRadius();
    Amg::Vector3D  lP = (*i).localPosition();
   //int  trackNumber = (*i).trackNumber();
    double stepLength = (*i).stepLength();
    double eneDeposit = (*i).energyDeposit();
    int         pdgID = (*i).particleEncoding();
    double  kinEnergy = (*i).kineticEnergy();


    MDTSimHit newMDTHit(id,time,radius,lP,CurBarcode,stepLength,eneDeposit,pdgID,kinEnergy);
    pMDTHitCollNew->Insert(newMDTHit);
  }


  //.......remove old MDTSimHitCollection
  sc=evtStore()->remove(pMDTHitC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can not remove MDTSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Remove  MDTSimHitCollection  "<< endreq;

  //.......write new  MDTSimHitCollection
  sc=evtStore()->record(pMDTHitCollNew,m_HitName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record new MDTSimHitCollection" << endreq;
    return sc;
  }
   msg(MSG::DEBUG)<< "Record new MDTSimHitCollection"<< endreq;

   return StatusCode::SUCCESS;

}
//--------------------------------------------------------
StatusCode McEventCollectionFilter::CSCHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all CSC hits to the new particle
//--------------------------------------------------------

  StatusCode sc;

  m_HitName="CSC_Hits";
  const DataHandle<CSCSimHitCollection> pCSCHitColl;

  if(evtStore()->contains<CSCSimHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pCSCHitColl, m_HitName);
    if (sc.isFailure() || !pCSCHitColl) {
      msg(MSG::ERROR) << "Could not find CSCSimHitCollection" << endreq;
      return sc;
     } else {
      msg(MSG::DEBUG) << " CSCSimHitCollection  found " <<pCSCHitColl->size()<<endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find CSCSimHitCollection  containing " << m_HitName << endreq;
     sc = StatusCode::FAILURE;
     return sc;
  }


  CSCSimHitCollection* pCSCHitC = const_cast<CSCSimHitCollection*> (&*pCSCHitColl);

  //.......Create new CSCSimHitCollection
  CSCSimHitCollection* pCSCHitCollNew = new CSCSimHitCollection();

  for(CSCSimHitConstIterator i=pCSCHitColl->begin();i!=pCSCHitColl->end();++i){

    const HepMcParticleLink McLink = (*i).particleLink();
    int CurBarcode=0;
    if(McLink.barcode()!=0)  CurBarcode=m_RefBarcode;

    int              id = (*i).CSCid();
    double         time = (*i).globalTime();
    double   eneDeposit = (*i).energyDeposit();
    Amg::Vector3D HitStart = (*i).getHitStart();
    Amg::Vector3D HitEnd = (*i).getHitEnd();
    int          pdgID  = (*i).particleID();
    double    kinEnergy = (*i).kineticEnergy();

    CSCSimHit newCSCHit(id,time,eneDeposit,HitStart,HitEnd,pdgID,CurBarcode,kinEnergy);
    pCSCHitCollNew->Insert(newCSCHit);
  }

  //.......remove old CSCSimHitCollection
  sc=evtStore()->remove(pCSCHitC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can not remove CSCSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Remove  CSCSimHitCollection  "<< endreq;

  //.......write new  CSCSimHitCollection
  sc=evtStore()->record(pCSCHitCollNew,m_HitName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record new CSCSimHitCollection" << endreq;
    return sc;
  }
   msg(MSG::DEBUG)<< "Record new CSCSimHitCollection"<< endreq;

   return StatusCode::SUCCESS;


}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::RPCHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all RPC hits to the new particle
//--------------------------------------------------------

  StatusCode sc;

  m_HitName="RPC_Hits";
  const DataHandle<RPCSimHitCollection> pRPCHitColl;

  if(evtStore()->contains<RPCSimHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pRPCHitColl, m_HitName);
    if (sc.isFailure() || !pRPCHitColl) {
      msg(MSG::ERROR) << "Could not find RPCSimHitCollection" << endreq;
      return sc;
     } else {
      msg(MSG::DEBUG) << " RPCSimHitCollection  found " << pRPCHitColl->size()<<endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find RPCSimHitCollection  containing " << m_HitName << endreq;
     sc = StatusCode::FAILURE;
     return sc;
  }


  RPCSimHitCollection* pRPCHitC = const_cast<RPCSimHitCollection*> (&*pRPCHitColl);

  //.......Create new RPCSimHitCollection
  RPCSimHitCollection* pRPCHitCollNew = new RPCSimHitCollection();

  for(RPCSimHitConstIterator i=pRPCHitColl->begin();i!=pRPCHitColl->end();++i){

    const HepMcParticleLink McLink = (*i).particleLink();
    int CurBarcode=0;
    if(McLink.barcode()!=0)  CurBarcode=m_RefBarcode;

    int            id = (*i).RPCid();
    double       time = (*i).globalTime();
    Amg::Vector3D prepos = (*i).preLocalPosition();
    Amg::Vector3D   ppos = (*i).postLocalPosition();
    double eneDeposit = (*i).energyDeposit();
    int         pdgID = (*i).particleEncoding();
    double kinEnergy  = (*i).kineticEnergy();
    double stepLength = (*i).stepLength();

    RPCSimHit newRPCHit(id,time,prepos,CurBarcode,ppos,eneDeposit,stepLength,pdgID,kinEnergy);
    pRPCHitCollNew->Insert(newRPCHit);

  }

  //.......remove old RPCSimHitCollection
  sc=evtStore()->remove(pRPCHitC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can not remove RPCSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Remove  RPCSimHitCollection  "<< endreq;

  //.......write new  RPCSimHitCollection
  sc=evtStore()->record(pRPCHitCollNew,m_HitName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record new RPCSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Record new container RPCSimHitCollection"<< endreq;

   return StatusCode::SUCCESS;
}

//--------------------------------------------------------
StatusCode McEventCollectionFilter::TGCHistsTruthRelink(){
//--------------------------------------------------------
//.......to relink all TGC hits to the new particle
//--------------------------------------------------------
  StatusCode sc;

  m_HitName="TGC_Hits";
  const DataHandle<TGCSimHitCollection> pTGCHitColl;

  if(evtStore()->contains<TGCSimHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pTGCHitColl, m_HitName);
    if (sc.isFailure() || !pTGCHitColl) {
      msg(MSG::ERROR) << "Could not find TGCSimHitCollection" << endreq;
      return sc;
     } else {
      msg(MSG::DEBUG) << " TGCSimHitCollection  found " << pTGCHitColl->size()<<endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find TGCSimHitCollection  containing " << m_HitName << endreq;
     sc = StatusCode::FAILURE;
     return sc;
  }


  TGCSimHitCollection* pTGCHitC = const_cast<TGCSimHitCollection*> (&*pTGCHitColl);

  //.......Create new TGCSimHitCollection
  TGCSimHitCollection* pTGCHitCollNew = new TGCSimHitCollection();

  for(TGCSimHitConstIterator i=pTGCHitColl->begin();i!=pTGCHitColl->end();++i){
    const HepMcParticleLink McLink = (*i).particleLink();
    int CurBarcode=0;
    if(McLink.barcode()!=0)  CurBarcode=m_RefBarcode;

    int             id = (*i).TGCid();
    double        time = (*i).globalTime();
    Amg::Vector3D  pos = (*i).localPosition();
    Amg::Vector3D  dir = (*i).localDireCos();
    double   enDeposit = (*i).energyDeposit();
    double      stpLen = (*i).stepLength();
    int          pdgID = (*i).particleEncoding();
    double  kinEnergy  = (*i).kineticEnergy();

    TGCSimHit newTGCHit(id,time,pos,dir,CurBarcode,enDeposit,stpLen,pdgID,kinEnergy);
    pTGCHitCollNew->Insert(newTGCHit);

  }

  //.......remove old TGCSimHitCollection
  sc=evtStore()->remove(pTGCHitC);
  if (sc.isFailure()) {
    msg(MSG::ERROR)  << "Can not remove TGCSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Remove  TGCSimHitCollection  "<< endreq;

  //.......write new  TGCSimHitCollection
  sc=evtStore()->record(pTGCHitCollNew,m_HitName);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << " cannot record new TGCSimHitCollection " << endreq;
    return sc;
  }
  msg(MSG::DEBUG)<< "Record new container TGCSimHitCollection"<< endreq;
  return StatusCode::SUCCESS;
}


//--------------------------------------------------------
StatusCode McEventCollectionFilter::FindTRTElectronHits(){
//--------------------------------------------------------
  StatusCode sc;


  //.......retrive TRTUncompressedHitCollection collection
  m_HitName  = "TRTUncompressedHits";
  const DataHandle<TRTUncompressedHitCollection> pTRTHitColl;

  if(evtStore()->contains<TRTUncompressedHitCollection>(m_HitName)) {
    sc=evtStore()->retrieve(pTRTHitColl, m_HitName);
    if (sc.isFailure() || !pTRTHitColl) {
      msg(MSG::ERROR) << "Could not find the data object '" << m_HitName << "' !" << endreq;
      return sc;
     } else {
      msg(MSG::DEBUG) << m_HitName << " container found" << endreq;
    }
  } else {
    msg(MSG::ERROR) << "Could not find collection containing " << m_HitName << endreq;
     sc = StatusCode::FAILURE;
     return sc;
  }

  msg(MSG::DEBUG) << "Found collection containing " << m_HitName << endreq;

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
