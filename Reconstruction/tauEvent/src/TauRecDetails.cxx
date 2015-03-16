/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauRecDetails.cxx  DEPRECATED DO NOT USE
/// - MIGRATE TO TauCommonDetails
///
/// Implementation of details class for TauRec
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// $Id: TauRecDetails.cxx,v 1.11 2009-05-20 13:10:37 slai Exp $

#include "tauEvent/TauRecDetails.h"
#include "DataModel/ClassName.h"

namespace Analysis
{
    //-------------------------------------------------------------------------
    // Constructor
    //-------------------------------------------------------------------------
    TauRecDetails :: TauRecDetails():
        m_emRadius( TauDetails::DEFAULT ),
        m_hadRadius(TauDetails::DEFAULT),
        m_sumEmCellEt(TauDetails::DEFAULT),
        m_sumHadCellEt(TauDetails::DEFAULT),
        m_ET12Frac( TauDetails::DEFAULT ),
        m_centralityFraction( TauDetails::DEFAULT ),
        m_stripWidth2( TauDetails::DEFAULT ),
        m_numStripCells( TauDetails::DEFAULT_INT ),
        m_ipSigLeadTrack( TauDetails::DEFAULT ),
        m_etOverPtLeadTrack( TauDetails::DEFAULT ),
        m_etEMCalib( TauDetails::DEFAULT ),
        m_secVertex( 0 ),
        m_leadingTrackPT(TauDetails::DEFAULT),
        m_trFlightPathSig(TauDetails::DEFAULT),
        m_etaCalo(TauDetails::DEFAULT),
        m_phiCalo(TauDetails::DEFAULT),
        m_nTracksdrdR(TauDetails::DEFAULT_INT),
        m_chargeLooseTracks(TauDetails::DEFAULT_INT)
    {
    }

    //-------------------------------------------------------------------------
    // Destructor
    //-------------------------------------------------------------------------
    TauRecDetails :: ~TauRecDetails()
    {
        delete m_secVertex;
    }

    //-------------------------------------------------------------------------
    // Copy constructor
    //-------------------------------------------------------------------------
    TauRecDetails :: TauRecDetails( const TauRecDetails &rhs ):
        TauDetails(),
        m_emRadius(rhs.m_emRadius),
        m_hadRadius(rhs.m_hadRadius),
        m_sumEmCellEt(rhs.m_sumEmCellEt),
        m_sumHadCellEt(rhs.m_sumHadCellEt),
        m_ET12Frac(rhs.m_ET12Frac),
        m_centralityFraction(rhs.m_centralityFraction),
        m_stripWidth2(rhs.m_stripWidth2),
        m_numStripCells(rhs.m_numStripCells),
        m_ipSigLeadTrack(rhs.m_ipSigLeadTrack),
        m_etOverPtLeadTrack(rhs.m_etOverPtLeadTrack),
        m_sumEM(rhs.m_sumEM),
        m_etEMCalib(rhs.m_etEMCalib),
        m_etHadCalib(rhs.m_etHadCalib),
        m_secVertex(new Trk::RecVertex( *rhs.m_secVertex )),
        m_trackCaloEta(rhs.m_trackCaloEta),
        m_trackCaloPhi(rhs.m_trackCaloPhi),
        m_leadingTrackPT(rhs.m_leadingTrackPT),
        m_trFlightPathSig(rhs.m_trFlightPathSig),
        m_etaCalo(rhs.m_etaCalo),
        m_phiCalo(rhs.m_phiCalo),
        m_nTracksdrdR(rhs.m_nTracksdrdR),
        m_chargeLooseTracks(rhs.m_chargeLooseTracks)
    {
    }

    //-------------------------------------------------------------------------
    // Assignment operator
    //-------------------------------------------------------------------------
    TauRecDetails &TauRecDetails :: operator = ( const TauRecDetails &rhs )
    {
        if( this != &rhs )
        {
            delete m_secVertex;
            m_emRadius=rhs.m_emRadius;
            m_hadRadius=rhs.m_hadRadius;
            m_sumEmCellEt=rhs.m_sumEmCellEt;
            m_sumHadCellEt=rhs.m_sumHadCellEt;
            m_ET12Frac=rhs.m_ET12Frac;
            m_centralityFraction=rhs.m_centralityFraction;
            m_stripWidth2=rhs.m_stripWidth2;
            m_numStripCells=rhs.m_numStripCells;
            m_ipSigLeadTrack=rhs.m_ipSigLeadTrack;
            m_etOverPtLeadTrack=rhs.m_etOverPtLeadTrack;
            m_sumEM=rhs.m_sumEM;
            m_etEMCalib=rhs.m_etEMCalib;
            m_etHadCalib=rhs.m_etHadCalib;
            if (rhs.m_secVertex)
                m_secVertex=new Trk :: RecVertex( *rhs.m_secVertex );
            m_trackCaloEta=rhs.m_trackCaloEta;
            m_trackCaloPhi=rhs.m_trackCaloPhi;
            m_leadingTrackPT=rhs.m_leadingTrackPT;
            m_trFlightPathSig=rhs.m_trFlightPathSig;
            m_etaCalo=rhs.m_etaCalo;
            m_phiCalo=rhs.m_phiCalo;
            m_nTracksdrdR=rhs.m_nTracksdrdR;
            m_chargeLooseTracks=rhs.m_chargeLooseTracks;
        }
        return *this;
    }


    //-------------------------------------------------------------------------
    // Get parameter
    //-------------------------------------------------------------------------
    double TauRecDetails :: parameter( TauJetParameters :: Detail detail ) const
    {
        switch( detail )
        {
            case TauJetParameters :: etHadCalib:
                return etHadCalib();
            case TauJetParameters :: etEMCalib:
                return etEMCalib();
            case TauJetParameters :: emRadius:
                return emRadius();
            case TauJetParameters :: isolationFraction:
                return isolationFraction();
            case TauJetParameters :: centralityFraction:
                return centralityFraction();
            case TauJetParameters :: stripWidth2:
                return stripWidth2();
            case TauJetParameters :: nStripCells:
                return (double)numStripCells();
            default:
                return TauDetails::DEFAULT;
        }
        return TauDetails::DEFAULT;
    }

    //-------------------------------------------------------------------------
    // Check if the object contains given parameter
    //-------------------------------------------------------------------------
    bool TauRecDetails :: hasParameter( TauJetParameters :: Detail detail ) const
    {
        switch( detail )
        {
            case TauJetParameters :: etHadCalib:
            case TauJetParameters :: etEMCalib:
            case TauJetParameters :: emRadius:
            case TauJetParameters :: isolationFraction:
            case TauJetParameters :: centralityFraction:
            case TauJetParameters :: stripWidth2:
            case TauJetParameters :: nStripCells:
                return true;
            default:
                return false;
        }
        return false;
    }

    std::string TauRecDetails::s_className =
        ClassName<TauRecDetails>::name();
    const std::string& TauRecDetails::className() const
    {
        return s_className;
    }
}
