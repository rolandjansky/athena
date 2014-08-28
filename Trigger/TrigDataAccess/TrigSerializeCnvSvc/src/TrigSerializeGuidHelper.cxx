/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigSerializeGuidHelper.h"

#include "PersistentDataModel/Guid.h"

#include "Reflex/Reflex.h"
#include "StorageSvc/DbReflex.h"

using ROOT::Reflex::Type;
using ROOT::Reflex::PropertyList;



TrigSerializeGuidHelper::TrigSerializeGuidHelper(const std::string& name, const std::string& type,
						 const IInterface* parent) :
  AthAlgTool(name, type, parent)
{
  declareInterface<ITrigSerializeGuidHelper>( this );

}

TrigSerializeGuidHelper::~TrigSerializeGuidHelper(){

}

StatusCode TrigSerializeGuidHelper::ClassNameToInts(const std::string &clname, uint32_t *iarr){
  iarr[0] = iarr[1] = iarr[2] = iarr[3] = 0;

  Type t = Type::ByName(clname);
  PropertyList pl = t.Properties();
  if (pl.HasProperty("ClassID")){
    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << clname << " has " << "ClassID" << endreq;
    size_t idx = pl.KeyByName("ClassID");
    std::string gecko = pl.PropertyAsString(idx);
    if (msgLvl(MSG::DEBUG))
      msg(MSG::DEBUG) << "Index retrieval got: " << pl.KeyAt(idx)
		      << " with " << gecko << endreq;
    const Guid guid(gecko);

    iarr[0] = guid.data1();
    iarr[1] = guid.data2() << 16 | guid.data3();
    iarr[2] = guid.data4(0) << 24 | guid.data4(1) << 16 | guid.data4(2) << 8 | guid.data4(3);
    iarr[3] = guid.data4(4) << 24 | guid.data4(5) << 16 | guid.data4(6) << 8 | guid.data4(7);
  }
  else{
    msg(MSG::WARNING) << "property list has no ClassID for class name " << clname << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigSerializeGuidHelper::IntsToClassName(const uint32_t *iarr, std::string &clname){

  Guid guid;
  guid.setData1(iarr[0]);
  guid.setData2(iarr[1] >> 16);
  guid.setData3(iarr[1] & 0xFFFF);
  guid.setData4(iarr[2] >> 24, 0);
  guid.setData4(iarr[2] >> 16, 1);
  guid.setData4(iarr[2] >> 8, 2);
  guid.setData4(iarr[2] & 0xFF, 3);
  guid.setData4(iarr[3] >> 24, 4);
  guid.setData4(iarr[3] >> 16, 5);
  guid.setData4(iarr[3] >> 8, 6);
  guid.setData4(iarr[3] & 0xFF, 7);

  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "constructed " << guid.toString() << " from ints" << endreq; 
  ROOT::Reflex::Type cltype(pool::DbReflex::forGuid(guid));
  
  clname = cltype.Name();
  std::string scope = cltype.DeclaringScope().Name();
  if (!scope.empty()) {
    clname = scope+"::"+clname;
  }
  if (msgLvl(MSG::DEBUG))
    msg(MSG::DEBUG) << "corresponds to " << clname << endreq; 

  if (clname.empty()){
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
