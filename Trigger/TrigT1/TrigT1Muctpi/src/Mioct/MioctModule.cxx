/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctModule.cxx 485324 2012-02-27 17:02:38Z krasznaa $

// STL include(s):
#include <sstream>

// Local include(s):
#include "MioctModule.h"
#include "OverlapLogic.h"
#include "EventReader.h"

//***********************************************************************
//
//       Version : $Revision: 485324 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2012-02-27 18:02:38 +0100 (Mon, 27 Feb 2012) $
//
//
//
//***********************************************************************

namespace LVL1MUCTPI {

   MioctModule::MioctModule( const MioctID& theID, const OverlapLogic* theOverlapLogic,
                             EventReader* reader )
      : m_logger( "MioctModule" ), m_ID( theID ),
        m_mioctSectors( theID, reader ), m_lvl1OverlapLogic( theOverlapLogic ) {

      if( theID.getRapidityRegion() == POSITIVE ) {
         REPORT_VERBOSE_MSG( "Printing sector information for MIOCT with number "
                             << theID.getNumber()
                             << " in the POSITIVE rapidity region" );
      } else {
         REPORT_VERBOSE_MSG( "Printing sector information for MIOCT with number "
                             << theID.getNumber()
                             << " in the NEGATIVE rapidity region" );
      }

      m_mioctSectors.report();
   }

   PtMultiplicitySet MioctModule::calculateMultiplicity() const {

      m_mioctSectors.print();

      return m_lvl1OverlapLogic->calculateMultiplicity( m_mioctSectors, m_ID ) ;
   }

   // implementation of method to get the data of this MIOCT as an
   // object MioctData, which allows easy access to the data in
   // MIBAK format
   MioctData* MioctModule::getMioctOnMibak() {

      m_mioctData.assignData( m_ID, m_mioctSectors, m_lvl1OverlapLogic );
      return &m_mioctData;
   }

   /**
    * This function simulates the behavior of one of the NIM outputs on the front
    * of the MIOCT module, which can be programmed to tell us whether a barrel
    * candidate was seen in the octant.
    *
    * @returns <code>true</code> if there was a barrel candidate in the octant,
    *          <code>false</code> if there was none
    */
   bool MioctModule::hasBarrelCandidate() const {

      if( ( m_mioctSectors.Barrel1.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Barrel1.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Barrel2.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Barrel2.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Barrel3.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Barrel3.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Barrel4.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Barrel4.getPtCand2() != 7 ) ) {

         return true;

      } else {

         return false;
      }
   }

   /**
    * This function simulates the behavior of one of the NIM outputs on the front
    * of the MIOCT module, which can be programmed to tell us whether an endcap
    * candidate was seen in the octant.
    *
    * @returns <code>true</code> if there was an endcap candidate in the octant,
    *          <code>false</code> if there was none
    */
   bool MioctModule::hasEndcapCandidate() const {

      if( ( m_mioctSectors.Endcap1.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap1.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Endcap2.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap2.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Endcap3.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap3.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Endcap4.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap4.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Endcap5.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap5.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Endcap6.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Endcap6.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Forward1.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Forward1.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Forward2.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Forward2.getPtCand2() != 7 ) ||
          ( m_mioctSectors.Forward3.getPtCand1() != 7 ) ||
          ( m_mioctSectors.Forward3.getPtCand2() != 7 ) ) {

         return true;

      } else {

         return false;
      }
   }

} // namespace LVL1MUCTPI
