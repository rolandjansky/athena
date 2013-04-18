/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcDbSvc_H
#define AmdcDbSvc_H

#include "AmdcDb/AmdcDbMisc.h"
#include "RDBAccessSvc/IRDBRecordset.h"

  /**
   @class AmdcDbSvc

   This class is managing set of AmdcDbRecordset
   
  @author samusog@cern.ch
  
  */

class AmdcDbSvc{
public:
    AmdcDbSvc();
    virtual ~AmdcDbSvc();

public:
///////////////////////////////////

   /**Get */
   const IRDBRecordset* getRecordset(const std::string& NameOfTheSet) const;

   /**Set */
   void addIRDBRecordset(
                     std::string NameOfTheSet, 
		     IRDBRecordset* pIRDBRecordset
		    );


private:
///////////////////////////////////
//Data

   std::map< std::string, IRDBRecordset*> m_MapOfIRDBRecordset ; //!< Map of IRDBRecordset

};

#endif
