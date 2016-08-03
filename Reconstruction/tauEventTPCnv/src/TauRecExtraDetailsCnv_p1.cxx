/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauRecExtraDetailsCnv_p1.cxx
///
/// Implementation of persistent/transient TauRecExtraDetails converter
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-07.
///
/// $Id: TauRecExtraDetailsCnv_p1.cxx,v 1.8 2009-01-20 17:19:01 tburgess Exp $

//Trick to access private members in tau rec extra details
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEventTPCnv/TauRecExtraDetailsCnv_p1.h"
#include "tauEvent/TauJetParameters.h"
#include "tauEvent/tauAnalysisHelperObject.h"

TauRecExtraDetailsCnv_p1::TauRecExtraDetailsCnv_p1() :
    T_AthenaPoolTPPolyCnvBase<
    Analysis::TauDetails,
    Analysis::TauRecExtraDetails,
    TauRecExtraDetails_p1 >(),
    m_tauAnalysisHelperObjectCnv(0)
{
}

TauRecExtraDetailsCnv_p1::~TauRecExtraDetailsCnv_p1()
{
}

void TauRecExtraDetailsCnv_p1::persToTrans(
    const TauRecExtraDetails_p1 *pers,
    Analysis::TauRecExtraDetails *trans,
    MsgStream &msg )
{
    trans->setNumEMCells (pers->m_numEMCells);
    trans->setStripET (pers->m_stripET);
    trans->setEMCentralityFraction (pers->m_emCentralityFraction);
    trans->setETHadAtEMScale (pers->m_etHadAtEMScale);
    trans->setETEMAtEMScale (pers->m_etEMAtEMScale);
    trans->setEnergy (pers->m_sumCellE);
    trans->setEMEnergy (pers->m_sumEMCellE);
    trans->setSumPTTracks (pers->m_sumPTTracks);

    trans->setAnalysisHelper (createTransFromPStore
                              (&m_tauAnalysisHelperObjectCnv, pers->m_analysisHelper, msg ));
    trans->setSeedType ((TauJetParameters::SeedTypes)pers->m_seedType);
}

void TauRecExtraDetailsCnv_p1::transToPers
   (const Analysis::TauRecExtraDetails */*trans*/,
    TauRecExtraDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "TauRecDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endmsg;
    return;
}
