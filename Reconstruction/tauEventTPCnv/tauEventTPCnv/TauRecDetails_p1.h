/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**@file TauRecDetails_p1.h
 * Definition file for class TauRecDetails_p1
 * package: Reconstruction/tauEventTPCnv
 * @author Thomas Burgess <thomas.burgess@cern.ch>
 * @author Lukasz Janyst
 * $Id: TauRecDetails_p1.h,v 1.12 2009-01-20 17:19:02 tburgess Exp $
 */


#ifndef tauEventTPCnv_TAU_REC_DETAILS_P1_H
#define tauEventTPCnv_TAU_REC_DETAILS_P1_H

#include "EventCommonTPCnv/HepLorentzVector_p1.h"
#include "DataModelAthenaPool/ElementLinkVector_p1.h"
#include "AthenaPoolUtilities/TPObjRef.h"

#include <string>

class TauRecDetails_p1
{
public:
    
    TauRecDetails_p1();
        
    ElementLinkIntVector_p1 m_looseTracks;
    float  m_emRadius;
    float  m_hadRadius;
    float  m_sumEmCellEt;
    float  m_sumHadCellEt;
    float  m_ET12Frac;
    float  m_centralityFraction;
    float  m_stripWidth2;
    int    m_numStripCells;
    HepLorentzVector_p1 m_sumEM;
    float  m_etEMCalib;
    float  m_etHadCalib;
    TPObjRef m_secVertex;
    std::vector<float> m_trackCaloEta;
    std::vector<float> m_trackCaloPhi;
    float  m_leadingTrackPT;
    float  m_trFlightPathSig;
    float  m_etaCalo;
    float  m_phiCalo;
    float  m_ipSigLeadTrack;
    float  m_etOverPtLeadTrack;
    int    m_nTracksdrdR; 
    int    m_chargeLooseTracks;
};
#endif // RECTPVNC_TAU_REC_DETAILS_P1_H
