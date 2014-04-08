/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauCommonExtraDetails.h
///
///Declaration of extra details class for tau event
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess
///
/// Created by Thomas Burgess on 2008-10-20.

#ifndef __TAU_COMMON_EXTRA_DETAILS_ANALYSIS_H__
#define __TAU_COMMON_EXTRA_DETAILS_ANALYSIS_H__

//TauEvent includes
#include "tauEvent/TauDetails.h"
//Other inlcudes
#include "DataModel/ElementLink.h"
#include "CaloEvent/CaloCellContainer.h"
//Standard C/C++ includes
#include <vector>

namespace Analysis {

    /// Extra detail class containing information for track and calo
    /// seeded tau
    /// reconstruction algorithms. Is saved only to ESD output.
    class TauCommonExtraDetails : public TauDetails {
        friend class TauCommonExtraDetailsCnv_p1;
        friend class TauJet;
        public:
        //-----------------------------------------------------------------
        // Constructors, destructor, operators
        //-----------------------------------------------------------------

        ///Default constructor
        TauCommonExtraDetails();

        ///Copy constructor
        ///@param rhs TauCommonExtraDetails instance to assign from
        TauCommonExtraDetails(
                const TauCommonExtraDetails& rhs );

        ///Virtual destructor
        virtual ~TauCommonExtraDetails();

        ///Assignment operator
        ///@param rhs TauCommonExtraDetails instance to assign from
        ///@return assigned object
        TauCommonExtraDetails& operator = (
                const TauCommonExtraDetails& rhs );

        /// Equality operator
        /// @param rhs right hand side TauCommonDetails instance to compare 
        /// @return true if equal, false otherwise
        bool operator==( const TauCommonExtraDetails& rhs ) const;

        /// Inequality operator
        /// @param rhs TauCommonDetails instance to compare to
        /// @return !operator==
        bool operator!=( const TauCommonExtraDetails& rhs ) const;

        ///Get name of class
        ///@return name of class
        virtual const std::string& className() const;

        /// Allocate data structures for given number of tracks and samplings
        ///@param tracks Number of tracks
        ///@param samplings Number of samplings
        void allocTracksAndSamplings(
                unsigned int tracks,
                unsigned int samplings );

        ///////////////////////////////////////////////////////////////////
        // Private member accessors
        ///////////////////////////////////////////////////////////////////

        //
        // Common for track and calo seeded tau reconstruction
        //

        //  ---------------------------------------------------------------
        /// @name Accessors for sumPtLooseTrk
        /// @copydoc Analysis::TauCommonExtraDetails::m_sumPtLooseTrk
        //@{ --------------------------------------------------------------

        ///Get sum of p_T for loose tracks
        ///@return p_T
        double sumPtLooseTrk() const;

        ///Set sum of p_T for loose tracks
        ///@param pt p_T
        void setSumPtLooseTrk(
                double pt);

        //@} --------------------------------------------------------------
        /// @name Accessors for sumPtTrk
        /// @copydoc Analysis::TauCommonExtraDetails::m_sumPtTrk
        //@{ --------------------------------------------------------------

        ///Get sum of p_T for loose tracks
        ///@return p_T
        double sumPtTrk() const;

        ///Set sum of p_T for loose tracks
        ///@param pt p_T
        void setSumPtTrk(
                double pt);

        //@} --------------------------------------------------------------
        /// @name Accessors for closestEtaTrkVertCell
        /// @copydoc Analysis::TauCommonExtraDetails::m_closestEtaTrkVertCell
        //@{ --------------------------------------------------------------

        ///Get closest eta vector
        ///@return closest eta vector
        const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestEtaTrkVertCell() const;

        ///Get closest eta vector
        ///@return const closest eta vector
        std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestEtaTrkVertCell();

        ///Get CaloCell in closest eta vector
        ///@return CaloCell in closest eta vector
        const CaloCell *closestEtaTrkVertCell(
                int itrk,
                int num ) const;

        ///Set closest eta vector
        ///@param closestEtaTrkVertCell closest eta vector
        void setClosestEtaTrkVertCell(
                const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
                closestEtaTrkVertCell);

        //Set closest eta vector by element
        //@param closestEtaTrkVertCell closest eta vector
        void setClosestEtaTrkVertCell(
                const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk,
                int num );

        //@} --------------------------------------------------------------
        /// @name Accessors for closestPhiTrkVertCell
        /// @copydoc Analysis::TauCommonExtraDetails::m_closestPhiTrkVertCell
        //@{ --------------------------------------------------------------

        ///Get closest phi vector
        ///@return const closest phi vector
        const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestPhiTrkVertCell() const;

        ///Get closest phi vector
        ///@return closest phi vector
        std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestPhiTrkVertCell();

        ///Get CaloCell in closest phi vector
        ///@return CaloCell in closest phi vector
        const CaloCell *closestPhiTrkVertCell(
                int itrk,
                int num ) const;

        ///Set closest phi vector
        ///@param closestPhiTrkVertCell closest phi vector
        void setClosestPhiTrkVertCell(
                const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
                closestPhiTrkVertCell);

        //Set closest phi vector by element
        //@param closestPhiTrkVertCell closest phi vector
        void setClosestPhiTrkVertCell(
                const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk,
                int num );

        //@} --------------------------------------------------------------
        /// @name Accessors for closestEtaTrkCell
        /// @copydoc Analysis::TauCommonExtraDetails::m_closestEtaTrkCell
        //@{ --------------------------------------------------------------

        ///Get closest eta vector
        ///@return const closest eta vector
        const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestEtaTrkCell() const;

        ///Get closest eta vector
        ///@return closest eta vector
        std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestEtaTrkCell();

        ///Get CaloCell in closest eta vector
        ///@return CaloCell in closest eta vector
        const CaloCell *closestEtaTrkCell( int itrk, int num ) const;

        ///Set closest eta vector
        ///@param closestEtaTrkCell closest eta vector
        void setClosestEtaTrkCell(
                const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
                closestEtaTrkCell);

        //Set closest eta vector by element
        //@param closestEtaTrkCell closest eta vector
        void setClosestEtaTrkCell(
                const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk,
                int num );

        //@} --------------------------------------------------------------
        /// @name Accessors for closestPhiTrkCell
        /// @copydoc Analysis::TauCommonExtraDetails::m_closestPhiTrkCell
        //@{ --------------------------------------------------------------

        ///Get closest phi vector
        ///@return constant closest phi vector
        const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestPhiTrkCell() const;

        ///Get closest phi vector
        ///@return closest phi vector
        std::vector<std::vector<ElementLink<CaloCellContainer> > >&
            closestPhiTrkCell();

        ///Get CaloCell in closest phi vector
        ///@return CaloCell in closest phi vector
        const CaloCell *closestPhiTrkCell( int itrk, int num ) const;

        ///Set closest phi vector
        ///@param closestPhiTrkCell closest phi vector
        void setClosestPhiTrkCell(
                const std::vector<std::vector<ElementLink<CaloCellContainer> > >&
                closestPhiTrkCell);

        //Set closest eta vector by element
        //@param closestPhiTrkCell closest eta vector
        void setClosestPhiTrkCell(
                const CaloCell *val,
                const CaloCellContainer *cont,
                int itrk,
                int num );

        //@} --------------------------------------------------------------
        /// @name Accessors for etaTrkCaloSamp
        /// @copydoc Analysis::TauCommonExtraDetails::m_etaTrkCaloSamp
        //@{ --------------------------------------------------------------

        ///Get eta vector
        ///@return const eta vector
        const std::vector<std::vector<double> > &
            etaTrkCaloSamp() const;

        ///Get eta vector
        ///@return eta
        std::vector<std::vector<double> > &
            etaTrkCaloSamp();

        ///Set eta vector
        ///@param etaTrkCell eta vector
        void setEtaTrkCaloSamp(
                const std::vector<std::vector<double> > &etaTrkCaloSamp);

        //@} --------------------------------------------------------------
        /// @name Accessors for phiTrkCaloSamp
        /// @copydoc Analysis::TauCommonExtraDphiils::m_phiTrkCaloSamp
        //@{ --------------------------------------------------------------

        ///Get phi vector
        ///@return const phi vector
        const std::vector<std::vector<double> > &
            phiTrkCaloSamp() const;

        ///Get phi vector
        ///@return phi vector
        std::vector<std::vector<double> > &
            phiTrkCaloSamp();

        ///Set phi vector
        ///@param phiTrkCell phi vector
        void setPhiTrkCaloSamp(
                const std::vector<std::vector<double> > &phiTrkCaloSamp);



        //@} --------------------------------------------------------------
        /// @name Accessors for etaLooseTrkCaloSamp
        /// @copydoc Analysis::TauCommonExtraDetails::m_etaLooseTrkCaloSamp
        //@{ --------------------------------------------------------------

        ///Get eta vector
        ///@return const eta vector
        const std::vector<std::vector<double> > &
            etaLooseTrkCaloSamp() const;

        ///Get eta vector
        ///@return eta
        std::vector<std::vector<double> > &
            etaLooseTrkCaloSamp();

        ///Set eta vector
        ///@param etaTrkCell eta vector
        void setEtaLooseTrkCaloSamp(
                const std::vector<std::vector<double> > &etaLooseTrkCaloSamp);

        //@} --------------------------------------------------------------
        /// @name Accessors for phiLooseTrkCaloSamp
        /// @copydoc Analysis::TauCommonExtraDphiils::m_phiLooseTrkCaloSamp
        //@{ --------------------------------------------------------------

        ///Get phi vector
        ///@return const phi vector
        const std::vector<std::vector<double> > &
            phiLooseTrkCaloSamp() const;

        ///Get phi vector
        ///@return phi vector
        std::vector<std::vector<double> > &
            phiLooseTrkCaloSamp();

        ///Set phi vector
        ///@param phiTrkCell phi vector
        void setPhiLooseTrkCaloSamp(
                const std::vector<std::vector<double> > &phiLooseTrkCaloSamp);


        //
        // Applicable for calo seeded tau reconstruction
        //

        //@} --------------------------------------------------------------
        /// @name Accessors for nEMCell
        /// @copydoc Analysis::TauCommonExtraDetails::m_nEMCell
        //@{ --------------------------------------------------------------

        ///Get number of EM cells
        ///@return n number of cells
        int seedCalo_nEMCell() const;

        ///Setumber of EM cells
        ///@param n number of cells
        void setSeedCalo_nEMCell(
                int n);

        //@} --------------------------------------------------------------
        /// @name Accessors for stripEt
        /// @copydoc Analysis::TauCommonExtraDetails::m_stripEt
        //@{ --------------------------------------------------------------

        ///Get sum of ET in strip layer
        ///@return et E_t
        double seedCalo_stripEt() const;

        ///Set sum of ET in strip layer
        ///@param et E_t
        void setSeedCalo_stripEt(
                double et);

        //@} --------------------------------------------------------------
        /// @name Accessors for
        /// @copydoc Analysis::TauCommonExtraDetails::m_
        //@{ --------------------------------------------------------------

        ///Set EM Centrality Fraction
        ///@return Centrality fraction
        double seedCalo_EMCentFrac() const;

        ///Get EM Centrality Fraction
        ///@param fraction Centrality fraction
        void setSeedCalo_EMCentFrac(
                double fraction);

        //@} --------------------------------------------------------------
        /// @name Accessors for sumCellEnergy
        /// @copydoc Analysis::TauCommonExtraDetails::m_sumCellEnergy
        //@{ --------------------------------------------------------------

        ///Get energy sum
        ///@return Energy sum
        double seedCalo_sumCellEnergy() const;

        ///Set energy sum
        ///@param energy Energy sum
        void setSeedCalo_sumCellEnergy(
                double energy);

        //@} --------------------------------------------------------------
        /// @name Accessors for sumEMCellEnergy
        /// @copydoc Analysis::TauCommonExtraDetails::m_sumEMCellEnergy
        //@{ --------------------------------------------------------------

        ///Get EM energy sum
        ///@return Energy sum
        double seedCalo_sumEMCellEnergy() const;

        ///Set EM energy sum
        ///@param energy Energy sum
        void setSeedCalo_sumEMCellEnergy(
                double energy);

        //@}

        protected:
        ///Printing function
        ///@param o string to print to
        template <class stream>
            void printOn(stream& o) const; 

        private:
        ///Name of class
        static std::string s_className;

        // -------------------------------------------------------------------
        ///@name Members applicable for track & calo seeded tau reconstruction
        //@{ -----------------------------------------------------------------

        /// Sum of p_T for loose tracks
        double m_sumPtLooseTrk;

        /// Sum of p_T for tracks
        double m_sumPtTrk;

        /// Closest cell to track position at vertex in eta
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestEtaTrkVertCell;

        /// Closest cell to track position at vertex in phi
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestPhiTrkVertCell;

        /// Closest cell to track position from extrapolation in eta
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestEtaTrkCell;

        /// Closest cell to track position from extrapolation in phi
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<ElementLink<CaloCellContainer> > >
            m_closestPhiTrkCell;

        /// Eta of extrapolation point in EM Calo
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<double> > m_etaTrkCaloSamp;

        /// Phi of extrapolation point in EM Calo
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<double> > m_phiTrkCaloSamp;


        /// Eta of extrapolation point in EM Calo
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<double> > m_etaLooseTrkCaloSamp;

        /// Phi of extrapolation point in EM Calo
        ///
        /// Indices are [track][sample]
        std::vector<std::vector<double> > m_phiLooseTrkCaloSamp;


        //@} -----------------------------------------------------------------
        ///@name Members applicable for calo seeded tau reconstruction
        //@{ -----------------------------------------------------------------

        /// Number of EM cells within dR < 0.4, with E > m_cellEthr
        ///
        /// Applicable for calo seeded tau rec
        int m_seedCalo_nEMCell;

        /// Uncalibrated sum of ET in the strip layer within dR < 0.4
        ///
        /// Applicable for calo seeded tau rec
        double m_seedCalo_stripEt;

        /// EM Centrality Fraction (ET(dr<0.1)/ET(dr<0.4) for EM calos only
        ///
        /// Applicable for calo seeded tau rec
        double m_seedCalo_EMCentFrac;

        /// Energy sum of all cells within dR < 0.4
        ///
        /// Applicable for calo seeded tau rec
        double m_seedCalo_sumCellEnergy;

        /// Energy sum of all em cells within dR < 0.4
        /// (Presampler + EM1 + EM2)
        ///
        /// Applicable for calo seeded tau rec
        double m_seedCalo_sumEMCellEnergy;

        //@}

    }; //class TauCommonExtraDetails

} //Namespace Analysis

#include "TauCommonExtraDetails.icc"

#endif //__TAU_COMMON_EXTRA_DETAILS_ANALYSIS_H__
