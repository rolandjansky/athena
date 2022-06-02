/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsTrkEvent/SpacePointMeasurementDetails.h"

namespace ActsTrk {
  std::size_t SpacePointMeasurementDetails::insert(const float& topHalfStripLength,
                         const Acts::Vector3& topStripDirection,
                         const float& bottomHalfStripLength,
                         const Acts::Vector3& bottomStripDirection,
                         const Acts::Vector3& stripCenterDistance,
                         const Acts::Vector3& topStripCenter) {
    std::size_t index = m_topHalfStripLength.size();
    // half length and direction of top strip
    m_topHalfStripLength.push_back( topHalfStripLength );
    m_topStripDirection.push_back( {topStripDirection[0], topStripDirection[1], topStripDirection[2]} );

    // half length and direction of bottom strip
    m_bottomHalfStripLength.push_back( bottomHalfStripLength );
    m_bottomStripDirection.push_back( {bottomStripDirection[0], bottomStripDirection[1], bottomStripDirection[2]} );

    // distance strip centers
    m_stripCenterDistance.push_back( {stripCenterDistance[0], stripCenterDistance[1], stripCenterDistance[2]} );

    // center of top strip
    m_topStripCenter.push_back( {topStripCenter[0], topStripCenter[1], topStripCenter[2]} );
    // return index where values have been saved
    return index;
  }

  void SpacePointMeasurementDetails::reserve(std::size_t n) {
    m_topHalfStripLength.reserve(n);
    m_topStripDirection.reserve(n);
    m_bottomHalfStripLength.reserve(n);
    m_bottomStripDirection.reserve(n);
    m_stripCenterDistance.reserve(n);
    m_topStripCenter.reserve(n);
  }

} // ActsTrk namespace
