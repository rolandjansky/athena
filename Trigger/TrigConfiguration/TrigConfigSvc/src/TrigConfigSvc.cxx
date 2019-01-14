/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GaudiKernel/ServiceHandle.h"

#include "AthAnalysisBaseComps/AthAnalysisHelper.h" // for AAH::setProperty. Header only, not linked

#include "TrigConfigSvc/TrigConfigSvc.h"
#include "TrigConfigSvc/HLTConfigSvc.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfL1Data/BunchGroupSet.h"

#include "boost/foreach.hpp"
#include "boost/algorithm/string.hpp"

#include <algorithm>

using namespace std;
using namespace TrigConf;

TrigConfigSvc::TrigConfigSvc( const std::string& name,
                              ISvcLocator* pSvcLocator ) :
   AthService( name, pSvcLocator ),
   m_l1topoSvc("TrigConf::L1TopoConfigSvc/L1TopoConfigSvc", name),
   m_lvl1Svc("TrigConf::LVL1ConfigSvc/LVL1ConfigSvc", name),
   m_hltSvc("TrigConf::HLTConfigSvc/HLTConfigSvc", name),
   m_dsSvc("TrigConf::DSConfigSvc/DSConfigSvc", name),
   m_l1service(0),
   m_hltservice(0)
{
  declareProperty( "LVL1ConfigSvc", m_lvl1Svc, "LVL1 config svc -> retrieve trigger configuration from (XML/DB)");
  declareProperty( "HLTConfigSvc",  m_hltSvc,  "HLT config svc -> retrieve trigger configuration from (XML/DB)");
  declareProperty( "DSConfigSvc",   m_dsSvc,   "TrigConfig svc -> retrieve trigger configuration from DetectorStore");
  declareProperty( "PriorityList",  m_priorityList, "priority list of TrigConfigSvc in which to query. Allowed values are ['xml','ds']");
  m_priorityList.push_back("ds");
  m_priorityList.push_back("xml");
}


StatusCode
TrigConfigSvc::initialize() {

   CHECK(AthService::initialize());

   ATH_MSG_INFO("TrigConfigSvc::initialize(): Testing for available services in this order : ");

   bool l1fromxml(false), hltfromxml(false);

   for(const string& svc: m_priorityList) {
      string testsvc(boost::to_lower_copy(svc));

      ATH_MSG_INFO("    => " << testsvc);

      //////////////////////////////////////////////////////////////
      // BEGIN RUN-3 TESTING BLOCK - THIS SHOULD BE TEMPORARY
      ////////////////////////////////////////////////////////////// 
      if ( testsvc == "run3_dummy" ) {
         ATH_CHECK( AAH::setProperty( m_hltSvc, "ConfigSource", testsvc ) );
         if (m_hltSvc.retrieve().isSuccess()) {
            m_hltservice = m_hltSvc.operator->();
            ATH_MSG_WARNING("Got HLT Svc " << m_hltSvc.typeAndName() << ", will use R3 dummy menu");
            hltfromxml = true;
         } else {
            ATH_MSG_FATAL("failed to retrieve HLT ConfigSvc: " << m_hltSvc << " using R3 dummy menu");
            return StatusCode::FAILURE;
         }
      }
      //////////////////////////////////////////////////////////////
      // END RUN-3 TESTING BLOCK - THIS SHOULD BE TEMPORARY
      ////////////////////////////////////////////////////////////// 

      if ( testsvc == "ds" ) {
         if (m_dsSvc.retrieve().isSuccess()) {
            if(m_l1service==0) m_l1service = m_dsSvc.operator->();
            if(m_hltservice==0) m_hltservice = m_dsSvc.operator->(); 
         } else {
            ATH_MSG_FATAL("failed to retrieve DetStore TrigConfigSvc: " << m_dsSvc);
            return StatusCode::FAILURE;
         }
      }

      if ( m_l1toposervice == 0 && (testsvc == "xmll1" || testsvc == "xml") ) {
         if (m_l1topoSvc.retrieve().isSuccess()) {
            m_l1toposervice = m_l1topoSvc.operator->();
         } else {
            ATH_MSG_FATAL("failed to retrieve L1TopoConfigSvc: " << m_l1topoSvc);
            return StatusCode::FAILURE;
         }
      }

      if ( m_l1service == 0 && (testsvc == "xmll1" || testsvc == "xml") ) {
         if (m_lvl1Svc.retrieve().isSuccess()) {
            m_l1service = m_lvl1Svc.operator->();
            l1fromxml = true;
         } else {
            ATH_MSG_FATAL("failed to retrieve LVL1 ConfigSvc: " << m_lvl1Svc);
            return StatusCode::FAILURE;
         }
      }

    
      if ( m_hltservice == 0 && (testsvc == "xmlhlt" || testsvc == "xml") ) {
         if (m_hltSvc.retrieve().isSuccess()) {
            m_hltservice = m_hltSvc.operator->();
            hltfromxml = true;
         } else {
            ATH_MSG_FATAL("failed to retrieve HLT ConfigSvc: " << m_hltSvc);
            return StatusCode::FAILURE;
         }
      }

      if( m_l1toposervice!=0 && m_l1service!=0 && m_hltservice!=0 )   break;
   }

   if( m_l1toposervice==0 && m_l1service==0 && m_hltservice==0 ) {
      ATH_MSG_FATAL("No trigger configuration service found");
      return StatusCode::FAILURE;
   }

   ATH_MSG_INFO("Using for L1 topo configuration  : " << (m_l1toposervice!=0?m_l1toposervice->name():"none") );
   ATH_MSG_INFO("Using for L1 configuration       : " << (m_l1service!=0?m_l1service->name():"none") );
   ATH_MSG_INFO("Using for HLT configuration      : " << (m_hltservice!=0?m_hltservice->name():"none") );

   // when configuring from xml we have to set the CTP items as lower chains for the HLT

   if(l1fromxml && hltfromxml && m_lvl1Svc->ctpConfig()!=0 ) {
      ATH_MSG_DEBUG("Setting lower chain counters since reading menu from XML");
      dynamic_cast<HLTConfigSvc&>(*m_hltSvc).setL2LowerChainCounter(m_lvl1Svc->ctpConfig()); // not sure this makes sense
   }

   ATH_MSG_INFO("Done with initialization of the trigger configuration provider");
   return StatusCode::SUCCESS;
}



StatusCode
TrigConfigSvc::queryInterface( const InterfaceID& riid,
                               void** ppvIF )
{

   if ( riid == ITrigConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< ITrigConfigSvc* >(this);
   } else if ( riid == IProperty::interfaceID() ) {
      *ppvIF = static_cast< IProperty* >(this);
   } else if ( riid == IL1TopoConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< IL1TopoConfigSvc* >(this);
   } else if ( riid == ILVL1ConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< ILVL1ConfigSvc* >(this);
   } else if ( riid == IHLTConfigSvc::interfaceID() )  {
      *ppvIF = static_cast< IHLTConfigSvc* >(this);
   } else   {
      return AthService::queryInterface( riid, ppvIF );
   }

   addRef();
   return StatusCode::SUCCESS;
}


const TXC::L1TopoMenu*
TrigConfigSvc::menu() const {
   if(m_l1toposervice)
      return m_l1toposervice->menu();
      
   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}

const Muctpi*
TrigConfigSvc::muctpiConfig() const {
   if(m_l1service)
      return m_l1service->muctpiConfig();
      
   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}


const CTPConfig*
TrigConfigSvc::ctpConfig() const {
   if(m_l1service)
      return m_l1service->ctpConfig();
      
   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}


const ThresholdConfig*
TrigConfigSvc::thresholdConfig() const {
   if(m_l1service)
      return m_l1service->thresholdConfig();
   
   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}


const BunchGroupSet*
TrigConfigSvc::bunchGroupSet() const {
   if(m_l1service)
      return m_l1service->bunchGroupSet();

   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}


uint32_t
TrigConfigSvc::lvl1PrescaleKey() const {
   if(m_l1service)
      return m_l1service->lvl1PrescaleKey();
   
   REPORT_MESSAGE(MSG::WARNING) << "No LVL1 trigger configuration available" << endmsg;
   return 0;
}


const HLTChainList&
TrigConfigSvc::chains() const {
   if(m_hltservice)
      return m_hltservice->chains();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return m_NullFrame.chains();
}


const HLTChainList*
TrigConfigSvc::chainList() const {
   if(m_hltservice)
      return m_hltservice->chainList();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return 0;
}


const HLTSequenceList&
TrigConfigSvc::sequences() const {
   if(m_hltservice)
      return m_hltservice->sequences();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return m_NullFrame.sequences();
}


const HLTSequenceList*
TrigConfigSvc::sequenceList() const {
   if(m_hltservice)
      return m_hltservice->sequenceList();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return 0;
}


uint32_t
TrigConfigSvc::masterKey() const {
   if(m_hltservice)
      return m_hltservice->masterKey();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return 0;
}


uint32_t
TrigConfigSvc::hltPrescaleKey() const {
   if(m_hltservice)
      return m_hltservice->hltPrescaleKey();

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return 0;
}


StatusCode
TrigConfigSvc::assignPrescalesToChains(uint lumiblock ) {
   if(m_hltservice)
      return m_hltservice->assignPrescalesToChains(lumiblock);

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return StatusCode::RECOVERABLE;
}


StatusCode
TrigConfigSvc::updatePrescaleSets(uint requestcount) {
   if(m_hltservice)
      return m_hltservice->updatePrescaleSets(requestcount);

   REPORT_MESSAGE(MSG::WARNING) << "No HLT trigger configuration available" << endmsg;
   return StatusCode::RECOVERABLE;
}

