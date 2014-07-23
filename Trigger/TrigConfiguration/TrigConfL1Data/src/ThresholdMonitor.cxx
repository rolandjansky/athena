/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/ClusterThresholdValue.h"
#include "TrigConfL1Data/JetThresholdValue.h"
#include "TrigConfL1Data/MuonThresholdValue.h"
#include "TrigConfL1Data/EtThresholdValue.h"
#include "TrigConfL1Data/NimThresholdValue.h"
#include "TrigConfL1Data/L1DataDef.h"
#include "TrigConfL1Data/CaloInfo.h"

#include <stdexcept>
#include <iostream>

using namespace std;

TrigConf::ThresholdMonitor::ThresholdMonitor() :
   L1DataBaseclass()
{}
  
TrigConf::ThresholdMonitor::ThresholdMonitor(const ThresholdMonitor& mon) :
   L1DataBaseclass(mon),
   m_CounterType(mon.m_CounterType), 
   m_ThresholdName(mon.m_ThresholdName),
   m_CtpinSlot(mon.m_CtpinSlot),
   m_CtpinConnector(mon.m_CtpinConnector),
   m_ThresholdId(mon.m_ThresholdId),
   m_BunchGroupId(mon.m_BunchGroupId),
   m_Multiplicity(mon.m_Multiplicity),
   m_InternalCounter(mon.m_InternalCounter),
   m_ThresholdStartBit(mon.m_ThresholdStartBit),
   m_ThresholdEndBit(mon.m_ThresholdEndBit),
   m_ThresholdActive(mon.m_ThresholdActive)
{
   setName(mon.name());
   setVersion(mon.version());
}
 
   TrigConf::ThresholdMonitor::~ThresholdMonitor()
   {
   }


   // Accessors
  
   int TrigConf::ThresholdMonitor::thresholdId() const
   {
      return m_ThresholdId;
   }

   void TrigConf::ThresholdMonitor::setThresholdId( const int& id )
   {
      m_ThresholdId = id;
   }


   int TrigConf::ThresholdMonitor::internalCounter() const
   {
      return m_InternalCounter;
   }

   void TrigConf::ThresholdMonitor::setInternalCounter( const int& internalcounter )
   {
      m_InternalCounter = internalcounter;
   }

   int TrigConf::ThresholdMonitor::multiplicity() const
   {
      return m_Multiplicity;
   }

   void TrigConf::ThresholdMonitor::setMultiplicity( const int& multiplicity)
   {
      m_Multiplicity = multiplicity;
   }

   int TrigConf::ThresholdMonitor::bunchGroupId() const
   {
      return m_BunchGroupId;
   }

   void TrigConf::ThresholdMonitor::setBunchGroupId( const int& bunchgroupid )
   {
      m_BunchGroupId = bunchgroupid;
   }

   std::string TrigConf::ThresholdMonitor::counterType() const
   {
      return m_CounterType;
   }
  
   void TrigConf::ThresholdMonitor::setCounterType(const std::string& countertype )
   {
      m_CounterType = countertype;
   }
 
   uint16_t TrigConf::ThresholdMonitor::ctpinSlot() const
   {
      return m_CtpinSlot;
   }
  
   void TrigConf::ThresholdMonitor::setCtpinSlot(const uint16_t& slot )
   {
      m_CtpinSlot = slot;
   }

   uint16_t TrigConf::ThresholdMonitor::ctpinConnector() const
   {
      return m_CtpinConnector;
   }
  
   void TrigConf::ThresholdMonitor::setCtpinConnector(const uint16_t& con )
   {
      m_CtpinConnector = con;
   }

   std::string TrigConf::ThresholdMonitor::thresholdName() const
   {
      return m_ThresholdName;
   }

   void TrigConf::ThresholdMonitor::setThresholdName(const std::string& name)
   {
      m_ThresholdName = name;
   }

   int TrigConf::ThresholdMonitor::thresholdStartBit() const
   {
      return m_ThresholdStartBit;
   }

   void TrigConf::ThresholdMonitor::setThresholdStartBit( const int& bit) 
   {
      m_ThresholdStartBit = bit;
   }
   
   int TrigConf::ThresholdMonitor::thresholdEndBit() const
   {
      return m_ThresholdEndBit;
   }

   void TrigConf::ThresholdMonitor::setThresholdEndBit( const int& bit) 
   {
      m_ThresholdEndBit = bit;
   }

   bool TrigConf::ThresholdMonitor::thresholdActive() const
   {
      return m_ThresholdActive;
   }

   void TrigConf::ThresholdMonitor::setThresholdActive( const bool& active)
   {
      m_ThresholdActive = active;
   }
   
   void TrigConf::ThresholdMonitor::print(const std::string& indent, unsigned int /*detail*/) const {
      std::cout << indent << "MonCounter : '" << name() << "'" << std::endl;
      std::cout << indent << "ThresholdId: " << m_ThresholdId << std::endl;
      std::cout << indent << "ThresholdName: " << m_ThresholdName << std::endl;
      std::cout << indent << "Ctpin slot: " << m_CtpinSlot << std::endl;
      std::cout << indent << "Connector: " << m_CtpinConnector << std::endl;
      std::cout << indent << "Multiplicity: " << m_Multiplicity << std::endl;
      std::cout << indent << "Start bit: " << m_ThresholdStartBit << std::endl; 
      std::cout << indent << "End bit: " << m_ThresholdEndBit << std::endl; 
      std::cout << indent << "Active: " << m_ThresholdActive << std::endl;
      std::cout << indent << "InternalCounter: " << m_InternalCounter << std::endl;
      std::cout << indent << "BunchGroupId: " << m_BunchGroupId << std::endl;
      std::cout << indent << "CounterType: " << m_CounterType << std::endl;
   }
