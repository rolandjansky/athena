/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauRecExtraDetails.h  DEPRECATED DO NOTT USE 
///- MIGRATE TO TauCommonDetails 
///
/// Definition of extra details class for TauRec
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// Created 2007-01-17
///
/// $Id: TauRecExtraDetails.h,v 1.11 2009-01-17 01:34:16 tburgess Exp $

#ifndef TAU_REC_EXTRA_DETAILS_ANALYSIS_H
#define TAU_REC_EXTRA_DETAILS_ANALYSIS_H

#include "tauEvent/TauDetails.h"
#include "tauEvent/tauAnalysisHelperObject.h"



namespace Analysis
{
    //-------------------------------------------------------------------------
    /// DEPRECATED DO NOT USE - MIGRATE TO TauCommonDetails
    //! Extended details class containing information produced by tauRec
    //! algorithm, goes to ESD only
    //-------------------------------------------------------------------------
    class TauRecExtraDetails: public TauDetails
    {
        friend class TauJet;
        friend class TauRecExtraDetailsCnv_p1;
        public:
        //-----------------------------------------------------------------
        // Constructors and destructor
        //-----------------------------------------------------------------
        //! Default Constructor
        TauRecExtraDetails();
        //! Contructor
        TauRecExtraDetails( const TauRecExtraDetails &rhs );
        //! assignment operator
        TauRecExtraDetails &operator = ( const TauRecExtraDetails &rhs );
        //! Destructor
        virtual ~TauRecExtraDetails();
        virtual const std::string& className() const;

        //-----------------------------------------------------------------
        //! Analysis Helper - this object is filled by tauAnalysisHelper
        //! and contains some useful information for tau performance
        //! studies
        //-----------------------------------------------------------------
        tauAnalysisHelperObject *analysisHelper() const;
        void setAnalysisHelper( tauAnalysisHelperObject *anahelper );

        //-----------------------------------------------------------------
        //! Number of EM cells within dR < 0.4, with E > m_cellEthr
        //-----------------------------------------------------------------
        int numEMCells() const;
        void setNumEMCells( int numEMCells );

        //-----------------------------------------------------------------
        //! Uncalibrated sum of ET in the strip layer within dR < 0.4
        //-----------------------------------------------------------------
        double stripET() const;
        void setStripET( double stripET );

        //-----------------------------------------------------------------
        //! EM Centrality Fraction (ET(dr<0.1)/ET(dr<0.4) for EM calos only
        //-----------------------------------------------------------------
        double emCentralityFraction() const;
        void setEMCentralityFraction( double emCentralityFraction );

        //-----------------------------------------------------------------
        //! Hadronic energy at the EM scale (unused)
        //-----------------------------------------------------------------
        double etHadAtEMScale();
        void setETHadAtEMScale( double etHadAtEMScale );

        //-----------------------------------------------------------------
        //! Electromagnetic energy at the EM scale (unused)
        //-----------------------------------------------------------------
        double etEMAtEMScale();
        void setETEMAtEMScale( double etEMAtEMScale );

        //-----------------------------------------------------------------
        //! Energy sum of all cells within dR < 0.4
        //-----------------------------------------------------------------
        double energy() const;
        void setEnergy( double energy );

        //-----------------------------------------------------------------
        //! Energy sum of all em cells within dR < 0.4
        //! (Presampler + EM1 + EM2)
        //-----------------------------------------------------------------
        double emEnergy() const;
        void setEMEnergy( double emEnergy );

        //-----------------------------------------------------------------
        //! Sum of PT of tracks
        //-----------------------------------------------------------------
        double sumPTTracks();
        void setSumPTTracks( double sumPTTracks );


        //-----------------------------------------------------------------
        //! The type of the seed
        //-----------------------------------------------------------------
        TauJetParameters :: SeedTypes seedType();
        void setSeedType( TauJetParameters :: SeedTypes type );

        private:
        static std::string s_className;
        tauAnalysisHelperObject *m_analysisHelper;
        TauJetParameters :: SeedTypes m_seedType;

        //-----------------------------------------------------------------
        // Identification variables
        //-----------------------------------------------------------------
        int    m_numEMCells;
        double m_stripET;
        double m_emCentralityFraction;

        //-----------------------------------------------------------------
        // Energy at the EM scale
        //-----------------------------------------------------------------
        double m_etHadAtEMScale;
        double m_etEMAtEMScale;
        double m_sumCellE;
        double m_sumEMCellE;
        double m_sumPTTracks;
    };

    //-------------------------------------------------------------------------
    // Implementation of accessors
    //-------------------------------------------------------------------------
    inline tauAnalysisHelperObject *TauRecExtraDetails :: analysisHelper() const
    {
        return m_analysisHelper;
    }

    inline void TauRecExtraDetails :: setAnalysisHelper( tauAnalysisHelperObject *anahelper )
    {
        delete m_analysisHelper;
        m_analysisHelper = anahelper;
    }

    inline int TauRecExtraDetails :: numEMCells() const
    {
        return m_numEMCells;
    }

    inline void TauRecExtraDetails :: setNumEMCells( int numEMCells )
    {
        m_numEMCells = numEMCells;
    }

    inline double TauRecExtraDetails :: stripET() const
    {
        return m_stripET;
    }

    inline void TauRecExtraDetails :: setStripET( double stripET )
    {
        m_stripET = stripET;
    }

    inline double TauRecExtraDetails :: emCentralityFraction() const
    {
        return m_emCentralityFraction;
    }

    inline void TauRecExtraDetails :: setEMCentralityFraction( double emCentralityFraction )
    {
        m_emCentralityFraction = emCentralityFraction;
    }

    inline double TauRecExtraDetails :: etHadAtEMScale()
    {
        return m_etHadAtEMScale;
    }

    inline void TauRecExtraDetails :: setETHadAtEMScale( double etHadAtEMScale )
    {
        m_etHadAtEMScale = etHadAtEMScale;
    }

    inline double TauRecExtraDetails :: etEMAtEMScale()
    {
        return m_etEMAtEMScale;
    }

    inline void TauRecExtraDetails :: setETEMAtEMScale( double etEMAtEMScale )
    {
        m_etEMAtEMScale = etEMAtEMScale;
    }

    inline double TauRecExtraDetails :: energy() const
    {
        return m_sumCellE;
    }

    inline void TauRecExtraDetails :: setEnergy( double energy )
    {
        m_sumCellE = energy;
    }

    inline double TauRecExtraDetails :: emEnergy() const
    {
        return m_sumEMCellE;
    }

    inline void TauRecExtraDetails :: setEMEnergy( double emEnergy )
    {
        m_sumEMCellE = emEnergy;
    }

    inline double TauRecExtraDetails :: sumPTTracks()
    {
        return m_sumPTTracks;
    }

    inline void TauRecExtraDetails :: setSumPTTracks( double sumPTTracks )
    {
        m_sumPTTracks = sumPTTracks;
    }

    inline TauJetParameters :: SeedTypes TauRecExtraDetails :: seedType()
    {
        return m_seedType;
    }

    inline void TauRecExtraDetails :: setSeedType( TauJetParameters :: SeedTypes type )
    {
        m_seedType = type;
    }
}

#endif // TAU_REC_EXTRA_DETAILS_ANALYSIS_H
