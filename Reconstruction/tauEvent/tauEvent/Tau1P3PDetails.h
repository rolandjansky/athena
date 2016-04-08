/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file Tau1P3PDetails.h DEPRECATED DO NOT USE 
/// - MIGRATE TO TauCommonDetails 
///
/// Declaration of details class for Tau1P3P
///
/// Package: Reconstruction/TauEvent
///
///@author Thomas Burgess  <thomas.burgess[]cern.ch>
///@author Lukasz Janyst
///@author Michael Heldmann
///@author Elzbieta Richter-Was
///
/// Created 2007-01-17
///
/// $Id: Tau1P3PDetails.h,v 1.15 2009-01-17 01:34:16 tburgess Exp $

#ifndef TAU1P3P_DETAILS_ANALYSIS_H
#define TAU1P3P_DETAILS_ANALYSIS_H

#include "tauEvent/TauDetails.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "VxVertex/RecVertex.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
#include "CaloEvent/CaloClusterContainer.h"

#include <vector>

namespace Analysis
{
    //-------------------------------------------------------------------------
    /// DEPRECATED DO NOT USE - MIGRATE TO TauCommonDetails
    //! Basic details class containing information produced
    //! by tau1P3P algorithm, goes to both AOD and ESD
    //-------------------------------------------------------------------------
    class Tau1P3PDetails: public TauDetails
    {
        friend class TauJet;
        friend class Tau1P3PDetailsCnv_p1;
        public:

        //-----------------------------------------------------------------
        // Constructor and destructor
        //-----------------------------------------------------------------

        //! Default Constructor
        Tau1P3PDetails();
        //! Destructor
        virtual ~Tau1P3PDetails();
        //! Constructor
        Tau1P3PDetails( const Tau1P3PDetails &rhs );
        //! Assignment Operator
        Tau1P3PDetails &operator = ( const Tau1P3PDetails &rhs );
        virtual const std::string& className() const;

        //-----------------------------------------------------------------
        //! Generic parameter accessors - for old AOD compatibility
        //-----------------------------------------------------------------
        virtual double parameter( TauJetParameters :: Detail detail ) const;
        virtual bool hasParameter( TauJetParameters :: Detail detail ) const;

        //-----------------------------------------------------------------
        //! cells used for candidate building, only EM calorimeter, without 3rd sampling
        //-----------------------------------------------------------------
        const CaloCluster *cellEM012Cluster() const;
        void setCellEM012Cluster( const CaloClusterContainer *cont, const CaloCluster *clust );

        //-----------------------------------------------------------------
        //! @c ElementLinks to CaloClusters
        //-----------------------------------------------------------------
        const ElementLink<CaloClusterContainer> cellEM012ClusterLink() const;
        ElementLink<CaloClusterContainer> &cellEM012ClusterLink();
        void setCellEM012ClusterLink( const ElementLink<CaloClusterContainer> &x );

        //-----------------------------------------------------------------
        //! Uncalibrated transverse energy weighted radius in the
        //! Presampler + EM1 + EM2 within dR < 0.2
        //-----------------------------------------------------------------
        double emRadius() const;
        void setEMRadius( double EMRadius );

        //-----------------------------------------------------------------
        //! EM part of the energy
        //-----------------------------------------------------------------
        const CLHEP::HepLorentzVector &sumEM() const;
        void setSumEM( const CLHEP::HepLorentzVector &sumEM );

        //-----------------------------------------------------------------
        //! Ratio of the uncalibrated transverse energy within dR < 0.1 and
        //! dR < 0.2
        //-----------------------------------------------------------------
        double isolationFraction() const;
        void setIsolationFraction( double isolationFraction );

        //-----------------------------------------------------------------
        //! Uncalibrated transverse energy weighted width in the strip
        //! layer within dR < 0.2
        //-----------------------------------------------------------------
        double stripWidth2() const;
        void setStripWidth2( double stripWidth2 );

        //-----------------------------------------------------------------
        //! Number of Strip cells within dR < 0.2, with E > m_stripEthr
        //-----------------------------------------------------------------
        int numStripCells() const;
        void setNumStripCells( int numStripCells );

        //-----------------------------------------------------------------
        //! Charged transverse energy in narrow window around track(s) in
        //! HAD calo
        //-----------------------------------------------------------------
        double etChrgHAD() const;
        void setETChrgHAD( double etChrgHAD );

        //-----------------------------------------------------------------
        //! Transverse energy in EM calo (at EM Scale) in  0.2 < dR < 0.4
        //-----------------------------------------------------------------
        double etIsolEM() const;
        void setETIsolEM( double etIsolEM );

        //-----------------------------------------------------------------
        //! Transverse energy in HAD calo (at EM Scale) in  0.2 < dR < 0.4
        //-----------------------------------------------------------------
        double etIsolHAD() const;
        void setETIsolHAD( double etIsolHad );

        //-----------------------------------------------------------------
        //! Associated, "not good" quality tracks in core region
        //-----------------------------------------------------------------
        int nAssocTracksCore() const;
        void setNAssocTracksCore( int nAssocTracksCore );

        //-----------------------------------------------------------------
        //! Associated tracks in isolation region
        //-----------------------------------------------------------------
        int nAssocTracksIsol() const;
        void setNAssocTracksIsol( int nAssocTracksIsol );

        //-----------------------------------------------------------------
        //! Invariant mass of the tracks system
        //-----------------------------------------------------------------
        double massTrk3P() const;
        void setMassTrk3P( double massTrk3P );

        //-----------------------------------------------------------------
        //! Width of tracks momenta
        //-----------------------------------------------------------------
        double rWidth2Trk3P() const;
        void setRWidth2Trk3P( double rWidth2Trk3P );

        //-----------------------------------------------------------------
        //! signed transverse impact parameter
        //-----------------------------------------------------------------
        double signD0Trk3P() const;
        void setSignD0Trk3P( double signD0Trk3P );

        //-----------------------------------------------------------------
        //! Significance of z0SinTheta
        //-----------------------------------------------------------------
        double z0SinThetaSig() const;
        void setZ0SinThetaSig( double z0SinThetaSig );

        //-----------------------------------------------------------------
        //! charged hadronic transverse energy over sum of transverse momenta of all tracks
        //-----------------------------------------------------------------
        double etChrgHADoverPttot() const;
        void setEtChrgHADoverPttot( double etChrgHADoverPttot );

        //-----------------------------------------------------------------
        //! Ratio of Transverse Energy in 0.2 < dr < 0.4 to total transverse energy at EM scale
        //-----------------------------------------------------------------
        double etIsolFrac() const;
        void setEtIsolFrac( double etIsolFrac );

        //-----------------------------------------------------------------
        //! ET from energy flow
        //-----------------------------------------------------------------
        double etEflow() const;
        void setETeflow( double etEflow );

        //-----------------------------------------------------------------
        //! Had energy at the EM scale
        //-----------------------------------------------------------------
        double etHadAtEMScale() const;
        void setETHadAtEMScale( double etHadAtEMScale );

        //-----------------------------------------------------------------
        //! EM energy at the EM scale
        //-----------------------------------------------------------------
        double etEMAtEMScale() const;
        void setETEMAtEMScale( double etEMAtEMScale );

        //-----------------------------------------------------------------
        //! Transverse energy of cells classified as "pure electromagnetic"
        //! seeded by egamma or topo cluster (steered from jobOptions), 
        //-----------------------------------------------------------------
        double etEMCL() const;
        void setETEMCL( double etEMCL );

        //-----------------------------------------------------------------
        //! Transverse energy of EM cells (at EM scale) classified as
        //! "charged electromagnetic" collected in narrow window around
        //! qualified track 
        //-----------------------------------------------------------------
        double etChrgEM() const;
        void setETChrgEM( double etChrgEM );

        //-----------------------------------------------------------------
        //! Transverse energy of EM cells (at EM scale), within  "core"
        //! cone around tau1P3P axis after subtraction of EMCL and Chrg
        //! cells
        //-----------------------------------------------------------------
        double etNeuEM() const;
        void setETNeuEM( double etNeuEM );

        //-----------------------------------------------------------------
        //! Correction term for eflow calculations
        //-----------------------------------------------------------------
        double etResNeuEM() const;
        void setETResNeuEM( double etResNeuEM );

        //-----------------------------------------------------------------
        //! Transverse energy in narrow window around the track(s), EM
        //! layers 0 1 only
        //-----------------------------------------------------------------
        double etChrgEM01Trk( unsigned tr ) const;
        void setETChrgEM01Trk( unsigned tr, double etChrgEM01Trk );
        void resizeETChrgEM01Trk( size_t sz );

        //! add transverse energy of track for EM layers 0 1 only
        void addETChrgEM01Trk( double etChrgEM01Trk );

        //-----------------------------------------------------------------
        //! Correction term for eflow calculations (evaluated for each
        //! track separately)
        //-----------------------------------------------------------------
        double etResChrgEMTrk( unsigned tr ) const;
        void setETResChrgEMTrk( unsigned tr, double etResChrgEMTrk );
        void resizeETResChrgEMTrk( size_t sz );

        //! add transverse energy of track (for eflow correction) to vector
        void addETResChrgEMTrk( double etResChrgEMTrk );

        //-----------------------------------------------------------------
        //! Secondary vertex reconstructed with Vtx::FastFit tool for taus
        //! with at least 2 associated tracks
        //-----------------------------------------------------------------
        void setSecVertex( const Trk :: RecVertex *secVertex );
        const Trk :: RecVertex *secVertex() const;

        //-----------------------------------------------------------------
        //! number of pi0 clusters
        //-----------------------------------------------------------------
        unsigned numPi0() const;

        //-----------------------------------------------------------------
        //! access pi0 clusters
        //-----------------------------------------------------------------
        const CaloCluster *pi0( unsigned num ) const;

        //-----------------------------------------------------------------
        //! add pi0 cluster to the cluster container
        //-----------------------------------------------------------------
        void addPi0( const CaloCluster *pi0, const CaloClusterContainer *pi0a );

        //-----------------------------------------------------------------
        //! Get Pi0 Link
        //-----------------------------------------------------------------
        ElementLinkVector<CaloClusterContainer>& pi0LinkVector();

        //-----------------------------------------------------------------
        //! Transverse flight path significance for taus with at least
        //! 2 associated tracks
        //-----------------------------------------------------------------
        double trFlightPathSig() const;
        void setTrFlightPathSig( double trFlightPathSig );

        //-----------------------------------------------------------------
        //! Ratio of sum Et of LAr Cells to the pT of leading track
        //-----------------------------------------------------------------
        double sumEtCellsLArOverLeadTrackPt() const;
        void setSumEtCellsLArOverLeadTrackPt(double SumEtCellsLArOverLeadTrackPt);

        //-----------------------------------------------------------------
        //! hadronic leakage in Et summed over cells
        //-----------------------------------------------------------------
        double hadronicLeak() const;
        void setHadronicLeak(double HadronicLeak);

        //-----------------------------------------------------------------
        //! secondary maximum
        //-----------------------------------------------------------------
        double secondaryMax() const;
        void setSecondaryMax(double SecondaryMax);



        private:
        //-----------------------------------------------------------------
        // Identification variables variables
        //-----------------------------------------------------------------
        static std::string s_className;
        int    m_numStripCells;
        double m_stripWidth2;
        double m_emRadius;
        double m_ET12Frac;
        double m_etIsolHAD;
        double m_etIsolEM;
        double m_etChrgHAD;
        int    m_nAssocTracksCore;
        int    m_nAssocTracksIsol;
        double m_signD0Trk3P;
        double m_massTrk3P;
        double m_rWidth2Trk3P;
        double m_z0SinThetaSig;
        double m_etChrgHADoverPttot;
        double m_etIsolFrac;
        ElementLink<CaloClusterContainer>  m_cellEM012Cluster;

        //-----------------------------------------------------------------
        // Energy at the EM scale
        //-----------------------------------------------------------------
        double m_etHadAtEMScale;
        double m_etEMAtEMScale;

        double m_etEMCL;
        double m_etChrgEM;
        double m_etNeuEM;
        double m_etResNeuEM;

        double m_trFlightPathSig;

        std :: vector<double> m_etChrgEM01Trk;
        std :: vector<double> m_etResChrgEMTrk;
        CLHEP::HepLorentzVector      m_sumEM;

        //-----------------------------------------------------------------
        // Secondary Vertex
        //-----------------------------------------------------------------
        Trk :: RecVertex *m_secVertex;

        //-----------------------------------------------------------------
        // Pi0
        //-----------------------------------------------------------------
        ElementLinkVector<CaloClusterContainer> m_pi0;

        double m_etEflow;

        double m_sumEtCellsLArOverLeadTrackPt;
        double m_hadronicLeak;
        double m_secondaryMax;



    };

    //-------------------------------------------------------------------------
    // Implementation of accessors
    //-------------------------------------------------------------------------
    inline double Tau1P3PDetails :: emRadius() const
    {
        return m_emRadius;
    }

    inline void Tau1P3PDetails :: setEMRadius( double emRadius )
    {
        m_emRadius = emRadius;
    }

    inline double Tau1P3PDetails :: isolationFraction() const
    {
        return m_ET12Frac;
    }

    inline void Tau1P3PDetails :: setIsolationFraction( double isolationFraction )
    {
        m_ET12Frac = isolationFraction;
    }

    inline const CLHEP::HepLorentzVector &Tau1P3PDetails :: sumEM() const
    {
        return m_sumEM;
    }

    inline void Tau1P3PDetails :: setSumEM( const CLHEP::HepLorentzVector &sumEM )
    {
        m_sumEM = sumEM;
    }

    inline double Tau1P3PDetails :: stripWidth2() const
    {
        return m_stripWidth2;
    }

    inline void Tau1P3PDetails :: setStripWidth2( double stripWidth2 )
    {
        m_stripWidth2 = stripWidth2;
    }

    inline int Tau1P3PDetails :: numStripCells() const
    {
        return m_numStripCells;
    }

    inline void Tau1P3PDetails :: setNumStripCells( int numStripCells )
    {
        m_numStripCells = numStripCells;
    }

    inline double Tau1P3PDetails :: etChrgHAD() const
    {
        return m_etChrgHAD;
    }

    inline void Tau1P3PDetails :: setETChrgHAD( double etChrgHAD )
    {
        m_etChrgHAD = etChrgHAD;
    }

    inline double Tau1P3PDetails :: etIsolEM() const
    {
        return m_etIsolEM;
    }

    inline void Tau1P3PDetails :: setETIsolEM( double etIsolEM )
    {
        m_etIsolEM = etIsolEM;
    }

    inline double Tau1P3PDetails :: etIsolHAD() const
    {
        return m_etIsolHAD;
    }

    inline void Tau1P3PDetails :: setETIsolHAD( double etIsolHAD )
    {
        m_etIsolHAD = etIsolHAD;
    }

    inline int Tau1P3PDetails :: nAssocTracksCore() const
    {
        return m_nAssocTracksCore;
    }

    inline void Tau1P3PDetails :: setNAssocTracksCore( int nAssocTracksCore )
    {
        m_nAssocTracksCore = nAssocTracksCore;
    }

    inline int Tau1P3PDetails :: nAssocTracksIsol() const
    {
        return m_nAssocTracksIsol;
    }

    inline void Tau1P3PDetails :: setNAssocTracksIsol( int nAssocTracksIsol )
    {
        m_nAssocTracksIsol = nAssocTracksIsol;
    }

    inline double Tau1P3PDetails :: massTrk3P() const
    {
        return m_massTrk3P;
    }

    inline void Tau1P3PDetails :: setMassTrk3P( double massTrk3P )
    {
        m_massTrk3P = massTrk3P;
    }

    inline double Tau1P3PDetails :: rWidth2Trk3P() const
    {
        return m_rWidth2Trk3P;
    }

    inline void Tau1P3PDetails :: setRWidth2Trk3P( double rWidth2Trk3P )
    {
        m_rWidth2Trk3P = rWidth2Trk3P;
    }

    inline double Tau1P3PDetails :: signD0Trk3P() const
    {
        return m_signD0Trk3P;
    }

    inline void Tau1P3PDetails :: setSignD0Trk3P( double signD0Trk3P )
    {
        m_signD0Trk3P = signD0Trk3P;
    }

    inline double Tau1P3PDetails :: z0SinThetaSig() const
    {
        return m_z0SinThetaSig;
    }

    inline void Tau1P3PDetails :: setZ0SinThetaSig( double z0SinThetaSig )
    {
        m_z0SinThetaSig = z0SinThetaSig;
    }

    inline double Tau1P3PDetails :: etChrgHADoverPttot() const
    {
        return m_etChrgHADoverPttot;
    }

    inline void Tau1P3PDetails :: setEtChrgHADoverPttot( double etChrgHADoverPttot )
    {
        m_etChrgHADoverPttot = etChrgHADoverPttot ;
    }

    inline double Tau1P3PDetails :: etIsolFrac() const
    {
        return m_etIsolFrac;
    }

    inline void Tau1P3PDetails :: setEtIsolFrac( double etIsolFrac )
    {
        m_etIsolFrac = etIsolFrac;
    }

    inline double Tau1P3PDetails :: etEflow() const
    {
        return m_etEflow;
    }

    inline void Tau1P3PDetails :: setETeflow( double etEflow )
    {
        m_etEflow = etEflow;
    }

    inline double Tau1P3PDetails :: etHadAtEMScale() const
    {
        return m_etHadAtEMScale;
    }

    inline void Tau1P3PDetails :: setETHadAtEMScale( double etHadAtEMScale )
    {
        m_etHadAtEMScale = etHadAtEMScale;
    }

    inline double Tau1P3PDetails :: etEMAtEMScale() const
    {
        return m_etEMAtEMScale;
    }

    inline void Tau1P3PDetails :: setETEMAtEMScale( double etEMAtEMScale )
    {
        m_etEMAtEMScale = etEMAtEMScale;
    }

    inline double Tau1P3PDetails :: etEMCL() const
    {
        return m_etEMCL;
    }

    inline void Tau1P3PDetails :: setETEMCL( double etEMCL )
    {
        m_etEMCL = etEMCL;
    }

    inline double Tau1P3PDetails :: etChrgEM() const
    {
        return m_etChrgEM;
    }

    inline void Tau1P3PDetails :: setETChrgEM( double etChrgEM )
    {
        m_etChrgEM = etChrgEM;
    }

    inline double Tau1P3PDetails :: etNeuEM() const
    {
        return m_etNeuEM;
    }

    inline void Tau1P3PDetails :: setETNeuEM( double etNeuEM )
    {
        m_etNeuEM = etNeuEM;
    }

    inline double Tau1P3PDetails :: etResNeuEM() const
    {
        return m_etResNeuEM;
    }

    inline void Tau1P3PDetails :: setETResNeuEM( double etResNeuEM )
    {
        m_etResNeuEM = etResNeuEM;
    }

    inline double Tau1P3PDetails :: etChrgEM01Trk( unsigned tr ) const
    {
        //ak if( m_etChrgEM01Trk.size() < tr )
        if( m_etChrgEM01Trk.size() > tr )
            return m_etChrgEM01Trk[tr];
        //return -11111.;
        return 0.;
    }

    inline void Tau1P3PDetails :: setETChrgEM01Trk( unsigned tr, double etChrgEM01Trk )
    {
        if( m_etChrgEM01Trk.size() < tr )
            m_etChrgEM01Trk[tr] = etChrgEM01Trk;
    }

    inline void Tau1P3PDetails :: resizeETChrgEM01Trk( size_t sz )
    {
      m_etChrgEM01Trk.resize (sz);
    }

    inline void Tau1P3PDetails :: addETChrgEM01Trk( double etChrgEM01Trk )
    {
        m_etChrgEM01Trk.push_back( etChrgEM01Trk );
    }

    inline double Tau1P3PDetails :: etResChrgEMTrk( unsigned tr ) const
    {
        if( m_etResChrgEMTrk.size() > tr )
            return m_etResChrgEMTrk[tr];
        return 0.;
    }

    inline void Tau1P3PDetails :: setETResChrgEMTrk( unsigned tr, double etResChrgEMTrk )
    {
        if( m_etResChrgEMTrk.size() < tr )
            m_etResChrgEMTrk[tr] = etResChrgEMTrk;
    }

    inline void Tau1P3PDetails :: resizeETResChrgEMTrk( size_t sz )
    {
      m_etResChrgEMTrk.resize (sz);
    }

    inline void Tau1P3PDetails :: addETResChrgEMTrk( double etResChrgEMTrk )
    {
        m_etResChrgEMTrk.push_back( etResChrgEMTrk );
    }

    inline const Trk :: RecVertex *Tau1P3PDetails :: secVertex() const
    {
        return m_secVertex;
    }

    inline void Tau1P3PDetails :: setSecVertex( const Trk :: RecVertex *secVertex )
    {
        delete m_secVertex;
        m_secVertex = new Trk :: RecVertex( *secVertex );
    }

    //-------------------------------------------------------------------------
    // Pi0
    //-------------------------------------------------------------------------
    inline unsigned Tau1P3PDetails :: numPi0() const
    {
        return  m_pi0.size();
    }

    inline const CaloCluster *Tau1P3PDetails :: pi0( unsigned num ) const
    {
        return *m_pi0[num];
    }

    inline ElementLinkVector<CaloClusterContainer>& Tau1P3PDetails :: pi0LinkVector()
    {
        return m_pi0;
    }

    inline void Tau1P3PDetails :: addPi0( const CaloCluster *pi0,
            const CaloClusterContainer *cont )
    {
        ElementLink<CaloClusterContainer> link;
        if( !cont )
            link.setElement( pi0 );
        else
            link.toContainedElement( *cont,  pi0 );
        m_pi0.push_back( link );
    }


    //-------------------------------------------------------------------------
    // Implementation of calorimetry info accessors
    //-------------------------------------------------------------------------
    inline const CaloCluster *Tau1P3PDetails :: cellEM012Cluster() const
    {
        return ( (m_cellEM012Cluster.isValid()) ? (*m_cellEM012Cluster) : 0 );
    }

    inline void Tau1P3PDetails :: setCellEM012Cluster( const CaloClusterContainer *cont,
            const CaloCluster *clust )
    {
        if( m_cellEM012Cluster.isValid() )
            m_cellEM012Cluster.reset();
        m_cellEM012Cluster.toContainedElement( *cont, clust);
    }

    inline const ElementLink<CaloClusterContainer> Tau1P3PDetails :: cellEM012ClusterLink() const
    {
        return m_cellEM012Cluster;
    }

    inline ElementLink<CaloClusterContainer> &Tau1P3PDetails :: cellEM012ClusterLink()
    {
        return m_cellEM012Cluster;
    }

    inline void Tau1P3PDetails :: setCellEM012ClusterLink( const ElementLink<CaloClusterContainer> &x)
    {
        m_cellEM012Cluster = x;
    }

    //-------------------------------------------------------------------------
    // Transverse flight path significance
    //-------------------------------------------------------------------------
    inline double Tau1P3PDetails :: trFlightPathSig() const
    {
        return m_trFlightPathSig;
    }

    inline void Tau1P3PDetails :: setTrFlightPathSig( double trFlightPathSig )
    {
        m_trFlightPathSig = trFlightPathSig;
    }


    inline void Tau1P3PDetails :: setSumEtCellsLArOverLeadTrackPt( double SumEtCellsLArOverLeadTrackPt )
    {
        m_sumEtCellsLArOverLeadTrackPt = SumEtCellsLArOverLeadTrackPt;
    }

    inline double Tau1P3PDetails :: sumEtCellsLArOverLeadTrackPt() const
    {
        return m_sumEtCellsLArOverLeadTrackPt;
    }

    inline void Tau1P3PDetails :: setHadronicLeak( double HadronicLeak )
    {
        m_hadronicLeak = HadronicLeak;
    }

    inline double Tau1P3PDetails :: hadronicLeak() const
    {
        return m_hadronicLeak;
    }

    inline void Tau1P3PDetails :: setSecondaryMax( double SecondaryMax )
    {
        m_secondaryMax = SecondaryMax;
    }

    inline double Tau1P3PDetails :: secondaryMax() const
    {
        return m_secondaryMax;
    }




}

#endif // TAU1P3P_DETAILS_ANALYSIS_H
