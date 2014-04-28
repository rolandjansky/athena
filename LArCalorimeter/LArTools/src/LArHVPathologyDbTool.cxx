/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTools/LArHVPathologyDbTool.h" 
#include "LArRecConditions/LArHVPathologiesDb.h"

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"

#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "CoralBase/AttributeListException.h"
#include "CoralBase/Blob.h"

#include "TBufferFile.h"
#include "TClass.h"


LArHVPathologyDbTool::LArHVPathologyDbTool(const std::string& type
					   ,const std::string& name
					   ,const IInterface* parent)
  : AlgTool(type,name,parent)
{
  declareInterface<ILArHVPathologyDbTool>(this);
}

LArHVPathologyDbTool::~LArHVPathologyDbTool()
{ 
}

StatusCode LArHVPathologyDbTool::initialize()
{
  return StatusCode::SUCCESS;
}

StatusCode LArHVPathologyDbTool::finalize()
{
  return StatusCode::SUCCESS;
}

AthenaAttributeList* LArHVPathologyDbTool::hvPathology2AttrList(const LArHVPathologiesDb& pathologyContainer)
{
  MsgStream log(msgSvc(), name());

  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();

  spec->extend("blobVersion","unsigned int");   //Should allow schema evolution if needed
  spec->extend("Constants","blob");             //Holds the container
 
  AthenaAttributeList* attrList = new AthenaAttributeList(*spec);
     
  (*attrList)["blobVersion"].data<unsigned int>()=(unsigned int)0;
  coral::Blob& blob=(*attrList)["Constants"].data<coral::Blob>();
     
  TClass* klass = TClass::GetClass("LArHVPathologiesDb");
  if (klass==NULL) {
    log << MSG::ERROR << "Can't find TClass LArHVPathologiesDb" << endreq;
    return 0;
  }
  else
    log << MSG::DEBUG << "Got TClass LArHVPathologiesDb" << endreq;
 
  TBufferFile buf(TBuffer::kWrite);
 
  if(buf.WriteObjectAny(&pathologyContainer, klass)!=1) {
    log << MSG::ERROR << "Failed to stream LArHVPathologiesDb" << endreq;
    return 0;
  }
  
  blob.resize(buf.Length());
  void* adr = blob.startingAddress();
  memcpy(adr,buf.Buffer(),buf.Length());
  return attrList;
}

LArHVPathologiesDb* LArHVPathologyDbTool::attrList2HvPathology(const AthenaAttributeList& attrList)
{
  MsgStream log(msgSvc(), name());

  try {
    const unsigned blobVersion=attrList["blobVersion"].data<unsigned int>();
    const coral::Blob& blob = attrList["Constants"].data<coral::Blob>();
 
    if (blobVersion!=0) {
      log << MSG::ERROR << "Can't interpret BLOB version " << blobVersion << endreq;
      return 0;
    }
     
    TClass* klass = TClass::GetClass("LArHVPathologiesDb");
    if(klass==NULL){
      log << MSG::ERROR << "Can't find TClass LArHVPathologiesDb" << endreq;
      return 0;
    }
    else
      log << MSG::DEBUG << "Got TClass LArHVPathologiesDb" << endreq;
 
    TBufferFile buf(TBuffer::kRead, blob.size(), (void*)blob.startingAddress(), false);
    LArHVPathologiesDb* container = (LArHVPathologiesDb*)buf.ReadObjectAny(klass);
    return container;
  }catch (coral::AttributeListException &e) {
    log << MSG::ERROR << e.what() << endreq;
  }
  return 0;
}
