/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "PixelCoralClientUtils/PixCalibCoralCoolDb.h"

// CoraCool stuff -- A.X.
#include "CoraCool/CoraCoolDatabaseSvcFactory.h"
#include "CoraCool/CoraCoolDatabaseSvc.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoraCool/CoraCoolFolder.h"
#include "CoraCool/CoraCoolObjectIter.h"
#include "CoraCool/CoraCoolObject.h"

#include "CxxUtils/checker_macros.h"

// std lib
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

//
// this code is based on CoraCoolExample by R. Hawkings
//
PixCalibCoralCoolDb::PixCalibCoralCoolDb(const std::string& dbString,
                                         int /*verbose*/) :
  m_dbstring(dbString)
{

  CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();

  //std::cout << "Done the CoraCool initialisation" << std::endl;

  // open the CORAL database connection which implicitly opens
  // the corresponding COOL connection
  // you can also pass in the COOL connection explicitly
  // std::cout << "Opening CORAL database" << std::endl;
  try {
    m_coradb=corasvc.openDatabase(m_dbstring,false);
    //std::cout << "Database connections open OK" << std::endl;
  }
  catch (std::exception&e) {
    std::cout << "Problem opening CORAL database: " << e.what() << std::endl;
  }
  //std::cout << "Done the database opening" << std::endl;

}

PixCalibCoralCoolDb::~PixCalibCoralCoolDb()
{
}

bool PixCalibCoralCoolDb::init()
{
  // open the database connection, move to constructor
  //  if (m_verbose) cout << "HEY HEY Connecting to " << m_dbstring << endl;
  //  CoraCoolDatabaseSvc& corasvc=CoraCoolDatabaseSvcFactory::databaseService();
  //  try {
  //    m_coradb = corasvc.openDatabase(m_dbstring, readonly);
  //  }
  //  catch (std::exception& e) {
  //    cout << "problem opening database: " << e.what() << endl;
  //    return false;
  //  }
  //  if (m_verbose) cout << "Connection established" << endl;

  return true;
}

bool PixCalibCoralCoolDb::load(cool::ValidityKey vkx)
{
  CoraCoolFolderPtr folder;
  string folderName("/PIXEL/PixCalibData");

  try {
    folder = m_coradb->getFolder(folderName);
  }
  catch (cool::Exception& e) {
    cout << "problem getting folder: " << e.what() << endl;
    return false;
  }

  // open text file for output
  ofstream out("pcd.sav");
  int nload = 0;

  CoraCoolObjectIterPtr itr = folder->browseObjects(vkx, cool::ChannelSelection::all());
  while (itr->hasNext()) {
    CoraCoolObjectPtr obj = itr->next();
    for (CoraCoolObject::const_iterator payitr = obj->begin();
	 payitr != obj->end(); ++payitr) {
      const coral::Blob& blob = (*payitr)["data"].data<coral::Blob>();
      const char* p = static_cast<const char*>(blob.startingAddress());
      unsigned int len = blob.size();
      for (unsigned int i = 0; i!=len; ++i) out << *p++;
    }
    ++nload;
  }
  out.close();
  cout << "loaded " << nload << " channels" << endl;

  return true;
}

bool PixCalibCoralCoolDb::saveCalibData ( string textfile , long long FK )
{
  cool::RecordSpecification payloadspec;
  // primary / foreign keys
  payloadspec.extend("PrimKey",cool::StorageType::Int32);
  payloadspec.extend("ForeignKey",cool::StorageType::Int64);
  // other data members
  payloadspec.extend("data",cool::StorageType::Blob64k);

  cool::RecordSpecification fkspec;
  fkspec.extend("CoolKey",cool::StorageType::Int64);

  string desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>AthenaAttributeList</typeName>";

  string folderName("/PIXEL/PixCalibData");
  string cfolderName("PixCalibData");

  CoraCoolFolderPtr folder;

  // delete existing folder, if any
  bool do_delete = false;
  if (do_delete && m_coradb->existsFolder(folderName)) {
    try {
      m_coradb->deleteFolder(folderName);
    }
    catch (cool::Exception& e) {
      cout << "problem deleting folder: " << e.what() << endl;
      return false;
    }
  }

  // create new folder or get the old one, if any
  if (!m_coradb->existsFolder(folderName)) {
    try {
      folder = m_coradb->createFolder(folderName, cfolderName,
	fkspec, payloadspec, "ForeignKey", "PrimKey", desc,
	cool::FolderVersioning::MULTI_VERSION, true);
    }
    catch (cool::Exception& e) {
      cout << "problem creating folder: " << e.what() << endl;
      return false;
    }
  } else {
    folder = m_coradb->getFolder(folderName);
  }

  std::cout << "Succesfully opened/created folders!" << std::endl;
 
  // open text file for input
  ifstream in(textfile.c_str() );
  if( !(in.is_open() ) ){
    cout << "calib-data file: " << textfile << " not found!" << endl;
    return false;
  }  
  //return error if textfile not found!

  string s;
  int nsave = 0;
  
  string dataString;
  while (in) {
    getline(in,s);
    if (in.eof()) break;
    dataString += s + '\n';
    
    for (int i = 0; i<16; ++i) {
      getline(in,s);
      dataString += s + '\n';
    }
    ++nsave;
  }
  cout << "read " << nsave << " channels" << endl;

  // prepare data to save in the payload
  // No sharing, ok.
  coral::AttributeList data ATLAS_THREAD_SAFE = folder->emptyAttrList();
  data["PrimKey"].data<int>() = 0;
  data["ForeignKey"].data<long long>() = FK;
  coral::Blob& blob = data["data"].data<coral::Blob>();
  unsigned int len = dataString.size();
  blob.resize(len);
  char* p = static_cast<char*>(blob.startingAddress());
  for (unsigned int i = 0; i!=len; ++i) *p++ = dataString[i];
 
    vector<coral::AttributeList> datavec;
    datavec.reserve(1);
    datavec.push_back(data);
    
    try {
      //store object and range of run-number
      //folder->storeObject(vk1, vk2, datavec.begin(), datavec.end(), chan);
    
      //just add payload
      folder->addPayload(datavec.begin(), datavec.end());
    }
    catch (cool::Exception& e) {
      cout << "problem writing data: " << e.what() << endl;
      return false;
    }

  return true;
}

bool PixCalibCoralCoolDb::referenceToRunInterval(long long FK, cool::ValidityKey vk1,cool::ValidityKey vk2, const std::string& tagname)
{
  cool::RecordSpecification payloadspec;
  // primary / foreign keys
  payloadspec.extend("CoolKey",cool::StorageType::Int64);
  payloadspec.extend("PrimKey",cool::StorageType::Int32);
  // payloadspec.extend("ModuleID",cool::StorageType::Int64);
  payloadspec.extend("data",cool::StorageType::String16M);

  cool::RecordSpecification fkspec;
  fkspec.extend("CoolKey",cool::StorageType::Int64);

  string desc="<timeStamp>run-lumi</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>AthenaAttributeList</typeName>";

  string folderName("/PIXEL/PixCalibData");
  string cfolderName("PixCalibData");

  CoraCoolFolderPtr folder;

  // delete existing folder, if any
  bool do_delete = false;
  if (do_delete && m_coradb->existsFolder(folderName)) {
    try {
      m_coradb->deleteFolder(folderName);
    }
    catch (cool::Exception& e) {
      cout << "problem deleting folder: " << e.what() << endl;
      return false;
    }
  }

  // create new folder or get the old one, if any
  if (!m_coradb->existsFolder(folderName)) {
    try {
      folder = m_coradb->createFolder(folderName, cfolderName,
	fkspec, payloadspec, "ForeignKey", "PrimKey", desc,
	cool::FolderVersioning::MULTI_VERSION, true);
    }
    catch (cool::Exception& e) {
      cout << "problem creating folder: " << e.what() << endl;
      return false;
    }
  } else {
    folder = m_coradb->getFolder(folderName);
  }

  //set the run-interval for the given foreign-key...
  cool::ChannelId chan(0);
  try {
    folder->referenceObject(vk1, vk2, FK, chan, tagname);
  }
  catch (cool::Exception& e) {
    cout << "problem when associating run-interval : " << e.what() << endl;
    return false;
  }
  
  cout << "saved runInterval (FK=" << FK << ") " << endl;

  return true;
}
