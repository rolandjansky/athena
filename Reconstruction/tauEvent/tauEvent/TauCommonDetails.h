/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/// @file TauCommonDetails.h
///
/// Declaration of details class for tau event
///
/// Package: Reconstruction/TauEvent
///
/// @author Noel Dawe, Thomas Burgess
///
/// Created by Thomas Burgess on 2008-10-20.

#ifndef TAUEVENT_TAUCOMMONDETAILS_H
#define TAUEVENT_TAUCOMMONDETAILS_H

//TauEvent includes
#include "TauDetails.h"
//Other inlcudes
#include "CLHEP/Vector/LorentzVector.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "DataModel/ElementLink.h"
#include "DataModel/ElementLinkVector.h"
#include "Particle/TrackParticleContainer.h"
//Standard C/C++ includes
#include <string>
//Predclarations

namespace Analysis {

    /// Basic detail class containing information for track and calo
    /// seeded tau reconstruction algorithms.
    ///
    /// This class replaces Tau1P3PDetails and TauRecDetails
    ///
    /// Is saved both to ESD and AOD output.
    class TauCommonDetails : public TauDetails {
        friend class TauCommonDetailsCnv_p1;
        friend class TauJet;
        public:
        //-----------------------------------------------------------------
        // Constructors, destructor, operators
        //-----------------------------------------------------------------

        /// Default constructor
        TauCommonDetails();

        /// Copy constructor
        /// @param rhs TauCommonDetails instance to assign from
        TauCommonDetails(
                const TauCommonDetails& rhs );

        /// Virtual destructor
        virtual ~TauCommonDetails();

        /// Assignment operator
        /// @param rhs TauCommonDetails instance to assign from
        /// @return assigned object
        TauCommonDetails& operator = (
                const TauCommonDetails& rhs );

        /// Equality operator
        /// @param rhs right hand side TauCommonDetails instance to compare 
        /// @return true if equal, false otherwise
        bool operator==( const TauCommonDetails& rhs ) const;

        /// Inequality operator
        /// @param rhs TauCommonDetails instance to compare to
        /// @return !operator==
        bool operator!=( const TauCommonDetails& rhs ) const;

        /// Get name of class
        /// @return name of class
        virtual const std::string& className() const;

        ///////////////////////////////////////////////////////////////////
        // Private member accessors
        ///////////////////////////////////////////////////////////////////

        //
        // Common for track and calo seeded tau reconstruction
        //

        //  ---------------------------------------------------------------
        /// @name Accessors for ipZ0SinThetaSigLeadTrk
        /// @copydoc Analysis::TauCommonDetails::m_ipZ0SinThetaSigLeadTrk
        //@{ --------------------------------------------------------------

        /// Get significance of z0SinTheta
        /// @return significance
        double ipZ0SinThetaSigLeadTrk() const;

        /// Set significance of z0SinTheta
        /// @param sig significance
        void setIpZ0SinThetaSigLeadTrk(
                double significance);

        //@} --------------------------------------------------------------
        /// @name Accessors for etOverPtLeadTrk
        /// @copydoc Analysis::TauCommonDetails::m_etOverPtLeadTrk
        //@{ --------------------------------------------------------------

        /// Get leading E_T to p_T ratio
        /// @return leading E_T to p_T ratio
        double etOverPtLeadTrk() const;

        /// Set leading E_T to p_T ratio
        /// @param ratio E_T to p_T ratio
        void setEtOverPtLeadTrk(
                double ratio );

        //@} --------------------------------------------------------------
        /// @name Accessors for leadTrkPt
        /// @copydoc Analysis::TauCommonDetails::m_leadTrkPt
        //@{ --------------------------------------------------------------

        /// Get leading track p_T
        /// @return leading track p_T
        double leadTrkPt() const;

        /// Set leading track p_T
        /// @param pt leading track p_T
        void setLeadTrkPt(
                double pt );

        //@} --------------------------------------------------------------
        /// @name Accessors for looseTrk
        /// @copydoc Analysis::TauCommonDetails::m_looseTrk
        //@{ --------------------------------------------------------------

        /// Get loose track
        ///
        /// Note: concider using direct acces to vector using looseTrk()
        ///
        /// @param i track index
        /// @return loose track
        const Rec::TrackParticle *looseTrk(
                unsigned int i) const;

        /// Get loose track vector (const)
        /// @return (const) loose track vector
        const ElementLinkVector<Rec::TrackParticleContainer>& looseTrk() const;

        /// Get loose track vector
        /// @return loose track vector
        ElementLinkVector<Rec::TrackParticleContainer>& looseTrk();

        /// Add a loose track
        /// @param cont Track container
        /// @param track Track to add
        void addLooseTrk(
                const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle *track );

        /// Get number of loose tracks
        /// @return number of loose track
        unsigned int nLooseTrk() const;


        //@} --------------------------------------------------------------
        /// @name Accessors for looseConvTrk
        /// @copydoc Analysis::TauCommonDetails::m_looseConvTrk
        //@{ --------------------------------------------------------------

        /// Get loose track
        ///
        /// Note: consider using direct acces to vector using looseConvTrk()
        ///
        /// @param i track index
        /// @return looseConv track
        const Rec::TrackParticle *looseConvTrk(
                unsigned int i) const;

        /// Get looseConv track vector (const)
        /// @return (const) looseConv track vector
        const ElementLinkVector<Rec::TrackParticleContainer>& looseConvTrk() const;

        /// Get looseConv track vector
        /// @return looseConv track vector
        ElementLinkVector<Rec::TrackParticleContainer>& looseConvTrk();

        /// Add a looseConv track
        /// @param cont Track container
        /// @param track Track to add
        void addLooseConvTrk(
                const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle *track );

        /// Get number of loose tracks
        /// @return number of loose track
        unsigned int nLooseConvTrk() const;

        ///Number of prongs
        ///
        /// number of loose tracks - number of loose conversion tracks
        int nProngLoose() const;

        ///Check if track is a conversion track
        ///@param i Id of track to check
        bool isLooseConvTrk( unsigned int i ) const;


        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_wideTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_wideTrk
        //@{ --------------------------------------------------------------

        /// Get seedCalo wide track
        ///
        /// @param i track index
        /// @return seedCalo wide track
        const Rec::TrackParticle *seedCalo_wideTrk(
                unsigned int i) const;

        /// Get seedCalo wide track vector (const)
        /// @return (const) seedCalo wide track vector
        const ElementLinkVector<Rec::TrackParticleContainer>& seedCalo_wideTrk() const;

        /// Get seedCalo wide track vector
        /// @return seedCalo wide track vector
        ElementLinkVector<Rec::TrackParticleContainer>& seedCalo_wideTrk();

        /// Add a seedCalo wide track
        /// @param cont Track container
        /// @param track Track to add
        void addSeedCalo_wideTrk(
                const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle *track );

        /// Get number of seedCalo wide tracks
        /// @return number of seedCalo wide track
        unsigned int seedCalo_nWideTrk() const;


        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_wideTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_wideTrk
        //@{ --------------------------------------------------------------

        /// Get seedTrk wide track
        ///
        /// @param i track index
        /// @return seedTrk wide track
        const Rec::TrackParticle *seedTrk_wideTrk(
                unsigned int i) const;

        /// Get seedTrk wide track vector (const)
        /// @return (const) seedTrk wide track vector
        const ElementLinkVector<Rec::TrackParticleContainer>& seedTrk_wideTrk() const;

        /// Get seedTrk wide track vector
        /// @return seedTrk wide track vector
        ElementLinkVector<Rec::TrackParticleContainer>& seedTrk_wideTrk();

        /// Add a seedTrk wide track
        /// @param cont Track container
        /// @param track Track to add
        void addSeedTrk_wideTrk(
                const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle *track );

        /// Get number of seedTrk wide tracks
        /// @return number of seedTrk wide track
        unsigned int seedTrk_nWideTrk() const;

        //@} --------------------------------------------------------------
        /// @name Accessors for otherTrk
        /// @copydoc Analysis::TauCommonDetails::m_otherTrk
        //@{ --------------------------------------------------------------

        /// Get other track
        ///
        /// @param i track index
        /// @return other track
        const Rec::TrackParticle *otherTrk(
                unsigned int i) const;

        /// Get other track vector (const)
        /// @return (const) other track vector
        const ElementLinkVector<Rec::TrackParticleContainer>& otherTrk() const;

        /// Get other track vector
        /// @return other track vector
        ElementLinkVector<Rec::TrackParticleContainer>& otherTrk();

        /// Add a other track
        /// @param cont Track container
        /// @param track Track to add
        void addOtherTrk(
                const Rec::TrackParticleContainer *cont,
                const Rec::TrackParticle *track );

        /// Get number of other tracks
        /// @return number of other track
        unsigned int nOtherTrk() const;        



        //@} --------------------------------------------------------------
        /// @name Accessors for ipSigLeadTrk
        /// @copydoc Analysis::TauCommonDetails::m_ipSigLeadTrk
        //@{ --------------------------------------------------------------

        /// Get impact parameter significance
        /// @return significance
        double ipSigLeadTrk() const;

        /// Set impact parameter significance
        /// @param significance Significance
        void setIpSigLeadTrk(
                double significance );

        //@} --------------------------------------------------------------
        /// @name Accessors for ipSigLeadLooseTrk
        /// @copydoc Analysis::TauCommonDetails::m_ipSigLeadLooseTrk
        //@{ --------------------------------------------------------------

        /// Get impact parameter significance
        /// @return significance
        double ipSigLeadLooseTrk() const;

        /// Set impact parameter significance
        /// @param significance Significance
        void setIpSigLeadLooseTrk(
                double significance );

        //@} --------------------------------------------------------------
        /// @name Accessors for etOverPtLeadLooseTrk
        /// @copydoc Analysis::TauCommonDetails::m_etOverPtLeadLooseTrk
        //@{ --------------------------------------------------------------

        /// Get E_t to p_T ratio for rau candidate leading loose track
        /// @return E_t to p_T ratio
        double etOverPtLeadLooseTrk() const;

        /// Set E_t to p_T ratio for rau candidate leading loose track
        /// @param ratio E_t to p_T ratio
        void setEtOverPtLeadLooseTrk(
                double ratio );

        //@} --------------------------------------------------------------
        /// @name Accessors for leadLooseTrkPt
        /// @copydoc Analysis::TauCommonDetails::m_leadLooseTrkPt
        //@{ --------------------------------------------------------------

        /// Get p_T of leading loose track
        /// @return Track p_T
        double leadLooseTrkPt() const;

        /// Set p_T of leading loose track
        /// @param pT Track p_T
        void setLeadLooseTrkPt(
                double pT );

        //@} --------------------------------------------------------------
        /// @name Accessors for chrgLooseTrk
        /// @copydoc Analysis::TauCommonDetails::m_chrgLooseTrk
        //@{ --------------------------------------------------------------

        /// Get chrg of loose tracks
        /// @return Charge
        double chrgLooseTrk() const;

        /// Set chrg of loose tracks
        /// @param charge Charge
        void setChrgLooseTrk(
                double charge );

        //@} --------------------------------------------------------------
        /// @name Accessors for massTrkSys
        /// @copydoc Analysis::TauCommonDetails::m_massTrkSys
        //@{ --------------------------------------------------------------

        /// Get track system invariant mass
        /// @return invariant mass
        double massTrkSys() const;

        /// Set track system invariant mass
        /// @param mass invariant mass
        void setMassTrkSys(
                double mass );

        //@} --------------------------------------------------------------
        /// @name Accessors for trkWidth2
        /// @copydoc Analysis::TauCommonDetails::m_trkWidth2
        //@{ --------------------------------------------------------------

        /// Get track width ^2
        /// @return Track width ^2
        double trkWidth2() const;

        /// Set track width ^2
        /// @param trkWidth2 Track width ^2
        void setTrkWidth2(
                double trkWidth2 );

        //@} --------------------------------------------------------------
        /// @name Accessors for cellEM012Cluster
        /// @copydoc Analysis::TauCommonDetails::m_cellEM012Cluster
        //@{ --------------------------------------------------------------

        /// Get EM calorimeter cell cluster
        /// @return cluster of EM calo cells if valid, 0 otherwise
        const CaloCluster *cellEM012Cluster() const;

        /// Set EM calorimeter cell cluster
        /// @param cont  Container with calo clusters
        /// @param clust Cluster of cells to set
        void setCellEM012Cluster(
                const CaloClusterContainer *cont,
                const CaloCluster *clust );

        /// Get ElementLink to EM calorimeter cell cluster (const)
        /// @return (const) ElementLink to CaloCluster
        const ElementLink<CaloClusterContainer>& cellEM012ClusterLink() const;

        /// Get ElementLink to EM calorimeter cell cluster
        /// @return ElementLink to CaloCluster
        ElementLink<CaloClusterContainer>& cellEM012ClusterLink();

        /// Set ElementLink to EM calorimeter cell cluster
        /// @param x ElementLink to CaloCluster to set
        void setCellEM012ClusterLink(
                const ElementLink<CaloClusterContainer> &x );

        //@} --------------------------------------------------------------
        /// @name Accessors for trFlightPathSig
        /// @copydoc Analysis::TauCommonDetails::m_trFlightPathSig
        //@{ --------------------------------------------------------------

        /// Get Transverse flight path significance
        /// @return signigicance
        double trFlightPathSig() const;

        /// Set Transverse flight path significance
        /// @param significance Significance
        void setTrFlightPathSig(
                double significance );

        //@} --------------------------------------------------------------
        /// @name Accessors for sumPi0Vec
        /// @copydoc Analysis::TauCommonDetails::m_sumPi0Vec
        //@{ --------------------------------------------------------------

        /// Get the Pi0 energy sum vector
        /// @return Pi0 energy sum vector
        const CLHEP::HepLorentzVector &sumPi0Vec() const;

        /// Set the Pi0 energy sum vector
        /// @param sumPi0Vec Pi0 energy sum vector
        void setSumPi0Vec(
                const CLHEP::HepLorentzVector &sumPi0Vec );

        //@} --------------------------------------------------------------
        /// @name Accessors for secVtx
        /// @copydoc Analysis::TauCommonDetails::m_secVtx
        //@{ --------------------------------------------------------------

        /// Get secondary vertex
        /// @return Secondary vertex
        const Trk::RecVertex* secVtx() const;

        /// Set secondary vertex
        /// @param secVtx Secondary vertex
        void setSecVtx(
                const Trk::RecVertex* secVtx );

        //@} --------------------------------------------------------------
        /// @name Accessors for etEflow
        /// @copydoc Analysis::TauCommonDetails::m_etEflow
        //@{ --------------------------------------------------------------

        /// Get E_T from energy flow
        /// @return E_T
        double etEflow() const;

        /// Set E_T from energy flow
        /// @param et E_T
        void setEtEflow(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for mEflow
        /// @copydoc Analysis::TauCommonDetails::m_mEflow
        //@{ --------------------------------------------------------------

        /// Get mass from energy flow
        /// @return m
        double mEflow() const;

        /// Set mass from energy flow
        /// @param m
        void setMEflow(
                double m );

        //@} --------------------------------------------------------------
        /// @name Accessors for pi0
        /// @copydoc Analysis::TauCommonDetails::m_pi0
        //@{ --------------------------------------------------------------

        /// Get pi0 cluster
        ///
        ///Concider using direct access through pi0LinkVec() instead
        ///
        /// @param i index
        /// @return pi0 cluster
        const CaloCluster* pi0(
                unsigned int i ) const;

        /// Get pi0 cluster vector (const)
        /// @return (const) pi0 clusters vector
        const ElementLinkVector<CaloClusterContainer>& pi0LinkVec() const;

        /// Get pi0 cluster vector
        /// @return pi0 clusters vector
        ElementLinkVector<CaloClusterContainer>& pi0LinkVec();

        /// Get length of pi0 cluster vector
        ///
        ///Concider using direct access through pi0LinkVec() instead
        ///
        /// @return length
        unsigned int nPi0() const;

        /// Add cluster to pi0 cluster vector
        ///
        ///Concider using direct access through pi0LinkVec() instead
        ///
        /// @param pi0 cluster
        /// @param cont cluster container
        void addPi0(
                const CaloCluster *pi0,
                const CaloClusterContainer *cont );



        /// name accessors for electron LLH variables

        /// get ratio of energy in window of 3X7 cells over 7X7 cells in second layer of ECAL
        double ele_E237E277() const;

        /// set ratio of energy in window of 3X7 cells over 7X7 cells in second layer of ECAL
        void setEle_E237E277( double value );

        /// get fraction of EM cluster energy in presampler 
        double ele_PresamplerFraction() const;

        /// set fraction of EM cluster energy in presampler 
        void setEle_PresamplerFraction( double value );

        /// get fraction of EM cluster energy in first layer of ECAL 
        double ele_ECALFirstFraction() const;

        /// set fraction of EM cluster energy in first layer of ECAL 
        void setEle_ECALFirstFraction( double value );



        //name accessors for topocluster ID variables

        /// get number of cells associated to tau candidate
        unsigned int numCells() const;

        /// set number of cells associated to tau candidate
        void setNumCells(unsigned int value);

        /// get number of topocluster constituents of jet associated to tau candidate
        unsigned int numTopoClusters() const;

        /// set number of topocluster constituents of jet  associated to tau candidate
        void setNumTopoClusters(unsigned int value);

        /// get number of effective topocluster constituents of jet associated to tau candidate !!! is a double
        double numEffTopoClusters() const;

        /// set number of effective topocluster constituents of jet  associated to tau candidate !!! is a double
        void setNumEffTopoClusters(double value);

        /// get invariant mass from topoclusters of jet associated to tau candidate 
        double topoInvMass() const;

        /// set invariant mass from topoclusters of jet associated to tau candidate 
        void setTopoInvMass(double value);

        /// get invariant mass from effective topoclusters of jet associated to tau candidate 
        double effTopoInvMass() const;

        /// set invariant mass from effective topoclusters of jet associated to tau candidate 
        void setEffTopoInvMass(double value);


        /// get mean distance to leading topocluster for topoclusters of jet associated to tau candidate 
        double topoMeanDeltaR() const;

        /// set mean distance to leading topocluster for topoclusters of jet associated to tau candidate 
        void setTopoMeanDeltaR(double value);

        /// get mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
        double effTopoMeanDeltaR() const;

        /// set mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
        void setEffTopoMeanDeltaR(double value);




        /////////////////////////////////////////////////////////////////////
        //
        // Applicable for calo seeded tau reconstruction
        //
        /////////////////////////////////////////////////////////////////////

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_EMRadius
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_EMRadius
        //@{ --------------------------------------------------------------

        /// Get E_T radius
        /// @return E_T radius
        double seedCalo_EMRadius() const;

        /// Set E_T radius
        /// @param EMRadius E_T radius
        void setSeedCalo_EMRadius(
                double EMRadius);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_hadRadius
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_hadRadius
        //@{ --------------------------------------------------------------

        /// Get hadron calorimeter radius
        /// @return Hadron calorimeter radius
        double seedCalo_hadRadius() const;

        /// Set hadron calorimeter radius
        /// @param hadRadius Hadron calorimeter radius
        void setSeedCalo_hadRadius(
                double hadRadius);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_etEMAtEMScale
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_etEMAtEMScale
        //@{ --------------------------------------------------------------

        /// Get EM energy at EM scale
        /// @return energy Energy
        double seedCalo_etEMAtEMScale() const;

        /// Set EM energy at EM scale
        /// @param energy Energy
        void setSeedCalo_etEMAtEMScale(
                double energy);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_etHadAtEMScale
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_etHadAtEMScale
        //@{ --------------------------------------------------------------

        /// Get Hadronic energy at EM scale
        /// @return Energy
        double seedCalo_etHadAtEMScale() const;

        /// Set hadronic energy at EM scale
        /// @param energy Energy
        void setSeedCalo_etHadAtEMScale( double energy );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_isolFrac
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_isolFrac
        //@{ --------------------------------------------------------------

        /// Get isolation fraction
        /// @return Isolation fraction
        double seedCalo_isolFrac() const;

        /// Set isolation fraction
        /// @param isolFrac Isolation fraction
        void setSeedCalo_isolFrac(
                double isolFrac);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_centFrac
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_centFrac
        //@{ --------------------------------------------------------------

        /// Get centrality fraction
        /// @return Centrality fraction
        double seedCalo_centFrac() const;

        /// Set centrality fraction
        /// @param centFrac Centrality fraction
        void setSeedCalo_centFrac(
                double centFrac);

        //@} --------------------------------------------------------------
        /// @name Accessors for  m_seedCalo_StripWidth2
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_stripWidth2
        //@{ --------------------------------------------------------------

        /// Get strip width ^2
        /// @return Strip width ^2
        double seedCalo_stripWidth2() const;

        /// Set strip width ^2
        /// @param stripWidth2 Strip width ^2
        void setSeedCalo_stripWidth2(
                double stripWidth2 );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_nStrip
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_nStrip
        //@{ --------------------------------------------------------------

        /// Get number of strips
        /// @return Number or strops
        int seedCalo_nStrip() const;

        /// Set number of strips
        /// @param nStrip Number of strips
        void setSeedCalo_nStrip(
                int nStrip);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_etEMCalib
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_etEMCalib
        //@{ --------------------------------------------------------------

        /// Get calibrated EM transverse energy
        /// @return Calibrated EM transverse energy
        double seedCalo_etEMCalib() const;

        /// Set calibrated EM transverse energy
        /// @param etEMCalib Calibrated EM transverse energy
        void setSeedCalo_etEMCalib(
                double etEMCalib );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_eta
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_eta
        //@{ --------------------------------------------------------------

        /// Get calibrated hadronic transverse energy
        /// @return Calibrated hadronic transverse energy
        double seedCalo_etHadCalib() const;

        /// Set calibrated hadronic transverse energy
        /// @param etHadCalib Calibrated hadronic transverse energy
        void setSeedCalo_etHadCalib(
                double etHadCalib);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_eta
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_eta
        //@{ --------------------------------------------------------------

        /// Get eta of TauJet calculated from calorimeter
        /// @return eta of TauJet calculated from calorimeter
        double seedCalo_eta() const;

        /// Set eta of TauJet calculated from calorimeter
        /// @param eta eta of TauJet calculated from calorimeter
        void setSeedCalo_eta(
                double eta);

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_phi
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_phi
        //@{ --------------------------------------------------------------

        /// Get phi of TauJet calculated from calorimeter
        /// @return phi of TauJet calculated from calorimeter
        double seedCalo_phi() const;

        /// Set phi of TauJet calculated from calorimeter
        /// @param phi phi of TauJet calculated from calorimeter
        void setSeedCalo_phi(
                double phi );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_nIsolLooseTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_nIsolLooseTrk
        //@{ --------------------------------------------------------------

        /// Get number of isolated tracks
        /// @return Number of isolated tracks
        double seedCalo_nIsolLooseTrk() const;

        /// Set number of isolated tracks
        /// @param nIsolTRk Number of isolated tracks
        void setSeedCalo_nIsolLooseTrk(
                double nIsolTRk );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_trkAvgDist
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_trkAvgDist
        //@{ --------------------------------------------------------------

        /// Get the average track distance to calorimeter seed
        /// @return Average track distance to calorimeter seed
        double seedCalo_trkAvgDist() const;

        /// Set average track distance to calorimeter seed
        /// @param trkAvgDist average track distance to calorimeter seed
        void setSeedCalo_trkAvgDist(
                double trkAvgDist );            

        //@} --------------------------------------------------------------
        /// @name Accessors for seedCalo_trkRmsDist
        /// @copydoc Analysis::TauCommonDetails::m_seedCalo_trkRmsDist
        //@{ --------------------------------------------------------------

        /// Get the RMS of track distance to calorimeter seed
        /// @return RMS of track distance to calorimeter seed
        double seedCalo_trkRmsDist() const;

        /// Set RMS of track distance to calorimeter seed
        /// @param trkRmsDist RMS track distance to calorimeter seed
        void setSeedCalo_trkRmsDist(
                double trkRmsDist );
                
        
        ///  Get sum of 2 leading cluster energy over all cluster energy
        ///  @return sum of 2 leading cluster energy over all cluster energy
        double seedCalo_lead2ClusterEOverAllClusterE() const;

        ///  Set sum of 2 leading cluster energy over all cluster energy
        ///  @param lead2ClusterEOverAllClusterE sum of 2 leading cluster energy over all cluster energy
        void setSeedCalo_lead2ClusterEOverAllClusterE( double lead2ClusterEOverAllClusterE );

        ///  Get sum of 3 leading cluster energy over all cluster energy
        ///  @return sum of 3 leading cluster energy over all cluster energy
        double seedCalo_lead3ClusterEOverAllClusterE() const;

        ///  Set sum of 3 leading cluster energy over all cluster energy
        ///  @param lead3ClusterEOverAllClusterE sum of 3 leading cluster energy over all cluster energy
        void setSeedCalo_lead3ClusterEOverAllClusterE( double lead3ClusterEOverAllClusterE );

        ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
        ///  @return sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
        double seedCalo_caloIso() const;

        ///  Set sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
        ///  @param caloIso sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
        void setSeedCalo_caloIso( double caloIso );

        ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
        ///  @return sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
        double seedCalo_caloIsoCorrected() const;

        ///  Set sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
        ///  @param caloIso sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
        void setSeedCalo_caloIsoCorrected( double caloIsoCorrected );

        ///  Get maximal dR of tracks associated to calo-seeded tau
        ///  @return maximal dR of tracks associated to calo-seeded tau
        double seedCalo_dRmax() const;

        ///  Set maximal dR of tracks associated to calo-seeded tau
        ///  @param dRmax maximal dR of tracks associated to calo-seeded tau
        void setSeedCalo_dRmax( double dRmax );



        /////////////////////////////////////////////////////////////////////
        //
        // Applicable for track seeded tau reconstruction
        //
        /////////////////////////////////////////////////////////////////////

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_EMRadius
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_EMRadius
        //@{ --------------------------------------------------------------

        /// Get EM radius
        /// @return EM Radius
        double seedTrk_EMRadius() const;

        /// Set EM radius
        /// @param EMRadius to set
        void setSeedTrk_EMRadius(
                double EMRadius );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_isolFrac
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_isolFrac
        //@{ --------------------------------------------------------------

        /// Get the isolation fraction
        /// @return Isolation fraction
        double seedTrk_isolFrac() const;

        /// Set the isolation fraction
        /// @param isolFrac Isolation fraction
        void setSeedTrk_isolFrac(
                double isolFrac );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etChrgHadOverSumTrkPt
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgHadOverSumTrkPt
        //@{ --------------------------------------------------------------

        /// Get E_T over p_T
        /// @return E_T over p_T
        double seedTrk_etChrgHadOverSumTrkPt () const;

        /// Set E_T over p_T
        /// @param fraction E_T over p_T
        void setSeedTrk_etChrgHadOverSumTrkPt (
                double fraction );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_isolFracWide
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_isolFracWide
        //@{ --------------------------------------------------------------

        /// Get wide solation fraction
        /// @return wide solation fraction
        double seedTrk_isolFracWide() const;

        /// Set wide solation fraction
        /// @param fraction wide solation fraction
        void setSeedTrk_isolFracWide(
                double fraction );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etHadAtEMScale
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etHadAtEMScale
        //@{ --------------------------------------------------------------

        /// Get hadronic E_T at EM scale
        /// @return E_T
        double seedTrk_etHadAtEMScale() const;

        /// Set hadronic E_T at EM scale
        /// @param et E_T
        void setSeedTrk_etHadAtEMScale (
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etEMAtEMScale
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etEMAtEMScale
        //@{ --------------------------------------------------------------

        /// Get hadronic E_T at EM scale
        /// @return E_T
        double seedTrk_etEMAtEMScale() const;

        /// Set hadronic E_T at EM scale
        /// @param et E_T
        void setSeedTrk_etEMAtEMScale(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etEMCL
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etEMCL
        //@{ --------------------------------------------------------------

        /// Get E_T of pure EM cells
        /// @return E_T
        double seedTrk_etEMCL() const;

        /// Set E_T of pure EM cells
        /// @param et E_T
        void setSeedTrk_etEMCL(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etChrgEM
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgEM
        //@{ --------------------------------------------------------------

        /// Get E_T of charged EM cells
        /// @return E_T
        double seedTrk_etChrgEM() const;

        /// Set E_T of charged EM cells
        /// @param et E_T
        void setSeedTrk_etChrgEM(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etNeuEM
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etNeuEM
        //@{ --------------------------------------------------------------

        /// Get E_T of neutral EM cells
        /// @return E_T
        double seedTrk_etNeuEM() const;

        /// Set E_T of neutral EM cells
        /// @param et E_T
        void setSeedTrk_etNeuEM(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etResNeuEM
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etResNeuEM
        //@{ --------------------------------------------------------------

        /// Get eflow correction term
        /// @return correction term
        double seedTrk_etResNeuEM() const;

        /// Set eflow correction term
        /// @param et correction term
        void setSeedTrk_etResNeuEM(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_hadLeakEt
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_hadLeakEt
        //@{ --------------------------------------------------------------

        /// Get hadronic leakage in Et summed over cells
        /// @return E_T
        double seedTrk_hadLeakEt() const;

        /// Set hadronic leakage in Et summed over cells
        /// @param et E_T
        void setSeedTrk_hadLeakEt(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etChrgEM01Trk
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgEM01Trk
        //@{ --------------------------------------------------------------

        /// Get transverse energy around track
        /// @param i track id
        /// @return E_T
        double seedTrk_etChrgEM01Trk(
                unsigned int i ) const;

        const std::vector<double>& seedTrk_etChrgEM01TrkVector() const;

        /// Set transverse energy around track
        /// @param i track id
        /// @param et E_T
        void setSeedTrk_etChrgEM01Trk(
                unsigned int i,
                double et );

        template <class ITERATOR>
        void setSeedTrk_etChrgEM01Trk(
                ITERATOR beg,
                ITERATOR end);

        /// Add transverse energy around track
        /// @param i track id
        /// @param et correction term
        void addSeedTrk_etChrgEM01Trk(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etResChrgEMTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etResChrgEMTrk
        //@{ --------------------------------------------------------------

        /// Get correction term for eflow calculations per track
        /// @param i track id
        /// @return correction term
        double seedTrk_etResChrgEMTrk(
                unsigned int i ) const;

        const std::vector<double>& seedTrk_etResChrgEMTrkVector() const;

        /// Set correction term for eflow calculations per track
        /// @param i track id
        /// @param et correction term
        void setSeedTrk_etResChrgEMTrk(
                unsigned int i,
                double et );

        template <class ITERATOR>
        void setSeedTrk_etResChrgEMTrk(
                ITERATOR beg,
                ITERATOR end);

        /// Add correction term for eflow calculations per track
        /// @param i track id
        /// @param et correction term
        void addSeedTrk_etResChrgEMTrk(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_sumEMCellEtOverLeadTrkPt
        ///@copydoc
        /// Analysis::TauCommonDetails::m_seedTrk_sumEMCellEtOverLeadTrkPt
        //@{ --------------------------------------------------------------

        /// Get ratio of LAr+EM E_T / leading track p_T
        /// @return ratio LAr+EM E_T / leading track p_T
        double seedTrk_sumEMCellEtOverLeadTrkPt() const;

        /// Set LAr+EM E_T / leading track p_T
        /// @param ratio LAr+EM E_T / leading track p_T
        void setSeedTrk_sumEMCellEtOverLeadTrkPt(
                double ratio );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_secMaxStripEt
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_secMaxStripEt
        //@{ --------------------------------------------------------------

        /// Get hadronic leakage in Et summed over cells
        /// @return E_T
        double seedTrk_secMaxStripEt() const;

        /// Set hadronic leakage in Et summed over cells
        /// @param et E_T
        void setSeedTrk_secMaxStripEt(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_stripWidth2
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_stripWidth2
        //@{ --------------------------------------------------------------

        /// Get the strip width ^2
        /// @return Strip width ^2
        double seedTrk_stripWidth2() const;

        /// Set the strip width ^2
        /// @param stripWidth2 Strip width^2
        void setSeedTrk_stripWidth2(
                double stripWidth2 );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_seedTrk_nStrip
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_nStrip
        //@{ --------------------------------------------------------------

        /// Get the number of strip cells
        /// @return Number of strip cells
        int seedTrk_nStrip() const;

        /// Set the number of strip cells
        /// @param nStrip Number of strip cells
        void setSeedTrk_nStrip(
                int nStrip );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etChrgHad
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etChrgHad
        //@{ --------------------------------------------------------------

        /// Get charged E_T in hadronic calorimeter
        /// @return Charged E_T in hadronic calorimeter
        double seedTrk_etChrgHad() const;

        /// Set charged E_T in hadronic calorimeter
        /// @param et Charged E_T in hadronic calorimeter
        void setSeedTrk_etChrgHad(
                double et );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_nOtherCoreTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_nOtherCoreTrk
        //@{ --------------------------------------------------------------

        /// Get number of other tracks
        /// @return Number of tracks
        int seedTrk_nOtherCoreTrk() const;

        /// Set number of other tracks
        /// @param n Number of tracks
        void setSeedTrk_nOtherCoreTrk(
                int n );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_nIsolTrk
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_nIsolTrk
        //@{ --------------------------------------------------------------

        /// Get number of isolated tracks
        /// @return Number of tracks
        int seedTrk_nIsolTrk() const;

        /// Set number of tracks
        /// @param n Number of isolated tracks
        void setSeedTrk_nIsolTrk(
                int n );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etIsolEM
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etIsolEM
        //@{ --------------------------------------------------------------

        /// Get transverse energy in EM calo
        /// @return Transverse energy in EM calo
        double seedTrk_etIsolEM() const;

        /// Set transverse energy in EM calo
        /// @param etIsolEM Transverse energy in EM calo
        void setSeedTrk_etIsolEM(
                double etIsolEM );

        //@} --------------------------------------------------------------
        /// @name Accessors for seedTrk_etIsolHad
        /// @copydoc Analysis::TauCommonDetails::m_seedTrk_etIsolHad
        //@{ --------------------------------------------------------------

        /// Get transverse energy in Had calo
        /// @return Transverse energy in EM calo
        double seedTrk_etIsolHad() const;

        /// Set transverse energy in EM calo
        /// @param etIsolHad Transverse energy in EM calo
        void setSeedTrk_etIsolHad(
                double etIsolHad );

        //@}

        //////////////////////////////////////////////////////////////////////
        //
        // Energy Scale getter/setters
        //
        //////////////////////////////////////////////////////////////////////


        /// EM+TES final scale
        double EM_TES_scale() const;

        /// set EM+TES final scale
        void set_EM_TES_scale(double value);

        /// LC+TES precalibration
        double LC_TES_precalib() const;

        /// set LC+TES precalibration
        void set_LC_TES_precalib(double value);


	      //@}

        //////////////////////////////////////////////////////////////////////
        //
        // Cell-based energies in rings getters/setters
        //
        //////////////////////////////////////////////////////////////////////
	      /// Ring 1: 0.00  < R < 0.05
	      double cellBasedEnergyRing1() const;
	      void setCellBasedEnergyRing1(double value);
	
	      /// Ring 2: 0.05  < R < 0.075
	      double cellBasedEnergyRing2() const;
	      void setCellBasedEnergyRing2(double value);

	      /// Ring 3: 0.075 < R < 0.10
	      double cellBasedEnergyRing3() const;
	      void setCellBasedEnergyRing3(double value);

	      /// Ring 4: 0.10  < R < 0.125
	      double cellBasedEnergyRing4() const;
	      void setCellBasedEnergyRing4(double value);

	      /// Ring 5: 0.125 < R < 0.15
	      double cellBasedEnergyRing5() const;
	      void setCellBasedEnergyRing5(double value);

	      /// Ring 6: 0.15  < R < 0.2
	      double cellBasedEnergyRing6() const;
	      void setCellBasedEnergyRing6(double value);

	      /// Ring 7: 0.2   < R < 0.4
	      double cellBasedEnergyRing7() const;
	      void setCellBasedEnergyRing7(double value);


        /// TRT hits high threshold over low threshold
        double TRT_NHT_OVER_NLT() const;
        void setTRT_NHT_OVER_NLT(double value);

        //@} -----------------------------------------------------------------
        /// @Tau Jet Vertex Fraction
        //@{ -----------------------------------------------------------------
	      double TauJetVtxFraction() const;
	      void setTauJetVtxFraction(double value);

        //@} -----------------------------------------------------------------
        /// @Track filter and generic pi0 cone
        //@{ -----------------------------------------------------------------

        // vector bool of which tracks pass the track filter
        std::vector<bool> TrackFilterPass() const;
        void setTrackFilterPass(std::vector<bool> value);

        // nProng based on what the track filter passes
        int TrackFilterProngs() const;
        void setTrackFilterProngs(int value);

        // flag used to ID probable fake taus by the track filter
        int TrackFilterQuality() const;
        void setTrackFilterQuality(int value);

        // generic cone size (dR)
        double Pi0ConeDR() const;
        void setPi0ConeDR(double value);

        protected:
        ///Printing function
        ///@param o string to print to
        template <class stream>
            void printOn(stream& o) const; 

        private:
        /// Name of class
        static std::string s_className;

        // -------------------------------------------------------------------
        /// @name Members applicable for track & calo seeded tau reconstruction
        //@{ -----------------------------------------------------------------

        /// Significance of z0 sin(Theta)
        double m_ipZ0SinThetaSigLeadTrk;

        /// Ratio of E_T of TauCandidate to p_T of leading track
        double m_etOverPtLeadTrk;

        /// Ratio of E_T of TauCandidate to p_T of leading loose track
        double m_etOverPtLeadLooseTrk;

        /// p_T of leading loose track - for Trigger
        double m_leadTrkPt;

        /// p_T of leading loose track - for Trigger
        double m_leadLooseTrkPt;

        /// Impact parameter significance of leading track
        double m_ipSigLeadTrk;

        /// Impact parameter significance of leading loose track
        double m_ipSigLeadLooseTrk;

        /// Loose tracks
        ElementLinkVector<Rec::TrackParticleContainer> m_looseTrk;

        /// Loose Conversion tracks
        ElementLinkVector<Rec::TrackParticleContainer> m_looseConvTrk;

        /// SeedCalo wide tracks
        ElementLinkVector<Rec::TrackParticleContainer> m_seedCalo_wideTrk;

        /// SeedTrk wide tracks
        ElementLinkVector<Rec::TrackParticleContainer> m_seedTrk_wideTrk;

        /// Other tracks
        ElementLinkVector<Rec::TrackParticleContainer> m_otherTrk;

        /// Charge of loose tracks
        double m_chrgLooseTrk;

        /// EM calorimeter cell cluster
        ///
        /// Cells used for candidate building, only EM calorimeter,
        /// without 3:rd sampling
        ElementLink<CaloClusterContainer>  m_cellEM012Cluster;

        /// Pi0 energy sum vector
        CLHEP::HepLorentzVector m_sumPi0Vec;

        /// Invariant mass of the tracks system
        double m_massTrkSys;

        /// Width of tracks momenta
        double m_trkWidth2;

        /// Transverse flight path significance for taus with at least
        /// 2 associated tracks
        double m_trFlightPathSig;

        /// Secondary vertex
        Trk::RecVertex* m_secVtx;

        /// E_T from energy flow
        double m_etEflow;

        /// mass from energy flow
        double m_mEflow;

        /// Pi0 clusters
        ElementLinkVector<CaloClusterContainer> m_pi0;


        //@} -----------------------------------------------------------------
        /// @Members for electron LLH calculation
        //@{ -----------------------------------------------------------------

        /// ratio of energy in window of 3X7 cells over 7X7 cells in second layer of ECAL
        double m_Ele_E237E277;

        /// fraction of EM cluster energy in presampler 
        double m_Ele_PresamplerFraction;

        /// fraction of EM cluster energy in first layer of ECAL 
        double m_Ele_ECALFirstFraction;

        //@} -----------------------------------------------------------------
        /// @Members for topocluster ID variables
        //@{ -----------------------------------------------------------------

        ///number of cells in topoclusters associated to tau candidate
        unsigned int m_numCells;

        ///number of topocluster constituents associated to tau candidate
        unsigned int m_numTopoClusters;

        ///number of effective topocluster constituents associated to tau candidate !!! is a double
        double m_numEffTopoClusters;

        ///invariant mass from topoclusters of jet associated to tau candidate 
        double m_topoInvMass;

        ///invariant mass from effective topoclusters of jet associated to tau candidate 
        double m_effTopoInvMass;

        ///mean distance to leading topocluster for topoclusters of jet associated to tau candidate 
        double m_topoMeanDeltaR;

        ///mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
        double m_effTopoMeanDeltaR;






        //@} -----------------------------------------------------------------
        /// @name Members applicable for calo seeded tau reconstruction
        //@{ -----------------------------------------------------------------

        /// Number of isolated tracks
        ///
        /// Number of tracks p_T>1GeV between dR = cmsdr and dR = cmsdR
        /// (from the jobOptions)
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_nIsolLooseTrk;

        /// Uncalibrated Et weighted radius in the Presampler + EM1 + EM2
        /// within dR < 0.4
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_EMRadius;

        /// Uncalibrated hadron calorimeter weighted radius
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_hadRadius;

        /// Uncalibrated Sum of Cell Transverse energy in the
        /// Presampler + EM1 + EM2 within dR < 0.4
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_etEMAtEMScale;

        /// Uncalibrated Sum of Cell Transverse energy in the
        /// Presampler + EM1 + EM2 within dR < 0.4
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_etHadAtEMScale;

        /// Ratio of the uncalibrated transverse energy of cells
        /// within 0.1<dR<0.2 and cells within 0<dR<0.4
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_isolFrac;

        /// Centrality fraction (ET(dr<0.1)/ET(dr<0.4) for all calos
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_centFrac;

        /// Uncalibrated transverse energy weighted width in the strip
        /// layer within dR < 0.4
        ///
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_stripWidth2;

        /// Number of Strip cells within dR < 0.4, with energy above
        /// specified threshold
        ///
        /// Applicable to calo seeded tau reconstruction
        int m_seedCalo_nStrip;

        /// Calibrated EM transverse energy
        ///
        /// EM is Presampler + EM1 + EM2
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_etEMCalib;

        /// Calibrated hadronic transverse energy
        ///
        /// HAD is cryo + EM3 + TILE1 + TILE2 + TILE3
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_etHadCalib;

        /// eta of TauJet calculated from calorimeter
        ///
        /// (for merged algorithm)
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_eta;

        /// phi of TauJet calculated from calorimeter
        ///
        /// (for merged algorithm)
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_phi;

        ///  average track distance from calorimeter seed
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_trkAvgDist;

        ///  RMS of track distance from calorimeter seed
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_trkRmsDist;
        
        ///  sum of 2 leading cluster energy over all cluster energy
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_lead2ClusterEOverAllClusterE;

        ///  sum of 3 leading cluster energy over all cluster energy
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_lead3ClusterEOverAllClusterE;
        
        ///  sum of transvers energy of clusters around jet seed
        ///  within 0.2 < dR < 0.4  
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_caloIso;
        
        ///  sum of transvers energy of clusters around jet seed
        ///  within 0.2 < dR < 0.4 (pile up corrected)  
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_caloIsoCorrected;
        
        ///  maximal dR of tracks associated to calo-seeded tau
        ///
        /// 
        /// Applicable to calo seeded tau reconstruction
        double m_seedCalo_dRmax;

        
        


        //@} -----------------------------------------------------------------
        /// @name Members applicable for track seeded tau reconstruction
        //@{ -----------------------------------------------------------------

        /// EM radius
        ///
        /// Uncalibrated transverse energy weighted radius in the
        /// Presampler + EM1 + EM2 within dR < 0.2.
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_EMRadius;

        /// Isolation fraction
        ///
        /// Ratio of uncalibrated E_T within dR<0.1 and dR<0.2
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_isolFrac;

        /// charged hadronic transverse energy over sum of
        /// transverse momenta of all tracks
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etChrgHadOverSumTrkPt ;

        /// Ratio of Transverse Energy in 0.2 < dr < 0.4 to
        /// total transverse energy at EM scale
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_isolFracWide;

        /// Hadronic E_T at EM scale
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etHadAtEMScale ;

        /// EM E_T at EM scale
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etEMAtEMScale ;

        ///  Transverse energy of cells classified as "pure electromagnetic"
        ///  seeded by egamma or topo cluster (steered from jobOptions),
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etEMCL ;

        /// Transverse energy of EM cells (at EM scale) classified as
        /// "charged electromagnetic" collected in narrow window around
        /// qualified track
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etChrgEM ;

        /// Transverse energy of EM cells (at EM scale), within  "core"
        /// cone around tau1P3P axis after subtraction of EMCL and Chrg
        /// cells
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etNeuEM ;

        /// Correction term for eflow calculations
        ///
        /// Applicable for track seeded tau reconsTruction
        double m_seedTrk_etResNeuEM ;

        /// hadronic leakage in Et summed over cells
        ///
        /// Applicable for track seeded tau reconsTruction
        double m_seedTrk_hadLeakEt;

        /// Transverse energy in narrow window around the track(s), EM
        /// layers 0 1 only
        ///
        /// Applicable for track seeded tau reconsTruction
        std :: vector<double> m_seedTrk_etChrgEM01Trk;

        /// Correction term for eflow calculations (evaluated for each
        /// track separately)
        ///
        /// Applicable for track seeded tau reconsTruction
        std :: vector<double> m_seedTrk_etResChrgEMTrk;

        /// Ratio of sum Et of LAr Cells to the pT of leading track
        ///
        /// Applicable for track seeded tau reconsTruction
        double m_seedTrk_sumEMCellEtOverLeadTrkPt;

        /// Secondary maximum
        ///
        /// Applicable for track seeded tau reconsTruction
        double m_seedTrk_secMaxStripEt;

        /// Strip width squared
        ///
        /// Square of uncalibrated transverse energy weighted width
        /// in the strip layer within dR < 0.2
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_stripWidth2;

        /// Number of strip cells
        ///
        /// Number of strip cells within dR < 0.2, with E > m_stripEthr
        /// Applicable for track seeded tau reconstruction
        int m_seedTrk_nStrip;

        /// Charged transverse energy in narrow window around track(s) in
        /// hadronic calorimeter
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etChrgHad;

        /// Associated, "not good" quality tracks in core region
        ///
        /// Applicable for track seeded tau reconstruction
        int m_seedTrk_nOtherCoreTrk;

        /// Associated, "not good" quality tracks in core region
        ///
        /// Applicable for track seeded tau reconstruction
        int m_seedTrk_nIsolTrk;

        /// Transverse energy in EM calo (at EM Scale) in  0.2 < dR < 0.4
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etIsolEM;

        /// Transverse energy in HAD calo (at EM Scale) in  0.2 < dR < 0.4
        ///
        /// Applicable for track seeded tau reconstruction
        double m_seedTrk_etIsolHad;
        
        /////////////////////////////////////////////////////////
        //
        // Energy Scale Attributes
        //
        /////////////////////////////////////////////////////////
        
        /// em+tes final scale
        double m_EM_TES_scale;

        /// lc+tes precalibration
        double m_LC_TES_precalib;

  	    //@} -----------------------------------------------------------------
        /// @Cell-based energies in rings for centrality fraction
        //@{ -----------------------------------------------------------------
	
	      /// Ring 1: 0.00  < R < 0.05
	      double m_cellBasedEnergyRing1;
	
	      /// Ring 2: 0.05  < R < 0.075
	      double m_cellBasedEnergyRing2;

	      /// Ring 3: 0.075 < R < 0.10
	      double m_cellBasedEnergyRing3;

	      /// Ring 4: 0.10  < R < 0.125
	      double m_cellBasedEnergyRing4;

	      /// Ring 5: 0.125 < R < 0.15
	      double m_cellBasedEnergyRing5;

	      /// Ring 6: 0.15  < R < 0.2
	      double m_cellBasedEnergyRing6;

	      /// Ring 7: 0.2   < R < 0.4
	      double m_cellBasedEnergyRing7;

        /// TRTHits high threshold over low threshold 
        /// used in electron veto
        double m_TRT_NHT_OVER_NLT;

        //@} -----------------------------------------------------------------
        /// @Tau Jet Vertex Fraction
        //@{ -----------------------------------------------------------------

        /// Tau Jet Vertex Fraction
        double m_TauJetVtxFraction;

        //@} -----------------------------------------------------------------
        /// @Track filter and generic pi0 cone
        //@{ -----------------------------------------------------------------

        // vector bool of which tracks pass the track filter
        std::vector<bool> m_TrackFilterPass;

        // nProng based on what the track filter passes
        int m_TrackFilterProngs;

        // flag used to ID probable fake taus by the track filter
        int m_TrackFilterQuality;

        // generic cone size (dR)
        double m_Pi0ConeDR;

    };//class TauCommonDetails
    } //Namespace Analysis

    // inline member function declarations
#include "TauCommonDetails.icc"

#endif //TAUEVENT_TAUCOMMONDETAILS_H
