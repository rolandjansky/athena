/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <cmath>
#include "boost/foreach.hpp"
#include <AthenaKernel/errorcheck.h>
#include "TrigSteering/ResultBuilder.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigInterfaces/AlgoConfig.h"
#include "TrigSteering/SteeringChain.h"
#include "TrigNavigation/Navigation.h"
//#include "TrigSteeringEvent/TrigOperationalInfo.h"
#include "TrigSteeringEvent/PartialEventBuildingInfo.h"
#include "TrigConfHLTData/HLTTriggerType.h"
#include "EventInfo/EventInfo.h"
#include "TrigSteeringEvent/ScoutingInfo.h"
#include "eformat/SourceIdentifier.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"

using namespace HLT;

ResultBuilder::ResultBuilder(const std::string& name, const std::string& type,
                             const IInterface* parent) :
   AthAlgTool(name, type, parent),
   m_config(0),
   m_streamingStrategy("HLT__DefaultStreamingStrategy", this),
   m_highestTriggerTypeBit(0),
   m_infoSize(0)
{
   declareInterface<HLT::ResultBuilder>( this );

   //declareProperty("doStreamTags", m_doStreamTags=true,
   //                  "Should the ResultBuilder extract the StreamTags from the HLT Chains and put them into the HLTResult object.");

   //  declareProperty("doTriggerTypes", m_doTriggerTypes=true,
   //                  "Should the ResultBuilder extract the TriggerTypes from the HLT Chains and put them into the HLTResult object.");
   declareProperty("ErrorStreamTags", m_errorStreamTagsProperty, "Mapping between errors and stream tags assigned to errorus event.");
   declareProperty("DefaultStreamTagForErrors", m_defaultStreamTagForErrors="hlterror", "Default stream name for all errors, it can be redefined for certain case by ErrorStreamTags");
   declareProperty("StreamingStrategy", m_streamingStrategy, "Streaming strategy");
   declareProperty("buildAlways", m_buildAlways=false, "Build events always, also for rejected events. This option is good for MC and lethal for online.");
}


ResultBuilder::~ResultBuilder()
{}


StatusCode ResultBuilder::initialize()
{
  CHECK(decodeErrorStreamTagsProperty());
  CHECK(m_streamingStrategy.retrieve());

  return StatusCode::SUCCESS;
}


ErrorCode ResultBuilder::setConfiguredChains(const std::vector<HLT::SteeringChain*>& configuredChains) {

  // find highest TriggerType bit configured for this run
  std::vector<HLT::SteeringChain*>::const_iterator chainIt;
  for ( chainIt = configuredChains.begin(); chainIt != configuredChains.end(); ++chainIt ) {
    if ( (int)m_highestTriggerTypeBit < (int)(*chainIt)->getChainCounter() ) {
      m_highestTriggerTypeBit = (int)(*chainIt)->getChainCounter();
    }
  }

  ATH_MSG_DEBUG("Highest TriggerType bit: " <<  m_highestTriggerTypeBit);

  m_infoSize = int(std::floor( m_highestTriggerTypeBit/(sizeof(uint32_t)*8) + 1. ));

  return HLT::OK;
}

ErrorCode ResultBuilder::setConfigurationKeys(uint32_t supermaster, uint32_t prescalers) {
  m_superMasterKey = supermaster;
  m_prescalesKey  = prescalers; 
  return HLT::OK;
}


StatusCode ResultBuilder::finalize()
{
  return StatusCode::SUCCESS;
}


bool lessThan_StreamTag(const TriggerInfo::StreamTag& a, const TriggerInfo::StreamTag& b) {
  return (a.name() + a.type() < b.name() + b.type());
}

bool equal_StreamTag(const TriggerInfo::StreamTag& a, const TriggerInfo::StreamTag& b) {
  return (a.name() + a.type() == b.name() + b.type());
}


ErrorCode ResultBuilder::hltExecute(const std::vector<HLT::SteeringChain*>& activeChains)
{
   ErrorCode retCode = HLT::OK;

   // find out whether HLTresult is already present in SG. If so, then take it
   // otherwise, create new and register it
   const HLTResult* constHltResult(0);
   HLTResult* hltResult(0);
   std::string resultKey = "HLTResult"+m_config->getInstance();
   if (evtStore()->transientContains<HLTResult>(resultKey) ) {
      if ( evtStore()->retrieve(constHltResult, resultKey).isFailure() ) {
         ATH_MSG_FATAL("Retrieval of precreated HLTResult failed");
         retCode = HLT::FATAL;
      }
      hltResult = const_cast<HLTResult*>(constHltResult);
   } else {
      hltResult = new HLTResult();
      StatusCode sc = evtStore()->overwrite(hltResult,resultKey, true, false);
      if(sc.isFailure()){
         ATH_MSG_FATAL("Store of HLTResult failed");
         retCode = HLT::FATAL;
      }
   }

   // if there is no config -> we write
   if (!m_config) {
      // Debug output
     ATH_MSG_ERROR("AlgoConfig pointers not set !!!"
                << " Creating dummy HLTResult only.");

      hltResult->setHLTStatus( HLT::FATAL );
      hltResult->setAccepted( false );
      hltResult->setPassThrough( false );
      //    hltResult->setCalibEvent( false ); // TODO
      //    hltResult->setCalibStatus( HLT::OK ); //TODO
      hltResult->setHLTLevel( HLT::UNKNOWN );

   } else {

      // init default flags
      bool eventPassed = false;
      bool eventPassedRaw = false; // w/o pass-through
      bool eventPT = false; // event passed-through, i.e. event not normally accepted but only because of a chain that was passThrough

      unsigned int satisfiedChainsCount = 0;
      unsigned int mostSevereChainCounter = 0;
      HLT::ErrorCode mostSevereChainCode = HLT::OK;

      for (std::vector<HLT::SteeringChain*>::const_iterator iterChain = activeChains.begin();
           iterChain != activeChains.end(); ++iterChain) {
         eventPT = eventPT || (*iterChain)->isPassedThrough();
         eventPassed = eventPassed || (*iterChain)->chainPassed();      
         eventPassedRaw = eventPassedRaw || ( (*iterChain)->chainPassedRaw() && !(*iterChain)->isPrescaled() );
         satisfiedChainsCount += ( (*iterChain)->chainPassed() ? 1 : 0);
         if (  mostSevereChainCode < (*iterChain)->getErrorCode()   ) {
            mostSevereChainCode = (*iterChain)->getErrorCode();
            mostSevereChainCounter = (*iterChain)->getChainCounter();
         }
      }
      // Debug output
      if (eventPassed)
        ATH_MSG_DEBUG("Event passed.");
      else
        ATH_MSG_DEBUG("Event not passed.");

      // generic stats
      // fixed fields in HLTResult
      hltResult->setLvl1Id( m_config->getLvl1Id() );
      hltResult->setAccepted( eventPassed );
      hltResult->setPassThrough( eventPT && !eventPassedRaw );
      hltResult->setHLTStatus( m_config->getHltStatus() );
      hltResult->setHLTLevel( m_config->getHLTLevel() );
      hltResult->setNumOfSatisfiedSigs( satisfiedChainsCount );
      hltResult->setLvlConverterStatus( m_config->getLvlConverterStatus() );
      hltResult->setCreatedOutsideHLT( false );
      hltResult->setErrorCoordinates( mostSevereChainCounter );

      hltResult->setConfigSuperMasterKey(m_superMasterKey);
      hltResult->setConfigPrescalesKey(m_prescalesKey);

      // TriggerType bits
      // moved up to the TrigSteer to be able to insert the Cost tools calls in between
      // ---------------------------------
      //bool eventToBeRecorded = false;
      //    retCode = fillTriggerInfo(activeChains, eventToBeRecorded);
      //    if ( retCode != HLT::OK ) {
      //      return retCode;
      //    }



      // if the event is rejected we have collected sufficient information for DF
      //if (hltResult->isAccepted()) {
      if ( m_uniqueStreams.empty() && hltResult->isAccepted()  ){
         ATH_MSG_WARNING("overall decision for event is positive while set of streams is empty");
      }

      if (!m_uniqueStreams.empty() || hltResult->isAccepted() || m_buildAlways ) { // this is redundant

         // first deal with the chains
         // --------------------------
         std::vector<uint32_t>& chainData = hltResult->getChainResult();
         // start with an empty std::vector<uint23_t>
         chainData.clear();
         // reserve first entry for number of chains:
         chainData.push_back(0);
         // used to copy results from each chain to main result vector
         std::back_insert_iterator< std::vector<uint32_t> > appendIterator(chainData);
         unsigned int nChains=0;

         // Include all chains from this level that were activated by the LvlConverter at the beginning of this event
         for (std::vector<HLT::SteeringChain*>::const_iterator iterChain = activeChains.begin();
              iterChain != activeChains.end(); ++iterChain) {
            std::vector<uint32_t> thisChainData;
            // each chain has size == 1 !
            //	thisChainData.push_back(0); // reserve for size of this chainData
            (*iterChain)->serialize(thisChainData); // get serialized chain
            //	thisChainData[0] = thisChainData.size(); // insert its size
            copy ( thisChainData.begin(), thisChainData.end(), appendIterator ); // copy to main result vector
            nChains++;
         }
         chainData.at(0) = nChains;

         // some debug output:
         if (msgLvl(MSG::DEBUG)) {
            ATH_MSG_DEBUG(nChains << " Chains that go into the HLTResult of " << m_config->getInstance().substr(1,3));
            ATH_MSG_DEBUG("REGTEST New Event ---------------");
            for (std::vector<SteeringChain*>::const_iterator it = activeChains.begin();
                 it != activeChains.end(); ++it) {
               ATH_MSG_DEBUG("REGTEST " <<  (**it));
               //          ATH_MSG_DEBUG("EXTREG  " << (**it) << " error code: " <<  strErrorCode((*it)->getErrorCode()) << " resurrected: " << (*it)->isResurrected());
            }
         }



         // third, navigation incl. features
         // ---------------------------------
         if (m_config->getNavigation()) {
            std::vector<uint32_t>& navData  = hltResult->getNavigationResult();
            std::vector<unsigned int>& cuts = hltResult->getNavigationResultCuts();
            std::vector<std::pair<CLID, std::string> >& clid_name = hltResult->getNavigationResultIDName();



            std::vector<uint32_t>& navData_DSonly  = hltResult->getNavigationResult_DSonly();
            std::vector<unsigned int>& cuts_DSonly = hltResult->getNavigationResultCuts_DSonly();
            std::vector<std::pair<CLID, std::string> >& clid_name_DSonly = hltResult->getNavigationResultIDName_DSonly();

            // start with an empty std::vector<uint23_t>
            navData.clear();


            cuts.clear();
            clid_name.clear();

            navData_DSonly.clear();
            cuts_DSonly.clear();
            clid_name_DSonly.clear();



            m_config->getNavigation()->serialize( navData, cuts, clid_name);
            m_config->getNavigation()->serialize_DSonly( navData_DSonly, cuts_DSonly, clid_name_DSonly);

            hltResult->setScoutingMap(m_modId_clid_name); //The map is filled into fillTriggerInfo. Now it is needed to copy it into the ScoutingInfo map in HLTResult.

            // some navigation debug output:
            ATH_MSG_DEBUG("\n" << *(m_config->getNavigation()));

        
         } else retCode =  HLT::FATAL;
      }
   }
   return retCode;
}


void ResultBuilder::setTriggerTypeBit(unsigned int bit, std::vector<uint32_t>& bitarray ) {
   // find if bit is still in the range of already allocated trigger info if no allocate more
   const unsigned word_size_in_bits = sizeof(uint32_t)*8;
   // find a word to set the bit
   unsigned word = bit/word_size_in_bits;
   unsigned shift = bit%word_size_in_bits;
   bitarray[word] |= 0x1 << shift;

}

struct StreamTagCollectionHelper {
  StreamTagCollectionHelper(const std::string& n, const std::string& t, bool l)
  : name(n), type(t), obey_lb(l) {}
  StreamTagCollectionHelper() : obey_lb(false) {}
  std::string name;
  std::string type;
  bool obey_lb;
  PartialEventBuildingInfo pebi;
  ScoutingInfo Scoutinfo;
};


ErrorCode ResultBuilder::fillTriggerInfo(const std::vector<SteeringChain*>& activeChains) {
   // record express stream PS decisions in new TrigOperationalInfo object
   xAOD::TrigCompositeContainer* streamContainer=0;
   xAOD::TrigComposite* streamInfo=0;
   xAOD::TrigCompositeAuxContainer aux;

   //   TrigOperationalInfo* streamOPI = 0;
   if(m_config->getHLTLevel() == HLT::EF || m_config->getHLTLevel() == HLT::HLT) {
     std::string key;
     // streamOPI = new TrigOperationalInfo();
     // m_config->getNavigation()->attachFeature(m_config->getNavigation()->getInitialNode(), 
     //                                          streamOPI, HLT::Navigation::ObjectCreatedByNew, key, 
     //                                          "EXPRESS_OPI"+m_config->getInstance() );
     streamContainer = new xAOD::TrigCompositeContainer();
     streamContainer->setStore(&aux);
     streamInfo = new xAOD::TrigComposite();
     streamContainer->push_back(streamInfo);
     streamInfo->setName("Express_stream");
     m_config->getNavigation()->attachFeature(m_config->getNavigation()->getInitialNode(), 
					      streamContainer, HLT::Navigation::ObjectCreatedByNew, key, 
					      "Express_stream"+m_config->getInstance() );
   }

   // the Chain's StreamTags
   // ---------------------------------

   // Collect all StreamTags for this event,
   // i.e. take all StreamTags that are _not_ prescaled from all chains that passed.
   // Then, make a unique list of these StreamTags and add it to the HLTResult.
   m_uniqueStreams.clear();
   m_modId_clid_name.clear();

   // special streams for errorous events  
   if ( m_config->getHltStatus().action() >= HLT::Action::ABORT_CHAIN ) {
      std::map<HLT::ErrorCode, TriggerInfo::StreamTag>::const_iterator errorStreamIt = m_errorStreamTags.find(m_config->getHltStatus());
      if ( errorStreamIt != m_errorStreamTags.end() ) { // custom stream name for this error      
         if ( errorStreamIt->second.name() != "" ) { // potential masking of the specific HLT::ErrorCode
            m_uniqueStreams.push_back(errorStreamIt->second);  // just copy it
         } 
      } else {
         if ( m_defaultStreamTagForErrors != "" ) { // another potential of total masking of debug stream comes here
            m_uniqueStreams.push_back(TriggerInfo::StreamTag( m_defaultStreamTagForErrors,"debug", false));	
         }
      }
      // we have a debug stream tag, so put module ID 0 into m_modId_clid_name
      m_modId_clid_name[0]; // (the default value is what we want for this key)
   }

   {
      typedef std::map<std::string, StreamTagCollectionHelper> StreamTagCollectionHelperMap;
      StreamTagCollectionHelperMap collectionHelper;
      StreamTagCollectionHelperMap::iterator strIt;

      //      std::vector<int> es_chain_counters; // chain_counters of chains which sent the event to the express stream

      if ( m_uniqueStreams.size() == 0 ) { // it means no debug stream
         // list of all StreamTags

         std::map<std::string, StreamTagCollectionHelper>::iterator strIt;

         // loop over all active chains:
         //for (std::vector<HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
         BOOST_FOREACH( const HLT::SteeringChain* chain, activeChains ) {
            // take only those that passed:
            if ( chain->chainPassed() ) {
               // and get all streams from the chain
               BOOST_FOREACH ( const HLT::StreamTag& chain_stream, chain->getStreamTags() ) {
                  //	for (std::vector<HLT::StreamTag>::const_iterator chain_stream = (*chain)->getStreamTags().begin();
                  //	     chain_stream !=  (*chain)->getStreamTags().end(); ++chain_stream ) {
                  // if stream is not prescaled!
                  if ( ! chain_stream.isPrescaled() ) {
                     
                     strIt = collectionHelper.find(chain_stream.getStream());
                     if ( strIt == collectionHelper.end() ) {
                        collectionHelper[chain_stream.getStream()] = StreamTagCollectionHelper( chain_stream.getStream(), chain_stream.getType(), chain_stream.getObeyLB());
                        strIt = collectionHelper.find(chain_stream.getStream()); // make the iterator valid
                     }
                     
                     // merge PEBI sets
		     if (( chain_stream.getType() == "calibration" ) || ( chain_stream.getType() == "monitoring" ))  {
                        const PartialEventBuildingInfo* chain_pebi = chain->getPEBInfo();
                        if ( chain_pebi ) {
                           strIt->second.pebi.merge(*chain_pebi);
                        }
                        // merge ScoutingInfo sets
                        if (chain_stream.getStream().find("DataScouting_") == 0){
                           const ScoutingInfo* chain_SI = chain->getScoutingInfo();
                           if ( chain_SI ){
                              strIt->second.Scoutinfo.merge(*chain_SI);
                           }
                        }
                     }
                     
                     // if(streamOPI && chain_stream.getType() == "express") {
                     //    streamOPI->set(chain->getChainName(), chain_stream.prescaleFactor());
                        
                     //    ATH_MSG_VERBOSE("Set expressOPI to 1 for: " << chain->getChainName());
                     // }
		     if (streamInfo && chain_stream.getType() == "express") {
		       //chid = chain->getChainCounter();
		       streamInfo->setDetail(chain->getChainName(), chain_stream.prescaleFactor());
		       //		       es_chain_counters.push_back(chid);
		       ATH_MSG_VERBOSE("Set express stream prescale to 1 for: " << chain->getChainName());
		     }
                  }
               }
            }
         }
      }
      
      for(auto& helper : collectionHelper) {
         if ( helper.first.find("DataScouting_") == 0){
            std::size_t found_first = helper.first.find_first_of('_');
            std::size_t found_last = helper.first.find_first_of('_', found_first+1);
            std::string rob_str = helper.first.substr(found_first+1,found_last-found_first+1);
            auto mod_id = static_cast<uint16_t>(atoi(rob_str.c_str())); // The module ID is taken from the stream tag
            
            // add the ROB ID to the stream tag
            auto id = eformat::helper::SourceIdentifier{eformat::TDAQ_HLT, mod_id};
            helper.second.pebi.add(id.code());
            
            std::set<std::pair<CLID, std::string> > clid_name= helper.second.Scoutinfo.get();
            
            auto it_map = m_modId_clid_name.find(mod_id);
            if ( it_map == m_modId_clid_name.end() ) // If the module ID is not in the map yet fill the map with the Mod ID -> set<CLID, collectionName>
               m_modId_clid_name.insert(std::pair<unsigned int, std::set<std::pair<CLID, std::string> > >(mod_id, clid_name));
            else
               {  // If the module ID is already in the map add the set<CLID, collectionName> to the already existing set
                  for (std::set<std::pair<CLID, std::string> >::iterator it_set = clid_name.begin(); it_set != clid_name.end(); ++it_set )
                     (it_map->second).insert(*it_set);
               }
         }
         else{ // If there is at least 1 passed chain without "DataScouting_" in the name add 0 to the map. Only one 0 is needed. (This is used to force accept).
            m_modId_clid_name[0]; // (the default value is what we want for this key)
         }
         
         m_uniqueStreams.emplace_back(helper.second.name,
                                      helper.second.type,
                                      helper.second.obey_lb,
                                      helper.second.pebi.getROBs(),
                                      helper.second.pebi.getSubDetectors());
      }

      //    // make a unique list of all streams
      //    std::vector<TriggerInfo::StreamTag>::iterator new_end = std::unique(allStreams.begin(), allStreams.end(), equal_StreamTag);
      //    m_uniqueStreams.insert(m_uniqueStreams.end(), allStreams.begin(), new_end);
   }


   // modify streams according to the policy
   m_streamingStrategy->apply(m_uniqueStreams);


   // Debug output: print streams
   if (msgLvl(MSG::DEBUG)) {
      ATH_MSG_DEBUG(m_uniqueStreams.size() << " StreamTags of this event: "
               << m_config->getInstance().substr(1,3));
      BOOST_FOREACH(const TriggerInfo::StreamTag& st, m_uniqueStreams ) {

         ATH_MSG_DEBUG("REGTEST " << "Stream name = " << st.name() << " type = " << st.type() << " obeyLB = "
                    << st.obeysLumiblock());

         if (!st.robs().empty()) {
            std::set<unsigned int> robs(st.robs());
            ATH_MSG_DEBUG("PEB ROBs " << std::vector<unsigned int>(robs.begin(), robs.end())); // uff, MsgStream supprts vector but not set
         }

         if (!st.dets().empty()) {
            std::set<unsigned int> dets(st.dets());
            ATH_MSG_DEBUG("PEB Dets " << std::vector<unsigned int>(dets.begin(), dets.end()));
         }
      }
   }


   // the Chain's TriggerTypes
   // ---------------------------------
   std::vector<uint32_t> triggerTypeBits;
   triggerTypeBits.resize( m_infoSize );

   ATH_MSG_VERBOSE("Bit array to hold TriggerInfo bits resized to: " << triggerTypeBits.size()
                << " words, highest chain_counter: " <<  m_highestTriggerTypeBit);


   // Collect all TriggerTypes for this event,
   // i.e. take all TriggerTypes that from all chains that passed.
   // Then, make combine these TriggerTypes with the logical OR operator and add it to the HLTResult.
   // loop over all active chains:
   for (std::vector<HLT::SteeringChain*>::const_iterator chain = activeChains.begin();
        chain != activeChains.end(); ++chain) {
      // take only those that passed:
      if ( (*chain)->chainPassed() ) {
         ATH_MSG_VERBOSE("setting bit: " << (*chain)->getChainCounter() << " in TriggerInfo");
         setTriggerTypeBit((*chain)->getChainCounter(), triggerTypeBits);
      }
   }

   //  hltResult->setTriggerTypeBits(triggerTypeBits);

   // Debug output: print streams
   if (msgLvl(MSG::DEBUG)) {
      msg() << MSG::DEBUG << "TriggerType bits of " << m_config->getInstance().substr(1,3 ) << " words: " << triggerTypeBits.size() << " content" ;
      for (  std::vector<uint32_t>::const_iterator it = triggerTypeBits.begin();
             it != triggerTypeBits.end(); ++it ) {
         msg() << MSG::DEBUG << " 0x" << MSG::hex  << *it;
      }
      msg() << MSG::DEBUG << MSG::dec << endmsg;
   }



   ////////////////////////////////////////////////////////////////////
   // recording
   ////////////////////////////////////////////////////////////////////


   // put it all to SG EventInfo object
   // get EventInfo
   const EventInfo* constEventInfo(0);
   StatusCode sc = evtStore()->retrieve(constEventInfo);
   if(sc.isFailure()){
      ATH_MSG_FATAL("Can't get EventInfo object for update of the StreamTag and TriggerInfo");
      return HLT::FATAL;
   }

   ATH_MSG_VERBOSE("Updating TriggerInfo");

   EventInfo* eventInfo = const_cast<EventInfo*>(constEventInfo);
   TriggerInfo* oldtriggerInfo = eventInfo->trigger_info();
   TriggerInfo* triggerInfo;
   if (oldtriggerInfo!= 0){
      triggerInfo = new TriggerInfo(*oldtriggerInfo);
   } else {
      triggerInfo = new TriggerInfo();
   }

   // update the TriggerInfo
   /*  uint32_t lvl1Id = 0;
       uint32_t lvl1Info = 0;
       uint32_t status = 0;
       std::vector<uint32_t> lvl2Info;
       std::vector<uint32_t> efInfo;
   */

   /*
     if ( triggerInfo != 0 ) {
     lvl1Id   = triggerInfo->extendedLevel1ID();
     lvl1Info = triggerInfo->level1TriggerType();
     status   = triggerInfo->statusElement();
     lvl2Info = triggerInfo->level2TriggerInfo();
     efInfo   = triggerInfo->eventFilterInfo();
     ATH_MSG_VERBOSE("Saved old TriggerInfo");
     //delete triggerInfo;    // delete old TriggerInfo if ptr was not null
     }
   */

   // replace trigger Type bits depending on the level (L2 or EF)
   if ( m_config->getHLTLevel() == HLT::L2 ) {
      //lvl2Info = triggerTypeBits; // lvl2 stuff is replaced
      triggerInfo->setLevel2TriggerInfo(triggerTypeBits);
      ATH_MSG_VERBOSE("Updated TriggerInfo for L2");
   } 
   else if ( m_config->getHLTLevel() == HLT::EF ){   
      //efInfo   = triggerTypeBits; // ef stuff is replaced
      triggerInfo->setEventFilterInfo(triggerTypeBits);
      ATH_MSG_VERBOSE("Updated TriggerInfo for EF");


      if(oldtriggerInfo!= 0){
         ATH_MSG_VERBOSE("Saving previous level calibration Stream Tags");
         
         std::vector<TriggerInfo::StreamTag> oldstream = oldtriggerInfo->streamTags();
         std::vector<TriggerInfo::StreamTag>::const_iterator oldstreamI = oldstream.begin();
         std::vector<TriggerInfo::StreamTag>::const_iterator oldstreamE = oldstream.end();
         for(;oldstreamI!=oldstreamE; ++oldstreamI){
            if(oldstreamI->type()=="calibration"){
               ATH_MSG_DEBUG("Keeping calibration Stream Tag: " << oldstreamI->name());
               m_uniqueStreams.push_back(*oldstreamI);
            }
         }
         std::sort(m_uniqueStreams.begin(), m_uniqueStreams.end(), lessThan_StreamTag);
	     
         // make a unique list of all streams
         std::vector<TriggerInfo::StreamTag>::iterator new_end = std::unique(m_uniqueStreams.begin(), m_uniqueStreams.end(), equal_StreamTag);
         m_uniqueStreams.erase(new_end,  m_uniqueStreams.end());   
      }
   }
   else if ( m_config->getHLTLevel() == HLT::HLT){
      std::vector<unsigned int> zeroL2(1); zeroL2[0]=0x0;
      triggerInfo->setLevel2TriggerInfo(zeroL2); // set L2 result to 0
      triggerInfo->setEventFilterInfo(triggerTypeBits);
      ATH_MSG_VERBOSE("Updated TriggerInfo for HLT");
   }
  
 
   //triggerInfo = new TriggerInfo( status, lvl1Id, lvl1Info, lvl2Info, efInfo, uniqueStreams );
   triggerInfo->setStreamTags(m_uniqueStreams);
   eventInfo->setTriggerInfo(triggerInfo);
   ATH_MSG_VERBOSE("Updated TriggerInfo attached to EventInfo:");
   ATH_MSG_VERBOSE(*triggerInfo);

  
  
  /////////////////////////////////
  //// Recording in xAODEventInfo
  ////////////////////////////////
  // put it all to SG xAOD::EventInfo object
 
  const xAOD::EventInfo* constxEventInfo(0);
  sc = evtStore()->retrieve(constxEventInfo);
  
  if (sc.isFailure()) {   
    ATH_MSG_FATAL("Can't get xAOD::EventInfo object for update of the StreamTag");
    return HLT::FATAL;
  }
  

#if 0
  //create xAOD::StreamTags
  std::vector < xAOD::EventInfo::StreamTag > xAODStreamTags;
  for (auto streamtag : m_uniqueStreams){
    //copy streamtag into xAOD object
    xAOD::EventInfo::StreamTag xstreamtag(streamtag.name(), streamtag.type(), streamtag.obeysLumiblock(), streamtag.robs(), streamtag.dets());
    xAODStreamTags.push_back(xstreamtag);
  }

  xAOD::EventInfo* xeventInfo = const_cast<xAOD::EventInfo*>(constxEventInfo);
  xeventInfo->setStreamTags(xAODStreamTags);
  ATH_MSG_VERBOSE("Updated xAOD::StreamTags into xAOD::EventInfo:");
#endif
  
  
   return HLT::OK;
}

/*
ErrorCode ResultBuilder::getPEBInfo() {

  // put it all to SG EventInfo object
  // get EventInfo
  std::vector<uint32_t> robs;
  std::vector<uint32_t> rob;
  PartialEventBuildingInfo* pebinfo = 0;
  pebinfo = new PartialEventBuildingInfo();
  const DataHandle<PartialEventBuildingInfo> it, end;
  (evtStore()->retrieve(it,end)).ignore();
  if(it==end){
    ATH_MSG_DEBUG("No partialEventBuildingInfo object for update of the StreamTag and TriggerInfo");
      //return StatusCode::SUCCESS;
  } else {
    int pebsize = distance(it,end);
    for( ; it!=end; ++it){
      it->getROBs(robs);
      std::cout << "ResultBuilder peb key = " << it.key() << std::endl;
      std::vector<uint32_t>::const_iterator robid = robs.begin();
      for(; robid!=robs.end();++robid){
         std::cout << "robs already here = " << std::hex << *robid << std::dec << std::endl;
      }
      //m_config->setPEBI(it);
    }
    if(pebsize > 1){
       std::cout << "there is more than one peb" << std::endl;
       std::vector<uint32_t>::iterator duplicate;
       sort(robs.begin(),robs.end());
       duplicate = unique(robs.begin(),robs.end());
       robs.erase(duplicate,robs.end());
       std::cout << "erasing duplicated rob id = " << *duplicate << std::endl;

    }
    std::cout << "rob vector size = " << robs.size() << std::endl;
    pebinfo->addROBs(robs);
  }
  m_config->setPEBI(pebinfo);
  //}

  return HLT::OK;

}
 */

StatusCode ResultBuilder::decodeErrorStreamTagsProperty() {
   std::stringstream ss;
   std::vector<std::string>::const_iterator it;
   for ( it = m_errorStreamTagsProperty.begin(); it != m_errorStreamTagsProperty.end(); ++it ){
      ss.clear();
      ss.str(*it);
      // read it now
      std::string errorName;
      getline(ss, errorName, ':');


      int code = getErrorCodePosFromStr(errorName);
      if ( code < 0 ) {
         ATH_MSG_FATAL("While setting streams for errorus events: ErrorCode: "
                       << errorName << " unrecognized");
         return StatusCode::FAILURE;
      } else {
         ErrorCode ec = static_cast<HLT::ErrorCode>(code);
         // read further stream name
         //
         std::string streamName;
         std::string streamType;
         //getline(ss, errorName,'\0');
         ss >> streamName;
         ss >> streamType;
         if ( streamName != "" ) {
            if (streamType == "")
               streamType = "debug";
            m_errorStreamTags[ec] = TriggerInfo::StreamTag (streamName, streamType, false);

            ATH_MSG_DEBUG("StreamTag for erroneous events with error code: "
                          << errorName << " is " << streamType << ":" << streamName);
         } else {
            m_errorStreamTags[ec] = TriggerInfo::StreamTag ("", "debug", false);
            ATH_MSG_DEBUG("StreamTag for erroneous events with error code: "
                          << errorName << " is disabled");
         }

      }
   }

   return StatusCode::SUCCESS;
}

std::vector<TriggerInfo::StreamTag> ResultBuilder::getErrorStreamTags() const 
{ 
  std::vector<TriggerInfo::StreamTag> streamTags;

  // Default error stream
  streamTags.push_back(TriggerInfo::StreamTag( m_defaultStreamTagForErrors,"debug", false));	
        
  // Additional configured debug streams
  for (auto& kv : m_errorStreamTags) {
    streamTags.push_back(kv.second);    
  }

  // make a unique list of all streams
  std::sort(streamTags.begin(), streamTags.end(), lessThan_StreamTag);
  std::vector<TriggerInfo::StreamTag>::iterator new_end = std::unique(streamTags.begin(), streamTags.end(), equal_StreamTag);
  streamTags.erase(new_end, streamTags.end());   
  
  return streamTags;
}
