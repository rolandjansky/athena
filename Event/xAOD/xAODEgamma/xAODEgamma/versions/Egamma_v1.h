// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODEGAMMA_VERSIONS_EGAMMA_V1_H
#define XAODEGAMMA_VERSIONS_EGAMMA_V1_H

// Core include(s):
#include "AthLinks/ElementLink.h"

// xAOD include(s):
#include "xAODBase/IParticle.h"

//Egamma includes
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaEnums.h"

//CaloCluster include
#include  "xAODCaloEvent/CaloCluster.h"
#include  "xAODCaloEvent/CaloClusterContainer.h"

//xAOD Primitives
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationFlavour.h"

//Eigen/Amg includes , prefer to do it via EventPrimitives for now
#include "EventPrimitives/EventPrimitives.h"

//std Include
#include <stdint.h>

// ROOT include(s):
#include "Math/Vector4D.h"

//Already include the DataVector specialization for this type
#include "xAODEgamma/EgammaContainerFwd.h"


namespace xAOD {

  /// @class xAOD::Egamma
  /// @brief  Class describing an e/gamma
  /// @name xAOD::Egamma provides a  public interface.
  /// @name xAOD::Electron and xAOD::Photon inherit from this class
  ///
  /// @author Christos Anastopoulos
  /// @author Anthony Morley
  /// @author Jovan Mitrevski
  ///
  /// $Revision: 788131 $
  /// $Date: 2016-12-05 17:24:11 +0100 (Mon, 05 Dec 2016) $
  ///
  class Egamma_v1 :public IParticle {

  protected:
    /// @name xAOD::Egamma constructors
    /// The xAOD::Egamma is not supposed to be created directly, only via xAOD::Electron and xAOD::Photon.
    /// xAOD::Egamma is an abstract class. It does not define the type() pure virtual function from IParticle
    /// The default constructors is protected

    /// @{

    /// Default constructor
    Egamma_v1();

    /// Copy constructor (default implementation)
    Egamma_v1(const Egamma_v1& eg );
    /// @}

  public:

    /// @name xAOD::Egamma Assignment operator
    /// @{

    /// Assignment Operator. Using the assignment of SG::AuxElement.
    Egamma_v1& operator=(const Egamma_v1& eg);

    /// @}


    /// @name xAOD::IParticle functions
    /// These are already virtual due to IParticle
    /// @{

    /// @brief The transverse momentum (\f$p_T\f$) of the particle
    virtual double           pt() const  final;

    /// @brief The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const final;

    /// @brief The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const final;

    /// @brief The invariant mass of the particle
    virtual double           m() const final;

    /// The total energy of the particle
    virtual double           e() const  final;

    /// @brief The true rapidity (y) of the particle
    virtual double           rapidity() const final;

    /// @brief Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// @brief The full 4-momentum of the particle as a TLoretzVector
    virtual FourMom_t         p4() const final;

    /// @brief The type of the object as a simple enumeration, remains pure virtual in e/gamma.
    virtual Type::ObjectType type() const override =0 ;

    /// @}


    /// @name xAOD::Egamma 4-Mom functions
    /// @{

    /// Base 4 Momentum type for egamma
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    ///  The full 4-momentum of the particle : internal egamma type.
    GenVecFourMom_t genvecP4() const;

    /// @brief set the 4-vec
    void setP4(float pt, float eta, float phi, float m);

    /// @brief set the Pt
    void setPt(float pt);

    ///  @brief set the eta
    void setEta(float eta);

    /// @brief set the phi
    void setPhi(float phi);

    /// @brief set the Mass
    void setM(float m);

    /// @}

    /// @name xAOD::Egamma 4x4 Covariance Matrix
    /// @{

    ///4x4 Covariance Matrix in EtEtaPhiM (needs decision)
    typedef Eigen::Matrix<float,4,4>  EgammaCovMatrix_t;

    /// Returns the 4x4 symmetric covariance matrix .
    EgammaCovMatrix_t covMatrix() const;

    /// set the 4x4 symmetric covariance matrix .
    void setCovMatrix(const EgammaCovMatrix_t& cov);
    /// @}

    /// @name xAOD::Egamma Pointer to CaloClusters
    /// @{

    /// @brief Return the number of xAOD::CaloClusters that define the electron
    ///        candidate
    size_t nCaloClusters() const;

    /// @brief  Pointer to the xAOD::CaloCluster/s that define the electron
    ///         candidate
    const xAOD::CaloCluster* caloCluster( size_t index = 0 ) const;

    /// @brief ElementLink to the xAOD::CaloCluster/s that match the electron
    ///        candidate
    const ElementLink< CaloClusterContainer >&
    caloClusterLink( size_t index = 0 ) const;

    /// Helper type definition
    typedef std::vector< ElementLink< CaloClusterContainer > > CLELVec_t;

    /// @brief Get all cluster links
    const CLELVec_t& caloClusterLinks() const;
    /// @brief set Pointer to the xAOD::CaloCluster
    void setCaloClusterLinks( const CLELVec_t& links );

    /// @}


    /// @name xAOD::Egamma author (i.e. which reco algorithm was used)
    /// @{

     /// @brief Get author
    uint16_t author(uint16_t bitmask=EgammaParameters::AuthorALL) const;

    /// @brief add author
    void addAuthor( uint16_t );

    /// @brief set author
    void setAuthor( uint16_t );
    /// @}

    /// @name xAOD::Egamma ambiguousObject i.e get the electron/photon corresponding to an ambiguous photon/electron
    /// as the same cluster leads to creation of both.
    /// @{

    /// @brief Get ambiguous
    const Egamma_v1* ambiguousObject() const;
    /// @}


    /// @name xAOD::Egamma Shower shape  Accesors
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    ///
    /// @{

    /// @brief Accessor for ShowerShape values.
    bool showerShapeValue(float& value,const EgammaParameters::ShowerShapeType information) const;

    /// @brief Accessor to ShowerShape values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the variable is not available
    float showerShapeValue(const EgammaParameters::ShowerShapeType information) const;

    /// @brief Set method for Shower Shape values.
    bool setShowerShapeValue(float value, const EgammaParameters::ShowerShapeType information) ;

    /// @}


    /// @name xAOD::Egamma  object quality of the calorimeter cluster
    /// @{

    /// @brief  Check object quality. Return True is it is Good Object Quality
    bool isGoodOQ(uint32_t mask ) const;

    /// @brief  Return the object quality bit word
    uint32_t OQ( ) const;

    /// @brief Set the object quality
    void setOQ(uint32_t newOQ);

    ///@}

    /// @name xAOD::Egamma Isolation value Accesors
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    ///
    /// @{

    /// @brief Accessor for Isolation values.
    bool isolation(float& value,   const Iso::IsolationType information) const;

    /// @brief Accessor to Isolation values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolation(const Iso::IsolationType information) const;

    /// @brief set method for Isolation values.
    bool setIsolation(float value, const Iso::IsolationType information);


    /// @brief old Accessor for Isolation values.
    bool isolationValue(float& value,   const Iso::IsolationType information) const{

      return isolation(value,information);
    }
    /// @brief old Accessor to Isolation values , this just returns the value without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationValue(const Iso::IsolationType information) const{

      return isolation(information);
    }
    /// @brief old set method for Isolation values.
    bool setIsolationValue(float value, const Iso::IsolationType information){
      return setIsolation(value,information);
    }

    /// @}

    /// @name xAOD::Egamma Isolation correction  Accesors
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    ///
    /// @{

    /// @brief Accessor for flavour and type depended Isolation Calo correction.
    bool isolationCaloCorrection(float& value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr,
				 const Iso::IsolationCorrectionParameter param) const;

    /// @brief Accessor for flavour and type depended Isolation Calo corrections , this just returns the correction without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationCaloCorrection(const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr,
				  const Iso::IsolationCorrectionParameter param) const;

    /// @brief set method for flavour and type depended Isolation Calo Corrections.
    bool setIsolationCaloCorrection(float value, const Iso::IsolationFlavour flavour, const Iso::IsolationCaloCorrection corr,
				    const Iso::IsolationCorrectionParameter param);

    /// @brief Accessor  for type depended Isolation Calo correction.
    bool isolationCaloCorrection(float& value,  Iso::IsolationType type , Iso::IsolationCaloCorrection corr) const;

    /// @brief Accessor for type depended Isolation Calo corrections , this just returns the correction without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationCaloCorrection(Iso::IsolationType type, Iso::IsolationCaloCorrection corr) const;

    /// @brief set method for type depended Isolation Calo Corrections.
    bool setIsolationCaloCorrection(float value, Iso::IsolationType type, Iso::IsolationCaloCorrection corr);

    /// @brief Accessor for Isolation Track correction.
    bool isolationTrackCorrection(float& value, const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection corr ) const;

    /// @brief Accessor to Isolation Track corrections , this just returns the correction without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    float isolationTrackCorrection(const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection corr) const;

    /// @brief Set method for Isolation Track Corrections.
    bool setIsolationTrackCorrection(float value, const Iso::IsolationFlavour flavour , const Iso::IsolationTrackCorrection corr);


    /// @brief Accessor for Isolation corection Bitset
    bool isolationCorrectionBitset(std::bitset<32>& value, const Iso::IsolationFlavour flavour ) const;

    /// @brief Accessor to Isolation corection Bitset , this just returns the bitset without internaly checking if it exists.
    /// Will lead to an exception if the information is not available
    std::bitset<32> isolationCorrectionBitset(const Iso::IsolationFlavour flavour ) const;

    /// @brief Set method for Isolation corection Bitset.
    bool setIsolationCorrectionBitset(uint32_t value, const Iso::IsolationFlavour flavour );

    /// @}




    /// @name xAOD::Egamma selector / isEM methods using the menu name
    /// @{

    /// @brief  Check if the egamma object pass a selection menu (using the name)
    ///If the menu decision is stored in this xAOD::Egamma,
    ///then the function fills 'value' with the decision (reference)
    ///and returns 'true', otherwise it returns 'false',
    /// and does not touch 'value'.
    bool passSelection(bool& value, const std::string& menu ) const;

    /// @brief  Check if the egamma object pass a selection menu (using the name)
    /// If the particular menu decision is not stored in this xAOD::Egamma,
    /// an exception will occur
    bool passSelection( const std::string& menu ) const;

    /// @brief Set the selection decision for a  menu (using the name)
    void setPassSelection(bool value, const std::string& menu);

    /// @brief Return the isEM  word for a selection menu
    ///If the menu isEM is stored in this xAOD::Egamma,
    ///then the function fills 'value' with the isEM (reference)
    ///and returns 'true', otherwise it returns 'false',
    ///and does not touch 'value'.
    bool selectionisEM(unsigned int&  value, const std::string& isEM) const;

    /// @brief Return the isEM word for a  selection menu (using the name)
    /// If the particular isEM word is not stored in this xAOD::Egamma,
    /// an exception will occur
    unsigned int selectionisEM(const std::string& isEM) const;

    /// @brief Set the isEM word for a selection menu (using the name)
    void setSelectionisEM(unsigned int value, const std::string& isEM);

    /// @brief Return the LH value as float
    ///If the LH decision is stored in this xAOD::Egamma,
    ///then the function fills 'value' with the decision (reference)
    ///and returns 'true', otherwise it returns 'false',
    ///and does not touch 'value'.
    bool likelihoodValue(float&  value, const std::string& LHValue=std::string("LHValue")) const;

    /// @brief Return the LH value as float
    /// If the LH Value  is not stored in this xAOD::Egamma,
    /// an exception will occur
    float likelihoodValue(const std::string& LHValue=std::string("LHValue")) const;

    /// @brief Set the LHValue as float
    void setLikelihoodValue(float value, const std::string& LHValue=std::string("LHValue"));

    ///@}


  }; // class Egamma


} // namespace xAOD

//Finish declaration of IParticle as a base class of Egamma
DATAVECTOR_BASE_FIN(xAOD::Egamma_v1, xAOD::IParticle);

#endif // XAODEGAMMA_VERSIONS_EGAMMA_V1_H
