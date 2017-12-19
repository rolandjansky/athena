/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Include files
#include "ByteStreamCnvSvcBase/ByteStreamAddressProviderSvc.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddress.h"
#include "ByteStreamCnvSvcBase/ByteStreamAddressL1R.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "StoreGate/StoreGate.h"
#include "SGTools/TransientAddress.h"
#include "GaudiKernel/ListItem.h"

#include "AthenaKernel/IClassIDSvc.h"

#include "eformat/SourceIdentifier.h"

/// Standard constructor
ByteStreamAddressProviderSvc::ByteStreamAddressProviderSvc(const std::string& name, ISvcLocator* pSvcLocator) :
	 ::AthService(name, pSvcLocator),
	m_rdpSvc("ROBDataProviderSvc", name),
	m_clidSvc("ClassIDSvc", name),
	m_storeID(StoreID::EVENT_STORE) {
  // set default Jet/Energy Processor RoI module IDs
  std::vector<unsigned int> default_jetProcModuleID;
  default_jetProcModuleID.push_back(0xac);
  default_jetProcModuleID.push_back(0xad);
  m_jetProcModuleID.setValue(default_jetProcModuleID);

  // set default Calorimeter Cluster Processor RoI module IDs
  std::vector<unsigned int> default_caloClusterProcModuleID;
  default_caloClusterProcModuleID.push_back(0xa8);
  default_caloClusterProcModuleID.push_back(0xa9);
  default_caloClusterProcModuleID.push_back(0xaa);
  default_caloClusterProcModuleID.push_back(0xab);
  m_caloClusterProcModuleID.setValue(default_caloClusterProcModuleID);

  // set default Topo Processor RoI module IDs
  std::vector<unsigned int> default_topoProcModuleID;
  // expect 0x82, 0x92 for new RoIB
  default_topoProcModuleID.push_back(0x81);
  default_topoProcModuleID.push_back(0x91);
  m_topoProcModuleID.setValue(default_topoProcModuleID);

  declareProperty("TypeNames", m_typeNames);
  // Properties to set L1 ROB module IDs
  declareProperty("CTPModuleID", m_ctpModuleID = 1);
  declareProperty("MuCTPIModuleID", m_muCTPIModuleID = 1);
  declareProperty("JetProcModuleID", m_jetProcModuleID);
  declareProperty("CaloClusterProcModuleID", m_caloClusterProcModuleID);
  declareProperty("TopoProcModuleID", m_topoProcModuleID);
  declareProperty("StoreID", m_storeID);
}
//________________________________________________________________________________
ByteStreamAddressProviderSvc::~ByteStreamAddressProviderSvc() {
}
//________________________________________________________________________________
StatusCode ByteStreamAddressProviderSvc::initialize() {
   ATH_MSG_INFO("Initializing " << name() << " - package version " << PACKAGE_VERSION);
   if (!::AthService::initialize().isSuccess()) {
      ATH_MSG_FATAL("Cannot initialize AthService base class.");
      return(StatusCode::FAILURE);
   }

   // Retrieve ClassIDSvc
   if (!m_clidSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ClassIDSvc.");
      return(StatusCode::FAILURE);
   }
   // Retrieve ROBDataProviderSvc
   if (!m_rdpSvc.retrieve().isSuccess()) {
      ATH_MSG_FATAL("Cannot get ROBDataProviderSvc.");
      return(StatusCode::FAILURE);
   }
   if (m_storeID < 0 || m_storeID > StoreID::UNKNOWN) {
      ATH_MSG_FATAL("Invalid StoreID " << m_storeID);
      return(StatusCode::FAILURE);
   }
   ATH_MSG_INFO("initialized ");
   ATH_MSG_INFO("-- Module IDs for: ");
   ATH_MSG_INFO("   CTP                                  = 0x" << MSG::hex << m_ctpModuleID.value() << MSG::dec);
   ATH_MSG_INFO("   muCTPi                               = 0x" << MSG::hex << m_muCTPIModuleID.value() << MSG::dec);
   ATH_MSG_INFO("   Calorimeter Cluster Processor RoI    = 0x" << MSG::hex << m_caloClusterProcModuleID.value()[0]
	   << ", 0x" << m_caloClusterProcModuleID.value()[1] << ", 0x" << m_caloClusterProcModuleID.value()[2]
	   << ", 0x" << m_caloClusterProcModuleID.value()[3] << MSG::dec);
   ATH_MSG_INFO("   Calorimeter Jet/Energy Processor RoI = 0x" << MSG::hex << m_jetProcModuleID.value()[0]
	   << ", 0x" << m_jetProcModuleID.value()[1] << MSG::dec);
   ATH_MSG_INFO("   Topo Processor RoI = 0x" << MSG::hex << m_topoProcModuleID.value()[0]
	   << ", 0x" << m_topoProcModuleID.value()[1] << MSG::dec);
   ATH_MSG_INFO("-- Will fill Store with id =  " << m_storeID);
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode ByteStreamAddressProviderSvc::finalize() {
   // Release ROBDataProviderSvc
   if (!m_rdpSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ROBDataProviderSvc.");
   }
   // Release ClassIDSvc
   if (!m_clidSvc.release().isSuccess()) {
      ATH_MSG_WARNING("Cannot release ClassIDSvc.");
   }
   return(::AthService::finalize());
}
//________________________________________________________________________________
StatusCode ByteStreamAddressProviderSvc::preLoadAddresses(StoreID::type id, tadList& tlist) {
   ATH_MSG_DEBUG("in preLoadAddress");
   if (id != m_storeID) {
      ATH_MSG_DEBUG("StoreID = " << id << " does not match required id (" << m_storeID << ") skip");
      return(StatusCode::SUCCESS);
   }

   for (std::vector<std::string>::const_iterator it = m_typeNames.begin(), it_e = m_typeNames.end();
	   it != it_e; it++) {
      ListItem item(*it);
      const std::string& t = item.type();
      const std::string& nm = item.name();
      CLID classid;
      if (!m_clidSvc->getIDOfTypeName(t, classid).isSuccess()) {
         ATH_MSG_WARNING("Cannot create TAD for (type, name)" << " no CLID for " << t << " " << nm);
      } else {
         SG::TransientAddress* tad = new SG::TransientAddress(classid, nm);
         tlist.push_back(tad);
         ATH_MSG_DEBUG("Created TAD for (type, clid, name)" << t << " " << classid << " " << nm);
         // save the clid and key.
         m_clidKey[classid].insert(nm);
      }
   }
   return(StatusCode::SUCCESS);
}
//________________________________________________________________________________
StatusCode ByteStreamAddressProviderSvc::updateAddress(StoreID::type id,
                                                       SG::TransientAddress* tad,
                                                       const EventContext& ctx) {
   if (id != m_storeID) {
      return(StatusCode::FAILURE);
   }
   CLID clid = tad->clID();
   std::string nm = tad->name();
   std::map<CLID, std::set<std::string> >::const_iterator it = m_clidKey.find(clid);
   if (it == m_clidKey.end() || (*it).second.count(nm) == 0) {
      return(StatusCode::FAILURE);
   }
   ATH_MSG_DEBUG("Creating address for " << clid << " " << nm);
   if (clid == 6000 || clid == 6001 || clid == 6421) {
      // L1 or L2 result, special address class, ByteStreamAddressL1R,
      // which contains the ROBFragment.
      std::vector<uint32_t> vid;
      if (clid == 6000 || clid == 6001) {
         // L1 result
         eformat::helper::SourceIdentifier ctpID(eformat::TDAQ_CTP, m_ctpModuleID.value());
         vid.push_back(ctpID.code());
         eformat::helper::SourceIdentifier muID(eformat::TDAQ_MUON_CTP_INTERFACE, m_muCTPIModuleID.value());
         vid.push_back(muID.code());
         for (int i = 0; i < 2; i++) {
            eformat::helper::SourceIdentifier jetID(eformat::TDAQ_CALO_JET_PROC_ROI, m_jetProcModuleID.value()[i]);
            vid.push_back(jetID.code());
         }
         for (int i = 0; i < 4; i++) {
            eformat::helper::SourceIdentifier emID(eformat::TDAQ_CALO_CLUSTER_PROC_ROI, m_caloClusterProcModuleID.value()[i]);
            vid.push_back(emID.code());
         }
         for (int i = 0; i < 2; i++) {
            eformat::helper::SourceIdentifier topoID(eformat::TDAQ_CALO_TOPO_PROC, m_topoProcModuleID.value()[i]);
            vid.push_back(topoID.code());
         }
      } else {
         // L2 result
         eformat::helper::SourceIdentifier l2ID(eformat::TDAQ_LVL2, 0);
         vid.push_back(l2ID.code());
      }
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vrob;
      m_rdpSvc->getROBData(vid, vrob);
      ATH_MSG_DEBUG("Getting ROBData for " << nm << ", number robs = " << vrob.size());
      ByteStreamAddressL1R* addr = new ByteStreamAddressL1R(clid, nm, "");
      for (unsigned int i = 0; i < vrob.size(); i++) {
         addr->addPointer(*vrob[i]);
      }
      tad->setAddress(addr);
   } else {
      ByteStreamAddress* add = new ByteStreamAddress(clid, nm, "");
      add->setEventContext(ctx);
      tad->setAddress(add);
   }
   return(StatusCode::SUCCESS);
}
