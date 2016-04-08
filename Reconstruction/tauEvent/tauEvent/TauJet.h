/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///@file TauJet.h
///
///Declaration of tau jet transient class
///
///Package: Reconstruction/tauEvent
///
///@author Thomas Burgess <tburgess-at-cern.ch>
///@author Kyle Cranmer
///@author Michael Heldmann
///@author Lukasz Janyst
///@author Srini Rajagopalan
///@author Elzbieta Richter-Was
///
/// Created 2007-01-13.
///
/// $Id: TauJet.h,v 1.24 2009/05/11 14:47:32 tburgess Exp $

#ifndef TAU_ANALYSIS_H
#define TAU_ANALYSIS_H

#include <bitset>

#include "CLHEP/Vector/LorentzVector.h"
#include "ParticleEvent/ParticleImpl.h"
#include "DataModel/ElementLinkVector.h"
#include "FourMom/Lib/P4ImplEEtaPhiM.h"
#include "Particle/TrackParticleContainer.h"
#include "CaloEvent/CaloClusterContainer.h"
#include "JetEvent/JetCollection.h"

#include "tauEvent/TauJetParameters.h"
#include "tauEvent/TauPID.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/Tau1P3PDetails.h"
#include "tauEvent/Tau1P3PExtraDetails.h"
#include "tauEvent/TauRecDetails.h"
#include "tauEvent/TauRecExtraDetails.h"
#include "tauEvent/TauCommonDetails.h"
#include "tauEvent/TauCommonExtraDetails.h"

//forward declaration to store link to PanTauSeed
//this connects the TauJet to PanTauSeed and vice versa
namespace PanTau {
  class PanTauSeed;
}

namespace Analysis
{
    typedef NavigableTerminalNode TauJetNavigation;

    /// Object for taus common for ESD and AOD. Based on design of Photon and
    /// Electron and on previously existing code of tauObject and TauJet
    class TauJet: public ParticleImpl<
                  ::Analysis::TauJetNavigation, // not really a terminal node: done here
                  ::P4ImplEEtaPhiM
                                  >
    {
        ///////////////////////////////////////////////////////////////////
        // Public typedefs:
        ///////////////////////////////////////////////////////////////////
        public:	
            //-----------------------------------------------------------------
            ///@name navigation stuff
            ///@{
            // for readability and lazy people
            typedef ParticleImpl< ::Analysis::TauJetNavigation,
                    ::P4ImplEEtaPhiM
                        > TauJetImpl_t;
            typedef TauJetImpl_t::navigable_type navigable_type;
            typedef TauJetImpl_t::momentum_type  momentum_type;
            typedef TauJetImpl_t::particle_type  particle_type;
            ///@}

            //-----------------------------------------------------------------
            ///@name TauJet Constructors, destructor, assignment operator
            ///@{

            /// Default Constructor
            TauJet();

            /// Constructor
            TauJet( TauJetParameters :: Author author );

            /// Copy Constructor
            ///@param rhs TauJet to copy
            TauJet( const TauJet &rhs );

            ///Virtual Destructor
            virtual ~TauJet();

            /// Printing operator
            /// @param o stream to write to
            /// @param tau tau to print
            friend std::ostream& operator<< (
                    std::ostream& o, const TauJet& tau);

            /// Printing operator
            /// @param o stream to write to
            /// @param tau tau to print
            friend std::ostream& operator<< (
                    std::ostream& o, const TauJet* tau);

            /// Printing operator
            /// @param o stream to write to
            /// @param tau tau to print
            friend MsgStream& operator<< (
                    MsgStream& o, const TauJet& tau);

            /// Printing operator
            /// @param o stream to write to
            /// @param tau tau to print
            friend MsgStream& operator<< (
                    MsgStream& o, const TauJet* tau);

            /// assignment operator
            ///@param rhs TauJet to copy
            TauJet &operator = ( const TauJet &rhs );

            /// Equality operator
            /// @param rhs right hand side TauJet instance to compare 
            /// @return true if equal, false otherwise
            bool operator==( const TauJet& rhs ) const ;

            /// Inequality operator
            /// @param rhs TauJet instance to compare to
            /// @return !operator==
            bool operator!=( const TauJet& rhs ) const;

            ///@}

            //-----------------------------------------------------------------
            ///@name Navigation methods
            //@{ ----------------------------------------------------------------

            /// @c INavigable interface,
            /// fill constituents (@c Jet,@c CaloCluster,@c CaloCell)
            /// @param thisToke Navigation Token
            virtual void fillToken(
                    INavigationToken &thisToken ) const;

            /// @c INavigable interface,
            ///fill constituents (@c Jet,@c CaloCluster,@c CaloCell)
            /// @param thisToke Navigation Token
            virtual void fillToken(
                    INavigationToken &thisToken,
                    const boost::any& ) const;

            /// dummy Parameter accessors,
            /// for analysis code backward compatibility
            ///
            /// DEPRECATED - DO NOT USE!
            ///
            /// @param detail detail to acess
            /// @return always returns -1111
            double parameter( TauJetParameters :: Detail detail ) const;

            //@} ----------------------------------------------------------------
            ///@name Accessors for calorimeter info
            //@{ ----------------------------------------------------------------

            /// seed @c CaloCluster, DEPRECATED as of release 13
            const CaloCluster *cluster() const;

            /// @c ElementLink to seed @c CaloCluster,
            ///DEPRECATED as of release 13
            const ElementLink<CaloClusterContainer> clusterLink() const;

            /// @c ElementLink to seed @c CaloCluster (const)
            ///DEPRECATED as of release 13
            ElementLink<CaloClusterContainer> &clusterLink();

            /// set the @c CaloCluster as the seed of @c TauJet,
            /// DEPRECATED as of release 13
            void setCluster(
                    const CaloClusterContainer *cont,
                    const CaloCluster *clust );

            /// set @c ElementLink of the @c CaloCluster seed of @c TauJet
            /// DEPRECATED as of release 13
            void setClusterLink(
                    const ElementLink<CaloClusterContainer> &x );

            /// cell @c CaloCluster, might be @c NULL
            /// the ID variables are calculated from this cluster if its not empty
            const CaloCluster *cellCluster() const;

            /// @c ElementLink to cell @c CaloCluster, might be invalid
            /// the ID variables are calculated from this cluster if its not empty
            const ElementLink<CaloClusterContainer> cellClusterLink() const;

            /// @c ElementLink to cell @c CaloCluster, might be invalid
            /// the ID variables are calculated from this cluster if its not empty
            ElementLink<CaloClusterContainer> &cellClusterLink();

            /// set the @c CaloCluster as the seed of @c TauJet
            void setCellCluster(
                    const CaloClusterContainer *cont,
                    const CaloCluster *clust );

            /// set @c ElementLink of the @c CaloCluster seed of @c TauJet
            void setCellClusterLink(
                    const ElementLink<CaloClusterContainer> &x );

            //@} -----------------------------------------------------------------
            ///@name Jet accessors
            //@{ --------------------------------------------------------------

            /// seed @c Jet, might be @c NULL if not seeded from a @c Jet
            const Jet *jet() const;

            /// @c ElementLink to seed @c Jet,
            ///might be invalid if not seeded from a @c Jet
            const ElementLink<JetCollection> jetLink() const;

            /// @c ElementLink to seed @c Jet,
            ///might be invalid if not seeded from a @c Jet
            ElementLink<JetCollection> &jetLink();

            /// set the @c Jet as the seed to the @c TauJet
            void setJet(
                    const JetCollection *cont,
                    const Jet *clust );

            /// set the @c ElementLink to the seed @c Jet
            void setJetLink( const ElementLink<JetCollection> &x );

            //@} --------------------------------------------------------------
            ///@name Accessors for tracking info
            //@{ ---------------------------------------------------------------

            /// Tracks associated to Tau candidate
            const Rec :: TrackParticle *track( unsigned int i ) const;

            /// number of Tracks associated to Tau candidate,
            /// CAUTION not to be confused with @c numberOfTracks()!
            unsigned int numTrack() const;

            ///Get track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            const ElementLinkVector<Rec::TrackParticleContainer>&
                trackLinkVector() const;

            ///Get track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            ElementLinkVector<Rec::TrackParticleContainer>&
                trackLinkVector();

            ///Add a track to the TauJet
            ///@param cont container to add to
            ///@param track track to add
            void addTrack( const Rec::TrackParticleContainer *cont,
                    const Rec::TrackParticle *track );

            ///Remove all associated track from TauJet
            void clearTracks();

            //@} --------------------------------------------------------------
            ///@name Accessors for conversion tracks
            //@{  --------------------------------------------------------------

            /// Get a conversion track
            ///@param i Id of track to access
            ///@return tau candidate conversion track, 0 otherwise
            const Rec :: TrackParticle *conversionTrack(
                    unsigned int i ) const;

            /// Number of conversion tracks
            unsigned int numConversionTrack() const;

            /// Get conversion track element link vector (const)
            ///@return conversion track element link vector
            const ElementLinkVector<Rec::TrackParticleContainer>&
                conversionTrackLinkVector() const;

            /// Get conversion track element link vector
            ///@return conversion track element link vector
            ElementLinkVector<Rec::TrackParticleContainer>&
                conversionTrackLinkVector();

            /// Add a conversion track
            ///@param cont container to add to
            ///@param track track to add
            void addConversionTrack(
                    const Rec::TrackParticleContainer *cont,
                    const Rec::TrackParticle *track );

            ///Remove all associated track from TauJet
            void clearConversionTracks();


            //@} --------------------------------------------------------------
            ///@name Accessors for seed calo tracking info
            //@{ ---------------------------------------------------------------

            /// Seed Calo Tracks associated to Tau candidate
            const Rec :: TrackParticle *seedCalo_track( unsigned int i ) const;

            /// number of seed calo Tracks associated to Tau candidate,
            unsigned int seedCalo_numTrack() const;

            ///Get seed calo track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            const ElementLinkVector<Rec::TrackParticleContainer>&
                seedCalo_trackLinkVector() const;

            ///Get seed calo track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            ElementLinkVector<Rec::TrackParticleContainer>&
                seedCalo_trackLinkVector();

            ///Add a seed calo track to the TauJet
            ///@param cont container to add to
            ///@param track track to add
            void addSeedCalo_track( const Rec::TrackParticleContainer *cont,
                    const Rec::TrackParticle *track );

            ///Remove all seed calo associated tracks from TauJet
            void clearSeedCalo_tracks();
            
            
            //@} --------------------------------------------------------------
            ///@name Accessors for seed track tracking info
            //@{ ---------------------------------------------------------------

            /// Seed Track Tracks associated to Tau candidate
            const Rec :: TrackParticle *seedTrk_track( unsigned int i ) const;

            /// number of seed trk Tracks associated to Tau candidate,
            unsigned int seedTrk_numTrack() const;

            ///Get seed trk track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            const ElementLinkVector<Rec::TrackParticleContainer>&
                seedTrk_trackLinkVector() const;

            ///Get seed trk track link vector
            ///@return ElementVector to Tracks associated to Tau candidate
            ElementLinkVector<Rec::TrackParticleContainer>&
                seedTrk_trackLinkVector();

            ///Add a seed trk track to the TauJet
            ///@param cont container to add to
            ///@param track track to add
            void addSeedTrk_track( const Rec::TrackParticleContainer *cont,
                    const Rec::TrackParticle *track );

            ///Remove all seed trk associated tracks from TauJet
            void clearSeedTrk_tracks();


            ///Number of prongs
            ///
            /// number of tracks - number of conversion tracks
            int nProng() const;

            ///Check if track is a conversion track
            ///@param i Id of track to check
            bool isConversionTrack( unsigned int i ) const;

            //@} --------------------------------------------------------------
            ///@name  Accessors for identification info
            //@{ --------------------------------------------------------------

            /// Get TauPID object containing discriminant values
            ///@return const TauPID object
            const TauPID *tauID() const;

            /// Get TauPID object containing discriminant values
            ///@return TauPID object
            TauPID *tauID();

            /// Set TauPID object that contains discriminant values
            ///@param tauID TauPID object
            void setTauID( TauPID *tauID );

            //@} ---------------------------------------------------------------
            ///@name Details accessors
            //@{ ---------------------------------------------------------------

            /// Add new tau detail instance
            /// @param container Container to store details in
            /// @param details Details instance to add
            void addDetails(
                    const TauDetailsContainer *container,
                    TauDetails *details );

            /// Remove all tau details object with matching name
            /// @param name Storegate name of class to remove
            /// @param className class name
            void removeDetails(
                    const std::string &name,
                    const std::string& className = "" );

            /// Remove tau details class by index
            /// @param i index (in order they were inserted)
            void removeDetails(
                    int i );

            /// Get first details object stored in SG collection with
            /// matching name.
            ///
            /// CAUTION: returns @c NULL if the first one happens to be the
            /// wrong type
            ///
            /// @param name storegate name
            ///
            /// @return details or @c NULL
            template <typename T>
                const T *details(
                        const std::string &name ) const;

            /// Get the first details object matching requested type or
            /// return @c NULL
            template <typename T>
                const T *details() const;

            /// first @c TauDetails object of type className and possibly
            /// stored with SG key name. Returns @c NULL if none match
            ///
            /// @param className name of class
            /// @param name Store gate name
            const TauDetails* detail (const std::string& className,
                    const std::string& name = "") const;

            /// @c Get TauDetails object by index
            /// @param i index (in the order they where inserted)
            const TauDetails* detail (
                    int i) const;

            /// @c Get TauDetails ElementLink by index
            /// @param i index (in the order they where inserted)
            const ElementLink<TauDetailsContainer> detailElementLink (
                    int i) const;	

            /// Get classname of @c TauDetails by index
            /// @param i index (in the order they where inserted)
            std::string detailName (
                    int i) const;

            /// number of details objects in this TauJet
            int nDetails() const;

            /// clean details for AOD
            /// Removes extra details and any invalid elementlinks from details list
            void CleanDetailsForAOD();

            ElementLinkVector<TauDetailsContainer>& tauDetailLinkVector();
            const ElementLinkVector<TauDetailsContainer>& tauDetailLinkVector() const;

            //@} ---------------------------------------------------------------
            ///@name Author functions
            //@{ ---------------------------------------------------------------

            /// Author of this object (DO NOT USE! only for backward compatibility)
            TauJetParameters :: Author author() const;

            /// check if this object has Author @c author (can be more than one)
            ///@param author Author to check
            bool hasAuthor(
                    const TauJetParameters :: Author &author ) const;

            /// set the author flag to true
            ///@param author Author to set
            void setAuthor(
                    const TauJetParameters :: Author &author );

            /// clear the author flag to value false
            ///@param author Author to clear
            void clearAuthor(
                    const TauJetParameters :: Author &author );

            //@} ---------------------------------------------------------------
            ///@name Number of tracks accessors
            /// Independent property nothing to do with the actual number
            /// of element links to tracks
            ///
            /// to be used by atlfast and the trigger, to get number
            /// of element links of associated tracks use numTrack method
            //@{ --------------------------------------------------------------

            /// Get number of tracks
            ///@return Number of tracks
            unsigned int numberOfTracks() const;

            /// Set number of tracks
            ///@ param numberOfTracks Number of tracks
            void setNumberOfTracks(
                    unsigned int numberOfTracks );

            //@} --------------------------------------------------------------
            ///@name ROI Word accessors
            //@{ --------------------------------------------------------------

            /// Set ROI Word (trigger purposes)
            ///@param RoiWord ROI Word to set
            void setROIWord( unsigned int RoiWord );

            /// Retrieve ROI Word (trigger purposes)
            ///@return ROI Word
            unsigned int ROIWord() const;

            //@} ---------------------------------------------------------------
            /// @name 4-mom modifiers
            //@{ --------------------------------------------------------------

            /// set energy data member
            ///@param energy
            void setE( double ene );

            /// set eta data member
            ///@param eta eta
            void setEta( double eta );

            /// set phi data member
            ///@param phi phi
            void setPhi( double phi );

            /// set mass data member
            ///@param m mass
            void setM( double m );

            //@} ----------------------------------------------------------------
            ///@name Merge to common details functions
            //@{ ----------------------------------------------------------------

            /// Merge Tau[1P3P|Rec](Extra)Details to TauCommon(Extra)Details
            ///
            /// Note: Will remove existing details and fill new common details!
            /// Note: if a container is 0, the merged details will not be stored!
            ///
            /// @param tauDetailsContainer container for details
            /// @param tauExtraDetailsContainer container for extra details
            void MergeToCommonDetails(
                    TauDetailsContainer* tauDetailsContainer = 0 , 
                    TauDetailsContainer* tauExtraDetailsContainer = 0);

            /// Overloading MergeToCommonDetails function

            //         void MergeToCommonDetails();  // used for transient only
            //        void MergeToCommonDetails( TauDetailsContainer* tauDetailsContainer); // used for AODs or data without extra details

            ///Get state Merge to common details state
            ///@return true if done merge to common details
            bool hasMergedToCommonDetails() const;

            ///Set state Merge to common details state
            ///@param state state to set
            void setHasMergedToCommonDetails(
                    bool state);

            //@}

            ///Uses printOn to make a string of the TauDetails
            ///@return string representation of tau details
            std::string toString() const;

            //@} --------------------------------------------------------------
            ///@name HEP Lorentz Vector storage
            //@{ ---------------------------------------------------------------
            void storeHLV( TauJetParameters :: TauCalibType type, CLHEP :: HepLorentzVector hlv );
            CLHEP::HepLorentzVector getHLV( TauJetParameters :: TauCalibType type ) const;
            void clearHLV();

            ///Get the list of links to PanTauSeed objects that were created for this tauJet      
            std::vector<PanTau::PanTauSeed*> getListOfPanTauSeeds() const;                        
                                                                                                 
            ///add a new PanTauSeed to this tau                                                   
            void addPanTauSeed(PanTau::PanTauSeed* pantauSeed);     

        protected:
            ///Printing function
            ///@param o string to print to
            template <class stream>
                void printOn(stream& o) const;

        private:
            //   ------------------------------------------------------------
            ///@name Copy to common details functions
            //@{ ------------------------------------------------------------

            ///Converts a Tau1P3PDetails to a TauCommonDetails
            ///@param tau1P3PDetails
            ///@param tauCommonDetails
            void CopyToCommonDetails(
                    const Tau1P3PDetails *tau1P3PDetails,
                    TauCommonDetails *tauCommonDetails);

            ///Converts a TauRecDetails to a TauCommonDetails
            ///@param tauRecDetails
            ///@param tauCommonDetails
            void CopyToCommonDetails(
                    const TauRecDetails *tauRecDetails,
                    TauCommonDetails *tauCommonDetails);

            ///Converts a Tau1P3PExtraDetails to a TauCommonExtraDetails
            ///@param tau1P3PExtraDetails
            ///@param tauCommonExtraDetails
            void CopyToCommonDetails(
                    const Tau1P3PExtraDetails *tau1P3PExtraDetails,
                    TauCommonExtraDetails *tauCommonExtraDetails);

            ///Converts a TauRecExtraDetails to a TauCommonExtraDetails
            ///@param tauRecExtraDetails
            ///@param tauCommonExtraDetails
            void CopyToCommonDetails(
                    const TauRecExtraDetails *tauRecExtraDetails,
                    TauCommonExtraDetails *tauCommonExtraDetails);

            ///@} ------------------------------------------------------------

            ///Tracks
            ElementLinkVector<Rec::TrackParticleContainer> m_tracks;

            ///Conversion tracks
            ElementLinkVector<Rec::TrackParticleContainer> m_conversionTracks;

            ///Seed Calo Tracks
            ElementLinkVector<Rec::TrackParticleContainer> m_seedCalo_tracks;
          
            ///Seed Trk Tracks
            ElementLinkVector<Rec::TrackParticleContainer> m_seedTrk_tracks;

            ///Vector containing all details
            ElementLinkVector<TauDetailsContainer> m_tauDetails;

            ///Calo cluster
            ElementLink<CaloClusterContainer> m_cluster;

            ///Calo cell
            ElementLink<CaloClusterContainer> m_cellCluster;

            ///Jets
            ElementLink<JetCollection> m_jet;

            ///Allow more than one author, TauJetParameters::Author used as index
            std::bitset<8> m_author;

            ///Number of tracks
            unsigned int m_numberOfTracks;

            //Tau particle ID info
            TauPID *m_tauID;

            ///ROI word
            unsigned int m_roiWord;

            ///True if MergeToCommonDetails has been called
            bool m_hasMergedToCommonDetails;

            ///HLV storage
            std::vector<std::pair< TauJetParameters::TauCalibType, CLHEP::HepLorentzVector > > m_tauHLVStorage;

            ///List of associated pantau seeds                                                    
            std::vector<PanTau::PanTauSeed*> m_listOfPanTauSeeds;

    }; //Class TauJet

    template <class stream>
    void TauJet::printOn(stream& o) const
    {
        o << "TauJet {"
            << "tracks: {";
        for(ElementLinkVector<Rec::TrackParticleContainer>::const_iterator itrk
                = m_tracks.begin(); itrk!=m_tracks.end(); ++itrk)
            o << *(*itrk) << " ";
        o << "};\n"
            << "conversionTracks: {";
        for(ElementLinkVector<Rec::TrackParticleContainer>::const_iterator itrk
                = m_conversionTracks.begin(); 
                itrk!=m_conversionTracks.end(); ++itrk)
            o << *(*itrk) << " ";
        o << "};\n"
            << "tauDetails: {";
        for(ElementLinkVector<TauDetailsContainer>::const_iterator idetail
                = m_tauDetails.begin(); idetail!=m_tauDetails.end(); ++idetail)
            if ((*idetail).isValid())
                o << *(*idetail) << " ";
        o << "};\n"
            //not m_cluster
            //not m_cellCluster
            //not m_jet
            << "author: " << m_author << ";\n"
            << "numberOfTracks: " << m_numberOfTracks <<";\n"
            //not m_tauID
            << "roiWord: " << m_roiWord << ";\n"
            << "hasMergedToCommonDetails: " << m_hasMergedToCommonDetails << ";\n"
            << "};";
    }

    inline std::ostream& operator<<(
            std::ostream& o, const TauJet& tau)
    {
        tau.printOn(o);
        return o;
    }   

    inline std::ostream& operator<<(
            std::ostream& o, const TauJet* tau)
    {
        tau->printOn(o);
        return o;
    }  

    inline MsgStream& operator<<(
            MsgStream& o, const TauJet& tau)
    {
        tau.printOn(o);
        return o;
    }   

    inline MsgStream& operator<<(
            MsgStream& o, const TauJet* tau)
    {
        tau->printOn(o);
        return o;
    }  

} //Namespace analysis

#include "TauJet.icc"

#endif // TAU_ANALYSIS_H
