/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Local includes:
#include "./ResultBuilder.h"
#include "./CTPTriggerThreshold.h"
#include "./CTPUtil.h"
#include "./CTPTriggerItem.h"
#include "./SpecialTrigger.h"

// eformat for data fragments:
#include "eformat/eformat.h"
#include "eformat/SourceIdentifier.h"

// Output to the DAQ:
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

// random number generator
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandFlat.h"

// STL includes:
#include <vector>
#include <map>

using namespace std;

LVL1CTP::ResultBuilder::ResultBuilder( const std::string& type,
                                       const std::string& name, 
                                       const IInterface* parent )
   : AthAlgTool(type, name, parent)
{
   m_ctpDataFormat = new CTPdataformatVersion(m_ctpVersionNumber);
}


LVL1CTP::ResultBuilder::~ResultBuilder() {
   delete m_ctpDataFormat;
   for(auto & x : m_internalTrigger) {
      delete x.second;
   }
}


StatusCode
LVL1CTP::ResultBuilder::setConfiguration( const TrigConf::CTPConfig* ctpConfig, 
                                          const TrigConf::L1Menu* l1menu ) const
{
   ATH_MSG_DEBUG( "Set configuration with CTP version " << m_ctpVersionNumber );

   ConfigSource cfgsrc(ctpConfig, l1menu);

   StatusCode sc = createTriggerConfigMaps(cfgsrc);

   return sc;
}


StatusCode
LVL1CTP::ResultBuilder::createTriggerConfigMaps(const ConfigSource & cfgSrc) const {

   if( cfgSrc.l1menu() != nullptr ) {

      ATH_MSG_DEBUG("Creating trigger configuration maps from run-3-style menu");

      std::vector<unsigned int> bg; bg.push_back( 1 );
      std::vector<unsigned int> bgEmpty; bgEmpty.push_back( 1 );

      // declare internal bunch group triggers
      for (size_t i = 0; i < 16; ++i) {
         auto bgrp = new BunchGroupTrigger(i, bg, m_ctpDataFormat);
         m_internalTrigger[ bgrp->name() ] = bgrp;
      }
     
      // declare internal random triggers
      for(int rndmIdx = 0; rndmIdx<4; rndmIdx++) {
         auto rndm = new RandomTrigger(rndmIdx, m_ctpDataFormat);
         m_internalTrigger[ rndm->name() ] = rndm;
      }

      // build map of name to ctp thresholds
      m_thrConfigMap = std::make_unique<ThresholdMap>( cfgSrc.l1menu() );

      // build map of name to ctp items
      m_itemConfigMap = std::make_unique<ItemMap>( cfgSrc.l1menu() );

   } else if( cfgSrc.ctpConfig() != nullptr ) {

      ATH_MSG_DEBUG("Creating trigger configuration maps from run-2-style menu");

      const std::vector<TrigConf::BunchGroup> & bunchGroups(cfgSrc.ctpConfig()->bunchGroupSet().bunchGroups());
      for (size_t i = 0; i < bunchGroups.size(); ++i) {
         std::vector<unsigned int> bunches;
         for(int b : bunchGroups[i].bunches()) {
            bunches.push_back(b);
         }
         auto bgrp = new BunchGroupTrigger(i, bunches, m_ctpDataFormat);
         m_internalTrigger[bgrp->name()] = bgrp;
      }
      
      for(int rndmIdx = 0; rndmIdx<4; rndmIdx++) {
         auto rndm = new RandomTrigger(rndmIdx, m_ctpDataFormat);
         m_internalTrigger[ rndm->name() ] = rndm;
      }

      m_thrConfigMap = std::make_unique<ThresholdMap>( cfgSrc.ctpConfig()->menu().thresholdVector());

      m_itemConfigMap = std::make_unique<ItemMap>( cfgSrc.ctpConfig()->menu().itemVector(),
                                     cfgSrc.ctpConfig()->prescaleSet() );
   } else {
      ATH_MSG_FATAL("No L1 trigger menu was provided");
      return StatusCode::FAILURE;
   }

   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::ResultBuilder::constructTIPVector( const std::map<std::string, unsigned int> & thrMultiMap,
                                            std::vector<uint32_t> & tip ) const
{
   tip.resize( m_ctpDataFormat->getTIPwords(), 0 );
   
   for( auto & entry : thrMultiMap ) {
      const std::string & thrName = entry.first;
      size_t multiplicity = entry.second;

      // if multiplicity is 0 then there is nothing to set
      if(multiplicity==0) {
         continue;
      }

      auto x = m_internalTrigger.find(thrName);
      if(x != m_internalTrigger.end()) {
         // internal triggers (BGRP or RNDM) 

         size_t wordNr = m_ctpDataFormat->getTIPwords() - 1; // internal triggers are stored in the last word
         size_t posWithinWord = x->second->pitPos() % 32;

         tip[wordNr] |= ( 1L << posWithinWord );

      } else {
         // all other trigger threshold multiplicities

         const CTPTriggerThreshold & ctpTT = m_thrConfigMap->getCTPThreshold( thrName );
         size_t startBit = ctpTT.startBit();
         size_t nBits = ctpTT.endBit() - startBit + 1;

         // restrict to the maximum possible multiplicity
         if(multiplicity >= (1U<<nBits)) {
            multiplicity = (1U<<nBits)-1;
         }

         size_t wordNr = startBit / 32;
         size_t posWithinWord = startBit % 32;

         uint64_t result = multiplicity; // need 64 bit in case the word boundary is crossed by the threshold start..end
         result <<= posWithinWord;

         tip[wordNr] |= result & 0xFFFFFFFF;
         if(wordNr+1 < m_ctpDataFormat->getTIPwords()-1) {
            tip[wordNr+1] |= uint32_t((result & 0xFFFFFFFF00000000) >> 32);
         }
      }
   }

   size_t wrdNr(0);
   for( uint32_t word : tip ) {
      ATH_MSG_DEBUG( "REGTEST - " << "TIP word #" << std::dec << wrdNr++
                     << " is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << word );
   }
		
   return StatusCode::SUCCESS;
}



StatusCode
LVL1CTP::ResultBuilder::buildItemDecision( const std::map<std::string, unsigned int> & thrMultiMap,
                                           std::map<std::string, unsigned int> & itemDecisionMap,
                                           CLHEP::HepRandomEngine* rndmEngine ) const
{
   // build trigger result for all items
   itemDecisionMap.clear();

   try {
      for( const auto & itemName : m_itemConfigMap->itemNames() ) {
         auto ctpItem = m_itemConfigMap->getItem(itemName);

         bool pass_beforePrescale =  ctpItem->evaluate(thrMultiMap);
         bool pass_afterPrescale = false;
         bool pass_afterVeto = false;

         if ( pass_beforePrescale ) {
            long random = CLHEP::RandFlat::shootInt( rndmEngine, pow(2,24) );
            int32_t cut = TrigConf::PrescaleSet::getCutFromPrescale( m_itemConfigMap->getItem( itemName )->prescale() );
            pass_afterPrescale = (random >= cut) && (cut > 0); // no pass if PS set to "-1"
            pass_afterVeto = pass_afterPrescale; // dead time is not simulated
         }

         unsigned int result = (pass_beforePrescale ? TBP : 0) + (pass_afterPrescale ? TAP : 0) + (pass_afterVeto ? TAV : 0);

         itemDecisionMap[itemName] = result;
      }
   } catch (TrigConf::LogicParsingException & ex) {
      ATH_MSG_FATAL("Error in evaluating logic. " << ex.msg());
      return StatusCode::FAILURE;
   }
   return StatusCode::SUCCESS;
}

StatusCode
LVL1CTP::ResultBuilder::constructResultVectors( const std::map<std::string, unsigned int> & itemDecisionMap,
                                                std::vector<uint32_t> & tbp, std::vector<uint32_t> & tap, std::vector<uint32_t> & tav,
                                                unsigned char & triggerType ) const
{
   tbp.resize( m_ctpDataFormat->getTBPwords(), 0 );
   tap.resize( m_ctpDataFormat->getTAPwords(), 0 );
   tav.resize( m_ctpDataFormat->getTAVwords(), 0 );
   triggerType = 0;

   for( auto & entry : itemDecisionMap ) {
      const std::string & itemName = entry.first;
      unsigned int result = entry.second;
      bool passBP = (result & TBP) != 0;
      bool passAP = (result & TAP) != 0;
      bool passAV = (result & TAV) != 0;




      auto l1Item = m_itemConfigMap->getItem( itemName );

      if( passAV ) {
         triggerType |= l1Item->triggerType();
      }

      auto ctpId = l1Item->ctpId();
      unsigned int wordNr = ctpId / 32;
      unsigned int posWithinWord = ctpId % 32;

      if( passBP ) { tbp[wordNr] |= 1L << posWithinWord; }
      if( passAP ) { tap[wordNr] |= 1L << posWithinWord; }
      if( passAV ) { tav[wordNr] |= 1L << posWithinWord; }

      ATH_MSG_DEBUG( "  --> Trigger item " << itemName << 
                     " is " << ( !passBP ? "INACTIVE" : ( passAV ? "ACTIVE" : "ACTIVE (but PRESCALED)" ) ) );
   }
   ATH_MSG_DEBUG( "REGTEST - " << "TriggerType byte is: 0x" << std::setw( 2 ) << std::setfill( '0' ) << std::hex << int(triggerType) );

   return StatusCode::SUCCESS;
}



	
std::unique_ptr<LVL1CTP::CTPSLink>
LVL1CTP::ResultBuilder::constructRoIResult( const EventIDBase & eventID,
                                            const std::vector<uint32_t> & tbp,
                                            const std::vector<uint32_t> & tap,
                                            const std::vector<uint32_t> & tav,
                                            const std::vector<uint32_t> & tip,
                                            const unsigned char triggerType ) const
{
   //
   // Build the data words:
   //
   std::vector<uint32_t> roi_vector;		
   roi_vector.push_back( eventID.time_stamp_ns_offset() ); // Time stamp: 28-bit nanoseconds
   roi_vector.push_back( eventID.time_stamp() );           // Time stamp: 32-bit UTC seconds
		
   roi_vector.insert(roi_vector.end(), tip.begin(), tip.end()); // TIP + 1 extra word
   roi_vector.insert(roi_vector.end(), tbp.begin(), tbp.end()); // TBP
   roi_vector.insert(roi_vector.end(), tap.begin(), tap.end()); // TAP
   roi_vector.insert(roi_vector.end(), tav.begin(), tav.end()); // TAV
		
   ATH_MSG_VERBOSE( "Stored data elements of RoI result" );
		
   //
   // Build the trailer:
   //
		
   std::vector<unsigned int> trailer(ROIB::Trailer(roi_vector.size()).trailer());
   roi_vector.insert(roi_vector.end(), trailer.begin(), trailer.end());
   ATH_MSG_VERBOSE( "Created trailer of RoI result" );
		
   //
   // Build the header:
   //
		
   // convention for source id in LVL1: 0 for DAQ, 1 for RoIB
   ROIB::Header helperHeader(eformat::helper::SourceIdentifier(eformat::TDAQ_CTP, 1).code());
		
   helperHeader.setRunNumber( eventID.run_number());
   //helperHeader.setL1ID( eventID.extendedL1ID());
   helperHeader.setBCID( eventID.bunch_crossing_id());
   helperHeader.setTriggerType(triggerType); 
   //helperHeader.setEventType(EventInfo::instance().eventType());
		
   std::vector<unsigned int> header(helperHeader.header());
   roi_vector.insert(roi_vector.begin(), header.begin(), header.end());
   ATH_MSG_VERBOSE( "Created header of RoI result" );
		
   //
   // Build the SLink:
   //
		
   std::unique_ptr<CTPSLink> result ( new CTPSLink( roi_vector, m_ctpVersionNumber ));
   ATH_MSG_DEBUG( "Created CTPSlink object" );
   ATH_MSG_VERBOSE( "Dump CTPSlink object:\n" + result->dump() );
		
   //
   // Debug output for ART
   // 
   if( msgLvl(MSG::VERBOSE) ) {
      for( auto & itemName : firedItems(tbp) ) { 
         ATH_MSG_VERBOSE( "REGTEST - Items fired before prescale: "  << itemName );
      }
      for( auto & itemName : firedItems(tap) ) { 
         ATH_MSG_VERBOSE( "REGTEST - Items fired after prescale: "  << itemName );
      }
      for( auto & itemName : firedItems(tav) ) { 
         ATH_MSG_VERBOSE( "REGTEST - Items fired after veto: "  << itemName );
      }
   }	
   return result;
}

	
std::unique_ptr<CTP_RDO>
LVL1CTP::ResultBuilder::constructRDOResult( const EventIDBase & eventID,
                                            const std::vector<uint32_t> & tbp,
                                            const std::vector<uint32_t> & tap,
                                            const std::vector<uint32_t> & tav,
                                            const std::vector<uint32_t> & tip ) const
{		
   std::unique_ptr<CTP_RDO> result( new CTP_RDO(m_ctpVersionNumber) );
   result->setTimeSec(eventID.time_stamp());                // Time stamp: 32-bit UTC seconds
   result->setTimeNanoSec(eventID.time_stamp_ns_offset());  // Time stamp: 28-bit nanoseconds
   for (unsigned int i(0); i < m_ctpDataFormat->getTIPwords(); ++i) {
      result->setTIPWord(i, tip[i]);
   }
   for (unsigned int i(0); i < tbp.size(); ++i) {
      result->setTBPWord(i, tbp[i]);
   }
   for (unsigned int i(0); i < m_ctpDataFormat->getTAPwords(); ++i) {
      result->setTAPWord(i, tap[i]);
   }
   for (unsigned int i(0); i < m_ctpDataFormat->getTAPwords(); ++i) {
      result->setTAVWord(i, tav[i]);
   }
   ATH_MSG_DEBUG( "Created CTP_RDO object" );
   return result;
}

	

	

	
std::vector<std::string>
LVL1CTP::ResultBuilder::firedItems(const std::vector<uint32_t>& triggerWords) const {
   std::vector<std::string> passedItems;    
   for( const std::string & itemName : m_itemConfigMap->itemNames() ) {
      auto item = m_itemConfigMap->getItem(itemName);
      size_t idx = item->ctpId() / 32;
      size_t bit = item->ctpId() % 32;
      if ( triggerWords[idx] % (1L << bit) ) {
         passedItems.push_back(item->name());
      }
   }
   std::sort(passedItems.begin(), passedItems.end());
   return passedItems;
}
	
