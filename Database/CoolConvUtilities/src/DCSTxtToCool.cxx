/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// DCSTxtToCool.cxx
// Program to upload text file DCS data to COOL conditons database
// Text files produced by Jim Cook's utility to dump from PVSS local archive
// Richard Hawkings, started 10/1/06
// Compiles in offline cmt framework to binary executable, needs offline env

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>

#include "CoolKernel/DatabaseId.h"
#include "CoolKernel/Exception.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/IObject.h"
#include "CoolKernel/Record.h"
#include "CoolApplication/DatabaseSvcFactory.h"

#include "DataPointInfo.h"
#include "SealBase/Time.h"

class DCSTxtToCool {
 public:
  DCSTxtToCool(const std::string cooldb, const std::string configfile,
	       const std::string datafile, const int offset);
  int execute();

 private:
  bool getCoolDB(const std::string coolst, cool::IDatabasePtr& dbPtr);
  void readMap(const std::string configfile);
  void beginDataPoint(std::string folder);
  void storeDataPoint(cool::ValidityKey since, float value);
  void flushBuffer();
  cool::IFolderPtr getFolder(const std::string folder,
			     const cool::IRecordSpecification& atrspec);

  std::string m_coolstr;
  std::string m_configfile;
  std::string m_datafile;
  int m_timeoffset;

  cool::IDatabasePtr m_coolDb;
  int m_ndata;
  std::string m_datap;
  cool::IFolderPtr m_folderp;
  DataPointInfo* m_dpinfo;
  cool::ValidityKey m_vkmin;
  cool::ValidityKey m_vkmax;
  typedef std::map<std::string, DataPointInfo*> DPMap;
  DPMap m_dpmap;
  std::vector<std::string> m_folderlist;
  std::vector<int> m_folderchan;
};

DCSTxtToCool::DCSTxtToCool(const std::string cooldb, 
  const std::string configfile, const std::string datafile, const int offset) :
  m_coolstr(cooldb), m_configfile(configfile), m_datafile(datafile),
  m_timeoffset(offset),
  m_ndata(0), m_datap(""), m_vkmin(cool::ValidityKeyMax), m_vkmax(0) {
  std::cout << "Read data into COOL db: " << m_coolstr << std::endl;
  std::cout << "Datapoint configuration file: " << m_configfile << std::endl;
  std::cout << "Data file: " << m_datafile << std::endl;
  std::cout << "Time offset " << m_timeoffset << " hours" << std::endl;
}

int DCSTxtToCool::execute() {
  // open COOL database
  if (getCoolDB(m_coolstr,m_coolDb)) {
    std::cout << "COOL database opened" << std::endl;
  } else {
    std::cout << "Database open fails" << std::endl;
    return 1;
  }
  // read datapoint mapping
  readMap(m_configfile);
  // loop over input lines
  std::ifstream infile;
  int nobj=0;
  std::vector<char> buffer(999);
  infile.open(m_datafile.c_str());
  while (infile.getline(&buffer[0],999,'\n')) {
    std::string sline(buffer.begin(),buffer.begin()+strlen(&buffer[0]));
    // parse line with assumptions
    // if string begins with '200' (e.g. 2006) assume date, hence datapoint
    if (sline.substr(0,3)=="200") {
      // parse the string as three fields - date, time, value (float)
      std::string sdate,stime;
      float value;
      std::istringstream sbuf(&buffer[0]);
      sbuf >> sdate >> stime >> value;
      int year,month,day,hour,min,sec;
      long long frac;
      year=atoi(sdate.substr(0,4).c_str());
      // note -1 as month is returned by timefunctions in range 0-11
      month=atoi(sdate.substr(5,2).c_str())-1;
      day=atoi(sdate.substr(8,2).c_str());
      hour=atoi(stime.substr(0,2).c_str());
      min=atoi(stime.substr(3,2).c_str());
      sec=atoi(stime.substr(6,2).c_str());
      frac=atoi(stime.substr(9,3).c_str())*1E6;
      seal::Time sincet=seal::Time(year,month,day,
					       hour,min,sec,frac,false);
      seal::TimeSpan tofs=seal::TimeSpan(3600*m_timeoffset,0);
      sincet=sincet+tofs;
      storeDataPoint(sincet.ns(),value);
    } else if (sline.size()>2) {
      // if size >2 assume a new datapoint with this name
      beginDataPoint(sline);
    }
    ++nobj;
  }
  // write last data
  if (m_ndata>0) flushBuffer();
  std::cout << "Read total of " << nobj << " lines from data file" << std::endl;
  return 0;
}

void DCSTxtToCool::beginDataPoint(std::string datap) {
  if (m_ndata>0) flushBuffer();
  // strip trailing linefeeds
  while (datap.substr(datap.size()-1,1)=="\r") 
    datap=datap.substr(0,datap.size()-1);
  // lookup datapoint to folder mapping
  std::cout << "Begin processing datapoint " << datap << std::endl;
  std::string key0=m_dpmap.begin()->first;
  DPMap::const_iterator fitr=m_dpmap.find(datap);
  if (fitr!=m_dpmap.end()) {
    m_dpinfo=fitr->second;
    m_datap=datap;
    std::cout << "Associated datapoint to folder " << m_dpinfo->folder() << 
      " channel " << m_dpinfo->channel() << std::endl;
    // now get pointer to COOL folder, creating if needed
    try {
      m_folderp=getFolder(m_dpinfo->folder(),m_dpinfo->atrspec());
    }
    catch (std::exception& e) {
    // if folder creation fails, don't let data be stored
      std::cout << "No valid folder to store data: " << e.what() 
      << std::endl;
      m_dpinfo=0;
    }
  } else {
    std::cout << "ERROR: No mapping defined for datapoint " << datap << 
      std::endl;
    m_dpinfo=0;
  }
}

void DCSTxtToCool::storeDataPoint(cool::ValidityKey since, float value) {
  // if no valid folder mapping defined, skip storage
  if (m_dpinfo==0) return;
  if (m_ndata==0) m_folderp->setupStorageBuffer();
  cool::Record payload(m_dpinfo->atrspec());
  payload[m_dpinfo->column()].setValue(value);
  m_folderp->
    storeObject(since,cool::ValidityKeyMax,payload,m_dpinfo->channel());
  ++m_ndata;
  // keep track of min/max times for printout
  if (since<m_vkmin) m_vkmin=since;
  if (since>m_vkmax) m_vkmax=since;
}

void DCSTxtToCool::flushBuffer() {
  // if no data, nothing to do
  if (m_ndata==0) return;
  if (m_datap=="") {
    std::cout << "Attempt to write " << m_ndata << " data points but no folder"
	      << std::endl;
    return;
  }
  std::cout << "Write " << m_ndata << " values [" <<
    seal::Time(m_vkmin).format(false,"%c") << ", " <<
    seal::Time(m_vkmax).format(false,"%c") << "] from datapoint "
	    << m_datap << " to folder " << m_dpinfo->folder() << " channel "
	    << m_dpinfo->channel() << std::endl;
  try {
    m_folderp->flushStorageBuffer();
  }
  catch (std::exception& e ) {
    std::cout << "Bulk insertion failed for folder " <<
      m_dpinfo->folder() << " channel " << m_dpinfo->channel() << 
      ", reason: " << e.what() << std::endl;
  }
  m_ndata=0;
  m_vkmin=cool::ValidityKeyMax;
  m_vkmax=0;
}


bool DCSTxtToCool::getCoolDB(const std::string coolstr,
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

void DCSTxtToCool::readMap(const std::string configfile) {
  std::cout << "Read configuration from file: " << configfile << std::endl;
  m_dpmap.clear();
  m_folderlist.clear();
  m_folderchan.clear();
  std::ifstream infile;
  infile.open(configfile.c_str());
  std::string dpname, folder, column;
  int channel;
  int nobj=0;
  while (infile >> dpname >> folder >> column >> channel) {
    ++nobj;
    cool::RecordSpecification atrspec;
    atrspec.extend(column,cool::StorageType::Float);
    DataPointInfo* dptr=new DataPointInfo(folder,column,channel,atrspec);
    m_dpmap.insert(std::pair<std::string,DataPointInfo*>(dpname,dptr));

    // check if this folder exists already in list
    bool found=false;
    for (unsigned int i=0;i<m_folderlist.size();++i) {
      if (m_folderlist[i]==folder) {
	++m_folderchan[i];
        found=true;
      }
    }
    if (!found) {
      m_folderlist.push_back(folder);
      m_folderchan.push_back(1);
    }
  } // end input loop
  std::cout << "Read " << nobj << " lines from file, giving " << 
    m_dpmap.size() << " datapoint mappings " << std::endl;
  std::cout << "Total of " << m_folderlist.size() << " COOL folders used" <<
    std::endl;
  for (unsigned int i=0;i<m_folderlist.size();++i)
    std::cout << "Folder " << m_folderlist[i] << " channel count " << 
      m_folderchan[i] << std::endl;
}

cool::IFolderPtr DCSTxtToCool::getFolder(const std::string folder,
  			   const cool::IRecordSpecification& atrspec) {
  if (m_coolDb->existsFolder(folder)) {
    std::cout << "Folder " << folder << " already exists in database" 
     << std::endl;
    return m_coolDb->getFolder(folder);
  } else {
    std::cout << "Folder " << folder << " not found - try to create it" << 
      std::endl;
    // create folder (single version), and create parents if needed     
    // description string is used to signal data type to Athena
    // depends if folder is single or multichannel
    int nchan=0;
    for (unsigned int i=0;i<m_folderlist.size();++i)
      if (m_folderlist[i]==folder) nchan=m_folderchan[i];
    std::string desc="dummy";
    if (nchan==1) desc="<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"40774348\" /></addrHeader><typeName>AthenaAttributeList</typeName>";
    if (nchan>1) desc="<timeStamp>time</timeStamp><addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
    std::cout << "Folder description string set to: " << desc << std::endl;
    cool::IFolderPtr folderptr=m_coolDb->createFolder(folder,cool::FolderSpecification(cool::FolderVersioning::SINGLE_VERSION,atrspec),desc,true);
    return folderptr;
  }
}

int main(int argc, const char* argv[]) {
  if (argc<4) {
    std::cout << "Syntax: DCSTxtToCool,exe <coolDBconnection> <configfile> <datafile> {<offset>}" << std::endl;
    return 1;
  }
  std::string coolstr=argv[1];
  std::string configfile=argv[2];
  std::string datafile=argv[3];
  int offset=0;
  if (argc>4) offset=atoi(argv[4]);
  DCSTxtToCool convert(coolstr,configfile,datafile,offset);
  return convert.execute();
}

