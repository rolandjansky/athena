/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1RecExtraDetails_p1.h
///
/// Declaration of persistent TauRecExtraDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-01.
///
/// $Id: TauRecExtraDetails_p1.h,v 1.6 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAURECEXTRADETAILS_P1_H
#define TAUEVENTTPCNV_TAURECEXTRADETAILS_P1_H

#include "AthenaPoolUtilities/TPObjRef.h"

/// DEPRECATED - Do not use except for backwards compatibility
/// Persistent TauRecExtraDetails class
class TauRecExtraDetails_p1
{
    friend class TauRecExtraDetailsCnv_p1;
public:
    ///Constructor
    TauRecExtraDetails_p1();
private:
    TPObjRef m_analysisHelper;
    int m_seedType;
    int m_numEMCells;
    float m_stripET;
    float m_emCentralityFraction;
    float m_etHadAtEMScale;
    float m_etEMAtEMScale;
    float m_sumCellE;
    float m_sumEMCellE;
    float m_sumPTTracks;
};

#endif // TAUEVENTTPCNV_TAURECEXTRADETAILS_P1_H

