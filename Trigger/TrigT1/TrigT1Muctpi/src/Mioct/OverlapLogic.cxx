/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: OverlapLogic.cxx 650693 2015-03-01 16:53:48Z masato $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "OverlapLogic.h"
#include "NullStrategy.h"
#include "LUTStrategy.h"
#include "../Common/PtMultiplicitySet.h"
#include "MioctSectorSet.h"
#include "MultiplicityLogic.h"

//***********************************************************************
//
//       Version : $Revision: 650693 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2015-03-01 17:53:48 +0100 (Sun, 01 Mar 2015) $
//
//
//
//***********************************************************************

namespace LVL1MUCTPI {

   OverlapLogic::OverlapLogic( MultiplicityLogic* logic )
      : m_logger( "OverlapLogic" ), m_multiplicityLogic( logic ),
        m_overlapStrategyMap() {

      m_overlapStrategyMap[ NO_OVERLAP ] =
         new NullStrategy( m_multiplicityLogic );
      m_overlapStrategyMap[ LUT_OVERLAP ] =
         new LUTStrategy( m_multiplicityLogic );

      // set a default Strategy: no overlap consideration
      m_currentStrategy = m_overlapStrategyMap[ NO_OVERLAP ];
      REPORT_MSG(DEBUG, "Overlap Logic with default strategy " << m_currentStrategy->getName() << " installed.");
   }

   OverlapLogic::~OverlapLogic() {

      std::map< StrategyName, OverlapStrategy* >::iterator itr =
         m_overlapStrategyMap.begin();
      std::map< StrategyName, OverlapStrategy* >::iterator end =
         m_overlapStrategyMap.end();
      for( ; itr != end; ++itr ) {
         delete itr->second;
      }
      m_overlapStrategyMap.clear();
   }

   PtMultiplicitySet OverlapLogic::calculateMultiplicity( const MioctSectorSet& mioctData,
                                                          const MioctID& mioct ) const {

      PtMultiplicitySet result;
      if ( m_currentStrategy ){
         result = m_currentStrategy->calculateMultiplicity( mioctData, mioct );
      } else {
         REPORT_ERROR_MSG( "No overlap strategy set, cannot calculate multiplicity!" );
      }

      return result;
   }

   void OverlapLogic::changeStrategy( StrategyName newStrategy ) {

      m_currentStrategy = m_overlapStrategyMap[ newStrategy ];
      m_logger << INFO << "Current Strategy changed to " << m_currentStrategy->getName()
               << MsgLogger::endmsg;

      return;
   }

   StatusCode OverlapLogic::initializeLUT( const std::string& lutXMLFile, bool flagMode,
                                           bool dumpLut,const std::string& runPeriod ) {

      LUTStrategy* strat =
         dynamic_cast< LUTStrategy* >( m_overlapStrategyMap[ LUT_OVERLAP ] );
      if( ! strat ) {
         REPORT_FATAL_MSG( "Couldn't cast strategy to LUTStrategy. Coding error!" );
         return StatusCode::FAILURE;
      }

      CHECK( strat->initialize( lutXMLFile, flagMode, dumpLut, runPeriod ) );
      return StatusCode::SUCCESS;
   }

} // namespace LVL1MUCTPI
