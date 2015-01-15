/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigConfL1Data/TriggerThreshold.h"

#include "TrigT1CTP/CTPTriggerThreshold.h"

namespace LVL1CTP {

   CTPTriggerThreshold::CTPTriggerThreshold( const TrigConf::TriggerThreshold* parent ) {

      m_parent = parent;
      m_value = -1;
      m_start_bit = -1;
      m_end_bit = -1;
      // m_pit_start = -1;
      //    m_pit_end = -1;

   }

   CTPTriggerThreshold::~CTPTriggerThreshold() {

   }

   const TrigConf::TriggerThreshold* CTPTriggerThreshold::threshold() const {

      return m_parent;

   }

   int CTPTriggerThreshold::value() const {

      return m_value;

   }

   void CTPTriggerThreshold::setValue( int value ) {

      m_value = value;

   }

   unsigned int CTPTriggerThreshold::startBit() const {
      return m_start_bit;
   }

   void CTPTriggerThreshold::setStartBit( unsigned int start ) {

      m_start_bit = start;

   }

   unsigned int CTPTriggerThreshold::endBit() const {

      return m_end_bit;

   }

   void CTPTriggerThreshold::setEndBit( unsigned int end ) {

      m_end_bit = end;

   }
  
   //  int CTPTriggerThreshold::pitStart() const {
   //    
   //    return m_pit_start;
   //    
   //  }
   //  
   //  void CTPTriggerThreshold::setPitStart( int pit_start ) {
   //    
   //    m_pit_start = pit_start;
   //    
   //  }
   //  
   //  int CTPTriggerThreshold::pitEnd() const {
   //    
   //    return m_pit_end;
   //    
   //  }
   //  
   //  void CTPTriggerThreshold::setPitEnd( int pit_end ) {
   //    
   //    m_pit_end = pit_end;
   //    
   //  }

} // namespace LVL1CTP
