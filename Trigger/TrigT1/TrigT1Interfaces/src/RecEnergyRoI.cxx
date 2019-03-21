/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
// $Id: RecEnergyRoI.cxx 342657 2011-01-28 23:42:18Z watsona $
/***************************************************************************
                         RecEnergyRoI.cxx  -  description
                            -------------------
   begin                : Mon Jan 22 2001
   email                : moyse@heppch.ph.qmw.ac.uk
***************************************************************************/


// Trigger config includes:
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/TriggerThreshold.h"
#include "TrigConfL1Data/TriggerThresholdValue.h"

// Local include(s):
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

using namespace std;
using namespace TrigConf;

namespace LVL1 {

   RecEnergyRoI::RecEnergyRoI( unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                               const std::vector< TriggerThreshold* >* caloThresholds ) {

      this->construct( RoIWord0, RoIWord1, RoIWord2, caloThresholds );

   }

   RecEnergyRoI::~RecEnergyRoI() {

   }
  
   /** The "construct" call unpacks the RoIWord and copies information about the passed thresholds
       from the vector of TriggerThresholds into maps for later retrieval.

       One warning here: the methods of the RecRoI classes assume threshold numbering starts at 1,
       rather than 0 in the calorimeter trigger simulation and Lvl1ConfigSvc. There will be some
       tweaks in this code to make this work. */
   void RecEnergyRoI::construct( unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                                 const std::vector< TriggerThreshold* >* caloThresholds ) {

      m_roiWord0 = RoIWord0;
      m_roiWord1 = RoIWord1;
      m_roiWord2 = RoIWord2;

      // Copy threshold values into maps for matching to list of thresholds passed
      std::map< int, TriggerThreshold* > etMissMap;
      std::map< int, TriggerThreshold* > sumEtMap;
      std::map< int, TriggerThreshold* > mEtSigMap;
      for( std::vector< TriggerThreshold* >::const_iterator it = caloThresholds->begin();
           it != caloThresholds->end(); ++it ) {
         if( ( *it )->type() == L1DataDef::xeType() ) {
            int num = ( *it )->thresholdNumber();
            etMissMap.insert( std::map< int, TriggerThreshold* >::value_type( num, *it ) );
         }
         else if( ( *it )->type() == L1DataDef::teType() ) {
            int num = ( *it )->thresholdNumber();
            sumEtMap.insert( std::map< int, TriggerThreshold* >::value_type( num, *it ) );
         }
         else if( ( *it )->type() == L1DataDef::xsType() ) {
            int num = ( *it )->thresholdNumber();
            mEtSigMap.insert( std::map< int, TriggerThreshold* >::value_type( num, *it ) );
         }
      }

      // Find which thresholds this RoI passed and copy the values into the maps
      vector< unsigned int >* passed_etmiss = this->etMissThresholdsPassed();
      for( std::vector<unsigned int>::const_iterator it = passed_etmiss->begin();
           it != passed_etmiss->end(); ++it ) {
         std::map<int, TriggerThreshold*>::const_iterator thr = etMissMap.find(*it - 1);
         if (thr != etMissMap.end()) {
            TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
            m_etMissThresholdValue.insert(std::map<int, unsigned int>::value_type(*it, ttv->thresholdValueCount()));
         }
      }
      delete passed_etmiss; // We *have to* delete this vector by hand

      vector< unsigned int >* passed_sumet  = this->sumEtThresholdsPassed();
      for( std::vector<unsigned int>::const_iterator it = passed_sumet->begin();
           it != passed_sumet->end(); ++it ) {
         std::map<int, TriggerThreshold*>::const_iterator thr = sumEtMap.find(*it - 1);
         if (thr != sumEtMap.end()) {
            TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
            m_sumEtThresholdValue.insert(std::map<int, unsigned int>::value_type(*it, ttv->thresholdValueCount()));
         }
      }
      delete passed_sumet; // We *have to* delete this vector by hand

      vector< unsigned int >* passed_metsig  = this->mEtSigThresholdsPassed();
      for( std::vector<unsigned int>::const_iterator it = passed_metsig->begin();
           it != passed_metsig->end(); ++it ) {
         std::map<int, TriggerThreshold*>::const_iterator thr = mEtSigMap.find(*it - 1);
         if (thr != mEtSigMap.end()) {
            TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
            m_mEtSigThresholdValue.insert(std::map<int, unsigned int>::value_type(*it, ttv->thresholdValueCount()));
         }
      }
      delete passed_metsig; // We *have to* delete this vector by hand

      return;

   }

   unsigned int RecEnergyRoI::roiWord0() const {

      return m_roiWord0;

   }

   unsigned int RecEnergyRoI::roiWord1() const {

      return m_roiWord1;

   }

   unsigned int RecEnergyRoI::roiWord2() const {

      return m_roiWord2;

   }

   /** returns TRUE if threshold number <em>threshold_number</em> has been pass
       ed by this ROI. */
   bool RecEnergyRoI::passedEtMissThreshold( int thresholdNumber ) const {

      if ( this->isValidEtMissThreshold( thresholdNumber ) ) {
         JEPRoIDecoder get;
         return ( get.etMissThresholdPassed( m_roiWord2, thresholdNumber ) );
      }
      return false;

   }

   /** returns TRUE if threshold number <em>threshold_number</em> has been pass
       ed by this ROI. */
   bool RecEnergyRoI::passedSumEtThreshold( int thresholdNumber ) const {

      if ( this->isValidSumEtThreshold( thresholdNumber ) ) {
         JEPRoIDecoder get;
         return ( get.sumEtThresholdPassed( m_roiWord1, thresholdNumber ) );
      }
      return false;

   }

   /** returns TRUE if threshold number <em>threshold_number</em> has been pass
       ed by this ROI. */
   bool RecEnergyRoI::passedMEtSigThreshold( int thresholdNumber ) const {

      if ( this->isValidSumEtThreshold( thresholdNumber ) ) {
         JEPRoIDecoder get;
         return ( get.mEtSigThresholdPassed( m_roiWord0, thresholdNumber ) );
      }
      return false;

   }

   /** returns a vector of thresholds passed. */
   vector< unsigned int >* RecEnergyRoI::etMissThresholdsPassed() const {

      JEPRoIDecoder get;
      vector< unsigned int >* newVec = new std::vector< unsigned int >
        ( get.etMissThresholdsPassed( m_roiWord2 )  );
      return newVec;

   }

   /** returns a vector of thresholds passed. */
   vector< unsigned int >* RecEnergyRoI::sumEtThresholdsPassed() const {

      JEPRoIDecoder get;
      vector< unsigned int >* newVec = new std::vector< unsigned int >
        ( get.etSumThresholdsPassed( m_roiWord1 ) );
      return newVec;

   }

   /** returns a vector of thresholds passed. */
   vector< unsigned int >* RecEnergyRoI::mEtSigThresholdsPassed() const {

      JEPRoIDecoder get;
      vector< unsigned int >* newVec = new std::vector< unsigned int >
        (get.mEtSigThresholdsPassed( m_roiWord0 ));
      return newVec;

   }

   /** returns the value of the trigger threshold for the threshold passed.
       The trigger threshold is also sometimes called the cluster threshold.*/
   unsigned int RecEnergyRoI::etMissTriggerThreshold( const unsigned int thresh ) const {

      std::map< int, unsigned int >::const_iterator it = m_etMissThresholdValue.find( thresh );
      if( it != m_etMissThresholdValue.end() ) {
         return it->second;
      }
      return TrigT1CaloDefs::Error;

   }

   /** returns the value of the trigger threshold for the threshold passed.
       The trigger threshold is also sometimes called the cluster threshold.*/
   unsigned int RecEnergyRoI::sumEtTriggerThreshold( const unsigned int thresh ) const {

      std::map< int, unsigned int >::const_iterator it = m_sumEtThresholdValue.find( thresh );
      if( it != m_sumEtThresholdValue.end() ) {
         return it->second;
      }
      return TrigT1CaloDefs::Error;

   }

   /** returns the value of the trigger threshold for the threshold passed.
       The trigger threshold is also sometimes called the cluster threshold.*/
   unsigned int RecEnergyRoI::mEtSigTriggerThreshold( const unsigned int thresh ) const {

      std::map< int, unsigned int >::const_iterator it = m_mEtSigThresholdValue.find( thresh );
      if( it != m_mEtSigThresholdValue.end() ) {
         return it->second;
      }
      return TrigT1CaloDefs::Error;

   }

   /**returns true if thresh is a valid threshold number */
   bool RecEnergyRoI::isValidEtMissThreshold( const unsigned int thresh ) const {

      return ( thresh <= TrigT1CaloDefs::numOfMissingEtThresholds );    

   }

   /**returns true if thresh is a valid threshold number */
   bool RecEnergyRoI::isValidSumEtThreshold( const unsigned int thresh ) const {

      return ( thresh <= TrigT1CaloDefs::numOfSumEtThresholds );

   }

   /**returns true if thresh is a valid threshold number */
   bool RecEnergyRoI::isValidMEtSigThreshold( const unsigned int thresh ) const {

      return ( thresh <= TrigT1CaloDefs::numOfMEtSigThresholds );

   }

   /** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
       then zero will be returned. */
   int RecEnergyRoI::energyX() const {

      JEPRoIDecoder get;
      return get.energyX( m_roiWord0 );

   }

   /** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
       then zero will be returned. */
   int RecEnergyRoI::energyY() const {

      JEPRoIDecoder get;
      return get.energyY( m_roiWord1 );

   }

   /** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
       then zero will be returned. */
   int RecEnergyRoI::energyT() const {

      JEPRoIDecoder get;
      return get.energyT( m_roiWord2 );

   }

   /** returns the Ex overflow bit */
   bool RecEnergyRoI::overflowX() const {

      JEPRoIDecoder get;
      return get.energyOverflow( m_roiWord0 );

   }

   /** returns the Ey overflow bit */
   bool RecEnergyRoI::overflowY() const {

      JEPRoIDecoder get;
      return get.energyOverflow( m_roiWord1 );

   }

   /** returns the ETsum overflow bit */
   bool RecEnergyRoI::overflowT() const {

      JEPRoIDecoder get;
      return get.energyOverflow( m_roiWord2 );

   }

} // namespace LVL1
