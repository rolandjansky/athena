/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetailsCnv_p1.cxx
/// 
/// Implementation of transient<->persistent converter 
/// for Analysis::TauCommonDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonDetailsCnv_p1.cxx,v 1.7 2009-03-02 17:20:51 binet Exp $

#include "tauEvent/TauCommonDetails.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "EventCommonTPCnv/HepLorentzVector_p1.h"
#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "tauEventTPCnv/VectorUtils.h"
#include "tauEventTPCnv/TauCommonDetailsCnv_p1.h"

//Pre allocate converters
static HepLorentzVectorCnv_p1 hepLorentzVectorCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> >
tracksCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<Rec::TrackParticleContainer> >
conversionTracksCnv;
static ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > clusterCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<CaloClusterContainer> > 
clusterVectCnv;

TauCommonDetailsCnv_p1::TauCommonDetailsCnv_p1() 
    :  T_AthenaPoolTPPolyCnvBase<Analysis::TauDetails, 
                                 Analysis::TauCommonDetails,
			         TauCommonDetails_p1>(),
       m_recVertexCnv(0)       
{
    
}

TauCommonDetailsCnv_p1::~TauCommonDetailsCnv_p1()
{

}

void TauCommonDetailsCnv_p1::persToTrans( 
    const TauCommonDetails_p1 *pers,
    Analysis::TauCommonDetails *trans,
    MsgStream &msg )
{
    trans->setIpZ0SinThetaSigLeadTrk (pers->m_ipZ0SinThetaSigLeadTrk);
    trans->setEtOverPtLeadTrk (pers->m_etOverPtLeadTrk);
    trans->setEtOverPtLeadLooseTrk (pers->m_etOverPtLeadLooseTrk);
    trans->setLeadTrkPt (pers->m_leadTrkPt);
    trans->setLeadLooseTrkPt (pers->m_leadLooseTrkPt);
    trans->setIpSigLeadTrk (pers->m_ipSigLeadTrk);
    trans->setIpSigLeadLooseTrk (pers->m_ipSigLeadLooseTrk);
    tracksCnv.persToTrans( 
 	&pers->m_looseTrk, 
 	&trans->looseTrk(), 
 	msg );
    conversionTracksCnv.persToTrans( 
 	&pers->m_looseConvTrk, 
 	&trans->looseConvTrk(), 
 	msg );
    trans->setChrgLooseTrk (pers->m_chrgLooseTrk);
    clusterCnv.persToTrans( 
 	&pers->m_cellEM012Cluster, 
 	&trans->cellEM012ClusterLink(), 
 	msg );
    CLHEP::HepLorentzVector sumPi0Vec;
    hepLorentzVectorCnv.persToTrans(
	&pers->m_sumPi0Vec, 
	&sumPi0Vec,
	msg);
    trans->setSumPi0Vec (sumPi0Vec);
    trans->setMassTrkSys (pers->m_massTrkSys);
    trans->setTrkWidth2 (pers->m_trkWidth2);
    trans->setTrFlightPathSig (pers->m_trFlightPathSig);
    trans->setSecVtx (createTransFromPStore( 
                                            &m_recVertexCnv, 
                                            pers->m_secVtx, 
                                            msg));
    trans->setEtEflow (pers->m_etEflow);
    trans->setMEflow (pers->m_mEflow);
    clusterVectCnv.persToTrans( 
 	&pers->m_pi0, 
 	&trans->pi0LinkVec(),
 	msg );
    trans->setEle_E237E277 (pers->m_Ele_E237E277);
    trans->setEle_PresamplerFraction (pers->m_Ele_PresamplerFraction);
    trans->setEle_ECALFirstFraction (pers->m_Ele_ECALFirstFraction);
    trans->setNumCells (pers->m_numCells);
    trans->setNumTopoClusters (pers->m_numTopoClusters);
    trans->setNumEffTopoClusters (pers->m_numEffTopoClusters);
    trans->setTopoInvMass (pers->m_topoInvMass);
    trans->setEffTopoInvMass (pers->m_effTopoInvMass);
    trans->setTopoMeanDeltaR (pers->m_topoMeanDeltaR);
    trans->setEffTopoMeanDeltaR (pers->m_effTopoMeanDeltaR);
    trans->setSeedCalo_nIsolLooseTrk (pers->m_seedCalo_nIsolLooseTrk);
    trans->setSeedCalo_EMRadius (pers->m_seedCalo_EMRadius);
    trans->setSeedCalo_hadRadius (pers->m_seedCalo_hadRadius);
    trans->setSeedCalo_etEMAtEMScale (pers->m_seedCalo_etEMAtEMScale);
    trans->setSeedCalo_etHadAtEMScale (pers->m_seedCalo_etHadAtEMScale);
    trans->setSeedCalo_isolFrac (pers->m_seedCalo_isolFrac);
    trans->setSeedCalo_centFrac (pers->m_seedCalo_centFrac);
    trans->setSeedCalo_stripWidth2 (pers->m_seedCalo_stripWidth2);
    trans->setSeedCalo_nStrip (pers->m_seedCalo_nStrip);
    trans->setSeedCalo_etEMCalib (pers->m_seedCalo_etEMCalib);
    trans->setSeedCalo_etHadCalib (pers->m_seedCalo_etHadCalib);
    trans->setSeedCalo_eta (pers->m_seedCalo_eta);
    trans->setSeedCalo_phi (pers->m_seedCalo_phi);
    trans->setSeedCalo_trkAvgDist (pers->m_seedCalo_trkAvgDist);
    trans->setSeedCalo_trkRmsDist (pers->m_seedCalo_trkRmsDist);
    trans->setSeedTrk_EMRadius (pers->m_seedTrk_EMRadius);
    trans->setSeedTrk_isolFrac (pers->m_seedTrk_isolFrac);
    trans->setSeedTrk_etChrgHadOverSumTrkPt
      (pers->m_seedTrk_etChrgHadOverSumTrkPt);
    trans->setSeedTrk_isolFracWide (pers->m_seedTrk_isolFracWide);
    trans->setSeedTrk_etHadAtEMScale (pers->m_seedTrk_etHadAtEMScale);
    trans->setSeedTrk_etEMAtEMScale (pers->m_seedTrk_etEMAtEMScale);
    trans->setSeedTrk_etEMCL (pers->m_seedTrk_etEMCL);
    trans->setSeedTrk_etChrgEM (pers->m_seedTrk_etChrgEM);
    trans->setSeedTrk_etNeuEM (pers->m_seedTrk_etNeuEM);
    trans->setSeedTrk_etResNeuEM (pers->m_seedTrk_etResNeuEM);
    trans->setSeedTrk_hadLeakEt (pers->m_seedTrk_hadLeakEt);

    trans->setSeedTrk_etChrgEM01Trk (pers->m_seedTrk_etChrgEM01Trk.begin(),
                                     pers->m_seedTrk_etChrgEM01Trk.end());
    trans->setSeedTrk_etResChrgEMTrk (pers->m_seedTrk_etResChrgEMTrk.begin(),
                                      pers->m_seedTrk_etResChrgEMTrk.end());

    trans->setSeedTrk_sumEMCellEtOverLeadTrkPt
      (pers->m_seedTrk_sumEMCellEtOverLeadTrkPt);
    trans->setSeedTrk_secMaxStripEt (pers->m_seedTrk_secMaxStripEt);
    trans->setSeedTrk_stripWidth2 (pers->m_seedTrk_stripWidth2);
    trans->setSeedTrk_nStrip (pers->m_seedTrk_nStrip);
    trans->setSeedTrk_etChrgHad (pers->m_seedTrk_etChrgHad);
    trans->setSeedTrk_nOtherCoreTrk (pers->m_seedTrk_nOtherCoreTrk);
    trans->setSeedTrk_nIsolTrk (pers->m_seedTrk_nIsolTrk);
    trans->setSeedTrk_etIsolEM (pers->m_seedTrk_etIsolEM);
    trans->setSeedTrk_etIsolHad (pers->m_seedTrk_etIsolHad);
}

void TauCommonDetailsCnv_p1::transToPers( 
    const Analysis::TauCommonDetails *trans,
    TauCommonDetails_p1 *pers,
    MsgStream &msg )
{
    pers->m_ipZ0SinThetaSigLeadTrk=trans->ipZ0SinThetaSigLeadTrk();   
    pers->m_etOverPtLeadTrk=trans->etOverPtLeadTrk();
    pers->m_etOverPtLeadLooseTrk=trans->etOverPtLeadLooseTrk();
    pers->m_leadTrkPt=trans->leadTrkPt();
    pers->m_leadLooseTrkPt=trans->leadLooseTrkPt();
    pers->m_ipSigLeadTrk=trans->ipSigLeadTrk();
    pers->m_ipSigLeadLooseTrk=trans->ipSigLeadLooseTrk();
    tracksCnv.transToPers( 
        &trans->looseTrk(), 
	&pers->m_looseTrk, 
	msg );
    conversionTracksCnv.transToPers( 
        &trans->looseConvTrk(), 
	&pers->m_looseConvTrk, 
	msg );
    pers->m_chrgLooseTrk=trans->chrgLooseTrk();
    clusterCnv.transToPers( 
        &trans->cellEM012ClusterLink(), 
	&pers->m_cellEM012Cluster, 
	msg );
    hepLorentzVectorCnv.transToPers(
        &trans->sumPi0Vec(), 
	&pers->m_sumPi0Vec,
	msg );
    pers->m_massTrkSys=trans->massTrkSys();
    pers->m_trkWidth2=trans->trkWidth2();
    pers->m_trFlightPathSig=trans->trFlightPathSig();
    pers->m_secVtx = toPersistent( 
	&m_recVertexCnv, 
	trans->secVtx(),
	msg);
    pers->m_etEflow=trans->etEflow();
    pers->m_mEflow=trans->mEflow();
    clusterVectCnv.transToPers( 
        &trans->pi0LinkVec(), 
	&pers->m_pi0, 
	msg );
    pers->m_Ele_E237E277=trans->ele_E237E277();
    pers->m_Ele_PresamplerFraction=trans->ele_PresamplerFraction();
    pers->m_Ele_ECALFirstFraction=trans->ele_ECALFirstFraction();
    pers->m_numCells=trans->numCells();   
    pers->m_numTopoClusters=trans->numTopoClusters();   
    pers->m_numEffTopoClusters=trans->numEffTopoClusters();   
    pers->m_topoInvMass=trans->topoInvMass();   
    pers->m_effTopoInvMass=trans->effTopoInvMass();   
    pers->m_topoMeanDeltaR=trans->topoMeanDeltaR();   
    pers->m_effTopoMeanDeltaR=trans->effTopoMeanDeltaR();   
    pers->m_seedCalo_nIsolLooseTrk=trans->seedCalo_nIsolLooseTrk();
    pers->m_seedCalo_EMRadius=trans->seedCalo_EMRadius();
    pers->m_seedCalo_hadRadius=trans->seedCalo_hadRadius();
    pers->m_seedCalo_etEMAtEMScale=trans->seedCalo_etEMAtEMScale();
    pers->m_seedCalo_etHadAtEMScale=trans->seedCalo_etHadAtEMScale();
    pers->m_seedCalo_isolFrac=trans->seedCalo_isolFrac();
    pers->m_seedCalo_centFrac=trans->seedCalo_centFrac();
    pers->m_seedCalo_stripWidth2=trans->seedCalo_stripWidth2();
    pers->m_seedCalo_nStrip=trans->seedCalo_nStrip();
    pers->m_seedCalo_etEMCalib=trans->seedCalo_etEMCalib();
    pers->m_seedCalo_etHadCalib=trans->seedCalo_etHadCalib();
    pers->m_seedCalo_eta=trans->seedCalo_eta();   
    pers->m_seedCalo_phi=trans->seedCalo_phi();
    pers->m_seedCalo_trkAvgDist=trans->seedCalo_trkAvgDist();
    pers->m_seedCalo_trkRmsDist=trans->seedCalo_trkRmsDist();
    pers->m_seedTrk_EMRadius=trans->seedTrk_EMRadius();
    pers->m_seedTrk_isolFrac=trans->seedTrk_isolFrac();
    pers->m_seedTrk_etChrgHadOverSumTrkPt=
      trans->seedTrk_etChrgHadOverSumTrkPt();
    pers->m_seedTrk_isolFracWide=trans->seedTrk_isolFracWide();
    pers->m_seedTrk_etHadAtEMScale=trans->seedTrk_etHadAtEMScale();
    pers->m_seedTrk_etEMAtEMScale=trans->seedTrk_etEMAtEMScale();
    pers->m_seedTrk_etEMCL=trans->seedTrk_etEMCL();
    pers->m_seedTrk_etChrgEM=trans->seedTrk_etChrgEM();
    pers->m_seedTrk_etNeuEM=trans->seedTrk_etNeuEM();
    pers->m_seedTrk_etResNeuEM=trans->seedTrk_etResNeuEM();
    pers->m_seedTrk_hadLeakEt=trans->seedTrk_hadLeakEt();

    AssignVector( 
	pers->m_seedTrk_etChrgEM01Trk, 
	trans->seedTrk_etChrgEM01TrkVector());
    AssignVector( 
	pers->m_seedTrk_etResChrgEMTrk, 
	trans->seedTrk_etResChrgEMTrkVector() );

    pers->m_seedTrk_sumEMCellEtOverLeadTrkPt=
      trans->seedTrk_sumEMCellEtOverLeadTrkPt();
    pers->m_seedTrk_secMaxStripEt=trans->seedTrk_secMaxStripEt();
    pers->m_seedTrk_stripWidth2=trans->seedTrk_stripWidth2();
    pers->m_seedTrk_nStrip=trans->seedTrk_nStrip();
    pers->m_seedTrk_etChrgHad=trans->seedTrk_etChrgHad();
    pers->m_seedTrk_nOtherCoreTrk=trans->seedTrk_nOtherCoreTrk();
    pers->m_seedTrk_nIsolTrk=trans->seedTrk_nIsolTrk();
    pers->m_seedTrk_etIsolEM=trans->seedTrk_etIsolEM();
    pers->m_seedTrk_etIsolHad=trans->seedTrk_etIsolHad();
}
