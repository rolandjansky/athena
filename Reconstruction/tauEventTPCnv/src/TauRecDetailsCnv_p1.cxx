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

#include "tauEvent/TauRecDetails.h"
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
    trans->setEMRadius (pers->m_emRadius);
    trans->setHadRadius (pers->m_hadRadius);
    trans->setSumEmCellEt (pers->m_sumEmCellEt);
    trans->setSumHadCellEt (pers->m_sumHadCellEt);
    trans->setIsolationFraction (pers->m_ET12Frac);
    trans->setCentralityFraction (pers->m_centralityFraction);
    trans->setStripWidth2 (pers->m_stripWidth2);
    trans->setNumStripCells (pers->m_numStripCells);
    trans->setETEMCalib (pers->m_etEMCalib);
    trans->setETHadCalib (pers->m_etHadCalib);

    trans->setNumTrack (pers->m_trackCaloEta.size());
    for (size_t i = 0; i < pers->m_trackCaloEta.size(); i++) {
      trans->setTrackCaloEta (i, pers->m_trackCaloEta[i]);
      trans->setTrackCaloPhi (i, pers->m_trackCaloPhi[i]);
    }

    trans->setLeadingTrackPT (pers->m_leadingTrackPT);
    trans->setTrFlightPathSig (pers->m_trFlightPathSig);

    CLHEP::HepLorentzVector sumEM;
    hepLorentzVectorCnv.persToTrans( &pers->m_sumEM, &sumEM, msg );
    trans->setSumEM (sumEM);
    trans->setSecVertex (createTransFromPStore 
                         ( &m_recVertexCnv, pers->m_secVertex, msg ));

    trans->setEtaCalo (pers->m_etaCalo);
    trans->setPhiCalo (pers->m_phiCalo);
    trans->setIpSigLeadTrack (pers->m_ipSigLeadTrack);
    trans->setEtOverPtLeadTrack (pers->m_etOverPtLeadTrack);
    trans->setNTracksdrdR (pers->m_nTracksdrdR);
    trans->setChargeLooseTracks (pers->m_chargeLooseTracks);

    tracksCnv.persToTrans( 
                          &pers->m_looseTracks, &trans->looseTracks(),msg );
}

void TauRecDetailsCnv_p1::transToPers
   (const Analysis::TauRecDetails */*trans*/,
    TauRecDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "TauRecDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endmsg;
    return;
}
