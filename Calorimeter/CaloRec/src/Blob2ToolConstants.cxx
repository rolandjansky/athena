/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloRec/Blob2ToolConstants.h"

#include "CaloConditions/ToolConstants.h"
#include "CoralBase/Attribute.h"
#include "CoralBase/AttributeListSpecification.h"
#include "CoralBase/AttributeListException.h"
#include "CoralBase/Blob.h"

#include "TBufferFile.h"
#include "TClass.h"

#include "CxxUtils/checker_macros.h"
#include "CxxUtils/crc64.h"

Blob2ToolConstants::Blob2ToolConstants (const std::string& type, 
					const std::string& name, 
					const IInterface* parent) :
  AthAlgTool(type,name,parent) 
{
   declareInterface<Blob2ToolConstants>(this);
}
  

StatusCode Blob2ToolConstants::initialize() {
  msg(MSG::DEBUG) << "Initializing..." << endmsg;
  return StatusCode::SUCCESS;
}


coral::AttributeList* Blob2ToolConstants::ToolConstantsToAttrList(const CaloRec::ToolConstants* tc) const {
    
    coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();

    spec->extend("clsname","string");             //Data member of CaloRec::ToolConstants
    spec->extend("version","unsigned int");       //Data member of CaloRec::ToolConstants
    spec->extend("blobVersion","unsigned int");   //Should allow schema evolution if needed
    spec->extend("Constants","blob");             //Holds the map<string,Arrayrep>


    auto attrList = std::make_unique<coral::AttributeList>(*spec);

    spec->release();
    // cppcheck-suppress memleak
    spec = nullptr;
    
    (*attrList)["clsname"].data<std::string>()=tc->clsname();
    (*attrList)["version"].data<unsigned int>()=(unsigned int)tc->version();
    (*attrList)["blobVersion"].data<unsigned int>()=(unsigned int)0;

    coral::Blob& blob=(*attrList)["Constants"].data<coral::Blob>();
    
    //typedef std::map<std::string, CaloRec::Arrayrep> T;
    TClass* klass = TClass::GetClass ("std::map<std::string, CaloRec::Arrayrep>");
    if (klass==nullptr) {
      msg( MSG::ERROR) << "Can't find TClass std::map<std::string, CaloRec::Arrayrep>" << endmsg;
      return nullptr;
    }
    else
      msg(MSG::DEBUG) << "Got TClass std::map<std::string, CaloRec::Arrayrep>" << endmsg;

    TBufferFile buf (TBuffer::kWrite);

    if (buf.WriteObjectAny (&tc->map(), klass) != 1) {
      msg(MSG::ERROR) << "Failed to stream CaloRec::ToolConstants::Maptype " << endmsg;
      return nullptr;
    }
    
    blob.resize(buf.Length());
    void* adr = blob.startingAddress();
    memcpy(adr,buf.Buffer(),buf.Length());
    return attrList.release();
  }

StatusCode Blob2ToolConstants::AttrListToToolConstants(const coral::AttributeList& attrList, CaloRec::ToolConstants& tc) const {

    using T = CaloRec::ToolConstants::Maptype;
    std::unique_ptr<T> map;

    try {
      tc.clsname(attrList["clsname"].data<std::string>());
      tc.version(attrList["version"].data<unsigned int>());
      const unsigned blobVersion=attrList["blobVersion"].data<unsigned int>();
      const coral::Blob& blob = attrList["Constants"].data<coral::Blob>();

      if (blobVersion!=0) {
	msg(MSG::ERROR) << "Can't interpret BLOB version " << blobVersion << endmsg;
	return StatusCode::FAILURE;
      }
    
      TClass* klass = TClass::GetClass ("std::map<std::string, CaloRec::Arrayrep>");
      if (klass==nullptr) {
	msg(MSG::ERROR) << "Can't find TClass std::map<std::string, CaloRec::Arrayrep>" << endmsg;
	return StatusCode::FAILURE;
      }
      else
	msg(MSG::DEBUG) << "Got TClass std::map<std::string, CaloRec::Arrayrep>" << endmsg;

      // TBufferFile needs a void* even in read-only mode:
      void* blob_start ATLAS_THREAD_SAFE = const_cast<void*>(blob.startingAddress());
      TBufferFile buf (TBuffer::kRead, blob.size(), blob_start, false);
      map.reset( (T*)buf.ReadObjectAny (klass) );
    }catch (coral::AttributeListException &e) {
      msg(MSG::ERROR) << e.what() << endmsg;
      return StatusCode::FAILURE;
    }

    T::const_iterator it=map->begin();
    T::const_iterator it_e=map->end();
    for (;it!=it_e;++it) {
      tc.setrep(it->first,it->second);
    }
    return StatusCode::SUCCESS;
}

uint32_t Blob2ToolConstants::nameToChannelNumber(const std::string& name) {
    const uint64_t hash64=CxxUtils::crc64(name);
    const uint32_t hash32=(uint32_t)(hash64 & 0xFFFFFFFF);
    //std::cout << "HASH: 64bit:" << std::hex << hash64 << " 32bit:" << hash32 << std::endl;
    return hash32;
  }


static const InterfaceID IID_Blob2ToolConstants ("Blob2ToolConstants", 1 , 0);


const InterfaceID& Blob2ToolConstants::interfaceID() {
  return IID_Blob2ToolConstants;
}

