/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// LisToCool.cxx
// Program to copy data from Lisbon MySQL DB to COOL conditions DB
// Richard Hawkings, started 12/1/06
// Compiles in offline cmt framework to binary executable, needs offline env

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "ConditionsDB/ICondDBMgr.h"
#include "ConditionsDB/ICondDBFolderMgr.h"
#include "ConditionsDB/CondDBMySQLMgrFactory.h"
#include "ConditionsDB/CondDBMySQLTableFactory.h"
#include "ConditionsDB/CondDBObjFactory.h"

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/IObject.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "SealBase/Time.h"

class LisToCool {
 public:
  LisToCool(const std::string& lisdb, const std::string& lisfolder,
	    const std::string& cooldb, const std::string& coolfolder,
	    const int mode, const int offset, const std::string& tagprefix,
            const int ichannel);
  int execute();

 private:
  bool getLisDB(const std::string lisdbstr, ICondDBMgr*& dbPtr,
                ICondDBDataAccess*& dbAcc, ICondDBFolderMgr*& dbFold,
                ICondDBTagMgr*& dbTag);
  static bool getCoolDB(const std::string& coolstr, cool::IDatabasePtr& dbPtr);
  bool extractFolderInfo(const std::string& folder) const;
  static cool::IFolderPtr getFolder(const std::string& folder,
    const cool::IRecordSpecification& atrspec, const std::string& desc,
    cool::FolderVersioning::Mode mode);
  void transSimpleTime(SimpleTime& ltime, cool::ValidityKey& ctime);
  void transTimes(SimpleTime lsince, SimpleTime ltill, 
		  cool::ValidityKey& csince, cool::ValidityKey& ctill);
  std::string strValidity(const cool::ValidityKey& key) const;
  void copyCondTable(bool compress);
  void copyBlobWithTags();

  // input parameters
  std::string m_lisdbstr;
  std::string m_lisfolder;
  std::string m_cooldbstr;
  std::string m_coolfolder;
  int m_mode;
  int m_timeoffset;
  std::string m_tagprefix;
  int m_channel;

  ICondDBMgr* m_lisdb{};
  ICondDBDataAccess* m_lisacc{};
  ICondDBFolderMgr* m_lisfold{};
  ICondDBTagMgr* m_listag{};
  cool::IDatabasePtr m_cooldb;

  std::string m_lisdesc;
  std::string m_cooldesc;
  bool m_runevent{};

};

LisToCool::LisToCool(const std::string& lisdb, const std::string& lisfolder,
	    const std::string& cooldb, const std::string& coolfolder,
	    const int mode, const int offset, const std::string& tagprefix,
		     const int ichannel) :
  m_lisdbstr(lisdb), m_lisfolder(lisfolder), m_cooldbstr(cooldb), 
  m_coolfolder(coolfolder),m_mode(mode),m_timeoffset(offset), 
  m_tagprefix(tagprefix),m_channel(ichannel)
{
  std::cout << "LisToCool XXXXXXXXinitialised" << std::endl;
  std::cout << "Read from Lisbon DB: " << m_lisdbstr << " folder " <<
    m_lisfolder << std::endl;
  std::cout << "Write to COOL DB: " << m_cooldbstr << " folder " <<
    m_coolfolder << std::endl;
  std::string_view smode="Undefined";
  if (m_mode==1) smode="CondDBTable";
  if (m_mode==11) smode="CondDBTable compressed";
  if (m_mode==2) smode="Blob with Tags";
  std::cout << "Mode: " << m_mode << " (" << smode << ")" << std::endl;
  std::cout << "Offset of " << m_timeoffset <<  
   " hours to be added when converting Lisbon timestamps to COOL" << std::endl;
  if (m_mode==2) std::cout << "Tag prefix for Blob tags: " << m_tagprefix;
}

int LisToCool::execute() {
  std::cout << std::endl << "Starting execution" << std::endl;
  // open Lisbon database
  if (getLisDB(m_lisdbstr,m_lisdb,m_lisacc,m_lisfold,m_listag)) {
    std::cout << "Lisbon MySQL database opened" << std::endl;
  } else {
    std::cout << "Lisbon MySQL database open failed" << std::endl;
    return 1;
  }
  // open COOL database
  if (getCoolDB(m_cooldbstr,m_cooldb)) {
    std::cout << "COOL database opened" << std::endl;
  } else {
    std::cout << "Database open fails" << std::endl;
    return 1;
  }
  if (m_mode==1) copyCondTable(false);
  if (m_mode==11) copyCondTable(true);
  if (m_mode==2) copyBlobWithTags();
  return 0;
}

static bool LisToCool::getLisDB(const std::string& lisdbstr, ICondDBMgr*&  /*dbPtr*/,
                     ICondDBDataAccess*&  /*dbAcc*/, ICondDBFolderMgr*&  /*dbFold*/,
                     ICondDBTagMgr*&  /*dbTag*/) {
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

bool LisToCool::getCoolDB(const std::string& coolstr,
    cool::IDatabasePtr& dbPtr) {
  std::cout << "Attempt to open COOL database with connection string: "
	    << coolstr << std::endl;
  cool::IDatabaseSvc& dbSvc=cool::DatabaseSvcFactory::databaseService();
  try {
    dbPtr=dbSvc.openDatabase(coolstr,false);
    return true;
  }
  catch (std::exception& e) {
    std::cout << "COOL exception caught: " << e.what() << std::endl;
    std::cout << "Try to create new conditions DB" << std::endl;
    try {
      dbPtr=dbSvc.createDatabase(coolstr);
      std::cout << "Creation succeeded" << std::endl;
      return true;
    }
    catch (std::exception& e) {
      std::cout << "Creation failed" << std::endl;
      return false;
    }
  }
  return false;
}

bool LisToCool::extractFolderInfo(const std::string& folder) const {
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
      std::cout << "Found CondDB folder " << folder << " with description string " << m_lisdesc << std::endl;
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

cool::IFolderPtr LisToCool::getFolder(const std::string& folder,
   const cool::IRecordSpecification& atrspec, const std::string& desc,
   cool::FolderVersioning::Mode mode) {
  if (m_cooldb->existsFolder(folder)) {
    std::cout << "Folder " << folder << " already exists in database" 
     << std::endl;
    return m_cooldb->getFolder(folder);
  } else {
    std::cout << "Folder " << folder << " not found - try to create it" << 
      std::endl;
    // create folder (single version), and create parents if needed     
    cool::IFolderPtr folderptr=m_cooldb->createFolder(folder,
	      cool::FolderSpecification(mode,atrspec),desc,true);
    std::cout << "Folder created with description string: " <<
      desc << std::endl;
    return folderptr;
  }
}

void LisToCool::transSimpleTime(SimpleTime&  /*ltime*/, cool::ValidityKey& ctime) const {
  if (ltime.isMinusInf()) {
    ctime=cool::ValidityKeyMin;
  } else if (ltime.isPlusInf()) {
    ctime=cool::ValidityKeyMax;
  } else {
    // adjust time - Lisbon in ns since 1900, COOL since 1970 (17 leap years)
    // offset built up in stages to avoid overflow
    long long offset=(70*365+17)*24;
    // add in correction (in hours) specified by user
    // this is ADDED to COOL time (subtraction of offset variable!)
    offset=(offset-m_timeoffset)*3600;
    offset=offset*1000*1000*1000;
    ctime=ltime-offset;
  }
}

void LisToCool::transTimes(SimpleTime  /*lsince*/, SimpleTime  /*ltill*/, 
		cool::ValidityKey& csince, cool::ValidityKey& ctill) const {
  if (m_runevent) {
    // copy straight over, same interpretation of 32bit run/32 bit event
    csince=lsince;
    ctill=ltill;
  } else {
    transSimpleTime(lsince,csince);
    transSimpleTime(ltill,ctill);
  }
}

std::string LisToCool::strValidity(const cool::ValidityKey& key) const {
  // return representation of validity key as a string
  std::ostringstream strs;
  if (m_runevent) {
    strs << "(" << (key >> 32) << "," << (key & 0xFFFFFFFF) << ")";
  } else {
    strs << seal::Time::Time(key).format(false,"%c");
  }
  return strs.str();
}


void LisToCool::copyCondTable(bool compress) {
  std::cout << "Copy CondTable " << m_lisfolder << " to " << m_coolfolder <<
    std::endl;
  if (compress) std::cout << 
   "Data assumed to be in ascending time order and will be zero-suppressed"
			  << std::endl;
  // set folder info, check for folder existance
  if (!extractFolderInfo(m_lisfolder)) return;

  // list of new column names for multichannel (need to override default
  // from each folder)
  std::vector<std::string> newcolname;
  // set folder description string for single channel folder in Athena
  std::string_view timestr;
  if (m_runevent) {
    timestr="<timeStamp>run-event</timeStamp>";
  } else {
    timestr="<timeStamp>time</timeStamp>";
  }
  if (m_channel==-1) {
    m_cooldesc=std::string(timestr);
    m_cooldesc+="<addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";
  } else {
    m_cooldesc=std::string(timestr);
    m_cooldesc+="<addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
    std::cout << "Data will be written in COOL channel " << m_channel 
	      << std::endl;
    // setup list of column names from tagprefix string (reused)
    // interpreted as column names separated by commas
    std::string::size_type p1,p2;
    p1=0;
    while (p1!=std::string::npos && p1<m_tagprefix.size()) {
      p2=m_tagprefix.find(',',p1+1);
      newcolname.push_back(m_tagprefix.substr(p1,p2-p1));
      if (p2!=std::string::npos) {
	p1=p2+1;
      } else {
	p1=std::string::npos;
      }
    }
  }

  bool foldermade=false;
  cool::IFolderPtr folderptr;

  // loop over objects in table
  ICondDBTable* table=CondDBMySQLTableFactory::createCondDBTable();
  m_lisdb->startRead();
  SimpleTime time1=SimpleTime();
  time1.setMinusInf();
  SimpleTime time2=SimpleTime();
  time2.setPlusInf();
  m_lisacc->browseHistory(m_lisfolder,table,time1,time2);
  int ncol=table->getNumColumns();
  int nrow=table->getNumRows();
  std::cout << "Table has " << ncol << " columns and " << nrow << " rows"
	    << std::endl;
  // set up attributelist spec and COOL folder if needed
  cool::RecordSpecification atrspec;
  std::vector<std::string> columnname;
  std::vector<ICondDBTable::cdb_types> columntype;
  table->getNames(columnname);
  table->getTypes(columntype);
  // construct attribute list spec from column names
  std::vector<cool::StorageType::TypeId> cooltype(ncol);
  std::vector<int> coolarray(ncol);
  for (int icol=0;icol<ncol;++icol) {
    // map CondDB typenames to COOL equivalents
    coolarray[icol]=false;
    switch (columntype[icol]) {
      // simple types
      case ICondDBTable::cdbBool: cooltype[icol]=cool::StorageType::Bool; break;
      case ICondDBTable::cdbInt: cooltype[icol]=cool::StorageType::Int32; break;
      case ICondDBTable::cdbFloat: cooltype[icol]=cool::StorageType::Float; break;
      case ICondDBTable::cdbString: cooltype[icol]=cool::StorageType::String4k; break;
      case ICondDBTable::cdbDouble: cooltype[icol]=cool::StorageType::Float; break;
      case ICondDBTable::cdbLongLong: cooltype[icol]=cool::StorageType::Int32; break;
      // array types
      case ICondDBTable::cdbArrayBool: 
         cooltype[icol]=cool::StorageType::Bool;coolarray[icol]=true; break;
      case ICondDBTable::cdbArrayInt: 
         cooltype[icol]=cool::StorageType::Int32;coolarray[icol]=true; break;
      case ICondDBTable::cdbArrayFloat: 
         cooltype[icol]=cool::StorageType::Float;coolarray[icol]=true; break;
      case ICondDBTable::cdbArrayString: 
         cooltype[icol]=cool::StorageType::String4k; coolarray[icol]=true; break;
      case ICondDBTable::cdbArrayDouble: 
         cooltype[icol]=cool::StorageType::Float;coolarray[icol]=true; break;
      case ICondDBTable::cdbArrayLongLong: 
         cooltype[icol]=cool::StorageType::Int64;coolarray[icol]=true; break;
      // placeholder for undefined type - we don't use UChar
      default: cooltype[icol]=cool::StorageType::UChar;
    }
    std::cout << "Column " << icol << " name: " << columnname[icol] << 
      " Lisbon type " << columntype[icol] << " COOLtype: " << 
       cooltype[icol] << std::endl;
    // add to attributelist
    if (cooltype[icol]!=cool::StorageType::UChar) {
      if (m_channel>-1) {
	columnname[icol]=newcolname[icol];
	std::cout << "Column " << icol << " renamed to " << newcolname[icol] 
		  << std::endl;
      }
      atrspec.extend(columnname[icol],cooltype[icol]);
    } else {
      std::cout << "WARNING: Column " << icol << " will be skipped" 
		<< std::endl;
    }
  }
  // define folder itself
  try {
    folderptr=getFolder(m_coolfolder,atrspec,m_cooldesc,
              cool::FolderVersioning::SINGLE_VERSION);
    folderptr->setupStorageBuffer();
    foldermade=true;
  }
  catch (std::exception& e) {
    std::cout << "ERROR: Cannot create COOL folder, exception: "
	      << e.what() << std::endl;
    return;
  }

  // loop over all objects in table
  int nobj=0;
  int nzsup=0;
  cool::ValidityKey osince;
  cool::Record opayload;
  bool isold=false;

  for (unsigned int irow=0;irow<nrow;++irow) {
    SimpleTime lsince,ltill;
    table->getSinceTime(irow,lsince);
    table->getTillTime(irow,ltill);
    cool::ValidityKey csince,ctill;
    transTimes(lsince,ltill,csince,ctill);
    // skip intervals where since=till as these are not allowed by COOL
    if (foldermade && ctill>csince) {
      cool::Record payload(atrspec);
      // get data for row into temporary buffer
      std::vector<std::string> buf;
      table->getRow(irow,buf);
       if (buf[0]!="NULL") {
	try {
         long pos=0;
         for (unsigned int icol=0;icol<ncol;++icol) {
	 int nvals;
	 // check for array data - if so first buffer entry=number of elements
	 if (ICondDBTable::cdbLongLong<columntype[icol]) {
	   nvals=atol(buf[pos].c_str());
	   std::cout << "Column " << columnname[icol] << " is array with " <<
	     nvals << " entries - all but 1 skipped" << std::endl;
	   ++pos;
	 } else {
	   nvals=1;
	 }
	 // only take the first element in case of arrays
	 // std::cout << "Column " << cooltype[icol] << " pos " << pos << 
	 //  " value " << buf[pos] << std::endl;
	 if (cooltype[icol]==cool::StorageType::Bool) {
           payload[columnname[icol]].setValue((atoi(buf[pos].c_str())!=0));
	 } else if (cooltype[icol]==cool::StorageType::Int32) {
	   payload[columnname[icol]].setValue(atoi(buf[pos].c_str()));
	 } else if (cooltype[icol]==cool::StorageType::Float) {
	   payload[columnname[icol]].setValue(static_cast<float>
					     (atof(buf[pos].c_str())));
	 }  else if (cooltype[icol]==cool::StorageType::Double) {
	   // converting as float
	   payload[columnname[icol]].setValue(static_cast<float>
					     (atof(buf[pos].c_str())));
	 } else if (cooltype[icol]==cool::StorageType::String4k) {
	   payload[columnname[icol]].setValue(buf[pos]);
	 }
   	 pos+=nvals;
        }
	// std::ostringstream atr;
	// payload.print(atr);
	// std::cout << "Object to store " << atr.str() << std::endl;
        // store object
	// possibly skip
	bool skip=false;
	if (compress && isold) {
	  // check time sequence
	  if (csince<osince) {
	    std::cout << "ERROR in since time sequences" << osince << ","
		      << csince << std::endl;
	    return;
	  }
	  // check if payloads are equal
	  if (payload==opayload) {
	     skip=true;
	     ++nzsup;
	  }
	}
	if (!skip) {
	  // for compressed folders, used open-ended intervals
	  if (compress) ctill=cool::ValidityKeyMax;
          // default channel
          int ichan=0;
	  if (m_channel>-1) ichan=m_channel;
          folderptr->storeObject(csince,ctill,payload,ichan);
	  // print payload for debugging
	  std::ostringstream attr;
	  payload.attributeList().toOutputStream(attr);
	  std::cout << "Insert [" << csince << "," << ctill << "] " << ichan
		    << attr.str() << std::endl;
	  // save values
	  opayload=payload;
	  osince=csince;
	  isold=true;
	}
         ++nobj;
	}
        catch (std::exception& e) {
	   std::cout << "Exception caught in payload creation/buffer insert: "
		   << e.what() << std::endl;
        }
      } else {
	std::cout << "Row skipped because of NULL values" << std::endl;
      }
    }
  }
  m_lisdb->commit();
  CondDBMySQLTableFactory::destroyCondDBTable(table);
  // write data to COOL
  try {
    folderptr->flushStorageBuffer();
    std::cout << "Written " << nobj << " objects to COOL, of which " << nzsup 
	      <<" zero-suppressed away"  << std::endl;
  }
  catch (std::exception& e) {
    std::cout << "ERROR: Exception from bulk insert: " << e.what() <<
      std::endl;
  }
  std::cout << "All data copied" << std::endl;
}

void LisToCool::copyBlobWithTags() {
  std::cout << "Copy Blob with Tags " << m_lisfolder << " to " << m_coolfolder
 <<  std::endl;

  // set folder info, check for folder existance
  if (!extractFolderInfo(m_lisfolder)) return;
  // COOL folder description is same as for Lisbon
  m_cooldesc=m_lisdesc;

  bool foldermade=false;
  cool::IFolderPtr folderptr;

  // get list of tags
  // note that the HEAD tag is not included in the list and so will not
  // be copied
  std::vector<std::string> tags;
  m_listag->getCondDBTag(m_lisfolder,tags);
  std::cout << "Folder has a total of " << tags.size() << " defined tags"
	    << std::endl;
  // if there are no tags, do copy the HEAD
  if (tags.empty()) {
    tags.push_back("HEAD");
    std::cout << "HEAD tag will be copied" << std::endl;
  }

  // loop over tags
  for (std::vector<std::string>::const_iterator tagitr=tags.begin();
       tagitr!=tags.end();++tagitr) {
    std::cout << "Beginning copy for tag: " << *tagitr << std::endl;
    // loop over objects in tag
    ICondDBDataIterator* oCondIt = 0;
    m_lisdb->startRead();
    if (*tagitr=="HEAD") {
       m_lisacc->browseHistory(oCondIt,m_lisfolder,
			       CondDBminusInf,CondDBplusInf);
    } else {
      m_lisacc->browseObjectsInTag(oCondIt,m_lisfolder,*tagitr);
    }
    std::cout << "Retrieved data from Lisbon CondDBMySQL" << std::endl;
    int nobj=0;
    // folder is POOLref - single string column named PoolRef
    cool::RecordSpecification atrspec;
    atrspec.extend("PoolRef",cool::StorageType::String4k);

    // loop over all objects in iterator
    ICondDBObject* obj=0;
    if (oCondIt!=0) {
      do {
        obj=oCondIt->current();
	if (!foldermade) {
	  // setup COOL folder if needed and init storage buffer
	  try {
  	    folderptr=getFolder(m_coolfolder,atrspec,m_cooldesc,
              cool::FolderVersioning::MULTI_VERSION);
	    folderptr->setupStorageBuffer();
	    foldermade=true;
	  }
	  catch (std::exception& e) {
	    std::cout << "ERROR: Cannot create COOL folder, exception: "
		      << e.what() << std::endl;
	    return;
	  }
	} 

        // copy data row to COOL, only if folder was initialised
	if (foldermade) {
          CondDBKey lsince(obj->validSince() );
          CondDBKey ltill(obj->validTill() );
  	  cool::ValidityKey csince,ctill;
  	  transTimes(lsince,ltill,csince,ctill);
	  cool::Record payload(atrspec);
	  std::string data;
          obj->data(data);
          payload["PoolRef"].setValue(data);
	  // store object - always into channel 0
	  folderptr->storeObject(csince,ctill,payload,0);
	  ++nobj;
	  std::cout << "Insert object for [" << strValidity(csince) << ","
		    << strValidity(ctill) << "]" << " payload " << data << 
                     std::endl;
	  delete obj;
	}
      } while (oCondIt->next());
    }
    try {
      folderptr->flushStorageBuffer();
      std::cout << "Written " << nobj << " objects to COOL" << std::endl;
      std::string newtag="";
      if (m_tagprefix!="") newtag+=m_tagprefix+"_";
      newtag+=*tagitr;
      folderptr->tagCurrentHead(newtag,"LisToCool import");
      std::cout << "Tagged with tag " << newtag << std::endl;
    }
    catch (std::exception& e) {
      std::cout << "ERROR: Exception from bulk insert: " << e.what() << 
	std::endl;
    }
    delete oCondIt;
    m_lisdb->commit();
  }
  std::cout << "All data copied" << std::endl;
}

int main(int argc, const char* argv[]) {
  if (argc<6) {
    std::cout << "Syntax: LisToCool.exe <LisbonDBconnection> <lisbon_folder> <coolDBConnection> <cool_folder> <mode> [<time_offset>] [<tag_prefix>] [channel]"
    << std::endl;
      std::cout << "<mode>=1 for CondDBTable, 2 for Blob with Tags" << 
      std::endl;
    return 1;
  }
  int ioffset=0;
  if (argc>6) ioffset=atoi(argv[6]);
  std::string tagprefix="";
  if (argc>7) tagprefix=argv[7];
  int ichannel=-1;
  if (argc>8) ichannel=atoi(argv[8]);
  LisToCool convert(argv[1],argv[2],argv[3],argv[4],atoi(argv[5]),
		    ioffset,tagprefix,ichannel);
  return convert.execute();
}
