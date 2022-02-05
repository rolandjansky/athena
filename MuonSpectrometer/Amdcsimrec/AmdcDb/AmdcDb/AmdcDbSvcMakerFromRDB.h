/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AMDCDB_AMDCDBSVCMAKERFROMRDB_H
#define AMDCDB_AMDCDBSVCMAKERFROMRDB_H

#include <string>
#include <map>

class IRDBAccessSvc;
class AmdcDbSvc;
class IRDBRecord;
class AmdcDbRecord;
namespace AmdcDb_detail {
  struct VarDesc;
}

  /**
   @class AmdcDbSvcMakerFromRDB

   This class is a class to set instantiation of  AmdcDbSvc from an IRDBAccessSvc
   
  @author samusog@cern.ch
  
  */

class AmdcDbSvcMakerFromRDB{
public:
    AmdcDbSvcMakerFromRDB();
    ~AmdcDbSvcMakerFromRDB()=default;

public:
///////////////////////////////////

// Set a AmdcDbSvc
   void SetEpsLengthMM(int EpsLengthMM);
   void SetEpsLengthCM(int EpsLengthCM);
   void SetEpsAngle   (int EpsAngle   );
   void SetEpsLengthMM(const std::string& NameOfTheSet, int EpsLengthMM);
   void SetEpsLengthCM(const std::string& NameOfTheSet, int EpsLengthCM);
   void SetEpsAngle   (const std::string& NameOfTheSet, int EpsAngle   );
   void Set(
            const std::string& detectorKey  ,
            const std::string& detectorNode ,
            IRDBAccessSvc* pIRDBAccessSvc,
	    AmdcDbSvc* pAmdcDbSvc
	   );

private:
///////////////////////////////////

// Db Key and node ;
   std::string m_detectorKey  ;
   std::string m_detectorNode ;

   int m_EpsLengthMM ;
   int m_EpsLengthCM ;
   int m_EpsAngle    ;
   
   int GetEpsLengthMM(const std::string& NameOfTheSet) ;
   int GetEpsLengthCM(const std::string& NameOfTheSet) ;
   int GetEpsAngle   (const std::string& NameOfTheSet) ;
   
   void AMDC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void AGDD(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ATYP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ACUT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WRPC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void AWLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WTGC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void GGLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ATLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WCSC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void DBAM(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ASMP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WDED(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WLBI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WCRO(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WCMI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WCHV(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WSUP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WSPA(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void WMDT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ALIN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ALMN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void APTP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ASZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   void ISZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
 
   void AgddXMLVariables(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);

   void HwSwIdMapping(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);
   
   void XtomoData(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc);

   void addVars (const std::string& NameOfTheSet,
                 const AmdcDb_detail::VarDesc* beg,
                 const AmdcDb_detail::VarDesc* end,
                 IRDBAccessSvc* pIRDBAccessSvc,
                 AmdcDbSvc* pAmdcDbSvc);


   std::map< std::string, int> m_Map_Set_EpsLengthMM  ;  
   std::map< std::string, int> m_Map_Set_EpsLengthCM  ;  
   std::map< std::string, int> m_Map_Set_EpsAngle     ;  
   
};

#endif
