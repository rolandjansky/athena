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

#include "tauEvent/tauAnalysisHelperObject.h"
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
    transObj->setDecmode (persObj->m_decmode);
    transObj->setJettype (persObj->m_jettype);
    transObj->setD0prf (persObj->m_d0prf);
    transObj->setD0iso (persObj->m_d0iso);
    transObj->setD0isoet (persObj->m_d0isoet);
    transObj->setD0ettr (persObj->m_d0ettr);
    transObj->setD0etem (persObj->m_d0etem);
    transObj->setD0etem2 (persObj->m_d0etem2);
    transObj->setD0emclet (persObj->m_d0emclet);
    transObj->setD0emcleta (persObj->m_d0emcleta);
    transObj->setD0emclphi (persObj->m_d0emclphi);
    transObj->setD0et05 (persObj->m_d0et05);
    transObj->setD0eta05 (persObj->m_d0eta05);
    transObj->setD0phi05 (persObj->m_d0phi05);
    transObj->setD0hadet (persObj->m_d0hadet);
    transObj->setD0hadeta (persObj->m_d0hadeta);
    transObj->setD0hadphi (persObj->m_d0hadphi);
    transObj->setD0type (persObj->m_d0type);
    transObj->setD0deltaR1 (persObj->m_d0deltaR1);
    transObj->setD0eTosumpT (persObj->m_d0eTosumpT);
    transObj->setD0deltaRhad (persObj->m_d0deltaR1had);
    transObj->setD0em3iso (persObj->m_d0em3iso);
    transObj->setD0mtrem3 (persObj->m_d0mtrem3);
    transObj->setD0deltaR2 (persObj->m_d0deltaR2);
    transObj->setD0ntr1030 (persObj->m_d0ntr1030);
    transObj->setD0EM12isof (persObj->m_d0EM12isof);
    transObj->setD0e1e2otaupT (persObj->m_d0e1e2otaupT);
    transObj->setD0ettro123 (persObj->m_d0ettro123);
    transObj->setD0ett1oEtiso (persObj->m_d0ett1oEtiso);
    transObj->setD0ett1oEtisoet (persObj->m_d0ett1oEtisoet);
    transObj->setD0dalpha (persObj->m_d0dalpha);
    transObj->setD0e1e2 (persObj->m_d0e1e2);
    transObj->setD0mtr1tr2 (persObj->m_d0mtr1tr2);
    transObj->setD0mtr1tr2tr3 (persObj->m_d0mtr1tr2tr3);
    transObj->setD0sumtaupt (persObj->m_d0sumtaupt);
    transObj->setD0sumnontaupt (persObj->m_d0sumnontaupt);
    transObj->setD0sumpt (persObj->m_d0sumpt);

    transObj->clearD0towers();
    for (size_t i = 0; i < persObj->m_towere.size(); i++)
      transObj->addD0tower (persObj->m_towere[i],
                            persObj->m_towereta[i],
                            persObj->m_towerphi[i]);

    for( int i = 0; i < 6; ++i ) {
      transObj->setEMRadii (i, persObj->m_emradii[i]);
      transObj->setHADRadii (i, persObj->m_hadradii[i]);
    }
    for( int i = 0; i < 10; ++i ) {
      transObj->setImpactParameter(i, 
                                   persObj->m_ImpactParameter[i]);
      transObj->setRawImpactParameter(i, 
                                     persObj->m_RawImpactParameter[i]);
      transObj->setSignedImpactParameter(i,
                                         persObj->m_SignedImpactParameter[i]);
      transObj->setImpactParameterSignificance(i,
                                               persObj->m_ImpactParameterSignificance[i]);
      transObj->setSignedImpactParameterSignificance(i,
                                                     persObj->m_SignedImpactParameterSignificance[i]);
    }
    for( int i = 0; i < 25; ++i ) {
      transObj->setD0uncaletlayers (i, persObj->m_d0uncaletlayers[i]);
    }
    cellVectCnv.persToTrans(
        &persObj->m_d0_emcluster, &transObj->d0_emcluster(), log );
    trackVectCnv.persToTrans(
        &persObj->m_d0_05_Tracks, &transObj->d0_05_tracks(), log );
    trackVectCnv.persToTrans(
	&persObj->m_d0_tau_Tracks, &transObj->d0_tau_tracks(), log );
    trackVectCnv.persToTrans(
        &persObj->m_d0_nontau_Tracks, &transObj->d0_nontau_tracks(), log );
}

void tauAnalysisHelperObjectCnv_p1 :: transToPers
   (const tauAnalysisHelperObject */*transObj*/,
    tauAnalysisHelperObject_p1 */*persObj*/,
    MsgStream &log )
{
    log << MSG::WARNING << "tauAnalysisHelperObjectCnv_p1::transToPers"
	"called this should not happen, as this method is deprecated"
	<< endmsg;
    return;
}
