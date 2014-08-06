/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ROIGetterToolImpl.cxx 500205 2012-05-11 10:17:25Z krasznaa $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "ROIGetterToolImpl.h"

namespace D3PD {

   ROIGetterToolImpl::ROIGetterToolImpl( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
      : CollectionGetterToolImpl( type, name, parent ), m_firstevent( true ) {

      // Declare the tool's properties:
      declareProperty( "SGKey", m_key = "LVL1_ROI",
                       "StoreGate key for the LVL1_ROI object" );
      declareProperty( "TypeName", m_type = "LVL1_ROI",
                       "Dummy type name setting. Only 'LVL1_ROI' is handled "
                       "by the tool anyway..." );

   }

   const LVL1_ROI* ROIGetterToolImpl::get( bool allowMissing ) {

      // If the LVL1 RoI object is not available, return gracefully:
      if( allowMissing && ( ! evtStore()->contains< LVL1_ROI >( m_key ) ) ) {
         return 0;
      }

      // Retrieve the LVL1_ROI object:
      const LVL1_ROI* lvl1_roi = 0;
      if( evtStore()->retrieve( lvl1_roi, m_key ).isFailure() ) {
         REPORT_ERROR( StatusCode::FAILURE )
            << "LVL1_ROI object could not be found with key: "
            << m_key;
         return 0;
      }

      if( m_firstevent ) {
         REPORT_MESSAGE( MSG::INFO ) << "Using StoreGate key: " << m_key;
         m_firstevent = false;
      }

      return lvl1_roi;
   }

   const std::type_info& ROIGetterToolImpl::typeinfo() const {

      return typeid( LVL1_ROI );
   }

   const void* ROIGetterToolImpl::getUntyped( bool allowMissing ) {

      return get( allowMissing );
   }

} // namespace D3PD
