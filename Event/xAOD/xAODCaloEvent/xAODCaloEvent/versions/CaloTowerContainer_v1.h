// -*- c++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAODCALOEVENT_VERSIONS_CALOTOWERCONTAINER_V1_H
#define XAODCALOEVENT_VERSIONS_CALOTOWERCONTAINER_V1_H

#include "AthContainers/DataVector.h"
#include "xAODBase/IParticleContainer.h"
#include "xAODCaloEvent/versions/CaloTower_v1.h"

DATAVECTOR_BASE( xAOD::CaloTower_v1, xAOD::IParticle );

namespace xAOD {

  typedef DataVector<CaloTower_v1> CaloTowerContainerBase_v1;

  class CaloTowerContainer_v1 : public CaloTowerContainerBase_v1
  {
  public:

    /// @brief Default constructor
    /// Constructs a tower container without structure (binning) or content. The @c configureGrid method has to be called after this
    /// container is instantiated and an auxiliary store is assigned.
    ///
    /// Arguments correspond to the @c DataVector constructor:
    ///
    /// @param[in] ownPolicy    ownership policy tag (default owns its elements)
    /// @param[in] trackIndices tracking indicies policy (default track indices)
    ///
    CaloTowerContainer_v1(SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);

    /// @brief Loaded constructor
    ///
    /// Constructs a configured @c CaloTowerContainer_v1 object with tower grid parameters
    ///
    /// @note Invoking the @c CaloTowerContainer_v1::configureGrid method will do nothing - the tower grid cannot be overwritten when
    ///       using this constructor.
    ///
    /// @note The @f$ \phi @f$ boundaries are hardcoded to  @f$ [-\pi,\pi] @f$
    ///
    /// @param[in] nEtaBins   number of @f$ \eta @f$ bins
    /// @param[in] etaMin     lower boundary of @f$ \eta @f$ range
    /// @param[in] etaMax     upper boundary of @f$ \eta @f$ range 
    /// @param[in] nPhiBins   number of @f$ \phi @f$ bins (default 64)
    /// @param[in] towDescr     reference to non-modifiable @c CaloTowerDescriptor
    /// @param[in] ownPolicy    ownership policy tag (default owns its elements)
    /// @param[in] trackIndices tracking indicies policy (default track indices)
    ///
    CaloTowerContainer_v1(int nEtaBins,double etaMin,double etaMax,int nPhiBins,SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS,SG::IndexTrackingPolicy trackIndices = SG::DEFAULT_TRACK_INDICES);


    /// @brief Destructor
    ~CaloTowerContainer_v1();
    
    /// @name Container management
    ///
    /// @note Invoking the @c configureGrid method is mandatory to set up a valid tower grid, if the @c CaloTowerContainer_v1 is constructed with the default constructor.
    /// @{
    /// @brief Configure the tower grid with individual parameters
    ///
    /// @param[in] nEtaBins number of bins in pseudorapidity
    /// @param[in] etaMin   lower boundary of pseudorapidity range
    /// @param[in] etaMax   upper boundary of pseudorapidity range
    /// @param[in] nPhiBins number of bins in azimuth
    /// @note The @f$ \phi @f$ boundaries are hardcoded to  @f$ [-\pi,\pi] @f$
    /// @note This method will internally invoke @c CaloTowerContainer_v1::configureGrid() method.

    bool configureGrid(int nEtaBins,double etaMin,double etaMax,int nPhiBins);
    /// @}

    /// @name Tower grid description accessors
    /// @{
    ///
    int    nEtaBins() const; ///> @brief Returns number of @f$ \eta @f$ bins 
    double etaMin()   const; ///> @brief Returns lower boundary of @f$ \eta @f$ range
    double etaMax()   const; ///> @brief Returns upper boudnary of @f$ \eta @f$ range
    double deltaEta() const; ///> @brief Returns grid size in @f$ \eta @f$
    int    nPhiBins() const; ///> @brief Returns number of @f$ \phi @f$ bins
    double phiMin()   const; ///> @brief Returns lower boundary of @f$ \phi @f$ range 
    double phiMax()   const; ///> @brief Returns upper boundary of @f$ \phi @f$ range
    double deltaPhi() const; ///> @brief Returns grid size in @f$ \phi @f$
    int    nTowers() const;  ///> @brief Returns the nominal number of towers 
    /// @}


    /// @name Conversion to/from indices
    /// @{
    int index(double eta, double phi) const; 
    double eta(size_t index) const;
    double phi(size_t index) const;

    const CaloTower_v1* tower(double eta, double phi) const; 
    /// @}

  private:

    bool configureGrid();

    /// @name Towerdescription
    /// @{
    unsigned short m_nEtaBins; ///> @brief Number of pseudorapidity bins
    double         m_etaMin;   ///> @brief Lower boundary pseudorapidity range 
    double         m_etaMax;   ///> @brief Upper boundary pseudorapidity range
    unsigned short m_nPhiBins; ///> @brief Number of bins in azimuth
    double         m_deltaEta;     ///> @brief Bin width in pseudorapidity 
    double         m_deltaPhi;     ///> @brief Bin width in azimuth
    bool           m_isConfigured = false; ///> @brief Configuration status flag
    /// @}
  };
}

/// @class xAOD::CaloTowerContainer_v1
/// 
/// @brief Basic container storing all information need for @c xAOD::CaloTower_v1 objects.
///
/// This container does not only provide the store for @c CaloTower_v1 object, but also all meta-data needed to provide the
/// tower grid and kinematics. The tower grid is defined in terms of pseudorapidity (@f$ \eta @f$) and azimuth (@f$ \phi @f$). 
/// In this it follows the @c CaloCell direction descriptors. As calorimeter towers represented by @c xAOD::CaloTower_v1 objects
/// are by definition massless, any four-momentum representation extracted from a calorimeter tower (a bin in tower grid 
/// specified by this container) yields @f$ y = \eta @f$ for rapidity @f$ y @f$. 
///
/// There is no need to rebuild this container event-by-event, because the direction of any given tower never 
/// changes, only its energy content; the direction is related to the tower's index in the container.
///
/// Usage default tower grid version (without the code recording in the event store):
/// @code
/// // create an empty container
/// xAOD::CaloTowerContainer_v1* towCont = new xAOD::CaloTowerContainer_v1();
/// // allocate the auxiliary store
/// xAOD::CaloTowerAuxContainer_v1* towAuxCont = new xAOD::CaloTowerAuxContainer();
/// // associate the two containers
/// towCont->setStore(towAuxCont);
/// // configure with a typical hadronic tower grid (0.1 x 0.1) and fill in empty CaloTowers
/// towCont->configureGrid(100,-5.,5.,64);
/// @endcode


#include "xAODCore/BaseInfo.h"
//SG_BASE(xAOD::CaloTowerContainer_v1, DataVector<xAOD::CaloTower_v1>);
SG_BASE(xAOD::CaloTowerContainer_v1,  xAOD::CaloTowerContainerBase_v1);

inline int xAOD::CaloTowerContainer_v1::nEtaBins() const { return m_nEtaBins; }
inline int xAOD::CaloTowerContainer_v1::nPhiBins() const { return m_nPhiBins; }
inline int xAOD::CaloTowerContainer_v1::nTowers() const { return  m_nEtaBins*m_nPhiBins;}

inline double xAOD::CaloTowerContainer_v1::etaMin()   const { return m_etaMin; }
inline double xAOD::CaloTowerContainer_v1::etaMax()   const { return m_etaMax; }
inline double xAOD::CaloTowerContainer_v1::deltaEta() const { return m_deltaEta; }

inline double xAOD::CaloTowerContainer_v1::phiMin()   const { return -M_PI; }
inline double xAOD::CaloTowerContainer_v1::phiMax()   const { return M_PI; }
inline double xAOD::CaloTowerContainer_v1::deltaPhi() const { return m_deltaPhi; }

inline const xAOD::CaloTower_v1* xAOD::CaloTowerContainer_v1::tower(double eta, double phi) const {
  int idx=index(eta,phi);
  if (idx>0 && idx<(int)this->size()) {
    return (*this)[idx];
  }
  
    return nullptr;
}

#endif
