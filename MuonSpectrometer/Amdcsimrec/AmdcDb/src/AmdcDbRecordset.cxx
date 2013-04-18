/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbRecord.h"

AmdcDbRecordset::AmdcDbRecordset(){}

AmdcDbRecordset::~AmdcDbRecordset()
{

  for(unsigned int ind=0; ind<m_records.size(); ind++)
    delete m_records[ind];
    
}

// Set
void AmdcDbRecordset::addIRDBRecord(IRDBRecord* pIRDBRecord){m_records.push_back(pIRDBRecord);}


// Return size, begin and end
unsigned int AmdcDbRecordset::size() const 
{

  return m_records.size();
    
}
IRDBRecordset::const_iterator AmdcDbRecordset::begin() const
{
  return m_records.begin();
}

IRDBRecordset::const_iterator AmdcDbRecordset::end() const
{
  return m_records.end();
}
const IRDBRecord* AmdcDbRecordset::operator[](unsigned int index) const 
{
  return m_records[index];
}

//Functions 
//  of IRDBRecordset Not implemented
std::string AmdcDbRecordset::nodeName() const{std::cout << " AmdcDbRecordset::nodeName() Not implemented " << std::endl ;std::string ToBeReturned = "Not Implemented";return ToBeReturned;}
std::string AmdcDbRecordset::tagName()  const{std::cout << " AmdcDbRecord::AmdcDbRecordset::tagName() Not implemented " << std::endl ;std::string ToBeReturned = "Not Implemented";return ToBeReturned;}
void        AmdcDbRecordset::ref()      const{std::cout << " AmdcDbRecord::AmdcDbRecordset::ref() Not implemented " << std::endl ;}
void        AmdcDbRecordset::unref()    const{std::cout << " AmdcDbRecord::AmdcDbRecordset::unref() Not implemented " << std::endl ;}
