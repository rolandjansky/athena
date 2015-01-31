/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file tauAnalysisHelperObjectCnv_p1.cxx
///
/// !!!!DEPRECATED!!!!
/// Implementation of persistent to transient tauAnalysisHelperObj converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// $Id: tauAnalysisHelperObjectCnv_p1.cxx,v 1.5 2009-05-08 14:32:44 tburgess Exp $

#define protected public
#define private public
#include "tauEvent/tauAnalysisHelperObject.h"
#undef private
#undef protected

#include "DataModelAthenaPool/ElementLinkVectorCnv_p1.h"
#include "tauEventTPCnv/tauAnalysisHelperObjectCnv_p1.h"

static ElementLinkVectorCnv_p1<ElementLinkVector<CaloCellContainer> >
cellVectCnv;
static ElementLinkVectorCnv_p1<ElementLinkVector<
				   Rec::TrackParticleContainer> >
trackVectCnv;

void tauAnalysisHelperObjectCnv_p1 :: persToTrans(
    const tauAnalysisHelperObject_p1 *persObj,
    tauAnalysisHelperObject *transObj,
    MsgStream &log )
{
    transObj->m_decmode = persObj->m_decmode;
    transObj->m_jettype = persObj->m_jettype;
    transObj->m_d0prf = persObj->m_d0prf;
    transObj->m_d0iso = persObj->m_d0iso;
    transObj->m_d0isoet = persObj->m_d0isoet;
    transObj->m_d0ettr = persObj->m_d0ettr;
    transObj->m_d0etem = persObj->m_d0etem;
    transObj->m_d0etem2 = persObj->m_d0etem2;
    transObj->m_d0emclet = persObj->m_d0emclet;
    transObj->m_d0emcleta = persObj->m_d0emcleta;
    transObj->m_d0emclphi = persObj->m_d0emclphi;
    transObj->m_d0et05 = persObj->m_d0et05;
    transObj->m_d0eta05 = persObj->m_d0eta05;
    transObj->m_d0phi05 = persObj->m_d0phi05;
    transObj->m_d0hadet = persObj->m_d0hadet;
    transObj->m_d0hadeta = persObj->m_d0hadeta;
    transObj->m_d0hadphi = persObj->m_d0hadphi;
    transObj->m_d0type = persObj->m_d0type;
    transObj->m_d0deltaR1 = persObj->m_d0deltaR1;
    transObj->m_d0eTosumpT = persObj->m_d0eTosumpT;
    transObj->m_d0deltaR1had = persObj->m_d0deltaR1had;
    transObj->m_d0em3iso = persObj->m_d0em3iso;
    transObj->m_d0mtrem3 = persObj->m_d0mtrem3;
    transObj->m_d0deltaR2 = persObj->m_d0deltaR2;
    transObj->m_d0ntr1030 = persObj->m_d0ntr1030;
    transObj->m_d0EM12isof = persObj->m_d0EM12isof;
    transObj->m_d0e1e2otaupT = persObj->m_d0e1e2otaupT;
    transObj->m_d0ettro123 = persObj->m_d0ettro123;
    transObj->m_d0ett1oEtiso = persObj->m_d0ett1oEtiso;
    transObj->m_d0ett1oEtisoet = persObj->m_d0ett1oEtisoet;
    transObj->m_d0dalpha = persObj->m_d0dalpha;
    transObj->m_d0e1e2 = persObj->m_d0e1e2;
    transObj->m_d0mtr1tr2 = persObj->m_d0mtr1tr2;
    transObj->m_d0mtr1tr2tr3 = persObj->m_d0mtr1tr2tr3;
    transObj->m_d0sumtaupt = persObj->m_d0sumtaupt;
    transObj->m_d0sumnontaupt = persObj->m_d0sumnontaupt;
    transObj->m_d0sumpt = persObj->m_d0sumpt;
    transObj->m_towere = persObj->m_towere;
    transObj->m_towereta = persObj->m_towereta;
    transObj->m_towerphi = persObj->m_towerphi;
    for( int i = 0; i < 6; ++i ) {
	transObj->m_emradii[i] = persObj->m_emradii[i];
	transObj->m_hadradii[i] = persObj->m_hadradii[i];
    }
    for( int i = 0; i < 10; ++i ) {
	transObj->m_ImpactParameter[i] =
	    persObj->m_ImpactParameter[i];
	transObj->m_RawImpactParameter[i] =
	    persObj->m_RawImpactParameter[i];
	transObj->m_SignedImpactParameter[i] =
	    persObj->m_SignedImpactParameter[i];
	transObj->m_ImpactParameterSignificance[i] =
	    persObj->m_ImpactParameterSignificance[i];
	transObj->m_SignedImpactParameterSignificance[i] =
	    persObj->m_SignedImpactParameterSignificance[i];
    }
    for( int i = 0; i < 25; ++i ) {
	transObj->m_d0uncaletlayers[i] = persObj->m_d0uncaletlayers[i];
    }
    cellVectCnv.persToTrans(
	&persObj->m_d0_emcluster, &transObj->m_d0_emcluster, log );
    trackVectCnv.persToTrans(
	&persObj->m_d0_05_Tracks, &transObj->m_d0_05_Tracks, log );
    trackVectCnv.persToTrans(
	&persObj->m_d0_tau_Tracks, &transObj->m_d0_tau_Tracks, log );
    trackVectCnv.persToTrans(
	&persObj->m_d0_nontau_Tracks, &transObj->m_d0_nontau_Tracks, log );
}

void tauAnalysisHelperObjectCnv_p1 :: transToPers
   (const tauAnalysisHelperObject */*transObj*/,
    tauAnalysisHelperObject_p1 */*persObj*/,
    MsgStream &log )
{
    log << MSG::WARNING << "tauAnalysisHelperObjectCnv_p1::transToPers"
	"called this should not happen, as this method is deprecated"
	<< endreq;
    return;
}
