/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoolStrFile.cxx
// Richard Hawkings, started 14/11/05

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CoolStrFileSvc.h"

// technology types for storage
#define INLINE_STRING 0
#define INLINE_CLOB   1

CoolStrFileSvc::CoolStrFileSvc(const std::string& name, ISvcLocator* svc) :
  Service(name,svc)
{
  // declare properties
}

CoolStrFileSvc::~CoolStrFileSvc() {}

const InterfaceID& CoolStrFileSvc::type() const
{
  return ICoolStrFileSvc::interfaceID();
}

StatusCode CoolStrFileSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if (ICoolStrFileSvc::interfaceID().versionMatch(riid)) {
    *ppvInterface=(ICoolStrFileSvc*)this;
  } else {
    return Service::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::initialize()
{
  // service initialisation 

  MsgStream log(msgSvc(),name());

  if (StatusCode::SUCCESS!=Service::initialize()) log << MSG::ERROR <<
	    "Service initialisation failed" << endreq;

  log << MSG::DEBUG << "in initialize()" << endreq;

  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    log << MSG::FATAL << "Detector store not found" << endreq; 
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::finalize()
{
  MsgStream log(msgSvc(),name());
  log << MSG::DEBUG << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::putFile(const std::string& folder, 
   const std::string& filename, const int chan, const int tech) const {
  MsgStream log(msgSvc(),name());
  log << MSG::INFO << "PutFile for file " << filename << " folder " <<
    folder << " chan " << chan << " technology " << tech << endreq;
  
  // transform to a string
  FILE* f = fopen (filename.c_str(),"rb");
  if (f != NULL)   {
    fseek (f, 0L, SEEK_END);
    int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    log << MSG::INFO << "Input file size is " << size << endreq;
    std::vector<char> sbuf(size);
    fread(&sbuf[0],size,1,f);
    fclose (f);
    // copy from buffer to string and save
    std::string sdata(sbuf.begin(),sbuf.begin()+size);
    return putData(folder,filename,chan,tech,sdata);
  } else {
    log << MSG::INFO << "Cannot open file " << filename << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::getString(const std::string& folder, 
		      const int chan, std::string& data) const {
  // dummy datafile name - this is not returned to the client
  std::string rfile;
  StatusCode sc=getData(folder,chan,rfile,data);
  return sc;
}

StatusCode CoolStrFileSvc::getFile(const std::string& folder, const int chan,
				   const std::string& file) const {
  MsgStream log(msgSvc(),name());
  std::string rfile,data;
  if (StatusCode::SUCCESS!=getData(folder,chan,rfile,data)) {
    return StatusCode::FAILURE;
  }
  // default to filename stored with data
  if (file!="") rfile=file;

  FILE* f = fopen (rfile.c_str(),"wb");
  if (f!=NULL) {
    int size=data.size();
    fwrite(data.c_str(),size,1,f);
    log << MSG::INFO << "getFile: written data of length " << size <<
    " into file " << rfile << endreq;
  } else {
    log << MSG::ERROR << "Failed to open file " << rfile << " for write" <<
      endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::putData(const std::string& folder, 
        const std::string& filename,const int chan,const int tech, 
				   const std::string& data) const  {
  MsgStream log(msgSvc(),name());

  // check technology type
  if (tech!=INLINE_STRING && tech!=INLINE_CLOB) {
    log << MSG::ERROR << "Bad technology type (must be 0 or 1)" << endreq;
    return StatusCode::FAILURE;
  }
  // check data length for strings
  if (tech==INLINE_STRING && data.size()>=4000) {
    log << MSG::ERROR << "String technology selected (" << INLINE_STRING << 
      ") but data size is >4000 characters " << endreq;
    return StatusCode::FAILURE;
  }
  // check channel zero is not being requested
  if (chan==0) {
    log << MSG::ERROR << "Channel 0 cannot be used" << endreq;
    return StatusCode::FAILURE;
  }

  // check if collection already exists, if not have to create in TDS
  log << MSG::INFO << "putData to folder " << folder << " channel " << chan <<
    " from file " << filename << " using technology ";
  if (tech==INLINE_STRING) log << INLINE_STRING << " (String)" << endreq;
  if (tech==INLINE_CLOB) log << INLINE_CLOB << " (CLOB<16M)" << endreq;
  CondAttrListCollection* atrc=0;
  if (!p_detstore->contains<CondAttrListCollection>(folder)) {
    log << MSG::DEBUG << "Creating new CondAttrListCollection for folder "
	<< folder << endreq;
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,folder)) {
    log << MSG::ERROR << "Could not create CondAttrListCollection " <<
     folder << endreq;
    return StatusCode::FAILURE;
    }
  }
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=0;
  log << MSG::DEBUG << "Attempting to retrieve collection (const)" << endreq;
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,folder)) {
    log << MSG::ERROR << "Could not retrieve CondAttrListCollection " <<
       folder << endreq;
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==0) {
    log << MSG::ERROR << "Could not retrieve non-const pointer to atrc" <<
      endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "About to create AttributeListSpecification" << endreq;
  coral::AttributeListSpecification* aspec=0;
  // on transient side, all data is simply string, no CLOB
  aspec=new coral::AttributeListSpecification();
  aspec->extend("tech","int");
  aspec->extend("file","string");
  aspec->extend("data","string");
  // define data contents
  AthenaAttributeList alist(*aspec);
  alist["tech"].setValue(tech);
  alist["file"].setValue(filename);
  alist["data"].setValue(data);
  CondAttrListCollection::ChanNum channum=chan;

  std::ostringstream atrstring;
  alist.print(atrstring);
  log << MSG::DEBUG << "About to add channel to: " << atrc << endreq;
  atrc->add(channum,alist);
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::getData(const std::string& folder, 
	    const int chan, std::string& file, std::string& data) const {
  MsgStream log(msgSvc(),name());
  const CondAttrListCollection* atrc;
  data="";
  if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,folder)) {
    log << MSG::ERROR << "getData failed for folder " << folder << " channel "
	<< chan << endreq;
    return StatusCode::FAILURE;
  }
  CondAttrListCollection::ChanNum channum=chan;
  CondAttrListCollection::const_iterator itr=atrc->chanAttrListPair(channum);
  if (itr!=atrc->end()) {
    const coral::AttributeList& atr=itr->second;
    data=atr["data"].data<std::string>();
    file=atr["file"].data<std::string>();
    if (log.level() < MSG::INFO) {
      std::ostringstream atrstring;
      atr.toOutputStream(atrstring);
      log << MSG::DEBUG << "read Channum " << channum << " atrlist: " << 
       atrstring.str() << endreq;
    }
  } else {
    log << MSG::ERROR << "Invalid channel number" << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
