/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MioctModule.cxx 795106 2017-02-02 11:21:27Z wengler $

// STL include(s):
#include <sstream>
#include <iostream>
#include<string>

// Local include(s):
#include "MioctModule.h"
#include "OverlapLogic.h"
#include "EventReader.h"

//***********************************************************************
//
//       Version : $Revision: 795106 $
//
//   Description :
//
//        Author : $Author: krasznaa $
//          Date : $Date: 2017-02-02 12:21:27 +0100 (Thu, 02 Feb 2017) $
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

      if( m_logger.msgLvl(LVL1MUCTPI::VERBOSE) ) {
        m_mioctSectors.print();
      }

      return m_lvl1OverlapLogic->calculateMultiplicity( m_mioctSectors, m_ID ) ;
   }

   LVL1::MuCTPIL1Topo MioctModule::getL1TopoCandidates(MioctL1TopoConverter & l1TopoConv) const{
     LVL1::MuCTPIL1Topo result;
     std::vector<LVL1::MuCTPIL1TopoCandidate> candList;
     
     // Add the unsupressed barrel candidates in the same order as in the hardware - 
     // this is important as the last ones added are taken after sorting if 
     // there are more than two candidates per Mioct
     // BA31, BA32, BA01, BA02, EC47, EC00, EC01, EC02, EC03, EC04, FW00, FW01, FW02
     // equivalent to Barrel1-4, Endcap1-6, Forward1-3

     if(  ! m_mioctSectors.Barrel1.getCand1Supressed() &&  m_mioctSectors.Barrel1.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel1, true ));

     if(  ! m_mioctSectors.Barrel1.getCand2Supressed() &&  m_mioctSectors.Barrel1.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel1, false ));

     if(  ! m_mioctSectors.Barrel2.getCand1Supressed() &&  m_mioctSectors.Barrel2.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel2, true ));

     if(  ! m_mioctSectors.Barrel2.getCand2Supressed() &&  m_mioctSectors.Barrel2.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel2, false ));

     if(  ! m_mioctSectors.Barrel3.getCand1Supressed() &&  m_mioctSectors.Barrel3.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel3, true ));

     if(  ! m_mioctSectors.Barrel3.getCand2Supressed() &&  m_mioctSectors.Barrel3.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel3, false ));

     if(  ! m_mioctSectors.Barrel4.getCand1Supressed() &&  m_mioctSectors.Barrel4.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel4, true ));

     if(  ! m_mioctSectors.Barrel4.getCand2Supressed() &&  m_mioctSectors.Barrel4.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Barrel4, false ));

 
     if(  ! m_mioctSectors.Endcap1.getCand1Supressed() &&  m_mioctSectors.Endcap1.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap1, true ));

     if(  ! m_mioctSectors.Endcap1.getCand2Supressed() &&  m_mioctSectors.Endcap1.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap1, false ));

     if(  ! m_mioctSectors.Endcap2.getCand1Supressed() &&  m_mioctSectors.Endcap2.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap2, true ));

     if(  ! m_mioctSectors.Endcap2.getCand2Supressed() &&  m_mioctSectors.Endcap2.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap2, false ));

     if(  ! m_mioctSectors.Endcap3.getCand1Supressed() &&  m_mioctSectors.Endcap3.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap3, true ));

     if(  ! m_mioctSectors.Endcap3.getCand2Supressed() &&  m_mioctSectors.Endcap3.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap3, false ));

     if(  ! m_mioctSectors.Endcap4.getCand1Supressed() &&  m_mioctSectors.Endcap4.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap4, true ));

     if(  ! m_mioctSectors.Endcap4.getCand2Supressed() &&  m_mioctSectors.Endcap4.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap4, false ));

     if(  ! m_mioctSectors.Endcap5.getCand1Supressed() &&  m_mioctSectors.Endcap5.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap5, true ));

     if(  ! m_mioctSectors.Endcap5.getCand2Supressed() &&  m_mioctSectors.Endcap5.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap5, false ));

     if(  ! m_mioctSectors.Endcap6.getCand1Supressed() &&  m_mioctSectors.Endcap6.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap6, true ));

     if(  ! m_mioctSectors.Endcap6.getCand2Supressed() &&  m_mioctSectors.Endcap6.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Endcap6, false ));

     if(  ! m_mioctSectors.Forward1.getCand1Supressed() && m_mioctSectors.Forward1.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward1, true ));

     if(  ! m_mioctSectors.Forward1.getCand2Supressed() && m_mioctSectors.Forward1.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward1, false ));

     if(  ! m_mioctSectors.Forward2.getCand1Supressed() && m_mioctSectors.Forward2.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward2, true ));

     if(  ! m_mioctSectors.Forward2.getCand2Supressed() && m_mioctSectors.Forward2.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward2, false ));

     if(  ! m_mioctSectors.Forward3.getCand1Supressed() && m_mioctSectors.Forward3.getPtCand1() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward3, true ));

     if(  ! m_mioctSectors.Forward3.getCand2Supressed() && m_mioctSectors.Forward3.getPtCand2() !=7 ) 
       candList.push_back(l1TopoConv.convertToL1Topo(m_ID, m_mioctSectors.Forward3, false ));


     // stable_sort on pt threshold ID (1-6) keeping the input order 
     std::stable_sort(candList.begin(), candList.end(), 
      		      [](const LVL1::MuCTPIL1TopoCandidate & cand1,  const LVL1::MuCTPIL1TopoCandidate & cand2)
      		      {
      			return cand1.getptThresholdID() < cand2.getptThresholdID();
      		      });


     // limit to 2 candidates per MIOCT as in the hardware
     if (candList.size() > 2) {
       candList.erase(candList.begin(), candList.end()-2);
       // set second candidate pt to 3 (i.e. 11) to indicate overflow - note sorting is in ascending order, 
       // so last element in vector is highest pt, i.e. first element in vector of two after cutoff is 
       // second candidate
       candList.at(0).setptL1TopoCodeToOvFl();
     } 
    

     result.setCandidates(candList);
     return result;
   
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
