/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKEVENT_SPACEPOINT_H
#define ACTSTRKEVENT_SPACEPOINT_H 1

#include "ActsTrkEvent/SpacePointData.h"
#include "ActsTrkEvent/SpacePointMeasurementDetails.h"
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

    template <typename position_t, typename variance_t>
    SpacePoint(const Eigen::MatrixBase<position_t>& pos,
               const Eigen::MatrixBase<variance_t>& var,
               float& topHalfStripLength,
               const Eigen::MatrixBase<position_t>& topStripDirection,
               float& bottomHalfStripLength,
               const Eigen::MatrixBase<position_t>& bottomStripDirection,
               const Eigen::MatrixBase<position_t>& stripCenterDistance,
               const Eigen::MatrixBase<position_t>& topStripCenter,
               SpacePointData& data,
               SpacePointMeasurementDetails& details,
               const boost::container::static_vector<std::size_t, 2>& measIndexes);
    
    double x() const;
    double y() const;
    double z() const;
    double varianceR() const;
    double varianceZ() const;

    bool hasMeasurementDetails() const;
    float topHalfStripLength() const;
    float bottomHalfStripLength() const;
    Acts::Vector3 topStripDirection() const;
    Acts::Vector3 bottomStripDirection() const;
    Acts::Vector3 stripCenterDistance() const;
    Acts::Vector3 topStripCenter() const;

    const boost::container::static_vector<std::size_t, 2>& measurementIndexes() const;

  private:
    std::size_t m_index;
    boost::container::static_vector<std::size_t, 2> m_measurementIndexes;
    SpacePointData* m_data;
    std::size_t m_indexDetails;
    SpacePointMeasurementDetails* m_details;
  };
  
  template <typename position_t, typename variance_t>
  SpacePoint::SpacePoint(const Eigen::MatrixBase<position_t>& pos,
                         const Eigen::MatrixBase<variance_t>& var,
                         SpacePointData& data,
                         const boost::container::static_vector<std::size_t, 2>& measIndexes)
    : m_measurementIndexes(measIndexes),
      m_data(&data),
      m_indexDetails(0),
      m_details(nullptr)
  {
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(position_t, 3);
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(variance_t, 2);

    m_index = m_data->insert(pos,var);
  }

  template <typename position_t, typename variance_t>
  SpacePoint::SpacePoint(const Eigen::MatrixBase<position_t>& pos,
                         const Eigen::MatrixBase<variance_t>& var,
                         float& topHalfStripLength,
                         const Eigen::MatrixBase<position_t>& topStripDirection,
                         float& bottomHalfStripLength,
                         const Eigen::MatrixBase<position_t>& bottomStripDirection,
                         const Eigen::MatrixBase<position_t>& stripCenterDistance,
                         const Eigen::MatrixBase<position_t>& topStripCenter,
                         SpacePointData& data,
                         SpacePointMeasurementDetails& details,
                         const boost::container::static_vector<std::size_t, 2>& measIndexes)
    : m_measurementIndexes(measIndexes),
      m_data(&data),
      m_details(&details)
  {
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(position_t, 3);
    EIGEN_STATIC_ASSERT_VECTOR_SPECIFIC_SIZE(variance_t, 2);

    m_index = m_data->insert(pos,var);

    m_indexDetails = m_details->insert(topHalfStripLength, topStripDirection,
                                       bottomHalfStripLength, bottomStripDirection,
                                       stripCenterDistance, topStripCenter);

  }
  
  inline double SpacePoint::x() const { return m_data->x(m_index); }
  inline double SpacePoint::y() const { return m_data->y(m_index); }
  inline double SpacePoint::z() const { return m_data->z(m_index); }
  inline double SpacePoint::varianceR() const { return m_data->varianceR(m_index); }
  inline double SpacePoint::varianceZ() const { return m_data->varianceZ(m_index); }

  inline const boost::container::static_vector<std::size_t, 2>& SpacePoint::measurementIndexes() const { return m_measurementIndexes; }

  inline bool SpacePoint::hasMeasurementDetails() const {
    return (m_details != nullptr);
  }

  inline float SpacePoint::topHalfStripLength() const {
    return m_details->topHalfStripLength(m_indexDetails);
  }

  inline float SpacePoint::bottomHalfStripLength() const {
    return m_details->bottomHalfStripLength(m_indexDetails);
  }

  inline Acts::Vector3 SpacePoint::topStripDirection() const {
    return m_details->topStripDirection(m_indexDetails);
  }

  inline Acts::Vector3 SpacePoint::bottomStripDirection() const {
    return m_details->bottomStripDirection(m_indexDetails);
  }

  inline Acts::Vector3 SpacePoint::stripCenterDistance() const {
    return m_details->stripCenterDistance(m_indexDetails);
  }

  inline Acts::Vector3 SpacePoint::topStripCenter() const {
    return m_details->topStripCenter(m_indexDetails);
  }

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

