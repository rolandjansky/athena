/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// FolderInfo.cxx
// class to hold details of COOL folders in reconstruction tets

#include <iostream>
#include <sstream>

#include "CoolKernel/FolderSpecification.h"
#include "CoraCool/CoraCoolDatabase.h"
#include "CoraCool/CoraCoolFolder.h"
#include "FolderInfo.h"

FolderInfo::FolderInfo(const std::string name,
		       DBConnection* dbconn, const int ndbconn,
		       const int nchan, PayloadTime ptime, 
		       PayloadType ptype, 
		       const int ncol, const int size, const int period,
		       const std::string tag, const bool payloadTable) :
  m_name(name), m_dbconn(dbconn), m_ndbconn(ndbconn),
  m_nchan(nchan), m_ptime(ptime), m_ptype(ptype),
  m_ncol(ncol), m_size(size), m_period(period), m_noisychan(), 
  m_noisyperiod(0.1), m_tag(tag), m_paytable(payloadTable), m_poolplace(0) {
  // construct tag
  if (m_ptime==RUNC || m_ptime==RUNP) {
    // construct tag from foldername - removing / and adding explicit -<tag>
    // at end, following standard folder tagging convention
    std::string tag2="";
    for (std::string::const_iterator itr=m_name.begin(); 
	 itr!=m_name.end();++itr) {
      if (*itr!='/') tag2.push_back(*itr);
    }
    m_tag=tag2+"-"+m_tag;
    std::cout << "Multi-version folder " << name << 
      " will be tagged with tag: " << m_tag << std::endl;
  } else {
    // single version folders are not tagged
    m_tag="";
  }
}

FolderInfo::~FolderInfo() {}

bool FolderInfo::createFolder() {
  // create the folder

  // set CoraCool flag from folder data type
  bool coracool=(m_ptype==CoraCool);
  bool coolvec=(m_ptype==CoolVector);

  // construct specification 
  cool::RecordSpecification spec;
  // use UChar to flag undefined storage type
  cool::StorageType::TypeId coltype=cool::StorageType::UChar;
  std::string colnm="Undef";
  if (m_ptype==Int || coracool || coolvec) {
    coltype=cool::StorageType::Int32;
    colnm="int";
  }
  if (m_ptype==Float) {
    coltype=cool::StorageType::Float;
    colnm="float";
  }
  if (m_ptype==String) {
    coltype=( m_size<4000 ? 
	      cool::StorageType::String4k : cool::StorageType::String16M );
    colnm="string";
  }
  if (m_ptype==Blob) {
     coltype=( m_size<64000 ? 
	       cool::StorageType::Blob64k : cool::StorageType::Blob16M );
    colnm="blob";
  }
  if (m_ptype==PoolRef) {
    spec.extend("PoolRef",cool::StorageType::String4k);
  } else {
    // for coracool folder, also need primary and foreign keys
    if (coracool) {
      spec.extend("PrimKey",cool::StorageType::Int32);
	spec.extend("ForeignKey",cool::StorageType::Int32);
    }
    for (int i=0;i<m_ncol;++i) {
      std::ostringstream colstr;
      colstr << colnm;
      if (m_ncol>1) colstr << i;
      spec.extend(colstr.str(),coltype);
    }
  }
  // construct description
  std::string desc="<timeStamp>";
  // set timestamp part of description
  if (m_ptime==DCSP || m_ptime==DCSC) {     
    desc+="time";
  } else {
    desc+="run-event";
  }
  desc+="</timeStamp>";
  if (m_ptype==PoolRef) {
    desc+="<addrHeader><address_header service_type=\"71\" clid=\"117237436\" /></addrHeader><typeName>TestCoolRecPoolDataColl</typeName>";
  } else if (m_ptype==CoraCool) {
    desc+="<addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>CondAttrListVec</typeName>";
  } else if (m_ptype==CoolVector) {
    desc+="<addrHeader><address_header service_type=\"71\" clid=\"55403898\" /></addrHeader><typeName>CondAttrListVec</typeName>";

  } else {
    desc+="<addrHeader><address_header service_type=\"71\" clid=\"1238547719\" /></addrHeader><typeName>CondAttrListCollection</typeName>";
  }
  std::cout << "Folder description: " << desc << std::endl;
  if (m_paytable) std::cout << "Folder will have separate payload table"
			      << std::endl;
  cool::FolderVersioning::Mode mode;
  if (m_tag=="") {
    mode=cool::FolderVersioning::SINGLE_VERSION;
  } else {
    mode=cool::FolderVersioning::MULTI_VERSION;
  }
  // access the database
  cool::IDatabasePtr cooldb=m_dbconn->open();

  if (coracool) {
    // set CORAL table name from folder leaf name
    std::string coraltable;
    std::string::size_type iofs=m_name.rfind("/");
    if (iofs!=std::string::npos) {
      coraltable=m_name.substr(iofs+1);
    } else {
      coraltable=m_name;
    }
    cool::RecordSpecification fkspec;
    fkspec.extend("ForeignKey",cool::StorageType::Int32);
    CoraCoolDatabasePtr coraDb=m_dbconn->coradbptr();
    CoraCoolFolderPtr corafolderptr=coraDb->createFolder(m_name,coraltable,
      	  fkspec,spec,"ForeignKey","PrimKey",desc,mode,true);
    std::cout << "Done CoraCool folder creation" << std::endl;
    printSpec(corafolderptr->payloadSpecification());
  } else {
    cool::IFolderPtr folderptr;
    cool::PayloadMode::Mode pmode=cool::PayloadMode::INLINEPAYLOAD;
    if (m_paytable) {
      pmode=cool::PayloadMode::SEPARATEPAYLOAD;
    } else if (coolvec) {
      pmode=cool::PayloadMode::VECTORPAYLOAD;
    }
    folderptr=cooldb->createFolder(m_name,
	cool::FolderSpecification(mode,spec,pmode),desc,true);

    std::cout << "Done COOL folder creation payload type " << pmode << std::endl;
    printSpec(folderptr->payloadSpecification());
  }
  return true;
}
 
void FolderInfo::printSpec(const cool::IRecordSpecification& catrspec) const {
  // print out payload specification
  std::cout << "AttributeList specification for folder " << m_name << " is ";
  for (unsigned int i=0;i<catrspec.size();++i) {
    const cool::IFieldSpecification& field=catrspec[i];
    std::cout << field.name() << " : " << 
      field.storageType().name() << " , ";
  }
  std::cout << std::endl;
}

FolderInfo::PayloadTime string2ptime(const std::string str) {
  if (str=="DCSC") return FolderInfo::DCSC;
  if (str=="DCSP") return FolderInfo::DCSP;
  if (str=="RUNC") return FolderInfo::RUNC;
  if (str=="RUNP") return FolderInfo::RUNP;
  return FolderInfo::PTimeUndefined;
}

FolderInfo::PayloadType string2ptype(const std::string str) {
  if (str=="INT" || str=="int") return FolderInfo::Int;
  if (str=="FLOAT" || str=="float") return FolderInfo::Float;
  if (str=="STRING" || str=="string") return FolderInfo::String;
  if (str=="POOLREF" || str=="poolref") return FolderInfo::PoolRef;
  if (str=="BLOB" || str=="blob") return FolderInfo::Blob;
  if (str=="CORACOOL" || str=="coracool") return FolderInfo::CoraCool;
  if (str=="COOLVECTOR" || str=="coolvector") return FolderInfo::CoolVector;
  return FolderInfo::PTypeUndefined;
}
