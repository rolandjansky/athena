// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "ActsEDM/ActsSpacePointData.h"

namespace ActsTrk {
  std::size_t SpacePointData::insert(const Acts::Vector3& position,
				     const Acts::Vector2& variance) {
    std::size_t index = m_pos_x.size();
    // position                                                                                                                                                                                                 
    m_pos_x.push_back( position[0] );
    m_pos_y.push_back( position[1] );
    m_pos_z.push_back( position[2] );
    // Variance                                                                                                                                                                                                 
    m_cov_r.push_back( variance[0] );
    m_cov_z.push_back( variance[1] );
    // return index where values have been saved                                                                                                                                                                
    return index;
  }
  
  void SpacePointData::reserve(std::size_t n) {
    m_pos_x.reserve(n);
    m_pos_y.reserve(n);
    m_pos_z.reserve(n);
    m_cov_r.reserve(n);
    m_cov_z.reserve(n);
  }
  
} // ActsTrk namespace 

