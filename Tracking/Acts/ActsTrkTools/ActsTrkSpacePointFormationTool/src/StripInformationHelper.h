/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKSPACEPOINTFORMATIONTOOL_STRIPINFORMATIONHELPER_H
#define ACTSTRKSPACEPOINTFORMATIONTOOL_STRIPINFORMATIONHELPER_H

#include "GeoPrimitives/GeoPrimitives.h"

namespace ActsTrk {

  class StripInformationHelper {

    /// @class StripInformationHelper
    /// This class contains the information on the strips used for
    /// space point formation.
    /// The quantities stored are used for mathematical evaluation of the
    /// space point location.

  public:
    /// @name Constructors with and without parameters
    //@{
    StripInformationHelper() {};
    StripInformationHelper(const Amg::Vector3D& stripStart,
                           const Amg::Vector3D& stripEnd,
                           const Amg::Vector3D& beamSpotVertex,
                           const float& locx,
                           const size_t& clusterIndex,
                           const size_t& stripIndex);
    //@}

    /// @name Destructor, copy construcor, assignment operator
    //@{
    virtual ~StripInformationHelper() = default;
    StripInformationHelper(const StripInformationHelper&) = default;
    StripInformationHelper& operator = (const StripInformationHelper&) = default;
    //@}

    /// @name Public method to set strip properties
    //@{
    void set(const Amg::Vector3D& stripStart,
             const Amg::Vector3D& stripEnd,
             const Amg::Vector3D& beamSpotVertex,
             const float& locx,
             const size_t& clusterIndex,
             const size_t& stripIndex);
    //@}

    /// @name Public methods to return strip properties
    //@{
    const size_t& clusterIndex() const {return m_clusterIndex;}
    const Amg::Vector3D& stripCenter () const {return m_stripCenter ;}
    const Amg::Vector3D& stripDirection () const {return m_stripDir ;}
    const Amg::Vector3D& trajDirection () const {return m_trajDir ;}
    const Amg::Vector3D& normal() const {return m_normal;}
    const double& oneOverStrip() const {return m_oneOverStrip;}
    const float& locX() const {return m_locX;}
    const size_t& stripIndex() const {return m_stripIndex;}
    Amg::Vector3D position(const double& shift) const;
    //@}

  private:

    /// @name Private members
    /// @param m_stripCenter Center of strip, evaluated in setting function
    /// as (stripStart+stripEnd)*0.5
    Amg::Vector3D m_stripCenter;
    /// @param m_stripDir Direction of strip, evaluated in setting function
    /// as (stripStart-stripEnd)
    Amg::Vector3D m_stripDir;
    /// @param m_trajDir Direction of trajectory, evaluated in setting function
    /// as (stripStart+stripEnd-2*beamSpotVertex)
    Amg::Vector3D m_trajDir;
    /// @param m_normal Normal to strip diretion and trjectory direction plane,
    /// evaluated in setting function as cross product
    /// between stripDirection and trajDirection
    Amg::Vector3D m_normal;
    /// @param m_oneOverStrip Inverse of length of the strip
    double        m_oneOverStrip;
    /// @param m_locX Location X of cluster
    float         m_locX;
    /// @param m_stripIndex index of the strip corresponding to location
    size_t        m_stripIndex;
    /// @param m_clusterIndex xAOD::StripCluster index in container
    size_t        m_clusterIndex;
  };

} // end of name space

#endif  // ACTSTRKSPACEPOINTFORMATIONTOOL_STRIPINFORMATIONHELPER_H
