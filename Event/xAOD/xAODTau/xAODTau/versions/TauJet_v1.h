// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: TauJet_v1.h 596235 2014-05-09 13:12:18Z janus $
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

namespace xAOD {

	/* define type traits for xAOD Tau.
	 * needs to be outside of the TauJet namespace
	 */
	// empty return type for all types T.
	// Leads to a compilation failure if one tries to use it with other types then a specialization is given for.
	template <class T>
	struct xAODTAU_return_type;

	// specialization for ints
	template <>
	struct xAODTAU_return_type<int> { typedef int type; };

	// specialization for floats
	template <>
	struct xAODTAU_return_type<float> { typedef float type; };


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
    virtual const FourMom_t& p4() const;
    virtual const FourMom_t& p4(const TauJetParameters::TauCalibType calib) const;


    /// Set methods for IParticle values
    void setP4(double pt, double eta, double phi, double m);
    bool setP4(const TauJetParameters::TauCalibType calib, double pt, double eta, double phi, double m);


    /// direct setters for 4 momentum components
    //void setPt(double pt);
    //void setEta(double eta);
    //void setPhi(double phi);
    //void setE(double e);
    //void setM(double m);

    
    //setters and getters for the individual 4-vector components at different calibration levels
    double ptJetSeed() const;
    double etaJetSeed() const;
    double phiJetSeed() const;
    //double eJetSeed() const;
    double mJetSeed() const;

//     void setPtJetSeed(double pt);
//     void setEtaJetSeed(double eta);
//     void setPhiJetSeed(double phi);
//     void setEJetSeed(double e);
//     void setMJetSeed(double m);

    double ptDetectorAxis() const;
    double etaDetectorAxis() const;
    double phiDetectorAxis() const;
    //double eDetectorAxis() const;
    double mDetectorAxis() const;

//     void setPtDetectorAxis(double pt);
//     void setEtaDetectorAxis(double eta);
//     void setPhiDetectorAxis(double phi);
//     void setEDetectorAxis(double e);
//     void setMDetectorAxis(double m);

    double ptIntermediateAxis() const;
    double etaIntermediateAxis() const;
    double phiIntermediateAxis() const;
    //double eIntermediateAxis() const;
    double mIntermediateAxis() const;

//     void setPtIntermediateAxis(double pt);
//     void setEtaIntermediateAxis(double eta);
//     void setPhiIntermediateAxis(double phi);
//     void setEIntermediateAxis(double e);
//     void setMIntermediateAxis(double m);

    double ptTauEnergyScale() const;
    double etaTauEnergyScale() const;
    double phiTauEnergyScale() const;
    //double eTauEnergyScale() const;
    double mTauEnergyScale() const;

//     void setPtTauEnergyScale(double pt);
//     void setEtaTauEnergyScale(double eta);
//     void setPhiTauEnergyScale(double phi);
//     void setETauEnergyScale(double e);
//     void setMTauEnergyScale(double m);


    double ptTauEtaCalib() const;
    double etaTauEtaCalib() const;
    double phiTauEtaCalib() const;
    //double eTauEtaCalib() const;
    double mTauEtaCalib() const;

//     void setPtTauEtaCalib(double pt);
//     void setEtaTauEtaCalib(double eta);
//     void setPhiTauEtaCalib(double phi);
//     void setETauEtaCalib(double e);
//     void setMTauEtaCalib(double m);

    double ptPanTauEFlowRecProto() const;
    double etaPanTauEFlowRecProto() const;
    double phiPanTauEFlowRecProto() const;
    //double ePanTauEFlowRecProto() const;
    double mPanTauEFlowRecProto() const;

//     void setPtPanTauEFlowRecProto(double pt);
//     void setEtaPanTauEFlowRecProto(double eta);
//     void setPhiPanTauEFlowRecProto(double phi);
//     void setEPanTauEFlowRecProto(double e);
//     void setMPanTauEFlowRecProto(double m);

    double ptPanTauEFlowRec() const;
    double etaPanTauEFlowRec() const;
    double phiPanTauEFlowRec() const;
    //double ePanTauEFlowRec() const;
    double mPanTauEFlowRec() const;

//     void setPtPanTauEFlowRec(double pt);
//     void setEtaPanTauEFlowRec(double eta);
//     void setPhiPanTauEFlowRec(double phi);
//     void setEPanTauEFlowRec(double e);
//     void setMPanTauEFlowRec(double m);


    double ptPanTauCellBasedProto() const;
    double etaPanTauCellBasedProto() const;
    double phiPanTauCellBasedProto() const;
    //double ePanTauCellBasedProto() const;
    double mPanTauCellBasedProto() const;

//     void setPtPanTauCellBasedProto(double pt);
//     void setEtaPanTauCellBasedProto(double eta);
//     void setPhiPanTauCellBasedProto(double phi);
//     void setEPanTauCellBasedProto(double e);
//     void setMPanTauCellBasedProto(double m);

    double ptPanTauCellBased() const;
    double etaPanTauCellBased() const;
    double phiPanTauCellBased() const;
    //double ePanTauCellBased() const;
    double mPanTauCellBased() const;

//     void setPtPanTauCellBased(double pt);
//     void setEtaPanTauCellBased(double eta);
//     void setPhiPanTauCellBased(double phi);
//     void setEPanTauCellBased(double e);
//     void setMPanTauCellBased(double m);
                

    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    
    /// @}

    // the summed up charge of all tau tracks
    float charge() const;

    /// the ROIWord, in case TauJet is used in EF trigger
    unsigned int ROIWord() const;

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


    

    //comment out for now, awaiting decision on whether enum-based accessors are sufficient
    // //details accessors
    // float ipZ0SinThetaSigLeadTrk() const;
    // float etOverPtLeadTrk() const;
    // float leadTrkPt() const;
    // float ipSigLeadTrk() const;
    // float ipSigLeadLooseTrk() const;
    // float etOverPtLeadLooseTrk() const;
    // float leadLooseTrkPt() const;
    // float chrgLooseTrk() const;
    // float massTrkSys() const;
    // float trkWidth2() const;
    // float trFlightPathSig() const;
    // float etEflow() const;
    // float mEflow() const;
    // /// name accessors for electron LLH variables
    // float ele_E237E277() const;
    // float ele_PresamplerFraction() const;
    // float ele_ECALFirstFraction() const;
    // //name accessors for topocluster ID variables
    // int numCells() const;
    // /// get number of topocluster constituents of jet associated to tau candidate
    // int numTopoClusters() const;
    // /// get number of effective topocluster constituents of jet associated to tau candidate !!! is a std::vector< float >
    // float numEffTopoClusters() const;
    // /// get invariant mass from topoclusters of jet associated to tau candidate 
    // float topoInvMass() const;
    // /// get invariant mass from effective topoclusters of jet associated to tau candidate 
    // float effTopoInvMass() const;
    // /// get mean distance to leading topocluster for topoclusters of jet associated to tau candidate
    // float topoMeanDeltaR() const;
    // /// get mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
    // float effTopoMeanDeltaR() const;
    // /////////////////////////////////////////////////////////////////////
    // //
    // // Applicable for calo seeded tau reconstruction
    // //
    // /////////////////////////////////////////////////////////////////////
    // /// Get E_T radius
    // float seedCalo_EMRadius() const;
    // /// Get hadron calorimeter radius
    // float seedCalo_hadRadius() const;
    // /// Get EM energy at EM scale
    // float seedCalo_etEMAtEMScale() const;
    // /// Get Hadronic energy at EM scale
    // float seedCalo_etHadAtEMScale() const;
    // /// Get isolation fraction
    // float seedCalo_isolFrac() const;
    // /// Get centrality fraction
    // float seedCalo_centFrac() const;
    // /// Get strip width ^2
    // float seedCalo_stripWidth2() const;
    // /// Get number of strips
    // int seedCalo_nStrip() const;
    // /// Get calibrated EM transverse energy
    // float seedCalo_etEMCalib() const;
    // /// Get calibrated hadronic transverse energy
    // float seedCalo_etHadCalib() const;
    // /// Get eta of TauJet calculated from calorimeter
    // float seedCalo_eta() const;
    // /// Get phi of TauJet calculated from calorimeter
    // float seedCalo_phi() const;
    // /// Get number of isolated tracks
    // float seedCalo_nIsolLooseTrk() const;
    // /// Get the average track distance to calorimeter seed
    // float seedCalo_trkAvgDist() const;
    // /// Get the RMS of track distance to calorimeter seed
    // float seedCalo_trkRmsDist() const;
    // ///  Get sum of 2 leading cluster energy over all cluster energy
    // float seedCalo_lead2ClusterEOverAllClusterE() const;
    // ///  Get sum of 3 leading cluster energy over all cluster energy
    // float seedCalo_lead3ClusterEOverAllClusterE() const;
    // ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
    // float seedCalo_caloIso() const;
    // ///  Get sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
    // float seedCalo_caloIsoCorrected() const;
    // ///  Get maximal dR of tracks associated to calo-seeded tau
    // float seedCalo_dRmax() const;
    
    // ///XXXXXdon't migrate seedTrk_ variables for now as they will probably become obsolete

    // /// EM+TES final scale
    // float EM_TES_scale() const;
    // /// LC+TES precalibration
    // float LC_TES_precalib() const;
 
    // //////////////////////////////////////////////////////////////////////
    // //
    // // Cell-based energies in rings getters/setters
    // //
    // //////////////////////////////////////////////////////////////////////
    // /// Ring 1: 0.00  < R < 0.05
    // float cellBasedEnergyRing1() const;
    // /// Ring 2: 0.05  < R < 0.075
    // float cellBasedEnergyRing2() const;
    // /// Ring 3: 0.075 < R < 0.10
    // float cellBasedEnergyRing3() const;
    // /// Ring 4: 0.10  < R < 0.125
    // float cellBasedEnergyRing4() const;
    // /// Ring 5: 0.125 < R < 0.15
    // float cellBasedEnergyRing5() const;
    // /// Ring 6: 0.15  < R < 0.2
    // float cellBasedEnergyRing6() const;
    // /// Ring 7: 0.2   < R < 0.4
    // float cellBasedEnergyRing7() const;
    // /// TRT hits high threshold over low threshold
    // float TRT_NHT_OVER_NLT() const;
    // /// @Tau Jet Vertex Fraction
    // float TauJetVtxFraction() const;

    

    
    // set the charge
    void setCharge(float);
    
    // set the ROIWord, needed by TauTrigger 
    void setROIWord(unsigned int);
    
    /// @}
    

    // //details setters
    // void setIpZ0SinThetaSigLeadTrk(float);
    // void setEtOverPtLeadTrk(float);
    // void setLeadTrkPt(float);
    // void setIpSigLeadTrk(float);
    // void setIpSigLeadLooseTrk(float);
    // void setEtOverPtLeadLooseTrk(float);
    // void setLeadLooseTrkPt(float);
    // void setChrgLooseTrk(float);
    // void setMassTrkSys(float);
    // void setTrkWidth2(float);
    // void setTrFlightPathSig(float);
    // void setEtEflow(float);
    // void setMEflow(float);
    // /// name accessors for electron LLH variables
    // void setEle_E237E277(float);
    // void setEle_PresamplerFraction(float);
    // void setEle_ECALFirstFraction(float);
    // //name accessors for topocluster ID variables
    // void setNumCells(int);
    // /// set number of topocluster constituents of jet associated to tau candidate
    // void setNumTopoClusters(int);
    // /// set number of effective topocluster constituents of jet associated to tau candidate !!! is a std::vector< float >
    // void setNumEffTopoClusters(float);
    // /// set invariant mass from topoclusters of jet associated to tau candidate 
    // void setTopoInvMass(float);
    // /// set invariant mass from effective topoclusters of jet associated to tau candidate 
    // void setEffTopoInvMass(float);
    // /// set mean distance to leading topocluster for topoclusters of jet associated to tau candidate
    // void setTopoMeanDeltaR(float);
    // /// set mean distance to leading topocluster for effective topoclusters of jet associated to tau candidate 
    // void setEffTopoMeanDeltaR(float);
    // /////////////////////////////////////////////////////////////////////
    // //
    // // Applicable for calo seeded tau reconstruction
    // //
    // /////////////////////////////////////////////////////////////////////
    // /// Set E_T radius
    // void setSeedCalo_EMRadius(float);
    // /// Set hadron calorimeter radius
    // void setSeedCalo_hadRadius(float);
    // /// Set EM energy at EM scale
    // void setSeedCalo_etEMAtEMScale(float);
    // /// Set Hadronic energy at EM scale
    // void setSeedCalo_etHadAtEMScale(float);
    // /// Set isolation fraction
    // void setSeedCalo_isolFrac(float);
    // /// Set centrality fraction
    // void setSeedCalo_centFrac(float);
    // /// Set strip width ^2
    // void setSeedCalo_stripWidth2(float);
    // /// Set number of strips
    // void setSeedCalo_nStrip(int);
    // /// Set calibrated EM transverse energy
    // void setSeedCalo_etEMCalib(float);
    // /// Set calibrated hadronic transverse energy
    // void setSeedCalo_etHadCalib(float);
    // /// Set eta of TauJet calculated from calorimeter
    // void setSeedCalo_eta(float);
    // /// Set phi of TauJet calculated from calorimeter
    // void setSeedCalo_phi(float);
    // /// Set number of isolated tracks
    // void setSeedCalo_nIsolLooseTrk(float);
    // /// Set the average track distance to calorimeter seed
    // void setSeedCalo_trkAvgDist(float);
    // /// Set the RMS of track distance to calorimeter seed
    // void setSeedCalo_trkRmsDist(float);
    // ///  Set sum of 2 leading cluster energy over all cluster energy
    // void setSeedCalo_lead2ClusterEOverAllClusterE(float);
    // ///  Set sum of 3 leading cluster energy over all cluster energy
    // void setSeedCalo_lead3ClusterEOverAllClusterE(float);
    // ///  Set sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  
    // void setSeedCalo_caloIso(float);
    // ///  Set sum of transvers energy of clusters around jet seed within 0.2 < dR < 0.4  (pile up corrected)
    // void setSeedCalo_caloIsoCorrected(float);
    // ///  Set maximal dR of tracks associated to calo-seeded tau
    // void setSeedCalo_dRmax(float);
    
    // ///XXXXXdon't migrate seedTrk_ variables for now as they will probably become obsolete

    // /// EM+TES final scale
    // void setEM_TES_scale(float);
    // /// LC+TES precalibration
    // void setLC_TES_precalib(float);
 
    // //////////////////////////////////////////////////////////////////////
    // //
    // // Cell-based energies in rings setters
    // //
    // //////////////////////////////////////////////////////////////////////
    // /// Ring 1: 0.00  < R < 0.05
    // void setCellBasedEnergyRing1(float);
    // /// Ring 2: 0.05  < R < 0.075
    // void setCellBasedEnergyRing2(float);
    // /// Ring 3: 0.075 < R < 0.10
    // void setCellBasedEnergyRing3(float);
    // /// Ring 4: 0.10  < R < 0.125
    // void setCellBasedEnergyRing4(float);
    // /// Ring 5: 0.125 < R < 0.15
    // void setCellBasedEnergyRing5(float);
    // /// Ring 6: 0.15  < R < 0.2
    // void setCellBasedEnergyRing6(float);
    // /// Ring 7: 0.2   < R < 0.4
    // void setCellBasedEnergyRing7(float);
    // /// TRT hits high threshold over low threshold
    // void setTRT_NHT_OVER_NLT(float);
    // /// @Tau Jet Vertex Fraction
    // void setTauJetVtxFraction(float);

    void dump();

   private:

    /// Cached 4-momentum object
    mutable FourMom_t m_p4;
    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;



  }; // class TauJet

} // namespace xAOD

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::TauJet_v1, 23821074, 1 )
#endif // not XAOD_STANDALONE

// Declare IParticle as a base class of TauJet_v1:
#include "AthContainers/DataVector.h"
DATAVECTOR_BASE( xAOD::TauJet_v1, xAOD::IParticle );

#endif // XAODTAU_VERSIONS_TAUJET_V1_H

//  LocalWords:  newROIWord
