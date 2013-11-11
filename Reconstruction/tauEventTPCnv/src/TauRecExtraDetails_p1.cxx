/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauRecExtraDetails_p1.h
///
/// Declaration of persistent TauRecExtraDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess <thomas.burgess@cern.ch>
/// @author Lukasz Janyst
///
/// Created by Thomas Burgess on 2008-11-28.
///
/// $Id: TauRecExtraDetails_p1.cxx,v 1.3 2009-01-20 17:19:02 tburgess Exp $

#include "tauEventTPCnv/TauRecExtraDetails_p1.h"

// Make sure that members added to this class since the first version
// get initialized.  (Otherwise, when reading an old version, they'll
// be left uninitialized.)
TauRecExtraDetails_p1::TauRecExtraDetails_p1()
  : m_seedType(0),
    m_numEMCells(0),
    m_stripET(0),
    m_emCentralityFraction(0),
    m_etHadAtEMScale(0),
    m_etEMAtEMScale(0),
    m_sumCellE(0),
    m_sumEMCellE(0),
    m_sumPTTracks(0)
{

}
