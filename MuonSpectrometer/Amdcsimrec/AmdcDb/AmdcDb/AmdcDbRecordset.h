/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCDB_AMDCDBRECORDSET_H
#define AMDCDB_AMDCDBRECORDSET_H

#include "RDBAccessSvc/IRDBRecordset.h"
#include <string>

  /**
   @class AmdcDbRecordset

   This class is an implementation of the class IRDBRecordset
   
  @author samusog@cern.ch
  
  */

class AmdcDbRecordset : public IRDBRecordset{
public:
    AmdcDbRecordset();
    virtual ~AmdcDbRecordset();

public:
///////////////////////////////////

// Set
   void addIRDBRecord(IRDBRecord* pIRDBRecord);

// Return size, begin and end
   virtual unsigned int size() const;
   IRDBRecordset::const_iterator begin() const;
   IRDBRecordset::const_iterator end() const;

   const IRDBRecord* operator[](unsigned int index) const;

//Functions 
//  of IRDBRecordset Not implemented
  virtual std::string nodeName() const ;
  virtual std::string tagName()  const ;
  virtual void        ref()      const ;
  virtual void        unref()    const ;

private:
///////////////////////////////////
//Data
   RecordsVector              m_records;

};

#endif
