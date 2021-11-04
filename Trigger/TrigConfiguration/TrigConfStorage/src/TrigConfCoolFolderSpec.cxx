/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigConfStorage/TrigConfCoolFolderSpec.h"

#include <iomanip>
#include <stdexcept>

#include "CoolKernel/IFolder.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/FolderSpecification.h"

using namespace std;
using namespace cool;
using namespace TrigConf;

// Size of BLOB used for ItemToBunchGroup map; BLOBs are cheap, in case
// we want to add information later on like more complicated logic /
// also OR's, we can easily increase the size here
const long TrigConf::TrigConfCoolFolderSpec::mBGDescBlobSize = 512;

// BLOB size of BG content is the number of LHC bunches
const long TrigConf::TrigConfCoolFolderSpec::mBGContentBlobSize = 3564;

const int TrigConf::TrigConfCoolFolderSpec::mDefaultSchemaVersion = 3;

// ------------------------------------------------------------
// readSchemaVersion
// ------------------------------------------------------------
int
TrigConfCoolFolderSpec::readSchemaVersion(cool::IDatabasePtr db) {

   // version 4:
   // L1 prescales are Int64

   const IFolderPtr & l1prescaleFolder = db->getFolder("/TRIGGER/LVL1/Prescales");
   const IRecordSpecification & l1psSpec = l1prescaleFolder->payloadSpecification();
   const IFieldSpecification& l1psfield = l1psSpec["Lvl1Prescale"];
   if(l1psfield.storageType() == StorageType::Int64) {
      return 4;
   }

   const IFolderPtr & hltMenuFolder = db->getFolder("/TRIGGER/HLT/Menu");
   const IRecordSpecification & hltMenuSpec = hltMenuFolder->payloadSpecification();

   if(hltMenuSpec.exists("Prescale")) {

      const IFieldSpecification& lowChNField = hltMenuSpec["LowerChainName"];

      // version 3:
      // hlt menu has long (4000 char) lower chain name field
      if(lowChNField.storageType() == StorageType::String4k) {
         return 3;
      }

      const IFieldSpecification& psField = hltMenuSpec["Prescale"];

      // version 1:
      // hlt menu has integer prescale and pass_through values
      if(psField.storageType() == StorageType::UInt32) {
         return 1;
      }

      // version 2:
      // hlt menu has float prescale and pass_through values
      if(psField.storageType() == StorageType::Float) {
         return 2;
      }
   }

   // should never happen
   throw runtime_error("Could not determine the COOL schema of the TriggerDB");

   return -1;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1MenuFolderSpecification() {
   return Lvl1MenuFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1MenuFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "ItemName",    StorageType::String255 );
   rspec.extend( "ItemVersion", StorageType::UInt32 );
   FolderDefinition fd( "/TRIGGER/LVL1/Menu", 
                        FolderVersioning::SINGLE_VERSION, rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1ItemDefFolderSpecification() {
   return Lvl1MenuFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1ItemDefFolderDefinition() {
   cool::RecordSpecification rspec;
   
   rspec.extend( "Logic",          StorageType::String255 );
   rspec.extend( "ConditionsList", StorageType::String4k );
   FolderDefinition fd( "/TRIGGER/LVL1/ItemDef", 
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1ThresholdFolderSpecification() {
   return Lvl1ThresholdFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1ThresholdFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "Threshold",      StorageType::String255 );
   rspec.extend( "ThresholdValue", StorageType::String4k );
   rspec.extend( "Cable",          StorageType::String255 );
   FolderDefinition fd( "/TRIGGER/LVL1/Thresholds", 
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1ConfigKeysFolderSpecification() {
   return Lvl1ConfigKeysFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1ConfigKeysFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "Lvl1PrescaleConfigurationKey", StorageType::UInt32 );
   FolderDefinition fd( "/TRIGGER/LVL1/Lvl1ConfigKey", 
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1PrescalesFolderSpecification(int schemaVersion) {
   return Lvl1PrescalesFolderDefinition(schemaVersion).rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1PrescalesFolderDefinition(int schemaVersion) {
   cool::RecordSpecification rspec;
   if(schemaVersion<4) {
      rspec.extend( "Lvl1Prescale", StorageType::Int32 );
   } else if(schemaVersion>=4) {
      rspec.extend( "Lvl1Prescale", StorageType::Int64 );
   }
   FolderDefinition fd( "/TRIGGER/LVL1/Prescales", 
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1InputMapFolderSpecification() {
   return Lvl1InputMapFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1InputMapFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "ThresholdName",    StorageType::String255 );
   rspec.extend( "CtpinSlot",        StorageType::UChar );
   rspec.extend( "CtpinConnector",   StorageType::UChar );
   rspec.extend( "ThresholdBit",     StorageType::UChar );
   rspec.extend( "CableBit",         StorageType::UChar );
   rspec.extend( "ThresholdMapping", StorageType::UChar );
   rspec.extend( "ThresholdActive",  StorageType::Bool );
   FolderDefinition fd( "/TRIGGER/LVL1/CTPCoreInputMapping",
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1BGKeyFolderSpecification() {
   return  Lvl1BGKeyFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1BGKeyFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "Lvl1BunchGroupConfigurationKey", StorageType::UInt32 );
   FolderDefinition fd( "/TRIGGER/LVL1/BunchGroupKey",
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1BGContentFolderSpecification() {
   return Lvl1BGContentFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1BGContentFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "BunchCode" , StorageType::Blob64k );
   FolderDefinition fd( "/TRIGGER/LVL1/BunchGroupContent",
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createLvl1BGDescFolderSpecification() {
   return Lvl1BGDescFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1BGDescFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "BunchGroup0", StorageType::String255 );
   rspec.extend( "BunchGroup1", StorageType::String255 );
   rspec.extend( "BunchGroup2", StorageType::String255 );
   rspec.extend( "BunchGroup3", StorageType::String255 );
   rspec.extend( "BunchGroup4", StorageType::String255 );
   rspec.extend( "BunchGroup5", StorageType::String255 );
   rspec.extend( "BunchGroup6", StorageType::String255 );
   rspec.extend( "BunchGroup7", StorageType::String255 );
   rspec.extend( "BunchGroup8", StorageType::String255 );
   rspec.extend( "BunchGroup9", StorageType::String255 );
   rspec.extend( "BunchGroup10", StorageType::String255 );
   rspec.extend( "BunchGroup11", StorageType::String255 );
   rspec.extend( "BunchGroup12", StorageType::String255 );
   rspec.extend( "BunchGroup13", StorageType::String255 );
   rspec.extend( "BunchGroup14", StorageType::String255 );
   rspec.extend( "BunchGroup15", StorageType::String255 );
   rspec.extend( "ItemToBunchGroupMap" ,StorageType::Blob64k );
   FolderDefinition fd( "/TRIGGER/LVL1/BunchGroupDescription", 
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createHltMenuFolderSpecification() {
   return HltMenuFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::HltMenuFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "ChainName",       StorageType::String255 );
   rspec.extend( "ChainVersion",    StorageType::UInt32 );
   rspec.extend( "ChainCounter",    StorageType::UInt32 );
   rspec.extend( "TriggerLevel",    StorageType::String255 );
   rspec.extend( "LowerChainName",  StorageType::String4k );
   rspec.extend( "Prescale",        StorageType::Float );
   rspec.extend( "PassThrough",     StorageType::Float );
   rspec.extend( "TriggerElements", StorageType::String4k );
   rspec.extend( "StreamInfo",      StorageType::String255 );
   FolderDefinition fd( "/TRIGGER/HLT/Menu", 
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createHltChainGroupFolderSpecification() {
   return HltChainGroupFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::HltChainGroupFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "ChainCounter", StorageType::UInt32 );
   rspec.extend( "Groups",       StorageType::String4k );
   FolderDefinition fd( "/TRIGGER/HLT/Groups",
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createHltConfigKeysFolderSpecification() {
   return HltConfigKeysFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::HltConfigKeysFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "MasterConfigurationKey",       StorageType::UInt32 );
   rspec.extend( "HltPrescaleConfigurationKey",  StorageType::UInt32 );
   rspec.extend( "ConfigSource",                 StorageType::String255 );
   FolderDefinition fd( "/TRIGGER/HLT/HltConfigKeys",
                        FolderVersioning::SINGLE_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createHltPrescalesFolderSpecification() {
   return HltPrescalesFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::HltPrescalesFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "Prescale",      StorageType::Float );
   rspec.extend( "Passthrough",   StorageType::Float );
   rspec.extend( "RerunPrescale", StorageType::Float );
   FolderDefinition fd( "/TRIGGER/HLT/Prescales",
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------

RecordSpecification
TrigConfCoolFolderSpec::createHltPrescaleKeyFolderSpecification() {
   return HltPrescaleKeyFolderDefinition().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::HltPrescaleKeyFolderDefinition() {
   cool::RecordSpecification rspec;
   rspec.extend( "HltPrescaleKey", StorageType::UInt32 );
   FolderDefinition fd( "/TRIGGER/HLT/PrescaleKey",
                        FolderVersioning::MULTI_VERSION,rspec);
   return fd;
}

// --------------------------------------------------------------------------------


// ------------------------------------------------------------
// createLvl1MonMapFolderSpecification()
// ------------------------------------------------------------
RecordSpecification
TrigConfCoolFolderSpec::createLvl1MonMapFolderSpecification() {
   return Lvl1MonMapFolderSpecification().rspec();
}

FolderDefinition
TrigConfCoolFolderSpec::Lvl1MonMapFolderSpecification() {
   RecordSpecification rspec;
   rspec.extend( "CounterType",       StorageType::String255 );
   rspec.extend( "BunchGroupId",      StorageType::UChar );
   rspec.extend( "ThresholdName",     StorageType::String255 );
   rspec.extend( "CtpinSlot",         StorageType::String255 );
   rspec.extend( "CtpinConnector",    StorageType::String255 );
   rspec.extend( "Multiplicity",      StorageType::String255 );
   rspec.extend( "ThresholdBitStart", StorageType::String255 );
   rspec.extend( "ThresholdBitEnd",   StorageType::String255 );
   rspec.extend( "ThresholdActive",   StorageType::String255 );
   rspec.extend( "CounterName",       StorageType::String255 );
   rspec.extend( "CounterLogic",      StorageType::String255 );  
   FolderDefinition fd("/TRIGGER/LVL1/CTPInMonitoringMapping", 
                       FolderVersioning::SINGLE_VERSION, rspec);
   return fd;
}




bool
TrigConfCoolFolderSpec::CreateFolderIfNotExist(IDatabasePtr db, const string& folder, const IRecordSpecification& spec, FolderVersioning::Mode mode, bool isMultiChannel) {

   cool::RecordSpecification rspec;
   rspec.extend(spec);
   FolderDefinition fd(folder, mode,rspec);
   return CreateFolderIfNotExist(db, fd, isMultiChannel);
}


bool
TrigConfCoolFolderSpec::CreateFolderIfNotExist(IDatabasePtr db, FolderDefinition fd, bool isMultiChannel) {

   static const string singleChannelDesc = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";
   static const string multiChannelDesc  = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";

   if( db->existsFolder( fd.folder ) )
      return false;

   const string & description  = isMultiChannel ? multiChannelDesc : singleChannelDesc;

   bool createParents = true;

   IFolderPtr ptr = db->createFolder( fd.folder, fd.fspec, description, createParents );

   return true;
}





// ------------------------------------------------------------
// createFolderStructure()
// ------------------------------------------------------------
IFolderSetPtr
TrigConfCoolFolderSpec::createFolderStructure(IDatabasePtr db, int schemaVersion) {

   if(schemaVersion==0) schemaVersion = getDefaultSchemaVersion();

   std::string singleChannelDesc = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";
   std::string multiChannelDesc  = "<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
   //std::string timeDesc  = "<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";

   bool newFolder = false;

   static const bool singleChannel(false);
   static const bool multiChannel(true);

   IFolderSetPtr topFolder;
   if( !db->existsFolderSet( "/TRIGGER" ) ) {
      newFolder = true;
      topFolder = db->createFolderSet( "/TRIGGER" );
   }
   if( !db->existsFolderSet( "/TRIGGER/LVL1" ) ) {
      newFolder = true;
      db->createFolderSet( "/TRIGGER/LVL1" );
   }
   if( !db->existsFolderSet( "/TRIGGER/HLT" ) ) {
      newFolder = true;
      db->createFolderSet( "/TRIGGER/HLT" );
   }

   newFolder |= CreateFolderIfNotExist(db, Lvl1MenuFolderDefinition(),       multiChannel);

   newFolder |= CreateFolderIfNotExist(db, Lvl1ItemDefFolderDefinition(),    multiChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1ThresholdFolderDefinition(),  multiChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1ConfigKeysFolderDefinition(), singleChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1PrescalesFolderDefinition(schemaVersion),  multiChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1BGKeyFolderDefinition(),      singleChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1BGContentFolderDefinition(),  singleChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1BGDescFolderDefinition(),     singleChannel);
     
   newFolder |= CreateFolderIfNotExist(db, Lvl1InputMapFolderDefinition(),   multiChannel);
     
   newFolder |= CreateFolderIfNotExist(db, HltConfigKeysFolderDefinition(),  singleChannel);
      
   newFolder |= CreateFolderIfNotExist(db, HltMenuFolderDefinition(),        multiChannel);
      
   newFolder |= CreateFolderIfNotExist(db, HltChainGroupFolderDefinition(),  multiChannel);
      
   newFolder |= CreateFolderIfNotExist(db, HltPrescalesFolderDefinition(),   multiChannel);
      
   newFolder |= CreateFolderIfNotExist(db, HltPrescaleKeyFolderDefinition(), singleChannel);
      
   if(newFolder) printFolderStructure(db, cout);

   return topFolder;
}

IFolderSetPtr
TrigConfCoolFolderSpec::createMonFolderStructure(IDatabasePtr db, int /*schemaVersion*/) {

   bool multiChannel(true);


   bool newFolder = CreateFolderIfNotExist(db, Lvl1MonMapFolderSpecification(), multiChannel);
   if(newFolder)
      printFolderStructure(db, cout);

   return db->getFolderSet( "/TRIGGER" );
}




// ------------------------------------------------------------
// folder accessors
// ------------------------------------------------------------

IFolderPtr TrigConfCoolFolderSpec::getLvl1MenuFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/Menu" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1ItemDefFolder(IDatabasePtr db) {
  return db->getFolder( "/TRIGGER/LVL1/ItemDef" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1ThresholdFolder(IDatabasePtr db) {
  return db->getFolder( "/TRIGGER/LVL1/Thresholds" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1ConfKeyFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/Lvl1ConfigKey" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1PrescalesFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/Prescales" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1BGKeyFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/BunchGroupKey" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1BGContentFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/BunchGroupContent" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1BGDescFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/BunchGroupDescription" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1InputMapFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/CTPCoreInputMapping" );
}

IFolderPtr TrigConfCoolFolderSpec::getLvl1MonMapFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/LVL1/CTPInMonitoringMapping" );
}

IFolderPtr TrigConfCoolFolderSpec::getHltMenuFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/Menu" );
}

IFolderPtr TrigConfCoolFolderSpec::getHltChainGroupFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/Groups" );
}

IFolderPtr TrigConfCoolFolderSpec::getHltConfKeyFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/HltConfigKeys" );
}

IFolderPtr TrigConfCoolFolderSpec::getMonConfKeyFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/MenuAwareMonConfigKey" );
}

IFolderPtr TrigConfCoolFolderSpec::getHltPrescalesFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/Prescales" );
}

IFolderPtr TrigConfCoolFolderSpec::getHltPrescaleKeyFolder(IDatabasePtr db) {
   return db->getFolder( "/TRIGGER/HLT/PrescaleKey" );
}


// ------------------------------------------------------------
// printFolderStructure(IDatabasePtr db)
// ------------------------------------------------------------
void
TrigConfCoolFolderSpec::printFolderStructure(IDatabasePtr db, std::ostream & o)
{

   for(const string& node : db->listAllNodes()) {
      bool isFolderSet = db->existsFolderSet(node);
      o << "        " << node;
      if(isFolderSet) 
         o << "/" << endl;
      else {
         IFolderPtr folder = db->getFolder(node);
         bool isMultiVersion = (folder->versioningMode() == FolderVersioning::MULTI_VERSION); 
         const IRecordSpecification& rspec = folder->payloadSpecification();
         o << " (" << (isMultiVersion?"Multiversion":"Singleversion") << ")" << endl;
         o.setf(ios::left);
         for(uint i=0;i<rspec.size(); i++) {
            o << "            " << setw(30) << rspec[i].name() 
              << "         (" << rspec[i].storageType().name() << ")" << endl;
         }
      }
   }
   return;
}

