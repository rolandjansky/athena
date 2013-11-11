/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PDetails_p1.h
///
/// Implementation of persistent Tau1P3PDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
///
/// $Id: Tau1P3PDetails_p1.cxx,v 1.6 2009-05-08 14:32:44 tburgess Exp $

#include "tauEventTPCnv/Tau1P3PDetails_p1.h"

// Make sure that members added to this class since the first version
// get initialized.  (Otherwise, when reading an old version, they'll
// be left uninitialized.)
Tau1P3PDetails_p1::Tau1P3PDetails_p1() :
    m_numStripCells(-1111),
    m_stripWidth2(-1111.),
    m_emRadius(-1111.),
    m_ET12Frac(-1111.),
    m_etIsolHAD(-1111.),
    m_etIsolEM(-1111.),
    m_etChrgHAD(-1111.),
    m_nAssocTracksCore(-1111),
    m_nAssocTracksIsol(-1111),
    m_signD0Trk3P(-1111.),
    m_massTrk3P(-1111.),
    m_rWidth2Trk3P(-1111.),
    m_etHadAtEMScale(-1111.),
    m_etEMAtEMScale(-1111.),
    m_etEMCL(-1111.),
    m_etChrgEM(-1111.),
    m_etNeuEM(-1111.),
    m_etResNeuEM(-1111.),
    m_trFlightPathSig(-1111.),
    m_z0SinThetaSig(-1111.),
    m_etChrgHADoverPttot(-1111.),
    m_etIsolFrac(-1111.),
    m_sumEtCellsLArOverLeadTrackPt(-1111.),
    m_hadronicLeak(-1111.),
    m_secondaryMax(-1111.),
    m_etEflow(-1111.)
{

}
