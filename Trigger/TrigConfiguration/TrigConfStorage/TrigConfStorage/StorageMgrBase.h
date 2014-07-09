/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_StorageMgrBase
#define TrigConf_StorageMgrBase

#include <string>
#include "TrigConfBase/MsgStream.h"
#include "TrigConfStorage/IStorageMgr.h"

namespace TrigConf {

   class IMasterTableLoader;
   class IMenuLoader;                 
   class IMuctpiLoader;               
   class IDeadTimeLoader;             
   class IPrescaledClockLoader;       
   class IRandomLoader;               
   class IThresholdConfigLoader;      
   class ITriggerThresholdLoader;     
   class ITriggerThresholdValueLoader;
   class IThresholdMonitorLoader;     
   class ICTPFilesLoader;             
   class ICaloJetInputLoader;         
   class ICaloSinCosLoader;           
   class ICaloInfoLoader;             
   class ILutCamLoader;               
   class IPrescaleSetLoader;          
   class IPrioritySetLoader;          
   class IBunchGroupLoader;           
   class IBunchGroupSetLoader;        
   class ICTPConfigLoader;            
   class IMuonThresholdSetLoader;     
   class IHLTFrameLoader;             
   class IJobOptionTableLoader;       
   class IL1TopoMenuLoader;

   class StorageMgrBase : virtual public IStorageMgr {
   public:
    
      /** @brief constructor */
      StorageMgrBase();
     
      /** @brief destructor */
      virtual ~StorageMgrBase();

      // setting log level of all loaders
      virtual void setLevel(MSGTC::Level lvl) = 0;

   protected:

      IMasterTableLoader *              m_masterTableLoader { nullptr };
      IMenuLoader *                     m_menuLoader { nullptr };
      IMuctpiLoader *                   m_muctpiLoader { nullptr };
      IDeadTimeLoader *                 m_deadTimeLoader { nullptr };
      IPrescaledClockLoader *           m_prescaledClockLoader { nullptr };
      IRandomLoader *                   m_randomLoader { nullptr };
      IThresholdConfigLoader *          m_thresholdConfigLoader { nullptr };
      ITriggerThresholdLoader *         m_triggerThresholdLoader { nullptr };
      ITriggerThresholdValueLoader *    m_triggerThresholdValueLoader { nullptr };
      IThresholdMonitorLoader *         m_thresholdMonitorLoader { nullptr }; 	 
      ICTPFilesLoader *                 m_ctpFilesLoader { nullptr };
      ICaloJetInputLoader *             m_caloJetInputLoader { nullptr };
      ICaloSinCosLoader *               m_caloSinCosLoader { nullptr };
      ICaloInfoLoader *                 m_caloInfoLoader { nullptr };
      ILutCamLoader *                   m_lutCamLoader { nullptr };
      IPrescaleSetLoader *              m_prescaleSetLoader { nullptr };
      IPrioritySetLoader *              m_prioritySetLoader { nullptr };
      IBunchGroupLoader *               m_bunchGroupLoader { nullptr };
      IBunchGroupSetLoader *            m_bunchGroupSetLoader { nullptr };
      ICTPConfigLoader *                m_ctpConfigLoader { nullptr };
      IMuonThresholdSetLoader *         m_muonThresholdSetLoader { nullptr };
      IHLTFrameLoader *                 m_HLTFrameLoader { nullptr };    
      IJobOptionTableLoader *           m_jobOptionTableLoader { nullptr };
      IL1TopoMenuLoader *               m_l1topoMenuLoader { nullptr };

      unsigned int   m_ctpVersion { 0 };
      unsigned int   m_l1Version { 0 };

   };

}

#endif
