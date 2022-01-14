/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCDB_AMDCDB2SQL_H
#define AMDCDB_AMDCDB2SQL_H

class IRDBAccessSvc;

  /**
   @class AmdcDb2Sql

   This class is  producing Sql files
   
  @author samusog@cern.ch
  
  */

class AmdcDb2Sql{
public:
    AmdcDb2Sql();
    ~AmdcDb2Sql();

public:
///////////////////////////////////

   /**Set */
   void SetAsNewTableOn(int AsNewTableOn);

   /**Set */
   void SetUseKeysOn(int UseKeysOn);
   
   /**Production of sql files */
   void DoIt(const std::string& TagFileName,const std::string& tag,IRDBAccessSvc* pIRDBAccessSvc);

private:
///////////////////////////////////

   /**Base Production of sql files */
   void DoSql(const std::string& TagFileName,const std::string& NameOfTheSet,const std::string& tag, IRDBAccessSvc* pIRDBAccessSvc);

   int m_AsNewTableOn ; //!< Control swl file prod 
   int m_UseKeysOn    ; //!< Control swl file prod 

};

#endif
