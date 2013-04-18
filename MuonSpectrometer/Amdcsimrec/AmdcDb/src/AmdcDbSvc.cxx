/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbSvc.h"

AmdcDbSvc::AmdcDbSvc(){}

AmdcDbSvc::~AmdcDbSvc()
{

  std::map< std::string, IRDBRecordset*>::iterator it = m_MapOfIRDBRecordset.begin();
  for(; it!=m_MapOfIRDBRecordset.end(); it++) delete it->second;

}

// Get
const IRDBRecordset* AmdcDbSvc::getRecordset(const std::string& NameOfTheSet) const{

  if (m_MapOfIRDBRecordset.find(NameOfTheSet) != m_MapOfIRDBRecordset.end()){
    return m_MapOfIRDBRecordset.find(NameOfTheSet)->second;
  }
  return 0 ;

}

// Set
void AmdcDbSvc::addIRDBRecordset(
                     std::string NameOfTheSet, 
		     IRDBRecordset* pIRDBRecordset
){

  m_MapOfIRDBRecordset[NameOfTheSet] = pIRDBRecordset;

}
