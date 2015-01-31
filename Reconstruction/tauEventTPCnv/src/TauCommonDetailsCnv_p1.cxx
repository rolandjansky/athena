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

//trick to access private members in tau common details
#define private public
#define protected public
#include "tauEvent/TauCommonDetails.h"
#undef private
#undef protected

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
    trans->m_ipZ0SinThetaSigLeadTrk=pers->m_ipZ0SinThetaSigLeadTrk;   
    trans->m_ipZ0SinThetaSigLeadTrk=pers->m_ipZ0SinThetaSigLeadTrk;
    trans->m_etOverPtLeadTrk=pers->m_etOverPtLeadTrk;
    trans->m_etOverPtLeadLooseTrk=pers->m_etOverPtLeadLooseTrk;
    trans->m_leadTrkPt=pers->m_leadTrkPt;
    trans->m_leadLooseTrkPt=pers->m_leadLooseTrkPt;
    trans->m_ipSigLeadTrk=pers->m_ipSigLeadTrk;
    trans->m_ipSigLeadLooseTrk=pers->m_ipSigLeadLooseTrk;
    tracksCnv.persToTrans( 
 	&pers->m_looseTrk, 
 	&trans->m_looseTrk, 
 	msg );
    conversionTracksCnv.persToTrans( 
 	&pers->m_looseConvTrk, 
 	&trans->m_looseConvTrk, 
 	msg );
    trans->m_chrgLooseTrk=pers->m_chrgLooseTrk;
    clusterCnv.persToTrans( 
 	&pers->m_cellEM012Cluster, 
 	&trans->m_cellEM012Cluster, 
 	msg );
    hepLorentzVectorCnv.persToTrans(
	&pers->m_sumPi0Vec, 
	&trans->m_sumPi0Vec,
	msg);
    trans->m_massTrkSys=pers->m_massTrkSys;
    trans->m_trkWidth2=pers->m_trkWidth2;
    trans->m_trFlightPathSig=pers->m_trFlightPathSig;
    trans->m_secVtx = createTransFromPStore( 
 	&m_recVertexCnv, 
 	pers->m_secVtx, 
 	msg);
    trans->m_etEflow=pers->m_etEflow;
    trans->m_mEflow=pers->m_mEflow;
    clusterVectCnv.persToTrans( 
 	&pers->m_pi0, 
 	&trans->m_pi0,
 	msg );
    trans->m_Ele_E237E277=pers->m_Ele_E237E277;
    trans->m_Ele_PresamplerFraction=pers->m_Ele_PresamplerFraction;
    trans->m_Ele_ECALFirstFraction=pers->m_Ele_ECALFirstFraction;
    trans->m_numCells=pers->m_numCells;   
    trans->m_numTopoClusters=pers->m_numTopoClusters;   
    trans->m_numEffTopoClusters=pers->m_numEffTopoClusters;   
    trans->m_topoInvMass=pers->m_topoInvMass;   
    trans->m_effTopoInvMass=pers->m_effTopoInvMass;   
    trans->m_topoMeanDeltaR=pers->m_topoMeanDeltaR;   
    trans->m_effTopoMeanDeltaR=pers->m_effTopoMeanDeltaR;   
    trans->m_seedCalo_nIsolLooseTrk=pers->m_seedCalo_nIsolLooseTrk;
    trans->m_seedCalo_EMRadius=pers->m_seedCalo_EMRadius;
    trans->m_seedCalo_hadRadius=pers->m_seedCalo_hadRadius;
    trans->m_seedCalo_etEMAtEMScale=pers->m_seedCalo_etEMAtEMScale;
    trans->m_seedCalo_etHadAtEMScale=pers->m_seedCalo_etHadAtEMScale;
    trans->m_seedCalo_isolFrac=pers->m_seedCalo_isolFrac;
    trans->m_seedCalo_centFrac=pers->m_seedCalo_centFrac;
    trans->m_seedCalo_stripWidth2=pers->m_seedCalo_stripWidth2;
    trans->m_seedCalo_nStrip=pers->m_seedCalo_nStrip;
    trans->m_seedCalo_etEMCalib=pers->m_seedCalo_etEMCalib;
    trans->m_seedCalo_etHadCalib=pers->m_seedCalo_etHadCalib;
    trans->m_seedCalo_eta=pers->m_seedCalo_eta;   
    trans->m_seedCalo_phi=pers->m_seedCalo_phi;
    trans->m_seedCalo_trkAvgDist=pers->m_seedCalo_trkAvgDist;
    trans->m_seedCalo_trkRmsDist=pers->m_seedCalo_trkRmsDist;
    trans->m_seedTrk_EMRadius=pers->m_seedTrk_EMRadius;
    trans->m_seedTrk_isolFrac=pers->m_seedTrk_isolFrac;
    trans->m_seedTrk_etChrgHadOverSumTrkPt=
	pers->m_seedTrk_etChrgHadOverSumTrkPt;
    trans->m_seedTrk_isolFracWide=pers->m_seedTrk_isolFracWide;
    trans->m_seedTrk_etHadAtEMScale=pers->m_seedTrk_etHadAtEMScale;
    trans->m_seedTrk_etEMAtEMScale=pers->m_seedTrk_etEMAtEMScale;
    trans->m_seedTrk_etEMCL=pers->m_seedTrk_etEMCL;
    trans->m_seedTrk_etChrgEM=pers->m_seedTrk_etChrgEM;
    trans->m_seedTrk_etNeuEM=pers->m_seedTrk_etNeuEM;
    trans->m_seedTrk_etResNeuEM=pers->m_seedTrk_etResNeuEM;
    trans->m_seedTrk_hadLeakEt=pers->m_seedTrk_hadLeakEt;
    AssignVector( 
	trans->m_seedTrk_etChrgEM01Trk, 
	pers->m_seedTrk_etChrgEM01Trk);
    AssignVector( 
	trans->m_seedTrk_etResChrgEMTrk,
	pers->m_seedTrk_etResChrgEMTrk );
    trans->m_seedTrk_sumEMCellEtOverLeadTrkPt=
	pers->m_seedTrk_sumEMCellEtOverLeadTrkPt;
    trans->m_seedTrk_secMaxStripEt=pers->m_seedTrk_secMaxStripEt;
    trans->m_seedTrk_stripWidth2=pers->m_seedTrk_stripWidth2;
    trans->m_seedTrk_nStrip=pers->m_seedTrk_nStrip;
    trans->m_seedTrk_etChrgHad=pers->m_seedTrk_etChrgHad;
    trans->m_seedTrk_nOtherCoreTrk=pers->m_seedTrk_nOtherCoreTrk;
    trans->m_seedTrk_nIsolTrk=pers->m_seedTrk_nIsolTrk;
    trans->m_seedTrk_etIsolEM=pers->m_seedTrk_etIsolEM;
    trans->m_seedTrk_etIsolHad=pers->m_seedTrk_etIsolHad;
}

void TauCommonDetailsCnv_p1::transToPers( 
    const Analysis::TauCommonDetails *trans,
    TauCommonDetails_p1 *pers,
    MsgStream &msg )
{
    pers->m_ipZ0SinThetaSigLeadTrk=trans->m_ipZ0SinThetaSigLeadTrk;   
    pers->m_ipZ0SinThetaSigLeadTrk=trans->m_ipZ0SinThetaSigLeadTrk;
    pers->m_etOverPtLeadTrk=trans->m_etOverPtLeadTrk;
    pers->m_etOverPtLeadLooseTrk=trans->m_etOverPtLeadLooseTrk;
    pers->m_leadTrkPt=trans->m_leadTrkPt;
    pers->m_leadLooseTrkPt=trans->m_leadLooseTrkPt;
    pers->m_ipSigLeadTrk=trans->m_ipSigLeadTrk;
    pers->m_ipSigLeadLooseTrk=trans->m_ipSigLeadLooseTrk;
    tracksCnv.transToPers( 
	&trans->m_looseTrk, 
	&pers->m_looseTrk, 
	msg );
    conversionTracksCnv.transToPers( 
	&trans->m_looseConvTrk, 
	&pers->m_looseConvTrk, 
	msg );
    pers->m_chrgLooseTrk=trans->m_chrgLooseTrk;
    clusterCnv.transToPers( 
	&trans->m_cellEM012Cluster, 
	&pers->m_cellEM012Cluster, 
	msg );
    hepLorentzVectorCnv.transToPers(
	&trans->m_sumPi0Vec, 
	&pers->m_sumPi0Vec,
	msg );
    pers->m_massTrkSys=trans->m_massTrkSys;
    pers->m_trkWidth2=trans->m_trkWidth2;
    pers->m_trFlightPathSig=trans->m_trFlightPathSig;
    pers->m_secVtx = toPersistent( 
	&m_recVertexCnv, 
	trans->m_secVtx,
	msg);
    pers->m_etEflow=trans->m_etEflow;
    pers->m_mEflow=trans->m_mEflow;
    clusterVectCnv.transToPers( 
	&trans->m_pi0, 
	&pers->m_pi0, 
	msg );
    pers->m_Ele_E237E277=trans->m_Ele_E237E277;
    pers->m_Ele_PresamplerFraction=trans->m_Ele_PresamplerFraction;
    pers->m_Ele_ECALFirstFraction=trans->m_Ele_ECALFirstFraction;
    pers->m_numCells=trans->m_numCells;   
    pers->m_numTopoClusters=trans->m_numTopoClusters;   
    pers->m_numEffTopoClusters=trans->m_numEffTopoClusters;   
    pers->m_topoInvMass=trans->m_topoInvMass;   
    pers->m_effTopoInvMass=trans->m_effTopoInvMass;   
    pers->m_topoMeanDeltaR=trans->m_topoMeanDeltaR;   
    pers->m_effTopoMeanDeltaR=trans->m_effTopoMeanDeltaR;   
    pers->m_seedCalo_nIsolLooseTrk=trans->m_seedCalo_nIsolLooseTrk;
    pers->m_seedCalo_EMRadius=trans->m_seedCalo_EMRadius;
    pers->m_seedCalo_hadRadius=trans->m_seedCalo_hadRadius;
    pers->m_seedCalo_etEMAtEMScale=trans->m_seedCalo_etEMAtEMScale;
    pers->m_seedCalo_etHadAtEMScale=trans->m_seedCalo_etHadAtEMScale;
    pers->m_seedCalo_isolFrac=trans->m_seedCalo_isolFrac;
    pers->m_seedCalo_centFrac=trans->m_seedCalo_centFrac;
    pers->m_seedCalo_stripWidth2=trans->m_seedCalo_stripWidth2;
    pers->m_seedCalo_nStrip=trans->m_seedCalo_nStrip;
    pers->m_seedCalo_etEMCalib=trans->m_seedCalo_etEMCalib;
    pers->m_seedCalo_etHadCalib=trans->m_seedCalo_etHadCalib;
    pers->m_seedCalo_eta=trans->m_seedCalo_eta;   
    pers->m_seedCalo_phi=trans->m_seedCalo_phi;
    pers->m_seedCalo_trkAvgDist=trans->m_seedCalo_trkAvgDist;
    pers->m_seedCalo_trkRmsDist=trans->m_seedCalo_trkRmsDist;
    pers->m_seedTrk_EMRadius=trans->m_seedTrk_EMRadius;
    pers->m_seedTrk_isolFrac=trans->m_seedTrk_isolFrac;
    pers->m_seedTrk_etChrgHadOverSumTrkPt=
	trans->m_seedTrk_etChrgHadOverSumTrkPt;
    pers->m_seedTrk_isolFracWide=trans->m_seedTrk_isolFracWide;
    pers->m_seedTrk_etHadAtEMScale=trans->m_seedTrk_etHadAtEMScale;
    pers->m_seedTrk_etEMAtEMScale=trans->m_seedTrk_etEMAtEMScale;
    pers->m_seedTrk_etEMCL=trans->m_seedTrk_etEMCL;
    pers->m_seedTrk_etChrgEM=trans->m_seedTrk_etChrgEM;
    pers->m_seedTrk_etNeuEM=trans->m_seedTrk_etNeuEM;
    pers->m_seedTrk_etResNeuEM=trans->m_seedTrk_etResNeuEM;
    pers->m_seedTrk_hadLeakEt=trans->m_seedTrk_hadLeakEt;
    AssignVector( 
	pers->m_seedTrk_etChrgEM01Trk, 
	trans->m_seedTrk_etChrgEM01Trk);
    AssignVector( 
	pers->m_seedTrk_etResChrgEMTrk, 
	trans->m_seedTrk_etResChrgEMTrk );
    pers->m_seedTrk_sumEMCellEtOverLeadTrkPt=
	trans->m_seedTrk_sumEMCellEtOverLeadTrkPt;
    pers->m_seedTrk_secMaxStripEt=trans->m_seedTrk_secMaxStripEt;
    pers->m_seedTrk_stripWidth2=trans->m_seedTrk_stripWidth2;
    pers->m_seedTrk_nStrip=trans->m_seedTrk_nStrip;
    pers->m_seedTrk_etChrgHad=trans->m_seedTrk_etChrgHad;
    pers->m_seedTrk_nOtherCoreTrk=trans->m_seedTrk_nOtherCoreTrk;
    pers->m_seedTrk_nIsolTrk=trans->m_seedTrk_nIsolTrk;
    pers->m_seedTrk_etIsolEM=trans->m_seedTrk_etIsolEM;
    pers->m_seedTrk_etIsolHad=trans->m_seedTrk_etIsolHad;
}
