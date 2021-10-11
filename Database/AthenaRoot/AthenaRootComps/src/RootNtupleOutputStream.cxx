/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cassert>
#include <string>
#include <vector>
#include <fnmatch.h>

// Framework include files
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IAlgManager.h"
#include "GaudiKernel/IClassIDSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ClassID.h"

#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IAthenaOutputStreamTool.h"

#include "StoreGate/StoreGateSvc.h"
#include "SGTools/DataProxy.h"
#include "SGTools/ProxyMap.h"
#include "SGTools/SGIFolder.h"

#include "RootNtupleOutputStream.h"

namespace Athena {

// Standard Constructor
RootNtupleOutputStream::RootNtupleOutputStream(const std::string& name, ISvcLocator* pSvcLocator)
  : FilteredAlgorithm(name, pSvcLocator),
    m_dataStore("StoreGateSvc", name),
    m_pCLIDSvc("ClassIDSvc", name),
    m_events(0),
    m_streamer(std::string("AthenaOutputStreamTool/") + 
               name + std::string("Tool"), this),
    m_helperTools(this) 
{
  assert(pSvcLocator);
  declareProperty("ItemList",               m_itemList);
  declareProperty("OutputFile",             m_outputName="DidNotNameOutput.root");
  declareProperty("EvtConversionSvc",       m_persName="EventPersistencySvc");
  declareProperty("WritingTool",            m_streamer);
  declareProperty("Store",                  m_dataStore);
  declareProperty("ProcessingTag",          m_processTag=name);
  declareProperty("ForceRead",              m_forceRead=false);
  declareProperty("PersToPers",             m_persToPers=false);
  declareProperty("ExemptPersToPers",       m_exemptPersToPers);
  declareProperty("ProvideDef",             m_provideDef=false);
  declareProperty("WriteOnExecute",         m_writeOnExecute=true);
  declareProperty("WriteOnFinalize",        m_writeOnFinalize=false);
  declareProperty("TakeItemsFromInput",     m_itemListFromTool=false);
  declareProperty("HelperTools",            m_helperTools);

  declareProperty("DynamicItemList",
                  m_dynamicItemList = false,
                  "dynamic output itemlist:\n" \
                  "  if enabled rediscover object list to be written out at each event\n" \
                  "  otherwise: reuse the one from the first event.");
}

// Standard Destructor
RootNtupleOutputStream::~RootNtupleOutputStream() 
{}

// initialize data writer
StatusCode 
RootNtupleOutputStream::initialize() 
{
  ATH_MSG_DEBUG("In initialize");
  if (!this->FilteredAlgorithm::initialize().isSuccess()) {
    ATH_MSG_ERROR("could not initialize base class");
    return StatusCode::FAILURE;
  }

  // Reset the number of events written
  m_events = 0;

  // set up the SG service:
  if (!m_dataStore.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Could not locate default store");
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("Found " << m_dataStore.type() << " store.");
  }
  assert(static_cast<bool>(m_dataStore));

  // set up the CLID service:
  if (!m_pCLIDSvc.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Could not locate default ClassIDSvc");
    return StatusCode::FAILURE;
  }
  assert(static_cast<bool>(m_pCLIDSvc));

  // Get Output Stream tool for writing
  if (!m_streamer.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Can not find " << m_streamer);
    return StatusCode::FAILURE;
  }
 
  const bool extendProvenanceRecord = true;
  if (!m_streamer->connectServices(m_dataStore.type(), 
                                   m_persName, 
                                   extendProvenanceRecord).isSuccess()) {
    ATH_MSG_FATAL("Unable to connect services");
    return StatusCode::FAILURE;
  }

  if (!m_helperTools.retrieve().isSuccess()) {
    ATH_MSG_FATAL("Can not find " << m_helperTools);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Found " << m_helperTools << endmsg << "Data output: " << m_outputName);

  bool allgood = true;
  for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator 
         iter = m_helperTools.begin(),
         iend = m_helperTools.end();
       iter != iend; 
       ++iter) {
    if (!(*iter)->postInitialize().isSuccess()) {
      allgood = false;
    }
  }
  if (!allgood) {
    ATH_MSG_ERROR("problem in postInitialize of a helper tool");
    return StatusCode::FAILURE;
  }

  // For 'write on finalize', we set up listener for 'MetaDataStop'
  // and perform write at this point. This happens at 'stop' of the
  // event selector. RDS 04/2010
  if (m_writeOnFinalize) {
    // Set to be listener for end of event
    ServiceHandle<IIncidentSvc> incSvc("IncidentSvc", this->name());
    if (!incSvc.retrieve().isSuccess()) {
      ATH_MSG_ERROR("Cannot get the IncidentSvc");
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("Retrieved IncidentSvc");
    }
    incSvc->addListener(this, "MetaDataStop", 50);
    ATH_MSG_DEBUG("Added MetaDataStop listener");
  }
  ATH_MSG_DEBUG("End initialize");
  return StatusCode::SUCCESS;
}

void 
RootNtupleOutputStream::handle(const Incident& inc) 
{
  ATH_MSG_DEBUG("handle() incident type: " << inc.type());
  static const std::string s_METADATASTOP = "MetaDataStop";
  if (inc.type() == s_METADATASTOP) {
    // Moved preFinalize of helper tools to stop - want to optimize the
    // output file in finalize RDS 12/2009
    for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator 
           iter = m_helperTools.begin(),
           iend = m_helperTools.end();
         iter != iend; 
         ++iter) {
      if (!(*iter)->preFinalize().isSuccess()) {
        ATH_MSG_ERROR("Cannot finalize helper tool");
      }
    }
    // Always force a final commit in stop - mainly applies to AthenaPool 
    if (m_writeOnFinalize) {
      if (!write().isSuccess()) {  // true mean write AND commit
        ATH_MSG_ERROR("Cannot write on finalize");
      }
    }
    // If there are no events, then connect the output for the metadata
    if (m_events == 0) {
      if (m_streamer->connectOutput(m_outputName).isFailure()) {
        ATH_MSG_ERROR("Unable to connect to file " << m_outputName);
      }
    }
  }
  ATH_MSG_INFO("Records written: " << m_events);
  ATH_MSG_DEBUG("Leaving handle");
}

// terminate data writer
StatusCode 
RootNtupleOutputStream::finalize() 
{
  bool failed = false;
  ATH_MSG_DEBUG("finalize: Optimize output");
  if (m_events == 0) { 
    if (!m_streamer->commitOutput().isSuccess()) {
      failed = true;
    }
  }
  if (!m_streamer->finalizeOutput().isSuccess()) {
    failed = true;
  }
  ATH_MSG_DEBUG("finalize: end optimize output");
  if (!m_helperTools.release().isSuccess()) {
    failed = true;
  }
  if (!m_streamer.release().isSuccess()) {
    failed = true;
  }
  return failed 
    ? StatusCode::FAILURE 
    : StatusCode::SUCCESS;
}

StatusCode
RootNtupleOutputStream::execute() 
{
  bool failed = false;
  for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator 
         iter = m_helperTools.begin(),
         iend = m_helperTools.end();
       iter != iend; 
       ++iter) {
    if (!(*iter)->preExecute().isSuccess()) {
      failed = true;
    }
  }
  if (m_writeOnExecute) {
    if (!write().isSuccess()) {
      failed = true;
    }
  }
  for (std::vector<ToolHandle<IAthenaOutputTool> >::iterator 
         iter = m_helperTools.begin(),
         iend = m_helperTools.end();
       iter != iend; 
       ++iter) {
    if(!(*iter)->postExecute().isSuccess()) {
      failed = true;
    }
  }

  return failed 
    ? StatusCode::FAILURE 
    : StatusCode::SUCCESS;
}

// Work entry point
StatusCode
RootNtupleOutputStream::write() 
{
  bool failed = false;
  // Clear any previously existing item list
  clearSelection();
  // Test whether this event should be output
  if (isEventAccepted()) {
    // Connect the output file to the service
    if (m_streamer->connectOutput(m_outputName).isSuccess()) {
      // First check if there are any new items in the list
      collectAllObjects();
      StatusCode sc = m_streamer->streamObjects(m_objects);
      // Do final check of streaming
      if (!sc.isSuccess()) {
        if (!sc.isRecoverable()) {
          ATH_MSG_FATAL("streamObjects failed.");
          failed = true;
        } else {
          ATH_MSG_DEBUG("streamObjects failed.");
        }
      }
      sc = m_streamer->commitOutput();
      if (!sc.isSuccess()) {
        ATH_MSG_FATAL("commitOutput failed.");
        failed = true;
      }
      clearSelection();
      if (!failed) {
        m_events++;
      }
    }
  }

  return failed
    ? StatusCode::FAILURE
    : StatusCode::SUCCESS;
}

// Clear collected object list
inline
void 
RootNtupleOutputStream::clearSelection()
{
  m_objects.resize(0);
  if (m_dynamicItemList) {
    m_selection.resize(0);
  }
}

void
RootNtupleOutputStream::collectAllObjects() 
{
  typedef std::vector<SG::FolderItem> Items_t;

  if (!m_dynamicItemList && !m_selection.empty()) {
    // reuse object list from previous event.
  } else {

    // if (m_itemListFromTool) {
    //   //FIXME:
    //   //   if (!m_streamer->getInputItemList(&*m_p2BWritten).isSuccess()) {
    //   //     ATH_MSG_WARNING("collectAllObjects() could not get ItemList from Tool.");
    //   //   }
    // }

    static const std::string s_plus = "+";
    static const std::string s_dash = "-";
    typedef std::vector<const SG::DataProxy*> Proxies_t;
    Proxies_t proxies = m_dataStore->proxies();

    const std::vector<std::string>& items = m_itemList.value();
    std::vector<std::string> toremove; 
    toremove.reserve(items.size());

    Items_t selection; 
    selection.reserve(items.size());

    for (Proxies_t::const_iterator
           iproxy = proxies.begin(),
           iend = proxies.end();
         iproxy != iend;
         ++iproxy) {
      const SG::DataProxy *proxy = *iproxy;
      if (!proxy) {
        continue;
      }
      for (std::vector<std::string>::const_iterator
             jkey = items.begin(),
             jend = items.end();
           jkey != jend;
           ++jkey) {
        if (!jkey->empty()) {
          if ((*jkey)[0] == s_dash[0]) {
            toremove.push_back(jkey->substr(1, std::string::npos));
            continue;
          }
          std::string key = *jkey;
          if ((*jkey)[0] == s_plus[0]) {
            key = jkey->substr(1, std::string::npos);
          }
          int o = fnmatch(key.c_str(), 
                          proxy->name().c_str(),
                          FNM_PATHNAME);
          if (o == 0) {
            // accept
            selection.push_back(SG::FolderItem(proxy->clID(), proxy->name()));
            break;
          }
        }
      }
    }

    m_selection.reserve(selection.size());
    if (toremove.empty()) {
      m_selection = selection;
    } else {
      for(Items_t::const_iterator 
            isel=selection.begin(),
            iend=selection.end();
          isel != iend;
          ++isel) {
        const std::string &name = isel->key();
        bool keep = true;
        for (std::vector<std::string>::const_iterator
               jkey = toremove.begin(),
               jend = toremove.end();
             jkey != jend;
             ++jkey) {
          const std::string& key = *jkey;
          int o = fnmatch(key.c_str(), 
                          name.c_str(),
                          FNM_PATHNAME);
          if (o == 0) {
            // reject
            keep = false;
            break;
          }
        }
        if (keep) {
          m_selection.push_back(*isel);
        }
      }
    }
  }

  for (Items_t::const_iterator
         itr = m_selection.begin(),
         iend = m_selection.end();
       itr != iend;
       ++itr) {
    const std::string &name = itr->key();
    SG::DataProxy *proxy = m_dataStore->proxy(itr->id(), name);
    if (NULL == proxy) {
      continue;
    }
    if (m_forceRead && proxy->isValid()) {
      if (!m_persToPers) {
        if (NULL == proxy->accessData()) {
          ATH_MSG_ERROR(" Could not get data object for id "
                        << proxy->clID() << ",\"" << proxy->name());
        }
      } else if (true /*m_exemptPersToPers.find(item.id()) != m_exemptPersToPers.end()*/) {
        if (NULL == proxy->accessData()) {
          ATH_MSG_ERROR(" Could not get data object for id "
                        << proxy->clID() << ",\"" << proxy->name());
        }
      }
    }
    DataObject *obj = proxy->object();
    if (NULL != obj) {
      m_objects.push_back(obj);
      ATH_MSG_DEBUG(" Added object " << proxy->clID() << ",\"" << proxy->name() << "\"");
      // if (m_checkNumberOfWrites && !m_provideDef) {
      //   std::string tn;
      //   if (!m_pCLIDSvc->getTypeNameOfID(item.id(), tn).isSuccess()) {
      //     ATH_MSG_ERROR(" Could not get type name for id "
      // 		    << item.id() << ",\"" << itemProxy->name());
      //   } else {
      //     tn += '_' + itemProxy->name();
      //     CounterMapType::iterator cit = m_objectWriteCounter.find(tn);
      //     if (cit == m_objectWriteCounter.end()) {
      // 	// First time through
      // 	//std::pair<CounterMapType::iterator, bool> result =
      // 	m_objectWriteCounter.insert(CounterMapType::value_type(tn, 1));
      //     } else {
      // 	// set to next iteration (to avoid double counting)
      // 	// StreamTools will eliminate duplicates.
      // 	(*cit).second = m_events + 1;
      //     }
      //   }
      // }
      // } else if (!m_forceRead && m_persToPers && proxy->isValid()) {
      //   tAddr = itemProxy->transientAddress();
    } //if data object there
  }

  return;
}

} //> ns Athena

