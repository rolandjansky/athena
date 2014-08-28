/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "TopInputsTPCnv/TopInputsCnv_p2.h"

    
void TopInputsCnv_p2::transToPers( const TopRec::TopInputs* transObj,
				   TopInputs_p2* persObj,
				   MsgStream &msg) {

  msg << MSG::DEBUG << typeid(*transObj).name() << " called" << endreq;

  TopRec::TopMuonMap muonmap = transObj->muonMap();
  TopInputMapTrigger_p1 muonmap_p1;
  m_mu_cnv.transToPers(&muonmap,&muonmap_p1,msg);
  persObj->m_muonMap_p1=muonmap_p1;

  TopRec::TopElectronMap electronmap = transObj->electronMap();
  TopInputMapTrigger_p1 electronmap_p1;
  m_el_cnv.transToPers(&electronmap,&electronmap_p1,msg);
  persObj->m_electronMap_p1=electronmap_p1;

  TopRec::TopPhotonMap photonmap = transObj->photonMap();
  TopInputMap_p1 photonmap_p1;
  m_ph_cnv.transToPers(&photonmap,&photonmap_p1,msg);
  persObj->m_photonMap_p1=photonmap_p1;

  TopRec::TopTauJetMap taujetmap = transObj->tauJetMap();
  TopInputMap_p1 taujetmap_p1;
  m_tau_cnv.transToPers(&taujetmap,&taujetmap_p1,msg);
  persObj->m_tauJetMap_p1=taujetmap_p1;

  TopRec::TopJetMap jetmap = transObj->jetMap();
  TopInputMap_p1 jetmap_p1;
  m_jet_cnv.transToPers(&jetmap,&jetmap_p1,msg);
  persObj->m_jetMap_p1=jetmap_p1;

  TopRec::TopBJetMap bJetmap = transObj->bJetMap();
  TopInputMap_p1 bJetmap_p1;
  m_jet_cnv.transToPers(&bJetmap,&bJetmap_p1,msg);
  persObj->m_bJetMap_p1=bJetmap_p1;

  TopRec::TopTrackMap trackmap = transObj->trackMap();
  TopInputMap_p1 trackmap_p1;
  m_track_cnv.transToPers(&trackmap,&trackmap_p1,msg);
  persObj->m_trackMap_p1=trackmap_p1;

  DataLink<MissingET> dl = transObj->metLink();
  DataLink_p1 dl_p1;
  m_dl_cnv.transToPers(&dl,&dl_p1,msg);
  persObj->m_metLink_p1=dl_p1;

  msg << MSG::DEBUG << "Created persistent state of " 
      << typeid(*transObj).name() << endreq;
}

void TopInputsCnv_p2::persToTrans( const TopInputs_p2* persObj,
				   TopRec::TopInputs* transObj,
				   MsgStream &msg ) {
  msg << MSG::DEBUG << "In TopInputsCnv_p2::persToTrans()" << endreq;

  TopRec::TopMuonMap muonmap;
  TopInputMapTrigger_p1 muonmap_p1 = persObj->m_muonMap_p1;
  m_mu_cnv.persToTrans(&muonmap_p1,&muonmap,msg);
  transObj->setMuonMap(muonmap);

  TopRec::TopElectronMap electronmap;
  TopInputMapTrigger_p1 electronmap_p1 = persObj->m_electronMap_p1;
  m_el_cnv.persToTrans(&electronmap_p1,&electronmap,msg);
  transObj->setElectronMap(electronmap);

  TopRec::TopPhotonMap photonmap;
  TopInputMap_p1 photonmap_p1 = persObj->m_photonMap_p1;
  m_ph_cnv.persToTrans(&photonmap_p1,&photonmap,msg);
  transObj->setPhotonMap(photonmap);

  TopRec::TopTauJetMap taujetmap;
  TopInputMap_p1 taujetmap_p1 = persObj->m_tauJetMap_p1;
  m_tau_cnv.persToTrans(&taujetmap_p1,&taujetmap,msg);
  transObj->setTauJetMap(taujetmap);  

  TopRec::TopJetMap jetmap;
  TopInputMap_p1 jetmap_p1 = persObj->m_jetMap_p1;
  m_jet_cnv.persToTrans(&jetmap_p1,&jetmap,msg);
  transObj->setJetMap(jetmap);

  TopRec::TopBJetMap bJetmap;
  TopInputMap_p1 bJetmap_p1 = persObj->m_bJetMap_p1;
  m_jet_cnv.persToTrans(&bJetmap_p1,&bJetmap,msg);
  transObj->setBJetMap(bJetmap);

  TopRec::TopTrackMap trackmap;
  TopInputMap_p1 trackmap_p1 = persObj->m_trackMap_p1;
  m_track_cnv.persToTrans(&trackmap_p1,&trackmap,msg);
  transObj->setTrackMap(trackmap);

  DataLink<MissingET> dl;
  DataLink_p1 dl_p1=persObj->m_metLink_p1;
  m_dl_cnv.persToTrans(&dl_p1,&dl,msg);
  dl.toTransient();
  transObj->setMETLink(dl);
  
  msg << MSG::DEBUG << "Loading " << typeid(*transObj).name() << endreq;
}
