/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails_p1.cxx
///
/// Implementation of persistent class
/// for Analysis::TauCommonDetails
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
///
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonDetails_p1.cxx,v 1.7 2009-03-02 17:20:51 binet Exp $

#include "tauEventTPCnv/TauCommonDetails_p1.h"
#include "limits.h"

TauCommonDetails_p1::TauCommonDetails_p1():
    m_ipZ0SinThetaSigLeadTrk(-1111.),
    m_etOverPtLeadTrk(-1111.),
    m_etOverPtLeadLooseTrk(-1111.),
    m_leadTrkPt(-1111.),
    m_leadLooseTrkPt(-1111.),
    m_ipSigLeadTrk(-1111.),
    m_ipSigLeadLooseTrk(-1111.),
    m_looseTrk(),
    m_looseConvTrk(),
    m_chrgLooseTrk(-1111.),
    m_cellEM012Cluster(),
    m_sumPi0Vec(),
    m_massTrkSys(-1111.),
    m_trkWidth2(-1111.),
    m_trFlightPathSig(-1111.),
    m_secVtx(),
    m_etEflow(-1111.),
    m_mEflow(-1111.),
    m_pi0(),
    m_Ele_E237E277(-1111.),
    m_Ele_PresamplerFraction(-1111.),
    m_Ele_ECALFirstFraction(-1111.),
    m_seedCalo_nIsolLooseTrk(-1111.),
    m_seedCalo_EMRadius(-1111.),
    m_seedCalo_hadRadius(-1111.),
    m_seedCalo_etEMAtEMScale(-1111.),
    m_seedCalo_etHadAtEMScale(-1111.),
    m_seedCalo_isolFrac(-1111.),
    m_seedCalo_centFrac(-1111.),
    m_seedCalo_stripWidth2(-1111.),
    m_seedCalo_nStrip(INT_MIN),
    m_seedCalo_etEMCalib(-1111.),
    m_seedCalo_etHadCalib(-1111.),
    m_seedCalo_eta(-1111.),
    m_seedCalo_phi(-1111.),
    m_seedCalo_trkAvgDist(-1111.),
    m_seedCalo_trkRmsDist(-1111.),
    m_seedTrk_EMRadius(-1111.),
    m_seedTrk_isolFrac(-1111.),
    m_seedTrk_etChrgHadOverSumTrkPt(-1111.),
    m_seedTrk_isolFracWide(-1111.),
    m_seedTrk_etHadAtEMScale(-1111.),
    m_seedTrk_etEMAtEMScale(-1111.),
    m_seedTrk_etEMCL(-1111.),
    m_seedTrk_etChrgEM(-1111.),
    m_seedTrk_etNeuEM(-1111.),
    m_seedTrk_etResNeuEM(-1111.),
    m_seedTrk_hadLeakEt(-1111.),
    m_seedTrk_etChrgEM01Trk(),
    m_seedTrk_etResChrgEMTrk(),
    m_seedTrk_sumEMCellEtOverLeadTrkPt(-1111.),
    m_seedTrk_secMaxStripEt(-1111.),
    m_seedTrk_stripWidth2(-1111.),
    m_seedTrk_nStrip(INT_MIN),
    m_seedTrk_etChrgHad(-1111.),
    m_seedTrk_nOtherCoreTrk(INT_MIN),
    m_seedTrk_nIsolTrk(INT_MIN),
    m_seedTrk_etIsolEM(-1111.),
    m_seedTrk_etIsolHad(-1111.),
    m_numCells(0),
    m_numTopoClusters(0),
    m_numEffTopoClusters(-1111.),
    m_topoInvMass(-1111.),
    m_effTopoInvMass(-1111.),
    m_topoMeanDeltaR(-1111.),
    m_effTopoMeanDeltaR(-1111.)
{

}

