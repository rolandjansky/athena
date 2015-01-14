/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/XMLStorageMgr.h"

#include "./XMLHLTFrameLoader.h"
#include "./XMLJobOptionTableLoader.h"
#include "./XMLMasterTableLoader.h"
#include "./XMLMenuLoader.h"
#include "./XMLMuctpiLoader.h"
#include "./XMLDeadTimeLoader.h"
#include "./XMLPrescaledClockLoader.h"
#include "./XMLRandomLoader.h"
#include "./XMLThresholdConfigLoader.h"
#include "./XMLPrescaleSetLoader.h"
#include "./XMLPrioritySetLoader.h"
#include "./XMLBunchGroupLoader.h"
#include "./XMLBunchGroupSetLoader.h"
#include "./XMLCTPConfigLoader.h"
#include "./XMLCTPConfigOnlineLoader.h"
#include "./XMLTriggerThresholdLoader.h"
#include "./XMLTriggerThresholdValueLoader.h"
#include "./CaloJetInputLoader.h"
#include "./CaloSinCosLoader.h"
#include "./CaloInfoLoader.h"
#include "./XMLLutCamLoader.h"
#include "./XMLCTPFilesLoader.h"
#include "./XMLThresholdMonitorLoader.h"
#include "./XMLCaloSinCosLoader.h"
#include "./XMLCaloInfoLoader.h"
#include "./XMLL1TopoMenuLoader.h"

#include <iostream>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using namespace std;
using namespace TrigConf;
using namespace boost::property_tree;


namespace {
   // function the returns pointer to menu and the corresponding top level tag, if among the list
   ptree* GetTreeAndType(const string& filename, string& menutype) {
      ptree * menu = new ptree();
      menutype="";
      read_xml<ptree>(filename, *menu);
      for(string x: {"TOPO_MENU", "LVL1Config", "HLT_MENU"})
         if(menu->find(x) != menu->not_found()) {
            menutype = x;
            return menu;
         }
      delete menu;
      return nullptr;
   }
}


XMLStorageMgr::XMLStorageMgr( const std::vector<std::string>& files ) :
   TrigConfMessaging("XMLStorageMgr"),
   m_l1topomenu(new ptree()),
   m_l1menu(new ptree()),
   m_hltmenu(new ptree())
{
   string menutype("");

   for(const string & filename: files) {
      string menutype = "";
      ptree* menu = GetTreeAndType(filename, menutype);
      if(menutype=="TOPO_MENU") {
         TRG_MSG_INFO("Using " << filename << " as L1Topo menu");
         delete m_l1topomenu;
         m_l1topomenu = menu;
         m_xmlL1TopoFile = filename;
         m_runL1Topo = true;
      } else if(menutype=="LVL1Config") {
         TRG_MSG_INFO("Using " << filename << " as L1 menu");
         delete m_l1menu;
         m_l1menu = menu;
         m_xmlL1File = filename;
         m_runLVL1 = true;
      } else if(menutype=="HLT_MENU") {
         TRG_MSG_INFO("Using " << filename << " as HLT menu");
         delete m_hltmenu;
         m_hltmenu = menu;
         m_xmlHLTFile = filename;
         m_runHLT = true;
      }
   }

   m_cs = m_xmlL1File; // have to check why we still need this
}


TrigConf::XMLStorageMgr::~XMLStorageMgr() {
   delete m_l1topomenu;
   delete m_l1menu;
   delete m_hltmenu;
}


// setting log level of all loaders and itself
void
XMLStorageMgr::setLevel(MSGTC::Level lvl) {
   msg().setLevel(lvl);
}

template<class I, class L>
I & CreateLoader( I * & loader, XMLStorageMgr* mgr, const ptree* menu, const std::string & path) {
   if( loader == nullptr && menu != nullptr ) {

      ptree::path_type p(path);

      const ptree * subtree = menu; // subtree is the current tree in the walk

      auto it = ptree::const_assoc_iterator{ subtree->not_found() }; // iterator for finding the tag

      while( ! p.empty() ) {
         const string & tag = p.reduce();
         it = subtree->find( tag );
         if( it == subtree->not_found() ) {
            break;
         } else {
            subtree = & it->second;
         }
      }
      loader = new L( *mgr, it == subtree->not_found() ? 0 : &*it );
   }
   return * loader;
}


IL1TopoMenuLoader&
XMLStorageMgr::l1topoMenuLoader() {

   return CreateLoader<IL1TopoMenuLoader, XMLL1TopoMenuLoader> ( m_l1topoMenuLoader, this, m_l1topomenu, "TOPO_MENU" );

}


IMasterTableLoader& XMLStorageMgr::masterTableLoader() {

   return CreateLoader<IMasterTableLoader, XMLMasterTableLoader> ( m_masterTableLoader, this, m_l1menu, "LVL1Config" );

}


IMenuLoader& XMLStorageMgr::menuLoader() {

   return CreateLoader<IMenuLoader, XMLMenuLoader> ( m_menuLoader, this, m_l1menu, "LVL1Config.TriggerMenu" );

   //    XMLMenuLoader & xmlml = dynamic_cast<XMLMenuLoader &>(ml);
   //    xmlml.setMonitoringPtree(m_l1menu, "LVL1Config.TriggerMenu");
}

IMuctpiLoader& XMLStorageMgr::muctpiLoader() {

   return CreateLoader<IMuctpiLoader, XMLMuctpiLoader> ( m_muctpiLoader, this, m_l1menu, "LVL1Config.MuctpiInfo" );

}

IPrescaleSetLoader& XMLStorageMgr::prescaleSetLoader() {

   return CreateLoader<IPrescaleSetLoader, XMLPrescaleSetLoader> ( m_prescaleSetLoader, this, m_l1menu, "LVL1Config.PrescaleSet" );

}

IPrioritySetLoader& XMLStorageMgr::prioritySetLoader() {

   return CreateLoader<IPrioritySetLoader, XMLPrioritySetLoader> ( m_prioritySetLoader, this, m_l1menu, "LVL1Config.PrioritySet" );

}

IDeadTimeLoader& XMLStorageMgr::deadTimeLoader() {

   return CreateLoader<IDeadTimeLoader, XMLDeadTimeLoader> ( m_deadTimeLoader, this, m_l1menu, "LVL1Config.Deadtime" );

}


IBunchGroupLoader& XMLStorageMgr::bunchGroupLoader() {
   if( ! m_bunchGroupLoader ) m_bunchGroupLoader = new XMLBunchGroupLoader(*this, 0);
   return *m_bunchGroupLoader;
}


IBunchGroupSetLoader& XMLStorageMgr::bunchGroupSetLoader() {

   return CreateLoader<IBunchGroupSetLoader, XMLBunchGroupSetLoader> ( m_bunchGroupSetLoader, this, m_l1menu, "LVL1Config.BunchGroupSet" );

}


IPrescaledClockLoader& XMLStorageMgr::prescaledClockLoader() {

   return CreateLoader<IPrescaledClockLoader, XMLPrescaledClockLoader> ( m_prescaledClockLoader, this, m_l1menu, "LVL1Config.PrescaledClock" );

}


IRandomLoader& XMLStorageMgr::randomLoader() {

   return CreateLoader<IRandomLoader, XMLRandomLoader> ( m_randomLoader, this, m_l1menu, "LVL1Config.Random" );

}


IThresholdConfigLoader& XMLStorageMgr::thresholdConfigLoader() {

   return CreateLoader<IThresholdConfigLoader, XMLThresholdConfigLoader> ( m_thresholdConfigLoader, this, m_l1menu, "LVL1Config.TriggerThresholdList" );

}


ITriggerThresholdLoader& XMLStorageMgr::triggerThresholdLoader() {

   if( ! m_triggerThresholdLoader ) m_triggerThresholdLoader = new XMLTriggerThresholdLoader(*this);
   return *m_triggerThresholdLoader;

}


ICTPConfigLoader& XMLStorageMgr::ctpConfigLoader() {

   return CreateLoader<ICTPConfigLoader, XMLCTPConfigLoader> ( m_ctpConfigLoader, this, m_l1menu, "LVL1Config" );

}


ICTPConfigLoader& XMLStorageMgr::ctpConfigOnlineLoader() {

   return ctpConfigLoader();

}


ITriggerThresholdValueLoader& XMLStorageMgr::triggerThresholdValueLoader() {

   if( ! m_triggerThresholdValueLoader ) m_triggerThresholdValueLoader = new XMLTriggerThresholdValueLoader(*this);
   return *m_triggerThresholdValueLoader;

}


ICaloJetInputLoader& XMLStorageMgr::caloJetInputLoader() {

   if( ! m_caloJetInputLoader ) m_caloJetInputLoader = 0; // new XMLCaloJetInputLoader( *this );
   return *m_caloJetInputLoader;

}


ICaloSinCosLoader& XMLStorageMgr::caloSinCosLoader() {

   if( ! m_caloSinCosLoader ) m_caloSinCosLoader = new XMLCaloSinCosLoader(*this);
   return *m_caloSinCosLoader;

}


ICaloInfoLoader& XMLStorageMgr::caloInfoLoader() {

   return CreateLoader<ICaloInfoLoader, XMLCaloInfoLoader> ( m_caloInfoLoader, this, m_l1menu, "LVL1Config.CaloInfo" );

}


ILutCamLoader& XMLStorageMgr::lutcamLoader() {
   if(! m_lutCamLoader) {
      std::string::size_type pos1 = m_cs.find_first_of(".xml");
      std::string::size_type nlen = 0;
      if (pos1 != std::string::npos) nlen = m_cs.length() - pos1;
      std::string lutFile = m_cs.replace(pos1, nlen, ".lut");
      std::string camFile = m_cs.replace(pos1, nlen, ".cam");

      msg() << "Source XML file " << m_cs << std::endl;
      msg() << "LUT file set to " << lutFile << std::endl;
      msg() << "CAM file set to " << camFile << std::endl;

      m_lutCamLoader = new XMLLutCamLoader(*this, lutFile, camFile);
   }
   return *m_lutCamLoader;
}


ICTPFilesLoader& XMLStorageMgr::ctpFilesLoader() {

   if(! m_ctpFilesLoader) {
      std::string::size_type pos1 = m_cs.find_first_of(".xml");
      std::string::size_type nlen = 0;
      if (pos1 != std::string::npos) nlen = m_cs.length() - pos1;
      std::string lutFile = m_cs.replace(pos1, nlen, ".lut");
      std::string camFile = m_cs.replace(pos1, nlen, ".cam");

      msg() << "Source XML file " << m_cs << std::endl;
      msg() << "LUT file set to " << lutFile << std::endl;
      msg() << "CAM file set to " << camFile << std::endl;

      m_ctpFilesLoader = new XMLCTPFilesLoader(*this, lutFile, camFile);
   }
   return *m_ctpFilesLoader;

}


IThresholdMonitorLoader& XMLStorageMgr::thresholdMonitorLoader() {

   if(! m_thresholdMonitorLoader) m_thresholdMonitorLoader = new XMLThresholdMonitorLoader( *this );			
   return *m_thresholdMonitorLoader;

}


IHLTFrameLoader& XMLStorageMgr::hltFrameLoader() {

   return CreateLoader<IHLTFrameLoader, XMLHLTFrameLoader> ( m_HLTFrameLoader, this, m_hltmenu, "HLT_MENU" );

}


IJobOptionTableLoader& XMLStorageMgr::jobOptionTableLoader() {

   return CreateLoader<IJobOptionTableLoader, XMLJobOptionTableLoader> ( m_jobOptionTableLoader, this, 0, "SETUP" );

}

