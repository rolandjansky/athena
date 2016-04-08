/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauRecDetails.h  DEPRECATED DO NOT USE 
/// - MIGRATE TO TauCommonDetails 
///
/// Defintion of details class for TauRec
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///
/// $Id: TauRecDetails.h,v 1.13 2009-01-17 01:34:16 tburgess Exp $

#ifndef TAU_REC_DETAILS_ANALYSIS_H
#define TAU_REC_DETAILS_ANALYSIS_H

#include "tauEvent/TauDetails.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "VxVertex/RecVertex.h"
#include "DataModel/ElementLinkVector.h"
#include "Particle/TrackParticleContainer.h"
#include <vector>

namespace Analysis
{

    //-------------------------------------------------------------------------
    /// DEPRECATED DO NOT USE - MIGRATE TO TauCommonDetails
    //! Basic details class containing information produced by tauRec
    //! algorithm, goes to both ESD and AOD
    //-------------------------------------------------------------------------
    class TauRecDetails: public TauDetails
    {
        friend class TauJet;
        friend class TauRecDetailsCnv_p1;
        public:
        //-----------------------------------------------------------------
        //! Constructor
        //-----------------------------------------------------------------
        TauRecDetails();

        //-----------------------------------------------------------------
        //! Destructor
        //-----------------------------------------------------------------
        virtual ~TauRecDetails();

        //-----------------------------------------------------------------
        //! Copy Constructor
        //-----------------------------------------------------------------
        TauRecDetails( const TauRecDetails &rhs );

        //-----------------------------------------------------------------
        //! Assignment Operator
        //-----------------------------------------------------------------
        TauRecDetails &operator = ( const TauRecDetails &rhs );

        virtual const std::string& className() const;

        //-----------------------------------------------------------------
        //! backward compatibility accessor (do not use)
        //-----------------------------------------------------------------
        virtual double parameter( TauJetParameters :: Detail detail ) const;

        //-----------------------------------------------------------------
        //! backward compatibility accessor (do not use)
        //-----------------------------------------------------------------
        virtual bool hasParameter( TauJetParameters :: Detail detail ) const;

        //-----------------------------------------------------------------
        //! Uncalibrated Et weighted radius in the Presampler + EM1 + EM2 within dR < 0.4
        //-----------------------------------------------------------------
        double emRadius() const;
        void setEMRadius( double EMRadius );

        //-----------------------------------------------------------------
        //! Uncalibrated HCAL weighted radius
        //-----------------------------------------------------------------
        double hadRadius() const;
        void setHadRadius( double HadRadius );

        //-----------------------------------------------------------------
        //! Uncalibrated Sum of Cell Transverse energy in the Presampler + EM1 + EM2 within dR < 0.4
        //-----------------------------------------------------------------
        double sumEmCellEt() const;
        void setSumEmCellEt( double SumEmCellEt );

        //-----------------------------------------------------------------
        //! Uncalibrated Sum of Cell Transverse energy in the HCAL
        //-----------------------------------------------------------------
        double sumHadCellEt() const;
        void setSumHadCellEt( double SumHadCellEt );

        //-----------------------------------------------------------------
        //! Ratio of the uncalibrated transverse energy of cells within 0.1<dR<0.2 and cells within 0<dR<0.4
        //-----------------------------------------------------------------
        double isolationFraction() const;
        void setIsolationFraction( double isolationFraction );

        //-----------------------------------------------------------------
        //! Centrality fraction (ET(dr<0.1)/ET(dr<0.4) for all calos
        //-----------------------------------------------------------------
        double centralityFraction() const;
        void setCentralityFraction( double centralityFraction );

        //-----------------------------------------------------------------
        //! Uncalibrated transverse energy weighted width in the strip layer within dR < 0.4
        //-----------------------------------------------------------------
        double stripWidth2() const;
        void setStripWidth2( double stripWidth2 );

        //-----------------------------------------------------------------
        //! Number of Strip cells within dR < 0.4, with energy above specified threshold
        //-----------------------------------------------------------------
        int numStripCells() const;
        void setNumStripCells( int numStripCells );

        //-----------------------------------------------------------------
        //! Impact Parameter Significance of Leading Track
        //-----------------------------------------------------------------
        double ipSigLeadTrack() const;
        void setIpSigLeadTrack( double ipSigLeadTrack );

        //-----------------------------------------------------------------
        //! Ratio of Transverse Energy of Tau Candidate to Transverse Momentum of Leading Track
        //-----------------------------------------------------------------
        double etOverPtLeadTrack() const;
        void setEtOverPtLeadTrack( double etOverPtLeadTrack );

        //-----------------------------------------------------------------
        //! Charge of loose tracks
        //-----------------------------------------------------------------

        /** Get the charge of loose tracks 
         *@return Charge of loose tracks
         */
        int chargeLooseTracks(  ) const;

        /** Set the charge of loose tracks 
         *@param chargeLooseTracks Charge of loose tracks
         */
        void setChargeLooseTracks( const int chargeLooseTracks );

        //-----------------------------------------------------------------
        //! EM part of the energy (Presampler+EM1 and EM2+EM3 with no nearby track)
        //-----------------------------------------------------------------
        const CLHEP::HepLorentzVector &sumEM() const;
        void setSumEM( const CLHEP::HepLorentzVector &sumEM );

        //-----------------------------------------------------------------
        //! Calibrated EM transverse energy, EM is Presampler + EM1 + EM2
        //-----------------------------------------------------------------
        double etEMCalib() const;
        void setETEMCalib( double etEMCalib );

        //-----------------------------------------------------------------
        //! Calibrated HAD transverse energy,
        //! HAD is cryo + EM3 + TILE1 + TILE2 + TILE3
        //-----------------------------------------------------------------
        double etHadCalib() const;
        void setETHadCalib( double etHadCalib );

        //-----------------------------------------------------------------
        //! Secondary vertex 
        //-----------------------------------------------------------------
        void setSecVertex( const Trk :: RecVertex *secVertex );
        const Trk :: RecVertex *secVertex() const;

        //-----------------------------------------------------------------
        //! only used internally for non-default tracks (original tauRec tracks)
        //-----------------------------------------------------------------
        unsigned numTrack() const;
        void setNumTrack( unsigned n );

        //-----------------------------------------------------------------
        //! eta of track i extrapolated to EM layer 2
        //-----------------------------------------------------------------
        double trackCaloEta( int i ) const;
        void setTrackCaloEta( int i, double trackCaloEta );

        //-----------------------------------------------------------------
        //! phi of track i extrapolated to EM layer 2
        //-----------------------------------------------------------------
        double trackCaloPhi( int i ) const;
        void setTrackCaloPhi( int i, double trackCaloPhi );

        //-----------------------------------------------------------------
        //! PT of leading track - for Trigger
        //-----------------------------------------------------------------
        double leadingTrackPT() const;
        void setLeadingTrackPT( double leadingTrackPT );

        //-----------------------------------------------------------------
        //! Transverse flight path significance for taus with reconstructed secondary vertex
        //-----------------------------------------------------------------
        double trFlightPathSig() const;
        void setTrFlightPathSig( double trFlightPathSig );

        //-----------------------------------------------------------------
        //! eta of @c TauJet calculated from calorimeter (for merged algorithm)
        //-----------------------------------------------------------------
        double etaCalo() const;
        void setEtaCalo( double eta );

        //-----------------------------------------------------------------
        //! phi of @c TauJet calculated from calorimeter (for merged algorithm)
        //-----------------------------------------------------------------
        double phiCalo() const;
        void setPhiCalo( double phi );

        //-----------------------------------------------------------------
        //! Number of tracks pT>1GeV between dR = cmsdr and dR = cmsdR
        //! (from the jobOptions)
        //-----------------------------------------------------------------
        int nTracksdrdR() const;
        void setNTracksdrdR( int nTracksdrdR );

        //-----------------------------------------------------------------
        //! Accessor to loose tracks
        //-----------------------------------------------------------------
        const Rec :: TrackParticle *looseTrack( unsigned i ) const;

        //-----------------------------------------------------------------
        //! Number of Loose Tracks associated to the TauJet
        //-----------------------------------------------------------------
        unsigned numLooseTrack() const;

        //-----------------------------------------------------------------
        //! Associate a loose track to a TauJet
        //-----------------------------------------------------------------
        void addLooseTrack( const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle          *track );



        ElementLinkVector<Rec::TrackParticleContainer>& looseTracks();
        const ElementLinkVector<Rec::TrackParticleContainer>& looseTracks() const;

        private:
        static std::string s_className;

        ElementLinkVector<Rec::TrackParticleContainer> m_looseTracks;

        //-----------------------------------------------------------------
        // Identification variables
        //-----------------------------------------------------------------
        double m_emRadius;
        double m_hadRadius;
        double m_sumEmCellEt;
        double m_sumHadCellEt;
        double m_ET12Frac;
        double m_centralityFraction;
        double m_stripWidth2;
        int    m_numStripCells;
        double m_ipSigLeadTrack;
        double m_etOverPtLeadTrack;

        //-----------------------------------------------------------------
        // Energy
        //-----------------------------------------------------------------
        CLHEP::HepLorentzVector m_sumEM;

        //-----------------------------------------------------------------
        // Recalibrated energy
        //-----------------------------------------------------------------
        double m_etEMCalib;
        double m_etHadCalib;

        //-----------------------------------------------------------------
        // Secondary Vertex
        //-----------------------------------------------------------------
        Trk :: RecVertex *m_secVertex;

        //-----------------------------------------------------------------
        // TODO: Comment
        //-----------------------------------------------------------------
        std::vector<double> m_trackCaloEta;
        std::vector<double> m_trackCaloPhi;

        double m_leadingTrackPT;
        double m_trFlightPathSig;

        double m_etaCalo;
        double m_phiCalo;


        //-----------------------------------------------------------------
        // Alternative identification variables
        //-----------------------------------------------------------------
        int    m_nTracksdrdR;

        ///Charge of loose tracks
        int    m_chargeLooseTracks;            
    };

    //-------------------------------------------------------------------------
    // Implementation of accessors - Identification part
    //-------------------------------------------------------------------------
    inline double TauRecDetails :: emRadius() const
    {
        return m_emRadius;
    }

    inline void TauRecDetails :: setEMRadius( double emRadius )
    {
        m_emRadius = emRadius;
    }

    inline double TauRecDetails :: hadRadius() const
    {
        return m_hadRadius;
    }

    inline void TauRecDetails :: setHadRadius( double HadRadius )
    {
        m_hadRadius = HadRadius;
    }

    inline double TauRecDetails :: sumEmCellEt() const
    {
        return m_sumEmCellEt;
    }

    inline void TauRecDetails :: setSumEmCellEt( double SumEmCellEt )
    {
        m_sumEmCellEt = SumEmCellEt;
    }

    inline double TauRecDetails :: sumHadCellEt() const
    {
        return m_sumHadCellEt;
    }

    inline void TauRecDetails :: setSumHadCellEt( double SumHadCellEt )
    {
        m_sumHadCellEt = SumHadCellEt;
    }

    inline double TauRecDetails :: isolationFraction() const
    {
        return m_ET12Frac;
    }

    inline void TauRecDetails :: setIsolationFraction( double isolationFraction )
    {
        m_ET12Frac = isolationFraction;
    }

    inline double TauRecDetails :: centralityFraction() const
    {
        return m_centralityFraction;
    }

    inline void TauRecDetails :: setCentralityFraction( double centralityFraction )
    {
        m_centralityFraction = centralityFraction;
    }

    inline double TauRecDetails :: stripWidth2() const
    {
        return m_stripWidth2;
    }

    inline void TauRecDetails :: setStripWidth2( double stripWidth2 )
    {
        m_stripWidth2 = stripWidth2;
    }

    inline int TauRecDetails :: numStripCells() const
    {
        return m_numStripCells;
    }

    inline void TauRecDetails :: setNumStripCells( int numStripCells )
    {
        m_numStripCells = numStripCells;
    }


    inline double TauRecDetails :: ipSigLeadTrack() const
    {
        return m_ipSigLeadTrack;
    }

    inline void TauRecDetails :: setIpSigLeadTrack( double ipSigLeadTrack )
    {
        m_ipSigLeadTrack = ipSigLeadTrack;
    }

    inline double TauRecDetails :: etOverPtLeadTrack() const
    {
        return m_etOverPtLeadTrack;
    }

    inline void TauRecDetails :: setEtOverPtLeadTrack( double etOverPtLeadTrack )
    {
        m_etOverPtLeadTrack = etOverPtLeadTrack;
    }


    inline int TauRecDetails :: chargeLooseTracks(  ) const 
    {
        return m_chargeLooseTracks;
    }

    inline void TauRecDetails :: setChargeLooseTracks( 
            const int chargeLooseTracks )
    {
        m_chargeLooseTracks = chargeLooseTracks;
    }

    inline int TauRecDetails :: nTracksdrdR() const
    {
        return m_nTracksdrdR;
    }

    inline void TauRecDetails :: setNTracksdrdR( int nTracksdrdR )
    {
        m_nTracksdrdR = nTracksdrdR;
    }



    //-----------------------------------------------------------------
    // Implementation of accessors - energy part
    //-----------------------------------------------------------------
    inline const CLHEP::HepLorentzVector &TauRecDetails :: sumEM() const
    {
        return m_sumEM;
    }

    inline void TauRecDetails :: setSumEM( const CLHEP::HepLorentzVector &sumEM )
    {
        m_sumEM = sumEM;
    }

    inline double TauRecDetails :: etEMCalib() const
    {
        return m_etEMCalib;
    }

    inline void TauRecDetails :: setETEMCalib( double etEMCalib )
    {
        m_etEMCalib = etEMCalib;
    }

    inline double TauRecDetails :: etHadCalib() const
    {
        return m_etHadCalib;
    }

    inline void TauRecDetails :: setETHadCalib( double etHadCalib )
    {
        m_etHadCalib = etHadCalib;
    }

    //-------------------------------------------------------------------------
    // Secondary vertex
    //-------------------------------------------------------------------------
    inline const Trk :: RecVertex *TauRecDetails :: secVertex() const
    {
        return m_secVertex;
    }

    inline void TauRecDetails :: setSecVertex( const Trk :: RecVertex *secVertex )
    {
        delete m_secVertex;
        m_secVertex = new Trk :: RecVertex( *secVertex );
    }

    inline void TauRecDetails :: setNumTrack( unsigned n )
    {
        m_trackCaloEta.resize( n );
        m_trackCaloPhi.resize( n );
    }

    inline unsigned TauRecDetails :: numTrack() const
    {
        return m_trackCaloPhi.size();
    }

    inline double TauRecDetails :: trackCaloEta( int i ) const
    {
        return m_trackCaloEta[i];
    }

    inline void TauRecDetails :: setTrackCaloEta( int i, double trackCaloEta )
    {
        m_trackCaloEta[i] = trackCaloEta;
    }

    inline double TauRecDetails :: trackCaloPhi( int i ) const
    {
        return m_trackCaloPhi[i];
    }

    inline void TauRecDetails :: setTrackCaloPhi( int i, double trackCaloPhi )
    {
        m_trackCaloPhi[i] = trackCaloPhi;
    }

    inline double TauRecDetails :: leadingTrackPT() const
    {
        return m_leadingTrackPT;
    }

    inline void TauRecDetails :: setLeadingTrackPT( double leadingTrackPT )
    {
        m_leadingTrackPT = leadingTrackPT;
    }

    //--------------------------------------------------------------------------
    // Transverse flight path significance
    //--------------------------------------------------------------------------
    inline double TauRecDetails :: trFlightPathSig() const
    {
        return m_trFlightPathSig;
    }

    inline void TauRecDetails :: setTrFlightPathSig( double trFlightPathSig )
    {
        m_trFlightPathSig = trFlightPathSig;
    }

    inline double TauRecDetails::etaCalo() const
    {
        return m_etaCalo;
    }

    inline void TauRecDetails::setEtaCalo( double eta )
    {
        m_etaCalo = eta;
    }

    inline double TauRecDetails::phiCalo() const
    {
        return m_phiCalo;
    }

    inline void TauRecDetails::setPhiCalo( double phi )
    {
        m_phiCalo = phi;
    }

    //-------------------------------------------------------------------------
    // Implementation of tracking info accessors
    //-------------------------------------------------------------------------
    inline const Rec :: TrackParticle *TauRecDetails :: looseTrack( unsigned i ) const
    {
        if( m_looseTracks.size() <= i )
            return 0;
        else if( m_looseTracks[i].isValid() )
            return *(m_looseTracks[i]);
        else
            return 0;
    }

    inline void TauRecDetails :: addLooseTrack( const Rec :: TrackParticleContainer *cont,
            const Rec :: TrackParticle          *track )
    {
        ElementLink<Rec::TrackParticleContainer> link;
        link.toContainedElement( *cont,  track ) ;
        m_looseTracks.push_back( link ) ;
    }

    inline unsigned TauRecDetails :: numLooseTrack() const
    {
        return m_looseTracks.size();
    }

    inline
    ElementLinkVector<Rec::TrackParticleContainer>& TauRecDetails::looseTracks()
    {
      return m_looseTracks;
    }
    
    inline
    const ElementLinkVector<Rec::TrackParticleContainer>& TauRecDetails::looseTracks() const
    {
      return m_looseTracks;
    }
}

#endif // TAU_REC_DETAILS_ANALYSIS_H
