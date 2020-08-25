// Dear emacs, this is -*- c++ -*-
//
// Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
//

// Local include(s).
#include "LinearTransformExampleAlg.h"
#include "HIPFunctions.h"

// System include(s).
#include <limits>
#include <sstream>

namespace AthHIPExamples {

   StatusCode LinearTransformExampleAlg::initialize() {

      // Print a bit of information about the selected device.
      std::ostringstream deviceInfo;
      printHipDeviceInfo( m_hipDevice.value(), deviceInfo );
      ATH_MSG_INFO( "Will be using device:\n" << deviceInfo.str() );

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

   StatusCode LinearTransformExampleAlg::execute( const EventContext& ) const {

      // Create a dummy array variable that will be linearly transformed.
      static constexpr float ARRAY_ELEMENT = 3.141592f;
      std::vector< float > dummyArray( m_arraySize.value(), ARRAY_ELEMENT );

      // Perform the transformation.
      linearTransform( m_hipDevice.value(), dummyArray, m_multiplier.value(),
                       m_shift.value() );

      // Validate the contents of the transformed array.
      const float expectedResult =
         m_multiplier.value() * ARRAY_ELEMENT + m_shift.value();
      static constexpr float allowedDeviation =
         std::numeric_limits< float >::epsilon() * 4;
      for( float element : dummyArray ) {
         if( std::abs( element - expectedResult ) > allowedDeviation ) {
            REPORT_MESSAGE( MSG::FATAL )
               << "The HIP linear transformation failed";
            return StatusCode::FAILURE;
         }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
   }

} // namespace AthHIPExamples
