/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/TrigConfCoolWriter.h"
#include "TrigConfStorage/TrigConfCoolFolderSpec.h"
#include "TrigConfStorage/TrigConfCoolHLTPayloadConverters.h"
#include "TrigConfStorage/TrigConfCoolL1PayloadConverters.h"
#include "TrigConfStorage/MCKLoader.h"

#include "TrigConfL1Data/PrescaleSet.h"
#include "TrigConfL1Data/BunchGroupSet.h"
#include "TrigConfL1Data/CTPConfig.h"
#include "TrigConfL1Data/Menu.h"
#include "TrigConfL1Data/ThresholdMonitor.h"
#include "TrigConfL1Data/PIT.h"
#include "TrigConfL1Data/TriggerItem.h"
#include "TrigConfL1Data/HelperFunctions.h"
#include "TrigConfHLTData/HLTFrame.h"
#include "TrigConfHLTData/HLTChain.h"
#include "TrigConfHLTData/HLTChainList.h"
#include "TrigConfHLTData/HLTPrescaleSet.h"
#include "TrigConfHLTData/HLTUtils.h"

#include "CoolApplication/Application.h"
#include "CoolKernel/ValidityKey.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoralKernel/Context.h"
#include "RelationalAccess/ConnectionService.h"
#include "RelationalAccess/IConnectionService.h"
#include "RelationalAccess/IConnectionServiceConfiguration.h"
#include "RelationalAccess/ILookupService.h"
#include "RelationalAccess/IAuthenticationService.h"
#include "RelationalAccess/IAuthenticationCredentials.h"
#include "RelationalAccess/IDatabaseServiceDescription.h"
#include "RelationalAccess/IDatabaseServiceSet.h"

#include "boost/lexical_cast.hpp"

#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <set>
#include <iterator>


using boost::lexical_cast;
using namespace std;
using namespace cool;
using namespace TrigConf;
using namespace TrigConfCoolL1PayloadConverters;
using namespace TrigConfCoolHLTPayloadConverters;


#define READ_ONLY true
#define READ_WRITE false



AutoDBOpen::AutoDBOpen(TrigConfCoolWriter *writer, bool readOnly) : 
   m_writer(writer),
   m_neededToOpen(false),
   m_ro(readOnly)
{
   if(m_writer->dbIsOpen()) {
      m_neededToOpen = false;
   } else {
      m_writer->openDb(m_ro);
      m_neededToOpen = true;
   }
}

AutoDBOpen::~AutoDBOpen() {
   if(m_neededToOpen)
      m_writer->closeDb();
}


cool::IDatabaseSvc&  
TrigConfCoolWriter::databaseService()
{
   static cool::Application app;
   return app.databaseService();
}


bool
TrigConf::TrigConfCoolWriter::checkDbConnection( bool readOnly ) {
   if(dbIsOpen()) return true;
   openDb( readOnly );
   closeDb();
   return true;
}


bool
TrigConf::TrigConfCoolWriter::dbIsOpen() { 
   return m_dbPtr.use_count()>0 && m_dbPtr->isOpen();
}


// ------------------------------------------------------------
// openDb(bool readOnly)
// ------------------------------------------------------------
cool::IDatabasePtr
TrigConf::TrigConfCoolWriter::openDb( bool readOnly ) {
   if(dbIsOpen()) return m_dbPtr;
   cool::IDatabaseSvc& dbSvc = databaseService();
   try {
      m_ostream << "Opening database '" << m_dbId << "' in " << (readOnly?"read-only":"read-write") << " mode" << endl;
      m_dbPtr = dbSvc.openDatabase( m_dbId, readOnly );   // If the database does not exist, an exception is thrown
      //     int schemaVersion = TrigConfCoolFolderSpec::readSchemaVersion(m_dbPtr);
      //     m_ostream << "Found schema with version " << schemaVersion << endl;
      return m_dbPtr;
   } catch(cool::DatabaseDoesNotExist& e) {
      if(readOnly) {
         m_ostream << "Database '" << m_dbId << "' does not exist. Can't create it since connection is read-only." << endl;
         throw;
      } else {
         m_ostream << "Database '" << m_dbId << "' does not exist, will create it" << endl;
         m_dbPtr = dbSvc.createDatabase( m_dbId );
         m_ostream << "Created DB" << endl;
         return m_dbPtr;
      }
   } catch(Exception& e) {
      m_ostream << "*** COOL exception caught: " << e.what() << endl;
      m_ostream << "Couldn't open database " << m_dbId << endl;
      throw;
   }
}


// ------------------------------------------------------------
// closeDb()
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::closeDb(cool::IDatabasePtr& db) {
   try {
      if(db.use_count()>0 && db->isOpen()) {
         m_ostream << "Closing database (old way) '" << m_dbId << "'" << endl;
         db->closeDatabase( );
      }
   } catch(std::exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << endl 
                << "Couldn't close conditions database: " << m_dbId << endl;
   }
   return;
}


// ------------------------------------------------------------
// closeDb()
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::closeDb() {
   try {
      if(dbIsOpen()) {
         m_ostream << "Closing database '" << m_dbId << "'" << endl;
         m_dbPtr->closeDatabase();
      }
   } catch(std::exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << endl 
                << "Couldn't close conditions database: " << m_dbId << endl;
   }
   TrigConfCoolFolderSpec::resetSchemaVersion();
   return;
}


// ------------------------------------------------------------
// createSchema()
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::createSchema(int schemaVersion) {
   bool needToOpen = !dbIsOpen();
   if(needToOpen) {
      try {
         if(needToOpen) openDb(false);
      }
      catch(cool::DatabaseDoesNotExist& e) {
         m_ostream << "*** : " << e.what() << endl;
         m_ostream << "Will create a new database: " << m_dbId << endl;
         createDb();
         openDb(false);
      }
      catch(cool::Exception& e) {
         m_ostream << "*** COOL  exception caught: " << e.what() << endl;
         m_ostream << "Coudn't create a new schema: " << m_dbId << endl;
         throw;
      }
   }

   try {
      if(schemaVersion==0) schemaVersion=TrigConfCoolFolderSpec::getDefaultSchemaVersion();
      m_ostream << "Creating schema '" << m_dbId << "' with version " << schemaVersion
                << (schemaVersion==TrigConfCoolFolderSpec::getDefaultSchemaVersion()?" (=default)":"") << endl;
      TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr, schemaVersion);
   }
   catch(cool::Exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << endl;
      m_ostream << "Coudn't create a new schema: " << m_dbId << endl;
      throw;
   }
   if(needToOpen) closeDb();
   return;
}


// ------------------------------------------------------------
// printSchema(ostream &)
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::printSchema(std::ostream & o) {
   AutoDBOpen db(this, READ_ONLY);
   TrigConfCoolFolderSpec::printFolderStructure(m_dbPtr, o);
}


// ------------------------------------------------------------
// printSchemaVersion(ostream &)
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::printSchemaVersion(std::ostream & o) {
   AutoDBOpen db(this, READ_ONLY);
   int schemaVersion = TrigConfCoolFolderSpec::readSchemaVersion(m_dbPtr);
   o << "Schema version is " << schemaVersion
     << (schemaVersion==TrigConfCoolFolderSpec::getDefaultSchemaVersion()?" (=default)":"") << endl;
}


// ------------------------------------------------------------
// recreateSchema()
// ------------------------------------------------------------
void TrigConf::TrigConfCoolWriter::recreateSchema(int ) {}


// ------------------------------------------------------------
// dropDb()
// ------------------------------------------------------------
void TrigConf::TrigConfCoolWriter::dropDb() {}


// ------------------------------------------------------------
// createDb()
// ------------------------------------------------------------
cool::IDatabasePtr 
TrigConf::TrigConfCoolWriter::createDb() {
   m_ostream << "Creating database '" << m_dbId << "'" << endl;
   try {
      cool::IDatabaseSvc& dbSvc = databaseService();
      m_dbPtr = dbSvc.createDatabase( m_dbId );
   }
   catch(std::exception& e) {
      m_ostream << "*** COOL  exception caught: " << e.what() << endl;
      m_ostream << "Coudn't create a new conditions database: " << m_dbId << endl;
      throw;
   }

   return m_dbPtr;
}


void
TrigConfCoolWriter::rangeInfo(const std::string& content,
                              cool::ValidityKey since,
                              cool::ValidityKey until) {


   if(since==ValidityKeyMin && until==ValidityKeyMax) {
      m_ostream << "Writing " << content << " for infinite range [0,oo]" << endl;
      return;
   }

   unsigned long long run[2];
   unsigned long long lb[2];
   lb[0] = since & 0xFFFFFFFF;
   lb[1] = until & 0xFFFFFFFF;
   run[0] = since>>32;
   run[0] &= 0x7FFFFFFF;
   run[1] = until>>32;
   run[1] &= 0x7FFFFFFF;

   if(lb[0]==0 && lb[1]==0) {
      unsigned int first = run[0];
      unsigned int last  = run[1]-1;
      if(first==last) {
         m_ostream << "IOV Info: writing " << content << " for run " << first << endl;
      } else {
         m_ostream << "IOV Info: writing " << content << " for run range [" << first << "," << last << "]" << endl;
      }
   } else {
      if(run[0]==run[1]) {
         if(lb[0]==lb[1]-1)
            m_ostream << "IOV Info: writing " << content << " for run " << run[0] << " and LB " << lb[0] << endl;
         else
            m_ostream << "IOV Info: writing " << content << " for run " << run[0] << " and LB range [" << lb[0] << "," << lb[1]-1 << "]" << endl;
      } else {
         m_ostream << "IOV Info: writing " << content << " for range [" << run[0] << "/" << lb[0] << "," << run[1] << "/" << lb[1] << "[" << endl;
      }
   }
}



void
TrigConfCoolWriter::writeL1Payload( ValidityRange vr, const CTPConfig& ctpConfig)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case
   try {
      writeL1MenuPayload         (vr, ctpConfig.menu());
      writeL1BunchGroupRunPayload(vr, ctpConfig.menu(), ctpConfig.bunchGroupSet() ); // writes bunchgroup description folder
      writeL1BunchGroupLBPayload (vr, ctpConfig.bunchGroupSet().id(), ctpConfig.bunchGroupSet()); // writes bunchgroup content and key folders
      writeL1PrescalePayload(vr.since(), vr.until(), ctpConfig.prescaleSet().id(), ctpConfig.prescaleSet());
   }
   catch(exception & e) {
      m_ostream << "<writeL1Payload> caught and re-throw exception: " << e.what() << endl;
      throw;
   }
}



void
TrigConf::TrigConfCoolWriter::writeHLTPayload( ValidityRange vr,
                                               const HLTFrame& hltFrame,
                                               const string& configSource)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case

   // writing configuration keys
   if( shouldFolderBeUpdated("/TRIGGER/HLT/HltConfigKeys") ) {
      rangeInfo("HLT configuration keys", vr.since(), vr.until());
      try {
         IFolderPtr confkeyFolder = TrigConfCoolFolderSpec::getHltConfKeyFolder(m_dbPtr);
         Record     payload = createHltConfigKeysPayload(confkeyFolder, hltFrame.smk(), 0, configSource);
         confkeyFolder->storeObject(vr.since(), vr.until(), payload, 0);
      }
      catch(exception & e) {
         m_ostream << "<writeHLTPayload> caught and re-throw exception: " << e.what() << endl
                   << "WARNING: Failed to write configuration keys to COOL" << endl;
         throw;
      }
   }

   if(hltFrame.getPrescaleSet() != 0)
      writeHltPrescalePayload(vr.since(), vr.until(),  *(hltFrame.getPrescaleSet()));//*fHLTPrescaleSet);
   
   if( shouldFolderBeUpdated("/TRIGGER/HLT/Menu") ||
       shouldFolderBeUpdated("/TRIGGER/HLT/Groups") ) {
      if( hltFrame.getHLTChainList().size() > 0 ) {
         
         rangeInfo("HLT menu", vr.since(), vr.until());

         // this call is needed to set the level of each sequence
         TrigConf::HLTUtils::allTEsProduced( hltFrame );

         // writing the chains of the HLT menu
         try {
						
            ChannelId menuChannel(0);
            IFolderPtr hltMenuFolder = TrigConfCoolFolderSpec::getHltMenuFolder(m_dbPtr);
            IFolderPtr hltGroupFolder = TrigConfCoolFolderSpec::getHltChainGroupFolder(m_dbPtr);
            // use buffer to speed up COOL writing
            hltMenuFolder->setupStorageBuffer(); 
            hltGroupFolder->setupStorageBuffer(); 

            for ( const HLTChain* chain : hltFrame.getHLTChainList() ) {
               menuChannel++;
               string concise = HLTUtils::ChainCondenseDisplay( *chain, hltFrame);
               Record payload = createHltMenuPayload( hltMenuFolder, *chain, concise );
               hltMenuFolder->storeObject(vr.since(), vr.until(), payload, menuChannel);

               Record payloadGroups = TrigConfCoolHLTPayloadConverters::createHltChainGroupPayload(hltGroupFolder, *chain );
               hltGroupFolder->storeObject(vr.since(), vr.until(), payloadGroups, menuChannel);
               m_ostream << "Writing (to buffer) HLT chain " << chain->chain_name()
                         << "(id " << chain->chain_counter()
			 << " level "<< chain->level()
                         << ") to COOL channel " << menuChannel
                         << " [belongs to groups " << payloadGroups["Groups"].data<cool::String4k>() << "]" << endl;
            }
            if( shouldFolderBeUpdated("/TRIGGER/HLT/Menu") ) {
               m_ostream << "Flushing HLT menu buffer to /TRIGGER/HLT/Menu" << endl; 
               try {
                  hltMenuFolder->flushStorageBuffer();
               } 
               catch ( cool::Exception & e) {
                  m_ostream << "WARNING: Flushing buffer to DB failed: " << e.what() << endl
                            << "warning will be ignored, job will continue" << endl;
               }
            }
            if( shouldFolderBeUpdated("/TRIGGER/HLT/Groups") ) {
               m_ostream << "Flushing HLT group buffer to /TRIGGER/HLT/Groups" << endl;
               try {
                  hltGroupFolder->flushStorageBuffer();
               }
               catch ( cool::Exception & e) {
                  m_ostream << "WARNING: Flushing buffer to DB failed: " << e.what() << endl
                            << "warning will be ignored, job will continue" << endl;
               }
            }
         }
         catch( cool::Exception & e) {
            m_ostream << "Caught cool::Exception: " << e.what() << endl;
            m_ostream << "Failed to write HLT menu to COOL" << endl;    
            throw;
         }
         catch(std::exception & e) {
            m_ostream << "<writeRunPayload> Caught std::exception: " << e.what() << endl;
            throw;
         }
      } else {
         m_ostream << "List of HLT Chains is empty, nothing is written to COOL" << endl;
      }
   }
}


// ------------------------------------------------------------
// writeMCKPayload()
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::writeMCKPayload(ValidityRange vr,
                                              unsigned int mck,
                                              std::string & release,
                                              std::string & info)
{
    AutoDBOpen db(this, READ_WRITE);
    TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case
    
    // writing monitoring configuration key
    if( shouldFolderBeUpdated("/TRIGGER/HLT/MenuAwareMonConfigKey") ) {
        rangeInfo("Monitoring configuration key", vr.since(), vr.until());
        try {
            IFolderPtr monconfkeyFolder = TrigConfCoolFolderSpec::getMonConfKeyFolder(m_dbPtr);
            Record     payload = createMonConfigKeyPayload(monconfkeyFolder, mck, info);
            monconfkeyFolder->storeObject(vr.since(), vr.until(), payload, 0, "MenuAwareMonConfigKey-"+release); //tag
            }
        catch(exception & e) {
            m_ostream << "<writeMCKPayload> caught and re-throw exception: " << e.what() << endl
            << "WARNING: Failed to write monitoring configuration key (MCK) to COOL" << endl;
            throw;
        }
    }

}


// ------------------------------------------------------------
// writeRunPayload()
// ------------------------------------------------------------
void
TrigConfCoolWriter::writeRunPayload( unsigned int runNumber,
                                     unsigned int masterConfigKey,
                                     unsigned int hltPrescaleKey,
                                     const TrigConf::ThresholdConfig & lvl1Thr,
                                     const TrigConf::CTPConfig & ctpConfig,
                                     const TrigConf::CaloInfo & caloInfo,
                                     const TrigConf::HLTFrame & hltFrame,
                                     const std::string & configSource)
{
   RunRangeVec runRanges;
   runRanges.push_back(std::pair<unsigned int,unsigned int>(runNumber,runNumber));
   writeRunPayload( runRanges, masterConfigKey, hltPrescaleKey, lvl1Thr, ctpConfig, caloInfo, hltFrame, configSource);
}


void
TrigConfCoolWriter::writeRunPayload( const RunRangeVec& runRanges,
                                     unsigned int /*masterConfigKey*/,
                                     unsigned int /*hltPrescaleKey*/,
                                     const TrigConf::ThresholdConfig & /*lvl1Thr*/,
                                     const TrigConf::CTPConfig & ctpConfig,
                                     const TrigConf::CaloInfo & /*caloInfo*/,
                                     const TrigConf::HLTFrame & hltFrame,
                                     const std::string & configSource)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case

   for ( RunRange rr : runRanges) {
      writeL1Payload(rr, ctpConfig);
   }

   // loop over the run ranges
   for ( RunRange rr : runRanges) {
      writeHLTPayload( ValidityRange(rr), hltFrame, configSource);
   }
}




void
TrigConf::TrigConfCoolWriter::writeL1MonPayload( unsigned int runNumber,
                                                 const TrigConf::Menu * lvl1Menu) {
   RunRangeVec runRanges;
   runRanges.push_back(std::pair<unsigned int,unsigned int>(runNumber,runNumber));
   writeL1MonPayload( runRanges, lvl1Menu);
}

void
TrigConf::TrigConfCoolWriter::writeL1MonPayload( const RunRangeVec& runRanges,
                                                 const TrigConf::Menu * lvl1Menu) 
{
   if(lvl1Menu == 0) throw std::runtime_error("L1 trigger menu ptr is NULL");

   AutoDBOpen db(this, READ_WRITE);
   // just in case
   TrigConfCoolFolderSpec::createMonFolderStructure(m_dbPtr); 

   // loop over the run ranges
   for ( RunRange rr : runRanges) {
  
      ValidityRange vr(rr);
      rangeInfo("LVL1 monitoring", vr.since(), vr.until());

      if( shouldFolderBeUpdated("/TRIGGER/LVL1/CTPInMonitoringMapping") ) {

         try {
            // maps to store exact description of mon counters; channel id is
            // internal mon counter id. Different  entries in the mon vector
            // belong to the same counter, the strings are concatenated,
            // separated by ':'
            std::map<cool::ChannelId,std::string> monNames;
            std::map<cool::ChannelId,std::string> thrNames;
            std::map<cool::ChannelId,std::string> slots;
            std::map<cool::ChannelId,std::string> cons;
            std::map<cool::ChannelId,std::string> mults;
            std::map<cool::ChannelId,std::string> starts;
            std::map<cool::ChannelId,std::string> ends;
            std::map<cool::ChannelId,std::string> actives;
            // not possible yet           std::map<cool::ChannelId,std::string> counterlogic;
            // the next 2 must be unique
            std::map<cool::ChannelId,std::string> counterType;
            std::map<cool::ChannelId,uint16_t> bgId;
            const std::vector<TrigConf::ThresholdMonitor*>& monCounter = lvl1Menu->moncountVector();
            std::vector<TrigConf::ThresholdMonitor*>::const_iterator mc = monCounter.begin();
            for(;mc != monCounter.end(); ++mc) {
               cool::ChannelId index = static_cast<cool::ChannelId>((*mc)->internalCounter());
               // the CTPMON goes into the same table, the configuration 
               // of the 160 counters is written with internal counter 
               // as MONCOUNTER + 768, i.e. is added at the end 
               if((*mc)->counterType().find("CTPMON") != std::string::npos) { 
                  index += 768; 
               } 

               //prepare strings
               std::stringstream strSlot; strSlot << (*mc)->ctpinSlot();
               std::stringstream strCon; strCon << (*mc)->ctpinConnector();
               std::stringstream strMult; strMult << (*mc)->multiplicity();
               std::stringstream strStart; strStart << (*mc)->thresholdStartBit();
               std::stringstream strEnd; strEnd << (*mc)->thresholdEndBit();
               std::string active;
               if((*mc)->thresholdActive()) active = "ENABLED";
               else active = "DISABLED";


               int flag=0;//0: not write, 1: write new entry, 2: concatenate
               if( monNames.find(index) == monNames.end() ){
                  //no entries for this counter
                  flag=1;
               }else{
                  flag=2;

                  //check if all parameters exist for the index
                  if( thrNames.find(index) == thrNames.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no thrNames in previous entries, overwrite all with new one" << endl;
                  }else if( slots.find(index) == slots.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no slots in previous entries, overwrite all with new one" << endl;
                  }else if( cons.find(index) == cons.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no cons in previous entries, overwrite all with new one" << endl;
                  }else if( mults.find(index) == mults.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no mults in previous entries, overwrite all with new one" << endl;
                  }else if( starts.find(index) == starts.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no starts in previous entries, overwrite all with new one" << endl;
                  }else if( ends.find(index) == ends.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no ends in previous entries, overwrite all with new one" << endl;
                  }else if( actives.find(index) == actives.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no actives in previous entries, overwrite all with new one" << endl;
                  }else if( counterType.find(index) == counterType.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no counterType in previous entries, overwrite all with new one" << endl;
                  }else if( bgId.find(index) == bgId.end() ){
                     flag=1;
                     m_ostream << "PROBLEM counter " << index << " : no bgId in previous entries, overwrite all with new one" << endl;
                  }else{

                     //check if previous entries for same name have same parameters
                     std::vector<std::string> thrNamesCheck = split(thrNames[index],":");
                     std::vector<std::string> slotsCheck = split(slots[index],":");
                     std::vector<std::string> consCheck = split(cons[index],":");
                     std::vector<std::string> multsCheck = split(mults[index],":");
                     std::vector<std::string> startsCheck = split(starts[index],":");
                     std::vector<std::string> endsCheck = split(ends[index],":");
                     std::vector<std::string> activesCheck = split(actives[index],":");
                     if (thrNamesCheck.size() != slotsCheck.size()||
                         thrNamesCheck.size() != consCheck.size()||
                         thrNamesCheck.size() != multsCheck.size()||
                         thrNamesCheck.size() != startsCheck.size()||
                         thrNamesCheck.size() != endsCheck.size()||
                         thrNamesCheck.size() != activesCheck.size()){
                        flag=1;
                        m_ostream << "PROBLEM counter " << index << " : numbers of entries are different, overwrite all with new one" << endl;
                     }else{
                        for (unsigned int i = 0; i < thrNamesCheck.size();i++){
                           if(thrNamesCheck[i]==(*mc)->thresholdName()&&
                              slotsCheck[i]==strSlot.str()&&
                              consCheck[i]==strCon.str()&&
                              multsCheck[i]==strMult.str()&&
                              startsCheck[i]==strStart.str()&&
                              endsCheck[i]==strEnd.str()&&
                              activesCheck[i]==active){

                              flag=0;
                              break;
                           }
                        }
                     }
                  }
               }

               if(flag==0)continue;


               if(flag==1){
                  monNames[index]=(*mc)->name();
                  thrNames[index]=(*mc)->thresholdName();
                  slots[index]=strSlot.str();
                  cons[index]=strCon.str();
                  mults[index]=strMult.str();
                  starts[index]=strStart.str();
                  ends[index]=strEnd.str();
                  actives[index]=active;
                  counterType[index]=(*mc)->counterType();
                  bgId[index]=(*mc)->bunchGroupId();
                  // not possible yet
                  //                  monNames[index]=(*mc)->counterLogic();
               }else if(flag==2){
                  if (monNames[index]!=(*mc)->name()){
                     std::stringstream str;
                     str << "PROBLEM counter " << index;
                     str << " : previous monitor name" << monNames[index];
                     str << ", current monitor name " << (*mc)->name() << endl;
                     m_ostream << str.str() << endl; 
                     // throw std::runtime_error(str.str()); 
                  }
                  if(counterType[index] != (*mc)->counterType()) {
                     std::stringstream str;
                     str << "PROBLEM counter " << index;
                     str << " : previous type " << counterType[index];
                     str << ", current type " << (*mc)->counterType() << endl;
                     m_ostream << str.str() << endl; 
                     // throw std::runtime_error(str.str()); 
                  }
                  if(bgId[index] != (*mc)->bunchGroupId()) {
                     std::stringstream str;
                     str << "PROBLEM counter " << index;
                     str << " : previous bunch-group id " << bgId[index];
                     str << ", current id " << (*mc)->bunchGroupId() << endl;
                     m_ostream << str.str() << endl; 
                     // throw std::runtime_error(str.str());
                  }
                  monNames[index]=(*mc)->name();
                  counterType[index]=(*mc)->counterType();
                  bgId[index]=(*mc)->bunchGroupId();

                  thrNames[index]+=":";thrNames[index]+=(*mc)->thresholdName();
                  slots[index]+=":";slots[index]+=strSlot.str();
                  cons[index]+=":";cons[index]+=strCon.str();
                  mults[index]+=":";mults[index]+=strMult.str();
                  starts[index]+=":";starts[index]+=strStart.str();
                  ends[index]+=":";ends[index]+=strEnd.str();
                  actives[index]+=":";actives[index]+=active;
               }else{
                  m_ostream << "PROBLEM counter " << index << " : unknown flag = " << flag << endl;
                  continue;
               } 
            }

            cool::ChannelId channel(0);
            cool::IFolderPtr monFolder = TrigConfCoolFolderSpec::getLvl1MonMapFolder(m_dbPtr);
            // use buffer to speed up COOL writing
            monFolder->setupStorageBuffer();
            for(std::map<cool::ChannelId,std::string>::iterator it = thrNames.begin();
                it != thrNames.end();++it) {
               channel = it->first;
               cool::Record payload = 
                  TrigConfCoolL1PayloadConverters::createLvl1MonMapPayload( monFolder,
                                                                            counterType[channel],
                                                                            bgId[channel],
                                                                            it->second,
                                                                            slots[channel],
                                                                            cons[channel],
                                                                            mults[channel],
                                                                            starts[channel],
                                                                            ends[channel],
                                                                            actives[channel],
                                                                            monNames[channel]);
               // add counterlogic above and below when implemented in trigger db

               m_ostream << "Write monitoring counter (channel " << channel << ") :: " 
                         <<	" monname '" << monNames[channel] << "'"
                         <<	" type '" << counterType[channel] << "'" 
                         <<	" bgid '" << bgId[channel] << "'" 
                         <<	" thnames '" << it->second << "'"
                         <<	" slots '" << slots[channel] << "'" 
                         <<	" connectors '" << cons[channel] << "'" 
                         <<	" multiplicities '" << mults[channel] << "'" 
                         <<	" start bits '" << starts[channel] << "'" 
                         <<	" end bits '" << ends[channel] << "'" 
                         <<	" thresholds active '" << actives[channel] << "'" 
                         << endl;
               monFolder->storeObject(vr.since(), vr.until(), payload, channel);
            }
            monFolder->flushStorageBuffer();
         } catch( cool::Exception & e) {
            m_ostream << "Caught cool::Exception: " << e.what() << endl;
            m_ostream << "Failed to write LVL1 monitoring mapping to COOL" << endl;    
            //throw;
         } catch(std::exception & e) {
            m_ostream << "<writeLVL1MonPayload> Caught std::exception: " << e.what() << endl;
            //throw;
         }
      }
   }
}


// ------------------------------------------------------------
// writeL1MenuPayload()
// ------------------------------------------------------------
void
TrigConf::TrigConfCoolWriter::writeL1MenuPayload( ValidityRange vr,
                                                  const Menu& lvl1Menu)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); // just in case

   // writing the Lvl1 menu
   try {
      if( shouldFolderBeUpdated("/TRIGGER/LVL1/Thresholds") &&
          lvl1Menu.thresholdConfig().thresholdVector().size()>0 ) { // if there are no thresholds we don't want to write anything (METsig, JETWeights)
         // get the folders for the menu, itemDef, and thresholds
         cool::IFolderPtr lvl1ThresholdFolder = TrigConfCoolFolderSpec::getLvl1ThresholdFolder(m_dbPtr);
         
         rangeInfo("LVL1 thresholds", vr.since(), vr.until());
         // first, parse the threshold list and write to threshold folder
         const std::vector<TrigConf::TriggerThreshold*> & lvl1Thrs = lvl1Menu.thresholdConfig().thresholdVector();
         std::vector<TrigConf::TriggerThreshold*>::const_iterator thrIt = lvl1Thrs.begin();
         // use buffer to speed up COOL writing

         lvl1ThresholdFolder->setupStorageBuffer();

         // go through the thresholds, channels assigned in the order (0..#thr-1)
         cool::ChannelId thrChannel = 0;
         for(;thrIt != lvl1Thrs.end(); thrIt++) {
            Record payloadThr = TrigConfCoolL1PayloadConverters::createLvl1ThresholdPayload( lvl1ThresholdFolder, **thrIt );
            m_ostream << "Writing (to buffer) LVL1 threshold " << (*thrIt)->name() 
                      << " (version " << (*thrIt)->version() << ") to channel "
                      << thrChannel << endl;
            lvl1ThresholdFolder->storeObject(vr.since(), vr.until(), payloadThr, thrChannel++);
         }


         // TODO
         if( lvl1Menu.thresholdConfig().l1Version() == 0 ) {
            m_ostream << "Writing (to buffer) LVL1 threshold 'JetWeights':";
            const std::vector<int>& jetweights = lvl1Menu.caloInfo().jetWeights();
            std::copy(jetweights.begin(), jetweights.end(), std::ostream_iterator<int>(m_ostream, ", "));
            m_ostream << endl;
            Record payloadJW = TrigConfCoolL1PayloadConverters::createLvl1JetWeightPayload( lvl1ThresholdFolder, lvl1Menu.caloInfo().jetWeights() );
            lvl1ThresholdFolder->storeObject(vr.since(), vr.until(), payloadJW, thrChannel++);
         }

         m_ostream << "Writing (to buffer) LVL1 threshold 'MET Significance parameters':";
         lvl1Menu.caloInfo().metSigParam().print();
         cool::Record payloadMETSigParams = TrigConfCoolL1PayloadConverters::createLvl1METSigPayload( lvl1ThresholdFolder, lvl1Menu.caloInfo().metSigParam() );
         lvl1ThresholdFolder->storeObject(vr.since(), vr.until(), payloadMETSigParams, thrChannel++);

         // write
         m_ostream << "Flushing LVL1 thresholds buffer to /TRIGGER/LVL1/Thresholds" << endl;
         try {
            lvl1ThresholdFolder->flushStorageBuffer();
         }
         catch ( cool::Exception & e) {
            m_ostream << "WARNING: Flushing buffer to DB failed: " << e.what() << endl
                      << "warning will be ignored, job will continue" << endl;
         }
      }
   } catch( cool::Exception & e) {
      m_ostream << "Caught cool::Exception: " << e.what() << endl;
      m_ostream << "Failed to write LVL1 menu to COOL" << endl;    
      throw;
   } catch(std::exception & e) {
      m_ostream << "<writeLVL1RunPayload> Caught std::exception: " << e.what() << endl;
      throw;
   }


   try {

      // second, parse the items and write to menu and itemDef folders
      if( shouldFolderBeUpdated("/TRIGGER/LVL1/Menu") ||
          shouldFolderBeUpdated("/TRIGGER/LVL1/ItemDef") ) {
         
         rangeInfo("LVL1 menu", vr.since(), vr.until());

         cool::IFolderPtr lvl1MenuFolder = TrigConfCoolFolderSpec::getLvl1MenuFolder(m_dbPtr);
         cool::IFolderPtr lvl1ItemDefFolder = TrigConfCoolFolderSpec::getLvl1ItemDefFolder(m_dbPtr);

         // use buffer to speed up COOL writing
         lvl1MenuFolder->setupStorageBuffer();
         lvl1ItemDefFolder->setupStorageBuffer();
         // go through the items
         for ( TriggerItem* item : lvl1Menu.items()) {
            cool::Record    payloadMenu = TrigConfCoolL1PayloadConverters::createLvl1MenuPayload( lvl1MenuFolder, *item );
            cool::Record    payloadItemDef  = TrigConfCoolL1PayloadConverters::createLvl1ItemDefPayload( lvl1ItemDefFolder, *item );
            cool::ChannelId menuChannel = static_cast<cool::ChannelId>(item->ctpId()); // int -> uint32
            m_ostream << "Writing (to buffer) LVL1 item " << item->name() 
                      << " (logic " << payloadItemDef["Logic"].data<cool::String255>() 
                      << ": [" << payloadItemDef["ConditionsList"].data<cool::String4k>() << "]) to channel "
                      << menuChannel << endl;
            if( shouldFolderBeUpdated("/TRIGGER/LVL1/Menu") )
               lvl1MenuFolder->storeObject(vr.since(), vr.until(), payloadMenu, menuChannel);

            if( shouldFolderBeUpdated("/TRIGGER/LVL1/ItemDef") )
               lvl1ItemDefFolder->storeObject(vr.since(), vr.until(), payloadItemDef, menuChannel);
         }
         // write
         if( shouldFolderBeUpdated("/TRIGGER/LVL1/Menu") ) {
            m_ostream << "Flushing LVL1 menu buffer to /TRIGGER/LVL1/Menu" << endl;
            try {
               lvl1MenuFolder->flushStorageBuffer();
            }
            catch ( cool::Exception & e) {
               m_ostream << "WARNING: Flushing buffer to DB failed: " << e.what() << endl
                         << "warning will be ignored, job will continue" << endl;
            }
         }
         if( shouldFolderBeUpdated("/TRIGGER/LVL1/ItemDef") ) {
            m_ostream << "Flushing LVL1 item buffer to /TRIGGER/LVL1/ItemDef" << endl;
            try {
               lvl1ItemDefFolder->flushStorageBuffer();
            }
            catch ( cool::Exception & e) {
               m_ostream << "WARNING: Flushing buffer to DB failed: " << e.what() << endl
                         << "warning will be ignored, job will continue" << endl;
            }
         }										 
      }
   } catch( cool::Exception & e) {
      m_ostream << "Caught cool::Exception: " << e.what() << endl;
      m_ostream << "Failed to write LVL1 menu to COOL" << endl;    
      throw;
   } catch(std::exception & e) {
      m_ostream << "<writeLVL1RunPayload> Caught std::exception: " << e.what() << endl;
      throw;
   }

   writeL1CTPCoreInputMapping( vr, lvl1Menu );

}



void
TrigConf::TrigConfCoolWriter::writeL1MenuPayload( const RunRangeVec& runRanges,
                                                  const TrigConf::Menu& lvl1Menu)
{
   AutoDBOpen db(this, READ_WRITE);
   for ( RunRange rr : runRanges)
      writeL1MenuPayload( rr, lvl1Menu );
}







void
TrigConf::TrigConfCoolWriter::writeL1CTPCoreInputMapping( ValidityRange vr,
                                                          const Menu& lvl1Menu)
{
   if( ! shouldFolderBeUpdated("/TRIGGER/LVL1/CTPCoreInputMapping") )
      return;

   AutoDBOpen db(this, READ_WRITE);

   // now write PIT mapping
   try {

      cool::IFolderPtr tipFolder = TrigConfCoolFolderSpec::getLvl1InputMapFolder(m_dbPtr);
      tipFolder->setupStorageBuffer();

      for ( const TIP * tip : lvl1Menu.tipVector() ) {

         cool::ChannelId tipNum = static_cast<cool::ChannelId>(tip->tipNumber());

         stringstream tipName;
         tipName << "name:" << tip->thresholdName();
         tipName << "|thresh bit:" << tip->thresholdBit();
         tipName << "|pos:" << tip->thresholdMapping();
         tipName << "|active:" << tip->thresholdActive();
         tipName << "|ctpin slot:" << tip->slot();
         tipName << "|con:" << tip->connector();
         tipName << "|cable bit:" << tip->cableBit();
         m_ostream << "Write TIP / channel " << tipNum << " : " << tipName.str() << endl;

         Record payload = TrigConfCoolL1PayloadConverters::createLvl1InputMapPayload( tipFolder, *tip );
         tipFolder->storeObject(vr.since(), vr.until(), payload, tipNum);
      }
      m_ostream << "Flushing LVL1 CTPInputMap buffer to /TRIGGER/LVL1/CTPCoreInputMapping" << endl;
      tipFolder->flushStorageBuffer();
   } catch( cool::Exception & e) {
      m_ostream << "Caught cool::Exception: " << e.what() << endl;
      m_ostream << "Failed to write LVL1 TIP mapping to COOL" << endl;
      throw;
   } catch( exception & e) {
      m_ostream << "<writeLVL1RunPayload> Caught std::exception: " << e.what() << endl;
      throw;
   }
}




// ------------------------------------------------------------
// writeLVL1BunchGroups()
// ------------------------------------------------------------

void 
TrigConf::TrigConfCoolWriter::writeL1BunchGroupRunPayload( ValidityRange vr,
                                                           const Menu& lvl1Menu,
                                                           const BunchGroupSet& bgs)
{
   if( !shouldFolderBeUpdated("/TRIGGER/LVL1/BunchGroupDescription") ) return;
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr);
   rangeInfo("LVL1 bunch group descriptions", vr.since(), vr.until());
   try {
      IFolderPtr bgDescFolder = TrigConfCoolFolderSpec::getLvl1BGDescFolder(m_dbPtr);
      Record payload = createLvl1BGDescPayload(bgDescFolder, bgs,lvl1Menu);
      m_ostream << "Store LVL1 BG description buffer to /TRIGGER/LVL1/BunchGroupDescription" << endl;
      bgDescFolder->storeObject(vr.since(), vr.until(), payload, 0);
   }
   catch(exception & e) {
      m_ostream << "<writeL1BunchGroupRunPayload> caught and re-throw exception: " << e.what() << endl;
      throw;
   }
}


void 
TrigConf::TrigConfCoolWriter::writeL1BunchGroupRunPayload( const RunRangeVec& runRanges,
                                                           const TrigConf::Menu& lvl1Menu,
                                                           const TrigConf::BunchGroupSet& bgs)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr);
   for ( RunRange rr : runRanges)
      writeL1BunchGroupRunPayload( rr, lvl1Menu, bgs);
}


void
TrigConf::TrigConfCoolWriter::writeL1BunchGroupLBPayload( const RunRangeVec& runRanges,
                                                          unsigned int bgKey,
                                                          const TrigConf::BunchGroupSet& bgs)
{
   AutoDBOpen db(this, READ_WRITE);
   for ( RunRange rr : runRanges) {
      ValidityRange vr(rr);
      writeL1BunchGroupLBPayload( rr, bgKey, bgs);
   }
}

void
TrigConf::TrigConfCoolWriter::writeL1BunchGroupLBPayload( ValidityRange vr,
                                                          unsigned int bgKey,
                                                          const BunchGroupSet& bgs)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr);
   try {
      if( shouldFolderBeUpdated("/TRIGGER/LVL1/BunchGroupKey") ) {
         rangeInfo("LVL1 bunchgroup key", vr.since(), vr.until());
         IFolderPtr lvl1BGKFolder = TrigConfCoolFolderSpec::getLvl1BGKeyFolder(m_dbPtr);
         Record payload = createLvl1BGKeyPayload(lvl1BGKFolder, bgKey);
         m_ostream << "Store LVL1 BG key to /TRIGGER/LVL1/BunchGroupKey" << endl;
         lvl1BGKFolder->storeObject(vr.since(), vr.until(), payload, 0);
      }

      if( shouldFolderBeUpdated("/TRIGGER/LVL1/BunchGroupContent") ) {
         rangeInfo("LVL1 bunchgroups", vr.since(), vr.until());
         IFolderPtr lvl1BGContentFolder = TrigConfCoolFolderSpec::getLvl1BGContentFolder(m_dbPtr);
         Record payload = createLvl1BGContentPayload(lvl1BGContentFolder, bgs);
         m_ostream << "Store LVL1 BG content to /TRIGGER/LVL1/BunchGroupContent" << endl;
         lvl1BGContentFolder->storeObject(vr.since(), vr.until(), payload, 0);
      }
   }
   catch(exception & e) {
      m_ostream << "<writeLVL1BGPayload> caugh and re-throw exception: " << e.what() << endl;
      throw;
   }
}




// ------------------------------------------------------------
// writeLbPayload()
// ------------------------------------------------------------
void
TrigConfCoolWriter::writeHltPrescalePayload( const RunRangeVec& runRanges,
                                             const HLTPrescaleSet& pss)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr);
   for ( RunRange rr : runRanges) {
      ValidityRange vr(rr);
      writeHltPrescalePayload( vr.since(), vr.until(), pss);
   }
}


void
TrigConfCoolWriter::writeHltPrescalePayload( unsigned int run, unsigned int lb,
                                             const HLTPrescaleSet& pss)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); 
   ValidityRange vr(run, lb);
   writeHltPrescalePayload( vr.since(), vr.until(), pss);
}

void
TrigConf::TrigConfCoolWriter::writeHltPrescalePayload( ValidityKey since,
                                                       ValidityKey until,
                                                       const HLTPrescaleSet& pss)
{

   AutoDBOpen db(this, READ_WRITE);
   // writing configuration keys and prescales
   try {
      if( shouldFolderBeUpdated("/TRIGGER/HLT/PrescaleKey") ) {
         cool::IFolderPtr hltpskFolder = TrigConfCoolFolderSpec::getHltPrescaleKeyFolder(m_dbPtr);
         rangeInfo("HLT prescale key", since, until);
         cool::Record payload = TrigConfCoolHLTPayloadConverters::createHltPrescaleKeyPayload(hltpskFolder, pss.id());
         m_ostream << "Store HLT prescale key " << pss.id() << " to /TRIGGER/HLT/PrescaleKey" << endl;
         hltpskFolder->storeObject(since, until, payload, 0);
      }

      if( shouldFolderBeUpdated("/TRIGGER/HLT/Prescales") ) {
         rangeInfo("HLT prescale set", since, until);
         cool::IFolderPtr hltpssFolder = TrigConfCoolFolderSpec::getHltPrescalesFolder(m_dbPtr);
         // use buffer to speed up COOL writing
         hltpssFolder->setupStorageBuffer(); 
         m_ostream << "Writing (to buffer) HLT prescales [(value/channel)]:" << endl;
         // L2
         const uint ccmax = 8192; //pss.maxChainCounter(TrigConf::L2);
         uint printcount=0;
         for(uint cc=0; cc<ccmax; cc++) {
            if(!pss.hasPrescale( cc, TrigConf::L2 )) continue;
            const HLTPrescale& sc = pss.getPrescale( cc, L2 );
            cool::Record payload = TrigConfCoolHLTPayloadConverters::createHltPrescalesPayload(hltpssFolder, sc.prescale(), sc.pass_through(), sc.getRerunPrescale("").first);
            hltpssFolder->storeObject(since, until, payload, 2*cc);
            m_ostream << "(L2:" << sc.prescale() << "/" << cc << ") ";
            if((++printcount)%10==0) m_ostream << endl;
         }
         // EF
         m_ostream << endl;
         //ccmax = pss.maxChainCounter(TrigConf::EF);
         printcount=0;
         for(uint cc=0; cc<ccmax; cc++) {
            if(!pss.hasPrescale( cc, TrigConf::EF )) continue;
            const HLTPrescale& sc = pss.getPrescale( cc, EF );
            cool::Record payload = TrigConfCoolHLTPayloadConverters::createHltPrescalesPayload(hltpssFolder, sc.prescale(), sc.pass_through(), sc.getRerunPrescale("").first);
            hltpssFolder->storeObject(since, until, payload, 2*cc+1);
            m_ostream << "(EF:" << sc.prescale() << "/" << cc << ") ";
            if((++printcount)%10==0) m_ostream << endl;
         }

	 // merged HLT 
        for(uint cc=0; cc<ccmax; cc++) {
            if(!pss.hasPrescale( cc, TrigConf::HLT )) continue;
            const HLTPrescale& sc = pss.getPrescale( cc, HLT );
            cool::Record payload = TrigConfCoolHLTPayloadConverters::createHltPrescalesPayload(hltpssFolder, sc.prescale(), sc.pass_through(), sc.getRerunPrescale("").first);
            hltpssFolder->storeObject(since, until, payload, 20000+cc); //shift block by 20 thousands
            m_ostream << "(HLT:" << sc.prescale() << "/" << 20000+cc << ") ";
            if((++printcount)%10==0) m_ostream << endl;
         }

         m_ostream << "]" << endl;
         m_ostream << "Flushing HLT prescales buffer to /TRIGGER/HLT/Prescales" << endl;
         hltpssFolder->flushStorageBuffer();
      }
   }
   catch(exception & e) {
      m_ostream << "<writeHltPrescalePayload> caught and re-throw exception: " << e.what() << endl;
      throw;
   }
}

void
TrigConfCoolWriter::readHltPrescalePayload( unsigned int run, unsigned int lb,
                                            HLTPrescaleSet& pss) {
   
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, lb);
   pss.reset();

   // read the prescale key
   {
      IFolderPtr hltpskFolder = TrigConfCoolFolderSpec::getHltPrescaleKeyFolder(m_dbPtr);
      IObjectIteratorPtr objects = hltpskFolder->browseObjects( vr.since(), vr.since()+1, 0 );
      objects->goToNext();
      const IObject& obj = objects->currentRef();
      const IRecord & payload = obj.payload();
      unsigned int hltpsk = readHltPrescaleKey( payload.attributeList() );
      pss.setId(hltpsk);
   }

   // read the prescale set
   {
      float ps, pt, rrps;
      HLTLevel level;
      cool::IFolderPtr hltpssFolder = TrigConfCoolFolderSpec::getHltPrescalesFolder(m_dbPtr);
      cool::IObjectIteratorPtr objects = hltpssFolder->browseObjects( vr.since(), vr.since()+1, cool::ChannelSelection());
      bool iovset=false;
      while(objects->goToNext()) {
         const cool::IObject& obj = objects->currentRef();
         if(!iovset) {
            pss.setIOV(obj.since(), obj.until()); // assume the IOV of the first channel is like all the others
            iovset=true;
         }
         const ChannelId& ch = obj.channelId();
         const IRecord& payload = obj.payload();
         readHltPrescale( payload.attributeList(), ps, pt, rrps);
	 unsigned int counter = ch/2;

	 if (ch >=20000 ) {
	   level=HLT;
	   counter = ch - 20000;
	 } else
	   level = (ch%2==0)?L2:EF;
         
         pss.thePrescale( counter, level )
            .setPrescale(ps)
            .setPassThrough(pt)
            .setRerunPrescale("",rrps);
      }
   }
}





void
TrigConfCoolWriter::writeL1PrescalePayload( const RunRangeVec& runRanges,
                                            unsigned int lvl1PrescaleKey,
                                            const PrescaleSet & prescale)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr);
   for ( RunRange rr : runRanges) {
      ValidityRange vr(rr);
      writeL1PrescalePayload( vr.since(), vr.until(), lvl1PrescaleKey, prescale);
   }
}


void 
TrigConfCoolWriter::writeL1PrescalePayload( unsigned int run, unsigned int lb,
                                            unsigned int lvl1PrescaleKey,
                                            const PrescaleSet& prescaleSet)
{
   AutoDBOpen db(this, READ_WRITE);
   TrigConfCoolFolderSpec::createFolderStructure(m_dbPtr); 
   ValidityRange vr(run, lb);
   writeL1PrescalePayload( vr.since(), vr.until(), lvl1PrescaleKey, prescaleSet);
}

void
TrigConf::TrigConfCoolWriter::writeL1PrescalePayload( cool::ValidityKey since,
                                                      cool::ValidityKey until,
                                                      unsigned int lvl1PrescaleKey,
                                                      const TrigConf::PrescaleSet & prescaleSet)
{
   prescaleSet.print("",5);
   AutoDBOpen db(this, READ_WRITE);
   // writing configuration keys and prescales
   try {
      IFolderPtr lvl1CkConfFolder = TrigConfCoolFolderSpec::getLvl1ConfKeyFolder(m_dbPtr);
      if( shouldFolderBeUpdated("/TRIGGER/LVL1/Lvl1ConfigKey") ) {
         rangeInfo("LVL1 prescale key", since, until);
         cool::Record payload =
            TrigConfCoolL1PayloadConverters::createLvl1ConfigKeyPayload(lvl1CkConfFolder, lvl1PrescaleKey);
         m_ostream << "Store LVL1 prescale key " << lvl1PrescaleKey << " to /TRIGGER/LVL1/Lvl1ConfigKey" << endl;
         lvl1CkConfFolder->storeObject(since, until, payload, 0);
      }

      if( shouldFolderBeUpdated("/TRIGGER/LVL1/Prescales") ) {
         rangeInfo("LVL1 prescale set", since, until);
         IFolderPtr lvl1PsConfFolder = TrigConfCoolFolderSpec::getLvl1PrescalesFolder(m_dbPtr);
         vector<int32_t> prescaleV = prescaleSet.cuts();

         vector<int32_t>::const_iterator psIt = prescaleV.begin();

         // use buffer to speed up COOL writing
         lvl1PsConfFolder->setupStorageBuffer();

         m_ostream << "Writing (to buffer) LVL1 prescales [(value/channel)]:" << endl;
         for(int channel=0; psIt != prescaleV.end(); channel++, psIt++) {
            Record payload =
               TrigConfCoolL1PayloadConverters::createLvl1PrescalesPayload(lvl1PsConfFolder, *psIt);
            lvl1PsConfFolder->storeObject(since, until, payload, channel);
            m_ostream << "(" << *psIt << "/" << channel << ") ";
            if((channel+1)%10==0) m_ostream << endl;
         }
         m_ostream << "]" << endl;
         m_ostream << "Flushing LVL1 prescales buffer to /TRIGGER/LVL1/Prescales" << endl;
         lvl1PsConfFolder->flushStorageBuffer();
      }
   }
   catch(std::exception & e) {
      m_ostream << "<writeL1PrescalePayload> caught and re-throw exception: " << e.what() << endl;
      throw;
   }

}






void
TrigConfCoolWriter::readHLTPayload( unsigned int run,
                                    HLTFrame & hltFrame)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityKey pointInTime( (ValidityKey(run)<<32) + 1);

   // read the general chain info
   IFolderPtr hltMenuFolder = TrigConfCoolFolderSpec::getHltMenuFolder(m_dbPtr);
   IObjectIteratorPtr objects = hltMenuFolder->findObjects( pointInTime, ChannelSelection() );
   while ( objects->goToNext() ) {
      const IObject& obj = objects->currentRef();
      const IRecord & payload = obj.payload();
      HLTChain* ch = createHLTChain( payload.attributeList(), &hltFrame.theHLTSequenceList() );
      hltFrame.theHLTChainList().addHLTChain(ch);
   }

   // read the chain groups (they are in a separate folder)
   IFolderPtr hltGroupFolder = TrigConfCoolFolderSpec::getHltChainGroupFolder(m_dbPtr);
   objects = hltGroupFolder->findObjects( pointInTime, ChannelSelection() );
   while ( objects->goToNext() ) {
      const IObject& obj = objects->currentRef();
      const IRecord & payload = obj.payload();
      addGroupsToHltChain( payload.attributeList(), hltFrame.theHLTChainList());
   }

   IFolderPtr hltConfigKeyFolder = TrigConfCoolFolderSpec::getHltConfKeyFolder(m_dbPtr);
   try {
      IObjectPtr object = hltConfigKeyFolder->findObject( pointInTime, (ChannelId) 0 );
      const IRecord & payload = object->payload();
      unsigned int smk, hltPrescaleKey;
      string configSource;
      readHltConfigKeys( payload.attributeList(), smk, hltPrescaleKey, configSource ); // the hltPrescaleKey here is not useable (it is superseeded by the multiversion one)
      hltFrame.setSMK(smk);
      hltFrame.setConfigSource(configSource);
   }
   catch(const cool::ObjectNotFound& o) {
      m_ostream << "<readHLTPayload> No entry found in HLT keys folder for run " << run  << endl;
   }
}

void
TrigConfCoolWriter::readRunPayload( unsigned int run,
                                    HLTFrame & hltFrame,
                                    unsigned int & masterConfigKey,
                                    unsigned int & /*hltPrescaleKey*/,
                                    string & configSource)
{
   readHLTPayload( run, hltFrame);
   masterConfigKey = hltFrame.smk();
   configSource = hltFrame.configSource();
}


void
TrigConfCoolWriter::readL1Payload( unsigned int run,
                                   CTPConfig & ctpc)
{

   /**
    *
    * Setting the ctpVersion based on the run number does not work for MC
    */

   readL1Menu( run, ctpc );
}

void 
TrigConfCoolWriter::readL1InputMapPayload( unsigned int run,
                                           vector<PIT*>& pits,
                                           vector<TIP*>& tips )
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, 1);
   IObjectIteratorPtr objects;
   IFolderPtr folder = TrigConfCoolFolderSpec::getLvl1InputMapFolder(m_dbPtr);
   vector<bool> foundTip(512,false);
   for(ChannelId channel = 0; channel < 512; ++channel) {
      objects = folder->browseObjects( vr.since(), vr.until()-1, channel );
      if(objects->size()==0) continue;
      while ( objects->goToNext() ) {
         const IObject& obj = objects->currentRef();
         const IRecord & payload = obj.payload();
         PIT* pit = readLvl1InputMap( payload.attributeList() );
         TIP* tip = readLvl1TIPMap( payload.attributeList() );
         uint16_t tipNumber = obj.channelId();
         if(tipNumber > 511) {
            m_ostream << "TIP number " << tipNumber << " out of bounds!" << endl;
            throw runtime_error("TIP number ouf ot bounds!");
         } else {
            if(foundTip[tipNumber]) {
               stringstream str;
               str << "TIP '" << tipNumber << "' had already been read!";
               throw runtime_error(str.str());
            }
            foundTip[tipNumber] = true;
            pit->setPitNumber(tipNumber);
            tip->setTipNumber(tipNumber);
            tip->setClock(tipNumber%2);
            pits.push_back(pit);
            tips.push_back(tip);
         }
      }     
   }
}

void
TrigConfCoolWriter::readL1MonMapPayload(unsigned int run,
                                        vector<ThresholdMonitor*>& mons)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, 1);

   IObjectIteratorPtr objects;
   IFolderPtr folder = TrigConfCoolFolderSpec::getLvl1MonMapFolder(m_dbPtr);
   vector<bool> foundCounter(768,false);

   for(ChannelId channel = 0; channel < 768; ++channel) {

      objects = folder->browseObjects( vr.since(), vr.until()-1, channel );
      if(objects->size()==0) continue;
      while ( objects->goToNext() ) {
         const IObject& obj = objects->currentRef();
         const IRecord & payload = obj.payload();
         vector<ThresholdMonitor*> counters = readLvl1MonMap( payload );
         int counterNumber = obj.channelId();
         if(counterNumber > 768) {
            m_ostream << "Monitoring counter number " << counterNumber << " out of bounds!" << endl;
            throw runtime_error("Monitoring counter ouf of bounds!");
         } else {
            if(foundCounter[counterNumber]) {
               stringstream str;
               str << "MonCounter '" << counterNumber << "' had already been read!";
               throw runtime_error(str.str());
            }
            foundCounter[counterNumber] = true;
            for(size_t i = 0 ; i < counters.size(); ++i) {
               counters[i]->setInternalCounter(counterNumber);
               mons.push_back( counters[i] );
            }
         }
      }     
      ++channel;
   }
}

void
TrigConfCoolWriter::readL1Items(unsigned int run,
                                vector<TriggerItem*>& items)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, 1);

   IObjectIteratorPtr objects;
   IFolderPtr folder = TrigConfCoolFolderSpec::getLvl1MenuFolder(m_dbPtr);
   vector<bool> foundItem(512,false);

   //for(ChannelId channel = 0; channel < 512; ++channel) {
   objects = folder->browseObjects( vr.since(), vr.until()-1,cool::ChannelSelection());
   //if(objects->size()==0) continue;
   while ( objects->goToNext() ) {
      const cool::IObject& obj = objects->currentRef();
      const cool::IRecord & payload = obj.payload();
      TriggerItem* item = createLvl1TriggerItem( payload.attributeList() );
      uint32_t ctpId = obj.channelId();
      if(ctpId > 511) {
         m_ostream << "Item ctpid " << ctpId << " out of bounds!" << endl;
         throw runtime_error("Item ctpid ouf ot bounds!");
      } else {
         if(foundItem[ctpId]) {
            stringstream str;
            str << "Item '" << ctpId << "' had already been read!";
            throw runtime_error(str.str());
         }
         foundItem[ctpId] = true;
         item->setCtpId(ctpId);
         items.push_back(item);
      }
   }
   //   }

}


void
TrigConfCoolWriter::readL1Thresholds(unsigned int run,
                                     vector<TriggerThreshold*>& thrs) {
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, 1);
   thrs.clear();
   IFolderPtr L1thrFolder = TrigConfCoolFolderSpec::getLvl1ThresholdFolder(m_dbPtr);
   IObjectIteratorPtr objects = L1thrFolder->browseObjects( vr.since(), vr.until()-1, cool::ChannelSelection() );
   while ( objects->goToNext() ) {
      const IObject& obj = objects->currentRef();
      const IRecord & payload = obj.payload();
      thrs.push_back( createLvl1Threshold( payload.attributeList() ) );
   }
}


void
TrigConfCoolWriter::readL1ItemDef(unsigned int run,
                                  const vector<TriggerItem*>& items,
                                  const vector<TriggerThreshold*>& thrs)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run, 1);

   IFolderPtr L1ItemDefFolder = TrigConfCoolFolderSpec::getLvl1ItemDefFolder(m_dbPtr);
   for(TriggerItem* item: items) {
      ChannelId itemChannel = item->ctpId();
      IObjectIteratorPtr objects = L1ItemDefFolder->browseObjects( vr.since(), vr.until()-1, itemChannel );
      while ( objects->goToNext() ) {
         const IObject& obj = objects->currentRef();
         const IRecord & payload = obj.payload();
         addThresholdsToTriggerItem( payload.attributeList(), item, thrs);
      }
   }
}


void
TrigConfCoolWriter::readL1Menu(unsigned int run, CTPConfig & ctpc)
{
   AutoDBOpen db(this, READ_ONLY);

   Menu & menu = ctpc.menu();

   // thresholds
   vector<TriggerThreshold*> thrs;
   readL1Thresholds(run, thrs);

   /**
    * a unpleasant hack so we can figure out if this Run1 (CTPVersion 3) or Run2 (CTPVersion 4)
    * 
    * we use the fact that in Run2 the cable names were different
    */
   bool isRun2 = false;
   for(TriggerThreshold* thr : thrs) {
      if( thr->cableName()=="EM1" || thr->cableName()=="EM2" || 
          thr->cableName()=="JET1" || thr->cableName()=="JET2" || 
          thr->cableName()=="TOPO1" || thr->cableName()=="TOPO2" || 
          thr->cableName()=="TAU1" || thr->cableName()=="TAU2") {
         isRun2 = true;
         break;
      }
   }
   ctpc.setCTPVersion(isRun2 ? 4 : 3);
   ctpc.setL1Version(isRun2 ? 1 : 0);
   L1DataDef::setMaxThresholdsFromL1Version( ctpc.l1Version() );
   cout << "TrigConfCoolWriter::readL1Menu for run " << run << ". Determined format " << (isRun2 ? "Run 2" : "Run 1") << endl;

   // items
   vector<TriggerItem*> items;
   readL1Items(run, items);
   for(TriggerItem* item: items)
      menu.addTriggerItem(item);


   readL1ItemDef(run, items, thrs);

   CaloInfo ci;

   for(TriggerThreshold* thr: thrs) {

      if ( thr->name()=="JetWeights" ) {
         if(!isRun2) {
            for ( const string& weights : split( thr->cableName(),",")  )
               ci.addJetWeight( boost::lexical_cast<int, string>(weights) );
         }
      }
      else if ( thr->name()=="METSigParams" ) {
         vector<string> metvals = split( thr->cableName(),",");
         int XSSigmaScale  = boost::lexical_cast<int, string>(metvals[0]);
         int XSSigmaOffset = boost::lexical_cast<int, string>(metvals[1]);
         int XEmin         = boost::lexical_cast<int, string>(metvals[2]);
         int XEmax         = boost::lexical_cast<int, string>(metvals[3]);
         int TESqrtMin     = boost::lexical_cast<int, string>(metvals[4]);
         int TESqrtMax     = boost::lexical_cast<int, string>(metvals[5]);
         ci.metSigParam().setValues( XSSigmaScale, XSSigmaOffset,
                                     XEmin, XEmax, TESqrtMin, TESqrtMax);
      }
      else {
         // trigger thresholds sorted by type
         menu.thresholdConfig().addTriggerThreshold(thr);
      }
   }
   menu.setCaloInfo(ci);

   // PITs (run1), TIPs (run2)
   vector<PIT*> pits;
   vector<TIP*> tips;
   readL1InputMapPayload(run, pits, tips);
   for ( PIT* pit : pits) {
      if(pit->ctpinSlot()==10) continue;
      menu.addPit(pit);
   }
   for ( TIP* tip : tips) {
      //       cout << tip->tipNumber() << "  :  thr = " << tip->thresholdName() << ", slot = " << tip->slot() << ", conn = " << tip->connector() << ", thrBit = "
      //            << tip->thresholdBit() << ", cableBit = " << tip->cableBit() << endl;
      menu.addTip(tip);
   }
}


pair< vector<string>, map<unsigned int,unsigned char> >
TrigConf::TrigConfCoolWriter::readL1BunchGroupRunPayload( unsigned int run)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run);
   IFolderPtr folder = TrigConfCoolFolderSpec::getLvl1BGDescFolder(m_dbPtr);
   IObjectIteratorPtr objects = folder->browseObjects( vr.since(), vr.until()-1, 0 );
   objects->goToNext();
   const IObject& obj = objects->currentRef();
   const IRecord & payload = obj.payload();
   return readLvl1BGDesc( payload.attributeList() );
}


// ------------------------------------------------------------
// readLVL1BGPayload
// requires:
//    runNumber, lumiblockNumber
// returns:
//    Lvl1 BunchGroupSet
//    lvl1BunchGroupKey
// ------------------------------------------------------------
void
TrigConfCoolWriter::readL1BunchGroupLBPayload( unsigned int run, unsigned int lb, int& bgKey,
                                               BunchGroupSet& bgs)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run,lb);
   vr.until() = vr.since(); // single LB

   // read the bunch group key
   IFolderPtr lvl1BGKeyFolder = TrigConfCoolFolderSpec::getLvl1BGKeyFolder(m_dbPtr);
   IObjectIteratorPtr objects = lvl1BGKeyFolder->browseObjects( vr.since(), vr.until(), 0 );
   objects->goToNext();
   const IObject& obj = objects->currentRef();
   const IRecord & payload1 = obj.payload();
   bgKey = readLvl1BGKey( payload1.attributeList() );

   // read the bunch group content
   IFolderPtr lvl1BGContentFolder = TrigConfCoolFolderSpec::getLvl1BGContentFolder(m_dbPtr);
   IObjectIteratorPtr objects2 = lvl1BGContentFolder->browseObjects( vr.since(), vr.until(), 0);
   objects2->goToNext();
   const IObject& obj2 = objects2->currentRef();
   const IRecord & payload2 = obj2.payload();
   vector<BunchGroup> bgV = readLvl1BGContent( payload2.attributeList() );

   // read the bunch group names
   pair< vector<string>, map<unsigned int,unsigned char> > bg_pair = readL1BunchGroupRunPayload(run);
   vector<string> names = bg_pair.first;


   
   uint newBGSSize = bgV.size() <= names.size() ? bgV.size() : names.size();

   if(bgV.size() !=  names.size()) {
      cout << "WARNING Bunchgroup content vector is of size " << bgV.size()
           << ", which is different from the size of the names vector: " << names.size()
           << ". Using " << newBGSSize << endl;

   }

   // create a new bunch group set, set names and bunch groups
   bgs = BunchGroupSet();
   for(unsigned int i=0; i<newBGSSize; i++) {
      string bgname = names[i];
      if(bgname=="") bgname = "NoName";
      bgV[i].setName(bgname);
      bgV[i].setInternalNumber(i);
      bgs.addBunchGroup(bgV[i]);
   }
   bgs.setId(bgKey);
}


// ------------------------------------------------------------
// readLbPayload
// requires:
//    runNumber, lumiblockNumber
// returns:
//    Lvl1 prescales
//    lvl1PrescaleKey
// ------------------------------------------------------------
void
TrigConfCoolWriter::readL1PrescalePayload( unsigned int run, unsigned int lb,
                                           unsigned int& lvl1PrescaleKey,
                                           PrescaleSet & prescale)
{
   AutoDBOpen db(this, READ_ONLY);
   ValidityRange vr(run,lb);
   vr.until() = vr.since()+1; // single LB

   IFolderPtr lvl1CkFolder = TrigConfCoolFolderSpec::getLvl1ConfKeyFolder(m_dbPtr);
   IObjectIteratorPtr objects = lvl1CkFolder->browseObjects( vr.since(), vr.since()+1, 0 );
   objects->goToNext();
   const IObject& obj = objects->currentRef();
   const IRecord & payload = obj.payload();
   readLvl1ConfigKey( payload.attributeList(), lvl1PrescaleKey );


   cool::IFolderPtr lvl1PsFolder = TrigConfCoolFolderSpec::getLvl1PrescalesFolder(m_dbPtr);

   // resize the vector to the correct size
   size_t nPrescales = lvl1PsFolder->listChannels().size();
   bool isRun2 = ( nPrescales == 512 );
   prescale.resize( nPrescales );

   for(cool::ChannelId channel = 0; channel < nPrescales; channel++) {

      objects = lvl1PsFolder->browseObjects( vr.since(), vr.until(), channel );

      if(objects->size()!=1) { 
         throw std::runtime_error("Lvl1 prescale access error: found empty prescale channel ");
      }

      objects->goToNext();
      const IObject& obj = objects->currentRef();
      const IRecord & payload = obj.payload();
      int64_t prescaleVal=0;
      readLvl1Prescale( payload.attributeList(), prescaleVal );
      if( isRun2 ) {
         prescale.setCut( channel, prescaleVal );
      } else {
         prescale.setPrescale( channel, (float)prescaleVal );
      }
   }
   prescale.setId( lvl1PrescaleKey );
}


bool
TrigConf::TrigConfCoolWriter::HLTPrescaleFolderExists() {
   AutoDBOpen db(this, READ_ONLY);
   return m_dbPtr->existsFolder( "/TRIGGER/HLT/Prescales" );
}


/*------------------------------------------------------------
   Shows status of all folders for a certain run
   (starting at lb)
  
   displayMode : 0 - no indicator index
                 1 - indicator index in front of empty 
                     folders
                 2 - indicator index in front of empty 
                     and multiversion folders
 *-----------------------------------------------------------*/

vector<string>
TrigConf::TrigConfCoolWriter::checkPayloadSize(unsigned int run, unsigned int lb, int displayMode, bool openend, unsigned int lbend) {
   AutoDBOpen db(this, READ_ONLY);


   ValidityRange vr(run);
   
   if(openend) {
       vr = ValidityRange(run, lb);
   } else {
       ValidityKey since(run); since <<= 32; since += lb;
       ValidityKey until(run); until <<= 32; until += lbend+1;
       vr = ValidityRange(since, until);
   }
   m_ostream << "Checking for run " << run << " and lb range " << (vr.since() & 0xFFFFFFFF) << " - " << ( (vr.until()-1) & 0xFFFFFFFF) << endl
             << endl
             << "    Folder                                     Payload Size" << endl
             << "================================================================================" << endl;

   vector<string> foldersToFix;

   vector<string>  folderList = { 
      "/TRIGGER/LVL1/Menu",
      "/TRIGGER/LVL1/ItemDef",
      "/TRIGGER/LVL1/Thresholds",
      "/TRIGGER/LVL1/CTPCoreInputMapping",
      "/TRIGGER/LVL1/Lvl1ConfigKey",
      "/TRIGGER/LVL1/Prescales",
      "/TRIGGER/LVL1/BunchGroupKey",
      "/TRIGGER/LVL1/BunchGroupContent",
      "/TRIGGER/LVL1/BunchGroupDescription",
      "/TRIGGER/HLT/HltConfigKeys",
      "/TRIGGER/HLT/Menu",
      "/TRIGGER/HLT/Groups",
      "/TRIGGER/HLT/PrescaleKey",
      "/TRIGGER/HLT/Prescales"
   };
   
   for(const string & folderName : folderList) {
      IFolderPtr folder = m_dbPtr->getFolder( folderName );
      unsigned int size = folder->countObjects( vr.since(), vr.until()-1, ChannelSelection() );
      bool isSingleVersion = folder->versioningMode()==FolderVersioning::SINGLE_VERSION;
      bool needsFixing = (size == 0);

      bool displayFixing = false;
      if(displayMode==1) { // only allow fixing of folders that are empty
         displayFixing = (size == 0);
      } else if(displayMode==2) { // allow fixing of folders that are empty or mv
         displayFixing = (size == 0) || !isSingleVersion;
      }

      string fn = folderName + (isSingleVersion ? " (sv)" : " (mv)");
      if(displayFixing) {
         m_ostream << setw(2) << foldersToFix.size()+1 << ") ";
         foldersToFix.push_back(folderName);
      } else {
         m_ostream << "    ";
      }
      m_ostream << left << setw(40) << fn << right << setw(15) << size << "            " << ( needsFixing ? "NEEDS FIX" : "       OK") << endl;
   }

   return foldersToFix;

}
