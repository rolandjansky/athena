/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigExMTHelloWorldLvl1/MTHelloWorldLvl1.h"
#include "TrigT1Result/RoIBResult.h"
#include "StoreGate/DataHandle.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "ByteStreamData/RawEvent.h"

// static const AlgFactory<MTHelloWorldLvl1>    Factory;
// const IAlgFactory& MTHelloWorldLvl1Factory = Factory;

/////////////////////////////////////////////////////////////////////////////

MTHelloWorldLvl1::MTHelloWorldLvl1(const std::string& name, ISvcLocator* pSvcLocator) :
   AthAlgorithm(name, pSvcLocator),
   m_pIROBDataProviderSvc(0), 
   m_retrieveLvl1(false),
   m_retrieveROBs(false)
{
   // Declare the properties
   declareProperty("RetrieveLvl1", m_retrieveLvl1);
   declareProperty("RetrieveROBs", m_retrieveROBs);
   declareProperty("RobId",        m_listRobIds);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::initialize() {
   
   // Part 1: Get the messaging service, print where you are
   ATH_MSG_INFO("initialize()");
                
#ifdef ATLAS_GAUDI_V21
   SmartIF<IService> tmp_msgSvc(msgSvc());
   if(tmp_msgSvc.isValid()) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = " << tmp_msgSvc->name());
   }
#else
   Service* tmp_msgSvc = dynamic_cast<Service*> (msgSvc());
   if(tmp_msgSvc != 0) {
      ATH_MSG_INFO(" Algorithm = " << name() << " is connected to Message Service = " << tmp_msgSvc->name());
   }
#endif
   
   // Print out the property values
   ATH_MSG_INFO(" RetrieveLvl1 = " << m_retrieveLvl1);
   ATH_MSG_INFO(" RetrieveROBs = " << m_retrieveROBs);
   if (m_retrieveROBs) {
      ATH_MSG_INFO(" Number of ROBs to access = " << m_listRobIds.value().size());                                      
      ATH_MSG_INFO(" List of ROBs: ");                                   
      std::vector<int>::const_iterator it = m_listRobIds.value().begin();
      while (it != m_listRobIds.value().end())
         {
            m_robIds.push_back(*it);
            ATH_MSG_DEBUG(" 0x" << MSG::hex << *it);
            ++it;
         }
   }  


   // Part 4: Locate the ROBDataProviderSvc
   StatusCode sc = service("ROBDataProviderSvc", m_pIROBDataProviderSvc);
   if (!sc.isSuccess()) {
      ATH_MSG_ERROR("Could not find ROBDataProviderSvc");
      return sc;
   } else {
      IService* svc = dynamic_cast<IService*>(m_pIROBDataProviderSvc);
      if(svc != 0 ) {
         ATH_MSG_INFO(" Algorithm = " << name() << " is connected to ROBDataProviderSvc Service = "
                      << svc->name());
      }
   }

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::execute() {

   // Get the messaging service, print where you are
   MsgStream log(msgSvc(), name());
   ATH_MSG_INFO("execute()");

   ATH_MSG_DEBUG(" StoreGate structure: Begin execute " << name() << "\n"
                 << evtStore()->dump());

   // Get the Lvl1 Id
   const DataHandle<ROIB::RoIBResult> dobj;
   if (m_retrieveLvl1) {
      StatusCode sc = evtStore()->retrieve(dobj,"RoIBResult") ;
      if ( sc.isFailure() ) {
         ATH_MSG_ERROR("Could not find Lvl1Result");
         return sc;
      } else {
         ATH_MSG_INFO(" ---> Lvl1Result ID " << dobj->cTPResult().header().L1ID());
      }
   }

   // Get the ROBs
   if (m_retrieveROBs) {
      // preload ROBs in memory
      m_pIROBDataProviderSvc->addROBData(m_robIds);

      // retrieve ROBs
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFragments;
      m_pIROBDataProviderSvc->getROBData(m_robIds, robFragments) ;
      ATH_MSG_INFO(" ---> number of ROBs retrieved " << robFragments.size());
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator it = robFragments.begin();
      ATH_MSG_INFO(" ---> List of ROBs found: ");                                   
      while (it != robFragments.end())
         {
            ATH_MSG_INFO(" ---> ROB ID = 0x" << MSG::hex << (*it)->rod_source_id() << MSG::dec 
                         << " Level-1 ID = " << (*it)->rod_lvl1_id());
            ++it;
         }
   }

   ATH_MSG_DEBUG(" StoreGate structure: End execute " << name() << "\n"
                 << evtStore()->dump());

   return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MTHelloWorldLvl1::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  ATH_MSG_INFO("finalize()");

  return StatusCode::SUCCESS;
}
