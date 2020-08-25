// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Muon_v1.h 745098 2016-05-05 15:47:04Z wleight $
#ifndef XAODMUON_VERSIONS_MUON_V1_H
#define XAODMUON_VERSIONS_MUON_V1_H

// Core include(s):
#include "AthContainers/AuxElement.h"
#include "AthLinks/ElementLink.h"

//xAOD Primitives
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"

// Misc xAOD include(s):
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackingPrimitives.h" 
#include "xAODTracking/TrackParticleContainer.h" 
#include "xAODCaloEvent/CaloClusterContainer.h" 
#include "xAODMuon/MuonSegmentContainer.h" 

#include <bitset>
#include <stdint.h>

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {
  /// Class describing a Muon.
  ///
  /// @author Edward Moyse <Edward.Moyse@cern.ch>
  /// @nosubgrouping
  class Muon_v1 : public IParticle {

  public:
    /// Default constructor
    Muon_v1();
    
    /// Copy constructor
    Muon_v1(const Muon_v1& rhs);
    
    /// Destructor
    virtual ~Muon_v1();
    
    /// Assignment operator
    Muon_v1& operator=(const Muon_v1& rhs);
    
    /// @name IParticle functions
    /// @{
    /// The transverse momentum (\f$p_T\f$) of the particle.
    virtual double           pt() const;
    /// The pseudorapidity (\f$\eta\f$) of the particle.
    virtual double           eta() const;
    /// The azimuthal angle (\f$\phi\f$) of the particle.
    virtual double           phi() const;
    /// The invariant mass of the particle..
    virtual double           m() const;
    /// The total energy of the particle.
    virtual double           e() const;
    /// The true rapidity (y) of the particle.
    virtual double           rapidity() const;

    /// Set method for IParticle values
    void setP4(double pt, double eta, double phi);
        
    /// Definition of the 4-momentum type.
    typedef IParticle::FourMom_t FourMom_t;

    /// The full 4-momentum of the particle.
    virtual FourMom_t p4() const;
        
    /// Base 4 Momentum type for Muon
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : GenVector
    GenVecFourMom_t genvecP4() const; 

    /// The type of the object as a simple enumeration
    virtual Type::ObjectType type() const;
    /// @}

    /// @name Charge
    /// Returns the charge.
    float charge() const;    
    
    /// Set the charge (*must* be the same as primaryTrackParticle() )
    void setCharge(float charge);

    /// @name Author
    /// Methods to query the author(s) of this Muon
    /// @{
    /// @todo - update and add documentation.
    enum Author {
      unknown=0, 
      MuidCo,
      STACO, 
      MuTag, 
      MuTagIMO,
      MuidSA,
      MuGirl,
      MuGirlLowBeta,
      CaloTag,
      CaloLikelihood,
      CaloScore,
      ExtrapolateMuonToIP,
      NumberOfMuonAuthors // increase this guy when adding
    };
    /// Returns the primary author of this Muon.
    Author author() const;
    /// Returns 'true' if 'author' is the an author of this muon.
    bool isAuthor ( const Author author ) const;
    /// set author
    void setAuthor(Author auth);
    /// Get all the authors of this Muon.
    /// For example during overlap checking, the same Muon may have been reconstructed by many different algorithms. This method returns a 16bit
    /// number, where each bit represents a muon algorithm, defined as follows (the lowest bit is indicates that something has gone wrong):
    /// unknown | MuidCo | STACO | MuTag | MuTagIMO | MuidSA | MuGirl | MuGirlLowBeta | CaloTag | CaloLikelihood | CaloScore | ExtrapolateMuonToIP | MuonCombinedRefit | ExtrapolateMuonToIP        
    /// @returns  16-bit word, 1-bit reserved for each muon Algorithm: 
    uint16_t allAuthors() const;
    void setAllAuthors(uint16_t authors);
    /// add author to all authors
    void addAllAuthor( const Author author ); 
 
    /// @}

    /// @name Summary information
    /// Return summary information about the muon, such as its MuonType, and the TrackSumary values of the primary TrackParticle.
    /// @{        
    ///@todo Add documentation.
    enum MuonType {
      Combined, MuonStandAlone, SegmentTagged, CaloTagged, SiliconAssociatedForwardMuon
    };
    /// Generic accessor to type information.
    MuonType muonType() const;
    /// @todo - do we actually need this? Deduce it from other information?
    void setMuonType(MuonType type);

    /// Accessor for TrackSummary values (in most cases, retrieved from the 'primary' TrackParticle - though it could be stored on the Muon, depending on the 
    /// job configuration)
    /// If 'information' is stored in the primary TrackParticle/Muon and is of the correct templated type T, 
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'. 
    /// See below for an example of how this is intended to be used.
    /// @code
    /// uint8_t numberOfInnermostPixelLayerHits=0;
    /// if( myParticle.summaryValue<uint8_t>(numberOfInnermostPixelLayerHits,numberOfInnermostPixelLayerHits) ){
    ///   ATH_MSG_INFO("Successfully retrieved the integer value, numberOfInnermostPixelLayerHits");
    /// }
    /// float numberOfCscPhiHits=0.0; //Wrong! This is actually an int too.
    /// if( !myParticle.summaryValue<float>(numberOfCscPhiHits,numberOfCscPhiHits) ){
    ///   ATH_MSG_INFO("Types must match!"); 
    /// }
    /// @endcode
    /// @param[in] information The information being requested. This is not guaranteed to be stored in all Muons (or primary TrackParticle).
    /// @param[out] value  Only filled if this Muon (or its primary TrackParticle) contains 'information', and the types match.
    /// @return Returns 'true' if the Muon contains 'information', and its concrete type matches 'value' (templated type T).
    bool summaryValue(uint8_t& value, const SummaryType information) const;	     
    /// Set method for storing TrackSummary SummaryType information on the Muon (see Aux to see which is already defined as static)       
    void setSummaryValue(uint8_t value, const SummaryType information);  
    /// @copydoc bool summaryValue(uint8_t& value, const SummaryType information) const;
    bool summaryValue(float& value, const SummaryType information) const;        
    /// Accessor for MuonSummaryType.
    bool summaryValue(uint8_t& value, const MuonSummaryType information) const;
    /// Set method for MuonSummaryType.        
    void setSummaryValue(uint8_t value, const MuonSummaryType information); 
     
    /// Same as bool summaryValue(float& value, const SummaryType &information) const , but without check (will throw exception if value isn't there)
    /// Primarily for use in Python.
    float floatSummaryValue(const SummaryType information) const;
    /// Same as bool summaryValue(uint8_t& value, const SummaryType &information) const, but without check (will throw exception if value isn't there)
    /// Primarily for use in Python.
    uint8_t uint8SummaryValue(const SummaryType information) const;
    /// Same as bool summaryValue(uint8_t& value, const MuonSummaryType &information) const, but without check (will throw exception if value isn't there)
    /// Primarily for use in Python.
    float uint8MuonSummaryValue(const MuonSummaryType information) const;
	    
    /// Enum for parameter indexes 
    enum ParamDef {
      /** Discriminators and further variables */
      spectrometerFieldIntegral=0  , //<! B-field integral in MS
      scatteringCurvatureSignificance=1 , //<! Scattering angle significance: curvature
      scatteringNeighbourSignificance=2 , //<! Scattering angle significance: neighbours
      momentumBalanceSignificance=3, //<! momentum balance significance
        
      /** MuTag parameters */
      segmentDeltaEta=4 , 
      segmentDeltaPhi=5 ,
      segmentChi2OverDoF=6 ,
      /** MuGirl parameter */
      t0=7        ,
      beta=8      ,
      annBarrel=9 ,
      annEndCap=10,
      /** common MuGirl and MuTag parameters */
      innAngle=11 ,
      midAngle=12 ,
      msInnerMatchChi2=13, //!< The chi^2 for the match of Inner detector (ID) and standalone (SA) tracks at the entrance to the spectrometer (MS).
      msInnerMatchDOF=14, //!< The 'degrees of freedom' (DOF) for the match of Inner detector (ID) and standalone (SA) tracks at the entrance to the spectrometer (MS).
      msOuterMatchChi2=15, //!< The chi^2 for the match of Inner detector (ID) and standalone (SA) tracks at the exit of the spectrometer (MS).
      msOuterMatchDOF=16, //!< The 'degrees of freedom' (DOF) for the match of Inner detector (ID) and standalone (SA) tracks at the exit of the spectrometer (MS).
      meanDeltaADCCountsMDT=17, //!< Difference between mean number of ADC count for given track and mean number of ADC for all muons from DATA.
      /** CaloMuon variables (EnergyLossType is stored separately and retrieved using energyLossType() */
      CaloLRLikelihood=18, //!< Calo Muon ID likelihood
      CaloMuonIDTag=19, //!< Calo Muon Identification tag
      FSR_CandidateEnergy=20, //!< FSR candidate energy [MeV]
      EnergyLoss=21, //!< Fitted energy loss (either param or meas depending on track isolation and calo meas) [Mev]
      ParamEnergyLoss=22, //!< Parametrised energy loss [Mev]
      MeasEnergyLoss=23, //!< Measured energy loss [Mev]
      EnergyLossSigma=24, //!< Sigma of Measured or parametrised energy loss used in the track fit [Mev]
      ParamEnergyLossSigmaPlus=25, //!< Sigma plus of Parametrised energy loss [Mev]
      ParamEnergyLossSigmaMinus=26, //!< Sigma minus of Parametrised energy loss [Mev]
      MeasEnergyLossSigma=27, //!< Sigma of Measured energy loss [Mev]
      CaloMuonScore=28, //!< Calo Muon convolutional neural network ID score
    };
        
    /// Get a parameter for this Muon - momentumBalanceSignificance for example 
    /// @todo Finish documentation
    /// include matchChi2, muonentrancechi2 (instead of outerMatchChi2). Store chi2/dof instead of two?
    /// fitChi2 comes from TrackParticle.
    bool parameter(float& value, const ParamDef parameter) const;

    /// Set method for parameter values.
    void setParameter(float value, const ParamDef parameter);

    /// Same as bool parameter(float& value, const ParamDef &parameter) const, but without check (will throw exception if value isn't there). 
    /// Primarily for use in Python.
    float floatParameter(const ParamDef parameter) const;

    /// Get an integer parameter for this Muon - msInnerMatchDOF for example 
    bool parameter(int& value, const ParamDef parameter) const;

    /// Set method for parameter values.
    void setParameter(int value, const ParamDef parameter);

    /// Same as bool parameter(float& value, const ParamDef &parameter) const, but without check (will throw exception if value isn't there). 
    /// Primarily for use in Python.
    int intParameter(const ParamDef parameter) const;

    /// The Muon Quality information is defined on the MCP twiki: https://twiki.cern.ch/twiki/bin/view/Atlas/MuonSelectionTool#Quality_definition
    /// @todo Finish documentation
    enum Quality {Tight, /// Highest purity, but lower efficiency
                  Medium, /// Usual recommended working point - a good balance between purity and efficiency
                  Loose, /// Adds segment tagged and calo tagged muons.
                  VeryLoose}; /// Low purity.
    Quality quality() const;
    void setQuality(Quality);
          
    /// Returns true if this Muon passes the MCP ID hit cuts (see the MCP twiki for definitions: 
    /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MuonPerformance)
    bool passesIDCuts() const;

    /// Set whether passes the MCP ID hit cuts.
    void setPassesIDCuts(bool);

    /// Returns true if this Muon passes the MCP high pT cuts (see the MCP twiki for definitions: 
    /// https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MuonPerformance)
    bool passesHighPtCuts() const;

    /// Set whether passes the MCP ID hit cuts.
    void setPassesHighPtCuts(bool);
              
    /// @}
	
    /// @name Isolation information.
    /// 
    /// @{   

    /// @brief Accessor for Isolation values. 
    /// If 'information' is stored in this xAOD::Muon and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    bool isolation(float& value,   const Iso::IsolationType information) const;

    /// Accessor to Isolation values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolation(const Iso::IsolationType information) const;

    /// Set method for Isolation values.
    void setIsolation(float value, const Iso::IsolationType information);
    /// @}

    /// @brief Accessor for Isolation Calo correction.
    /// If 'information' is stored in this xAOD::Muon and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    bool isolationCaloCorrection(float& value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection type,
                                 const Iso::IsolationCorrectionParameter param) const;

    /// Accessor to Isolation Calo corrections , this just returns the correction without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationCaloCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection type,
                                  const Iso::IsolationCorrectionParameter param) const;

    /// set method for Isolation Calo Corrections.
    bool setIsolationCaloCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection type,
                                    const Iso::IsolationCorrectionParameter param);


    /// @brief Accessor for Isolation Track correction.
    bool isolationTrackCorrection(float& value, const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection type ) const;

    /// Accessor to Isolation Track corrections , this just returns the correction without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationTrackCorrection(const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection type) const;

    /// Set method for Isolation Track Corrections.
    bool setIsolationTrackCorrection(float value, const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection type);


    /// @brief Accessor for Isolation corection Bitset
    bool isolationCorrectionBitset(std::bitset<32>& value, const Iso::IsolationFlavour flavour ) const;

    /// Accessor to Isolation corection Bitset , this just returns the bitset without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    std::bitset<32> isolationCorrectionBitset(const Iso::IsolationFlavour flavour ) const;

    /// Set method for Isolation corection Bitset.
    bool setIsolationCorrectionBitset(uint32_t value, const Iso::IsolationFlavour flavour );

    /// @}

    /// @name Links
    /// With the following methods you can retrieve links to the objects used to identify this muon - depending on how the muon was built the link may
    /// or may not be valid (i.e. a muon built from a standalone MS track won't have an ID TrackParticle associated to it). 
    /// @todo finish documentation
    /// @code
    /// Add some code here, showing how to properly use the element links
    /// @endcode
    /// @note Some links were removed from the "Run-1" AOD::muon, in particular 
    /// @{
    /// @brief Returns an ElementLink  to the primary TrackParticle corresponding to the MuonType of this muon. This is determined in the following order:
    ///  1. CombinedTrackParticle
    ///  2. InnerDetectorTrackParticle
    ///  3. ExtrapolatedMuonSpectrometerTrackParticle
    ///  4. MSOnlyExtrapolatedMuonSpectrometerTrackParticle
    ///  5. MuonSpectrometerTrackParticle
    /// This method can throw a std::runtime_error exception if either the 'muontype' is unknown, or if the type is MuonStandAlone, 
    /// but there is no available extrapolatedMuonSpectrometerTrackParticleLink or muonSpectrometerTrackParticleLink to return.
    const ElementLink< TrackParticleContainer >& primaryTrackParticleLink() const;
		
    /// @brief Returns a pointer (which should not usually be NULL, but might be if the muon has been stripped of information) to the
    /// primary TrackParticle corresponding to the MuonType of this muon.
    ///This is determined in the following order:
    ///  1. CombinedTrackParticle
    ///  2. InnerDetectorTrackParticle
    ///  3. ExtrapolatedMuonSpectrometerTrackParticle
    ///  4. MSOnlyExtrapolatedMuonSpectrometerTrackParticle
    ///  5. MuonSpectrometerTrackParticle    
    const TrackParticle* primaryTrackParticle() const; 

    /// @brief Returns an ElementLink to the InnerDetector TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& inDetTrackParticleLink() const;
    /// @brief Returns an ElementLink to the InnerDetector TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& muonSpectrometerTrackParticleLink() const;
    /// @brief Returns an ElementLink to the InnerDetector TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& combinedTrackParticleLink() const;
    /// @brief Returns an ElementLink to the Extrapolated Muon Spectrometer TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& extrapolatedMuonSpectrometerTrackParticleLink() const;
    /// @brief Returns an ElementLink to the MS-only Extrapolated Muon Spectrometer TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& msOnlyExtrapolatedMuonSpectrometerTrackParticleLink() const;
        
    enum TrackParticleType {
      Primary, InnerDetectorTrackParticle, MuonSpectrometerTrackParticle, CombinedTrackParticle, ExtrapolatedMuonSpectrometerTrackParticle, MSOnlyExtrapolatedMuonSpectrometerTrackParticle
    };
    /// @brief Returns an ElementLink to the  TrackParticle used in identification of this muon.
    const ElementLink< TrackParticleContainer >& trackParticleLink( TrackParticleType type) const;
    /// @brief Set method for TrackParticle links.
    void setTrackParticleLink(TrackParticleType type, const ElementLink< TrackParticleContainer >& link);
    /// @brief Returns a pointer (which can be NULL) to the  TrackParticle used in identification of this muon.
    const TrackParticle* trackParticle( TrackParticleType type) const; 
       
    /// @brief Returns an ElementLinkto the cluster associated to this muon.
    ///@todo Why just one?
    const ElementLink<CaloClusterContainer>& clusterLink() const; 
    /// @brief Set method for cluster links.
    void setClusterLink(const ElementLink<CaloClusterContainer>& link);
    /// Retrieve the associated cluster with a bare pointer
    const CaloCluster* cluster() const;
        
    /// Defines how the energy loss was handled for this muon 
    enum EnergyLossType { Parametrized=0, 
      NotIsolated=1,  //!< Reconstruction configured to use the parametrization w/o looking in the calo (eg calo off)
      MOP=2,          //!< Measurement found to be compatible with most probable value --> mop used as more reliable at this region of the eloss
      Tail=3,         //!< Measured eloss significantly higher than mop --> the calo measurement used
      FSRcandidate=4  //!< In standalone reconstruction the Tail option was used. but an imbalance is observed when comparing Pstandalone and Pinnerdetector (Pstandalone>Pinnerdetector) --> if using the mop resolves the imbalance the excess energy loss is stored as fsrEnergy and the mop is used as the eloss.
    };
    /** Energy determined from parametrization or not (measured). The actual energy loss is returned via     
    @code
    float etCore;
    bool hasEnergyLoss = parameter(float& value, const ParamDef parameter)    
    @endcode
    */
    EnergyLossType energyLossType (void) const;
    /// Set method for the type
    void setEnergyLossType (EnergyLossType type) ;
        
    ///@todo complete the various calo energy additions (i.e. depositInCalo etc)

    /// @brief Returns a vector of ElementLinks to the MuonSegments used to create this Muon.
    const std::vector< ElementLink< MuonSegmentContainer > > & muonSegmentLinks() const;
    /// @brief Set the vector of ElementLinks to the MuonSegments used to create this Muon.
    void setMuonSegmentLinks(const std::vector< ElementLink< MuonSegmentContainer > >& segments) ;
    /// @brief Number of MuonSegments linked to by this Muon.
    size_t nMuonSegments() const;
    /// @brief Returns a pointer to the specified MuonSegment. 
    /// @param i Index of the MuonSegment requested. If i is not in range (0<i<nMuonSegments()) an exception will be thrown.
    const MuonSegment* muonSegment( size_t i ) const;
    /// @brief Returns a link to the specified MuonSegment. 
    /// @param i Index of the MuonSegment requested. If i is not in range (0<i<nMuonSegments()) an exception will be thrown.
    const ElementLink< MuonSegmentContainer >& muonSegmentLink( size_t i ) const;
        
    /// @}
  }; // class xAOD::Muon

} // namespace xAOD

// Set up a CLID for the class:
#include "xAODCore/CLASS_DEF.h"
CLASS_DEF( xAOD::Muon_v1, 239636954, 1 )

  // Declare IParticle as a base class of Muon_v1:
#include "AthContainers/DataVector.h"
  DATAVECTOR_BASE( xAOD::Muon_v1, xAOD::IParticle );

#endif // XAODMUON_VERSIONS_Muon_v1_H
