// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v2.h 747258 2016-05-15 02:57:19Z griffith $
#ifndef XAODTAU_VERSIONS_TAUJET_V2_H
#define XAODTAU_VERSIONS_TAUJET_V2_H


// Core include(s):
#include "AthLinks/ElementLink.h"


// xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODTau/TauDefs.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODPFlow/PFOContainer.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {



   /// Class describing a tau jet
   ///
   /// More details of how xAOD::TauJet is supposed to work...
   ///
   /// @author Michel Janus <janus@cern.ch>
   ///
  class TauJet_v2 : public IParticle {

  public:
    /// Default constructor
    TauJet_v2();
    
    /// @name xAOD::IParticle functions
    /// @{
    
    /// The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const;
    /// The total energy of the particle
    virtual double           e() const;
    
    /// The invariant mass of the particle
    virtual double           m() const;
    /// The true rapidity (y) of the particle
    virtual double           rapidity() const;
    
    /// The full 4-momentum of the particle
    virtual FourMom_t p4() const;
    virtual FourMom_t p4(const TauJetParameters::TauCalibType calib) const;

    /// Base 4 Momentum type for taus
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : internal taus type.
    GenVecFourMom_t genvecP4() const; 
    GenVecFourMom_t genvecP4(const TauJetParameters::TauCalibType calib) const; 

    /// Set methods for IParticle values
    void setP4(double pt, double eta, double phi, double m);
    bool setP4(const TauJetParameters::TauCalibType calib, double pt, double eta, double phi, double m);

    //setters and getters for the individual 4-vector components at different calibration levels
    double ptJetSeed() const;
    double etaJetSeed() const;
    double phiJetSeed() const;
    double mJetSeed() const;

    double ptDetectorAxis() const;
    double etaDetectorAxis() const;
    double phiDetectorAxis() const;
    double mDetectorAxis() const;

    double ptIntermediateAxis() const;
    double etaIntermediateAxis() const;
    double phiIntermediateAxis() const;
    double mIntermediateAxis() const;

    double ptTauEnergyScale() const;
    double etaTauEnergyScale() const;
    double phiTauEnergyScale() const;
    double mTauEnergyScale() const;

    double ptTauEtaCalib() const;
    double etaTauEtaCalib() const;
    double phiTauEtaCalib() const;
    double mTauEtaCalib() const;

    // double ptPanTauEFlowRecProto() const;
    // double etaPanTauEFlowRecProto() const;
    // double phiPanTauEFlowRecProto() const;
    // double mPanTauEFlowRecProto() const;

    // double ptPanTauEFlowRec() const;
    // double etaPanTauEFlowRec() const;
    // double phiPanTauEFlowRec() const;
    // double mPanTauEFlowRec() const;

    double ptPanTauCellBasedProto() const;
    double etaPanTauCellBasedProto() const;
    double phiPanTauCellBasedProto() const;
    double mPanTauCellBasedProto() const;

    double ptPanTauCellBased() const;
    double etaPanTauCellBased() const;
    double phiPanTauCellBased() const;
    double mPanTauCellBased() const;

    double ptTrigCaloOnly() const;
    double etaTrigCaloOnly() const;
    double phiTrigCaloOnly() const;
    double mTrigCaloOnly() const;

    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    
    /// @}

    // the summed up charge of all tau tracks
    float charge() const;

    // set the charge
    void setCharge(float);

    /// the ROIWord, in case TauJet is used in EF trigger
    unsigned int ROIWord() const;

    // set the ROIWord, needed by TauTrigger 
    void setROIWord(unsigned int);

    /// Check if class contains requested discriminant
    /// @param discID discriminant id requested
    bool hasDiscriminant( TauJetParameters::TauID discID ) const;
    
    /// Get value of discriminant
    /// @param discID discriminant id requested
    double discriminant( TauJetParameters::TauID discID ) const;

    /// Set value of discriminant
    /// @param discID discriminant id requested
    /// @param disc discriminant value
    void setDiscriminant( TauJetParameters::TauID discID, double disc );

    ///Get Flag for tau acceptance based on predefined arbitrary criteria
    /// @param flag flag to check
    bool isTau( TauJetParameters::IsTauFlag flag ) const;

    ///Set Flag for tau acceptance based on predefined arbitrary criteria
    /// @param flag flag to set
    /// @param value value to set
    void setIsTau( TauJetParameters::IsTauFlag flag, bool value );

    /// Set veto flag
    /// @param flag flag to set
    /// @param value value to set
    void setFlag( TauJetParameters::VetoFlags flag, bool value );

    /// Get veto flag
    /// @param flag flag to check
    bool flag( TauJetParameters::VetoFlags flag ) const;


    /// Get and set values of common details variables via enum
    bool detail( TauJetParameters::Detail detail, int& value ) const;
    bool detail( TauJetParameters::Detail detail, float& value ) const;
    void setDetail( TauJetParameters::Detail detail, int value );
    void setDetail( TauJetParameters::Detail detail, float value );

    // getter for specialized types. Compilation error if used with other types.
    template <class T>
    typename xAODTAU_return_type<T>::type detail( TauJetParameters::Detail detail ) const
    {
    	T temp;
    	if (this->detail(detail,temp)) return temp;
    	else return 0;
    }

    /// Get and set values of pantau details variables via enum
    bool panTauDetail( TauJetParameters::PanTauDetails panTauDetail, int& value ) const;
    bool panTauDetail( TauJetParameters::PanTauDetails panTauDetail, float& value ) const;
    void setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, int value );
    void setPanTauDetail( TauJetParameters::PanTauDetails panTauDetail, float value );

    // getter for specialized types. Compilation error if used with other types.
    template <class T>
    typename xAODTAU_return_type<T>::type panTauDetail( TauJetParameters::PanTauDetails panTauDetail ) const
    {
    	T temp;
    	if (this->panTauDetail(panTauDetail,temp)) return temp;
    	else return 0;
    }

    //seed jet
    typedef ElementLink< xAOD::JetContainer > JetLink_t;

    const JetLink_t& jetLink() const;
    
    void setJetLink( const JetLink_t& jetLink ); 

    void setJet( const xAOD::JetContainer *cont, const xAOD::Jet *jet ); 

    const Jet* jet() const;

  
    //vertex that the tau comes from
    typedef ElementLink< xAOD::VertexContainer > VertexLink_t;

    const VertexLink_t& vertexLink() const;
    
    void setVertexLink( const VertexLink_t& vertexLink ); 

    void setVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex ); 

    const Vertex* vertex() const;


    //secondary vertex that the tau decays in

    const VertexLink_t& secondaryVertexLink() const;
    
    void setSecondaryVertexLink( const VertexLink_t& vertexLink ); 

    void setSecondaryVertex( const xAOD::VertexContainer *cont, const xAOD::Vertex *vertex ); 

    const Vertex* secondaryVertex() const;
  

    //tracks
    typedef std::vector< ElementLink< xAOD::TrackParticleContainer > >  TrackParticleLinks_t;
   
    const TrackParticleLinks_t& trackLinks() const;

    void setTrackLinks( const TrackParticleLinks_t& trackParticles );
    /// Get the pointer to a given track associated with this tau
    const TrackParticle* track( size_t i ) const;
    // /// Get the number of track particles associated with this tau
    size_t nTracks() const;
    /// add a track to the tau
    void addTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the tau
    void clearTrackLinks();
    
    // Get and set track flags
    bool trackFlag(const TrackParticle* track, TauJetParameters::TauTrackFlag flag) const;
    void setTrackFlag(const TrackParticle* track, TauJetParameters::TauTrackFlag flag, bool value);

    //name accessors for trackFilterProngs
    int trackFilterProngs() const;
    //name accessors for trackFilterProngs
    void setTrackFilterProngs(int);

    //name accessors for trackFilterQuality
    int trackFilterQuality() const;
    //name accessors for trackFilterQuality
    void setTrackFilterQuality(int);

    //name accessors for pi0ConeDR
    float pi0ConeDR() const;
    //name accessors for pi0ConeDR
    void setPi0ConeDR(float);


    //name accessors for trackEtaStrip
    float trackEtaStrip(unsigned int numTrack) const;
    //name accessors for trackEtaStrip
    void setTrackEtaStrip(unsigned int numTrack, float eta);


    //name accessors for trackPhiStrip
    float trackPhiStrip(unsigned int numTrack) const;
    //name accessors for trackPhiStrip
    void setTrackPhiStrip(unsigned int numTrack, float phi);


    //wide tracks
    const TrackParticleLinks_t& wideTrackLinks() const;
    
    void setWideTrackLinks( const TrackParticleLinks_t& trackParticles );
    /// Get the pointer to a given track associated with this tau
    const TrackParticle* wideTrack( size_t i ) const;
    /// Get the number of track particles associated with this tau
    size_t nWideTracks() const;
    /// add a track to the tau
    void addWideTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the tau
    void clearWideTrackLinks();


    //other tracks
    const TrackParticleLinks_t& otherTrackLinks() const;
    
    void setOtherTrackLinks( const TrackParticleLinks_t& trackParticles );
    /// Get the pointer to a given track associated with this tau
    const TrackParticle* otherTrack( size_t i ) const;
    /// Get the number of track particles associated with this tau
    size_t nOtherTracks() const;
    /// add a track to the tau
    void addOtherTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the tau
    void clearOtherTrackLinks();



    typedef std::vector< ElementLink< xAOD::PFOContainer > >  PFOLinks_t;


    //photon shot PFOs
    const PFOLinks_t& hadronicPFOLinks() const;

    void setHadronicPFOLinks( const PFOLinks_t& hadronicPFOs );
    /// Get the pointer to a given hadronic PFO associated with this tau
    const PFO* hadronicPFO( size_t i ) const;
    /// Get the number of hadronic PFO particles associated with this tau
    size_t nHadronicPFOs() const;
    /// add a hadronic PFO to the tau
    void addHadronicPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all hadronic PFOs from the tau
    void clearHadronicPFOLinks();


    //photon shot PFOs
    const PFOLinks_t& shotPFOLinks() const;

    void setShotPFOLinks( const PFOLinks_t& shotPFOs );
    /// Get the pointer to a given shot PFO associated with this tau
    const PFO* shotPFO( size_t i ) const;
    /// Get the number of shot PFO particles associated with this tau
    size_t nShotPFOs() const;
    /// add a shot PFO to the tau
    void addShotPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all shot PFOs from the tau
    void clearShotPFOLinks();


    ///default pfos
    //
    //charged PFOs
    const PFOLinks_t& chargedPFOLinks() const;

    void setChargedPFOLinks( const PFOLinks_t& chargedPFOs );
    /// Get the pointer to a given charged PFO associated with this tau
    const PFO* chargedPFO( size_t i ) const;
    /// Get the number of charged PFO particles associated with this tau
    size_t nChargedPFOs() const;
    /// add a charged PFO to the tau
    void addChargedPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all charged PFOs from the tau
    void clearChargedPFOLinks();


    //neutral PFOs
    const PFOLinks_t& neutralPFOLinks() const;

    void setNeutralPFOLinks( const PFOLinks_t& neutralPFOs );
    /// Get the pointer to a given neutral PFO associated with this tau
    const PFO* neutralPFO( size_t i ) const;
    /// Get the number of neutral PFO particles associated with this tau
    size_t nNeutralPFOs() const;
    /// add a neutral PFO to the tau
    void addNeutralPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all neutral PFOs from the tau
    void clearNeutralPFOLinks();


    //pi0 PFOs
    const PFOLinks_t& pi0PFOLinks() const;

    void setPi0PFOLinks( const PFOLinks_t& pi0PFOs );
    /// Get the pointer to a given pi0 PFO associated with this tau
    const PFO* pi0PFO( size_t i ) const;
    /// Get the number of pi0 PFO particles associated with this tau
    size_t nPi0PFOs() const;
    /// add a pi0 PFO to the tau
    void addPi0PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all pi0 PFOs from the tau
    void clearPi0PFOLinks();




    ///cellbased pfos
    //
    //cellbased_charged PFOs
    const PFOLinks_t& protoChargedPFOLinks() const;

    void setProtoChargedPFOLinks( const PFOLinks_t& protoChargedPFOs );
    /// Get the pointer to a given cellbased_charged PFO associated with this tau
    const PFO* protoChargedPFO( size_t i ) const;
    /// Get the number of cellbased_charged PFO particles associated with this tau
    size_t nProtoChargedPFOs() const;
    /// add a cellbased_charged PFO to the tau
    void addProtoChargedPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_charged PFOs from the tau
    void clearProtoChargedPFOLinks();


    //cellbased_neutral PFOs
    const PFOLinks_t& protoNeutralPFOLinks() const;

    void setProtoNeutralPFOLinks( const PFOLinks_t& protoNeutralPFOs );
    /// Get the pointer to a given cellbased_neutral PFO associated with this tau
    const PFO* protoNeutralPFO( size_t i ) const;
    /// Get the number of cellbased_neutral PFO particles associated with this tau
    size_t nProtoNeutralPFOs() const;
    /// add a cellbased_neutral PFO to the tau
    void addProtoNeutralPFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_neutral PFOs from the tau
    void clearProtoNeutralPFOLinks();


    //pi0 PFOs
    const PFOLinks_t& protoPi0PFOLinks() const;

    void setProtoPi0PFOLinks( const PFOLinks_t& protoPi0PFOs );
    /// Get the pointer to a given cellbased_pi0 PFO associated with this tau
    const PFO* protoPi0PFO( size_t i ) const;
    /// Get the number of cellbased_pi0 PFO particles associated with this tau
    size_t nProtoPi0PFOs() const;
    /// add a cellbased_pi0 PFO to the tau
    void addProtoPi0PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_pi0 PFOs from the tau
    void clearProtoPi0PFOLinks();


  
    /// @}
    

    void dump() const;


  }; // class TauJet

} // namespace xAOD


// Declare IParticle as a base class of TauJet_v2:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::TauJet_v2, xAOD::IParticle );

#endif // XAODTAU_VERSIONS_TAUJET_V2_H

//  LocalWords:  newROIWord
