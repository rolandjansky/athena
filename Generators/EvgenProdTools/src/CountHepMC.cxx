/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef XAOD_ANALYSIS

#include "EvgenProdTools/CountHepMC.h"
#include "GaudiKernel/IEventProcessor.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "AthenaKernel/errorcheck.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventType.h"
#include "xAODEventInfo/EventAuxInfo.h"
#include "StoreGate/ReadDecorHandle.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "IOVDbDataModel/IOVPayloadContainer.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <cmath>
#include <cassert>
#include <string>

CountHepMC::CountHepMC(const std::string& name, ISvcLocator* pSvcLocator) :
  GenBase(name, pSvcLocator)
{
  declareProperty("RequestedOutput", m_nCount=5000);
  declareProperty("FirstEvent",      m_firstEv=1);
  declareProperty("CorrectHepMC",    m_corHepMC=false);
  declareProperty("CorrectEventID",  m_corEvtID=false);
  declareProperty("CorrectRunNumber", m_corRunNumber=false);
  declareProperty("NewRunNumber",    m_newRunNumber=999999);
  declareProperty("inputKeyName", m_inputKeyName = "GEN_EVENT");
}

StatusCode CountHepMC::initialize()
{
  ATH_CHECK(GenBase::initialize());

  if(m_corRunNumber) ATH_CHECK(m_metaDataStore.retrieve());

  if(m_corEvtID || m_corRunNumber) {
    ATH_CHECK(m_inputEvtInfoKey.initialize());
    ATH_CHECK(m_outputEvtInfoKey.initialize());
    ATH_CHECK(m_mcWeightsKey.initialize());
  }

  return StatusCode::SUCCESS;
}

StatusCode CountHepMC::execute() {

  /// @todo Replace the old event ?
  m_nPass++;
  ATH_MSG_DEBUG("Current count = " << m_nPass);
  ATH_MSG_INFO("Options for HepMC event number, EvtID event number, EvtID run number = " << m_corHepMC << m_corEvtID << m_corRunNumber );
  // Fix the event number
  int newnum = m_nPass + m_firstEv - 1;

  if (m_corHepMC) {
    std::string   key = m_inputKeyName;
    // retrieve event from Transient Store (Storegate)
    const McEventCollection* oldmcEvtColl=0;
    if (evtStore()->retrieve(oldmcEvtColl, key).isSuccess()){
      McEventCollection* newmcEvtColl = new McEventCollection(*oldmcEvtColl);
      McEventCollection::iterator evt = newmcEvtColl->begin();
      HepMC::GenEvent* hepMC = *evt;
      HepMC::set_ll_event_number(hepMC, newnum);
      CHECK(evtStore()->overwrite( newmcEvtColl, key));
    }
    else{
      ATH_MSG_ERROR("No McEventCollection object found");
      return StatusCode::SUCCESS;
    }
  }

  xAOD::EventInfo* outputEvtInfo{nullptr};
  if(m_corEvtID||m_corRunNumber) {
    SG::ReadHandle<xAOD::EventInfo> inputEvtInfoHandle(m_inputEvtInfoKey);
    SG::WriteHandle<xAOD::EventInfo> outputEvtInfoHandle(m_outputEvtInfoKey);
    ATH_CHECK(outputEvtInfoHandle.record(std::make_unique<xAOD::EventInfo>(), std::make_unique<xAOD::EventAuxInfo>()));

    outputEvtInfo = outputEvtInfoHandle.ptr();
    *outputEvtInfo = *inputEvtInfoHandle;

    SG::ReadDecorHandle<xAOD::EventInfo,std::vector<float>> mcWeights(m_mcWeightsKey);
    outputEvtInfo->setMCEventWeights(mcWeights(0));
  }

  if (m_corEvtID) {
    // Change the EventID in the eventinfo header
    const EventInfo* pInputEvt(nullptr);
    if (evtStore()->retrieve(pInputEvt).isSuccess()) {
      assert(pInputEvt);
      EventID* eventID = const_cast<EventID*>(pInputEvt->event_ID());
      eventID->set_event_number(newnum);
      ATH_MSG_DEBUG("Set new event number in event_ID");
    } else {
      ATH_MSG_ERROR("No EventInfo object found");
      return StatusCode::SUCCESS;
    }

    outputEvtInfo->setEventNumber(newnum);
  }

  if (m_corRunNumber) {
    // Change the EventID in the eventinfo header
    const EventInfo* pInputEvt(nullptr);
    unsigned int oldRunNumber = 0;
    if (evtStore()->retrieve(pInputEvt).isSuccess()) {
      assert(pInputEvt);
      EventID* eventID = const_cast<EventID*>(pInputEvt->event_ID());
      oldRunNumber = eventID->run_number();
      eventID->set_run_number(m_newRunNumber);
      ATH_MSG_DEBUG("Set new run number in event_ID " << m_newRunNumber);

      // also set the MC channel number
      EventType* event_type = const_cast<EventType*>(pInputEvt->event_type());
      event_type->set_mc_channel_number(m_newRunNumber);
      ATH_MSG_DEBUG("Set new MC channel number " << event_type->mc_channel_number());
    } else {
      ATH_MSG_ERROR("No EventInfo object found");
      return StatusCode::SUCCESS;
    }

    oldRunNumber = outputEvtInfo->runNumber();
    outputEvtInfo->setRunNumber(m_newRunNumber);
    outputEvtInfo->setMCChannelNumber(m_newRunNumber);

    {
      // change the channel number where /Generation/Parameters are found
      auto newChannelNumber =
	static_cast< CondAttrListCollection::ChanNum >(m_newRunNumber);
      auto oldChannelNumber =
	static_cast< CondAttrListCollection::ChanNum >(oldRunNumber);

      const char* key = "/Generation/Parameters";
      const IOVMetaDataContainer * iovContainer = nullptr;
      if (m_metaDataStore->retrieve(iovContainer, key).isSuccess()
          && iovContainer) {
        // get a hold of the payload
        const IOVPayloadContainer * payloadContainer =
            iovContainer->payloadContainer();

        // Grab the attribute list
        for (CondAttrListCollection* collection : *payloadContainer) {
          for(unsigned int index = 0; index < collection->size(); ++index) {
            if (collection->chanNum(index) != oldChannelNumber) {
              ATH_MSG_INFO("Not updating \"" << key << "\" on channel number "
                           << collection->chanNum(index));
              continue;
            }

            if (collection->fixChanNum(oldChannelNumber, newChannelNumber))
              ATH_MSG_INFO("Updated \"" << key << "\" channel number from "
                           << oldChannelNumber << " to " << newChannelNumber);
            else
              ATH_MSG_ERROR("Channel number update from " << oldChannelNumber
                            << " to " << newChannelNumber << " on \"" << key
                            << "\" FAILED");
          }
        }

        {
          // Update the MC channel number in the "/TagInfo"
          const char* key = "/TagInfo";
          const IOVMetaDataContainer * iovContainer = nullptr;
          if (m_metaDataStore->retrieve(iovContainer, key).isSuccess()
              && iovContainer) {
            // get a hold of the payload
            const IOVPayloadContainer * payloadContainer =
              iovContainer->payloadContainer();

            // Grab the attribute list
            for (CondAttrListCollection* collection : *payloadContainer) {
              for (auto pair : *collection) {
                // pair is a pair of Channel number and AttributeList
                if (pair.second.exists("mc_channel_number")) {
                  try {
                    pair.second["mc_channel_number"].setValue(
                      std::to_string(m_newRunNumber));
                    ATH_MSG_INFO("Updated \"" << key << "\" mc_channel_number"
                                 << " to " << m_newRunNumber);
                  } catch (std::exception&) {
                    try {
                      pair.second["mc_channel_number"].setValue(m_newRunNumber);
                      ATH_MSG_INFO("Updated \"" << key << "\" mc_channel_number"
                                   << " to " << m_newRunNumber);
                    } catch (std::exception&) {
                      ATH_MSG_ERROR("mc_channel_number update from to "
                                    << m_newRunNumber << " on \"" << key
                                    << "\" FAILED");
                    }
                  }
                }
              }
            }
          }
        }
      } else {
        ATH_MSG_INFO("Could not retrieve \"" << key << "\" from MetaDataStore");
      }
    }
  }


  if (m_nPass == m_nCount) {
    ATH_MSG_INFO("Stopping the event processing...." << m_nPass << "/" << m_nCount);
    IEventProcessor* apm = 0;
    IService* ssvc = 0;
    static const bool CRE(false);
    StatusCode sc = serviceLocator()->getService("AthenaEventLoopMgr", ssvc, CRE);
    if (sc.isSuccess()) {
      sc = ssvc->queryInterface(IEventProcessor::interfaceID(), pp_cast<void>(&apm));
    } else {
      ATH_MSG_WARNING("No EventLoop Manager found ");
    }
    if (sc.isSuccess()) {
      if ((apm->stopRun()).isFailure()) {
        ATH_MSG_ERROR("StatusCode Failure in stopRun()");
        return StatusCode::FAILURE;
      }
      apm->release();
    } else {
      ATH_MSG_WARNING("No EventLoop Manager specified ");
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode CountHepMC::finalize() {
  ATH_MSG_INFO("Events passing all checks and written = " << m_nPass);
  return StatusCode::SUCCESS;
}

#endif
