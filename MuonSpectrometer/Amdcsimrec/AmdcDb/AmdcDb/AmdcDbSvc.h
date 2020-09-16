/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCDB_AMDCDBSVC_H
#define AMDCDB_AMDCDBSVC_H

#include "AmdcDb/AmdcDbMisc.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"

class pIRDBRecordset;

/**
 * @class AmdcDbSvc
 *
 * This class is managing set of AmdcDbRecordset
 * 
 * @author samusog@cern.ch
 *
 */

class AmdcDbSvc{
 public:
  AmdcDbSvc();
  ~AmdcDbSvc();

  IRDBRecordset_ptr getRecordset(const std::string& NameOfTheSet) const;

  void addIRDBRecordset(std::string NameOfTheSet
			, IRDBRecordset* pIRDBRecordset);
  
 private:
  std::map<std::string, IRDBRecordset_ptr> m_mapOfIRDBRecordset;
  IRDBRecordset_ptr m_emptyRecordset;
};

#endif
