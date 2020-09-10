/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbSvc.h"

AmdcDbSvc::AmdcDbSvc()
  : m_emptyRecordset(new AmdcDbRecordset())
{}

AmdcDbSvc::~AmdcDbSvc()
{}

IRDBRecordset_ptr AmdcDbSvc::getRecordset(const std::string& NameOfTheSet) const
{
  auto it = m_mapOfIRDBRecordset.find(NameOfTheSet);
  return it!=m_mapOfIRDBRecordset.end()?it->second:m_emptyRecordset;
}

void AmdcDbSvc::addIRDBRecordset(std::string NameOfTheSet
				 , IRDBRecordset* pIRDBRecordset)
{
  m_mapOfIRDBRecordset.emplace(NameOfTheSet,IRDBRecordset_ptr(pIRDBRecordset));
}
