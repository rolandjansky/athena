/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file Tau1P3PExtraDetails_p1.h
///
/// Declaration of persistent Tau1P3PExtraDetails class
///
/// Package: Reconstruction/tauEventTPCnv
///
/// @author Thomas Burgess
/// @author Lukasz Janyst
///
/// Created on 2007-07-01
///
/// $Id: Tau1P3PExtraDetails_p1.h,v 1.4 2009-01-20 17:19:02 tburgess Exp $

#ifndef TAUEVENTTPCNV_TAU1P3PEXTRADETAILS_P1_H
#define TAUEVENTTPCNV_TAU1P3PEXTRADETAILS_P1_H

#include "DataModelAthenaPool/ElementLink_p2.h"
#include "DataModelAthenaPool/ElementLinkContNames_p2.h"
#include <vector>

/// DEPRECATED - Do not use except for backwards compatibility
/// Persistent Tau1P3PExtraDetails class
class Tau1P3PExtraDetails_p1
{
    friend class Tau1P3PExtraDetailsCnv_p1;
public:
    ///Constructor
    Tau1P3PExtraDetails_p1();
private:
    std::vector<ElementLinkInt_p2> m_closestEtaTrkVertCell;
    std::vector<ElementLinkInt_p2> m_closestEtaTrkCell;
    std::vector<ElementLinkInt_p2> m_closestPhiTrkVertCell;
    std::vector<ElementLinkInt_p2> m_closestPhiTrkCell;
    std::vector<float> m_etaTrackCaloSamp;
    std::vector<float> m_phiTrackCaloSamp;
    float m_sumPTTracks;
    unsigned char m_tracks;
    // Element link container name lookup table
    ElementLinkContNames_p2 m_linkNames;
};

#endif // TAUEVENTTPCNV_TAU1P3PEXTRADETAILS_P1_H
