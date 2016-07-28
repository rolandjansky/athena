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

#include "tauEvent/Tau1P3PDetails.h"
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
    trans->setNumStripCells (pers->m_numStripCells);
    trans->setStripWidth2 (pers->m_stripWidth2);
    trans->setEMRadius (pers->m_emRadius);
    trans->setIsolationFraction (pers->m_ET12Frac);
    trans->setETIsolHAD (pers->m_etIsolHAD);
    trans->setETIsolEM (pers->m_etIsolEM);
    trans->setETChrgHAD (pers->m_etChrgHAD);
    trans->setNAssocTracksCore (pers->m_nAssocTracksCore);
    trans->setNAssocTracksIsol (pers->m_nAssocTracksIsol);
    trans->setSignD0Trk3P (pers->m_signD0Trk3P);
    trans->setMassTrk3P (pers->m_massTrk3P);
    trans->setRWidth2Trk3P (pers->m_rWidth2Trk3P);
    trans->setETHadAtEMScale (pers->m_etHadAtEMScale);
    trans->setETEMAtEMScale (pers->m_etEMAtEMScale);
    trans->setETEMCL  (pers->m_etEMCL);
    trans->setETChrgEM (pers->m_etChrgEM);
    trans->setETNeuEM (pers->m_etNeuEM);
    trans->setETResNeuEM (pers->m_etResNeuEM);

    trans->resizeETChrgEM01Trk (pers->m_etChrgEM01Trk.size());
    for (size_t i = 0; i < pers->m_etChrgEM01Trk.size(); i++)
      trans->setETChrgEM01Trk (i, pers->m_etChrgEM01Trk[i]);

    trans->resizeETResChrgEMTrk (pers->m_etResChrgEMTrk.size());
    for (size_t i = 0; i < pers->m_etResChrgEMTrk.size(); i++)
      trans->setETResChrgEMTrk (i, pers->m_etResChrgEMTrk[i]);

    trans->setTrFlightPathSig (pers->m_trFlightPathSig);
    trans->setZ0SinThetaSig (pers->m_z0SinThetaSig);
    trans->setEtChrgHADoverPttot (pers->m_etChrgHADoverPttot);
    trans->setEtIsolFrac (pers->m_etIsolFrac);
    trans->setSumEtCellsLArOverLeadTrackPt
      (pers->m_sumEtCellsLArOverLeadTrackPt);
    trans->setHadronicLeak (pers->m_hadronicLeak);
    trans->setSecondaryMax (pers->m_secondaryMax);

    CLHEP::HepLorentzVector sumEM;
    hepLorentzVectorCnv.persToTrans( 
	&pers->m_sumEM, &sumEM, msg );
    trans->setSumEM (sumEM);
    trans->setSecVertex (createTransFromPStore 
                         (&m_recVertexCnv, pers->m_secVertex, msg ));
    clusterVectCnv.persToTrans(&pers->m_pi0, &trans->pi0LinkVector(), msg );
    clusterCnv.persToTrans(
	&pers->m_cellEM012Cluster, 
	&trans->cellEM012ClusterLink(), msg );
    trans->setETeflow (pers->m_etEflow);
}

void Tau1P3PDetailsCnv_p1::transToPers
   (const Analysis::Tau1P3PDetails */*trans*/,
    Tau1P3PDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "Tau1P3PDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endmsg;
    return;
}
