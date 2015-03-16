/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file Tau1P3PExtraDetails.cxx DEPRECATED DO NOT USE
/// - MIGRATE TO TauCommonDetails
///
/// Implementation of extra details class for Tau1P3P
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <tburgess-at-cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// Created 2007-01-17
///
/// $Id: Tau1P3PExtraDetails.cxx,v 1.8 2009-01-17 01:34:16 tburgess Exp $

#include "tauEvent/Tau1P3PExtraDetails.h"
#include "DataModel/ClassName.h"

namespace Analysis
{
    Tau1P3PExtraDetails::Tau1P3PExtraDetails()
        : TauDetails()
    {
        allocTracksAndSamplings( 10, 4 );
    }

    Tau1P3PExtraDetails::~Tau1P3PExtraDetails()
    {
    }

    Tau1P3PExtraDetails::Tau1P3PExtraDetails(
            const Tau1P3PExtraDetails &rhs ):
        TauDetails(),
        m_closestEtaTrkVertCell(rhs.m_closestEtaTrkVertCell),
        m_closestEtaTrkCell(rhs.m_closestEtaTrkCell),
        m_closestPhiTrkVertCell(rhs.m_closestPhiTrkVertCell),
        m_closestPhiTrkCell(rhs.m_closestPhiTrkCell),
        m_etaTrackCaloSamp(rhs.m_etaTrackCaloSamp),
        m_phiTrackCaloSamp(rhs.m_phiTrackCaloSamp),
        m_sumPTTracks(rhs.m_sumPTTracks)
    {
    }

    Tau1P3PExtraDetails& Tau1P3PExtraDetails::operator = (
            const Tau1P3PExtraDetails& rhs )
    {
        if(this != &rhs) {
            m_closestEtaTrkVertCell=rhs.m_closestEtaTrkVertCell;
            m_closestEtaTrkCell=rhs.m_closestEtaTrkCell;
            m_closestPhiTrkVertCell=rhs.m_closestPhiTrkVertCell;
            m_closestPhiTrkCell=rhs.m_closestPhiTrkCell;
            m_etaTrackCaloSamp=rhs.m_etaTrackCaloSamp;
            m_phiTrackCaloSamp=rhs.m_phiTrackCaloSamp;
            m_sumPTTracks=rhs.m_sumPTTracks;
        }
        return *this;
    }

    std::string Tau1P3PExtraDetails::s_className =
        ClassName<Tau1P3PExtraDetails>::name();
    const std::string& Tau1P3PExtraDetails::className() const
    {
        return s_className;
    }

    unsigned int Tau1P3PExtraDetails::ntrack() const
    {
        return m_closestEtaTrkVertCell.size();
    }

    unsigned int Tau1P3PExtraDetails::nsamp() const
    {
        if (m_closestEtaTrkVertCell.empty())
            return 0;
        return m_closestEtaTrkVertCell[0].size();
    }
}
