// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v1.h 740338 2016-04-14 16:03:58Z griffith $
#ifndef XAODTAU_VERSIONS_TAUJET_V1_H
#define XAODTAU_VERSIONS_TAUJET_V1_H


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
  class TauJet_v1 : public IParticle {

  public:
    /// Default constructor
    TauJet_v1();
    
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
    /// Get the number of track particles associated with this tau
    size_t nTracks() const;
    /// add a track to the tau
    void addTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the tau
    void clearTrackLinks();
    
    //name accessors for trackFilterPass
    int trackFilterPass(unsigned int numTrack) const;
    //name accessors for trackFilterPass
    void setTrackFilterPass(unsigned int numTrack, int pass);

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


    //conversion tracks
    const TrackParticleLinks_t& conversionTrackLinks() const;
    
    void setConversionTrackLinks( const TrackParticleLinks_t& trackParticles );
    /// Get the pointer to a given track associated with this tau
    const TrackParticle* conversionTrack( size_t i ) const;
    /// Get the number of track particles associated with this tau
    size_t nConversionTracks() const;
    /// add a track to the tau
    void addConversionTrackLink( const ElementLink< TrackParticleContainer >& tr);
    /// Remove all tracks from the tau
    void clearConversionTrackLinks();


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
    const PFOLinks_t& hadronic_PFOLinks() const;

    void setHadronic_PFOLinks( const PFOLinks_t& hadronic_PFOs );
    /// Get the pointer to a given hadronic PFO associated with this tau
    const PFO* hadronic_PFO( size_t i ) const;
    /// Get the number of hadronic PFO particles associated with this tau
    size_t nHadronic_PFOs() const;
    /// add a hadronic PFO to the tau
    void addHadronic_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all hadronic PFOs from the tau
    void clearHadronic_PFOLinks();


    //photon shot PFOs
    const PFOLinks_t& shot_PFOLinks() const;

    void setShot_PFOLinks( const PFOLinks_t& shot_PFOs );
    /// Get the pointer to a given shot PFO associated with this tau
    const PFO* shot_PFO( size_t i ) const;
    /// Get the number of shot PFO particles associated with this tau
    size_t nShot_PFOs() const;
    /// add a shot PFO to the tau
    void addShot_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all shot PFOs from the tau
    void clearShot_PFOLinks();


    ///default pfos
    //
    //charged PFOs
    const PFOLinks_t& charged_PFOLinks() const;

    void setCharged_PFOLinks( const PFOLinks_t& charged_PFOs );
    /// Get the pointer to a given charged PFO associated with this tau
    const PFO* charged_PFO( size_t i ) const;
    /// Get the number of charged PFO particles associated with this tau
    size_t nCharged_PFOs() const;
    /// add a charged PFO to the tau
    void addCharged_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all charged PFOs from the tau
    void clearCharged_PFOLinks();


    //neutral PFOs
    const PFOLinks_t& neutral_PFOLinks() const;

    void setNeutral_PFOLinks( const PFOLinks_t& neutral_PFOs );
    /// Get the pointer to a given neutral PFO associated with this tau
    const PFO* neutral_PFO( size_t i ) const;
    /// Get the number of neutral PFO particles associated with this tau
    size_t nNeutral_PFOs() const;
    /// add a neutral PFO to the tau
    void addNeutral_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all neutral PFOs from the tau
    void clearNeutral_PFOLinks();


    //pi0 PFOs
    const PFOLinks_t& pi0_PFOLinks() const;

    void setPi0_PFOLinks( const PFOLinks_t& pi0_PFOs );
    /// Get the pointer to a given pi0 PFO associated with this tau
    const PFO* pi0_PFO( size_t i ) const;
    /// Get the number of pi0 PFO particles associated with this tau
    size_t nPi0_PFOs() const;
    /// add a pi0 PFO to the tau
    void addPi0_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all pi0 PFOs from the tau
    void clearPi0_PFOLinks();




    ///cellbased pfos
    //
    //cellbased_charged PFOs
    const PFOLinks_t& cellBased_Charged_PFOLinks() const;

    void setCellBased_Charged_PFOLinks( const PFOLinks_t& cellBased_Charged_PFOs );
    /// Get the pointer to a given cellbased_charged PFO associated with this tau
    const PFO* cellBased_Charged_PFO( size_t i ) const;
    /// Get the number of cellbased_charged PFO particles associated with this tau
    size_t nCellBased_Charged_PFOs() const;
    /// add a cellbased_charged PFO to the tau
    void addCellBased_Charged_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_charged PFOs from the tau
    void clearCellBased_Charged_PFOLinks();


    //cellbased_neutral PFOs
    const PFOLinks_t& cellBased_Neutral_PFOLinks() const;

    void setCellBased_Neutral_PFOLinks( const PFOLinks_t& cellBased_Neutral_PFOs );
    /// Get the pointer to a given cellbased_neutral PFO associated with this tau
    const PFO* cellBased_Neutral_PFO( size_t i ) const;
    /// Get the number of cellbased_neutral PFO particles associated with this tau
    size_t nCellBased_Neutral_PFOs() const;
    /// add a cellbased_neutral PFO to the tau
    void addCellBased_Neutral_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_neutral PFOs from the tau
    void clearCellBased_Neutral_PFOLinks();


    //pi0 PFOs
    const PFOLinks_t& cellBased_Pi0_PFOLinks() const;

    void setCellBased_Pi0_PFOLinks( const PFOLinks_t& cellBased_Pi0_PFOs );
    /// Get the pointer to a given cellbased_pi0 PFO associated with this tau
    const PFO* cellBased_Pi0_PFO( size_t i ) const;
    /// Get the number of cellbased_pi0 PFO particles associated with this tau
    size_t nCellBased_Pi0_PFOs() const;
    /// add a cellbased_pi0 PFO to the tau
    void addCellBased_Pi0_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all cellbased_pi0 PFOs from the tau
    void clearCellBased_Pi0_PFOLinks();




    ///eflowRec pfos
    //
    //eflowRec_charged PFOs
    const PFOLinks_t& eflowRec_Charged_PFOLinks() const;

    void setEflowRec_Charged_PFOLinks( const PFOLinks_t& eflowRec_Charged_PFOs );
    /// Get the pointer to a given eflowRec_charged PFO associated with this tau
    const PFO* eflowRec_Charged_PFO( size_t i ) const;
    /// Get the number of eflowRec_charged PFO particles associated with this tau
    size_t nEflowRec_Charged_PFOs() const;
    /// add a eflowRec_charged PFO to the tau
    void addEflowRec_Charged_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all eflowRec_charged PFOs from the tau
    void clearEflowRec_Charged_PFOLinks();


    //eflowRec_neutral PFOs
    const PFOLinks_t& eflowRec_Neutral_PFOLinks() const;

    void setEflowRec_Neutral_PFOLinks( const PFOLinks_t& eflowRec_Neutral_PFOs );
    /// Get the pointer to a given eflowRec_neutral PFO associated with this tau
    const PFO* eflowRec_Neutral_PFO( size_t i ) const;
    /// Get the number of eflowRec_neutral PFO particles associated with this tau
    size_t nEflowRec_Neutral_PFOs() const;
    /// add a eflowRec_neutral PFO to the tau
    void addEflowRec_Neutral_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all eflowRec_neutral PFOs from the tau
    void clearEflowRec_Neutral_PFOLinks();


    //pi0 PFOs
    const PFOLinks_t& eflowRec_Pi0_PFOLinks() const;

    void setEflowRec_Pi0_PFOLinks( const PFOLinks_t& eflowRec_Pi0_PFOs );
    /// Get the pointer to a given eflowRec_pi0 PFO associated with this tau
    const PFO* eflowRec_Pi0_PFO( size_t i ) const;
    /// Get the number of eflowRec_pi0 PFO particles associated with this tau
    size_t nEflowRec_Pi0_PFOs() const;
    /// add a eflowRec_pi0 PFO to the tau
    void addEflowRec_Pi0_PFOLink( const ElementLink< PFOContainer >& pfo);
    /// Remove all eflowRec_pi0 PFOs from the tau
    void clearEflowRec_Pi0_PFOLinks();

   
    /// @}
    

    void dump() const;


  }; // class TauJet

} // namespace xAOD

// Declare IParticle as a base class of TauJet_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::TauJet_v1, xAOD::IParticle );

#endif // XAODTAU_VERSIONS_TAUJET_V1_H

//  LocalWords:  newROIWord
