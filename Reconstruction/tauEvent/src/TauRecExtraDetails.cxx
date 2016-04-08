/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauRecExtraDetails.cxx  DEPRECATED DO NOT USE
///- MIGRATE TO TauCommonDetails
///
/// Implementation of extra details class for TauRec
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <tburgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// Created 2007-01-17
///
/// $Id: TauRecExtraDetails.cxx,v 1.8 2009-01-17 11:16:25 tburgess Exp $

#include "tauEvent/TauRecExtraDetails.h"
#include "DataModel/ClassName.h"

namespace Analysis
{
    TauRecExtraDetails :: TauRecExtraDetails():
        m_analysisHelper( 0 ),
        m_numEMCells( TauDetails::DEFAULT_INT ),
        m_stripET( TauDetails::DEFAULT ),
        m_emCentralityFraction( TauDetails::DEFAULT ),
        m_etHadAtEMScale( TauDetails::DEFAULT ),
        m_etEMAtEMScale( TauDetails::DEFAULT ),
        m_sumCellE( TauDetails::DEFAULT ),
        m_sumEMCellE( TauDetails::DEFAULT ),
        m_sumPTTracks( TauDetails::DEFAULT )
    {
    }

    TauRecExtraDetails :: TauRecExtraDetails( const TauRecExtraDetails &rhs ):
        TauDetails(),
        m_analysisHelper(new tauAnalysisHelperObject(*rhs.m_analysisHelper)),
        m_seedType(rhs.m_seedType),
        m_numEMCells(rhs.m_numEMCells),
        m_stripET(rhs.m_stripET),
        m_emCentralityFraction(rhs.m_emCentralityFraction),
        m_etHadAtEMScale(rhs.m_etHadAtEMScale),
        m_etEMAtEMScale(rhs.m_etEMAtEMScale),
        m_sumCellE(rhs.m_sumCellE),
        m_sumEMCellE(rhs.m_sumEMCellE),
        m_sumPTTracks(rhs.m_sumPTTracks)
    {
    }

    TauRecExtraDetails &TauRecExtraDetails :: operator= ( const TauRecExtraDetails &rhs )
    {
        if( this != &rhs ) {
            delete m_analysisHelper;
            m_analysisHelper=new tauAnalysisHelperObject(*rhs.m_analysisHelper);
            m_seedType=(TauJetParameters::SeedTypes)m_seedType;
            m_numEMCells=rhs.m_numEMCells;
            m_stripET=rhs.m_stripET;
            m_emCentralityFraction=rhs.m_emCentralityFraction;
            m_etHadAtEMScale=rhs.m_etHadAtEMScale;
            m_etEMAtEMScale=rhs.m_etEMAtEMScale;
            m_sumCellE=rhs.m_sumCellE;
            m_sumEMCellE=rhs.m_sumEMCellE;
            m_sumPTTracks=rhs.m_sumPTTracks;
        }
        return *this;
    }

    TauRecExtraDetails :: ~TauRecExtraDetails()
    {
        delete m_analysisHelper;
    }

    std::string TauRecExtraDetails::s_className=
        ClassName<TauRecExtraDetails>::name();
    const std::string& TauRecExtraDetails::className() const
    {
        return s_className;
    }
}
