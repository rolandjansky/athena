/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: AtlasGeoPoint.h 689815 2015-08-17 15:39:55Z wsfreund $ 
#ifndef XAODCALORINGS_UTILS_ATLASGEOPOINT_H
#define XAODCALORINGS_UTILS_ATLASGEOPOINT_H

#include <limits>

#include "xAODCaloRings/tools/cxx/RingerUseNewCppFeatures.h"

/**
 * @class xAOD::RingSet_v1::AtlasGeoPoint
 * @brief Declare a point holder for Atlas usual geometry
 **/
class AtlasGeoPoint {

  public:

    AtlasGeoPoint():
      m_eta(std::numeric_limits<float>::max()),
      m_phi(std::numeric_limits<float>::max())
    {;}

    AtlasGeoPoint(const float eta, const float phi):
      m_eta(eta),
      m_phi(phi)
    {;}

    static const float PHI_MIN;
    static const float PHI_MAX;
#if RINGER_USE_NEW_CPP_FEATURES
    // This cannot be compiled with Root Genflex below version 6
    // because there is no C++11 compatibility.

    // static constexpr float PHI_MIN = CaloPhiRange::phi_min(); <- not constexpr
    // static constexpr float PHI_MAX = CaloPhiRange::phi_max(); <- not constexpr
    static constexpr float ETA_MIN = -5;
    static constexpr float ETA_MAX = 5;
#else
    // static const float PHI_MIN;
    // static const float PHI_MAX;
    static const float ETA_MIN;
    static const float ETA_MAX;
#endif // RINGER_USE_NEW_CPP_FEATURES

    // Assign operator
    AtlasGeoPoint& operator=(const AtlasGeoPoint &p);

    // Check if AtlasGeoPoint is within bounds
    bool isValid() const;

    // Get point eta
    float eta() const;
    // Get point phi
    float phi() const;

    // Set point eta
    void setEta(const float eta);
    // Set point phi
    void setPhi(const float phi);
  
  private:

    // Eta 
    float m_eta;
    // Phi
    float m_phi;

};

//==============================================================================
// @name RingSet_v1::AtlasGeoPoint operator=:
inline
AtlasGeoPoint& AtlasGeoPoint::operator=(const AtlasGeoPoint &p)
{
  if(this==&p){
    return *this;
  }
  m_eta = p.eta(); 
  m_phi = p.phi();
  return *this;
}

//==============================================================================
inline
bool AtlasGeoPoint::isValid() const 
{
  if ( ( m_eta >= ETA_MIN && m_phi >= PHI_MIN ) &&
      ( m_eta <= ETA_MAX && m_phi <= PHI_MAX ) ){
    return true;
  }
  return false;
}

//==============================================================================
inline
float AtlasGeoPoint::eta() const
{
  return m_eta;
}

//==============================================================================
inline
float AtlasGeoPoint::phi() const
{
  return m_phi;
}

//==============================================================================
inline
void AtlasGeoPoint::setEta(const float eta)
{
  m_eta = eta;
}

//==============================================================================
inline
void AtlasGeoPoint::setPhi(const float phi)
{
  m_phi = phi;
}


#endif // XAODCALORINGS_UTILS_ATLASGEOPOINT_H
