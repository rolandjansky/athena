/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LisIOVList.exe
// Lists tags and IOVs in Lisbon database
// Walter Lampl based LisToCool.exe by Richard Hawkings
// Compiles in offline cmt framework to binary executable, needs offline env

//Example commmand string:
//LisIOVList.exe "atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions" "/lar/LArElecCalibTB04/LArRamp" testfile tag


#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdint.h>

#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/ICondDBFolderMgr.h"
#include "ConditionsDB/CondDBMySQLMgrFactory.h"
#include "ConditionsDB/CondDBMySQLTableFactory.h"
#include "ConditionsDB/CondDBObjFactory.h"

class LisIOVList {
 public:
  LisIOVList(const std::string& lisdb, const std::string& lisfolder,
	    const std::string& filename, const std::string& tagprefix);
  int execute();

 private:
  bool getLisDB(const std::string& lisdbstr, ICondDBMgr*& dbPtr,
                ICondDBDataAccess*& dbAcc, ICondDBFolderMgr*& dbFold,
                ICondDBTagMgr*& dbTag);
  bool extractFolderInfo();//const std::string folder);
  void dumpCondTable();

  // input parameters
  std::string m_lisdbstr;
  std::string m_lisfolder;
  std::string m_filename;
  std::string m_tagprefix;
  
  ICondDBMgr* m_lisdb;
  ICondDBDataAccess* m_lisacc;
  ICondDBFolderMgr* m_lisfold;
  ICondDBTagMgr* m_listag;

  std::string m_lisdesc;
  bool m_runevent;

};

LisIOVList::LisIOVList(const std::string& lisdb, const std::string& lisfolder, 
		     const std::string& filename, const std::string& tagprefix) :
  m_lisdbstr(lisdb), m_lisfolder(lisfolder), m_filename(filename), m_tagprefix(tagprefix),
  m_lisdb(nullptr),
  m_lisacc(nullptr),
  m_lisfold(nullptr),
  m_listag(nullptr),
  m_runevent(false)
{
  std::cout << "LisIOVList initialised" << std::endl;
  std::cout << "Read from Lisbon DB: " << m_lisdbstr << " folder " <<
    m_lisfolder << std::endl;
  if (m_filename.size()==0)
    std::cout << "No output file given." << std::endl;
  else
    std::cout << "Write to file " << m_filename << std::endl;
  if (m_tagprefix.size()==0)
    std::cout << "No tag given." << std::endl;
  else
    std::cout << "Tag: " << m_tagprefix << std::endl;
}

int LisIOVList::execute() {
  std::cout << std::endl << "Starting execution" << std::endl;
  // open Lisbon database
  if (getLisDB(m_lisdbstr,m_lisdb,m_lisacc,m_lisfold,m_listag)) {
    std::cout << "Lisbon MySQL database opened" << std::endl;
  } else {
    std::cout << "Lisbon MySQL database open failed" << std::endl;
    return 1;
  }
  dumpCondTable();
  return 0; 
}

bool LisIOVList::getLisDB(const std::string& lisdbstr, ICondDBMgr*& dbPtr,
                     ICondDBDataAccess*& dbAcc, ICondDBFolderMgr*& dbFold,
                     ICondDBTagMgr*& dbTag) {
  dbPtr=CondDBMySQLMgrFactory::createCondDBMgr();
  try {
    dbPtr->init(lisdbstr);
    dbPtr->startRead();
    dbPtr->openDatabase();
    dbPtr->commit();
    dbAcc=dbPtr->getCondDBDataAccess();
    dbFold=dbPtr->getCondDBFolderMgr();
    dbTag=dbPtr->getCondDBTagMgr();
    return true;
  }
  catch (CondDBException &e) {
    std::cout << "Lisbon CondDB exception caught: " <<  e.getMessage() << 
      "\n error code: " << e.getErrorCode() << std::endl;
    return false;
  }
}

bool LisIOVList::extractFolderInfo(){
  // extract folder description string from Lisbon DB folder
  // and interpret to set run/event or timestamp flag
  // return true if OK, false if folder not existing
  try {
    m_lisdb->startRead();
    ICondDBFolder* pfolder;
    m_lisfold->getCondDBFolder(m_lisfolder,pfolder);
    m_lisdb->commit();
    if (pfolder!=0) {
      m_lisdesc=pfolder->getDescription();
      //std::cout << "Found CondDB folder " << folder << " with description string " << m_lisdesc << std::endl;
    } else {
      std::cout << "Null folder pointer - folder does not exist?" << std::endl;
      return false;
    }
  }
  catch (CondDBException &e) {
    std::cout << "Error retrieving CondDB folder: " << e.getMessage() << 
     std::endl;
    return false;
  }
  // set run/event flag based on presence of key string (timestamp is default)
  m_runevent=
    (m_lisdesc.find("<timeStamp>run-event</timeStamp>")!=std::string::npos);
  std::cout << "Time format is: ";
  if (m_runevent) {
    std::cout << "run-event" << std::endl;
  } else {
    std::cout << "Timestamp" << std::endl;
  }
  return true;
}

//========================================================

void LisIOVList::dumpCondTable() {
  std::cout << "Dump CondTable " << m_lisfolder << std::endl;
  // set folder info, check for folder existance
  if (!extractFolderInfo()) return;
	    
  std::ofstream outfile;
  if (m_filename.size()) {
    outfile.open(m_filename.c_str());
    if (!outfile.is_open())
      std::cout << "Could not open file " << m_filename << " for writing" << std::endl;
  }
  std::vector<std::string> tags;
  m_listag->getCondDBTag(m_lisfolder,tags);
  std::cout << "Folder has a total of " << tags.size() << " defined tags" << std::endl;

  // loop over tags
  for (std::vector<std::string>::const_iterator tagitr=tags.begin();
       tagitr!=tags.end();++tagitr) {
    //std::cout << "Tag: " << *tagitr << std::endl;
    // loop over objects in tag
    ICondDBDataIterator* oCondIt = 0;
    m_lisdb->startRead();
    if (*tagitr=="HEAD") {
       m_lisacc->browseHistory(oCondIt,m_lisfolder,
			       CondDBminusInf,CondDBplusInf);
    } else {
      m_lisacc->browseObjectsInTag(oCondIt,m_lisfolder,*tagitr);
    }
    //std::cout << "Reading data from Lisbon CondDBMySQL" << std::endl;
    int nobj=0;
    // loop over all objects in iterator
    ICondDBObject* obj=0;
    if (oCondIt!=0) {
      do {
        obj=oCondIt->current();
	CondDBKey lsince(obj->validSince() );
	CondDBKey ltill(obj->validTill() );
	nobj++;
	//Stolen from AthenaKernel/IOVTime.h
	//inline uint32_t run() const { return m_time>>32; }
	//inline uint32_t event() const { return m_time & 0xFFFFFFFF; }
	uint32_t run_since=lsince>>32;
	uint32_t evt_since=lsince & 0xFFFFFFFF;
	uint32_t run_till=ltill>>32;
	uint32_t evt_till=ltill & 0xFFFFFFFF;
	//std::cout << "Timestamps: From " << lsince << " to " << ltill << std::endl;
	//std::cout << "R/E: From " << run_since << "/" << evt_since << " to " << run_till << "/" << evt_till << std::endl;
	if (outfile.is_open()) 
	  if (m_tagprefix.size()==0 || m_tagprefix==*tagitr) {
	    //std::cout << "Writing to file... [" << *tagitr << "]"<< std::endl;
	    outfile << run_since << "," << evt_since << "," << run_till << "," << evt_till;
	    if (m_tagprefix.size()==0) 
	      outfile << "," << *tagitr << std::endl;
	    else
	      outfile << std::endl;
	  }
      }
      while(oCondIt->next());
    }
    std::cout << "Tag "<< *tagitr << ": " << nobj << " IOVs" << std::endl;
      //std::cout << "Found "<< nobj <<" IOVs "<< " for tag "<< *tagitr << std::endl;
  }
  if (outfile.is_open()) 
    outfile.close();

  return;
}



int main(int argc, const char* argv[]) {
  if (argc<3) {
    std::cout << "Syntax: ListIOVList.exe <LisbonDBconnection> <lisbon_folder> [<filename>] [<tag>]"
    << std::endl;
    return 1;
  }
  std::string filename;
  std::string tag;
  
  if (argc>3)
    filename=argv[3];
  if (argc>4)
    tag=argv[4];
  LisIOVList convert(argv[1],argv[2],filename,tag);
  return convert.execute();
}
