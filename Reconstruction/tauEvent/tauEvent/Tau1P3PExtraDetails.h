/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file Tau1P3PExtraDetails.h  DEPRECATED DO NOT USE
/// - MIGRATE TO TauCommonDetails
///
/// Declaration of extra details class for Tau1P3P
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///@author Elzbieta Richter-Was
///@author Anna Kaczmarska
///
/// Created 2007-01-17
///
/// $Id: Tau1P3PExtraDetails.h,v 1.9 2009-01-17 01:34:16 tburgess Exp $

#ifndef TAU1P3P_EXTRA_DETAILS_ANALYSIS_H
#define TAU1P3P_EXTRA_DETAILS_ANALYSIS_H

#include "tauEvent/TauDetails.h"
#include "CaloEvent/CaloClusterContainer.h"
#include <vector>

namespace Analysis
{
    //-------------------------------------------------------------------------
    /// DEPRECATED DO NOT USE - MIGRATE TO TauCommonDetails
    //! Extended details class containing information produced by tau1P3P
    //! algorithm, goes to ESD only
    //-------------------------------------------------------------------------
    class Tau1P3PExtraDetails: public TauDetails
    {
        friend class TauJet;
        friend class Tau1P3PExtraDetailsCnv_p1;
        public:
        //-----------------------------------------------------------------
        // Constructors,  destructor, operators
        //-----------------------------------------------------------------

        //! Constructor
        Tau1P3PExtraDetails();

        ///Copy constructor
        ///@param rhs Tau1P3PExtraDetails instance to copy
        Tau1P3PExtraDetails(const Tau1P3PExtraDetails &rhs );

        //! Destructor
        virtual ~Tau1P3PExtraDetails();

        /// Assignment operator
        /// @param rhs Tau1P3PExtrauDetails instance to assign from
        /// @return assigned object
        Tau1P3PExtraDetails& operator = (
                const Tau1P3PExtraDetails& rhs );

        virtual const std::string& className() const;

        //-----------------------------------------------------------------
        //! Allocate data structures for given number of tracks and
        //! samplings
        //-----------------------------------------------------------------
        void allocTracksAndSamplings( unsigned tracks, unsigned samplings );

        //-----------------------------------------------------------------
        //! Closest cell to track position at vertex in eta for
        //! track "itrk" in samp "num"
        //-----------------------------------------------------------------
        const CaloCell *closestEtaTrkVertCell( int itrk, int num ) const;
        void setClosestEtaTrkVertCell( const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk, int num );
        void setClosestEtaTrkVertCellLink( const ElementLink<CaloCellContainer>& el,
                                           int itrk, int num );

        //-----------------------------------------------------------------
        //! Closest cell to track position at vertex in phi for
        //! track "itrk" in samp "num"
        //-----------------------------------------------------------------
        const CaloCell *closestPhiTrkVertCell( int itrk, int num ) const;
        void setClosestPhiTrkVertCell( const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk, int num );
        void setClosestPhiTrkVertCellLink( const ElementLink<CaloCellContainer>& el,
                                           int itrk, int num );

        //-----------------------------------------------------------------
        //! Closest cell to track position from extrapolation in eta for
        //! track "itrk" in samp "num"
        //-----------------------------------------------------------------
        const CaloCell *closestEtaTrkCell( int itrk, int num ) const;
        void setClosestEtaTrkCell( const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk, int num );
        void setClosestEtaTrkCellLink( const ElementLink<CaloCellContainer>& el,
                                       int itrk, int num );

        //-----------------------------------------------------------------
        //! Closest cell to track position from extrapolation in phi for
        //! track "itrk" in samp "num"
        //-----------------------------------------------------------------
        const CaloCell *closestPhiTrkCell( int itrk,int num ) const;
        void setClosestPhiTrkCell( const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk, int num );
        void setClosestPhiTrkCellLink( const ElementLink<CaloCellContainer>& el,
                                       int itrk, int num );

        //-----------------------------------------------------------------
        //! Eta of extrapolation point in EM Calo in "i" sampling for track "itrk"
        //-----------------------------------------------------------------
        double etaTrackCaloSamp( int itrk, int i ) const;
        void setEtaTrackCaloSamp( int itrk, int i, double eta );

        //-----------------------------------------------------------------
        //! Phi of extrapolation point in EM Calo in "i" sampling for track "itrk"
        //-----------------------------------------------------------------
        double phiTrackCaloSamp( int itrk, int i ) const;
        void setPhiTrackCaloSamp( int itrk, int i, double eta );

        //-----------------------------------------------------------------
        //! Sum of PT of tracks
        //-----------------------------------------------------------------
        double sumPTTracks();
        void setSumPTTracks( double sumPTTracks );

        unsigned int ntrack() const;
        unsigned int nsamp() const;


        private:
        static std::string s_className;
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestEtaTrkVertCell;
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestEtaTrkCell;
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestPhiTrkVertCell;
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestPhiTrkCell;
        std::vector<std::vector<double> > m_etaTrackCaloSamp;
        std::vector<std::vector<double> > m_phiTrackCaloSamp;
        double m_sumPTTracks;
    };

    //-------------------------------------------------------------------------
    // Allocate data structures
    //-------------------------------------------------------------------------
    inline void Tau1P3PExtraDetails :: allocTracksAndSamplings( unsigned tracks, unsigned samplings )
    {
        if( tracks == 0 || samplings == 0 )
            return;

        std::vector<ElementLink<CaloCellContainer> > initEL( samplings );

        m_closestEtaTrkVertCell.clear();
        m_closestEtaTrkVertCell.resize( tracks, initEL );

        m_closestEtaTrkCell.clear();
        m_closestEtaTrkCell.resize( tracks, initEL );

        m_closestPhiTrkVertCell.clear();
        m_closestPhiTrkVertCell.resize( tracks, initEL );

        m_closestPhiTrkCell.clear();
        m_closestPhiTrkCell.resize( tracks, initEL );

        std::vector<double> initD( samplings, TauDetails::DEFAULT );
        m_etaTrackCaloSamp.clear();
        m_etaTrackCaloSamp.resize( tracks, initD );
        m_phiTrackCaloSamp.clear();
        m_phiTrackCaloSamp.resize( tracks, initD );
    }

    //-------------------------------------------------------------------------
    // Cells
    //-------------------------------------------------------------------------
    inline const CaloCell *Tau1P3PExtraDetails :: closestEtaTrkVertCell( int itrk, int num ) const
    {
        if( m_closestEtaTrkVertCell[itrk][num].isValid() )
            return *m_closestEtaTrkVertCell[itrk][num];
        else
            return 0;
    }

    inline void Tau1P3PExtraDetails :: setClosestEtaTrkVertCell( const CaloCell *val,
            const CaloCellContainer *cont,
            int itrk, int num )
    {
        m_closestEtaTrkVertCell[itrk][num].toContainedElement( *cont,val );
    }

    inline void Tau1P3PExtraDetails :: setClosestEtaTrkVertCellLink( const ElementLink<CaloCellContainer>& el,
                                                                     int itrk, int num)
    {
      m_closestEtaTrkVertCell[itrk][num] = el;
    }

    inline const CaloCell *Tau1P3PExtraDetails :: closestPhiTrkVertCell( int itrk, int num ) const
    {
        if( m_closestPhiTrkVertCell[itrk][num].isValid() )
            return *m_closestPhiTrkVertCell[itrk][num];
        else
            return 0;
    }

    inline void Tau1P3PExtraDetails :: setClosestPhiTrkVertCell( const CaloCell *val,
            const CaloCellContainer *cont,
            int itrk, int num )
    {
        m_closestPhiTrkVertCell[itrk][num].toContainedElement( *cont, val );
    }


    inline void Tau1P3PExtraDetails :: setClosestPhiTrkVertCellLink( const ElementLink<CaloCellContainer>& el,
                                                                     int itrk, int num)
    {
      m_closestPhiTrkVertCell[itrk][num] = el;
    }

    inline const CaloCell *Tau1P3PExtraDetails :: closestEtaTrkCell( int itrk, int num ) const
    {
        if( m_closestEtaTrkCell[itrk][num].isValid() )
            return *m_closestEtaTrkCell[itrk][num];
        else
            return 0;
    }

    inline void Tau1P3PExtraDetails :: setClosestEtaTrkCell( const CaloCell *val,
            const CaloCellContainer *cont,
            int itrk, int num )
    {
        m_closestEtaTrkCell[itrk][num].toContainedElement( *cont, val );
    }

    inline void Tau1P3PExtraDetails :: setClosestEtaTrkCellLink( const ElementLink<CaloCellContainer>& el,
                                                                 int itrk, int num)
    {
      m_closestEtaTrkCell[itrk][num] = el;
    }

    inline const CaloCell *Tau1P3PExtraDetails :: closestPhiTrkCell( int itrk, int num ) const
    {
        if( m_closestPhiTrkCell[itrk][num].isValid() )
            return *m_closestPhiTrkCell[itrk][num];
        else
            return 0;
    }

    inline void Tau1P3PExtraDetails :: setClosestPhiTrkCell( const CaloCell *val,
            const CaloCellContainer *cont,
            int itrk, int num )
    {
        m_closestPhiTrkCell[itrk][num].toContainedElement( *cont, val );
    }

    inline void Tau1P3PExtraDetails :: setClosestPhiTrkCellLink( const ElementLink<CaloCellContainer>& el,
                                                                 int itrk, int num)
    {
      m_closestPhiTrkCell[itrk][num] = el;
    }

    //-------------------------------------------------------------------------
    // Calo samples
    //-------------------------------------------------------------------------
    inline double Tau1P3PExtraDetails :: etaTrackCaloSamp( int itrk, int i ) const
    {
        return m_etaTrackCaloSamp[itrk][i];
    }

    inline void Tau1P3PExtraDetails :: setEtaTrackCaloSamp( int itrk, int i,
            double eta )
    {
        m_etaTrackCaloSamp[itrk][i] = eta ;
    }

    inline double Tau1P3PExtraDetails :: phiTrackCaloSamp( int itrk, int i ) const
    {
        return m_phiTrackCaloSamp[itrk][i];
    }

    inline void Tau1P3PExtraDetails :: setPhiTrackCaloSamp( int itrk, int i,
            double phi )
    {
        m_phiTrackCaloSamp[itrk][i] = phi ;
    }

    inline double Tau1P3PExtraDetails :: sumPTTracks()
    {
        return m_sumPTTracks;
    }

    inline void Tau1P3PExtraDetails :: setSumPTTracks( double sumPTTracks )
    {
        m_sumPTTracks = sumPTTracks;
    }
}

#endif // TAU1P3P_EXTRA_DETAILS_ANALYSIS_H
