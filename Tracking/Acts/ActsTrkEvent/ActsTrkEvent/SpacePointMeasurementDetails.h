/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ACTSTRKEVENT_SPACEPOINTMESUREMENTDETAILS_H
#define ACTSTRKEVENT_SPACEPOINTMESUREMENTDETAILS_H 1

#include <vector>
#include <array>
#include "Acts/Definitions/Algebra.hpp"

namespace ActsTrk {

  class SpacePointMeasurementDetails {
      /// @class SpacePointMeasurementDetails
      /// This class contains details on the strips used for
      /// space point formation.
  public:
      /// @name Constructors, destructor, copy constructor
      SpacePointMeasurementDetails() = default;
      SpacePointMeasurementDetails(const SpacePointMeasurementDetails&) = delete;
      ~SpacePointMeasurementDetails() = default;

      /// @name Public method to insert parameters
      //@{
      std::size_t insert(const float& topHalfStripLength,
                         const Acts::Vector3& topStripDirection,
                         const float& bottomHalfStripLength,
                         const Acts::Vector3& bottomStripDirection,
                         const Acts::Vector3& stripCenterDistance,
                         const Acts::Vector3& topStripCenter);
      //@}

      /// @name Public methods to return strip properties for space points
      //@{
      inline float topHalfStripLength(std::size_t n) const;
      inline float bottomHalfStripLength(std::size_t n) const;
      inline Acts::Vector3 topStripDirection(std::size_t n) const;
      inline Acts::Vector3 bottomStripDirection(std::size_t n) const;
      inline Acts::Vector3 stripCenterDistance(std::size_t n) const;
      inline Acts::Vector3 topStripCenter(std::size_t n) const;
      //@}

      /// @name Public method to reserve space in containers
      void reserve(std::size_t n);

  private:
      /// @name Private members
      //@{
      /// @param m_topHalfStripLength half length of top strip for space point
      std::vector< float > m_topHalfStripLength;
      /// @param m_bottomHalfStripLength half length of bottom strip for space point
      std::vector< float > m_bottomHalfStripLength;

      /// @param m_topStripDirection top strip direction (normalised)
      std::vector< std::array< double, 3> > m_topStripDirection;

      /// @param m_bottomStripDirection bottom strip direction (normalised)
      std::vector< std::array< double, 3> > m_bottomStripDirection;

      /// @param m_stripCenterDistance distance between strips
      std::vector< std::array< double, 3> > m_stripCenterDistance;

      /// @param m_topStripCenter top strip center position
      std::vector< std::array< double, 3> > m_topStripCenter;
      //@}
  };

  inline float SpacePointMeasurementDetails::topHalfStripLength(std::size_t n) const {
      return m_topHalfStripLength.at(n);
  }

  inline float SpacePointMeasurementDetails::bottomHalfStripLength(std::size_t n) const {
      return m_bottomHalfStripLength.at(n);
  }

  inline Acts::Vector3 SpacePointMeasurementDetails::topStripDirection(std::size_t n) const {
      return Acts::Vector3(m_topStripDirection.at(n)[0],
                           m_topStripDirection.at(n)[1],
                           m_topStripDirection.at(n)[2]);
  }

  inline Acts::Vector3 SpacePointMeasurementDetails::bottomStripDirection(std::size_t n) const {
      return Acts::Vector3(m_bottomStripDirection.at(n)[0],
                           m_bottomStripDirection.at(n)[1],
                           m_bottomStripDirection.at(n)[2]);
  }

  inline Acts::Vector3 SpacePointMeasurementDetails::stripCenterDistance(std::size_t n) const {
      return Acts::Vector3(m_stripCenterDistance.at(n)[0],
                           m_stripCenterDistance.at(n)[1],
                           m_stripCenterDistance.at(n)[2]);
  }

  inline Acts::Vector3 SpacePointMeasurementDetails::topStripCenter(std::size_t n) const {
      return Acts::Vector3(m_topStripCenter.at(n)[0],
                           m_topStripCenter.at(n)[1],
                           m_topStripCenter.at(n)[2]);
  }

} // Acts namespace


// Set up a CLID for the type:
#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( ActsTrk::SpacePointMeasurementDetails, 194957671, 1 )

#endif

