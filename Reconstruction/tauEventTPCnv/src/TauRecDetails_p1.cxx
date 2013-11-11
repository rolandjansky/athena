/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**@file TauRecDetails_p1.cxx 
 * Implementation file for class TauRecDetails_p1
 * package: Reconstruction/tauEventTPCnv
 * @author Thomas Burgess <thomas.burgess@cern.ch>
 * $Id: TauRecDetails_p1.cxx,v 1.5 2009-01-20 17:19:01 tburgess Exp $
 */


#include "tauEventTPCnv/TauRecDetails_p1.h"

// Make sure that members added to this class since the first version
// get initialized.  (Otherwise, when reading an old version, they'll
// be left uninitialized.)
TauRecDetails_p1::TauRecDetails_p1()
  : m_emRadius(0),
    m_hadRadius(0),
    m_sumEmCellEt(0),
    m_sumHadCellEt(0),
    m_ET12Frac(0),
    m_centralityFraction(0),
    m_stripWidth2(0),
    m_numStripCells(0),
    m_etEMCalib(0),
    m_etHadCalib(0),
    m_leadingTrackPT(0),
    m_trFlightPathSig (0),
    m_etaCalo (0),
    m_phiCalo (0),
    m_ipSigLeadTrack (0),
    m_etOverPtLeadTrack (0),
    m_nTracksdrdR(0),
    m_chargeLooseTracks(-1111)
{
}

