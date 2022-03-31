/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "StripInformationHelper.h"

namespace ActsTrk {

    StripInformationHelper::StripInformationHelper (const Amg::Vector3D& stripStart,
                                                    const Amg::Vector3D& stripEnd,
                                                    const Amg::Vector3D& beamSpotVertex,
                                                    const float& locx,
                                                    const size_t& clusterIndex)
    {
        this->set(stripStart, stripEnd, beamSpotVertex, locx, clusterIndex);
    }

    void StripInformationHelper::set(const Amg::Vector3D& stripStart,
                                     const Amg::Vector3D& stripEnd,
                                     const Amg::Vector3D& beamSpotVertex,
                                     const float& locx,
                                     const size_t& clusterIndex)
    {
        m_stripCenter  = 0.5*(stripStart+stripEnd);
        m_stripDir     = stripStart-stripEnd;
        m_trajDir      = 2.*(m_stripCenter-beamSpotVertex);
        m_normal       = m_stripDir.cross(m_trajDir);
        m_oneOverStrip = 1./m_stripDir.mag();
        m_locX         = locx;
        m_clusterIndex = clusterIndex;
    }

    Amg::Vector3D StripInformationHelper::position(const double& shift) const
    {
        return (m_stripCenter+(0.5*shift)*m_stripDir);
    }
}
