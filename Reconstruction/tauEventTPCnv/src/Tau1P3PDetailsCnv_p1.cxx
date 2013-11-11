/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PDetailsCnv_p1.cxx
///
/// Implementation of persistent to transient Tau1P3PDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: Tau1P3PDetailsCnv_p1.cxx,v 1.9 2009-01-20 17:19:01 tburgess Exp $

//Trick to access private members in tau 1P3P details
#define private public
#define protected public
#include "tauEvent/Tau1P3PDetails.h"
#undef private
#undef protected

#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "tauEventTPCnv/VectorUtils.h"
#include "tauEventTPCnv/Tau1P3PDetails_p1.h"
#include "tauEventTPCnv/Tau1P3PDetailsCnv_p1.h"

///Converter for calo cluster container
static ElementLinkCnv_p1<ElementLink<CaloClusterContainer> > clusterCnv;

///Converter for cluster vector
static ElementLinkVectorCnv_p1<ElementLinkVector<CaloClusterContainer> > clusterVectCnv;

///Converter for Hep lorentz vector
static HepLorentzVectorCnv_p1 hepLorentzVectorCnv;

Tau1P3PDetailsCnv_p1::Tau1P3PDetailsCnv_p1() :
    T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::Tau1P3PDetails,
    Tau1P3PDetails_p1 >(),
    m_recVertexCnv(0)
{
}

Tau1P3PDetailsCnv_p1::~Tau1P3PDetailsCnv_p1()
{
}

void Tau1P3PDetailsCnv_p1::persToTrans(
    const Tau1P3PDetails_p1 *pers,
    Analysis::Tau1P3PDetails *trans,
    MsgStream &msg )
{
    trans->m_numStripCells=pers->m_numStripCells;
    trans->m_stripWidth2=pers->m_stripWidth2;
    trans->m_emRadius=pers->m_emRadius;
    trans->m_ET12Frac=pers->m_ET12Frac;
    trans->m_etIsolHAD=pers->m_etIsolHAD;
    trans->m_etIsolEM=pers->m_etIsolEM;
    trans->m_etChrgHAD=pers->m_etChrgHAD;
    trans->m_nAssocTracksCore=pers->m_nAssocTracksCore;
    trans->m_nAssocTracksIsol=pers->m_nAssocTracksIsol;
    trans->m_signD0Trk3P=pers->m_signD0Trk3P;
    trans->m_massTrk3P=pers->m_massTrk3P;
    trans->m_rWidth2Trk3P=pers->m_rWidth2Trk3P;
    trans->m_etHadAtEMScale=pers->m_etHadAtEMScale;
    trans->m_etEMAtEMScale=pers->m_etEMAtEMScale;
    trans->m_etEMCL =pers->m_etEMCL;
    trans->m_etChrgEM=pers->m_etChrgEM;
    trans->m_etNeuEM=pers->m_etNeuEM;
    trans->m_etResNeuEM=pers->m_etResNeuEM;
    AssignVector( 
	trans->m_etChrgEM01Trk, pers->m_etChrgEM01Trk );
    AssignVector( 
	trans->m_etResChrgEMTrk, pers->m_etResChrgEMTrk );
    trans->m_trFlightPathSig=pers->m_trFlightPathSig;
    trans->m_z0SinThetaSig=pers->m_z0SinThetaSig;
    trans->m_etChrgHADoverPttot=pers->m_etChrgHADoverPttot;
    trans->m_etIsolFrac=pers->m_etIsolFrac;
    trans->m_sumEtCellsLArOverLeadTrackPt=
	pers->m_sumEtCellsLArOverLeadTrackPt;
    trans->m_hadronicLeak=pers->m_hadronicLeak;
    trans->m_secondaryMax=pers->m_secondaryMax;

    hepLorentzVectorCnv.persToTrans( 
	&pers->m_sumEM, &trans->m_sumEM, msg );
    trans->m_secVertex=createTransFromPStore( 
	&m_recVertexCnv, pers->m_secVertex, msg );
    clusterVectCnv.persToTrans( 
	&pers->m_pi0, &trans->m_pi0, msg );
    clusterCnv.persToTrans(
	&pers->m_cellEM012Cluster, 
	&trans->m_cellEM012Cluster, msg );
    trans->m_etEflow=pers->m_etEflow;
}

void Tau1P3PDetailsCnv_p1::transToPers
   (const Analysis::Tau1P3PDetails */*trans*/,
    Tau1P3PDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "Tau1P3PDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endreq;
    return;
}
