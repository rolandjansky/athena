/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// **********************************************************************
// $Id: StatusFlagCOOLBase.cxx,v 1.6 2009-02-13 12:32:11 ponyisi Exp $
// **********************************************************************

#include "DataQualityUtils/StatusFlagCOOLBase.h"

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

ClassImp(dqutils::StatusFlagCOOLBase)

namespace dqutils{

cool::IDatabasePtr 
StatusFlagCOOLBase::
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
StatusFlagCOOLBase::
coolFolderInstance(std::string folderStr) {
    try {
        cool::IFolderPtr folder = coolDb->getFolder(folderStr.c_str());
        std::cout << "Browsing objects of '" << folderStr << "'" << std::endl;
	//folder->setupStorageBuffer();
        return folder;
    }
    catch (cool::FolderNotFound& ) {
        std::cout << "Error! Folder '" << folderStr << "' does not exist!" << std::endl;
        throw;
    }
}

void 
StatusFlagCOOLBase::
setSince(cool::Int64 run, cool::Int64 lumi) {
    since = ((run << 32) + lumi);
}

void 
StatusFlagCOOLBase::
setUntil(cool::Int64 run, cool::Int64 lumi) {
    until = ((run << 32) + lumi);
}

void
StatusFlagCOOLBase::
setIOV(cool::Int64 runS, cool::Int64 lumiS, cool::Int64 runU, cool::Int64 lumiU) {
    this->setSince(runS, lumiS);
    this->setUntil(runU, lumiU);
    //this->printIOV();
}

void
StatusFlagCOOLBase::
setIOV(cool::Int64 run) {
    this->setSince(run, 0);
    this->setUntil(run, cool::UInt32Max);
    //this->printIOV();
}


void
StatusFlagCOOLBase::
printIOV(){
    cool::Int64 runS=since>>32;
    cool::Int64 lumiS=since-(runS<<32);
    cool::Int64 runU=until>>32;
    cool::Int64 lumiU=until-(runU<<32);
    std::cout << "Using IOVrange [(" << runS << "," << lumiS << "),("  << runU << "," << lumiU << ")[ " << "[" << since << "," << until << "[" << std::endl;
}

void
StatusFlagCOOLBase::
flush() {
  //coolFolder->flushStorageBuffer();
}

void
StatusFlagCOOLBase::
Initialize(std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU) {
  coolDb = this->coolDbInstance(dbStr, false);
  coolFolder = this->coolFolderInstance(folderStr);
  this->setIOV(runS, lumiS, runU, lumiU);
}

  
StatusFlagCOOLBase::
StatusFlagCOOLBase (std::string dbStr, std::string folderStr, int runS, int lumiS, int runU, int lumiU) {
  Initialize(dbStr, folderStr, runS, lumiS, runU, lumiU);
}

StatusFlagCOOLBase::
StatusFlagCOOLBase(int runS, int lumiS, int runU, int lumiU) {
  Initialize("COOLOFL_GLOBAL/OFLP200", "/GLOBAL/DETSTATUS/SHIFTOFL", 
	     runS, lumiS, runU, lumiU);
}

StatusFlagCOOLBase::
StatusFlagCOOLBase() {
  Initialize("COOLOFL_GLOBAL/OFLP200", "/GLOBAL/DETSTATUS/SHIFTOFL", 
	     0, 0, 0, 0);
}  

StatusFlagCOOLBase::
~StatusFlagCOOLBase () {
  //coolFolder->flushStorageBuffer();
    coolDb->closeDatabase();
    std::cout << "Cleared!" << std::endl;
}

void 
StatusFlagCOOLBase::
dump(cool::ChannelSelection selection, std::string tag_name) {      
    try {
        cool::IObjectIteratorPtr objects = coolFolder->browseObjects(since, until-1, selection, tag_name);
        while (objects->goToNext()) {
            const cool::IObject& element = objects->currentRef();
            std::cout << element << std::endl;;
        }
    }
    catch (cool::Exception& e) {
        std::cout << "Unknown exception caught!" << e.what() << std::endl;
    }
}

std::string 
StatusFlagCOOLBase::
dumpField(cool::ChannelId channelId, std::string field, std::string tag_name) {      
    std::string result ="";
    try {
        cool::ChannelSelection selection = cool::ChannelSelection(channelId);
        cool::IObjectIteratorPtr objects = coolFolder->browseObjects(since, until-1, selection, tag_name);
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
StatusFlagCOOLBase::
dumpCode(std::string channelName, std::string tag_name) {
    std::string result = this->dumpField(this->getCoolFolder()->channelId(channelName.c_str()), "Code", tag_name);
    if (result == "") {
      return INT_MAX;
    } else {
      return atoi(result.c_str());
    }
}

void 
StatusFlagCOOLBase::
dumpall(std::string tag_name) {      
  this->dump(cool::ChannelSelection::all(), tag_name);
}

void
StatusFlagCOOLBase::
insert_helper(cool::ChannelId channelId, coral::AttributeList& payload,
	      std::string& tag_name) {
  if (tag_name=="HEAD") {
    coolFolder->storeObject(since, until, cool::Record(coolFolder->payloadSpecification(), payload), channelId);
  } else {
    coolFolder->storeObject(since, until, cool::Record(coolFolder->payloadSpecification(), payload), channelId, tag_name, true);
  }
}

cool::IFolderPtr 
StatusFlagCOOLBase::
getCoolFolder() {
    return this->coolFolder;
}

cool::IDatabasePtr 
StatusFlagCOOLBase::
getCoolDb() {
    return this->coolDb;
}


} //namespace dqutils
