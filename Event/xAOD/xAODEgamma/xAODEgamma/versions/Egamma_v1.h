// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Egamma_v1.h 612532 2014-08-19 14:07:39Z christos $
#ifndef XAODEGAMMA_VERSIONS_EGAMMA_V1_H
#define XAODEGAMMA_VERSIONS_EGAMMA_V1_H

// Core include(s):
#include "AthLinks/ElementLink.h"

// ROOT include(s):
#include <Math/SMatrix.h>

// xAOD include(s):
#include "xAODBase/IParticle.h"

//Egamma includes
#include "xAODEgamma/EgammaDefs.h"
#include "xAODEgamma/EgammaEnums.h"
//CaloCluster include
#include  "xAODCaloEvent/CaloCluster.h" 
#include  "xAODCaloEvent/CaloClusterContainer.h" 

#include <stdint.h>

//Eigen/Amg includes , prefer to do it via EventPrimitives for now
#include "EventPrimitives/EventPrimitives.h"

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
  /// $Revision: 612532 $
  /// $Date: 2014-08-19 16:07:39 +0200 (Tue, 19 Aug 2014) $
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
    virtual double           pt() const  /*override*/;

    /// @brief The pseudorapidity (\f$\eta\f$) of the particle
    virtual double           eta() const /*override*/;

    /// @brief The azimuthal angle (\f$\phi\f$) of the particle
    virtual double           phi() const /*override*/;

    /// @brief The invariant mass of the particle
    virtual double           m() const /*override*/;

    /// The total energy of the particle
    virtual double           e() const  /*override*/;

    /// @brief The true rapidity (y) of the particle
    virtual double           rapidity() const /*override*/;
    
    /// @brief Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// @brief The full 4-momentum of the particle as a TLoretzVector
    virtual const FourMom_t& p4() const /*override*/;

    /// @brief The type of the object as a simple enumeration, remains pure virtual in e/gamma.
    virtual Type::ObjectType type() const /*override*/ =0 ; 
    
    /// @}


    /// @name xAOD::Egamma 4-Mom functions
    /// @{
    
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
    typedef Eigen::Matrix<float,4,4>    EgammaCovMatrix_t;

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

   
    /// @name xAOD::Egamma Shower shape and Isolation Accesors
    /// If 'information' is stored in this xAOD::Egamma and is of the correct type,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', and does not touch 'value'.
    ///
    /// @{    

    /// @brief Accessor for ShowerShape values.
    bool showerShapeValue(float& value,   const EgammaParameters::ShowerShapeType information) const;

    /// @brief Set method for Shower Shape values.
    bool setShowerShapeValue(float& value, const EgammaParameters::ShowerShapeType information) ;

    /// @brief Accessor for Isolation values.
    bool isolationValue(float& value,   const EgammaParameters::IsolationType information) const;

    /// @method for Isolation values.
    bool setIsolationValue(float& value, const EgammaParameters::IsolationType information);

    /// @}
    
    /// @name xAOD::Egamma  object quality of the calorimeter cluster 
    /// @{    
    
    /// @brief  Check object quality
    bool isGoodOQ(uint32_t mask ) const;

    /// @brief Set the object quality
    void setOQ(uint32_t newOQ);
    
    ///@}

    /// @name xAOD::Egamma selector methods
    /// @{    

    /// @brief  Check if the egamma object pass a selection menu
    ///If the menu decision is stored in this xAOD::Egamma,
    /// then the function fills 'value' and returns 'true', otherwise it returns 'false', 
    //and does not touch 'value'.
    bool passSelection(bool& value, const std::string& menu ) const;

    /// @brief Set  if the egamma object pass a selection menu
    void setPassSelection(bool value, const std::string& menu);

    ///@}



  private:
    ///

    /// Cached 4-momentum object
    mutable FourMom_t m_p4;
    ///

    /// Cache state of the internal 4-momentum (reset from the streamer)
    mutable bool m_p4Cached;

      
    
  }; // class Egamma


} // namespace xAOD

// Set up a CLID for the class:
#ifndef XAOD_STANDALONE
#include "SGTools/CLASS_DEF.h"
CLASS_DEF( xAOD::Egamma_v1 , 237874247 , 1 )
#endif // not XAOD_STANDALONE

#endif // XAODEGAMMA_VERSIONS_EGAMMA_V1_H
