/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PDetails_p1.h
///
/// Declaration of persistent Tau1P3PDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-01.
///
/// $Id: Tau1P3PDetails_p1.h,v 1.10 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAU1P3PDETAILS_P1_H
#define TAUEVENTTPCNV_TAU1P3PDETAILS_P1_H

#include <vector>
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "DataModelAthenaPool/ElementLink_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"
#include "EventCommonTPCnv/HepLorentzVector_p1.h"

/// DEPRECATED - Do not use except for backwards compatibility
/// Persistent Tau1P3PDetails class
class Tau1P3PDetails_p1
{
    friend class Tau1P3PDetailsCnv_p1;
public:
    ///Constructor
    Tau1P3PDetails_p1();

private:
    int m_numStripCells;
    float m_stripWidth2;
    float m_emRadius;
    float m_ET12Frac;
    float m_etIsolHAD;
    float m_etIsolEM;
    float m_etChrgHAD;
    int m_nAssocTracksCore;
    int m_nAssocTracksIsol;
    float m_signD0Trk3P;
    float m_massTrk3P;
    float m_rWidth2Trk3P;
    float m_etHadAtEMScale;
    float m_etEMAtEMScale;
    float m_etEMCL;
    float m_etChrgEM;
    float m_etNeuEM;
    float m_etResNeuEM;
    float m_trFlightPathSig;
    float m_z0SinThetaSig;
    float m_etChrgHADoverPttot;
    float m_etIsolFrac;
    float m_sumEtCellsLArOverLeadTrackPt;
    float m_hadronicLeak;
    float m_secondaryMax;
    std :: vector<float> m_etChrgEM01Trk;
    std :: vector<float> m_etResChrgEMTrk;
    HepLorentzVector_p1 m_sumEM;
    TPObjRef m_secVertex;
    ElementLinkIntVector_p1 m_pi0;
    ElementLinkInt_p1  m_cellEM012Cluster;
    float m_etEflow;
};

#endif // TAUEVENTTPCNV_TAU1P3PDETAILS_P1_H
