/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/StorageMgrBase.h"

TrigConf::StorageMgrBase::StorageMgrBase()
{}
     
TrigConf::StorageMgrBase::~StorageMgrBase()
{
   delete m_masterTableLoader;
   delete m_menuLoader;
   delete m_muctpiLoader;
   delete m_deadTimeLoader;
   delete m_prescaledClockLoader;
   delete m_randomLoader;
   delete m_thresholdConfigLoader;
   delete m_triggerThresholdLoader;
   delete m_triggerThresholdValueLoader;
   delete m_thresholdMonitorLoader; 	 
   delete m_ctpFilesLoader;
   delete m_caloJetInputLoader;
   delete m_caloSinCosLoader;
   delete m_caloInfoLoader;
   delete m_lutCamLoader;
   delete m_prescaleSetLoader;
   delete m_prioritySetLoader;
   delete m_bunchGroupLoader;
   delete m_bunchGroupSetLoader;
   delete m_ctpConfigLoader;
   delete m_muonThresholdSetLoader;
   delete m_HLTFrameLoader;    
   delete m_jobOptionTableLoader;
   delete m_l1topoMenuLoader;
}

