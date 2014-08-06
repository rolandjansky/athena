/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: EnergySum_ROIGetterTool.cxx 500205 2012-05-11 10:17:25Z krasznaa $

// Athena/Gaudi include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "AnalysisTriggerEvent/LVL1_ROI.h"

// Local include(s):
#include "EnergySum_ROIGetterTool.h"

namespace D3PD {

   EnergySum_ROIGetterTool::EnergySum_ROIGetterTool( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
      : ObjGetterTool< EnergySum_ROI >( type, name, parent ), m_firstevent( true ) {

      // Declare the tool's properties:
      declareProperty( "SGKey", m_key = "LVL1_ROI",
                       "StoreGate key for the LVL1_ROI object" );
      declareProperty( "TypeName", m_type = "LVL1_ROI",
                       "Dummy type name setting. Only 'LVL1_ROI' is handled "
                       "by the tool anyway..." );
   }

   const EnergySum_ROI* EnergySum_ROIGetterTool::get( bool allowMissing ) {

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

      // Return gracefully if there are no EnergySum RoIs in the event:
      if( ! lvl1_roi->getEnergySumROIs().size() ) {
         if( ! allowMissing ) {
            REPORT_ERROR( StatusCode::FAILURE )
               << "No EnergySum_ROI found in the LVL1_ROI object!";
         } else {
            REPORT_MESSAGE( MSG::WARNING )
               << "No EnergySum_ROI found in the LVL1_ROI object!";
         }
         return 0;
      }

      // Check that there is only one:
      if( lvl1_roi->getEnergySumROIs().size() != 1 ) {
         REPORT_MESSAGE( MSG::WARNING ) << "More than 1 EnergySum_ROI in the event.";
         REPORT_MESSAGE( MSG::WARNING ) << "Will use the first one...";
      }

      return &( lvl1_roi->getEnergySumROIs().front() );
   }

} // namespace D3PD
