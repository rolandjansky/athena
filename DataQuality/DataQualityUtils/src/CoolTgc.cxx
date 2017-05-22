/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************
// Class for the TGC interface with the COOL DB
// copy of CoolMdt.cxx by Monica Verducci
// author Akimasa Ishikawa 
//************************************************
#include "DataQualityUtils/CoolTgc.h"

//CORAL API include files
#include "CoralBase/Attribute.h"

//COOL API include files (CoolKernel)
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/StorageType.h"
#include "CoolKernel/ConstRecordAdapter.h"

ClassImp(dqutils::CoolTgc)

namespace dqutils{

 cool::IDatabasePtr
 CoolTgc::
 coolDbInstance(std::string dbStr, bool readOnly) {
     try {
 	 std::cout << "Opening database '" << dbStr << "'...";
 	 cool::IDatabaseSvc& dbSvc = this->databaseService();
 	 std::cout << "done." << std::endl;
 	 return dbSvc.openDatabase(dbStr.c_str(), readOnly);
     }
     catch (cool::DatabaseDoesNotExist&) {
 	 std::cout << "Error! Database does not exist!" << std::endl;
 	 throw;
     }
 }

 cool::IFolderPtr
 CoolTgc::
 coolFolderInstance(std::string folderStr) {
     try {
 	 cool::IFolderPtr folder = m_coolDb->getFolder(folderStr.c_str());
 	// std::cout << "Browsing objects of '" << folderStr << "'" << std::endl;
 	 return folder;
     }
     catch (cool::FolderNotFound& ) {
 	 std::cout << "Error! Folder '" << folderStr << "' does not exist!" << std::endl;
 	 throw;
     }
 }


void
 CoolTgc::coolDbFolder(std::string dbStr, std::string folderStr) {
   m_coolDb = this->coolDbInstance(dbStr, false); 
   m_coolFolder=this->coolFolderInstance(folderStr);  
 }
 

 void
 CoolTgc::
 setSince(cool::Int64 run, cool::Int64 lumi) {
     m_since = ((run << 32) + lumi);
 }

 void
 CoolTgc::
 setUntil(cool::Int64 run, cool::Int64 lumi) {
     m_until = ((run << 32) + lumi);
 }
   
 void
 CoolTgc::
 setIOV(cool::Int64 runS, cool::Int64 lumiS, cool::Int64 runU, cool::Int64 lumiU) {
     this->setSince(runS, lumiS);
     this->setUntil(runU, lumiU);
     this->printIOV();
 }

 void
 CoolTgc::
 setIOV(cool::Int64 run) {
     this->setSince(run, 0);
     this->setUntil(run, 4294967295U);
     this->printIOV();
 }
   

 void
 CoolTgc::
 printIOV(){
     cool::Int64 runS=m_since>>32;
     cool::Int64 lumiS=m_since-(runS<<32);
     cool::Int64 runU=m_until>>32;
     cool::Int64 lumiU=m_until-(runU<<32);
     std::cout << "Using IOVrange [(" << runS << "," << lumiS << "),("  << runU << "," << lumiU << ")] " << "[" << m_since << "," << m_until << "]" << std::endl;
 }


 void
 CoolTgc::
 CoolOpen(std::string dbStr) {
     m_coolDb = this->coolDbInstance(dbStr, false);
 
 }
 
 CoolTgc::
 ~CoolTgc () {
     m_coolDb->closeDatabase();
     std::cout << "Cleared!" << std::endl;
 }


 cool::RecordSpecification
 CoolTgc::
 createSpecDataDead() {
   //std::cout << "Preparing RecordSpecification" << std::endl;
     cool::RecordSpecification spec;
     spec.extend("Chamber_Name",cool::StorageType::String255);
     spec.extend("Dead_multilayer",cool::StorageType::String255);
     spec.extend("Dead_tube",cool::StorageType::String4k);
    
     m_coolFolder=this->coolFolderInstance("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_DEAD");
     if (!(spec==m_coolFolder->payloadSpecification())) {
       std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
     }
  //   std::cout << "CREATE DONE" << std::endl;
     return spec;
 }


 coral::AttributeList
 CoolTgc::
 createPayloadDataDead(std::string ChamberName, std::string  DeadMultilayer, std::string DeadTube, const cool::RecordSpecification& spec) {
  //  std::cout << "createPayloadData "<< std::endl;

     coral::AttributeList payload = cool::Record( spec ).attributeList();
 
     payload["Chamber_Name"].data<cool::String255>() = ChamberName;
     payload["Dead_multilayer"].data<cool::String255>() = DeadMultilayer;
     payload["Dead_tube"].data<cool::String4k>() = DeadTube;

     return payload;
 }

  // noisy

 cool::RecordSpecification
 CoolTgc::
 createSpecDataNoisy() {
   //std::cout << "Preparing RecordSpecification" << std::endl;
     cool::RecordSpecification spec;
     spec.extend("Chamber_Name",cool::StorageType::String255);
     spec.extend("Noisy_multilayer",cool::StorageType::String255);
     spec.extend("Noisy_tube",cool::StorageType::String4k);
    
     m_coolFolder=this->coolFolderInstance("/OFFLINE/DQMFOFFLINE/DQMFOFFLINE_NOISY");
     if (!(spec==m_coolFolder->payloadSpecification())) {
       std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
     }
  //   std::cout << "CREATE DONE" << std::endl;
     return spec;
 }


 coral::AttributeList
 CoolTgc::
 createPayloadDataNoisy(std::string ChamberName, std::string  NoisyMultilayer, std::string NoisyTube, const cool::RecordSpecification& spec) {
  //  std::cout << "createPayloadData "<< std::endl;

     coral::AttributeList payload = cool::Record( spec ).attributeList();
 
     payload["Chamber_Name"].data<cool::String255>() = ChamberName;
     payload["Noisy_multilayer"].data<cool::String255>() = NoisyMultilayer;
     payload["Noisy_tube"].data<cool::String4k>() = NoisyTube;

     return payload;
 }
  //

 
 void
 CoolTgc::
 dump(cool::ChannelSelection selection) {
     try {
 	 cool::IObjectIteratorPtr objects = m_coolFolder->browseObjects(m_since, m_until, selection,"");
 	 while (objects->goToNext()) {
 	     const cool::IObject& element = objects->currentRef();
 	     std::cout << element << std::endl;
 	 }
     }
     catch (cool::Exception& e) {
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
 	 std::cout << " dentro create payload" << std::endl;
     }
 }

 std::string
 CoolTgc::
 dumpField(cool::ChannelId channelId, std::string field) {
     std::string result ="";
     try {
 	 cool::ChannelSelection selection = cool::ChannelSelection(channelId);
 	 cool::IObjectIteratorPtr objects = m_coolFolder->browseObjects(m_since, m_until, selection,"");
 	 while (objects->goToNext()) {
 	     const cool::IObject& element = objects->currentRef();
 	     result = element.payloadValue(field);
 	 }
     }
     catch (cool::Exception& e) {
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
     return result;
 }


 int
 CoolTgc::
 dumpCode(std::string channelName) {
     std::string result = this->dumpField(this->getCoolFolder()->channelId(channelName.c_str()), "Code");
     return atoi(result.c_str());
 
}

 void
 CoolTgc::
 dumpall() {
   this->dump(cool::ChannelSelection::all());
 }



 void
 CoolTgc::
 insertDeadFlag_withTag(cool::Int64 run, cool::ChannelId channelId, std::string ChamberName, std::string  DeadMultilayer, std::string DeadTube, std::string cool_tag) {

     try {
  
       cool::RecordSpecification spec = this->createSpecDataDead();
       coral::AttributeList payload = this->createPayloadDataDead(ChamberName,DeadMultilayer , DeadTube, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       m_coolFolder->storeObject(since_u, until_u, cool::Record(m_coolFolder->payloadSpecification(), payload), channelId, cool_tag);
       std::cout << "stored! With Tag =" << cool_tag <<std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " dentro create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }


 void
 CoolTgc::
 insertDeadFlag(cool::Int64 run, cool::ChannelId channelId,std::string ChamberName, std::string  DeadMultilayer, std::string DeadTube) {
   std::cout << "Trying to store payload [channel " << std::endl;
     try {
  
       cool::RecordSpecification spec = this->createSpecDataDead();
       coral::AttributeList payload = this->createPayloadDataDead(ChamberName,DeadMultilayer , DeadTube, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       m_coolFolder->storeObject(since_u, until_u, cool::Record(m_coolFolder->payloadSpecification(), payload), channelId);
       std::cout << "stored! without Tag" << std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " dentro create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }
  //



 void
 CoolTgc::
 insertNoisyFlag_withTag(cool::Int64 run, cool::ChannelId channelId, std::string ChamberName, std::string  NoisyMultilayer, std::string NoisyTube, std::string cool_tag) {

     try {
  
       cool::RecordSpecification spec = this->createSpecDataNoisy();
       coral::AttributeList payload = this->createPayloadDataNoisy(ChamberName,NoisyMultilayer , NoisyTube, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       m_coolFolder->storeObject(since_u, until_u, cool::Record(m_coolFolder->payloadSpecification(), payload), channelId, cool_tag);
       std::cout << "stored! With Tag =" << cool_tag <<std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " dentro create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }


 void
 CoolTgc::
 insertNoisyFlag(cool::Int64 run, cool::ChannelId channelId,std::string ChamberName, std::string  NoisyMultilayer, std::string NoisyTube) {
   std::cout << "Trying to store payload [channel " << std::endl;
     try {
  
       cool::RecordSpecification spec = this->createSpecDataNoisy();
       coral::AttributeList payload = this->createPayloadDataNoisy(ChamberName,NoisyMultilayer , NoisyTube, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       m_coolFolder->storeObject(since_u, until_u, cool::Record(m_coolFolder->payloadSpecification(), payload), channelId);
       std::cout << "stored! without Tag" << std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " dentro create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }



 
 cool::IFolderPtr
 CoolTgc::
 getCoolFolder() {
     return this->m_coolFolder;
 }

 cool::IDatabasePtr
 CoolTgc::
 getCoolDb() {
     return this->m_coolDb;
 }


} //namespace dqutils
