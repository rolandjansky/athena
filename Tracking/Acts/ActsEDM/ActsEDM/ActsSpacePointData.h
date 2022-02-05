// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSEDM_SPACEPOINTDATA_H
#define ACTSEDM_SPACEPOINTDATA_H 1

#include <vector>
#include "Acts/Definitions/Algebra.hpp"

namespace ActsTrk {

  class SpacePointData {
  public:
    SpacePointData() = default;
    SpacePointData(const SpacePointData&) = delete;
    ~SpacePointData() = default;

    inline double x(std::size_t n) const;
    inline double y(std::size_t n) const;
    inline double z(std::size_t n) const;
    inline double varianceR(std::size_t n) const;
    inline double varianceZ(std::size_t n) const;

    std::size_t insert(const Acts::Vector3& position,
		       const Acts::Vector2& variance);

    void reserve(std::size_t n);

  private:
    std::vector< double > m_pos_x;
    std::vector< double > m_pos_y;
    std::vector< double > m_pos_z;
    std::vector< double > m_cov_r;
    std::vector< double > m_cov_z;
  };

  inline double SpacePointData::x(std::size_t n) const { return m_pos_x.at(n); }
  inline double SpacePointData::y(std::size_t n) const { return m_pos_y.at(n); }
  inline double SpacePointData::z(std::size_t n) const { return m_pos_z.at(n); }
  inline double SpacePointData::varianceR(std::size_t n) const { return m_cov_r.at(n); }
  inline double SpacePointData::varianceZ(std::size_t n) const { return m_cov_z.at(n); }

} // Acts namespace


// Set up a CLID for the type:
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ActsTrk::SpacePointData, 207128530, 1 )

#endif 
