/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauRecDetailsCnv_p1.cxx
///
/// Implementation of persistent to transient TauRecDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// $Id: TauRecDetailsCnv_p1.cxx,v 1.13 2009-01-20 17:19:01 tburgess Exp $

//Trick to access private members in tau common details
#define private public
#define protected public
#include "tauEvent/TauRecDetails.h"
#undef private
#undef protected

#include "EventCommonTPCnv/HepLorentzVectorCnv_p1.h"
#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p1.h"
#include "tauEventTPCnv/VectorUtils.h"
#include "tauEventTPCnv/TauRecDetailsCnv_p1.h"

static HepLorentzVectorCnv_p1 hepLorentzVectorCnv;

static ElementLinkVectorCnv_p1<
    ElementLinkVector<
	Rec::TrackParticleContainer> > tracksCnv;

TauRecDetailsCnv_p1::TauRecDetailsCnv_p1() :
    T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::TauRecDetails,
    TauRecDetails_p1 >(),
    m_recVertexCnv(0)
{
}

TauRecDetailsCnv_p1::~TauRecDetailsCnv_p1()
{
}

void TauRecDetailsCnv_p1::persToTrans(
    const TauRecDetails_p1 *pers,
    Analysis::TauRecDetails *trans,
    MsgStream &msg )
{
    trans->m_emRadius=pers->m_emRadius;
    trans->m_hadRadius=pers->m_hadRadius;
    trans->m_sumEmCellEt=pers->m_sumEmCellEt;
    trans->m_sumHadCellEt=pers->m_sumHadCellEt;
    trans->m_ET12Frac=pers->m_ET12Frac;
    trans->m_centralityFraction=pers->m_centralityFraction;
    trans->m_stripWidth2=pers->m_stripWidth2;
    trans->m_numStripCells=pers->m_numStripCells;
    trans->m_etEMCalib=pers->m_etEMCalib;
    trans->m_etHadCalib=pers->m_etHadCalib;
    AssignVector( trans->m_trackCaloEta, pers->m_trackCaloEta );
    AssignVector( trans->m_trackCaloPhi, pers->m_trackCaloPhi );
    trans->m_leadingTrackPT=pers->m_leadingTrackPT;
    trans->m_trFlightPathSig=pers->m_trFlightPathSig;
    hepLorentzVectorCnv.persToTrans( 
	&pers->m_sumEM, &trans->m_sumEM, msg );
    trans->m_secVertex=createTransFromPStore( 
	&m_recVertexCnv, pers->m_secVertex, msg );
    trans->m_etaCalo=pers->m_etaCalo;
    trans->m_phiCalo=pers->m_phiCalo;
    trans->m_ipSigLeadTrack=pers->m_ipSigLeadTrack;
    trans->m_etOverPtLeadTrack=pers->m_etOverPtLeadTrack;
    trans->m_nTracksdrdR=pers->m_nTracksdrdR;
    trans->m_chargeLooseTracks=pers->m_chargeLooseTracks;
    tracksCnv.persToTrans( 
	&pers->m_looseTracks, &trans->m_looseTracks,msg );
}

void TauRecDetailsCnv_p1::transToPers
   (const Analysis::TauRecDetails */*trans*/,
    TauRecDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "TauRecDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endreq;
    return;
}
