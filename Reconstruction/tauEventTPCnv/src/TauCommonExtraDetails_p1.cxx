/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonExtraDetails_p1.cxx
/// 
/// Implementation of persistent class 
/// for Analysis::TauCommonExtraDetails
/// 
/// Package: Reconstruction/tauEventTPCnv
/// 
/// @author Thomas Burgess  
/// 
/// Created by Thomas Burgess on 2008-11-04.
///
/// $Id: TauCommonExtraDetails_p1.cxx,v 1.3 2009-01-20 17:19:01 tburgess Exp $

#include "tauEventTPCnv/TauCommonExtraDetails_p1.h"

TauCommonExtraDetails_p1::TauCommonExtraDetails_p1()
  : m_sumPtLooseTrk(0) 
  , m_sumPtTrk(0) 
  , m_seedCalo_nEMCell(0) 
  , m_seedCalo_stripEt(0) 
  , m_seedCalo_EMCentFrac(0) 
  , m_seedCalo_sumCellEnergy(0) 
  , m_seedCalo_sumEMCellEnergy(0) 
  , m_tracks(0) 
  , m_looseTracks(0) 
{

}

