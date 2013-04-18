/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcDb2Sql_H
#define AmdcDb2Sql_H

#include "AmdcDb/AmdcDbMisc.h"
class IRDBAccessSvc;

  /**
   @class AmdcDb2Sql

   This class is  producing Sql files
   
  @author samusog@cern.ch
  
  */

class AmdcDb2Sql{
public:
    AmdcDb2Sql();
    virtual ~AmdcDb2Sql();

public:
///////////////////////////////////

   /**Set */
   void SetAsNewTableOn(int AsNewTableOn);

   /**Set */
   void SetUseKeysOn(int UseKeysOn);
   
   /**Production of sql files */
   void DoIt(std::string TagFileName,std::string tag,IRDBAccessSvc* pIRDBAccessSvc);

private:
///////////////////////////////////

   /**Base Production of sql files */
   void DoSql(std::string TagFileName,std::string NameOfTheSet,std::string tag, IRDBAccessSvc* pIRDBAccessSvc);

   int m_AsNewTableOn ; //!< Control swl file prod 
   int m_UseKeysOn    ; //!< Control swl file prod 

};

#endif
