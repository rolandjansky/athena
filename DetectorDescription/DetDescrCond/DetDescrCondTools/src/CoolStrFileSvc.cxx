/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// CoolStrFile.cxx
// Richard Hawkings, started 14/11/05

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"

#include "CoolStrFileSvc.h"

// technology types for storage
#define INLINE_STRING 0
#define INLINE_CLOB   1

CoolStrFileSvc::CoolStrFileSvc(const std::string& name, ISvcLocator* svc) :
  AthService(name,svc),
  p_detstore(0)
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
    return AthService::queryInterface(riid,ppvInterface);
  }
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::initialize()
{
  // no explicit service initialisation - done by AthService
  ATH_MSG_DEBUG("in initialize()");

  // get detector store
  if (StatusCode::SUCCESS!=service("DetectorStore",p_detstore)) {
    ATH_MSG_FATAL("Detector store not found");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::finalize() { return StatusCode::SUCCESS;}

StatusCode CoolStrFileSvc::putFile(const std::string& folder, 
   const std::string& filename, const int chan, const int tech) const {
  ATH_MSG_INFO("PutFile for file "+filename+" folder "+
	       folder+" chan " << chan << " technology " << tech);
  
  // transform to a string
  FILE* f = fopen (filename.c_str(),"rb");
  if (f != nullptr)   {
    fseek (f, 0L, SEEK_END);
    int size = ftell (f);
    fseek (f, 0L, SEEK_SET);
    ATH_MSG_INFO("Input file size is " << size);
    std::vector<char> sbuf(size);
    if (fread(&sbuf[0],size,1,f) != 1) {
      ATH_MSG_ERROR("Short read of input file.");
      fclose (f);
      return StatusCode::FAILURE;
    }
    // copy from buffer to string and save
    std::string sdata(sbuf.begin(),sbuf.begin()+size);
    fclose (f);
    return putData(folder,filename,chan,tech,sdata);
  }

  ATH_MSG_INFO("Cannot open file "+filename);
  return StatusCode::FAILURE;
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
  std::string rfile,data;
  if (StatusCode::SUCCESS!=getData(folder,chan,rfile,data)) {
    return StatusCode::FAILURE;
  }
  // default to filename stored with data
  if (file!="") rfile=file;

  FILE* f = fopen (rfile.c_str(),"wb");
  if (f!=nullptr) {
    int size=data.size();
    fwrite(data.c_str(),size,1,f);
    ATH_MSG_INFO("getFile: written data of length " << size <<
		 " into file " << rfile);
  } else {
    ATH_MSG_ERROR("Failed to open file "+rfile+" for write");
    return StatusCode::FAILURE;
  }
  fclose(f);
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::putData(const std::string& folder, 
        const std::string& filename,const int chan,const int tech, 
				   const std::string& data) const  {

  // check technology type
  if (tech!=INLINE_STRING && tech!=INLINE_CLOB) {
    ATH_MSG_ERROR("Bad technology type (must be 0 or 1)");
    return StatusCode::FAILURE;
  }
  // check data length for strings
  if (tech==INLINE_STRING && data.size()>=4000) {
    ATH_MSG_ERROR("String technology selected (" << INLINE_STRING << 
      ") but data size is >4000 characters ");
    return StatusCode::FAILURE;
  }
  // check channel zero is not being requested
  if (chan==0) {
    ATH_MSG_ERROR("Channel 0 cannot be used");
    return StatusCode::FAILURE;
  }

  // check if collection already exists, if not have to create in TDS
  ATH_MSG_INFO("putData to folder "+folder+" channel " << chan <<
	       " from file "+filename+" using technology:");
  if (tech==INLINE_STRING) ATH_MSG_INFO( INLINE_STRING << " (String)");
  if (tech==INLINE_CLOB) ATH_MSG_INFO( INLINE_CLOB << " (CLOB<16M)");
  CondAttrListCollection* atrc=0;
  if (!p_detstore->contains<CondAttrListCollection>(folder)) {
    ATH_MSG_DEBUG("Creating new CondAttrListCollection for folder "+folder);
    CondAttrListCollection* atrc=new CondAttrListCollection(true);
    if (StatusCode::SUCCESS!=p_detstore->record(atrc,folder)) {
      ATH_MSG_ERROR("Could not create CondAttrListCollection "+folder);
    return StatusCode::FAILURE;
    }
  }
  // do const cast here so we can add to already exisiting collections
  const CondAttrListCollection* catrc=0;
  ATH_MSG_DEBUG("Attempting to retrieve collection (const)");
  if (StatusCode::SUCCESS!=p_detstore->retrieve(catrc,folder)) {
    ATH_MSG_ERROR("Could not retrieve CondAttrListCollection ");
    return StatusCode::FAILURE;
  }
  atrc=const_cast<CondAttrListCollection*>(catrc);
  if (atrc==0) {
    ATH_MSG_ERROR("Could not retrieve non-const pointer to atrc");
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("About to create AttributeListSpecification");
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
  ATH_MSG_DEBUG("About to add channel to: " << atrc);
  atrc->add(channum,alist);
  return StatusCode::SUCCESS;
}

StatusCode CoolStrFileSvc::getData(const std::string& folder, 
	    const int chan, std::string& file, std::string& data) const {
  const CondAttrListCollection* atrc;
  data="";
  if (StatusCode::SUCCESS!=p_detstore->retrieve(atrc,folder)) {
    ATH_MSG_ERROR("getData failed for folder "+folder+" channel "
		  << chan);
    return StatusCode::FAILURE;
  }
  CondAttrListCollection::ChanNum channum=chan;
  CondAttrListCollection::const_iterator itr=atrc->chanAttrListPair(channum);
  if (itr!=atrc->end()) {
    const coral::AttributeList& atr=itr->second;
    data=atr["data"].data<std::string>();
    file=atr["file"].data<std::string>();
    if (msgLvl(MSG::DEBUG)) {
      std::ostringstream atrstring;
      atr.toOutputStream(atrstring);
      ATH_MSG_DEBUG("read Channum " << channum << " atrlist: " << 
		    atrstring.str());
    }
  } else {
	ATH_MSG_ERROR("Invalid channel number");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
