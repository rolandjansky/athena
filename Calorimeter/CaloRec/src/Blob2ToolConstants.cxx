/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloRec/Blob2ToolConstants.h"

#include "CaloConditions/ToolConstants.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeList.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/AttributeListException.h"
#include "CoralBase/Blob.h"
#include "GaudiKernel/MsgStream.h"

#include "TBufferFile.h"
#include "TClass.h"

#include "SGTools/crc64.h"

Blob2ToolConstants::Blob2ToolConstants (const std::string& type, 
					const std::string& name, 
					const IInterface* parent) :
  AthAlgTool(type,name,parent) 
{
   declareInterface<Blob2ToolConstants>(this);
}
  

StatusCode Blob2ToolConstants::initialize() {
  msg(MSG::DEBUG) << "Initializing..." << endreq;
  return StatusCode::SUCCESS;
}



StatusCode Blob2ToolConstants::addFolder(const std::string& fn, const std::string& key) {

  std::vector<std::string> keys(1,key);
  return addFolder(fn,keys);

}

StatusCode Blob2ToolConstants::addFolder(const std::string& fn, const std::vector<std::string> & keys) {

  if (keys.size()==0) {
    msg(MSG::ERROR) << "addFolder: List of keys is empty" << endreq;
    return StatusCode::FAILURE;
  }

  FOLDEROBJ& fobj=m_data[fn];

  //Check if this folder is alreayd konwn
  const bool newCallback=(fobj.key_tc.size()==0);


  if (msgLvl(MSG::DEBUG)) {
    if (newCallback) 
      msg(MSG::DEBUG) << "Adding COOL inline folder " << fn << endreq; 
    else
      msg(MSG::DEBUG) << "Have already a callback for COOL inline folder " << fn << endreq; 
  }

  std::vector<std::string>::const_iterator kit=keys.begin();
  std::vector<std::string>::const_iterator kit_e=keys.end();

  for(;kit!=kit_e;++kit) {
    const std::string& key=*kit;
    //Insert key/ToolConstants pair:
    if (!fobj.key_tc.insert(std::make_pair(key,(CaloRec::ToolConstants*)NULL)).second)
      msg (MSG::DEBUG) << "COOL channel "<< key << " already set up." << endreq;
    else
      msg (MSG::DEBUG) << "Added COOL channel " << key << endreq;
  }


  if (newCallback) {
    //New folder
    StatusCode sc=detStore()->regFcn(&Blob2ToolConstants::fillObjects,this,
				     fobj.inlineData,fn,true); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to register callback for CondAttributeListCollection object with key " << fn << endreq;
      // Note that once we've started registering callbacks,
      // we cannot return FAILURE from initialize() --- otherwise,
      // we'll be left with a dangling callback.
      //return StatusCode::FAILURE;
    }
    else
      msg(MSG::INFO) << "Successfully registered callback for CondAttributeListCollection object with key " << fn << endreq;
  }

  return StatusCode::SUCCESS; 
}
    
StatusCode Blob2ToolConstants::fillObjects(IOVSVC_CALLBACK_ARGS_K(inlineFN)) {
  

  msg(MSG::DEBUG) << "Callback method Blob2ToolConstants::fillObjects" << endreq;
  //msg(MSG::DEBUG) << i << endreq;
  std::list<std::string>::const_iterator it=inlineFN.begin();
  std::list<std::string>::const_iterator it_e=inlineFN.end();
  
  for (;it!=it_e;++it){ //Loop over COOL folders
    msg(MSG::DEBUG) << "Working on COOL inline folder " << *it << endreq;
    FOLDEROBJ& fobj=m_data[*it];
    if (!fobj.inlineData.isValid()) {
      msg(MSG::ERROR) << "COOL inline data not valid for key " << *it << endreq;
      return StatusCode::FAILURE;
    }

    KEYOBJMAP::iterator kit=fobj.key_tc.begin();
    KEYOBJMAP::iterator kit_e=fobj.key_tc.end();
    for (;kit!=kit_e;++kit) {
      const std::string& key=kit->first;
      
      msg(MSG::DEBUG) << "Working on COOL inline channel name " << key << endreq;
      if (!kit->second) {//
	kit->second=new CaloRec::ToolConstants();
	StatusCode sc=detStore()->record(kit->second,key);
	if (sc.isFailure()) {
	  msg(MSG::ERROR) << "Failed to record ToolConstants object with key " << key << endreq;
	  delete kit->second;
	  return sc;
	}
	else
	  msg(MSG::DEBUG) << "Successfully recoreded ToolConstants object with key " << key << endreq;
      }//end if !kit->second
      
      const unsigned chNbr=nameToChannelNumber(key);
      //Fixme: Check that this channel actually exits
      const std::string& chanName=fobj.inlineData->chanName(chNbr);
      if (chanName.size()>0 && key!=chanName) {
	msg(MSG::ERROR) << "Channel name does not match! Expected " << key << " found " << chanName << endreq;
	return StatusCode::FAILURE;
      }
      else
	msg(MSG::DEBUG) << "Found channel number " << chNbr << " named " << key << endreq;

      const coral::AttributeList& attrList=fobj.inlineData->attributeList(chNbr);
      //msg(MSG::DEBUG) << "Size of attrList=" << attrList.size() << endreq;
      if (attrList.size()==0) {
	msg(MSG::ERROR) << "Failed to get valid AttributeList for channel number " << chNbr << " (key " << key << ")" << endreq;
	return StatusCode::FAILURE;
      }
      StatusCode sc=AttrListToToolConstants(attrList, *(kit->second));
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to convert AttributeList to ToolConstants" <<endreq;
	return sc;
      }

      //Drop inline data from DetStore
      sc=detStore()->remove(fobj.inlineData.cptr());
      if (sc.isFailure()) {
	msg(MSG::WARNING) << "Failed to remove AttributeList object from DetStore" << endreq;
	//Not a big problem, continue running
      }
   
    }//end loop over keys == COOL channels
  }//end loop over COOL folders
  return StatusCode::SUCCESS;
}


coral::AttributeList* Blob2ToolConstants::ToolConstantsToAttrList(const CaloRec::ToolConstants* tc) const {
    
    coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();

    spec->extend("clsname","string");             //Data member of CaloRec::ToolConstants
    spec->extend("version","unsigned int");       //Data member of CaloRec::ToolConstants
    spec->extend("blobVersion","unsigned int");   //Should allow schema evolution if needed
    spec->extend("Constants","blob");             //Holds the map<string,Arrayrep>


    coral::AttributeList* attrList = new coral::AttributeList(*spec);
    
    (*attrList)["clsname"].data<std::string>()=tc->clsname();
    (*attrList)["version"].data<unsigned int>()=(unsigned int)tc->version();
    (*attrList)["blobVersion"].data<unsigned int>()=(unsigned int)0;

    coral::Blob& blob=(*attrList)["Constants"].data<coral::Blob>();
    
    //typedef std::map<std::string, CaloRec::Arrayrep> T;
    TClass* klass = TClass::GetClass ("std::map<std::string, CaloRec::Arrayrep>");
    if (klass==NULL) {
      msg( MSG::ERROR) << "Can't find TClass std::map<std::string, CaloRec::Arrayrep>" << endreq;
      return 0;
    }
    else
      msg(MSG::DEBUG) << "Got TClass std::map<std::string, CaloRec::Arrayrep>" << endreq;

    TBufferFile buf (TBuffer::kWrite);

    if (buf.WriteObjectAny (&tc->map(), klass) != 1) {
      msg(MSG::ERROR) << "Failed to stream CaloRec::ToolConstants::Maptype " << endreq;
      return 0;
    }
    
    blob.resize(buf.Length());
    void* adr = blob.startingAddress();
    memcpy(adr,buf.Buffer(),buf.Length());
    return attrList;
  }


StatusCode Blob2ToolConstants::AttrListToToolConstants(const coral::AttributeList& attrList, CaloRec::ToolConstants& tc) const {

    typedef CaloRec::ToolConstants::Maptype T;
    T* map=0;

    try {
      tc.clsname(attrList["clsname"].data<std::string>());
      tc.version(attrList["version"].data<unsigned int>());
      const unsigned blobVersion=attrList["blobVersion"].data<unsigned int>();
      const coral::Blob& blob = attrList["Constants"].data<coral::Blob>();

      if (blobVersion!=0) {
	msg(MSG::ERROR) << "Can't interpret BLOB version " << blobVersion << endreq;
	return StatusCode::FAILURE;
      }
    
      TClass* klass = TClass::GetClass ("std::map<std::string, CaloRec::Arrayrep>");
      if (klass==NULL) {
	msg(MSG::ERROR) << "Can't find TClass std::map<std::string, CaloRec::Arrayrep>" << endreq;
	return StatusCode::FAILURE;
      }
      else
	msg(MSG::DEBUG) << "Got TClass std::map<std::string, CaloRec::Arrayrep>" << endreq;

      TBufferFile buf (TBuffer::kRead, blob.size(), (void*)blob.startingAddress(), false);
      map = (T*)buf.ReadObjectAny (klass);
    }catch (coral::AttributeListException &e) {
      msg(MSG::ERROR) << e.what() << endreq;
      delete map;
      return StatusCode::FAILURE;
    }

    T::const_iterator it=map->begin();
    T::const_iterator it_e=map->end();
    for (;it!=it_e;++it) {
      tc.setrep(it->first,it->second);
    }
    delete map;
    return StatusCode::SUCCESS;
}


uint32_t Blob2ToolConstants::nameToChannelNumber(const std::string& name) const {
    const uint64_t hash64=SG::crc64(name);
    const uint32_t hash32=(uint32_t)(hash64 & 0xFFFFFFFF);
    //std::cout << "HASH: 64bit:" << std::hex << hash64 << " 32bit:" << hash32 << std::endl;
    return hash32;
  }



  bool Blob2ToolConstants::hasChannel(const std::string& inlineFolder, const std::string& chName) const {
    std::map< std::string, FOLDEROBJ >::const_iterator itf=m_data.find(inlineFolder);
    if (itf==m_data.end()) {
      msg(MSG::WARNING) << "hasChannel: Nothing known about folder " << inlineFolder << endreq;
      return false;
    }

    KEYOBJMAP::const_iterator itk=itf->second.key_tc.find(chName);
    return (itk!=itf->second.key_tc.end());
  }


static const InterfaceID IID_Blob2ToolConstants ("Blob2ToolConstants", 1 , 0);


const InterfaceID& Blob2ToolConstants::interfaceID() {
  return IID_Blob2ToolConstants;
}

