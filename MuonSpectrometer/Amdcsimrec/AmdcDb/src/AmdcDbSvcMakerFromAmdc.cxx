/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbSvcMakerFromAmdc.h"
#include "AmdcCore/Amdcsimrec.h"
#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbRecord.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/getMessageSvc.h"

#include <cmath>
 
AmdcDbSvcMakerFromAmdc::AmdcDbSvcMakerFromAmdc(){

 m_version = 7 ;
 m_UniversalIdKounter = 0 ;

 m_UglyCodeOn  = 1 ;

 m_EpsLengthMM = 4 ;
 m_EpsLengthCM = 5 ;
 m_EpsAngle    = 6 ;

 m_Map_Set_EpsLengthMM["Dummy"] = 0 ; m_Map_Set_EpsLengthMM.clear() ;
 m_Map_Set_EpsLengthCM["Dummy"] = 0 ; m_Map_Set_EpsLengthCM.clear() ;
 m_Map_Set_EpsAngle   ["Dummy"] = 0 ; m_Map_Set_EpsAngle.clear()    ;

}

// Set a AmdcDbSvc
void AmdcDbSvcMakerFromAmdc::SetUglyCodeOn(int UglyCodeOn){ m_UglyCodeOn  = UglyCodeOn ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthMM(int EpsLengthMM){ m_EpsLengthMM  = EpsLengthMM ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthCM(int EpsLengthCM){ m_EpsLengthCM  = EpsLengthCM ;}
void AmdcDbSvcMakerFromAmdc::SetEpsAngle(int EpsAngle){ m_EpsAngle  = EpsAngle ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthMM(std::string NameOfTheSet, int EpsLengthMM) { m_Map_Set_EpsLengthMM[NameOfTheSet] = EpsLengthMM ; }
void AmdcDbSvcMakerFromAmdc::SetEpsLengthCM(std::string NameOfTheSet, int EpsLengthCM) { m_Map_Set_EpsLengthCM[NameOfTheSet] = EpsLengthCM ; }
void AmdcDbSvcMakerFromAmdc::SetEpsAngle   (std::string NameOfTheSet, int EpsAngle   ) { m_Map_Set_EpsAngle   [NameOfTheSet] = EpsAngle    ; }
void AmdcDbSvcMakerFromAmdc::Set(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  AMDC(pAmdcsimrec,pAmdcDbSvc);
  AGDD(pAmdcsimrec,pAmdcDbSvc);

  ATYP(pAmdcsimrec,pAmdcDbSvc);
  ACUT(pAmdcsimrec,pAmdcDbSvc);

  WRPC(pAmdcsimrec,pAmdcDbSvc);
  AWLN(pAmdcsimrec,pAmdcDbSvc);
  WTGC(pAmdcsimrec,pAmdcDbSvc);
  GGLN(pAmdcsimrec,pAmdcDbSvc);
  ATLN(pAmdcsimrec,pAmdcDbSvc);
  WCSC(pAmdcsimrec,pAmdcDbSvc);
  DBAM(pAmdcsimrec,pAmdcDbSvc);
  ASMP(pAmdcsimrec,pAmdcDbSvc);
  WDED(pAmdcsimrec,pAmdcDbSvc);
  WLBI(pAmdcsimrec,pAmdcDbSvc);
  WCRO(pAmdcsimrec,pAmdcDbSvc);
  WCMI(pAmdcsimrec,pAmdcDbSvc);
  WCHV(pAmdcsimrec,pAmdcDbSvc);
  WSUP(pAmdcsimrec,pAmdcDbSvc);
  WSPA(pAmdcsimrec,pAmdcDbSvc);
  WMDT(pAmdcsimrec,pAmdcDbSvc);
  ALIN(pAmdcsimrec,pAmdcDbSvc);
  ALMN(pAmdcsimrec,pAmdcDbSvc);
  APTP(pAmdcsimrec,pAmdcDbSvc);

  ASZT(pAmdcsimrec,pAmdcDbSvc);
  ISZT(pAmdcsimrec,pAmdcDbSvc);

  HwSwIdMapping(pAmdcsimrec,pAmdcDbSvc);

  XtomoData(pAmdcsimrec,pAmdcDbSvc);
  
}

int AmdcDbSvcMakerFromAmdc::GetEpsLengthMM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthMM.find(NameOfTheSet) != m_Map_Set_EpsLengthMM.end()) return m_Map_Set_EpsLengthMM.find(NameOfTheSet)->second ; return m_EpsLengthMM; }
int AmdcDbSvcMakerFromAmdc::GetEpsLengthCM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthCM.find(NameOfTheSet) != m_Map_Set_EpsLengthCM.end()) return m_Map_Set_EpsLengthCM.find(NameOfTheSet)->second ; return m_EpsLengthCM; }
int AmdcDbSvcMakerFromAmdc::GetEpsAngle   (std::string NameOfTheSet){ if (m_Map_Set_EpsAngle.find(NameOfTheSet)    != m_Map_Set_EpsAngle.end()   ) return m_Map_Set_EpsAngle.find(NameOfTheSet)->second    ; return m_EpsAngle   ; }

void AmdcDbSvcMakerFromAmdc::AMDC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AMDC";

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

  DbVar = "VERS"    ; DbVarComment="VERSION"  	     ; iDbVal = m_version                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  std::string TheAmdcName = pAmdcsimrec->AmdcName() ;

  DbVar = "VNAME"   ; DbVarComment="NAME"	     ; sDbVal  = TheAmdcName.substr(0,4)       ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

  std::string TheBlob = pAmdcsimrec->GetAmdcString() ;
  
  DbVar = "LENAMDC" ; DbVarComment="STRING LENGTH"   ; iDbVal = TheBlob.size()	               ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
  DbVar = "NLINE"   ; DbVarComment="CHAR4 NUMBER"    ; iDbVal = int ( (TheBlob.size()+2.)/4. ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  DbVar = "DATA"    ; DbVarComment="(NLINE)-ASCII"   ; sDbVal = TheBlob                        ; pAmdcDbRecord->addBlob(DbVar,DbVarComment,sDbVal);
 
  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::AGDD(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){
  std::string NameOfTheSet = "AGDD";
  std::string DbVar       = "";
  std::string DbVarComment = "";
  int iDbVal       = 0 ;
  std::string sDbVal      = ""; 
  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();
  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  DbVar = "VERS"    ; DbVarComment="VERSION"        ; iDbVal = m_version                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  std::string TheAmdcName = pAmdcsimrec->AmdcName() ;
  DbVar = "VNAME"   ; DbVarComment="NAME"           ; sDbVal  = TheAmdcName.substr(0,4)       ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
  std::string TheBlob = pAmdcsimrec->GetAgddString() ;
  DbVar = "LENAGDD" ; DbVarComment="STRING LENGTH"   ; iDbVal = TheBlob.size()                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  DbVar = "NLINE"   ; DbVarComment="CHAR4 NUMBER"    ; iDbVal = int ( (TheBlob.size()+2.)/4. ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  DbVar = "DATA"    ; DbVarComment="(NLINE)-ASCII"   ; sDbVal = TheBlob                        ; pAmdcDbRecord->addBlob(DbVar,DbVarComment,sDbVal);
  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);
}

void AmdcDbSvcMakerFromAmdc::ATYP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ATYP";
  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    int iDbVal_NSTA = 0;
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){

//UGLY: 
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0) iDbVal_NSTA = DB_INDX;

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
int Ifound = 0 ;  
int DB_IZ_Max = pAmdcsimrec->Mzz();
int DB_JFF_Max = 8;
for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
  for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)!=0){
      if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) == DB_INDX){
       Ifound = 1 ;
       break;
      }
    }
  }
  if (Ifound==1) break;
}
if (Ifound == 1 ){

      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0) iDbVal_NSTA = DB_INDX;

}
}
//UGLY: 

    }
    if ( iDbVal_NSTA != 0 ){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
      DbVar = "VERS"    ; DbVarComment="VERSION"  		            ; iDbVal = m_version                                  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "JTYP"    ; DbVarComment="AMDB STATION TYPE"                  ; iDbVal = DB_JTYP                                    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "TYP"     ; DbVarComment="AMDB STATION NAME"	            ; sDbVal = sDbVal = pAmdcsimrec->StationName(DB_JTYP) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

//DRING: What is it? This a guess     						        
      DbVar = "NSTA"    ; DbVarComment="NUMBER OF STATIONS OF THIS TYPE"    ; iDbVal = iDbVal_NSTA                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }
 
  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ACUT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ACUT";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  int DB_ICUT_Max = 4;
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){
      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0){
        for (int DB_ICUT=1 ; DB_ICUT<=DB_ICUT_Max ; DB_ICUT++){
          if (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT)!=0){

//UGLY: 
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

            m_UniversalIdKounter = m_UniversalIdKounter + 1;
            AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
            DbVar = "VERS" ; DbVarComment="VERSION"			      ; iDbVal = m_version	                                                   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            DbVar = "I"    ; DbVarComment="INDEX"			      ; iDbVal = DB_INDX                                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
            DbVar = "ICUT" ; DbVarComment="CUT-OUT INDEX"		      ; iDbVal = DB_ICUT                                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
            DbVar = "N"    ; DbVarComment="NUMBER OF SUB-CUTS IN THE STATION" ; iDbVal = pAmdcsimrec->NOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT)) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
int Ifound = 0 ;  
int DB_IZ_Max = pAmdcsimrec->Mzz();
int DB_JFF_Max = 8;
for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
  for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)!=0){
      if (pAmdcsimrec->ICUT   (DB_JTYP,DB_JFF,DB_IZ) == DB_ICUT
      &&  pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) == DB_INDX) {
        Ifound = 1 ;
        break ;
      }
    }
  }
  if (Ifound==1) break;
}
if (Ifound == 1 ){

            m_UniversalIdKounter = m_UniversalIdKounter + 1;
            AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
            DbVar = "VERS" ; DbVarComment="VERSION"			      ; iDbVal = m_version	                                                   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            DbVar = "I"    ; DbVarComment="INDEX"			      ; iDbVal = DB_INDX                                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
            DbVar = "ICUT" ; DbVarComment="CUT-OUT INDEX"		      ; iDbVal = DB_ICUT                                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
            DbVar = "N"    ; DbVarComment="NUMBER OF SUB-CUTS IN THE STATION" ; iDbVal = pAmdcsimrec->NOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT)) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

}
}
//UGLY: 
          }
        }
      }
    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WRPC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WRPC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "RPC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
  int  DB_Jsta  = 1;

  DbVar = "VERS"    ; DbVarComment="VERSION"                               ; iDbVal = m_version                                 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it? This is a guess   
  DbVar = "NVRS"    ; DbVarComment="VERSION OF RPC TECHNOLOGY"             ; iDbVal = pAmdcsimrec->NLAS (DB_Jtec,DB_Jsta)       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it? This is a guess       
  DbVar = "LAYRPC"  ; DbVarComment="LAYERS NUMBER"                         ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  DbVar = "TCKRLA"  ; DbVarComment="THICK. OF AN RPC LAYER"                ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 4)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TOTTCK"  ; DbVarComment="TOTAL THICKNESS"                       ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 9)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TCKFSP"  ; DbVarComment="THICK. OF FOAM SPACER"                 ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 5)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "ACKFSP"  ; DbVarComment="THICK. OF AL PLATE OF FOAM SPACER"     ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 6)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TLOHCB"  ; DbVarComment="THICK. OF LOWER HONEYCOMB"             ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 7)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "ALOHCB"  ; DbVarComment="THICK. OF AL PLATE OF LOWER HONEYCOMB" ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 8)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TCKBAK"  ; DbVarComment="THICK. OF BAKELITE"                    ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,13)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TCKGAS"  ; DbVarComment="THICK. OF GAS GAP "                    ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,14)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TCKSSU"  ; DbVarComment="THICK. OF STRIPS SUPPORT"              ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,15)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "TCKSTR"  ; DbVarComment="THICK. OF STRIPS"                      ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,16)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "SDEDMI"  ; DbVarComment="S INTERNAL MID-CHBER DEAD REGION"      ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,17)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "ZDEDMI"  ; DbVarComment="Z INTERNAL MID-CHBER DEAD REGION"      ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,18)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "SPDIAM"  ; DbVarComment="SPACER DIAMETER"                       ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,19)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "SPPITC"  ; DbVarComment="SPACER PITCH"                          ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,20)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  DbVar = "STROFF_0"; DbVarComment="STRIP OFFSET S, FIRST Z, SECOND Z"     ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 1)	; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
  DbVar = "STROFF_1"; DbVarComment=""                                      ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 4)	; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
  DbVar = "STROFF_2"; DbVarComment=""                                      ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 5)	; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::AWLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AWLN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "RPC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
      DbVar = "VERS"   ; DbVarComment="VERSION"                        ; iDbVal = m_version                                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"   ; DbVarComment="JSTA TYPE NUMBER"               ; iDbVal = DB_Jsta                                        ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "SPITCH" ; DbVarComment="S-STRIPS PITCH"                 ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,10)/10.     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "ZPITCH" ; DbVarComment="Z-STRIPS PITCH"                 ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,11)/10.     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "DEDSTR" ; DbVarComment="DEAD REAGION BETWEEN STRIP"     ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,12)/10.     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      DbVar = "NSREST" ; DbVarComment="NBER OF S STRIPS READOUTS"      ; iDbVal = int ( pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 2) ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NZREST" ; DbVarComment="NBER OF S GAS GAPS"             ; iDbVal = int ( pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 3) ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "SFIRST" ; DbVarComment="S-PHI STRIP OFFSET"             ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,11)         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "ZFIRST" ; DbVarComment="Z-ETA STRIP OFFSET"             ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,12)         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "DEDSEP" ; DbVarComment="DEAD SEPARATION"                ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,13)         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      DbVar = "NSROST" ; DbVarComment="NUMBER OF S-PHI READOUT STRIPS" ; iDbVal = int ( pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,14) ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NZROST" ; DbVarComment="NUMBER OF Z-ETA READOUT STRIPS" ; iDbVal = int ( pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,15) ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);


      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WTGC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WTGC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "TGC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
      DbVar = "VERS"    ; DbVarComment="VERSION"                    ; iDbVal = m_version                              ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"    ; DbVarComment="JSTA INDEX"                 ; iDbVal = DB_Jsta                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NBEVOL"  ; DbVarComment="NUMBER OF DETAILS"          ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "X0"      ; DbVarComment="X0"                         ; dDbVal = pAmdcsimrec->STAX0(DB_Jtec,DB_Jsta)    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "WIDCHB"  ; DbVarComment="WIDTH OF THE CHBER ALONG Z" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "FWIRCH"  ; DbVarComment="FRAME WIDTH IN R"           ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "FWIXCH"  ; DbVarComment="FRAME WIDTH IN X"           ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/10.; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      for (int DB_Item=1; DB_Item<=9 ; DB_Item++){
        DbVar = "ALLNAME" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
        sDbVal  = ""; 
        if (DB_Item == 1 ) sDbVal = "Gas";
        if (DB_Item == 2 ) sDbVal = "G10";
        if (DB_Item == 3 ) sDbVal = "Hone";
        if (DB_Item == 4 ) sDbVal = "Copp";
        if (DB_Item == 5 ) sDbVal = "Carb";
        DbVarComment="";
        if (DB_Item == 1) DbVarComment="MATERIAL";
	
        pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

      }

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::GGLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "GGLN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "TGC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
      DbVar = "VERS"    ; DbVarComment="VERSION"                                  ; iDbVal = m_version                       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"    ; DbVarComment="JSTA INDEX"                               ; iDbVal = DB_Jsta                         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NWGS_0"  ; DbVarComment="N OF GROUPS OF WIRES (GANG) PER CH. IN E" ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 1); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NWGS_1"  ; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 2); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NWGS_2"  ; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 3); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NSPS_0"  ; DbVarComment="N OF STRIPS PER CHAMBER"                  ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 4); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NSPS_1"  ; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 5); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NSPS_2"  ; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 6); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "ROFFST_0"; DbVarComment="OFFSET OF WIRE GROUP ADDRESS 1"           ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 7); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "ROFFST_1"; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 8); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "ROFFST_2"; DbVarComment=""                                         ; iDbVal = pAmdcsimrec->NtgcAdd(DB_Jsta, 9); pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?  From SB reserved but never used     
      DbVar = "POFFST_0"; DbVarComment=""                                         ; dDbVal = 0.                              ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

//DRING: What is it?  From SB reserved but never used     
      DbVar = "POFFST_1"; DbVarComment=""                                         ; dDbVal = 0.                              ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

//DRING: What is it?  From SB reserved but never used    
     DbVar = "POFFST_2"; DbVarComment=""                                          ; dDbVal = 0.                              ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      DbVar = "WIRESP"  ; DbVarComment="WIRE SPACING [MM]"                        ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 4); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "STRIPSP" ; DbVarComment="STRIP SPACING [MM]"                       ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 5); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      for (int DB_IWGS=1; DB_IWGS<=3 ; DB_IWGS++){
        for (int DB_Item=1; DB_Item<=130 ; DB_Item++){
          DbVar = "IWGS" ;
          std::ostringstream Aostringstream_F1;
          Aostringstream_F1 << DB_IWGS ;
          DbVar = DbVar+Aostringstream_F1.str();
          DbVar = DbVar+"_";
          std::ostringstream Aostringstream_F2;
	  int DB_Item_Shifted = DB_Item - 1 ;
          Aostringstream_F2 << DB_Item_Shifted ;
          DbVar = DbVar+Aostringstream_F2.str();
	  iDbVal = pAmdcsimrec->NtgcGang(DB_Jsta,DB_Item,DB_IWGS); 
          DbVarComment="";
          if ( DB_IWGS==1 && DB_Item==1 ) DbVarComment="IWGS1 WIRE GANGING";
          if ( DB_IWGS==2 && DB_Item==1 ) DbVarComment="IWGS2 WIRE GANGING";
          if ( DB_IWGS==3 && DB_Item==1 ) DbVarComment="IWGS3 WIRE GANGING";
	  
          pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        }
      }
    
      DbVar = "PDIST"   ; DbVarComment="! PHYSICAL DISTANCE OF STRIPS W.R.T. BAS" ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 1); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "S1PP"    ; DbVarComment="WIRE SUPPORT : WIDTH"                     ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 6); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "S2PP"    ; DbVarComment="WIRE SUPPORT : WIDTH OF GAS CHANNEL"      ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 7); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "WSEP"    ; DbVarComment="WIRE SUPPORT : SEPARATION"                ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 8); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP1WI"   ; DbVarComment="WIRE SUPPORT : OFFSET OF WIRE SUPPORT LA" ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta, 9); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP2WI"   ; DbVarComment="WIRE SUPPORT : LAYER 2"                   ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,10); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP3WI"   ; DbVarComment="WIRE SUPPORT : LAYER 3"                   ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,11); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "TILT"    ; DbVarComment="WIRE SUPPORT : TILT ANGLE (DEG)"          ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,12); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP1BU"   ; DbVarComment="BUTTON SUPPORT : RADIUS"                  ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,13); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP2BU"   ; DbVarComment="BUTTON SUPPORT :SEPARATION"               ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,14); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP3BU"   ; DbVarComment="BUTTON SUPPORT : PITCH"                   ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,15); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "SP4BU"   ; DbVarComment="BUTTON SUPPORT : ANGLE IN TRAPEZOID REGI" ; dDbVal = pAmdcsimrec->XtgcAdd(DB_Jsta,16); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      for (int DB_Item=1; DB_Item<=33 ; DB_Item++){
        DbVar = "SLARGE" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = pAmdcsimrec->XtgcStrp(DB_Jsta,DB_Item,1); 
        DbVarComment="";
        if (DB_Item==1) DbVarComment="STRIPS LAYOUT";

        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      }
    
      for (int DB_Item=1; DB_Item<=33 ; DB_Item++){
        DbVar = "SHORT" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = pAmdcsimrec->XtgcStrp(DB_Jsta,DB_Item,2); 
        DbVarComment="";
        if (DB_Item==1) DbVarComment="START OF STRIP ON LARGE BASE 1ST PLANE";

        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      }

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ATLN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ATLN";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "TGC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      for (int DB_I=1 ; DB_I<=pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta) ; DB_I++){
        m_UniversalIdKounter = m_UniversalIdKounter + 1;
        AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

        DbVar = "VERS"    ; DbVarComment="VERSION"             ; iDbVal = m_version                                       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
        DbVar = "I"       ; DbVarComment="INDEX"               ; iDbVal = DB_I                                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
        DbVar = "ICOVOL"  ; DbVarComment="MATERIAL CODE"       ; iDbVal = pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I)	  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        DbVar = "ZPOVOL"  ; DbVarComment="RELATIVE Z POSITION" ; dDbVal = pAmdcsimrec->STATT (DB_Jtec,DB_Jsta,DB_I) / 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
        DbVar = "WIDVOL"  ; DbVarComment="Z WIDTH"             ; dDbVal = pAmdcsimrec->STAOO (DB_Jtec,DB_Jsta,DB_I) / 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      
        DbVar = "NAMVOL"  ; DbVarComment="MATERIAL NAME" ;
        sDbVal ="?";
        if (pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I) == 1 ) sDbVal = "Gas";
        if (pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I) == 2 ) sDbVal = "G10";
        if (pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I) == 3 ) sDbVal = "Hone";
        if (pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I) == 4 ) sDbVal = "Copp";
        if (pAmdcsimrec->ISTAMA(DB_Jtec,DB_Jsta,DB_I) == 5 ) sDbVal = "Carb";
        pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

        DbVar = "JSTA"    ; DbVarComment="JSTA INDEX "         ; iDbVal = DB_Jsta                                         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
        pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
      }

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WCSC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCSC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "CSC";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);


      DbVar = "VERS"	; DbVarComment="VERSION"                           ; iDbVal = m_version			                  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"	; DbVarComment="JSTA TYPE NUMBER"                  ; iDbVal = DB_Jsta			                  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "LAYCSC"  ; DbVarComment="NBER OF CSCS LAYERS"               ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "TTOTAL"  ; DbVarComment="TOTAL THICKNESS"                   ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 1)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TNOMEX"  ; DbVarComment="NOMEX HONEYCOMB THICKNESS"         ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 2)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TLAG10"  ; DbVarComment="G10 LAMINATES THICKNESS"           ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 3)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WISPA"   ; DbVarComment="WIRE SPACING"                      ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 4)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "DANCAT"  ; DbVarComment="ANODE-CATHODE DISTANCE"            ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 5)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "PCATRE"  ; DbVarComment="CATHODE READOUT PITCH"             ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 6)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      DbVar = "AZCAT"   ; DbVarComment="CATHODE READOUT PITCH, AZIMUTAL"   ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 4)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      DbVar = "GSTRIP"  ; DbVarComment="GAP BETWEEN CATHODE STRIPS"        ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 7)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WRESTR"  ; DbVarComment="WIDTH OF READOUT STRIPS"           ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 8)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WFLSTR"  ; DbVarComment="WIDTH OF FLOATING STRIPS"          ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta, 9)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TRRWAS"  ; DbVarComment="RIGIT RECTANGULAR WASHER THICKNES" ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,10)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WROXA"   ; DbVarComment="ROXACELL WIDTH"                    ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,11)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "GROXWI"  ; DbVarComment="ROXACELL AND WIRE BAR GAP"         ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,12)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WGASBA"  ; DbVarComment="FULL GAS GAP BAR WIDTH"            ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,14)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TGASBA"  ; DbVarComment="FULL GAS GAP BAR THICK."           ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,13)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WGASCU"  ; DbVarComment="CUTS GAS GAP BAR WIDTH"            ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,15)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TGASCU"  ; DbVarComment="CUTS GAS GAP BAR THICK."           ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,16)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "WFIXWI"  ; DbVarComment="FULL WIRE FIX. BAR WID."           ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,17)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TFIXWI"  ; DbVarComment="FULL WIRE FIX. BAR THICK."         ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,18)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "PBA1WI"  ; DbVarComment="WIRE BAR POSITION"                 ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 1)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "PBA2WI"  ; DbVarComment="WIRE BAR POSITION"                 ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 2)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "PBA3WI"  ; DbVarComment="WIRE BAR POSITION"                 ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta, 3)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

//DRING: What is it?  From SB reserved but never used     
      DbVar = "PSNDCO"  ; DbVarComment="2ND COORDINATE PITCH"              ; dDbVal = 0.                                          ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::DBAM(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "DBAM";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

  DbVar = "VERS"  ; DbVarComment="VERSION"                ; iDbVal = m_version		    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  DbVar = "NVRS"  ; DbVarComment="VERSION OF READING"     ; iDbVal = pAmdcsimrec->VersionAmdc() ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  std::string TheAmdcName = pAmdcsimrec->AmdcName() ;

  DbVar = "AMDB"  ; DbVarComment="AMDB NAME"              ; sDbVal  = TheAmdcName.substr(0,4)   ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

  DbVar = "TEST"  ; DbVarComment="TEST NAME"              ; sDbVal  = TheAmdcName.substr(5,2)   ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

  DbVar = "MTYP"  ; DbVarComment="MAXIMUM STATION NUMBER" ; 
  iDbVal = 0;
  for (int Jtyp=1; Jtyp<=pAmdcsimrec->Mtyp () ; Jtyp ++){
    std::string aStationName = pAmdcsimrec->StationName(Jtyp);
    if ( aStationName != "XXX") iDbVal = iDbVal + 1 ;
  }
  pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  int DB_MTYP = iDbVal ;
  for (int DB_Item=1; DB_Item<=DB_MTYP ; DB_Item++){
    DbVar = "NAME" ;
    DbVar = DbVar+"_";
    std::ostringstream Aostringstream;
    int DB_Item_Shifted = DB_Item - 1 ;
    Aostringstream << DB_Item_Shifted ;
    DbVar = DbVar+Aostringstream.str();
    sDbVal  = pAmdcsimrec->StationName(DB_Item); 
    DbVarComment="" ;
    if (DB_Item==1) DbVarComment="STATION NAME" ;

    pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

  }
  
//DRING: I do not know what is the meaning of this variable
  DbVar = "NUMBOX" ; DbVarComment="FILE INDEX" ; iDbVal = 63000 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);


  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ASMP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ASMP";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){
      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0){

//UGLY: 
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

        m_UniversalIdKounter = m_UniversalIdKounter + 1;
        AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

        DbVar = "VERS"    ; DbVarComment="VERSION"                      ; iDbVal = m_version			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
 
        DbVar = "INDX"    ; DbVarComment="STATION NUMBER (INSIDE TYPE)" ; iDbVal = DB_INDX			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        DbVar = "N"       ; DbVarComment="NUMBER OF ELEMENTS"           ; iDbVal = pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        DbVar = "JTYP"    ; DbVarComment="AMDB STATION TYPE"            ; iDbVal = DB_JTYP			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
int Ifound = 0 ;  
int DB_IZ_Max = pAmdcsimrec->Mzz();
int DB_JFF_Max = 8;
for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
  for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)!=0){
      if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) == DB_INDX) {
        Ifound = 1 ;
        break ;
      }
    }
  }
  if (Ifound==1) break;
}
if (Ifound == 1 ){
	
        m_UniversalIdKounter = m_UniversalIdKounter + 1;
        AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

        DbVar = "VERS"    ; DbVarComment="VERSION"                      ; iDbVal = m_version			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
 
        DbVar = "INDX"    ; DbVarComment="STATION NUMBER (INSIDE TYPE)" ; iDbVal = DB_INDX			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        DbVar = "N"       ; DbVarComment="NUMBER OF ELEMENTS"           ; iDbVal = pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        DbVar = "JTYP"    ; DbVarComment="AMDB STATION TYPE"            ; iDbVal = DB_JTYP			       ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

        pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

}
}
//UGLY: 
  
      }
    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WDED(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WDED";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "DED";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"   ; DbVarComment="VERSION"             ; iDbVal = m_version			          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"   ; DbVarComment="INDEX"               ; iDbVal = DB_Jsta			                  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NB"     ; DbVarComment="NUMBER OF DETAILS"   ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)        ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "X0"     ; DbVarComment="X0"                  ; dDbVal = pAmdcsimrec->STAX0(DB_Jtec,DB_Jsta)	  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "AUPHCB" ; DbVarComment="HONEYCOMB THICKNESS" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)  / 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TCKDED" ; DbVarComment="ALUMINIUM THICKNESS" ; dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,1)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WLBI(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WLBI";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "LB";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"      ; DbVarComment="VERSION"           ; iDbVal = m_version			         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
   
      DbVar = "JSTA"      ; DbVarComment="INDEX"             ; iDbVal = DB_Jsta			                 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NUM"       ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "HEIGHT"    ; DbVarComment="HEIGHT"            ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "THICKNESS" ; DbVarComment="WALL THICKNESS"    ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      DbVar = "LOWERTHICK"    ; DbVarComment="LOWER THICK"   ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "SHIFTYSTATION" ; DbVarComment="SHIFTY STATION"; dDbVal = pAmdcsimrec->STAPG(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WCRO(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCRO";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "CRO";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"       ; DbVarComment="VERSION"	      ; iDbVal = m_version				  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"       ; DbVarComment="INDEX"	      ; iDbVal = DB_Jsta				  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NUM"        ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)	  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"	      ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WCMI(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCMI";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "CMI";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"       ; DbVarComment="VERSION"           ; iDbVal = m_version		                  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"       ; DbVarComment="INDEX"             ; iDbVal = DB_Jsta			          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
   
      DbVar = "NUM"        ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"            ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WCHV(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCHV";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "CHV";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"       ; DbVarComment="VERSION"	      ; iDbVal = m_version				  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"       ; DbVarComment="INDEX"	      ; iDbVal = DB_Jsta				  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NUM"        ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)	  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"	      ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WSUP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WSUP";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "SUP";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"   ; DbVarComment="VERSION"                 ; iDbVal = m_version			           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"   ; DbVarComment="INDEX"                   ; iDbVal = DB_Jsta			           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NXXSUP" ; DbVarComment="MAX NB. FOR X FRAGMENTS" ; iDbVal = pAmdcsimrec->NLAS (DB_Jtec,DB_Jsta)	   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
      DbVar = "NZZSUP" ; DbVarComment="MAX NB. FOR Z FRAGMENTS" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)	   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "X0"     ; DbVarComment="X0"                      ; dDbVal = pAmdcsimrec->STAX0(DB_Jtec,DB_Jsta)	   ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "THICKN" ; DbVarComment="THICKNESS"               ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      
      for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
        DbVar = "XXSUP" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = 0. ;
	if (DB_Item<=pAmdcsimrec->NLAS (DB_Jtec,DB_Jsta)) dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,DB_Item)/10. ; 
        DbVarComment="" ;
	if (DB_Item==1) DbVarComment="X DIMENSION" ; 
 
        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      }

      for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
        DbVar = "ZZSUP" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = 0. ;
	if (DB_Item<=pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)) dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,DB_Item)/10. ; 
        DbVarComment="" ;
	if (DB_Item==1) DbVarComment="Z DIMENSION" ; 

        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      }

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WSPA(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WSPA";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "SPA";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"   ; DbVarComment="VERSION"             ; iDbVal = m_version			        ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "JSTA"   ; DbVarComment="JSTA INDEX"          ; iDbVal = DB_Jsta			                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "NB"     ; DbVarComment="NUMBER OF DETAILS"   ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "X0"     ; DbVarComment="X0"                  ; dDbVal = pAmdcsimrec->STAX0(DB_Jtec,DB_Jsta)	; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
      DbVar = "TCKSPA" ; DbVarComment="THICKNESS OF SPACER" ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::WMDT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WMDT";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DB_TecName = "MDT";
  int	      DB_Jtec	 = pAmdcsimrec->GetJtec(DB_TecName);

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jsta_Max = pAmdcsimrec->Msta();
  for (int DB_Jsta=1 ; DB_Jsta<=DB_Jsta_Max ; DB_Jsta++){
    if (pAmdcsimrec->IVALIDSTA(DB_Jtec,DB_Jsta)==1){
    
      m_UniversalIdKounter = m_UniversalIdKounter + 1;
      AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

      DbVar = "VERS"   ; DbVarComment="VERSION"              ; iDbVal = m_version			         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
      DbVar = "TYP"    ; DbVarComment="NAME"                 ; sDbVal = "MDT" 			                 ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
  
      DbVar = "IW"     ; DbVarComment="INDEX"                ; iDbVal = DB_Jsta			                 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "X0"     ; DbVarComment="X0"                   ; dDbVal = pAmdcsimrec->STAX0(DB_Jtec,DB_Jsta)	 ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

      DbVar = "LAYMDT" ; DbVarComment="MAXIMUM LAYER NUMBER" ; iDbVal = pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      DbVar = "TUBPIT" ; DbVarComment="PITCH BETWEEN TUBE"   ; dDbVal = pAmdcsimrec->STAPP(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TUBRAD" ; DbVarComment="RADIUS OF TUBE"       ; dDbVal = pAmdcsimrec->STARR(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TUBSTA" ; DbVarComment="THICKNESS OF TUBE"    ; dDbVal = pAmdcsimrec->STAEE(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      DbVar = "TUBDEA" ; DbVarComment="DEAD LENGTH IN TUBES" ; dDbVal = pAmdcsimrec->STAPG(DB_Jtec,DB_Jsta)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      
      for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
        DbVar = "TUBXCO" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = 0. ;
	if (DB_Item<=pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)) dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,DB_Item)/10.; 
        DbVarComment="" ;
	if (DB_Item==1) DbVarComment="Y TUBE POSITION" ;
	
        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      }
      
      for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
        DbVar = "TUBYCO" ;
        DbVar = DbVar+"_";
        std::ostringstream Aostringstream;
	int DB_Item_Shifted = DB_Item - 1 ;
        Aostringstream << DB_Item_Shifted ;
        DbVar = DbVar+Aostringstream.str();
	dDbVal = 0.;
	if (DB_Item<=pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)) dDbVal = pAmdcsimrec->STATT(DB_Jtec,DB_Jsta,DB_Item)/10.; 
        DbVarComment="" ;
	if (DB_Item==1) DbVarComment="X TUBE POSITION" ;

        pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

      }

      DbVar = "TUBWAL" ; DbVarComment="TUBE WALL THICKNESS" ; dDbVal = pAmdcsimrec->STAOO(DB_Jtec,DB_Jsta,pAmdcsimrec->NLAZ (DB_Jtec,DB_Jsta)+1)/ 10. ; 
                                                                                                              pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
      pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ALIN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ALIN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  int DB_ICUT_Max = 4;
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){
      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0){
        for (int DB_ICUT=1 ; DB_ICUT<=DB_ICUT_Max ; DB_ICUT++){
          if (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT)!=0){

//UGLY: 
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

            int KounterCutLines_Max = pAmdcsimrec->NOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT));
            for (int KounterCutLines=1 ; KounterCutLines<=KounterCutLines_Max ; KounterCutLines++){

              m_UniversalIdKounter = m_UniversalIdKounter + 1;
              AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

              DbVar = "VERS"     ; DbVarComment="VERSION"                                  ; iDbVal = m_version		                                                                     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

              DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE SUB-CUT"       ; dDbVal = pAmdcsimrec->Cutdx (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "DY";
              DbVarComment="Y RELATIVE POSITION OF THE SUB-CUT";
              int theDbIdx = DB_INDX;
              // when amdb is mirroring chambers from the Aside to the Cside, it increments the station numbers, e.g. BIS7 becomes BIS17 etc.
              // however, something inside the INOCUT function (https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/Amdcsimrec/AmdcStand/src/readmdb.F90#0611)
              // cannot translate these larger station numbers to the correct amdb numbers needed by the amdb Cut_dy function 
              // (https://acode-browser1.usatlas.bnl.gov/lxr/source/athena/MuonSpectrometer/Amdcsimrec/AmdcStand/src/readmdb.F90#0971)
              // this Cut_dy function only returns the correct H Line dy values in case the original amdb station number is given.
              // This only happens for BI upgrades, BIS78 and BI1-6
              if (TheStationName.find("BI")!=std::string::npos) {
                if (DB_INDX>16) {
                  MsgStream log(Athena::getMessageSvc(),"AmdcDbSvcMakerFromAmdc::ALIN");
                  log<<MSG::WARNING<<"ATTENTION: Adjusting DB_INDX value for H Line Cutdy method call. This should only be done for BI upgrade chambers."<<endmsg;
                }
                if (DB_INDX==17) theDbIdx=7;
                else if (DB_INDX==18) theDbIdx=13;
                else if (DB_INDX==19) theDbIdx=9;
                else if (DB_INDX==20) theDbIdx=10;
                else if (DB_INDX==21) theDbIdx=8;
                else if (DB_INDX==24) theDbIdx=11;
                else if (DB_INDX==25) theDbIdx=12;
                else if (DB_INDX==26) theDbIdx=14;
              }
              dDbVal = pAmdcsimrec->Cutdy(pAmdcsimrec->INOCUT(DB_JTYP,theDbIdx,DB_ICUT),KounterCutLines)/10.; 
// JFL Thu Apr  3 14:47:44 CEST 2008: 
//  found by Stefania Spagnolo: When computed for the -Z part there are rounding errors which giving non nul value to Dy while it should be null
//  patch: put it a 0. if too small
              double Alim_DySmall = 0.0002 ;
              if ( std::abs(dDbVal) <= Alim_DySmall ) dDbVal = 0.;
              pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

              DbVar = "I"        ; DbVarComment="SERIAL NB. OF THE OBJECT IN WHICH THE SU" ; iDbVal = pAmdcsimrec->IOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)	     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
              DbVar = "WIDTH_XS" ; DbVarComment="S DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutWs (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "WIDTH_XL" ; DbVarComment="L DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutWl (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "LENGTH_Y" ; DbVarComment="Y DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutLe (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "EXCENT"   ; DbVarComment="EXC DIMENSIONS OF THE SUB-CUT"            ; dDbVal = pAmdcsimrec->CutEx (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "DEAD1"    ; DbVarComment="D1 DIMENSIONS OF THE SUB-CUT"             ; dDbVal = pAmdcsimrec->CutAn (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)	     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

              DbVar = "JTYP"     ; DbVarComment="STATION TYPE"                             ; iDbVal = DB_JTYP                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
              DbVar = "INDX"     ; DbVarComment="INDEX"                                    ; iDbVal = DB_INDX                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
              DbVar = "ICUT"     ; DbVarComment="CUT-OUT INDEX"                            ; iDbVal = DB_ICUT                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

              pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

            }

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
int Ifound = 0 ;  
int DB_IZ_Max = pAmdcsimrec->Mzz();
int DB_JFF_Max = 8;
for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
  if (Ifound==1) continue;
  for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    if (Ifound==1) continue;
    if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)!=0){
      // if (Ifound==1) continue;
      if (pAmdcsimrec->ICUT   (DB_JTYP,DB_JFF,DB_IZ) == DB_ICUT
      &&  pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) == DB_INDX) Ifound = 1 ;
    }
  }
}
if (Ifound == 1 ){
	  
            int KounterCutLines_Max = pAmdcsimrec->NOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT));
            for (int KounterCutLines=1 ; KounterCutLines<=KounterCutLines_Max ; KounterCutLines++){

              m_UniversalIdKounter = m_UniversalIdKounter + 1;
              AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

              DbVar = "VERS"     ; DbVarComment="VERSION"                                  ; iDbVal = m_version		                                                                     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

              DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE SUB-CUT"       ; dDbVal = pAmdcsimrec->Cutdx (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE SUB-CUT"       ; dDbVal = pAmdcsimrec->Cutdy (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

              DbVar = "I"        ; DbVarComment="SERIAL NB. OF THE OBJECT IN WHICH THE SU" ; iDbVal = pAmdcsimrec->IOBCUT(pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)	     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
              DbVar = "WIDTH_XS" ; DbVarComment="S DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutWs (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "WIDTH_XL" ; DbVarComment="L DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutWl (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "LENGTH_Y" ; DbVarComment="Y DIMENSIONS OF THE SUB-CUT"              ; dDbVal = pAmdcsimrec->CutLe (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "EXCENT"   ; DbVarComment="EXC DIMENSIONS OF THE SUB-CUT"            ; dDbVal = pAmdcsimrec->CutEx (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
              DbVar = "DEAD1"    ; DbVarComment="D1 DIMENSIONS OF THE SUB-CUT"             ; dDbVal = pAmdcsimrec->CutAn (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)	     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

              DbVar = "JTYP"     ; DbVarComment="STATION TYPE"                             ; iDbVal = DB_JTYP                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
              DbVar = "INDX"     ; DbVarComment="INDEX"                                    ; iDbVal = DB_INDX                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
              DbVar = "ICUT"     ; DbVarComment="CUT-OUT INDEX"                            ; iDbVal = DB_ICUT                                                                                ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

              pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

            }

}
}
//UGLY: 
          }
        }
      }
    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ALMN(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ALMN";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){
      if (pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX)!=0){

//UGLY: 
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

        int DB_JOB_Max = pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX); 
        for (int DB_JOB=1 ; DB_JOB<=DB_JOB_Max ; DB_JOB++){

          m_UniversalIdKounter = m_UniversalIdKounter + 1;
          AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

          DbVar = "VERS"     ; DbVarComment="VERSION"                                  ; iDbVal = m_version                                        ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: this is a guess
          DbVar = "I"        ; DbVarComment="ELEMENT NUMBER"                           ; iDbVal = DB_JOB                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geodx (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geody (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DZ"       ; DbVarComment="Z RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geodz (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

          DbVar = "JOB"      ; DbVarComment="OBJECT SERIAL NUMBER IN THE STATION"      ; iDbVal = DB_JOB                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "TEC"  ; 
          sDbVal =pAmdcsimrec->TechnologyName(pAmdcsimrec->ITEC  (DB_JTYP,DB_INDX,DB_JOB));
          std::string sDbValTest(sDbVal,0,2);
          if (sDbValTest == "LB") sDbVal = sDbValTest;
	  DbVarComment="TYPE OF TECHNOLOGY USED FOR THE OBJECT" ;
          pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal); 

          DbVar = "IW"       ; DbVarComment="INNER STRUCTURE TYPE INDEX, ISTA"         ; iDbVal = pAmdcsimrec->ISTA  (DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "ISPLIT_X" ; DbVarComment="NUMBER OF SECTIONS IN X"                  ; iDbVal = pAmdcsimrec->ISPLIX(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "ISPLIT_Y" ; DbVarComment="NUMBER OF SECTIONS IN Y"                  ; iDbVal = pAmdcsimrec->ISPLIY(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "ISHAPE"   ; DbVarComment="TYPE OF GEOMETRICAL SHAPE 0-TRAPEZOIDAL," ; iDbVal = pAmdcsimrec->ISHAPE(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "WIDTH_XS" ; DbVarComment="S WIDTH, WS"                              ; dDbVal = pAmdcsimrec->GeoWs (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "WIDTH_XL" ; DbVarComment="L WIDTH, WL"                              ; dDbVal = pAmdcsimrec->GeoWl (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "LENGTH_Y" ; DbVarComment="Y LENGTH, LE"                             ; dDbVal = pAmdcsimrec->GeoLe (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "EXCENT"   ; DbVarComment="ADDITIONAL INFORMATION, EX"               ; dDbVal = pAmdcsimrec->GeoEx (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD1"    ; DbVarComment="FIRST DEAD MATERIAL, D1"                  ; dDbVal = pAmdcsimrec->GeoD1 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD2"    ; DbVarComment="SECOND DEAD MATERIAL, D2"                 ; dDbVal = pAmdcsimrec->GeoD2 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD3"    ; DbVarComment="STEPS, D3"                                ; dDbVal = pAmdcsimrec->GeoD3 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

          DbVar = "JTYP"     ; DbVarComment="STATION TYPE"                             ; iDbVal = DB_JTYP                                          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "INDX"     ; DbVarComment="ELEMENT NUMBER"                           ; iDbVal = DB_INDX                                          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

        }

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
int Ifound = 0 ;  
int DB_IZ_Max = pAmdcsimrec->Mzz();
int DB_JFF_Max = 8;
for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
  if (Ifound==1) continue;
  for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    if (Ifound==1) continue;
    if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)!=0){
      // if (Ifound==1) continue;
      if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) == DB_INDX) Ifound = 1 ;
    }
  }
}
if (Ifound == 1 ){

        int DB_JOB_Max = pAmdcsimrec->NOBJ (DB_JTYP,DB_INDX); 
        for (int DB_JOB=1 ; DB_JOB<=DB_JOB_Max ; DB_JOB++){

          m_UniversalIdKounter = m_UniversalIdKounter + 1;
          AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

          DbVar = "VERS"     ; DbVarComment="VERSION"                                  ; iDbVal = m_version                                        ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: this is a guess
          DbVar = "I"        ; DbVarComment="ELEMENT NUMBER"                           ; iDbVal = DB_JOB                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geodx (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geody (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DZ"       ; DbVarComment="Z RELATIVE POSITION OF THE OBJECT"        ; dDbVal = pAmdcsimrec->Geodz (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

          DbVar = "JOB"      ; DbVarComment="OBJECT SERIAL NUMBER IN THE STATION"      ; iDbVal = DB_JOB                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "TEC"  ; 
          sDbVal =pAmdcsimrec->TechnologyName(pAmdcsimrec->ITEC  (DB_JTYP,DB_INDX,DB_JOB));
          std::string sDbValTest(sDbVal,0,2);
          if (sDbValTest == "LB") sDbVal = sDbValTest;
	  DbVarComment="TYPE OF TECHNOLOGY USED FOR THE OBJECT" ;
          pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal); 

          DbVar = "IW"       ; DbVarComment="INNER STRUCTURE TYPE INDEX, ISTA"         ; iDbVal = pAmdcsimrec->ISTA  (DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "ISPLIT_X" ; DbVarComment="NUMBER OF SECTIONS IN X"                  ; iDbVal = pAmdcsimrec->ISPLIX(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "ISPLIT_Y" ; DbVarComment="NUMBER OF SECTIONS IN Y"                  ; iDbVal = pAmdcsimrec->ISPLIY(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "ISHAPE"   ; DbVarComment="TYPE OF GEOMETRICAL SHAPE 0-TRAPEZOIDAL," ; iDbVal = pAmdcsimrec->ISHAPE(DB_JTYP,DB_INDX,DB_JOB)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "WIDTH_XS" ; DbVarComment="S WIDTH, WS"                              ; dDbVal = pAmdcsimrec->GeoWs (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "WIDTH_XL" ; DbVarComment="L WIDTH, WL"                              ; dDbVal = pAmdcsimrec->GeoWl (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "LENGTH_Y" ; DbVarComment="Y LENGTH, LE"                             ; dDbVal = pAmdcsimrec->GeoLe (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "EXCENT"   ; DbVarComment="ADDITIONAL INFORMATION, EX"               ; dDbVal = pAmdcsimrec->GeoEx (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD1"    ; DbVarComment="FIRST DEAD MATERIAL, D1"                  ; dDbVal = pAmdcsimrec->GeoD1 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD2"    ; DbVarComment="SECOND DEAD MATERIAL, D2"                 ; dDbVal = pAmdcsimrec->GeoD2 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "DEAD3"    ; DbVarComment="STEPS, D3"                                ; dDbVal = pAmdcsimrec->GeoD3 (DB_JTYP,DB_INDX,DB_JOB)/ 10. ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

          DbVar = "JTYP"     ; DbVarComment="STATION TYPE"                             ; iDbVal = DB_JTYP                                          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          DbVar = "INDX"     ; DbVarComment="ELEMENT NUMBER"                           ; iDbVal = DB_INDX                                          ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

        }
	
}
}
//UGLY: 
      }
    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::APTP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "APTP";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_JFF_Max = 8;
  int DB_IZ_Max = pAmdcsimrec->Mzz();
  int DB_JTYP_Max = pAmdcsimrec->Mtyp();
  int DB_INDX_Max = pAmdcsimrec->Mgeo();
  for (int DB_JTYP=1 ; DB_JTYP<=DB_JTYP_Max ; DB_JTYP++){
    for (int DB_INDX=1 ; DB_INDX<=DB_INDX_Max ; DB_INDX++){
      int LINEcounter = 0 ;

//UGLY:
//UGLY: do not do this for TGC, CSC and BEE
std::string TheStationName = pAmdcsimrec->StationName(DB_JTYP);
char StationNameHEAD[1];
StationNameHEAD[0]= TheStationName[0];
if ((StationNameHEAD[0] != 'T'  
&&   StationNameHEAD[0] != 'C' 
&&   TheStationName	!= "BEE")
||   m_UglyCodeOn == 0
){

      for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
        for (int DB_IZ=-DB_IZ_Max ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
          if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)==DB_INDX){

            m_UniversalIdKounter = m_UniversalIdKounter + 1;
            AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

            DbVar = "VERS"   ; DbVarComment="VERSION"                       ; iDbVal = m_version 			                     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it? This a guess  
            LINEcounter = LINEcounter + 1;
            DbVar = "LINE"   ; DbVarComment="LINE NUMBER"                   ; iDbVal = LINEcounter                                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            DbVar = "TYP"    ; DbVarComment="STATION TYPE"                  ; sDbVal = pAmdcsimrec->StationName(DB_JTYP)               ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

            DbVar = "I"      ; DbVarComment="STATION AMDB INDEX"            ; iDbVal = pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            DbVar = "ICUT"   ; DbVarComment="CUT-OUT INDEX,ZERO IF MISSING" ; iDbVal = pAmdcsimrec->ICUT   (DB_JTYP,DB_JFF,DB_IZ)      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            for (int DB_JFF_2=1; DB_JFF_2<=8 ; DB_JFF_2++){
	      int DB_JFF_2_Shifted = DB_JFF_2 - 1 ;
              std::ostringstream Aostringstream;
              Aostringstream << DB_JFF_2_Shifted ;
              DbVar = "IPHI_"+Aostringstream.str();
	      iDbVal = 0; 
	      if (DB_JFF_2 == DB_JFF) iDbVal = 2; 
	      DbVarComment="" ;
	      if (DB_JFF_2==1) DbVarComment="PHI INDICATES OF OCTANTS" ;
	      pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal); 
            }
	  
            DbVar = "IZ"     ; DbVarComment="Z (FOR BARREL) OR R (FOR END-CAPS) POS." ; iDbVal = DB_IZ 			                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

            DbVar = "DPHI"  ; 
            double PIS180 = M_PI/180.;
            double PI2SN  = 360./8.;
            dDbVal = pAmdcsimrec->PosPhi (DB_JTYP,DB_JFF,DB_IZ) / PIS180 -  ( DB_JFF * 1. - 1.) * PI2SN ; 
            DbVarComment="RELATIVE PHI POSITION OF THE STATION IN" ;
            pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

            DbVar = "Z"     ; DbVarComment="Z POSITION OF THE LOWEST Z EDGE OF THE S" ; dDbVal = pAmdcsimrec->PosZ   (DB_JTYP,DB_JFF,DB_IZ)/ 10.  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
            DbVar = "R"     ; DbVarComment="RADIAL POSITION OF ITS INNERMOST EDGE"    ; dDbVal = pAmdcsimrec->PosR   (DB_JTYP,DB_JFF,DB_IZ)/ 10.  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
            DbVar = "S"     ; DbVarComment="ORTHO-RADIAL POSITION OF THE CENTER OF T" ; dDbVal = pAmdcsimrec->PosS   (DB_JTYP,DB_JFF,DB_IZ)/ 10.  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
	  
            DbVar = "ALFA"  ; DbVarComment="ALFA ANGLE DEFINING THE DEVIATION [GRAD]" ; dDbVal = pAmdcsimrec->PosAlfa(DB_JTYP,DB_JFF,DB_IZ)       ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
            DbVar = "BETA"  ; DbVarComment="BETA ANGLE DEFINING THE DEVIATION"        ; dDbVal = pAmdcsimrec->PosBeta(DB_JTYP,DB_JFF,DB_IZ)       ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
            DbVar = "GAMMA" ; DbVarComment="GAMMA ANGLE DEFINING THE DEVIATION"       ; dDbVal = pAmdcsimrec->PosGama(DB_JTYP,DB_JFF,DB_IZ)       ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

            pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

          }
        }
      }

//UGLY: 
//UGLY: do this for TGC, CSC and BEE
}else{
    for (int DB_IZ=1 ; DB_IZ<=DB_IZ_Max ; DB_IZ++){
    
      std::string sDbVal_TYP = "";
      int iDbVal_IZ   = 0 ; 
      int iDbVal_I    = 0 ;
      int iDbVal_ICUT = 0 ;

      int iDbVal_IPHI_1 = 0 ;
      int iDbVal_IPHI_2 = 0 ;
      int iDbVal_IPHI_3 = 0 ;
      int iDbVal_IPHI_4 = 0 ;
      int iDbVal_IPHI_5 = 0 ;
      int iDbVal_IPHI_6 = 0 ;
      int iDbVal_IPHI_7 = 0 ;
      int iDbVal_IPHI_8 = 0 ;
      
      double  dDbVal_DPHI = 0 ;
      
      double  dDbVal_Z = 0 ;
      double  dDbVal_R = 0 ;
      double  dDbVal_S = 0 ;

      double  dDbVal_ALFA  = 0 ;
      double  dDbVal_BETA  = 0 ;
      double  dDbVal_GAMMA = 0 ;
      
      int Ifound = 0 ;
      for (int DB_JFF=1 ; DB_JFF<=DB_JFF_Max ; DB_JFF++){
        if (pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ)==DB_INDX){
          Ifound = 1;

          sDbVal_TYP  = pAmdcsimrec->StationName(DB_JTYP) ;
	  
          iDbVal_IZ   = DB_IZ ;
          iDbVal_I    = pAmdcsimrec->IGEO   (DB_JTYP,DB_JFF,DB_IZ) ;
          iDbVal_ICUT = pAmdcsimrec->ICUT   (DB_JTYP,DB_JFF,DB_IZ) ;
          
          if ( DB_JFF == 1 ) iDbVal_IPHI_1 = 1 ;
          if ( DB_JFF == 2 ) iDbVal_IPHI_2 = 1 ;
          if ( DB_JFF == 3 ) iDbVal_IPHI_3 = 1 ;
          if ( DB_JFF == 4 ) iDbVal_IPHI_4 = 1 ;
          if ( DB_JFF == 5 ) iDbVal_IPHI_5 = 1 ;
          if ( DB_JFF == 6 ) iDbVal_IPHI_6 = 1 ;
          if ( DB_JFF == 7 ) iDbVal_IPHI_7 = 1 ;
          if ( DB_JFF == 8 ) iDbVal_IPHI_8 = 1 ;

          double PIS180 = M_PI/180.;
          double PI2SN  = 360./8.;
          dDbVal_DPHI = pAmdcsimrec->PosPhi (DB_JTYP,DB_JFF,DB_IZ) / PIS180 -  ( DB_JFF * 1. - 1.) * PI2SN ;
	  
          dDbVal_Z = pAmdcsimrec->PosZ	(DB_JTYP,DB_JFF,DB_IZ)/ 10. ; 
          dDbVal_R = pAmdcsimrec->PosR	(DB_JTYP,DB_JFF,DB_IZ)/ 10. ; 
          dDbVal_S = pAmdcsimrec->PosS	(DB_JTYP,DB_JFF,DB_IZ)/ 10. ; 

          dDbVal_ALFA  = pAmdcsimrec->PosAlfa(DB_JTYP,DB_JFF,DB_IZ) ;
          dDbVal_BETA  = pAmdcsimrec->PosBeta(DB_JTYP,DB_JFF,DB_IZ) ;
          dDbVal_GAMMA = pAmdcsimrec->PosGama(DB_JTYP,DB_JFF,DB_IZ) ;
        }
      }
      
      if (Ifound==1){
          m_UniversalIdKounter = m_UniversalIdKounter + 1;
          AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

          DbVar = "VERS"   ; DbVarComment="VERSION"                       ; iDbVal = m_version   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it? This a guess     
          LINEcounter = LINEcounter + 1;
          DbVar = "LINE"   ; DbVarComment="LINE NUMBER"                   ; iDbVal = LINEcounter ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "TYP"    ; DbVarComment="STATION TYPE"                  ; sDbVal = sDbVal_TYP  ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

          DbVar = "I"      ; DbVarComment="STATION AMDB INDEX"            ; iDbVal = iDbVal_I    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "ICUT"   ; DbVarComment="CUT-OUT INDEX,ZERO IF MISSING" ; iDbVal = iDbVal_ICUT ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          for (int DB_JFF_2=1; DB_JFF_2<=8 ; DB_JFF_2++){
	    int DB_JFF_2_Shifted = DB_JFF_2 - 1 ;
            std::ostringstream Aostringstream;
            Aostringstream << DB_JFF_2_Shifted ;
            DbVar = "IPHI_"+Aostringstream.str();
	    iDbVal = 0; 
	    if ( DB_JFF_2 == 1 ) iDbVal = iDbVal_IPHI_1 ;
	    if ( DB_JFF_2 == 2 ) iDbVal = iDbVal_IPHI_2 ;
	    if ( DB_JFF_2 == 3 ) iDbVal = iDbVal_IPHI_3 ;
	    if ( DB_JFF_2 == 4 ) iDbVal = iDbVal_IPHI_4 ;
	    if ( DB_JFF_2 == 5 ) iDbVal = iDbVal_IPHI_5 ;
	    if ( DB_JFF_2 == 6 ) iDbVal = iDbVal_IPHI_6 ;
	    if ( DB_JFF_2 == 7 ) iDbVal = iDbVal_IPHI_7 ;
	    if ( DB_JFF_2 == 8 ) iDbVal = iDbVal_IPHI_8 ;
	    DbVarComment="" ;
	    if (DB_JFF_2==1) DbVarComment="PHI INDICATES OF OCTANTS" ;
	    pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
          }
	  
          DbVar = "IZ"    ; DbVarComment="Z (FOR BARREL) OR R (FOR END-CAPS) POS."  ; iDbVal = iDbVal_IZ    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

          DbVar = "DPHI"  ; DbVarComment="RELATIVE PHI POSITION OF THE STATION IN"  ; dDbVal = dDbVal_DPHI  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

          DbVar = "Z"     ; DbVarComment="Z POSITION OF THE LOWEST Z EDGE OF THE S" ; dDbVal = dDbVal_Z     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "R"     ; DbVarComment="RADIAL POSITION OF ITS INNERMOST EDGE"    ; dDbVal = dDbVal_R     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
          DbVar = "S"     ; DbVarComment="ORTHO-RADIAL POSITION OF THE CENTER OF T" ; dDbVal = dDbVal_S     ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
	  
          DbVar = "ALFA"  ; DbVarComment="ALFA ANGLE DEFINING THE DEVIATION [GRAD]" ; dDbVal = dDbVal_ALFA  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
          DbVar = "BETA"  ; DbVarComment="BETA ANGLE DEFINING THE DEVIATION"        ; dDbVal = dDbVal_BETA  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
          DbVar = "GAMMA" ; DbVarComment="GAMMA ANGLE DEFINING THE DEVIATION"       ; dDbVal = dDbVal_GAMMA ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

          pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

	}
      }
    }

//UGLY:

    }
  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ASZT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ASZT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jadjust_Max = pAmdcsimrec->NBadjust();
  for (int DB_Jadjust=1 ; DB_Jadjust<=DB_Jadjust_Max ; DB_Jadjust++){

    m_UniversalIdKounter = m_UniversalIdKounter + 1;
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

    int    DB_JTYP ;
    int    DB_JFF  ;
    int    DB_JZZ  ;
    int    DB_JOB  ;
    double Amdc_TraS ;
    double Amdc_TraZ ;
    double Amdc_TraT ;
    double Amdc_RotS ;
    double Amdc_RotZ ;
    double Amdc_RotT ;
    pAmdcsimrec->GetStationDisplacementFromAMDCJadjust(
    	     DB_Jadjust,
    	     DB_JTYP,DB_JFF,DB_JZZ,DB_JOB,
             Amdc_TraS, Amdc_TraZ, Amdc_TraT,
             Amdc_RotS, Amdc_RotZ, Amdc_RotT );
 
    DbVar = "VERS"   ; DbVarComment="VERSION"            ; iDbVal = m_version                         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?     
    DbVar = "LINE"   ; DbVarComment="LINE NUMBER"        ; iDbVal = 0                                 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"    ; DbVarComment="STATION TYPE"       ; sDbVal = pAmdcsimrec->StationName(DB_JTYP) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
    DbVar = "JFF"    ; DbVarComment="PHI POSITION"       ; iDbVal = DB_JFF                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JZZ"    ; DbVarComment="Z POSITION"         ; iDbVal = DB_JZZ                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JOB"    ; DbVarComment="JOB POSITION"       ; iDbVal = DB_JOB                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TRAS"   ; DbVarComment="S TRANSLATION [MM]" ; dDbVal = Amdc_TraS/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAZ"   ; DbVarComment="Z TRANSLATION"      ; dDbVal = Amdc_TraZ/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAT"   ; DbVarComment="T TRANSLATION"      ; dDbVal = Amdc_TraT/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    
    DbVar = "ROTS"   ; DbVarComment="S ROTATION [RAD]"   ; dDbVal = Amdc_RotS                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTZ"   ; DbVarComment="Z ROTATION"         ; dDbVal = Amdc_RotZ                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTT"   ; DbVarComment="T ROTATION"         ; dDbVal = Amdc_RotT                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

//DRING: What is it? From SB error     
    DbVar = "I"      ; DbVarComment="STATION AMDB INDEX" ; iDbVal = 0                                 ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::ISZT(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ISZT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int DB_Jdwnlay_Max = pAmdcsimrec->NBdwnlay();
  for (int DB_Jdwnlay=1 ; DB_Jdwnlay<=DB_Jdwnlay_Max ; DB_Jdwnlay++){

    m_UniversalIdKounter = m_UniversalIdKounter + 1;
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

    int    DB_JTYP ;
    int    DB_JFF  ;
    int    DB_JZZ  ;
    int    DB_JOB  ;
    int    DB_JLAY ;
    double Amdc_TraS ;
    double Amdc_TraZ ;
    double Amdc_TraT ;
    double Amdc_RotS ;
    double Amdc_RotZ ;
    double Amdc_RotT ;
    pAmdcsimrec->GetStationInternalAlignmentFromAMDCJdwnlay(
             DB_Jdwnlay,
    	     DB_JTYP,DB_JFF,DB_JZZ,DB_JOB,DB_JLAY,
             Amdc_TraS, Amdc_TraZ, Amdc_TraT,
             Amdc_RotS, Amdc_RotZ, Amdc_RotT );
 
    DbVar = "VERS"   ; DbVarComment="VERSION"            ; iDbVal = m_version                         ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"    ; DbVarComment="STATION TYPE"       ; sDbVal = pAmdcsimrec->StationName(DB_JTYP) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
    DbVar = "JFF"    ; DbVarComment="PHI POSITION"       ; iDbVal = DB_JFF                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JZZ"    ; DbVarComment="Z POSITION"         ; iDbVal = DB_JZZ                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JOB"    ; DbVarComment="JOB POSITION"       ; iDbVal = DB_JOB                            ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JLAY"   ; DbVarComment="JLAY POSITION"      ; iDbVal = DB_JLAY                           ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TRAS"   ; DbVarComment="S TRANSLATION [MM]" ; dDbVal = Amdc_TraS/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAZ"   ; DbVarComment="Z TRANSLATION"      ; dDbVal = Amdc_TraZ/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAT"   ; DbVarComment="T TRANSLATION"      ; dDbVal = Amdc_TraT/ 10.                    ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    
    DbVar = "ROTS"   ; DbVarComment="S ROTATION [RAD]"   ; dDbVal = Amdc_RotS                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTZ"   ; DbVarComment="Z ROTATION"         ; dDbVal = Amdc_RotZ                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTT"   ; DbVarComment="T ROTATION"         ; dDbVal = Amdc_RotT                         ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::HwSwIdMapping(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "HwSwIdMapping";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int iHardEntries_Max = pAmdcsimrec->GetHARDENTRIES();
  for (int iHardEntries=1 ; iHardEntries<=iHardEntries_Max ; iHardEntries++){

    m_UniversalIdKounter = m_UniversalIdKounter + 1;
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "HARDNAME"   ; DbVarComment="Hard name"   ; sDbVal = pAmdcsimrec->GetHARDNAME(iHardEntries)   ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "SOFTNAME"   ; DbVarComment="Soft name"   ; sDbVal = pAmdcsimrec->GetSOFTNAME(iHardEntries)   ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "SOFTOCTANT" ; DbVarComment="Soft octant" ; iDbVal = pAmdcsimrec->GetSOFTOCTANT(iHardEntries) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "SOFTIZ"     ; DbVarComment="Soft Iz"     ; iDbVal = pAmdcsimrec->GetSOFTIZ(iHardEntries)     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }
 
  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::XtomoData(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "XtomoData";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int iXtomoEntries_Max = pAmdcsimrec->GetXTOMOENTRIES();
  for (int iXtomoEntries1=1 ; iXtomoEntries1<=iXtomoEntries_Max ; iXtomoEntries1++){

    m_UniversalIdKounter = m_UniversalIdKounter + 1;
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

    DbVar = "XtomoChberName"  ; DbVarComment="XtomoChberName"   ; sDbVal = pAmdcsimrec->GetXtomoChberName (iXtomoEntries1) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoSite"       ; DbVarComment="XtomoSite     "   ; sDbVal = pAmdcsimrec->GetXtomoSite      (iXtomoEntries1) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoSiteId"     ; DbVarComment="XtomoSiteId   "   ; iDbVal = pAmdcsimrec->GetXtomoSiteId    (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoTime"       ; DbVarComment="XtomoTime     "   ; iDbVal = pAmdcsimrec->GetXtomoTime      (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ; 
    DbVar = "XtomoPassed"     ; DbVarComment="XtomoPassed   "   ; iDbVal = pAmdcsimrec->GetXtomoPassed    (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoSide"       ; DbVarComment="XtomoSide     "   ; sDbVal = pAmdcsimrec->GetXtomoSide      (iXtomoEntries1) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoNberTube1"  ; DbVarComment="XtomoNberTube1"   ; iDbVal = pAmdcsimrec->GetXtomoNberTube1 (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberTube2"  ; DbVarComment="XtomoNberTube2"   ; iDbVal = pAmdcsimrec->GetXtomoNberTube2 (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberML"     ; DbVarComment="XtomoNberML   "   ; iDbVal = pAmdcsimrec->GetXtomoNberML    (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberLayer"  ; DbVarComment="XtomoNberLayer"   ; iDbVal = pAmdcsimrec->GetXtomoNberLayer (iXtomoEntries1) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoML1NYtub"   ; DbVarComment="XtomoML1NYtub "   ; dDbVal = pAmdcsimrec->GetXtomoML1NYtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NZtub"   ; DbVarComment="XtomoML1NZtub "   ; dDbVal = pAmdcsimrec->GetXtomoML1NZtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDely"   ; DbVarComment="XtomoML1NDely "   ; dDbVal = pAmdcsimrec->GetXtomoML1NDely  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDelz"   ; DbVarComment="XtomoML1NDelz "   ; dDbVal = pAmdcsimrec->GetXtomoML1NDelz  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDela"   ; DbVarComment="XtomoML1NDela "   ; dDbVal = pAmdcsimrec->GetXtomoML1NDela  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NYpit"   ; DbVarComment="XtomoML1NYpit "   ; dDbVal = pAmdcsimrec->GetXtomoML1NYpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NZpit"   ; DbVarComment="XtomoML1NZpit "   ; dDbVal = pAmdcsimrec->GetXtomoML1NZpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PYtub"   ; DbVarComment="XtomoML1PYtub "   ; dDbVal = pAmdcsimrec->GetXtomoML1PYtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PZtub"   ; DbVarComment="XtomoML1PZtub "   ; dDbVal = pAmdcsimrec->GetXtomoML1PZtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDely"   ; DbVarComment="XtomoML1PDely "   ; dDbVal = pAmdcsimrec->GetXtomoML1PDely  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDelz"   ; DbVarComment="XtomoML1PDelz "   ; dDbVal = pAmdcsimrec->GetXtomoML1PDelz  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDela"   ; DbVarComment="XtomoML1PDela "   ; dDbVal = pAmdcsimrec->GetXtomoML1PDela  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PYpit"   ; DbVarComment="XtomoML1PYpit "   ; dDbVal = pAmdcsimrec->GetXtomoML1PYpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PZpit"   ; DbVarComment="XtomoML1PZpit "   ; dDbVal = pAmdcsimrec->GetXtomoML1PZpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NYtub"   ; DbVarComment="XtomoML2NYtub "   ; dDbVal = pAmdcsimrec->GetXtomoML2NYtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NZtub"   ; DbVarComment="XtomoML2NZtub "   ; dDbVal = pAmdcsimrec->GetXtomoML2NZtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDely"   ; DbVarComment="XtomoML2NDely "   ; dDbVal = pAmdcsimrec->GetXtomoML2NDely  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDelz"   ; DbVarComment="XtomoML2NDelz "   ; dDbVal = pAmdcsimrec->GetXtomoML2NDelz  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDela"   ; DbVarComment="XtomoML2NDela "   ; dDbVal = pAmdcsimrec->GetXtomoML2NDela  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NYpit"   ; DbVarComment="XtomoML2NYpit "   ; dDbVal = pAmdcsimrec->GetXtomoML2NYpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NZpit"   ; DbVarComment="XtomoML2NZpit "   ; dDbVal = pAmdcsimrec->GetXtomoML2NZpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PYtub"   ; DbVarComment="XtomoML2PYtub "   ; dDbVal = pAmdcsimrec->GetXtomoML2PYtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PZtub"   ; DbVarComment="XtomoML2PZtub "   ; dDbVal = pAmdcsimrec->GetXtomoML2PZtub  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDely"   ; DbVarComment="XtomoML2PDely "   ; dDbVal = pAmdcsimrec->GetXtomoML2PDely  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDelz"   ; DbVarComment="XtomoML2PDelz "   ; dDbVal = pAmdcsimrec->GetXtomoML2PDelz  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDela"   ; DbVarComment="XtomoML2PDela "   ; dDbVal = pAmdcsimrec->GetXtomoML2PDela  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PYpit"   ; DbVarComment="XtomoML2PYpit "   ; dDbVal = pAmdcsimrec->GetXtomoML2PYpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PZpit"   ; DbVarComment="XtomoML2PZpit "   ; dDbVal = pAmdcsimrec->GetXtomoML2PZpit  (iXtomoEntries1) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);
  }
 
  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
