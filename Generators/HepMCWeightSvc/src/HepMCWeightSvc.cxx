/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "HepMCWeightSvc.h"

#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventType.h"
#include "EventInfo/EventID.h"
#include "EventInfo/EventStreamInfo.h"

#include "IOVDbDataModel/IOVMetaDataContainer.h"

HepMCWeightSvc::HepMCWeightSvc(const std::string& name, ISvcLocator* pSvcLocator)
  : AthService(name, pSvcLocator),
   m_metaDataTool("IOVDbMetaDataTool"),m_weightNamesLoaded(false)
{

}


bool HepMCWeightSvc::weightNamesChanged(const std::map<std::string, std::size_t>& weightNames) {
   
   //check all weights are the same 
   if(m_currentWeightNames.size() != weightNames.size()) {
      return true;
   }
   auto itr1 = m_currentWeightNames.begin();
   auto itr2 = weightNames.begin();

   for(;itr1!=m_currentWeightNames.end();++itr1,++itr2) {
      if( itr1->first != itr2->first ) return true;
      if( itr1->second != itr2->second ) return true;
   }
   return false;
}

StatusCode HepMCWeightSvc::initialize() {
   ServiceHandle<IIncidentSvc> incsvc("IncidentSvc",name());
   int priority = 100;
   incsvc->addListener( this, IncidentType::BeginInputFile, priority);
   return StatusCode::SUCCESS;
}

void HepMCWeightSvc::handle(const Incident& inc) {
   if(inc.type()==IncidentType::BeginInputFile) { //resets the weight names
      m_weightNamesLoaded = false; 
      m_currentWeightNames.clear();
   }
}

StatusCode HepMCWeightSvc::loadWeights() {
   //clear the currentWeightNames 
   m_currentWeightNames.clear();
   m_weightNamesLoaded = true; //regardless of success or failure, we will say the weights are loaded!

   //read the weight map from the metadata, it will be std::map<std::string,int> so we need to convert to our internal type 
   ServiceHandle<StoreGateSvc> inputMetaStore("StoreGateSvc/InputMetaDataStore",name());
   CHECK( inputMetaStore.retrieve() );
   
   const IOVMetaDataContainer* cont = inputMetaStore->tryConstRetrieve<IOVMetaDataContainer>("/Generation/Parameters");
   if(cont==NULL) {
     //exit quietly... 
     return StatusCode::SUCCESS;
   }
   //payload is a collection of condattrlistcollections
   //only look a the first one, assuming it exists, and within that only look at the first channel;
   if(! (cont->payloadContainer()->size()>0 && cont->payloadContainer()->at(0)->size()>0) ) return StatusCode::FAILURE;


   int chanNum(0);
   //if there is only one collection of weights, then we just load that one 
   if(cont->payloadContainer()->at(0)->size()==1) {
     chanNum = cont->payloadContainer()->at(0)->chanNum(0);
   } else {
     ATH_MSG_DEBUG("Multiple /Generation/Parameters attributeLists found ... using EventStreamInfo to determine which to use");
     //need to retrieve the EventStreamInfo to get at the channel number 
     const EventStreamInfo* esi = 0;
     CHECK( inputMetaStore->retrieve(esi) );
     if(esi->getEventTypes().size() == 0) return StatusCode::FAILURE;
     chanNum = esi->getEventTypes().begin()->mc_channel_number();
     if(chanNum==0) {
       //perhaps channel number not set yet, use the first run number instead 
       if(esi->getRunNumbers().size()==0) return StatusCode::FAILURE;
       chanNum = *esi->getRunNumbers().begin();
     }
   }
   const coral::Attribute& attr = cont->payloadContainer()->at(0)->attributeList(chanNum)["HepMCWeightNames"];

   std::map<std::string, int> in; 

   CHECK( Gaudi::Parsers::parse(in,attr.data<std::string>()) );

   ATH_MSG_DEBUG("Loaded weightnames: " << attr.data<std::string>());

   
   for(auto& i : in) m_currentWeightNames[i.first] = i.second;



   return StatusCode::SUCCESS;


}

StatusCode HepMCWeightSvc::setWeightNames(const std::map<std::string, std::size_t>& weightNames) {

   //ignore any attempt to set 'nothing' for the weight names
   if(weightNames.size()==0 || (weightNames.size()==1 && weightNames.begin()->first=="0") ) return StatusCode::SUCCESS;

   if(m_weightNamesLoaded) return StatusCode::SUCCESS; //we only allow setting of weightNames ONCE! ... if the weights have ever been loaded, we will not allow them to be set again!
   //effectively means the weights are only set in evgen jobs.

   //check that the weight names correspond to what we have currently. 
   //if( weightNamesChanged(weightNames) ) {
      m_currentWeightNames = weightNames;

      //create and register the metadata containter with these weight names 
      //use the MetaDataTool to do this 
      CHECK( m_metaDataTool->registerFolder("/Generation/Parameters","Metadata created during Event Generation") );

      //create a new attributelist collection for it ...
      CondAttrListCollection* cont = new CondAttrListCollection(true /* use regular timestamps, not run-lumiblock timestamps */);
      //create a single attribute list
      CondAttrListCollection::AttributeList  myAttributes;

      //store as strings ... when read back in we use a gaudi parser to parse the list
      myAttributes.extend("HepMCWeightNames","string");
      
      std::string stringToStore = "{";
      for(auto& name : weightNames) {
         if(stringToStore != "{") stringToStore += ", ";
         std::stringstream ss;
         ss << "'" << name.first << "':" << name.second; stringToStore += ss.str();
      }
      stringToStore += "}";
      myAttributes["HepMCWeightNames"].data<std::string>() = stringToStore;

      //use the run-number as the 'channel' ... all weightnames should be the same for the same channel 

      //set the start time ... do we set the stop time?  
      const EventInfo* evt = 0;
      ServiceHandle<StoreGateSvc> evtStore("StoreGateSvc/StoreGateSvc",name());
      CHECK( evtStore->retrieve(evt) );
      //IOVTime evtTime;
      //evtTime.setRunEvent(evt->event_ID()->run_number(), evt->event_ID()->lumi_block());
      //uint64_t nsTime=evt->event_ID()->time_stamp()*1000000000LL;
      //nsTime += evt->event_ID()->time_stamp_ns_offset();
      //evtTime.setTimestamp(nsTime);
      //cont->addNewStart(evtTime);
      cont->add(evt->event_ID()->run_number(),myAttributes);



      ATH_MSG_INFO("Storing /Generation/Parameters :: WeightNames = " << stringToStore);

      //and assign it to 'channel 0' .. consistent with other metadata
      
      CHECK( m_metaDataTool->addPayload("/Generation/Parameters",cont) );

      m_weightNamesLoaded=true;

   //}

   return StatusCode::SUCCESS;
}


const std::map<std::string,std::size_t>& HepMCWeightSvc::weightNames() {
   if(!m_weightNamesLoaded) {
      if(loadWeights().isFailure()) { ATH_MSG_WARNING("Unable to load weightnames from metadata ... do not trust the weightnames!"); }
   }
   return m_currentWeightNames;
}
