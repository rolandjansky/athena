// -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOEVENT_CALOTOWER_V1_H
#define CALOEVENT_CALOTOWER_V1_H

#include "xAODBase/IParticle.h"

// ROOT include(s):
#include "Math/Vector4D.h"

namespace xAOD {

  class CaloTower_v1 : public IParticle
  {
  public:

    enum {
      INVALIDINDEX=-1
    };


    /// @brief Default constructor
    ///
    /// Constructs empty @c xAOD::CaloTower_v1 object. 
    CaloTower_v1();

    /// @brief Destructor
    ~CaloTower_v1();

    /// @brief Reset function
    ///
    /// Sets the tower energy to zero. Indices/directions associated with this object are not changed.
    void reset(); 

    /// @brief Add energy
    ///
    /// @param[in] energy value added to tower energy
    void addEnergy(double energy);
    /// @brief Sets the energy
    ///
    /// @param[in] energy value to which the tower energy will be set to
    void setEnergy(double energy);

    /// @name Kinematic accessors (@c IParticle interface)
    /// @{
    virtual double pt()  const;                     ///< @brief transverse momentum @f$ p_{\mathrm{T}} @f$
    virtual double eta() const;                     ///< @brief pseudorapidity @f$ \eta @f$$
    virtual double phi() const;                     ///< @brief azimuth @f$ \phi @f$
    virtual double rapidity() const;                ///< @brief rapidity @f$ y @f$
    virtual double m()   const;                     ///< @brief mass @f$ m = 0 @f$ (by convention)
    virtual double e()   const;                     ///< @brief energy @f$ E @f$

    /// Definition of the 4-momentum type
    typedef IParticle::FourMom_t FourMom_t;

    /// Base 4 Momentum type for calo
    typedef ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > GenVecFourMom_t;

    GenVecFourMom_t genvecP4() const; ///< @brief Four-momentum representation: GenVector

    virtual FourMom_t p4() const; ///< @brief Four-momentum representation
    /// @}

    /// @name Implementations of the other @c IParticle interface methods
    /// @{
    virtual Type::ObjectType type() const; ///< @brief object type - presently @c Type::Other (FIXME)
    /// @}
    
  private:

    /// @name Set accessors to persistent data
    /// @{
    float& f_ref_e();           ///< @brief Reference to modifiable energy store
    /// @}
    /// @name Get accessors to persistent data
    /// @{
    float f_val_e()    const;   ///< @brief Accessor for energy 
    /// @}

  };
}

inline float& xAOD::CaloTower_v1::f_ref_e()    { static const Accessor<float> acc("towerE"); return acc(*this); }

inline float xAOD::CaloTower_v1::f_val_e()    const { static const ConstAccessor<float> acc("towerE");  return acc(*this); }

///! @class xAOD::CaloTower_v1
///
/// @brief Object representing calorimeter signal towers.  
///
/// This object represents calorimeter energy towers located on a regular grid in azimuth and pseudorapidity. The
/// grid is associated with the @c xAOD::CaloTowerContainer_v1. Due to the minimalistic underlying storage model, 
/// @c xAOD::CaloTower_v1 objects are only completely described if allocated in its container. 
///
/// General kinematic features 
/// --------------------------
///
/// Kinematic features of calorimeter towers are (1) they are massless, and (2) their direction is given by the
/// center of the @f$ (\eta,\phi) @f$ bin they represent. This means that @f$ (\eta,\phi) @f$ for any given shower never
/// changes, only its energy content is dynamic.
///
/// The full four-momentum of the calorimeter tower is calculated from its energy and direction
/// @f[
///       \left(E,\eta,\phi\right) \mapsto \left(E=p,p_{x},p_{y},p_{z}\right)
/// @f]
///
/// Formally, this can be considered a massless pseudoparticle. If the net energy of the tower is negative, no physically
/// meaningful four-momentum can be constructed (see below), rather the tower is represented by @f$ (0.,0., 0., 0.) @f$.
///
/// Treatment of negative energy towers
/// -----------------------------------
///
/// Negative energy towers cannot provide a four-momentum, but still contain valuable data. The direction @f$ ( \eta, \phi ) @f$ are
/// independent of the value and sign of the tower energy, as towers are located at a fixed location in this space. The tower energy 
/// is provided independent of its sign as well. This means the @c xAOD::CaloTower_v1::e(), @c xAOD::CaloTower_v1::eta(), and 
/// @c xAOD::CaloTower_v1::phi() methods return correct information. In addition, the @c xAOD::CaloTower_v1::m() and @c xAOD::CaloTower_v1::rapidity()
/// methods provide meaningful information as well, with rapidity @f$ y = \eta @f$ due to tower mass @f$ m = 0 @f$.
#endif
