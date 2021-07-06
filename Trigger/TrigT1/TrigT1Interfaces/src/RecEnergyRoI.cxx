/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
#include "TrigConfData/L1Menu.h"
#include "TrigConfData/L1Threshold.h"

// Local include(s):
#include "TrigT1Interfaces/RecEnergyRoI.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

using namespace std;
using namespace TrigConf;

LVL1::RecEnergyRoI::RecEnergyRoI(unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                                 const std::vector<TrigConf::TriggerThreshold *> *caloThresholds)
{

   this->construct(RoIWord0, RoIWord1, RoIWord2, caloThresholds);
}

LVL1::RecEnergyRoI::RecEnergyRoI(unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                                 const TrigConf::L1Menu *const l1menu)
    : m_roiWord0(RoIWord0),
      m_roiWord1(RoIWord1),
      m_roiWord2(RoIWord2)
{
   construct(l1menu);
}

//copy constructor
LVL1::RecEnergyRoI::RecEnergyRoI(const LVL1::RecEnergyRoI &obj)
    : m_roiWord0(obj.m_roiWord0),
      m_roiWord1(obj.m_roiWord1),
      m_roiWord2(obj.m_roiWord2),
      m_etMissThresholdValue(obj.m_etMissThresholdValue),
      m_sumEtThresholdValue(obj.m_sumEtThresholdValue),
      m_mEtSigThresholdValue(obj.m_mEtSigThresholdValue)
{
}

//assignment operator
LVL1::RecEnergyRoI &
LVL1::RecEnergyRoI::operator=(const LVL1::RecEnergyRoI &rhs)
{
   if(this != &rhs) {
      m_roiWord0 = rhs.m_roiWord0;
      m_roiWord1 = rhs.m_roiWord1;
      m_roiWord2 = rhs.m_roiWord2;
      m_etMissThresholdValue = rhs.m_etMissThresholdValue;
      m_sumEtThresholdValue = rhs.m_sumEtThresholdValue;
      m_mEtSigThresholdValue = rhs.m_mEtSigThresholdValue;
   }
   return *this;
}

LVL1::RecEnergyRoI::~RecEnergyRoI() {}

/** The "construct" call unpacks the RoIWord and copies information about the passed thresholds
    from the vector of TriggerThresholds into maps for later retrieval.

      One warning here: the methods of the RecRoI classes assume threshold numbering starts at 1,
      rather than 0 in the calorimeter trigger simulation and Lvl1ConfigSvc. There will be some
      tweaks in this code to make this work. */
void LVL1::RecEnergyRoI::construct(unsigned int RoIWord0, unsigned int RoIWord1, unsigned int RoIWord2,
                                   const std::vector<TriggerThreshold *> *caloThresholds)
{
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
   for (unsigned int xeMapping : etMissThresholdsPassed())
   {
      std::map<int, TriggerThreshold *>::const_iterator thr = etMissMap.find(xeMapping - 1);
      if (thr != etMissMap.end()) {
         TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
         m_etMissThresholdValue.insert(std::map<int, unsigned int>::value_type(xeMapping, ttv->thresholdValueCount()));
      }
   }

   for (unsigned int teMapping : sumEtThresholdsPassed())
   {
      std::map<int, TriggerThreshold *>::const_iterator thr = sumEtMap.find(teMapping - 1);
      if (thr != sumEtMap.end()) {
         TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
         m_sumEtThresholdValue.insert(std::map<int, unsigned int>::value_type(teMapping, ttv->thresholdValueCount()));
      }
   }

   for (unsigned int xsMapping : mEtSigThresholdsPassed())
   {
      std::map<int, TriggerThreshold *>::const_iterator thr = mEtSigMap.find(xsMapping - 1);
      if (thr != mEtSigMap.end()) {
         TriggerThresholdValue* ttv = thr->second->triggerThresholdValue(0,0);
         m_mEtSigThresholdValue.insert(std::map<int, unsigned int>::value_type(xsMapping, ttv->thresholdValueCount()));
      }
   }
}

void LVL1::RecEnergyRoI::construct(const TrigConf::L1Menu *const l1menu)
{
   // Find which thresholds this RoI passed and copy the values into the maps
   for (unsigned int xeMapping : etMissThresholdsPassed())
   {
      const TrigConf::L1Threshold &thr = l1menu->threshold("XE", xeMapping - 1);
      auto &caloThr = dynamic_cast<const TrigConf::L1Threshold_Calo &>(thr);
      m_etMissThresholdValue[xeMapping] = caloThr.thrValueCounts();
   }

   for (unsigned int teMapping : sumEtThresholdsPassed())
   {
      const TrigConf::L1Threshold &thr = l1menu->threshold("TE", teMapping - 1);
      auto &caloThr = dynamic_cast<const TrigConf::L1Threshold_Calo &>(thr);
      m_sumEtThresholdValue[teMapping] = caloThr.thrValueCounts();
   }

   for (unsigned int xsMapping : mEtSigThresholdsPassed())
   {
      const TrigConf::L1Threshold &thr = l1menu->threshold("XS", xsMapping - 1);
      auto &caloThr = dynamic_cast<const TrigConf::L1Threshold_Calo &>(thr);
      m_mEtSigThresholdValue[xsMapping] = caloThr.thrValueCounts();
   }
}

unsigned int LVL1::RecEnergyRoI::roiWord0() const
{

   return m_roiWord0;
}

unsigned int LVL1::RecEnergyRoI::roiWord1() const
{

   return m_roiWord1;
}

unsigned int LVL1::RecEnergyRoI::roiWord2() const
{

   return m_roiWord2;
}

/** returns TRUE if threshold number <em>threshold_number</em> has been pass
    ed by this ROI. */
bool LVL1::RecEnergyRoI::passedEtMissThreshold(int thresholdNumber) const
{

   if ( this->isValidEtMissThreshold( thresholdNumber ) ) {
      JEPRoIDecoder get;
      return ( get.etMissThresholdPassed( m_roiWord2, thresholdNumber ) );
   }
   return false;
}

/** returns TRUE if threshold number <em>threshold_number</em> has been pass
    ed by this ROI. */
bool LVL1::RecEnergyRoI::passedSumEtThreshold(int thresholdNumber) const
{

   if ( this->isValidSumEtThreshold( thresholdNumber ) ) {
      JEPRoIDecoder get;
      return ( get.sumEtThresholdPassed( m_roiWord1, thresholdNumber ) );
   }
   return false;
}

/** returns TRUE if threshold number <em>threshold_number</em> has been pass
    ed by this ROI. */
bool LVL1::RecEnergyRoI::passedMEtSigThreshold(int thresholdNumber) const
{

   if ( this->isValidSumEtThreshold( thresholdNumber ) ) {
      JEPRoIDecoder get;
      return ( get.mEtSigThresholdPassed( m_roiWord0, thresholdNumber ) );
   }
   return false;
}

/** returns a vector of thresholds passed. */
vector<unsigned int> LVL1::RecEnergyRoI::etMissThresholdsPassed() const
{
   JEPRoIDecoder get;
   return get.etMissThresholdsPassed(m_roiWord2);
}

/** returns a vector of thresholds passed. */
vector<unsigned int> LVL1::RecEnergyRoI::sumEtThresholdsPassed() const
{
   JEPRoIDecoder get;
   return get.etSumThresholdsPassed(m_roiWord1);
}

/** returns a vector of thresholds passed. */
vector<unsigned int> LVL1::RecEnergyRoI::mEtSigThresholdsPassed() const
{
   JEPRoIDecoder get;
   return get.mEtSigThresholdsPassed(m_roiWord0);
}

/** returns the value of the trigger threshold for the threshold passed.
    The trigger threshold is also sometimes called the cluster threshold.*/
unsigned int LVL1::RecEnergyRoI::etMissTriggerThreshold(const unsigned int thresh) const
{
   std::map< int, unsigned int >::const_iterator it = m_etMissThresholdValue.find( thresh );
   if( it != m_etMissThresholdValue.end() ) {
      return it->second;
   }
   return TrigT1CaloDefs::Error;
}

/** returns the value of the trigger threshold for the threshold passed.
    The trigger threshold is also sometimes called the cluster threshold.*/
unsigned int LVL1::RecEnergyRoI::sumEtTriggerThreshold(const unsigned int thresh) const
{

   std::map< int, unsigned int >::const_iterator it = m_sumEtThresholdValue.find( thresh );
   if( it != m_sumEtThresholdValue.end() ) {
      return it->second;
   }
   return TrigT1CaloDefs::Error;
}

/** returns the value of the trigger threshold for the threshold passed.
    The trigger threshold is also sometimes called the cluster threshold.*/
unsigned int LVL1::RecEnergyRoI::mEtSigTriggerThreshold(const unsigned int thresh) const
{

   std::map< int, unsigned int >::const_iterator it = m_mEtSigThresholdValue.find( thresh );
   if( it != m_mEtSigThresholdValue.end() ) {
      return it->second;
   }
   return TrigT1CaloDefs::Error;
}

/**returns true if thresh is a valid threshold number */
bool LVL1::RecEnergyRoI::isValidEtMissThreshold(const unsigned int thresh) const
{

   return (thresh <= TrigT1CaloDefs::numOfMissingEtThresholds);
}

/**returns true if thresh is a valid threshold number */
bool LVL1::RecEnergyRoI::isValidSumEtThreshold(const unsigned int thresh) const
{

   return (thresh <= TrigT1CaloDefs::numOfSumEtThresholds);
}

/**returns true if thresh is a valid threshold number */
bool LVL1::RecEnergyRoI::isValidMEtSigThreshold(const unsigned int thresh) const
{

   return (thresh <= TrigT1CaloDefs::numOfMEtSigThresholds);
}

/** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
    then zero will be returned. */
int LVL1::RecEnergyRoI::energyX() const
{

   JEPRoIDecoder get;
   return get.energyX(m_roiWord0);
}

/** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
    then zero will be returned. */
int LVL1::RecEnergyRoI::energyY() const
{

   JEPRoIDecoder get;
   return get.energyY(m_roiWord1);
}

/** returns the (signed) Ex energy projection. If the RoIWord looks invalid,
    then zero will be returned. */
int LVL1::RecEnergyRoI::energyT() const
{
   JEPRoIDecoder get;
   return get.energyT(m_roiWord2);
}

/** returns the Ex overflow bit */
bool LVL1::RecEnergyRoI::overflowX() const
{
   JEPRoIDecoder get;
   return get.energyOverflow(m_roiWord0);
}

/** returns the Ey overflow bit */
bool LVL1::RecEnergyRoI::overflowY() const
{
   JEPRoIDecoder get;
   return get.energyOverflow(m_roiWord1);
}

/** returns the ETsum overflow bit */
bool LVL1::RecEnergyRoI::overflowT() const
{
   JEPRoIDecoder get;
   return get.energyOverflow(m_roiWord2);
}
