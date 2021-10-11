/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
  : AthAlgTool(type,name,parent)
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

AthenaAttributeList*
LArHVPathologyDbTool::hvPathology2AttrList(const LArHVPathologiesDb& pathologyContainer) const
{
  AthenaAttributeList* attrList = newAttrList();
     
  (*attrList)["blobVersion"].data<unsigned int>()=(unsigned int)0;
  coral::Blob& blob=(*attrList)["Constants"].data<coral::Blob>();
     
  TClass* klass = TClass::GetClass("LArHVPathologiesDb");
  if (klass==NULL) {
    ATH_MSG_ERROR ( "Can't find TClass LArHVPathologiesDb" );
    return 0;
  }
  else
    ATH_MSG_DEBUG ( "Got TClass LArHVPathologiesDb" );
 
  TBufferFile buf(TBuffer::kWrite);
 
  if(buf.WriteObjectAny(&pathologyContainer, klass)!=1) {
    ATH_MSG_ERROR ( "Failed to stream LArHVPathologiesDb" );
    return 0;
  }
  
  blob.resize(buf.Length());
  void* adr = blob.startingAddress();
  memcpy(adr,buf.Buffer(),buf.Length());
  return attrList;
}


AthenaAttributeList*
LArHVPathologyDbTool::newAttrList () const
{
  coral::AttributeListSpecification* spec = new coral::AttributeListSpecification();
  spec->extend("blobVersion","unsigned int");   //Should allow schema evolution if needed
  spec->extend("Constants","blob");             //Holds the container
  return new AthenaAttributeList(*spec);
}



LArHVPathologiesDb* LArHVPathologyDbTool::attrList2HvPathology(const AthenaAttributeList& attrList) const
{
  try {
    const unsigned blobVersion=attrList["blobVersion"].data<unsigned int>();
    const coral::Blob& blob = attrList["Constants"].data<coral::Blob>();
 
    if (blobVersion!=0) {
      ATH_MSG_ERROR ( "Can't interpret BLOB version " << blobVersion );
      return 0;
    }
     
    TClass* klass = TClass::GetClass("LArHVPathologiesDb");
    if(klass==NULL){
      ATH_MSG_ERROR ( "Can't find TClass LArHVPathologiesDb" );
      return 0;
    }
    else
      ATH_MSG_DEBUG ( "Got TClass LArHVPathologiesDb" );

    void* data ATLAS_THREAD_SAFE = const_cast<void*> (blob.startingAddress());
    TBufferFile buf(TBuffer::kRead, blob.size(), data, false);
    LArHVPathologiesDb* container = (LArHVPathologiesDb*)buf.ReadObjectAny(klass);
    return container;
  }catch (coral::AttributeListException &e) {
    ATH_MSG_ERROR ( e.what() );
  }
  return 0;
}
