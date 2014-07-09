/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigConf_XMLStorageMgr
#define TrigConf_XMLStorageMgr

#include "TrigConfBase/TrigConfMessaging.h"
#include "TrigConfStorage/StorageMgrBase.h"
#include "TrigConfStorage/IStorageMgr.h"

#include <string>
#include <memory>

#include <boost/property_tree/ptree_fwd.hpp>

namespace TrigConf {

   /**@brief XML Storage Manager, controls the different loader classes
      for XML access and the XML nodes*/
   class XMLStorageMgr : public StorageMgrBase, public TrigConfMessaging, virtual public IStorageMgr {
   public:
    
      /**@brief constructor to specify variable list of input files
       * 
       * @param files menu file names (random order allowed, maximum one per level L1Topo, L1, HLT)
       */
      XMLStorageMgr( const std::vector<std::string>& files );
     
      /**@brief destructor*/
      virtual ~XMLStorageMgr();

      // setting log level of all loaders
      void setLevel(MSGTC::Level lvl);

      // provide CTP and L1 version to be accessible by all the L1 loaders
      void setCTPVersion(unsigned int v) { m_ctpVersion = v; }
      void setL1Version(unsigned int v) { m_l1Version = v; }
      unsigned int ctpVersion() const { return m_ctpVersion; }
      unsigned int l1Version() const { return m_l1Version; }


      // Mandatory interface implementation
      virtual IL1TopoMenuLoader&             l1topoMenuLoader();
      virtual IMasterTableLoader&            masterTableLoader();
      virtual IMenuLoader&                   menuLoader();
      virtual IMuctpiLoader&                 muctpiLoader();
      virtual IDeadTimeLoader&               deadTimeLoader();
      virtual IPrescaledClockLoader&         prescaledClockLoader();
      virtual IRandomLoader&                 randomLoader();
      virtual IThresholdConfigLoader&        thresholdConfigLoader();
      virtual ITriggerThresholdLoader&       triggerThresholdLoader();
      virtual ITriggerThresholdValueLoader&  triggerThresholdValueLoader();
      virtual IThresholdMonitorLoader&       thresholdMonitorLoader();
      virtual ICTPFilesLoader&               ctpFilesLoader();
      virtual ICaloJetInputLoader&           caloJetInputLoader();
      virtual ICaloSinCosLoader&             caloSinCosLoader();
      virtual ICaloInfoLoader&               caloInfoLoader();
      virtual ILutCamLoader&                 lutcamLoader();
      virtual IPrescaleSetLoader&            prescaleSetLoader();
      virtual IPrioritySetLoader&            prioritySetLoader();
      virtual IBunchGroupLoader&             bunchGroupLoader();
      virtual IBunchGroupSetLoader&          bunchGroupSetLoader();
      virtual ICTPConfigLoader&              ctpConfigLoader();
      virtual ICTPConfigLoader&              ctpConfigOnlineLoader();
      virtual IHLTFrameLoader&               hltFrameLoader();
      virtual IJobOptionTableLoader&         jobOptionTableLoader();


      bool hasL1Topo() const { return m_runL1Topo; }
      bool hasLVL1() const { return m_runLVL1; }
      bool hasHLT() const { return m_runHLT; }

      std::string m_cs = "";

      // xml files
      std::string m_xmlL1TopoFile = "";
      std::string m_xmlL1File = "";
      std::string m_xmlHLTFile = "";

   private:




      void Init(const std::vector<std::string>& files = std::vector<std::string>());

      // allow also for running only LVL1 or only HLT
      bool m_runL1Topo = false;
      bool m_runLVL1 = false;
      bool m_runHLT = false;

      //std::unique_ptr<boost::property_tree::ptree> m_l1topomenu;
      boost::property_tree::ptree * m_l1topomenu = { nullptr };
      boost::property_tree::ptree * m_l1menu = { nullptr };
      boost::property_tree::ptree * m_hltmenu = { nullptr };

   };
}
#endif
