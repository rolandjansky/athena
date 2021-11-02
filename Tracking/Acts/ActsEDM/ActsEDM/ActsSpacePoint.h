// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSEDM_SPACEPOINT_H
#define ACTSEDM_SPACEPOINT_H 1

#include "ActsEDM/ActsSpacePointData.h"
#include <boost/container/static_vector.hpp>

namespace ActsTrk {
  
  class SpacePoint {
  public:
    SpacePoint() = delete;
    template <typename position_t, typename variance_t>
    SpacePoint(const Eigen::MatrixBase<position_t>& pos,
	       const Eigen::MatrixBase<variance_t>& var,
	       SpacePointData& data,
	       const boost::container::static_vector<std::size_t, 2>& measIndexes);
    
    double x() const;
    double y() const;
    double z() const;
    double varianceR() const;
    double varianceZ() const;

    const boost::container::static_vector<std::size_t, 2>& measurementIndexes() const;

  private:
    std::size_t m_index;
    boost::container::static_vector<std::size_t, 2> m_measurementIndexes;
    SpacePointData* m_data;
  };
  
  template <typename position_t, typename variance_t>
  SpacePoint::SpacePoint(const Eigen::MatrixBase<position_t>& pos,
			 const Eigen::MatrixBase<variance_t>& var,
			 SpacePointData& data,
			 const boost::container::static_vector<std::size_t, 2>& measIndexes)
    : m_measurementIndexes(measIndexes),
      m_data(&data)
  {
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(position_t, 3);
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(variance_t, 2);

    m_index = m_data->insert(pos,var);
  }
  
  inline double SpacePoint::x() const { return m_data->x(m_index); }
  inline double SpacePoint::y() const { return m_data->y(m_index); }
  inline double SpacePoint::z() const { return m_data->z(m_index); }
  inline double SpacePoint::varianceR() const { return m_data->varianceR(m_index); }
  inline double SpacePoint::varianceZ() const { return m_data->varianceZ(m_index); }

  inline const boost::container::static_vector<std::size_t, 2>& SpacePoint::measurementIndexes() const { return m_measurementIndexes; }
} // Acts namespace

#include "AthContainers/DataVector.h"
namespace ActsTrk {
  typedef DataVector< ActsTrk::SpacePoint > SpacePointContainer;
}

// Set up a CLID for the type:
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ActsTrk::SpacePoint, 207128531, 1 )
CLASS_DEF( ActsTrk::SpacePointContainer, 1262318102, 1)

#endif

