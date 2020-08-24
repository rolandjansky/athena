/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AmdcDbSvcMakerFromAmdc_H
#define AmdcDbSvcMakerFromAmdc_H

#include "AmdcDb/AmdcDbMisc.h"
class Amdcsimrec;
class AmdcDbSvc;
class AmdcDbRecordset;
class AmdcDbRecord;

  /**
   @class AmdcDbSvcMakerFromAmdc

   This class is a class to set instantiation of  AmdcDbSvc from an Amdcsimrec interface
   
  @author samusog@cern.ch
  
  */

class AmdcDbSvcMakerFromAmdc{
public:
    AmdcDbSvcMakerFromAmdc();
    virtual ~AmdcDbSvcMakerFromAmdc()=default;

public:
///////////////////////////////////

// Set a AmdcDbSvc
   void SetUglyCodeOn(int UglyCodeOn);
   void SetEpsLengthMM(int EpsLengthMM);
   void SetEpsLengthCM(int EpsLengthCM);
   void SetEpsAngle   (int EpsAngle   );
   void SetEpsLengthMM(std::string NameOfTheSet, int EpsLengthMM);
   void SetEpsLengthCM(std::string NameOfTheSet, int EpsLengthCM);
   void SetEpsAngle   (std::string NameOfTheSet, int EpsAngle   );
   void Set(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);

private:
///////////////////////////////////
   int m_version             ;
   long m_UniversalIdKounter ;

   int m_UglyCodeOn ;

   int m_EpsLengthMM ;
   int m_EpsLengthCM ;
   int m_EpsAngle    ;

   int GetEpsLengthMM(std::string NameOfTheSet) ;
   int GetEpsLengthCM(std::string NameOfTheSet) ;
   int GetEpsAngle   (std::string NameOfTheSet) ;
   
   void AMDC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void AGDD(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ATYP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ACUT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WRPC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void AWLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WTGC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void GGLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ATLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WCSC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void DBAM(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ASMP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WDED(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WLBI(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WCRO(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WCMI(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WCHV(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WSUP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WSPA(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void WMDT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ALIN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ALMN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void APTP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);

   void ASZT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   void ISZT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);

   void HwSwIdMapping(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   
   void XtomoData(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc);
   
   std::map< std::string, int> m_Map_Set_EpsLengthMM  ;  
   std::map< std::string, int> m_Map_Set_EpsLengthCM  ;  
   std::map< std::string, int> m_Map_Set_EpsAngle     ;  
   
};

#endif
