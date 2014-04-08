/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file Tau1P3PDetails.cxx DEPRECATED DO NOT USE
/// - MIGRATE TO TauCommonDetails
///
/// Implementation of details class for Tau1P3P
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// Created 2007-01-17
///
/// $Id: Tau1P3PDetails.cxx,v 1.12 2009-05-20 13:10:37 slai Exp $

#include "tauEvent/Tau1P3PDetails.h"
#include "DataModel/ClassName.h"

namespace Analysis
{
    Tau1P3PDetails :: Tau1P3PDetails():
        TauDetails(),
        m_numStripCells( TauDetails::DEFAULT_INT ),
        m_stripWidth2( TauDetails::DEFAULT ),
        m_emRadius( TauDetails::DEFAULT ),
        m_ET12Frac( TauDetails::DEFAULT ),
        m_etIsolHAD( TauDetails::DEFAULT ),
        m_etIsolEM( TauDetails::DEFAULT ),
        m_etChrgHAD( TauDetails::DEFAULT ),
        m_nAssocTracksCore( TauDetails::DEFAULT_INT ),
        m_nAssocTracksIsol( TauDetails::DEFAULT_INT ),
        m_signD0Trk3P( TauDetails::DEFAULT ),
        m_massTrk3P( TauDetails::DEFAULT ),
        m_rWidth2Trk3P( TauDetails::DEFAULT ),
        m_z0SinThetaSig( TauDetails::DEFAULT ),
        m_etChrgHADoverPttot( TauDetails::DEFAULT ),
        m_etIsolFrac( TauDetails::DEFAULT ),
        m_etHadAtEMScale( TauDetails::DEFAULT ),
        m_etEMAtEMScale( TauDetails::DEFAULT ),
        m_etEMCL( TauDetails::DEFAULT ),
        m_etChrgEM( TauDetails::DEFAULT ),
        m_etNeuEM( TauDetails::DEFAULT ),
        m_etResNeuEM( TauDetails::DEFAULT ),
        m_trFlightPathSig(TauDetails::DEFAULT),
        m_secVertex( 0 ),
        m_etEflow(TauDetails::DEFAULT),
        m_sumEtCellsLArOverLeadTrackPt(TauDetails::DEFAULT),
        m_hadronicLeak(TauDetails::DEFAULT),
        m_secondaryMax(TauDetails::DEFAULT)
    {
    }

    Tau1P3PDetails :: ~Tau1P3PDetails()
    {
        delete m_secVertex;
    }

    Tau1P3PDetails :: Tau1P3PDetails( const Tau1P3PDetails &rhs ):
        TauDetails(),
        m_numStripCells(rhs.m_numStripCells),
        m_stripWidth2(rhs.m_stripWidth2),
        m_emRadius(rhs.m_emRadius),
        m_ET12Frac(rhs.m_ET12Frac),
        m_etIsolHAD(rhs.m_etIsolHAD),
        m_etIsolEM(rhs.m_etIsolEM),
        m_etChrgHAD(rhs.m_etChrgHAD),
        m_nAssocTracksCore(rhs.m_nAssocTracksCore),
        m_nAssocTracksIsol(rhs.m_nAssocTracksIsol),
        m_signD0Trk3P(rhs.m_signD0Trk3P),
        m_massTrk3P(rhs.m_massTrk3P),
        m_rWidth2Trk3P(rhs.m_rWidth2Trk3P),
        m_z0SinThetaSig(rhs.m_z0SinThetaSig),
        m_etChrgHADoverPttot(rhs.m_etChrgHADoverPttot),
        m_etIsolFrac(rhs.m_etIsolFrac),
        m_cellEM012Cluster(rhs.m_cellEM012Cluster),
        m_etHadAtEMScale(rhs.m_etHadAtEMScale),
        m_etEMAtEMScale(rhs.m_etEMAtEMScale),
        m_etEMCL(rhs.m_etEMCL),
        m_etChrgEM(rhs.m_etChrgEM),
        m_etNeuEM(rhs.m_etNeuEM),
        m_etResNeuEM(rhs.m_etResNeuEM),
        m_trFlightPathSig(rhs.m_trFlightPathSig),	
        m_etChrgEM01Trk(rhs.m_etChrgEM01Trk),
        m_etResChrgEMTrk(rhs.m_etResChrgEMTrk),
        m_sumEM(rhs.m_sumEM),
        m_secVertex(new Trk::RecVertex(*rhs.m_secVertex)),
        m_pi0 (rhs.m_pi0),
        m_etEflow(rhs.m_etEflow),
        m_sumEtCellsLArOverLeadTrackPt(rhs.m_sumEtCellsLArOverLeadTrackPt),
        m_hadronicLeak(rhs.m_hadronicLeak),
        m_secondaryMax(rhs.m_secondaryMax)
    {
    }

    Tau1P3PDetails &Tau1P3PDetails :: operator = ( const Tau1P3PDetails &rhs )
    {
        if( this != &rhs )
        {
            delete m_secVertex;
            m_numStripCells= rhs.m_numStripCells;
            m_stripWidth2= rhs.m_stripWidth2;
            m_emRadius= rhs.m_emRadius;
            m_ET12Frac= rhs.m_ET12Frac;
            m_etIsolHAD = rhs.m_etIsolHAD;
            m_etIsolEM= rhs.m_etIsolEM;
            m_etChrgHAD = rhs.m_etChrgHAD;
            m_nAssocTracksCore = rhs.m_nAssocTracksCore;
            m_nAssocTracksIsol = rhs.m_nAssocTracksIsol;
            m_signD0Trk3P= rhs.m_signD0Trk3P;
            m_massTrk3P = rhs.m_massTrk3P;
            m_rWidth2Trk3P = rhs.m_rWidth2Trk3P;
            m_z0SinThetaSig= rhs.m_z0SinThetaSig;
            m_etChrgHADoverPttot = rhs.m_etChrgHADoverPttot;
            m_etIsolFrac= rhs.m_etIsolFrac;
            m_etHadAtEMScale = rhs.m_etHadAtEMScale;
            m_etEMAtEMScale= rhs.m_etEMAtEMScale;
            m_etEMCL = rhs.m_etEMCL;
            m_etChrgEM= rhs.m_etChrgEM;
            m_etNeuEM= rhs.m_etNeuEM;
            m_etResNeuEM= rhs.m_etResNeuEM;
            m_etChrgEM01Trk= rhs.m_etChrgEM01Trk;
            m_etResChrgEMTrk = rhs.m_etResChrgEMTrk;
            m_sumEM= rhs.m_sumEM;
            if (rhs.m_secVertex)
                m_secVertex = new Trk :: RecVertex( *rhs.m_secVertex );
            m_etEflow= rhs.m_etEflow;
            m_pi0  = rhs.m_pi0;
            m_cellEM012Cluster = rhs.m_cellEM012Cluster;
            m_trFlightPathSig= rhs.m_trFlightPathSig;
            m_sumEtCellsLArOverLeadTrackPt = rhs.m_sumEtCellsLArOverLeadTrackPt;
            m_hadronicLeak = rhs.m_hadronicLeak;
            m_secondaryMax = rhs.m_secondaryMax;
        }
        return *this;
    }

    double Tau1P3PDetails :: parameter( TauJetParameters :: Detail detail ) const
    {
        switch( detail )
        {
            case TauJetParameters :: emRadius:
                return emRadius();
            case TauJetParameters :: isolationFraction:
                return isolationFraction();
            case TauJetParameters :: stripWidth2:
                return stripWidth2();
            case TauJetParameters :: annularIsolationFraction:
                return isolationFraction();
            case TauJetParameters :: etCaloAtEMScale:
                return etEMAtEMScale() + etHadAtEMScale();
            case TauJetParameters :: etChargedHadCells:
                return etChrgHAD();
            case TauJetParameters :: etOtherEMCells:
                return etIsolEM();
            case TauJetParameters :: etOtherHadCells:
                return etIsolHAD();
                //            case TauJetParameters :: sumPTtracks:
                //                return sumPTTracks();
            case TauJetParameters :: nAssocTracksCore:
                return (double)nAssocTracksCore();
            case TauJetParameters :: nAssocTracksIsol:
                return (double)nAssocTracksIsol();
            case TauJetParameters :: rWidth2Trk3P:
                return rWidth2Trk3P();
            case TauJetParameters :: massTrk3P:
                return massTrk3P();
            case TauJetParameters :: signDOTrk3P:
                return signD0Trk3P();
            default:
                return TauDetails::DEFAULT;
        }
        return TauDetails::DEFAULT;
    }

    bool Tau1P3PDetails :: hasParameter( TauJetParameters :: Detail detail ) const
    {
        switch( detail )
        {
            case TauJetParameters :: emRadius:
            case TauJetParameters :: isolationFraction:
            case TauJetParameters :: stripWidth2:
            case TauJetParameters :: annularIsolationFraction:
            case TauJetParameters :: etCaloAtEMScale:
            case TauJetParameters :: etChargedHadCells:
            case TauJetParameters :: etOtherEMCells:
            case TauJetParameters :: etOtherHadCells:
            case TauJetParameters :: nAssocTracksCore:
            case TauJetParameters :: nAssocTracksIsol:
            case TauJetParameters :: rWidth2Trk3P:
            case TauJetParameters :: massTrk3P:
            case TauJetParameters :: signDOTrk3P:
                return true;
            default:
                return false;
        }
        return false;
    }

    std::string Tau1P3PDetails::s_className =
        ClassName<Tau1P3PDetails>::name();
    const std::string& Tau1P3PDetails::className() const
    {
        return s_className;
    }
}
