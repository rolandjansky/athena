/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// STL includes:
#include <cassert>
#include <vector>
#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>

// Trigger configuration includes:
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/TriggerItem.h"

// eformat for data fragments:
#include "eformat/eformat.h"
#include "eformat/SourceIdentifier.h"

// Output to the RoIB:
#include "TrigT1Interfaces/CTPSLink.h"

// Output to the DAQ:
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/Header.h"
#include "TrigT1Result/Trailer.h"

// Local includes:
#include "TrigT1CTP/ResultBuilder.h"
#include "TrigT1CTP/ThresholdMap.h"
#include "TrigT1CTP/ItemMap.h"
#include "TrigT1CTP/CTPTriggerItemNode.h"
#include "TrigT1CTP/CTPTriggerThreshold.h"
#include "TrigT1CTP/EventInfo.h"
#include "TrigT1CTP/CTPUtil.h"
#include "TrigT1CTP/CTPTriggerItem.h"

// tdaq-common includes for CTP format definition
//#include "CTPfragment/CTPdataformat.h"

using namespace std;

LVL1CTP::ResultBuilder::ResultBuilder( unsigned int ctpVersionNumber, 
                                       const TrigConf::CTPConfig* ctpConfig,
                                       const ThresholdMap* decisionMap,
                                       const ItemMap* itemMap, 
                                       const InternalTriggerMap* internalTrigger,
                                       const uint32_t readoutWindow) :
   m_ctpConfig( ctpConfig ), 
   m_decisionMap( decisionMap ),
   m_itemMap( itemMap ),
   m_internalTrigger(internalTrigger),
   m_ctpVersionNumber(ctpVersionNumber),
   m_logger( "ResultBuilder" ),
   m_readoutWindow(readoutWindow),
   m_parseLine(0)
{
		
   if( m_logger.msgLvl(MSG::VERBOSE) ) {
      std::ostringstream message;
      message << "Created with CTP version " << m_ctpVersionNumber;
      m_logger.send( MSG::VERBOSE, message.str() );
   }

   m_ctpDataFormat = new CTPdataformatVersion(m_ctpVersionNumber);
		

   // build TIP word
   m_tip.resize(m_ctpDataFormat->getTIPwords());
   for (unsigned int i = 0; i < m_ctpDataFormat->getTIPwords(); ++i ) {
      m_tip[i] = constructTIPWord( i );           // TIP + 1 extra word with rndm trigger, bcid etc.
   }
   m_logger.send( MSG::VERBOSE, "TIP build" );
		
   // build trigger result before prescale
   m_tbp.resize(m_ctpDataFormat->getTBPwords());
   for (unsigned int i = 0; i < m_ctpDataFormat->getTBPwords(); ++i ) {
      m_tbp[i] = this->constructResultWord( i, TBP );   // TBP
   }
   m_logger.send( MSG::VERBOSE, "TBP build" );
		
   // build trigger result after prescale
   m_tap.resize(m_ctpDataFormat->getTAPwords());
   for (unsigned int i = 0; i < m_ctpDataFormat->getTAPwords(); ++i ) {
      m_tap[i] = this->constructResultWord( i, TAP );   // TAP
   }
   m_logger.send( MSG::VERBOSE, "TAP build" );
    
   // build trigger result after veto
   m_tav.resize(m_ctpDataFormat->getTAVwords());
   for (unsigned int i = 0; i < m_ctpDataFormat->getTAVwords(); ++i ) {
      m_tav[i] = this->constructResultWord( i, TAV );   // TAV
   }
   m_logger.send( MSG::VERBOSE, "TAV build" );
		
}
	
LVL1CTP::ResultBuilder::~ResultBuilder() {

   if (m_ctpDataFormat) delete m_ctpDataFormat;
   m_ctpDataFormat = NULL;
   m_logger.send( MSG::VERBOSE, "Deleted" );
		
}

	
bool
LVL1CTP::ResultBuilder::readOffsetConfig(const std::string& filename) {
   if (filename.empty()) return false;
		
   m_logger.send( MSG::VERBOSE, "Reading offset configuration..." );
		
   std::ifstream file(filename.c_str());
   std::string line;
   m_parseLine = &LVL1CTP::ResultBuilder::findSectionParser;
   while (std::getline(file,line)){
      const std::string::size_type lPos(line.find('#'));
      if (lPos != std::string::npos) line.resize(lPos);
      (this->*m_parseLine)(line);
   }
		
   m_logger.send( MSG::VERBOSE, "Offset configuration read" );
   return true;
}

	
bool
LVL1CTP::ResultBuilder::findSectionParser(const std::string& line) {
   std::istringstream inputStream(line);
   std::string readWord;
   if (inputStream >> readWord){
      if (readWord == "begin"){
         if (inputStream >> readWord){
            if (readWord == "StaticOffsetMap"){
               m_logger.send( MSG::VERBOSE, "Found StaticOffsetMap" );
               m_parseLine = &LVL1CTP::ResultBuilder::staticOffsetParser;
               return true;
            } // if (readWord == "StaticOffsetMap")
         } // if (inputStream >> readWord)
      }	// if (readWord == "begin")
   } // if (inputStream >> readWord)
   return false;
}

	
bool
LVL1CTP::ResultBuilder::staticOffsetParser(const std::string& line) {
   std::istringstream inputStream(line);
   std::string readWord;
   bool sc(false);
   if (inputStream >> readWord){
      if (readWord == "end"){
         m_logger.send( MSG::VERBOSE, "Found end of StaticOffsetMap" );
         m_parseLine = &LVL1CTP::ResultBuilder::findSectionParser;
         sc = true;
      } else {
         unsigned short firedItem(0);
         std::istringstream wordStream(readWord);
         if ((wordStream >> firedItem) && (firedItem < 256u)){
            std::stringstream configStream;
            configStream << firedItem << ":\t";
            unsigned short offsetItem(0);
            int offsetValue(0);
            while (inputStream >> readWord){
               wordStream.clear();
               wordStream.str(readWord);
               if ((wordStream >> offsetItem) && (offsetItem < 256u)){
                  wordStream.ignore();
                  if ((wordStream >> offsetValue) && (offsetValue != 0)){
                     configStream << offsetItem << ',' << offsetValue << '\t';
                     m_changeRules[firedItem][offsetItem] = offsetValue;
                     sc = true;
                  }	// if ((wordStream >> offsetValue) && (offsetValue != 0))
               } // if ((wordStream >> offsetItem) && (offsetItem < 256u))
            } // while (inputStream >> readWord)
            m_logger.send( MSG::VERBOSE, configStream.str().c_str() );
         } // if ((wordStream >> firedItem) && (firedItem < 256u))
      }	// if (readWord == "end") {...} else
   } // if (inputStream >> readWord)
   return sc;
}
	
const LVL1CTP::CTPSLink*
LVL1CTP::ResultBuilder::constructRoIResult() const {
		
   m_logger.send( MSG::DEBUG, "Constructing RoI result" );
		
   std::vector<uint32_t> roi_vector;
   //
   // Build the data words:
   //
		
   roi_vector.push_back( EventInfo::instance().timeNs() );         // Time stamp: 28-bit nanoseconds
   roi_vector.push_back( EventInfo::instance().time() );           // Time stamp: 32-bit UTC seconds
		
   roi_vector.insert(roi_vector.end(), m_tip.begin(), m_tip.end()); // TIP + 1 extra word
   roi_vector.insert(roi_vector.end(), m_tbp.begin(), m_tbp.end()); // TBP
   roi_vector.insert(roi_vector.end(), m_tap.begin(), m_tap.end()); // TAP
   roi_vector.insert(roi_vector.end(), m_tav.begin(), m_tav.end()); // TAV
		
   // change fired trigger items to simulated badly timed-in subcomponents (if option requested)
   changeRoIResult(roi_vector); 
		
   m_logger.send( MSG::VERBOSE, "Stored data elements of RoI result" );
		
   //
   // Build the trailer:
   //
		
   std::vector<unsigned int> trailer(ROIB::Trailer(roi_vector.size()).trailer());
   roi_vector.insert(roi_vector.end(), trailer.begin(), trailer.end());
   m_logger.send( MSG::VERBOSE, "Created trailer of RoI result" );
		
   //
   // Build the header:
   //
		
   // convention for source id in LVL1: 0 for DAQ, 1 for RoIB
   ROIB::Header helperHeader(eformat::helper::SourceIdentifier(eformat::TDAQ_CTP, 1).code());
		
   helperHeader.setRunNumber(EventInfo::instance().runNumber());
   helperHeader.setL1ID(EventInfo::instance().extendedL1ID());
   helperHeader.setBCID(EventInfo::instance().bcid());
   helperHeader.setTriggerType(triggerType()); 
   helperHeader.setEventType(EventInfo::instance().eventType());
		
   std::vector<unsigned int> header(helperHeader.header());
   roi_vector.insert(roi_vector.begin(), header.begin(), header.end());
   m_logger.send( MSG::VERBOSE, "Created header of RoI result" );
		
   //
   // Build the SLink:
   //
		
   CTPSLink* result = new CTPSLink( roi_vector, m_ctpVersionNumber );
   m_logger.send( MSG::DEBUG, "Created CTPSlink object" );
   m_logger.send( MSG::VERBOSE, "Dump CTPSlink object:\n" + result->dump() );
		
   //
   // Debug output for ANT
   // 
   if( m_logger.msgLvl(MSG::DEBUG) ) {
      {
         std::vector<std::string> passedItems = firedItems(m_tbp);
         std::ostringstream message;
         for (size_t i(0); i < passedItems.size(); ++i) {
            message << " " << passedItems[i];
         }
         m_logger.send( MSG::DEBUG, "REGTEST - Items fired before prescale: " + message.str() );
      }
      {
         std::vector<std::string> passedItems = firedItems(m_tap);
         std::ostringstream message;
         for (size_t i(0); i < passedItems.size(); ++i) {
            message << " " << passedItems[i];
         }
         m_logger.send( MSG::DEBUG, "REGTEST - Items fired after prescale: " + message.str() );
      }
		
      {
         std::vector<std::string> passedItems = firedItems(m_tav);
         std::ostringstream message;
         for (size_t i(0); i < passedItems.size(); ++i) {
            message << " " << passedItems[i];
         }
         m_logger.send( MSG::DEBUG, "REGTEST - Items fired after veto: " + message.str() );
      }
   }	
   return result;
}

	
const CTP_RDO*
LVL1CTP::ResultBuilder::constructRDOResult() const {
		
   m_logger.send( MSG::DEBUG, "Constructing RDO result" );
		
   CTP_RDO* result = new CTP_RDO(m_ctpVersionNumber);
		
   result->setTimeSec(EventInfo::instance().time());               // Time stamp: 32-bit UTC seconds
   result->setTimeNanoSec(EventInfo::instance().timeNs());         // Time stamp: 28-bit nanoseconds
		
   for (unsigned int i(0); i < m_ctpDataFormat->getTIPwords(); ++i) {
      result->setTIPWord(i, m_tip[i]);
   }
		
   for (unsigned int i(0); i < m_tbp.size(); ++i) {
      result->setTBPWord(i, m_tbp[i]);
   }
   for (unsigned int i(0); i < m_ctpDataFormat->getTAPwords(); ++i) {
      result->setTAPWord(i, m_tap[i]);
   }
   for (unsigned int i(0); i < m_ctpDataFormat->getTAPwords(); ++i) {
      result->setTAVWord(i, m_tav[i]);
   }
		
   // change fired trigger items to simulated badly timed-in subcomponents 
   // and add additional BCs (if option requested)
   changeRDOResult(result);
		
   m_logger.send( MSG::DEBUG, "Created CTP_RDO object" );
		
   return result;
		
}


void
LVL1CTP::ResultBuilder::changeTriggerWords(std::vector<uint32_t>& triggerWords) const {
   // check if anything needs to be done at all
   if ((m_changeRules.size() == 0) || (triggerWords.size() != m_ctpDataFormat->getTAVwords())) return; 
		
   // loop over the rules defined in the config file
   for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it){
			
      // 32-bit word that holds the current trigger bit
      const unsigned short iWord(it->first/32u); 
      // bit within the current word that fired
      const unsigned short iPos(it->first%32u);  
      // 32-bit trigger mask that has only the current bit 'on'
      const unsigned short firedMask(1u << iPos);
			
      // check if the current bit has actually fired in the event
      if (triggerWords[iWord] & firedMask){                
				
         // this map now contains the items that are supposed to be shifted by the configured offset
         const OffsetMap& triggerOffsets = it->second;
         // loop over the items that shall be shifted
         for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt){
            // offset map should never be filled when offset is 0
            if (jt->second == 0) continue; 
					
            // same as above, determine the word...
            unsigned short jWord(jt->first/32u);
            // the position ...
            const unsigned short jPos(jt->first%32u);
            // and the bitmask of the item
            const unsigned short bitMask(1u << jPos);
					
            // and now eliminate this item in the trigger pattern
            triggerWords[jWord] &= ~bitMask;
         } // for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt)
				
      }	// if(tav[iWord] & firedMask)
			
   } // for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it)
		
   return;
}


void
LVL1CTP::ResultBuilder::changeRoIResult(std::vector<uint32_t>& roi_vector) const {
   // check if anything needs to be done at all
   if ((m_changeRules.size() == 0) ||
       (roi_vector.size() != m_ctpDataFormat->getNumberTimeWords() + m_ctpDataFormat->getDAQwordsPerBunch())) return; 
		
   std::vector<uint32_t> tav(roi_vector.end()-m_ctpDataFormat->getTAVwords(), roi_vector.end());
		
   // loop over the rules defined in the config file
   for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it){
			
      // 32-bit word that holds the current trigger bit
      const unsigned short iWord(it->first/32u); 
      // bit within the current word that fired
      const unsigned short iPos(it->first%32u);  
      // 32-bit trigger mask that has only the current bit 'on'
      const unsigned short firedMask(1u << iPos);
			
      // check if the current bit has actually fired in the event
      if(tav[iWord] & firedMask){
				
         // this map now contains the items that are supposed to be shifted by the configured offset
         const OffsetMap& triggerOffsets = it->second;
         // loop over the items that shall be shifted
         for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt){
            // offset map should never be filled when offset is 0
            if (jt->second == 0) continue; 
            // same as above, determine the word...
            unsigned short jWord(jt->first/32u);
            // the position ...
            const unsigned short jPos(jt->first%32u);
            // and the bitmask of the item
            const unsigned short bitMask(1u << jPos);
            // and now eliminate this item in the trigger pattern
            tav[jWord] &= ~bitMask;
            // account for the number of time words
            jWord += 2u;		
            // TODO: manipulate TIP
            jWord += m_ctpDataFormat->getTIPwords();
            // manipulate TBP, eliminate this item
            roi_vector[jWord] &= ~bitMask; 
            jWord += m_ctpDataFormat->getTBPwords();
            // manipulate TAP, eliminate this item
            roi_vector[jWord] &= ~bitMask;
            jWord += m_ctpDataFormat->getTAPwords();
            // manipulate TAV, eliminate this item
            roi_vector[jWord] &= ~bitMask;
         } // for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt)
      }	// if(tav[iWord] & firedMask)
   } // for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it)
		
   return;
}


void
LVL1CTP::ResultBuilder::changeRDOResult(CTP_RDO*& result) const {
   // check if anything needs to be done at all
   if ((m_changeRules.size() == 0)||(result == 0)) return;
		
   std::vector<uint32_t> tav = result->getTAVWords();
   std::vector<uint32_t> tap = result->getTAPWords();
   std::vector<uint32_t> tbp = result->getTBPWords();
   std::vector<uint32_t> tip = result->getTIPWords();
		
   // create a new CTP_RDO object that has the requested size
   CTP_RDO* new_result = new CTP_RDO(m_ctpVersionNumber,m_readoutWindow);
		
   new_result->setTimeSec(result->getTimeSec());
   new_result->setTimeNanoSec(result->getTimeNanoSec());
		
   delete result;
   result = new_result;
		
   typedef std::map<unsigned short, uint32_t> WordMap;
   typedef std::map<uint32_t, WordMap> ResultMap;
   ResultMap tbp_result;
   ResultMap tap_result;
		
   // loop over the rules defined in the config file
   for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it){
      // 32-bit word that holds the current trigger bit
      const unsigned short iWord(it->first/32u);
      // bit within the current word that fired
      const unsigned short iPos(it->first%32u);
      // 32-bit trigger mask that has only the current bit 'on'
      const unsigned short firedMask(1u << iPos);
      // check if the current bit has actually fired in the event
      if(tav[iWord] & firedMask){
         // this map now contains the items that are supposed to be shifted by the configured offset
         const OffsetMap& triggerOffsets = it->second;
         // loop over the items that shall be shifted
         for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt){
            // offset map should never be filled when offset is 0
            if (jt->second == 0) continue; 
            // same as above, determine the word...
            const unsigned short jWord(jt->first/32u);
            // the position ...
            const unsigned short jPos(jt->first%32u);
            // and the bitmask of the item
            const unsigned short bitMask(1u << jPos);
            // TODO: manipulate TIP
					
            // only continue if the item appears in the tbp
            if (tbp[jWord] & bitMask) {
               // eliminate it in the tbp for the current BC
               tbp[jWord] &= ~bitMask;
               // get the offset that will be added
               const int offset(jt->second + (m_readoutWindow-1u)/2u);
               // check if the offset is in the range of the readoutwindow
               const bool inRange((0 < offset)&&(static_cast<uint32_t>(offset) < m_readoutWindow));
               if (inRange){
                  // switch on the corresponding bit, shifted by 'offset'
                  tbp_result[offset][jWord] |= bitMask;
               }
               // only continue if the item appears in the tap
               if (tap[jWord] & bitMask){
                  // eliminate it in the tap for the current BC
                  tap[jWord] &= ~bitMask;
                  if (inRange){
                     // switch on the corresponding bit, shifted by 'offset'
                     tap_result[offset][jWord] |= bitMask;
                  }
                  // 	      if (tav[jWord] & bitMask){
							
                  // and now eliminate the bit from the tav
                  tav[jWord] &= ~bitMask;
                  // 	      }
               } // if (tap[jWord] & bitMask)
            } // if (tbp[jWord] & bitMask)
         } // for (OffsetMap::const_iterator jt(triggerOffsets.begin()); jt != triggerOffsets.end(); ++jt)
      }	// if(tav[iWord] & firedMask)
   } // for (ChangeRulesMap::const_iterator it(m_changeRules.begin()); it != m_changeRules.end(); ++it)
		
   // and now copy the trigger bits to the actual result
	  
   // for the tbp
   for (ResultMap::const_iterator it(tbp_result.begin()); it != tbp_result.end(); ++it){
      result->selectBunch(it->first);
      const WordMap& dataWords(it->second);
      for (WordMap::const_iterator jt(dataWords.begin()); jt != dataWords.end(); ++jt){
         result->setTBPWord(jt->first, jt->second);
      }
   }
   // for the tap
   for (ResultMap::const_iterator it(tap_result.begin()); it != tap_result.end(); ++it){
      result->selectBunch(it->first);
      const WordMap& dataWords(it->second);
      for (WordMap::const_iterator jt(dataWords.begin()); jt != dataWords.end(); ++jt){
         result->setTAPWord(jt->first, jt->second);
      }
   }
		
   // the L1A is assumed to be in the middle of the readout window
   result->selectBunch((m_readoutWindow-1u)/2u);
		
   // for the BC of the L1A, we also set the tip and the tav
   unsigned short i(0u);
   for (std::vector<uint32_t>::const_iterator it(tip.begin()); it != tip.end(); ++it, ++i){
      result->setTIPWord(i, *it);
   }
   i=0u;
   for (std::vector<uint32_t>::const_iterator it(tbp.begin()); it != tbp.end(); ++it, ++i){
      result->setTBPWord(i, *it);
   }
   i=0u;
   for (std::vector<uint32_t>::const_iterator it(tap.begin()); it != tap.end(); ++it, ++i){
      result->setTAPWord(i, *it);
   }
   i=0u;
   for (std::vector<uint32_t>::const_iterator it(tav.begin()); it != tav.end(); ++it, ++i){
      result->setTAVWord(i, *it);
   }
    
   return;
}
	
	
uint32_t
LVL1CTP::ResultBuilder::extendedLevel1ID() const {
   //if MC extendedLVL1ID will be 0, otherwise will !=0
   uint32_t id = EventInfo::instance().extendedL1ID()?EventInfo::instance().extendedL1ID():EventInfo::instance().eventNumber();

   if( m_logger.msgLvl(MSG::DEBUG) ) {
      std::ostringstream message;
      message << "Created extendedLevel1ID: " << id;
      m_logger.send( MSG::DEBUG, "REGTEST - " + message.str() );
   }
   return id;
}
	
	
/**
 * Constructs the TIP words, which store the information about the threshold multiplicities
 * sent to the CTP in the event.
 */
uint32_t
LVL1CTP::ResultBuilder::constructTIPWord( unsigned int wrd_num ) const {
   
   if( wrd_num < m_ctpDataFormat->getTIPwords()-1 ) { //TIP words are input bits + 1 additional word => hence -1
			
      uint32_t result = 0;
			
      for( const TrigConf::TriggerThreshold* threshold : m_ctpConfig->menu().thresholdVector() ) {

         const CTPTriggerThreshold* ctpTT = m_decisionMap->decision( threshold );
         
         if ( ( ( ctpTT->startBit() >= 32*wrd_num ) && ( ctpTT->startBit() < 32*(wrd_num+1) ) ) ||
              ( ( ctpTT->endBit()   >= 32*wrd_num ) && ( ctpTT->endBit()   < 32*(wrd_num+1) ) )
              ) {
            
            result |= CTPUtil::alignBits( ctpTT->value(),
                                          ctpTT->startBit() - 32*wrd_num,
                                          ctpTT->endBit() - 32*wrd_num
                                          );
         }
				
      }

      if( m_logger.msgLvl(MSG::DEBUG) ) {
         std::ostringstream message;
         message << "TIP word #" << std::dec << wrd_num << " is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << result;
         m_logger.send( MSG::DEBUG, "REGTEST - " + message.str() );
      }
      return result;
			
   } else if( wrd_num == m_ctpDataFormat->getAuxTIPwordPos()-1 ) { // This is a special word. (And I don't calculate it at the moment...)
			
      uint32_t result = 0;
			
      for (InternalTriggerMap::const_iterator iter = m_internalTrigger->begin(); iter != m_internalTrigger->end(); 
           ++iter) {
         result |= CTPUtil::alignBits( iter->second->evaluate(), iter->second->pit() - 32*wrd_num, iter->second->pit() - 32*wrd_num);
      }
		
      if( m_logger.msgLvl(MSG::DEBUG) ) {
         std::ostringstream message;
         message << "TIP word #" << std::dec << wrd_num << " is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << result;
         m_logger.send( MSG::DEBUG, "REGTEST - " + message.str() );
      }			
      return result;
			
   } else {
      std::ostringstream message;
      message << "ResultBuilder::constructTIPWord() called with wrd_num = " << wrd_num;
      m_logger.send( MSG::FATAL, message.str() );
      assert( 0 );
   }
		
   return 0;
		
}


/**
 * Constructs the Trigger Before Prescale words.
 */
uint32_t
LVL1CTP::ResultBuilder::constructResultWord(unsigned int wrd_num, WrdType type ) const {
   

   if( wrd_num >= m_ctpDataFormat->getTBPwords() ) {
			
      std::ostringstream message;
      message << "ResultBuilder::constructResultWord() called with wrd_num = " << wrd_num;
      m_logger.send( MSG::FATAL, message.str() );
      assert( 0 );
			
   }


   uint32_t result = 0;
			
   for( auto item : m_ctpConfig->menu().itemVector() ) {

      if( ( m_itemMap->getItem( item )->itemPos() >= 32 * wrd_num ) &&
          ( m_itemMap->getItem( item )->itemPos() < 32 * ( wrd_num + 1 ) ) ) {
					
            bool decision =  CTPTriggerItemNode::evaluate( item->topNode(), m_decisionMap, m_internalTrigger );

            std::string sDecision = decision ? "ACTIVE" : "INACTIVE";
					
            if( type == TBP ) {
						
               result |= CTPUtil::alignBits( ( decision ? 1 : 0 ),
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num,
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num );
						
            } else if( type == TAP ) {
						
               //const bool pass_prescale = m_itemMap->getItem( *item )->prescaleCounter() == m_itemMap->getItem( *item )->prescale();
               //for float prescales
               int32_t cut = TrigConf::PrescaleSet::getCutFromPrescale( m_itemMap->getItem( item )->prescale() );
               const bool pass_prescale = (m_itemMap->getItem( item )->prescaleCounter() >= cut) && cut > 0; // no pass if PS set to "-1"
            
               result |= CTPUtil::alignBits( ( decision & pass_prescale ? 1 : 0 ),
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num,
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num );
						
               if (decision == true && pass_prescale == false) sDecision = "INACTIVE(PRESCALED)";
						
            } else if( type == TAV ) {
						
               //for float prescales
               int32_t cut = TrigConf::PrescaleSet::getCutFromPrescale( m_itemMap->getItem( item )->prescale() );
               const bool pass_prescale = (m_itemMap->getItem( item )->prescaleCounter() >= cut)  && cut > 0; // no pass if PS set to "-1"
            
               const bool pass_veto = true; // xxx apply dead time
						
               result |= CTPUtil::alignBits( ( decision & pass_prescale & pass_veto ? 1 : 0 ),
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num,
                                             m_itemMap->getItem( item )->itemPos() - 32 * wrd_num );
						
               if (decision == true && pass_prescale == true && pass_veto == false) sDecision = "INACTIVE(VETOED)";
						
            }
            
            if( m_logger.msgLvl(MSG::VERBOSE) ) {
               std::ostringstream message;
               message << "  --> Trigger item " << std::setw( 12 ) << item->name() << " is " << sDecision;
               m_logger.send( MSG::VERBOSE, message.str() );
            }
					
         }
				
   }
			
   if( m_logger.msgLvl(MSG::DEBUG) ) {
      std::ostringstream message;
      if( type == TBP ) message << "TBP";
      else if( type == TAP ) message << "TAP";
      else if( type == TAV ) message << "TAV";
      message << " word #" << std::dec << wrd_num << " is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << result;
      m_logger.send( MSG::DEBUG, "REGTEST - " + message.str() );
   }
   return result;
		
}

	
uint32_t
LVL1CTP::ResultBuilder::constructTriggerType(const std::vector<uint32_t>& triggerWords) const {
   uint32_t tt = 0;
		
   // change fired trigger items to simulated badly timed-in subcomponents (if option requested)
   std::vector<uint32_t> updatedTriggerWords(triggerWords);
   changeTriggerWords(updatedTriggerWords);
		
   // convert trigger result into bitset 
   std::bitset<512> items = CTPUtil::convertToLargeBitset(updatedTriggerWords);
		
   // loop over all items and or the trigger type for all fired items
   for( TrigConf::ItemContainer::const_iterator item = m_ctpConfig->menu().itemVector().begin();
        item != m_ctpConfig->menu().itemVector().end(); ++item ) {
      if (items[(*item)->ctpId()]) tt |= (*item)->triggerType();
   }
		
   // print output
   if( m_logger.msgLvl(MSG::VERBOSE) ) {
      for (unsigned int i(0); i < 8; ++i) {
         std::ostringstream message;
         message << "TriggerType bit" << i << " is: " << ( (tt & (0x1 << i)) ? "ACTIVE" : "INACTIVE" );
         m_logger.send( MSG::VERBOSE, message.str() );
      }
   }
  
   if( m_logger.msgLvl(MSG::DEBUG) ) {
      std::ostringstream message;
      message << "TriggerType word is: 0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << tt;
      m_logger.send( MSG::DEBUG, "REGTEST - " + message.str() );
   }    
   return tt;
}

	
std::vector<std::string>
LVL1CTP::ResultBuilder::firedItems(const std::vector<uint32_t>& triggerWords) const {
   const std::bitset<512> items = CTPUtil::convertToLargeBitset(triggerWords);      
   std::vector<std::string> passedItems;
    
   for( TrigConf::ItemContainer::const_iterator item = m_ctpConfig->menu().itemVector().begin();
        item != m_ctpConfig->menu().itemVector().end(); ++item ) {
      if (items[(*item)->ctpId()]) passedItems.push_back((*item)->name());
   }
		
   std::sort(passedItems.begin(), passedItems.end());
		
   return passedItems;
}
	
