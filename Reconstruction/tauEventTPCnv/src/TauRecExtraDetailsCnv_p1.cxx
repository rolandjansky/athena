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
#define private public
#define protected public
#include "tauEvent/TauRecExtraDetails.h"
#undef private
#undef protected

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
    trans->m_numEMCells=pers->m_numEMCells;
    trans->m_stripET=pers->m_stripET;
    trans->m_emCentralityFraction=pers->m_emCentralityFraction;
    trans->m_etHadAtEMScale=pers->m_etHadAtEMScale;
    trans->m_etEMAtEMScale=pers->m_etEMAtEMScale;
    trans->m_sumCellE=pers->m_sumCellE;
    trans->m_sumEMCellE=pers->m_sumEMCellE;
    trans->m_sumPTTracks=pers->m_sumPTTracks;
    trans->m_analysisHelper=createTransFromPStore( 
	&m_tauAnalysisHelperObjectCnv, pers->m_analysisHelper, msg );
    trans->m_seedType=(TauJetParameters::SeedTypes)pers->m_seedType;
}

void TauRecExtraDetailsCnv_p1::transToPers
   (const Analysis::TauRecExtraDetails */*trans*/,
    TauRecExtraDetails_p1 */*pers*/,
    MsgStream &msg )
{
    msg << MSG::WARNING << "TauRecDetailsCnv_p1::transToPers called "
	"this should not happen, as this method is deprecated" << endreq;
    return;
}
