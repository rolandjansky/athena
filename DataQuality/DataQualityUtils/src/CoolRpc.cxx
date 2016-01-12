/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//*************************************************
// Class for the RPC interface with the COOL DB
// author Monica Verducci monica.verducci@cern.ch
//************************************************
#include "DataQualityUtils/CoolRpc.h"

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

ClassImp(dqutils::CoolRpc)

namespace dqutils{

 cool::IDatabasePtr
 CoolRpc::
 coolDbInstance(std::string dbStr, bool readOnly) {
     try {
 	 //std::cout << "Opening database '" << dbStr << "'...";
 	 cool::IDatabaseSvc& dbSvc = this->databaseService();
 	// std::cout << "done." << std::endl;
 	 return dbSvc.openDatabase(dbStr.c_str(), readOnly);
     }
     catch (cool::DatabaseDoesNotExist&) {
 	 std::cout << "Error! Database does not exist!" << std::endl;
 	 throw;
     }
 }

 cool::IFolderPtr
 CoolRpc::
 coolFolderInstance(std::string folderStr) {
     try {
 	 cool::IFolderPtr folder = coolDb->getFolder(folderStr.c_str());
 	// std::cout << "Browsing objects of '" << folderStr << "'" << std::endl;
 	 return folder;
     }
     catch (cool::FolderNotFound& ) {
 	 std::cout << "Error! Folder '" << folderStr << "' does not exist!" << std::endl;
 	 throw;
     }
 }


void
 CoolRpc::coolDbFolder(std::string dbStr, std::string folderStr) {
   coolDb = this->coolDbInstance(dbStr, false); 
   coolFolder=this->coolFolderInstance(folderStr);  
 }
 

 void
 CoolRpc::
 setSince(cool::Int64 run, cool::Int64 lumi) {
     since = ((run << 32) + lumi);
 }

 void
 CoolRpc::
 setUntil(cool::Int64 iovmax, cool::Int64 lumi) {
     until = ((iovmax << 32) + lumi);
 }
   
 void
 CoolRpc::
 setIOV(cool::Int64 runS, cool::Int64 lumiS, cool::Int64 runU, cool::Int64 lumiU) {
     this->setSince(runS, lumiS);
     this->setUntil(runU, lumiU);
     this->printIOV();
 }

 void
 CoolRpc::
 setIOV(cool::Int64 run) {
     this->setSince(run, 0);
     this->setUntil(run, 4294967295U);
     this->printIOV();
 }
   

 void
 CoolRpc::
 printIOV(){
     cool::Int64 runS=since>>32;
     cool::Int64 lumiS=since-(runS<<32);
     cool::Int64 runU=until>>32;
     cool::Int64 lumiU=until-(runU<<32);
     std::cout << "Using IOVrange [(" << runS << "," << lumiS << "),("  << runU << "," << lumiU << ")] " << "[" << since << "," << until << "]" << std::endl;
 }


 void
 CoolRpc::
 CoolOpen(std::string dbStr) {
     coolDb = this->coolDbInstance(dbStr, false);
 
 }
 
 CoolRpc::
 ~CoolRpc () {
     coolDb->closeDatabase();
     std::cout << "Cleared!" << std::endl;
 }


 cool::RecordSpecification
 CoolRpc::
 createSpecData() {
   //std::cout << "Preparing RecordSpecification" << std::endl;
     cool::RecordSpecification spec;
     spec.extend("recEta",cool::StorageType::String4k);
     spec.extend("detEta",cool::StorageType::String4k);
     spec.extend("recPhi1",cool::StorageType::String4k);
     spec.extend("recPhi2",cool::StorageType::String4k);
     spec.extend("detPhi1",cool::StorageType::String4k);
     spec.extend("detPhi2",cool::StorageType::String4k);
     coolFolder=this->coolFolderInstance("/OFFLINE/OFFLINE_DQMF");
     if (!(spec==coolFolder->payloadSpecification())) {
       std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
     }
  //   std::cout << "CREATE DONE" << std::endl;
     return spec;
 }


 cool::RecordSpecification
 CoolRpc::
 createSpecDataCondDB() {
   //std::cout << "Preparing RecordSpecification" << std::endl;
     cool::RecordSpecification spec;
     spec.extend("PanelRes",  cool::StorageType::String255);
     spec.extend("StripStatus",cool::StorageType::String4k);
     coolFolder=this->coolFolderInstance("/OFFLINE/FINAL");
     if (!(spec==coolFolder->payloadSpecification())) {
       std::cout << "ERROR Source and destination folder specifications differ." << std::endl;
     }
  //   std::cout << "CREATE DONE" << std::endl;
     return spec;
 }

 coral::AttributeList
 CoolRpc::
 createPayloadData(std::string recEta, std::string detEta, std::string recPhi1, std::string recPhi2, std::string detPhi1, std::string detPhi2, const cool::RecordSpecification& spec) {
  //  std::cout << "createPayloadData "<< std::endl;

     coral::AttributeList payload = cool::Record( spec ).attributeList();
 
     payload["recEta"].data<cool::String4k>() = recEta;
     payload["detEta"].data<cool::String4k>() = detEta;
     payload["recPhi1"].data<cool::String4k>() = recPhi1;

     payload["recPhi2"].data<cool::String4k>() = recPhi2;
     payload["detPhi1"].data<cool::String4k>() = detPhi1;
     payload["detPhi2"].data<cool::String4k>() = detPhi2;
 
   //  std::cout << "Creating payload: ";
   //  std::cout << "[RecEta : " << recEta << "],";
   //  std::cout << "[DetEta : " << detEta <<  "],"<< std::endl;
     return payload;
 }

 coral::AttributeList
 CoolRpc::
 createPayloadDataCondDB(std::string PanelRes,std::string StripStatus,  const cool::RecordSpecification& spec) {
  //  std::cout << "createPayloadData "<< std::endl;

     coral::AttributeList payload = cool::Record( spec ).attributeList();
 
     payload["PanelRes"].data<cool::String255>() = PanelRes;
     payload["StripStatus"].data<cool::String4k>() = StripStatus;
 
     return payload;
 }
 
 void
 CoolRpc::
 dump(cool::ChannelSelection selection) {
     try {
 	 cool::IObjectIteratorPtr objects = coolFolder->browseObjects(since, until, selection,"");
 	 while (objects->goToNext()) {
 	     const cool::IObject& element = objects->currentRef();
 	     std::cout << element << std::endl;
 	 }
     }
     catch (cool::Exception& e) {
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
 	 std::cout << " Inside create payload" << std::endl;
     }
 }

 std::string
 CoolRpc::
 dumpField(cool::ChannelId channelId, std::string field) {
     std::string result ="";
     try {
 	 cool::ChannelSelection selection = cool::ChannelSelection(channelId);
 	 cool::IObjectIteratorPtr objects = coolFolder->browseObjects(since, until, selection,"");
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
 CoolRpc::
 dumpCode(std::string channelName) {
     std::string result = this->dumpField(this->getCoolFolder()->channelId(channelName.c_str()), "Code");
     return atoi(result.c_str());
 
}

 void
 CoolRpc::
 dumpall() {
   this->dump(cool::ChannelSelection::all());
 }



   // efficiency/calibration

 void
 CoolRpc::
 insert_withTag(cool::Int64 run, cool::ChannelId channelId, std::string recEta, std::string detEta, std::string recPhi1, std::string recPhi2, std::string detPhi1, std::string detPhi2 , std::string cool_tag) {
  // std::cout << "Trying to store payload [channel " << std::endl;
     try {
  
       cool::RecordSpecification spec = this->createSpecData();
       coral::AttributeList payload = this->createPayloadData(recEta, detEta, recPhi1, recPhi2, detPhi1, detPhi2, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       coolFolder->storeObject(since_u, until_u, cool::Record(coolFolder->payloadSpecification(), payload), channelId, cool_tag);
    //   std::cout << "stored! With Tag =" << cool_tag <<std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " Inside create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }

 void
 CoolRpc::
 insertCondDB_withTag(cool::Int64 run, cool::ChannelId channelId, std::string PanelRes, std::string StripStatus, std::string cool_tag) {
  // std::cout << "Trying to store payload [channel " << std::endl;
     try {
  
       cool::RecordSpecification spec = this->createSpecDataCondDB();
       coral::AttributeList payload = this->createPayloadDataCondDB(PanelRes, StripStatus, spec);
       cool::ValidityKey since_u = (run << 32)*0;
       cool::ValidityKey  until_u = (run+1) << 32;
       coolFolder->storeObject(since_u, until_u, cool::Record(coolFolder->payloadSpecification(), payload), channelId, cool_tag);
    //   std::cout << "stored! With Tag =" << cool_tag <<std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " Inside create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }

 void
 CoolRpc::
 insert(cool::Int64 run, cool::ChannelId channelId, std::string recEta, std::string detEta, std::string recPhi1, std::string recPhi2, std::string detPhi1, std::string detPhi2) {
   std::cout << "Trying to store payload [channel " << std::endl;
     try {
  
       cool::RecordSpecification spec = this->createSpecData();
       coral::AttributeList payload = this->createPayloadData(recEta, detEta, recPhi1, recPhi2, detPhi1, detPhi2, spec);
       cool::ValidityKey since_u = (run << 32);
       cool::ValidityKey  until_u = (run+1) << 32;
       coolFolder->storeObject(since_u, until_u, cool::Record(coolFolder->payloadSpecification(), payload), channelId);
       std::cout << "stored! without Tag" << std::endl;
     }
     catch (cool::Exception& e) {
       std::cout << " Inside create insert" << std::endl;
 	 std::cout << "Unknown exception caught!" << e.what() << std::endl;
     }
 }



 
 cool::IFolderPtr
 CoolRpc::
 getCoolFolder() {
     return this->coolFolder;
 }

 cool::IDatabasePtr
 CoolRpc::
 getCoolDb() {
     return this->coolDb;
 }


} //namespace dqutils
