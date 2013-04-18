/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbSvcMakerFromAmdc.h"
#include "AmdcCore/Amdcsimrec.h"
#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbRecord.h"
 
#include "MboySvcInterfaces/IMboySvc.h"

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


AmdcDbSvcMakerFromAmdc::~AmdcDbSvcMakerFromAmdc(){}


// Set a AmdcDbSvc
void AmdcDbSvcMakerFromAmdc::SetUglyCodeOn(int UglyCodeOn){ m_UglyCodeOn  = UglyCodeOn ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthMM(int EpsLengthMM){ m_EpsLengthMM  = EpsLengthMM ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthCM(int EpsLengthCM){ m_EpsLengthCM  = EpsLengthCM ;}
void AmdcDbSvcMakerFromAmdc::SetEpsAngle(int EpsAngle){ m_EpsAngle  = EpsAngle ;}
void AmdcDbSvcMakerFromAmdc::SetEpsLengthMM(std::string NameOfTheSet, int EpsLengthMM) { m_Map_Set_EpsLengthMM[NameOfTheSet] = EpsLengthMM ; }
void AmdcDbSvcMakerFromAmdc::SetEpsLengthCM(std::string NameOfTheSet, int EpsLengthCM) { m_Map_Set_EpsLengthCM[NameOfTheSet] = EpsLengthCM ; }
void AmdcDbSvcMakerFromAmdc::SetEpsAngle   (std::string NameOfTheSet, int EpsAngle   ) { m_Map_Set_EpsAngle   [NameOfTheSet] = EpsAngle    ; }
void AmdcDbSvcMakerFromAmdc::Set(Amdcsimrec* pAmdcsimrec,IMboySvc* /* pIMboySvc */,AmdcDbSvc* pAmdcDbSvc){

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

//  ABRT(pIMboySvc,pAmdcDbSvc);
//  AECT(pIMboySvc,pAmdcDbSvc);
//  ECST(pIMboySvc,pAmdcDbSvc);
//  FEET(pIMboySvc,pAmdcDbSvc);
//  GENV(pIMboySvc,pAmdcDbSvc);
//  JDSH(pIMboySvc,pAmdcDbSvc);
//  JFSH(pIMboySvc,pAmdcDbSvc);
//  JTSH(pIMboySvc,pAmdcDbSvc);
//  MBAP(pIMboySvc,pAmdcDbSvc);

//   AgddXMLVariables(pIMboySvc,pAmdcDbSvc);

  HwSwIdMapping(pAmdcsimrec,pAmdcDbSvc);

  XtomoData(pAmdcsimrec,pAmdcDbSvc);
  
}

int AmdcDbSvcMakerFromAmdc::GetEpsLengthMM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthMM.find(NameOfTheSet) != m_Map_Set_EpsLengthMM.end()) return m_Map_Set_EpsLengthMM.find(NameOfTheSet)->second ; return m_EpsLengthMM; }
int AmdcDbSvcMakerFromAmdc::GetEpsLengthCM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthCM.find(NameOfTheSet) != m_Map_Set_EpsLengthCM.end()) return m_Map_Set_EpsLengthCM.find(NameOfTheSet)->second ; return m_EpsLengthCM; }
int AmdcDbSvcMakerFromAmdc::GetEpsAngle   (std::string NameOfTheSet){ if (m_Map_Set_EpsAngle.find(NameOfTheSet)    != m_Map_Set_EpsAngle.end()   ) return m_Map_Set_EpsAngle.find(NameOfTheSet)->second    ; return m_EpsAngle   ; }

void AmdcDbSvcMakerFromAmdc::AMDC(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AMDC";
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

  DbVar = "VERS"    ; DbVarComment="VERSION"  	     ; iDbVal = m_version                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  std::string TheAmdcName = pAmdcsimrec->AmdcName() ;

  DbVar = "VNAME"   ; DbVarComment="NAME"	     ; sDbVal  = TheAmdcName.substr(0,4)       ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

  std::string TheBlob = pAmdcsimrec->GetAgddString() ;
  
  DbVar = "LENAGDD" ; DbVarComment="STRING LENGTH"   ; iDbVal = TheBlob.size()	               ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
  DbVar = "NLINE"   ; DbVarComment="CHAR4 NUMBER"    ; iDbVal = int ( (TheBlob.size()+2.)/4. ) ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

  DbVar = "DATA"    ; DbVarComment="(NLINE)-ASCII"   ; sDbVal = TheBlob                        ; pAmdcDbRecord->addBlob(DbVar,DbVarComment,sDbVal);
 
  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::ATYP(Amdcsimrec* pAmdcsimrec,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ATYP";
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

  int DB_MTYP = iDbVal = iDbVal ;
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
              DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE SUB-CUT"       ; dDbVal = pAmdcsimrec->Cutdy (pAmdcsimrec->INOCUT(DB_JTYP,DB_INDX,DB_ICUT),KounterCutLines)/ 10. ; 
// JFL Thu Apr  3 14:47:44 CEST 2008: 
//  found by Stefania Spagnolo: When computed for the -Z part there are rounding errors which giving non nul value to Dy while it should be null
//  patch: put it a 0. if too small
              double Alim_DySmall = 0.0002 ;
              if ( fabs(dDbVal) <= Alim_DySmall ) dDbVal = 0.;
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
      if (Ifound==1) continue;
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
      if (Ifound==1) continue;
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
            double PIS180 = 3.1415926535898/ 180. ;
	    double PI2SN  = 360. / 8. ;
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

          double PIS180 = 3.1415926535898/ 180. ;
	  double PI2SN  = 360. / 8. ;
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

void AmdcDbSvcMakerFromAmdc::ABRT(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ABRT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "CRYORMIN"  ; DbVarComment="CRYORMIN                    " ; NameVar = "ABRT_CryoRmin                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORMAX"  ; DbVarComment="CRYORMAX                    " ; NameVar = "ABRT_CryoRmax                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOZMAX"  ; DbVarComment="CRYOZMAX                    " ; NameVar = "ABRT_CryoZmax                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORCUR"  ; DbVarComment="CRYORCUR                    " ; NameVar = "ABRT_CryoRcur                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORADI"  ; DbVarComment="RADIUS                      " ; NameVar = "ABRT_CryoRadi_Radius                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORADT"  ; DbVarComment="RADTHICK                    " ; NameVar = "ABRT_CryoRadT_RadThick                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTD0"  ; DbVarComment="CRYOATTDIAM0                " ; NameVar = "ABRT_CryAttD0_CryoAttDiam0                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTD1"  ; DbVarComment="CRYOATTDIAM1                " ; NameVar = "ABRT_CryAttD1_CryoAttDiam1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTAL"  ; DbVarComment="CRYOATTALFA                 " ; NameVar = "ABRT_CryAttAl_CryoAttAlfa                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTXH"  ; DbVarComment="CRYOATTXHIGH                " ; NameVar = "ABRT_CryAttXh_CryoAttXhigh                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTTH"  ; DbVarComment="CRYOATTTHICK                " ; NameVar = "ABRT_CryAttTh_CryoAttThick                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTRX"  ; DbVarComment="CRYOATTRMAX                 " ; NameVar = "ABRT_CryAttRx_CryoAttRmax                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWIY"  ; DbVarComment="CRYOATTWINGY                " ; NameVar = "ABRT_CryAtWiY_CryoAttWingY                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWXP"  ; DbVarComment="CRYOATTWINGXP               " ; NameVar = "ABRT_CryAtWXp_CryoAttWingXp               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWXN"  ; DbVarComment="CRYOATTWINGXN               " ; NameVar = "ABRT_CryAtWXn_CryoAttWingXn               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWBO"  ; DbVarComment="CRYOATTWINGBOSS             " ; NameVar = "ABRT_CryAtWBo_CryoAttWingBoss             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWTH"  ; DbVarComment="CRYOATTWINGTHIC             " ; NameVar = "ABRT_CryAtWTh_CryoAttWingThick            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWZE"  ; DbVarComment="CRYOATTWINGZESP             " ; NameVar = "ABRT_CryAtWZe_CryoAttWingZesp             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWRA"  ; DbVarComment="CRYOATTWINGRAD              " ; NameVar = "ABRT_CryAtWRa_CryoAttWingRad              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATWYC"  ; DbVarComment="CRYOATTWINGYCUT             " ; NameVar = "ABRT_CryAtWYc_CryoAttWingYcut             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIBYW"  ; DbVarComment="CRYORIBYWID                 " ; NameVar = "ABRT_CryRibYw_CryoRibYwid                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIBZL"  ; DbVarComment="CRYORIBZLEN                 " ; NameVar = "ABRT_CryRibZl_CryoRibZlen                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIBTH"  ; DbVarComment="CRYORIBTHICK                " ; NameVar = "ABRT_CryRibTh_CryoRibThick                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIWYP"  ; DbVarComment="CRYORIBWINGYP               " ; NameVar = "ABRT_CryRiWYp_CryoRibWingYp               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIWYN"  ; DbVarComment="CRYORIBWINGYN               " ; NameVar = "ABRT_CryRiWYn_CryoRibWingYn               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIWXP"  ; DbVarComment="CRYORIBWINGXP               " ; NameVar = "ABRT_CryRiWXp_CryoRibWingXp               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIWXN"  ; DbVarComment="CRYORIBWINGXN               " ; NameVar = "ABRT_CryRiWXn_CryoRibWingXn               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIWTH"  ; DbVarComment="CRYORIBWINGTHIC             " ; NameVar = "ABRT_CryRiWTh_CryoRibWingThick            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRNGRM"  ; DbVarComment="CRYORINGRMED                " ; NameVar = "ABRT_CryRngRm_CryoRingRmed                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRNGZM"  ; DbVarComment="CRYORINGZMED                " ; NameVar = "ABRT_CryRngZm_CryoRingZmed                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRNGRA"  ; DbVarComment="CRYORINGRADIUS              " ; NameVar = "ABRT_CryRngRa_CryoRingRadius              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTRMAX"  ; DbVarComment="STRTRMAX                    " ; NameVar = "ABRT_StrtRmax                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTRMIN"  ; DbVarComment="STRTRMIN                    " ; NameVar = "ABRT_StrtRmin                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTYLEN"  ; DbVarComment="STRTYLEN                    " ; NameVar = "ABRT_StrtYlen                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTZWID"  ; DbVarComment="STRUTZWIDTH                 " ; NameVar = "ABRT_StrtZwid_StrutZwidth                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTRTHI"  ; DbVarComment="STRUTRTHICK                 " ; NameVar = "ABRT_StrtRthi_StrutRthick                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTZTHI"  ; DbVarComment="STRUTZTHICK                 " ; NameVar = "ABRT_StrtZthi_StrutZthick                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWRMAX"  ; DbVarComment="STRUTWINGRMAX               " ; NameVar = "ABRT_StrWRmax_StrutWingRmax               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWYMAX"  ; DbVarComment="STRUTWINGYMAX               " ; NameVar = "ABRT_StrWYmax_StrutWingYmax               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWRMIN"  ; DbVarComment="STRUTWINGRMIN               " ; NameVar = "ABRT_StrWRmin_StrutWingRmin               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWYMIN"  ; DbVarComment="STRUTWINGYMIN               " ; NameVar = "ABRT_StrWYmin_StrutWingYmin               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWZTHI"  ; DbVarComment="STRUTWINGZTHICK             " ; NameVar = "ABRT_StrWZthi_StrutWingZthick             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWYTHI"  ; DbVarComment="STRUTWINGYTHICK             " ; NameVar = "ABRT_StrWYthi_StrutWingYthick             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRWZLEN"  ; DbVarComment="STRUTWINGZLEN               " ; NameVar = "ABRT_StrWZlen_StrutWingZlen               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_0"  ; DbVarComment="ARRAY                       " ; NameVar = "ABRT_Zvouss%0                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_1"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%1                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_2"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%2                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_3"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%3                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_4"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%4                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_5"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%5                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_6"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%6                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZVOUSS_7"  ; DbVarComment="                            " ; NameVar = "ABRT_Zvouss%7                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_0"    ; DbVarComment="ARRAY                       " ; NameVar = "ABRT_Zrib%0                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_1"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%1                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_2"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%2                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_3"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%3                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_4"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%4                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_5"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%5                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZRIB_6"    ; DbVarComment="                            " ; NameVar = "ABRT_Zrib%6                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYRIXHI"  ; DbVarComment="CRYORIBXHIGH                " ; NameVar = "ABRT_CryRiXhi_CryoRibXhigh                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTRMOY"  ; DbVarComment="STRTRMOY                    " ; NameVar = "ABRT_StrtRmoy                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STRTYLNP"  ; DbVarComment="STRUTYLENP                  " ; NameVar = "ABRT_StrtYlnP_StrutYlenP                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYATTXS"  ; DbVarComment="CRYOATTXSEC                 " ; NameVar = "ABRT_CryAttXs_CryoAttXsec                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARMIN"  ; DbVarComment="COLDMASSRMIN                " ; NameVar = "ABRT_CoMaRmin_ColdMassRmin                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARMAX"  ; DbVarComment="COLDMASSRMAX                " ; NameVar = "ABRT_CoMaRmax_ColdMassRmax                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMAZMAX"  ; DbVarComment="COLDMASSZMAX                " ; NameVar = "ABRT_CoMaZmax_ColdMassZmax                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARCUI"  ; DbVarComment="COLDMASSRCURVI              " ; NameVar = "ABRT_CoMaRcuI_ColdMassRcurvI              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARTHI"  ; DbVarComment="COLDMASSRTHICK              " ; NameVar = "ABRT_CoMaRThi_ColdMassRThick              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMAYTHI"  ; DbVarComment="COLDMASSYTHICK              " ; NameVar = "ABRT_CoMaYThi_ColdMassYThick              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARIBY"  ; DbVarComment="COLDMASSRIBY                " ; NameVar = "ABRT_CoMaRibY_ColdMassRibY                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARIBZ"  ; DbVarComment="COLDMASSRIBZ                " ; NameVar = "ABRT_CoMaRibZ_ColdMassRibZ                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMABOSS"  ; DbVarComment="COLDMASSBOSS                " ; NameVar = "ABRT_CoMaBoss_ColdMassBoss                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COMARIBX"  ; DbVarComment="COLDMASSRIBX                " ; NameVar = "ABRT_CoMaRibX_ColdMassRibX                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELYHEI"  ; DbVarComment="BIELLYHEIGHT                " ; NameVar = "ABRT_BielYhei_BiellYheight                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELYSO1"  ; DbVarComment="BIELLYSOLE1                 " ; NameVar = "ABRT_BielYso1_BiellYsole1                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELYSO2"  ; DbVarComment="BIELLYSOLE2                 " ; NameVar = "ABRT_BielYso2_BiellYsole2                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELXSOL"  ; DbVarComment="BIELLXSOLE                  " ; NameVar = "ABRT_BielXsol_BiellXsole                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELXTH1"  ; DbVarComment="BIELLXTHICK1                " ; NameVar = "ABRT_BielXth1_BiellXthick1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELXTH2"  ; DbVarComment="BIELLXTHICK2                " ; NameVar = "ABRT_BielXth2_BiellXthick2                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELZTH1"  ; DbVarComment="BIELLZTHICK1                " ; NameVar = "ABRT_BielZth1_BiellZthick1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELRHOL"  ; DbVarComment="BIELLRADHOLE                " ; NameVar = "ABRT_BielRHol_BiellRadHole                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELYCHO"  ; DbVarComment="BIELLYCENHOLE               " ; NameVar = "ABRT_BielYcHo_BiellYcenHole               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELATL1"  ; DbVarComment="BIELLATTLENG1               " ; NameVar = "ABRT_BielAtL1_BiellAttLeng1               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELATL2"  ; DbVarComment="BIELLATTLENG2               " ; NameVar = "ABRT_BielAtL2_BiellAttLeng2               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELATTH"  ; DbVarComment="BIELLATTTHICK               " ; NameVar = "ABRT_BielAtTh_BiellAttThick               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BIELATHE"  ; DbVarComment="BIELLATTHEIGHT              " ; NameVar = "ABRT_BielAtHe_BiellAttHeight              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSBLXH"  ; DbVarComment="VOUSSBLOCKXHEIGHT           " ; NameVar = "ABRT_VousBlXh_VoussBlockXheight           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSPLIX"  ; DbVarComment="VOUSSPLATEINNERXWID         " ; NameVar = "ABRT_VousPliX_VoussPlateInnerXwid         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSPLOX"  ; DbVarComment="VOUSSPLATEOUTERXWID         " ; NameVar = "ABRT_VousPloX_VoussPlateOuterXwid         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSBLYW"  ; DbVarComment="VOUSSBLOCKYWID              " ; NameVar = "ABRT_VousBlYw_VoussBlockYwid              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUBLYWS"  ; DbVarComment="VOUSSBLOCKYWIDSUB           " ; NameVar = "ABRT_VouBlYwS_VoussBlockYwidSub           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOURCYWI"  ; DbVarComment="VOUSSRECTCUTYWID            " ; NameVar = "ABRT_VouRCYwi_VoussRectCutYwid            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOURPYWI"  ; DbVarComment="VOUSSREINFORCEPLATYWID      " ; NameVar = "ABRT_VouRPYwi_VoussReinforcePlatYwid      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUBLZLE"  ; DbVarComment="VOUSSBLOCKZLEN              " ; NameVar = "ABRT_VouBlZle_VoussBlockZlen              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUBLZLS"  ; DbVarComment="VOUSSBLOCKZLENSUB           " ; NameVar = "ABRT_VouBlZlS_VoussBlockZlenSub           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUBZWTH"  ; DbVarComment="VOUSSBLOCKZWALLTH           " ; NameVar = "ABRT_VouBZWTh_VoussBlockZWallTh           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUCUTZE"  ; DbVarComment="VOUSSCUTZEXT                " ; NameVar = "ABRT_VouCutZe_VoussCutZExt                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOURCUTR"  ; DbVarComment="VOUSSROUNDCUTRAD            " ; NameVar = "ABRT_VouRCutR_VoussRoundCutRad            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOURECSL"  ; DbVarComment="VOUSSRECTCUTXYSLANT         " ; NameVar = "ABRT_VouReCsl_VoussRectCutXYslant         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCOYEX"  ; DbVarComment="CONBOXCOREYEXTENSION        " ; NameVar = "ABRT_CnbCoYex_ConboxCoreYextension        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCOXEX"  ; DbVarComment="CONBOXCOREXEXTENSION        " ; NameVar = "ABRT_CnbCoXex_ConboxCoreXextension        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCOXSU"  ; DbVarComment="CONBOXCOREXSUP              " ; NameVar = "ABRT_CnbCoXsu_ConboxCoreXsup              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCOXIN"  ; DbVarComment="CONBOXCOREXINF              " ; NameVar = "ABRT_CnbCoXin_ConboxCoreXinf              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEAHEI"  ; DbVarComment="CONBOXEARHEIGHT             " ; NameVar = "ABRT_CnbEaHei_ConboxEarHeight             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACXI"  ; DbVarComment="CONBOXEARCUTXIN             " ; NameVar = "ABRT_CnbEaCxi_ConboxEarCutXin             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACTL"  ; DbVarComment="CONBOXEARCUTPLATETHICKLOWOUT" ; NameVar = "ABRT_CnbEaCtl_ConboxEarCutPlateThickLowOut" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACTU"  ; DbVarComment="CONBOXEARCUTPLATETHICKUP    " ; NameVar = "ABRT_CnbEaCtu_ConboxEarCutPlateThickUp    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACZW"  ; DbVarComment="CONBOXEARCUTZWALL           " ; NameVar = "ABRT_CnbEaCzw_ConboxEarCutZwall           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACZE"  ; DbVarComment="CONBOXEARCUTZEXTR           " ; NameVar = "ABRT_CnbEaCze_ConboxEarCutZextr           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEACPL"  ; DbVarComment="CONBOXINNEREARCUTPLATE      " ; NameVar = "ABRT_CnbEaCPl_ConboxInnerEarCutPlate      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCADMA"  ; DbVarComment="CONBOXCAVITYDMAX            " ; NameVar = "ABRT_CnbCaDma_ConboxCavityDmax            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCADMI"  ; DbVarComment="CONBOXCAVITYDMIN            " ; NameVar = "ABRT_CnbCaDmi_ConboxCavityDmin            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCAZEX"  ; DbVarComment="CONBOXCAVITYZEXT            " ; NameVar = "ABRT_CnbCaZex_ConboxCavityZext            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBOXZEX"  ; DbVarComment="CONBOXZEXTENSION            " ; NameVar = "ABRT_CnboxZex_ConboxZextension            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSMBRA"  ; DbVarComment="VOUSSOIRMAINBLOCKRADIUS     " ; NameVar = "ABRT_VousMBRa_VoussoirMainBlockRadius     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXMBRA"  ; DbVarComment="CONBOXMAINBLOCKRADIUS       " ; NameVar = "ABRT_CnbxMBRa_ConboxMainBlockRadius       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUCRCYR"  ; DbVarComment="VOUSSCENTRRECTCUTYREACH     " ; NameVar = "ABRT_VouCRCYr_VoussCentrRectCutYreach     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSRCXW"  ; DbVarComment="VOUSSRECTCUTXWID            " ; NameVar = "ABRT_VousRCXw_VoussRectCutXwid            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSCXOF"  ; DbVarComment="VOUSSCUTXOFFSET             " ; NameVar = "ABRT_VousCXof_VoussCutXoffset             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSCYPO"  ; DbVarComment="VOUSSOIRCUTYPOS             " ; NameVar = "ABRT_VousCYpo_VoussoirCutYpos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSCZPO"  ; DbVarComment="VOUSSOIRCUTZPOS             " ; NameVar = "ABRT_VousCZpo_VoussoirCutZpos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEAXTP"  ; DbVarComment="CONBOXEARXTOP               " ; NameVar = "ABRT_CnbEaXtp_ConboxEarXtop               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEAYTP"  ; DbVarComment="CONBOXEARYTOP               " ; NameVar = "ABRT_CnbEaYtp_ConboxEarYtop               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEAXBT"  ; DbVarComment="CONBOXEARXBOT               " ; NameVar = "ABRT_CnbEaXbt_ConboxEarXbot               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBEAYBT"  ; DbVarComment="CONBOXEARYBOT               " ; NameVar = "ABRT_CnbEaYbt_ConboxEarYbot               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECXOL"  ; DbVarComment="CONBOXEARCUTXOUTLOW         " ; NameVar = "ABRT_CnbECXol_ConboxEarCutXoutlow         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECYOL"  ; DbVarComment="CONBOXEARCUTYOUTLOW         " ; NameVar = "ABRT_CnbECYol_ConboxEarCutYoutlow         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECYIL"  ; DbVarComment="CONBOXEARCUTYINLOW          " ; NameVar = "ABRT_CnbECYil_ConboxEarCutYinlow          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECOHE"  ; DbVarComment="CONBOXEARCUTOUTHEIGHT       " ; NameVar = "ABRT_CnbECoHe_ConboxEarCutOutHeight       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECXOU"  ; DbVarComment="CONBOXEARCUTXOUTUP          " ; NameVar = "ABRT_CnbECXou_ConboxEarCutXoutup          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECYOU"  ; DbVarComment="CONBOXEARCUTYOUTUP          " ; NameVar = "ABRT_CnbECYou_ConboxEarCutYoutup          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECYIU"  ; DbVarComment="CONBOXEARCUTYINUP           " ; NameVar = "ABRT_CnbECYiu_ConboxEarCutYinup           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBECZPO"  ; DbVarComment="CONBOXEARCUTZPOS            " ; NameVar = "ABRT_CnbECZpo_ConboxEarCutZpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBIECZP"  ; DbVarComment="CONBOXINNEREARCUTZPOS       " ; NameVar = "ABRT_CnbIECZp_ConboxInnerEarCutZpos       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBCAZIN"  ; DbVarComment="CONBOXCAVITYZINTER          " ; NameVar = "ABRT_CnbCaZin_ConboxCavityZinter          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VOUSSRAD"  ; DbVarComment="VOUSSOIRRADIUS              " ; NameVar = "ABRT_VoussRad_VoussoirRadius              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBOXRAD"  ; DbVarComment="CONBOXRADIUS                " ; NameVar = "ABRT_CnboxRad_ConboxRadius                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::AECT(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AECT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "CRYOT1"      ; DbVarComment="CRYOT1           " ; NameVar = "AECT_CryoT1                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOT2"      ; DbVarComment="CRYOT2           " ; NameVar = "AECT_CryoT2                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOS1"      ; DbVarComment="CRYOS1           " ; NameVar = "AECT_CryoS1                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOS2"      ; DbVarComment="CRYOS2           " ; NameVar = "AECT_CryoS2                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOEDGE"    ; DbVarComment="CRYOEDGE         " ; NameVar = "AECT_CryoEdge                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOFLTY"    ; DbVarComment="FLATY            " ; NameVar = "AECT_CryoFltY_FlatY            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOR0"      ; DbVarComment="CRYOR0           " ; NameVar = "AECT_CryoR0                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOZMIN"    ; DbVarComment="CRYOZMIN         " ; NameVar = "AECT_CryoZmin                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOZEXT"    ; DbVarComment="CRYOZEXT         " ; NameVar = "AECT_CryoZext                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOTHI1"    ; DbVarComment="THICK1           " ; NameVar = "AECT_CryoThi1_Thick1           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOTHI2"    ; DbVarComment="THICK2           " ; NameVar = "AECT_CryoThi2_Thick2           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOTHI3"    ; DbVarComment="THICK3           " ; NameVar = "AECT_CryoThi3_Thick3           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOTTU0"    ; DbVarComment="CRYOTTU0         " ; NameVar = "AECT_CryoTtu0                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORTU0"    ; DbVarComment="CRYORTU0         " ; NameVar = "AECT_CryoRtu0                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYORTU1"    ; DbVarComment="CRYORTU1         " ; NameVar = "AECT_CryoRtu1                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYODPHT"    ; DbVarComment="DPHITU           " ; NameVar = "AECT_CryodPhT_dPhiTu           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTL0"    ; DbVarComment="CRYOSTOPLENGTH0  " ; NameVar = "AECT_CryoStL0_CryoStopLength0  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTL1"    ; DbVarComment="CRYOSTOPLENGTH1  " ; NameVar = "AECT_CryoStL1_CryoStopLength1  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTH0"    ; DbVarComment="CRYOSTOPHEIGHT0  " ; NameVar = "AECT_CryoStH0_CryoStopHeight0  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTW0"    ; DbVarComment="CRYOSTOPWIDTH0   " ; NameVar = "AECT_CryoStW0_CryoStopWidth0   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTW1"    ; DbVarComment="CRYOSTOPWIDTH1   " ; NameVar = "AECT_CryoStW1_CryoStopWidth1   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTTH"    ; DbVarComment="CRYOSTOPTHICKNESS" ; NameVar = "AECT_CryoStTh_CryoStopThickness" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTH1"    ; DbVarComment="CRYOSTOPHEIGHT1  " ; NameVar = "AECT_CryoStH1_CryoStopHeight1  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTH2"    ; DbVarComment="CRYOSTOPHEIGHT2  " ; NameVar = "AECT_CryoStH2_CryoStopHeight2  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSTPZ"    ; DbVarComment="CRYOSTOPPOSIZ    " ; NameVar = "AECT_CryoStPZ_CryoStopPosiZ    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSUPL"    ; DbVarComment="CRYOSUPL         " ; NameVar = "AECT_CryoSupL                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSUPH"    ; DbVarComment="CRYOSUPH         " ; NameVar = "AECT_CryoSupH                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSUPW"    ; DbVarComment="CRYOSUPW         " ; NameVar = "AECT_CryoSupW                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOSUPX"    ; DbVarComment="CRYOSUPX         " ; NameVar = "AECT_CryoSupX                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXZMOF"    ; DbVarComment="CONDBOXZMINOFFSET" ; NameVar = "AECT_CnBxZmOf_CondBoxZminOffset" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXZEXT"    ; DbVarComment="CONDBOXZEXT      " ; NameVar = "AECT_CnBxZext_CondBoxZext      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXTMIN"    ; DbVarComment="CONDBOXTMIN      " ; NameVar = "AECT_CnBxTmin_CondBoxTmin      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXTMAX"    ; DbVarComment="CONDBOXTMAX      " ; NameVar = "AECT_CnBxTmax_CondBoxTmax      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXTHIC"    ; DbVarComment="CONDBOXTHICK     " ; NameVar = "AECT_CnBxThic_CondBoxThick     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXEDGE"    ; DbVarComment="CONDBOXEDGE      " ; NameVar = "AECT_CnBxEdge_CondBoxEdge      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLZOF"    ; DbVarComment="SUPPLATZOFFSET   " ; NameVar = "AECT_SupPlZof_SupPlatZoffset   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLZEX"    ; DbVarComment="SUPPLATZEXT      " ; NameVar = "AECT_SupPlZex_SupPlatZext      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLTMI"    ; DbVarComment="SUPPLATTMIN      " ; NameVar = "AECT_SupPlTmi_SupPlatTmin      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLTMA"    ; DbVarComment="SUPPLATTMAX      " ; NameVar = "AECT_SupPlTma_SupPlatTmax      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLTHZ"    ; DbVarComment="SUPPLATTHICKZ    " ; NameVar = "AECT_SupPlThZ_SupPlatThickZ    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLTHT"    ; DbVarComment="SUPPLATTHICKT    " ; NameVar = "AECT_SupPlThT_SupPlatThickT    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLRHO"    ; DbVarComment="SUPPLATRHOLE     " ; NameVar = "AECT_SupPlRho_SupPlatRhole     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU1DZ"    ; DbVarComment="SERVTUR1DZ       " ; NameVar = "AECT_SrvTu1Dz_ServTur1Dz       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU1HE"    ; DbVarComment="SERVTUR1HEIGHT   " ; NameVar = "AECT_SrvTu1He_ServTur1Height   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU1OW"    ; DbVarComment="SERVTUR1OUTWIDTH " ; NameVar = "AECT_SrvTu1oW_ServTur1OutWidth " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU1IW"    ; DbVarComment="SERVTUR1INNWIDTH " ; NameVar = "AECT_SrvTu1iW_ServTur1InnWidth " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU1ED"    ; DbVarComment="SERVTUR1EDGE     " ; NameVar = "AECT_SrvTu1Ed_ServTur1Edge     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU2DZ"    ; DbVarComment="SERVTUR2DZ       " ; NameVar = "AECT_SrvTu2Dz_ServTur2Dz       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU2HE"    ; DbVarComment="SERVTUR2HEIGHT   " ; NameVar = "AECT_SrvTu2He_ServTur2Height   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU2OR"    ; DbVarComment="SERVTUR2OUTRAD   " ; NameVar = "AECT_SrvTu2oR_ServTur2OutRad   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU2IR"    ; DbVarComment="SERVTUR2INNRAD   " ; NameVar = "AECT_SrvTu2iR_ServTur2InnRad   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU3DZ"    ; DbVarComment="SERVTUR3DZ       " ; NameVar = "AECT_SrvTu3Dz_ServTur3Dz       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SRVTU3HE"    ; DbVarComment="SERVTUR3HEIGHT   " ; NameVar = "AECT_SrvTu3He_ServTur3Height   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SIFITU"      ; DbVarComment="SIFITU           " ; NameVar = "AECT_SiFiTu                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COFITU"      ; DbVarComment="COFITU           " ; NameVar = "AECT_CoFiTu                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOZMAX"    ; DbVarComment="CRYOZMAX         " ; NameVar = "AECT_CryoZmax                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXZMIN"    ; DbVarComment="CONDBOXZMIN      " ; NameVar = "AECT_CnBxZmin_CondBoxZmin      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNBXZMAX"    ; DbVarComment="CONDBOXZMAX      " ; NameVar = "AECT_CnBxZmax_CondBoxZmax      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLZMI"    ; DbVarComment="SUPPLATZMIN      " ; NameVar = "AECT_SupPlZmi_SupPlatZmin      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLZMA"    ; DbVarComment="SUPPLATZMAX      " ; NameVar = "AECT_SupPlZma_SupPlatZmax      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOZMOY"    ; DbVarComment="CRYOZMOY         " ; NameVar = "AECT_CryoZmoy                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOALFA"    ; DbVarComment="CRYOALFA         " ; NameVar = "AECT_CryoAlfa                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOBETA"    ; DbVarComment="CRYOBETA         " ; NameVar = "AECT_CryoBeta                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOT1P"     ; DbVarComment="CRYOT1P          " ; NameVar = "AECT_CryoT1P                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOT2P"     ; DbVarComment="CRYOT2P          " ; NameVar = "AECT_CryoT2P                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOS1P"     ; DbVarComment="CRYOS1P          " ; NameVar = "AECT_CryoS1P                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOS2P"     ; DbVarComment="CRYOS2P          " ; NameVar = "AECT_CryoS2P                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CRYOFLYP"    ; DbVarComment="FLATYP           " ; NameVar = "AECT_CryoFlYP_FlatYP           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLZME"    ; DbVarComment="SUPPLATZMOY      " ; NameVar = "AECT_SupPlZme_SupPlatZmoy      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTEMP_0"     ; DbVarComment="ARRAY            " ; NameVar = "AECT_Xtemp%0                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTEMP_1"     ; DbVarComment="                 " ; NameVar = "AECT_Xtemp%1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTEMP_2"     ; DbVarComment="                 " ; NameVar = "AECT_Xtemp%2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTEMP_0"     ; DbVarComment="ARRAY            " ; NameVar = "AECT_Ytemp%0                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTEMP_1"     ; DbVarComment="                 " ; NameVar = "AECT_Ytemp%1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTEMP_2"     ; DbVarComment="                 " ; NameVar = "AECT_Ytemp%2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "DTEMP01"     ; DbVarComment="DTEMP01          " ; NameVar = "AECT_Dtemp01                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTIMP_0"     ; DbVarComment="ARRAY            " ; NameVar = "AECT_Xtimp%0                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTIMP_1"     ; DbVarComment="                 " ; NameVar = "AECT_Xtimp%1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XTIMP_2"     ; DbVarComment="                 " ; NameVar = "AECT_Xtimp%2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTIMP_0"     ; DbVarComment="ARRAY            " ; NameVar = "AECT_Ytimp%0                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTIMP_1"     ; DbVarComment="                 " ; NameVar = "AECT_Ytimp%1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YTIMP_2"     ; DbVarComment="                 " ; NameVar = "AECT_Ytimp%2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "DTIMP01"     ; DbVarComment="DTIMP01          " ; NameVar = "AECT_Dtimp01                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UTEMP61"     ; DbVarComment="UTEMP61          " ; NameVar = "AECT_Utemp61                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UTEMP62"     ; DbVarComment="UTEMP62          " ; NameVar = "AECT_Utemp62                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UTIMP61"     ; DbVarComment="UTIMP61          " ; NameVar = "AECT_Utimp61                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UTIMP62"     ; DbVarComment="UTIMP62          " ; NameVar = "AECT_Utimp62                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "DTEMP"       ; DbVarComment="DTEMP            " ; NameVar = "AECT_Dtemp                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATY_0"  ; DbVarComment="ARRAY            " ; NameVar = "AECT_SupPlatY%0                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATY_1"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatY%1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATY_2"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatY%2                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATY_3"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatY%3                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATX_0"  ; DbVarComment="ARRAY            " ; NameVar = "AECT_SupPlatX%0                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATX_1"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatX%1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATX_2"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatX%2                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SUPPLATX_3"  ; DbVarComment="                 " ; NameVar = "AECT_SupPlatX%3                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "X1TEMP"      ; DbVarComment="X1TEMP           " ; NameVar = "AECT_X1temp                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "X0TEMP"      ; DbVarComment="X0TEMP           " ; NameVar = "AECT_X0temp                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;


  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::ECST(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ECST";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "STOTHIC1"      ; DbVarComment="STOTHIC1  " ; NameVar = "ECST_SToThic1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOTHIC2"      ; DbVarComment="STOTHIC2  " ; NameVar = "ECST_SToThic2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOTHIC3"      ; DbVarComment="STOTHIC3  " ; NameVar = "ECST_SToThic3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOTHIC4"      ; DbVarComment="STOTHIC4  " ; NameVar = "ECST_SToThic4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOLENGT"      ; DbVarComment="STOLENGT  " ; NameVar = "ECST_SToLengt" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOLENGA"      ; DbVarComment="STOLENGA  " ; NameVar = "ECST_SToLenga" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOXPREF"      ; DbVarComment="STOXPREF  " ; NameVar = "ECST_SToXpref" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOYPREF"      ; DbVarComment="STOYPREF  " ; NameVar = "ECST_SToYpref" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOHEIGH"      ; DbVarComment="STOHEIGH  " ; NameVar = "ECST_SToHeigh" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOHEIG1"      ; DbVarComment="STOHEIG1  " ; NameVar = "ECST_SToHeig1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOHEIG2"      ; DbVarComment="STOHEIG2  " ; NameVar = "ECST_SToHeig2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOHEIG3"      ; DbVarComment="STOHEIG3  " ; NameVar = "ECST_SToHeig3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOHEIG4"      ; DbVarComment="STOHEIG4  " ; NameVar = "ECST_SToHeig4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZBOT"      ; DbVarComment="STODZBOT  " ; NameVar = "ECST_SToDzBot" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZIN1"      ; DbVarComment="STODZIN1  " ; NameVar = "ECST_SToDzIn1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZIN2"      ; DbVarComment="STODZIN2  " ; NameVar = "ECST_SToDzIn2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZIN3"      ; DbVarComment="STODZIN3  " ; NameVar = "ECST_SToDzIn3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZIN4"      ; DbVarComment="STODZIN4  " ; NameVar = "ECST_SToDzIn4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZIN5"      ; DbVarComment="STODZIN5  " ; NameVar = "ECST_SToDzIn5" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STODZTOP"      ; DbVarComment="STODZTOP  " ; NameVar = "ECST_SToDzTop" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOANGLE"      ; DbVarComment="STOANGLE  " ; NameVar = "ECST_SToAngle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOYPOS"       ; DbVarComment="STOYPOS   " ; NameVar = "ECST_SToYpos " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOGECXZ"      ; DbVarComment="STOGECXZ  " ; NameVar = "ECST_SToGeCxz" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOGECHE"      ; DbVarComment="STOGECHE  " ; NameVar = "ECST_SToGeChe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCLCZP"      ; DbVarComment="STOCLCZP  " ; NameVar = "ECST_SToCLCzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOANGD2"      ; DbVarComment="STOANGD2  " ; NameVar = "ECST_SToAngd2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOANGD4"      ; DbVarComment="STOANGD4  " ; NameVar = "ECST_SToAngd4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOLENG1"      ; DbVarComment="STOLENG1  " ; NameVar = "ECST_SToLeng1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFXPOS"      ; DbVarComment="STOFXPOS  " ; NameVar = "ECST_SToFxpos" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOZPOS"       ; DbVarComment="STOZPOS   " ; NameVar = "ECST_SToZpos " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEYP"       ; DbVarComment="STOUEYP   " ; NameVar = "ECST_SToUeYp " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOTEYP"       ; DbVarComment="STOTEYP   " ; NameVar = "ECST_SToTeYp " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOTEZP"       ; DbVarComment="STOTEZP   " ; NameVar = "ECST_SToTeZp " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXA"       ; DbVarComment="STOFLXA   " ; NameVar = "ECST_SToFlXA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYA"       ; DbVarComment="STOFLYA   " ; NameVar = "ECST_SToFlYA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXB"       ; DbVarComment="STOFLXB   " ; NameVar = "ECST_SToFlXB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYB"       ; DbVarComment="STOFLYB   " ; NameVar = "ECST_SToFlYB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXC"       ; DbVarComment="STOFLXC   " ; NameVar = "ECST_SToFlXC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYC"       ; DbVarComment="STOFLYC   " ; NameVar = "ECST_SToFlYC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXD"       ; DbVarComment="STOFLXD   " ; NameVar = "ECST_SToFlXD " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYD"       ; DbVarComment="STOFLYD   " ; NameVar = "ECST_SToFlYD " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXE"       ; DbVarComment="STOFLXE   " ; NameVar = "ECST_SToFlXE " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYE"       ; DbVarComment="STOFLYE   " ; NameVar = "ECST_SToFlYE " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXF"       ; DbVarComment="STOFLXF   " ; NameVar = "ECST_SToFlXF " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYF"       ; DbVarComment="STOFLYF   " ; NameVar = "ECST_SToFlYF " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXG"       ; DbVarComment="STOFLXG   " ; NameVar = "ECST_SToFlXG " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYG"       ; DbVarComment="STOFLYG   " ; NameVar = "ECST_SToFlYG " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXH"       ; DbVarComment="STOFLXH   " ; NameVar = "ECST_SToFlXH " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYH"       ; DbVarComment="STOFLYH   " ; NameVar = "ECST_SToFlYH " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLXI"       ; DbVarComment="STOFLXI   " ; NameVar = "ECST_SToFlXI " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOFLYI"       ; DbVarComment="STOFLYI   " ; NameVar = "ECST_SToFlYI " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXA"       ; DbVarComment="STOCOXA   " ; NameVar = "ECST_SToCoXA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYA"       ; DbVarComment="STOCOYA   " ; NameVar = "ECST_SToCoYA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXA1"      ; DbVarComment="STOCOXA1  " ; NameVar = "ECST_SToCoXA1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYA1"      ; DbVarComment="STOCOYA1  " ; NameVar = "ECST_SToCoYA1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXB"       ; DbVarComment="STOCOXB   " ; NameVar = "ECST_SToCoXB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYB"       ; DbVarComment="STOCOYB   " ; NameVar = "ECST_SToCoYB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXB1"      ; DbVarComment="STOCOXB1  " ; NameVar = "ECST_SToCoXB1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYB1"      ; DbVarComment="STOCOYB1  " ; NameVar = "ECST_SToCoYB1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXC"       ; DbVarComment="STOCOXC   " ; NameVar = "ECST_SToCoXC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYC"       ; DbVarComment="STOCOYC   " ; NameVar = "ECST_SToCoYC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXC1"      ; DbVarComment="STOCOXC1  " ; NameVar = "ECST_SToCoXC1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYC1"      ; DbVarComment="STOCOYC1  " ; NameVar = "ECST_SToCoYC1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXD"       ; DbVarComment="STOCOXD   " ; NameVar = "ECST_SToCoXD " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYD"       ; DbVarComment="STOCOYD   " ; NameVar = "ECST_SToCoYD " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXD1"      ; DbVarComment="STOCOXD1  " ; NameVar = "ECST_SToCoXD1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYD1"      ; DbVarComment="STOCOYD1  " ; NameVar = "ECST_SToCoYD1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXE"       ; DbVarComment="STOCOXE   " ; NameVar = "ECST_SToCoXE " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYE"       ; DbVarComment="STOCOYE   " ; NameVar = "ECST_SToCoYE " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXE1"      ; DbVarComment="STOCOXE1  " ; NameVar = "ECST_SToCoXE1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYE1"      ; DbVarComment="STOCOYE1  " ; NameVar = "ECST_SToCoYE1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXF"       ; DbVarComment="STOCOXF   " ; NameVar = "ECST_SToCoXF " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYF"       ; DbVarComment="STOCOYF   " ; NameVar = "ECST_SToCoYF " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXF1"      ; DbVarComment="STOCOXF1  " ; NameVar = "ECST_SToCoXF1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYF1"      ; DbVarComment="STOCOYF1  " ; NameVar = "ECST_SToCoYF1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXG"       ; DbVarComment="STOCOXG   " ; NameVar = "ECST_SToCoXG " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYG"       ; DbVarComment="STOCOYG   " ; NameVar = "ECST_SToCoYG " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXG1"      ; DbVarComment="STOCOXG1  " ; NameVar = "ECST_SToCoXG1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYG1"      ; DbVarComment="STOCOYG1  " ; NameVar = "ECST_SToCoYG1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXH"       ; DbVarComment="STOCOXH   " ; NameVar = "ECST_SToCoXH " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYH"       ; DbVarComment="STOCOYH   " ; NameVar = "ECST_SToCoYH " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXH1"      ; DbVarComment="STOCOXH1  " ; NameVar = "ECST_SToCoXH1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYH1"      ; DbVarComment="STOCOYH1  " ; NameVar = "ECST_SToCoYH1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXI"       ; DbVarComment="STOCOXI   " ; NameVar = "ECST_SToCoXI " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYI"       ; DbVarComment="STOCOYI   " ; NameVar = "ECST_SToCoYI " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOXI1"      ; DbVarComment="STOCOXI1  " ; NameVar = "ECST_SToCoXI1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCOYI1"      ; DbVarComment="STOCOYI1  " ; NameVar = "ECST_SToCoYI1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCO3DX"      ; DbVarComment="STOCO3DX  " ; NameVar = "ECST_SToCo3dx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCO3DY"      ; DbVarComment="STOCO3DY  " ; NameVar = "ECST_SToCo3dy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCO3DZ"      ; DbVarComment="STOCO3DZ  " ; NameVar = "ECST_SToCo3dz" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCO3ZP"      ; DbVarComment="STOCO3ZP  " ; NameVar = "ECST_SToCo3zp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOCO3YP"      ; DbVarComment="STOCO3YP  " ; NameVar = "ECST_SToCo3yp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEXA"       ; DbVarComment="STOUEXA   " ; NameVar = "ECST_SToUeXA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEYA"       ; DbVarComment="STOUEYA   " ; NameVar = "ECST_SToUeYA " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEXB"       ; DbVarComment="STOUEXB   " ; NameVar = "ECST_SToUeXB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEYB"       ; DbVarComment="STOUEYB   " ; NameVar = "ECST_SToUeYB " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEXC"       ; DbVarComment="STOUEXC   " ; NameVar = "ECST_SToUeXC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUEYC"       ; DbVarComment="STOUEYC   " ; NameVar = "ECST_SToUeYC " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECXA"      ; DbVarComment="STOUECXA  " ; NameVar = "ECST_SToUeCXA" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECYA"      ; DbVarComment="STOUECYA  " ; NameVar = "ECST_SToUeCYA" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECXB"      ; DbVarComment="STOUECXB  " ; NameVar = "ECST_SToUeCXB" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECYB"      ; DbVarComment="STOUECYB  " ; NameVar = "ECST_SToUeCYB" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECZP"      ; DbVarComment="STOUECZP  " ; NameVar = "ECST_SToUeCzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STOUECYP"      ; DbVarComment="STOUECYP  " ; NameVar = "ECST_SToUeCyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::FEET(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "FEET";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "MAINPLXO"      ; DbVarComment="MAINPLATEXORIGIN                        " ; NameVar = "FEET_MainPlXo_MainPlateXorigin                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLGXWI"      ; DbVarComment="MAINPLATE_GROUNDXWIDTH                  " ; NameVar = "FEET_MnPlGXwi_MainPlate_GroundXwidth                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLRCDX"      ; DbVarComment="MAINPLATEROUNDCUTDX                     " ; NameVar = "FEET_MnPlRCdx_MainPlateRoundCutDX                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLDXEX"      ; DbVarComment="MAINPLATEDXEXTR                         " ; NameVar = "FEET_MnPldxex_MainPlateDXextr                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLUPDX"      ; DbVarComment="MAINPLATEUPPERDX                        " ; NameVar = "FEET_MnPlUpDx_MainPlateUpperDX                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLUPXW"      ; DbVarComment="MAINPLATEUPPERXWIDTH                    " ; NameVar = "FEET_MnPlUpXw_MainPlateUpperXwidth                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLMIDX"      ; DbVarComment="MAINPLATEMIDDLEDX                       " ; NameVar = "FEET_MnPlMiDx_MainPlateMiddleDX                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYO"      ; DbVarComment="MAINPLATEYORIGIN                        " ; NameVar = "FEET_MainPlYo_MainPlateYorigin                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLHE"      ; DbVarComment="MAINPLATEHEIGHT                         " ; NameVar = "FEET_MainPlHe_MainPlateHeight                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLH1"      ; DbVarComment="MAINPLATEHEIGHT_INTERMEDIATE1           " ; NameVar = "FEET_MainPlH1_MainPlateHeight_intermediate1            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLRCYP"      ; DbVarComment="MAINPLATEROUNDCUT_Y                     " ; NameVar = "FEET_MnPlRCYp_MainPlateRoundCut_Y                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLSRCY"      ; DbVarComment="MAINPLATESECONDARYROUNDCUT_Y            " ; NameVar = "FEET_MnPlSRCY_MainPlateSecondaryRoundCut_Y             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLUPDY"      ; DbVarComment="MAINPLATEUPPERDY                        " ; NameVar = "FEET_MnPlUpDy_MainPlateUpperDY                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLMIDY"      ; DbVarComment="MAINPLATEMIDDLEDY                       " ; NameVar = "FEET_MnPlMiDy_MainPlateMiddleDY                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLPEHE"      ; DbVarComment="MAINPLATE_PEDESTALHEIGHT                " ; NameVar = "FEET_MnPlPeHe_MainPlate_PedestalHeight                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLDZ"      ; DbVarComment="MAINPLATEDZ                             " ; NameVar = "FEET_MainPldZ_MainPlateDZ                              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLZSEP"      ; DbVarComment="MAINPLATEZSEP                           " ; NameVar = "FEET_MnPlZsep_MainPlateZsep                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLRCRA"      ; DbVarComment="MAINPLATEROUNDCUT_R                     " ; NameVar = "FEET_MnPlRCra_MainPlateRoundCut_R                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MNPLSRCR"      ; DbVarComment="MAINPLATESECONDARYROUNDCUT_R            " ; NameVar = "FEET_MnPlSRCr_MainPlateSecondaryRoundCut_R             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MINCPLXW"      ; DbVarComment="MINICONNPLATE_XWIDTH                    " ; NameVar = "FEET_MinCPlxw_MiniConnPlate_Xwidth                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MINCPLYH"      ; DbVarComment="MINICONNPLATE_YHEIGHT                   " ; NameVar = "FEET_MinCPlyh_MiniConnPlate_Yheight                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MINCPLTZ"      ; DbVarComment="MINICONNPLATE_THETAZ                    " ; NameVar = "FEET_MinCPltz_MiniConnPlate_ThetaZ                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MINCPLZL"      ; DbVarComment="MINICONNPLATE_ZLENGTH                   " ; NameVar = "FEET_MinCPlzl_MiniConnPlate_Zlength                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLACPLXW"      ; DbVarComment="SLANTEDCONNPLATE_XWIDTH                 " ; NameVar = "FEET_SlaCPlxw_SlantedConnPlate_Xwidth                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLACPLYH"      ; DbVarComment="SLANTEDCONNPLATE_YHEIGHT                " ; NameVar = "FEET_SlaCPlyh_SlantedConnPlate_Yheight                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLACPLTZ"      ; DbVarComment="SLANTEDCONNPLATE_THETAZ                 " ; NameVar = "FEET_SlaCPltz_SlantedConnPlate_ThetaZ                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLACPLZL"      ; DbVarComment="SLANTEDCONNPLATE_ZLENGTH                " ; NameVar = "FEET_SlaCPlzl_SlantedConnPlate_Zlength                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GRNDPLZL"      ; DbVarComment="GROUNDPLATE_ZLENGTH                     " ; NameVar = "FEET_GrndPlzl_GroundPlate_Zlength                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VCNPLXWI"      ; DbVarComment="VERTICALCONNPLATE_XWIDTH                " ; NameVar = "FEET_VCnPlxwi_VerticalConnPlate_Xwidth                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VCNPLYHE"      ; DbVarComment="VERTICALCONNPLATE_YHEIGHT               " ; NameVar = "FEET_VCnPlyhe_VerticalConnPlate_Yheight                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VCNPLDXE"      ; DbVarComment="VERTICALCONNPLATE_DX                    " ; NameVar = "FEET_VCnPldxe_VerticalConnPlate_DX                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VCNPLDYE"      ; DbVarComment="VERTICALCONNPLATE_DY                    " ; NameVar = "FEET_VCnPldye_VerticalConnPlate_DY                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UCNPLXWI"      ; DbVarComment="UPPERCONNPLATE_XWIDTH                   " ; NameVar = "FEET_UCnPlxwi_UpperConnPlate_Xwidth                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UCNPLYHE"      ; DbVarComment="UPPERCONNPLATE_YHEIGHT                  " ; NameVar = "FEET_UCnPlyhe_UpperConnPlate_Yheight                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XWIDTH"        ; DbVarComment="XWIDTH                                  " ; NameVar = "FEET_Xwidth                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UPPERHEY"      ; DbVarComment="RAILSUPPORTUPPER_YHEIGHT                " ; NameVar = "FEET_UpperHeY_RailSupportUpper_Yheight                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UPPERLEZ"      ; DbVarComment="RAILSUPPORTUPPER_ZLENGTH                " ; NameVar = "FEET_UpperLeZ_RailSupportUpper_Zlength                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LOWERHEY"      ; DbVarComment="RAILSUPPORTLOWER_YHEIGHT                " ; NameVar = "FEET_LowerHeY_RailSupportLower_Yheight                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LOWERLEZ"      ; DbVarComment="RAILSUPPORTLOWER_ZLENGTH                " ; NameVar = "FEET_LowerLeZ_RailSupportLower_Zlength                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TOTALHEY"      ; DbVarComment="RAILSUPPORTTOTAL_YHEIGHT                " ; NameVar = "FEET_TotalHeY_RailSupportTotal_Yheight                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CENTRLEZ"      ; DbVarComment="RAILSUPPORTCENTRAL_ZLENGTH              " ; NameVar = "FEET_CentrLeZ_RailSupportCentral_Zlength               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CENTRXTH"      ; DbVarComment="RAILSUPPORTCENTRAL_XTHICK               " ; NameVar = "FEET_CentrXth_RailSupportCentral_Xthick                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MIDLLYTH"      ; DbVarComment="RAILSUPPORTMIDDLE_YTHICK                " ; NameVar = "FEET_MidllYth_RailSupportMiddle_Ythick                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VERTIZTH"      ; DbVarComment="RAILSUPPORTVERTICAL_ZTHICK              " ; NameVar = "FEET_VertiZth_RailSupportVertical_Zthick               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MIDLLYPO"      ; DbVarComment="RAILSUPPORTMIDDLE_YPOS                  " ; NameVar = "FEET_MidllYpo_RailSupportMiddle_Ypos                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXTREHEY"      ; DbVarComment="RAILSUPPORTEXTR_YHEIGHT                 " ; NameVar = "FEET_ExtreHeY_RailSupportExtr_Yheight                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXTRELEZ"      ; DbVarComment="RAILSUPPORTEXTR_ZLENGTH                 " ; NameVar = "FEET_ExtreLeZ_RailSupportExtr_Zlength                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CENTRHEY"      ; DbVarComment="RAILSUPPORTCENTRAL_YHEIGHT              " ; NameVar = "FEET_CentrHeY_RailSupportCentral_Yheight               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOXW"      ; DbVarComment="CONNFEETVOUSS_XWIDTH                    " ; NameVar = "FEET_CnFeVoXw_ConnFeetVouss_Xwidth                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOYH"      ; DbVarComment="CONNFEETVOUSS_YHEIGHT                   " ; NameVar = "FEET_CnFeVoYh_ConnFeetVouss_Yheight                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOZL"      ; DbVarComment="CONNFEETVOUSS_ZLENGTH                   " ; NameVar = "FEET_CnFeVoZl_ConnFeetVouss_Zlength                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOXT"      ; DbVarComment="CONNFEETVOUSS_XTHICK                    " ; NameVar = "FEET_CnFeVoXt_ConnFeetVouss_Xthick                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOYT"      ; DbVarComment="CONNFEETVOUSS_YTHICK                    " ; NameVar = "FEET_CnFeVoYt_ConnFeetVouss_Ythick                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOZT"      ; DbVarComment="CONNFEETVOUSS_ZTHICK                    " ; NameVar = "FEET_CnFeVoZt_ConnFeetVouss_Zthick                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPXWT"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_TOTAL      " ; NameVar = "FEET_G12LPxwt_Girder12_LateralPlate_Xwidth_Total       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSXO"      ; DbVarComment="GIRDER12_INNERPLATESMALLOFFSET_XDIST    " ; NameVar = "FEET_G12IPSxo_Girder12_InnerPlateSmalloffset_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSLX"      ; DbVarComment="GIRDER12_INNERPLATESMALL2LARGE_XDIST    " ; NameVar = "FEET_G12IPSLx_Girder12_InnerPlateSmall2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLLX"      ; DbVarComment="GIRDER12_INNERPLATELARGE2LARGE_XDIST    " ; NameVar = "FEET_G12IPLLx_Girder12_InnerPlateLarge2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLSX"      ; DbVarComment="GIRDER12_INNERPLATELARGE2SMALL_XDIST    " ; NameVar = "FEET_G12IPLSx_Girder12_InnerPlateLarge2Small_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12RPXWI"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XWIDTH      " ; NameVar = "FEET_G12RPxwi_Girder12_ReinforcementPlate_Xwidth       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPXW1"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_INTER1     " ; NameVar = "FEET_G12LPxw1_Girder12_LateralPlate_Xwidth_Inter1      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPX12"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_INTER1INTER" ; NameVar = "FEET_G12LPx12_Girder12_LateralPlate_Xwidth_Inter1Inter2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPXWI"      ; DbVarComment="GIRDER12_INNERPLATE_XWIDTH              " ; NameVar = "FEET_G12IPxwi_Girder12_InnerPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UMPWI"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_XWIDTH          " ; NameVar = "FEET_G12UMPwi_Girder12_UpperMiniPlate_Xwidth           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPWI"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_XWIDTH           " ; NameVar = "FEET_G12SMPwi_Girder12_SideMiniPlate_Xwidth            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12YORIG"      ; DbVarComment="GIRDER12_YORIGIN                        " ; NameVar = "FEET_G12yorig_Girder12_Yorigin                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPYHT"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_TOTAL     " ; NameVar = "FEET_G12LPyht_Girder12_LateralPlate_Yheight_Total      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPYH1"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_INTER1    " ; NameVar = "FEET_G12LPyh1_Girder12_LateralPlate_Yheight_Inter1     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPYH2"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_INTER2    " ; NameVar = "FEET_G12LPyh2_Girder12_LateralPlate_Yheight_Inter2     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPYTH"      ; DbVarComment="GIRDER12_LOWERPLATE_YTHICK              " ; NameVar = "FEET_G12LPyth_Girder12_LowerPlate_Ythick               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UPYTH"      ; DbVarComment="GIRDER12_UPPERPLATE_YTHICK              " ; NameVar = "FEET_G12UPyth_Girder12_UpperPlate_Ythick               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12RPYDI"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YDIST       " ; NameVar = "FEET_G12RPydi_Girder12_ReinforcementPlate_Ydist        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UMPYH"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_YHEIGHT         " ; NameVar = "FEET_G12UMPyh_Girder12_UpperMiniPlate_Yheight          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UPZLE"      ; DbVarComment="GIRDER12_UPPERPLATE_ZLENGTH             " ; NameVar = "FEET_G12UPzle_Girder12_UpperPlate_Zlength              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UMPZL"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_ZLENGTH         " ; NameVar = "FEET_G12UMPzl_Girder12_UpperMiniPlate_Zlength          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BPZLE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_ZLENGTH            " ; NameVar = "FEET_G12BPzle_Girder12_BottomPlate_Zlength             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12LPZLE"      ; DbVarComment="GIRDER12_LATERALPLATE_ZLENGTH           " ; NameVar = "FEET_G12LPzle_Girder12_LateralPlate_Zlength            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12RPZLE"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_ZLENGTH     " ; NameVar = "FEET_G12RPzle_Girder12_ReinforcementPlate_Zlength      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPZL"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_ZLENGTH          " ; NameVar = "FEET_G12SMPzl_Girder12_SideMiniPlate_Zlength           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23LPXWT"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_TOTAL      " ; NameVar = "FEET_G23LPxwt_Girder23_LateralPlate_Xwidth_Total       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPSXO"      ; DbVarComment="GIRDER23_INNERPLATESMALLOFFSET_XDIST    " ; NameVar = "FEET_G23IPSxo_Girder23_InnerPlateSmalloffset_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPSLX"      ; DbVarComment="GIRDER23_INNERPLATESMALL2LARGE_XDIST    " ; NameVar = "FEET_G23IPSLx_Girder23_InnerPlateSmall2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPLLX"      ; DbVarComment="GIRDER23_INNERPLATELARGE2LARGE_XDIST    " ; NameVar = "FEET_G23IPLLx_Girder23_InnerPlateLarge2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPLSX"      ; DbVarComment="GIRDER23_INNERPLATELARGE2SMALL_XDIST    " ; NameVar = "FEET_G23IPLSx_Girder23_InnerPlateLarge2Small_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23RPXWI"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XWIDTH      " ; NameVar = "FEET_G23RPxwi_Girder23_ReinforcementPlate_Xwidth       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23LPXW1"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_INTER1     " ; NameVar = "FEET_G23LPxw1_Girder23_LateralPlate_Xwidth_Inter1      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23LPX12"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_INTER1INTER" ; NameVar = "FEET_G23LPx12_Girder23_LateralPlate_Xwidth_Inter1Inter2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPXWI"      ; DbVarComment="GIRDER23_INNERPLATE_XWIDTH              " ; NameVar = "FEET_G23IPxwi_Girder23_InnerPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23UMPWI"      ; DbVarComment="GIRDER23_UPPERMINIPLATE_XWIDTH          " ; NameVar = "FEET_G23UMPwi_Girder23_UpperMiniPlate_Xwidth           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23SMPWI"      ; DbVarComment="GIRDER23_SIDEMINIPLATE_XWIDTH           " ; NameVar = "FEET_G23SMPwi_Girder23_SideMiniPlate_Xwidth            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34LPXWT"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_TOTAL      " ; NameVar = "FEET_G34LPxwt_Girder34_LateralPlate_Xwidth_Total       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPSXO"      ; DbVarComment="GIRDER34_INNERPLATESMALLOFFSET_XDIST    " ; NameVar = "FEET_G34IPSxo_Girder34_InnerPlateSmalloffset_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPSLX"      ; DbVarComment="GIRDER34_INNERPLATESMALL2LARGE_XDIST    " ; NameVar = "FEET_G34IPSLx_Girder34_InnerPlateSmall2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPLLX"      ; DbVarComment="GIRDER34_INNERPLATELARGE2LARGE_XDIST    " ; NameVar = "FEET_G34IPLLx_Girder34_InnerPlateLarge2Large_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPLSX"      ; DbVarComment="GIRDER34_INNERPLATELARGE2SMALL_XDIST    " ; NameVar = "FEET_G34IPLSx_Girder34_InnerPlateLarge2Small_Xdist     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34RPXWI"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XWIDTH      " ; NameVar = "FEET_G34RPxwi_Girder34_ReinforcementPlate_Xwidth       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34LPXW1"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_INTER1     " ; NameVar = "FEET_G34LPxw1_Girder34_LateralPlate_Xwidth_Inter1      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34LPX12"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_INTER1INTER" ; NameVar = "FEET_G34LPx12_Girder34_LateralPlate_Xwidth_Inter1Inter2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPXWI"      ; DbVarComment="GIRDER34_INNERPLATE_XWIDTH              " ; NameVar = "FEET_G34IPxwi_Girder34_InnerPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34UMPWI"      ; DbVarComment="GIRDER34_UPPERMINIPLATE_XWIDTH          " ; NameVar = "FEET_G34UMPwi_Girder34_UpperMiniPlate_Xwidth           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34SMPWI"      ; DbVarComment="GIRDER34_SIDEMINIPLATE_XWIDTH           " ; NameVar = "FEET_G34SMPwi_Girder34_SideMiniPlate_Xwidth            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPHEIG"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT               " ; NameVar = "FEET_ExMPheig_ExtremityMainPlate_Height                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPRCRA"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_R            " ; NameVar = "FEET_ExMPRCra_ExtremityMainPlateRoundCut_R             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPRCDX"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_DX           " ; NameVar = "FEET_ExMPRCdx_ExtremityMainPlateRoundCut_DX            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPRCYP"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_Y            " ; NameVar = "FEET_ExMPRCyp_ExtremityMainPlateRoundCut_Y             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMCPYHE"      ; DbVarComment="EXTREMITYMINICONNPLATE_YHEIGHT          " ; NameVar = "FEET_ExMCPyhe_ExtremityMiniConnPlate_Yheight           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMCPZLE"      ; DbVarComment="EXTREMITYMINICONNPLATE_ZLENGTH          " ; NameVar = "FEET_ExMCPzle_ExtremityMiniConnPlate_Zlength           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXGPZLEN"      ; DbVarComment="EXTREMITYGROUNDPLATE_ZLENGTH            " ; NameVar = "FEET_ExGPzlen_ExtremityGroundPlate_Zlength             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXSCPZLE"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATE_ZLENGTH       " ; NameVar = "FEET_ExSCPzle_ExtremitySlantedConnPlate_Zlength        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSZOFF"      ; DbVarComment="EXTREMITYRAILSUPPORT_ZOFFSET            " ; NameVar = "FEET_ExRSzoff_ExtremityRailSupport_Zoffset             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPHEI1"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE1 " ; NameVar = "FEET_ExMPhei1_ExtremityMainPlate_Height_intermediate1  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPHEI2"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE2 " ; NameVar = "FEET_ExMPhei2_ExtremityMainPlate_Height_intermediate2  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXWID"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH               " ; NameVar = "FEET_ExMPxwid_ExtremityMainPlate_Xwidth                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXWI1"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH_INTER1        " ; NameVar = "FEET_ExMPxwi1_ExtremityMainPlate_Xwidth_inter1         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXWI2"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH_INTER2        " ; NameVar = "FEET_ExMPxwi2_ExtremityMainPlate_Xwidth_inter2         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYHI1"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER1       " ; NameVar = "FEET_ExMPyhi1_ExtremityMainPlate_Yheight_inter1        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYHI2"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER2       " ; NameVar = "FEET_ExMPyhi2_ExtremityMainPlate_Yheight_inter2        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYHI3"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER3       " ; NameVar = "FEET_ExMPyhi3_ExtremityMainPlate_Yheight_inter3        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYHI4"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER4       " ; NameVar = "FEET_ExMPyhi4_ExtremityMainPlate_Yheight_inter4        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXDI1"      ; DbVarComment="EXTREMITYMAINPLATE_XDIST_INTER1         " ; NameVar = "FEET_ExMPxdi1_ExtremityMainPlate_Xdist_inter1          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXDI2"      ; DbVarComment="EXTREMITYMAINPLATE_XDIST_INTER2         " ; NameVar = "FEET_ExMPxdi2_ExtremityMainPlate_Xdist_inter2          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYDI2"      ; DbVarComment="EXTREMITYMAINPLATE_YDIST_INTER1         " ; NameVar = "FEET_ExMPydi2_ExtremityMainPlate_Ydist_inter1          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLDX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_DX           " ; NameVar = "FEET_ExVCPldx_ExtremityVerticalConnPlate_DX            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLDY"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_DY           " ; NameVar = "FEET_ExVCPldy_ExtremityVerticalConnPlate_DY            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPZSEP"      ; DbVarComment="EXTREMITYMAINPLATE_ZSEP                 " ; NameVar = "FEET_ExMPzsep_ExtremityMainPlate_Zsep                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXSPRCRA"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATEROUNDCUT_RADIUS" ; NameVar = "FEET_ExSPRCra_ExtremitySlantedConnPlateRoundCut_Radius " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXSPRCOF"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATEROUNDCUT_OFFSET" ; NameVar = "FEET_ExSPRCof_ExtremitySlantedConnPlateRoundCut_Offset " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPTHI"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_THICKNESS    " ; NameVar = "FEET_ExVCPthi_ExtremityVerticalConnPlate_Thickness     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPXEX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XEXT         " ; NameVar = "FEET_ExVCPxex_ExtremityVerticalConnPlate_Xext          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPHEI"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_HEIGHT       " ; NameVar = "FEET_ExVCPhei_ExtremityVerticalConnPlate_Height        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPYCU"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_CUTY         " ; NameVar = "FEET_ExVCPycu_ExtremityVerticalConnPlate_CutY          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPXCU"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_CUTX         " ; NameVar = "FEET_ExVCPxcu_ExtremityVerticalConnPlate_CutX          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXCFVXWI"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_XWIDTH           " ; NameVar = "FEET_ExCFVxwi_ExtremityConnFeetVouss_Xwidth            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXCFVYHE"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_YHEIGHT          " ; NameVar = "FEET_ExCFVyhe_ExtremityConnFeetVouss_Yheight           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXCFVZLE"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_ZLENGTH          " ; NameVar = "FEET_ExCFVzle_ExtremityConnFeetVouss_Zlength           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSUZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTUPPER_ZLENGTH       " ; NameVar = "FEET_ExRSUzle_ExtremityRailSupportUpper_Zlength        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSLDZL"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_DZLENGTH      " ; NameVar = "FEET_ExRSLdzl_ExtremityRailSupportLower_DZlength       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSTYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTTOTAL_YHEIGHT       " ; NameVar = "FEET_ExRSTyhe_ExtremityRailSupportTotal_Yheight        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSLYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_YHEIGHT       " ; NameVar = "FEET_ExRSLyhe_ExtremityRailSupportLower_Yheight        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSEYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_YHEIGHT        " ; NameVar = "FEET_ExRSEyhe_ExtremityRailSupportExtr_Yheight         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSCYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_YHEIGHT     " ; NameVar = "FEET_ExRSCyhe_ExtremityRailSupportCentral_Yheight      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSCZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_ZLENGTH     " ; NameVar = "FEET_ExRSCzle_ExtremityRailSupportCentral_Zlength      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_ZLENGTH      " ; NameVar = "FEET_ExRSMzle_ExtremityRailSupportMiddle_Zlength       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSCXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_XWIDTH      " ; NameVar = "FEET_ExRSCxwi_ExtremityRailSupportCentral_Xwidth       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_XWIDTH       " ; NameVar = "FEET_ExRSMxwi_ExtremityRailSupportMiddle_Xwidth        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_XWIDTH     " ; NameVar = "FEET_ExRSVxwi_ExtremityRailSupportVertical_Xwidth      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVZI1"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_ZPOS_INTER1" ; NameVar = "FEET_ExRSVzi1_ExtremityRailSupportVertical_Zpos_inter1 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSEZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZLENGTH        " ; NameVar = "FEET_ExRSEzle_ExtremityRailSupportExtr_Zlength         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSEZOF"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZOFFSET        " ; NameVar = "FEET_ExRSEzof_ExtremityRailSupportExtr_Zoffset         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1DE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_DEPTH          " ; NameVar = "FEET_ExRSC1de_ExtremityRailSupportCut1_Depth           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1WI"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_WIDTH          " ; NameVar = "FEET_ExRSC1wi_ExtremityRailSupportCut1_Width           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1XE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_DUMXEXT        " ; NameVar = "FEET_ExRSC1xe_ExtremityRailSupportCut1_dumXext         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC2XE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_DUMXEXT        " ; NameVar = "FEET_ExRSC2xe_ExtremityRailSupportCut2_dumXext         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC2Z1"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZOFFSET1       " ; NameVar = "FEET_ExRSC2z1_ExtremityRailSupportCut1_Zoffset1        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC2Z2"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZOFFSET2       " ; NameVar = "FEET_ExRSC2z2_ExtremityRailSupportCut1_Zoffset2        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCDE"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DEPTH     " ; NameVar = "FEET_ExRSMCde_ExtremityRailSupportMiddleCut_Depth      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCWI"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_WIDTH     " ; NameVar = "FEET_ExRSMCwi_ExtremityRailSupportMiddleCut_Width      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCZO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_ZOFFSET   " ; NameVar = "FEET_ExRSMCzo_ExtremityRailSupportMiddleCut_Zoffset    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCY1"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DUMY1     " ; NameVar = "FEET_ExRSMCy1_ExtremityRailSupportMiddleCut_dumy1      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCY2"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DUMY2     " ; NameVar = "FEET_ExRSMCy2_ExtremityRailSupportMiddleCut_dumy2      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STDFOOXP"      ; DbVarComment="STANDARDFOOT_XPOS                       " ; NameVar = "FEET_StdFooXp_StandardFoot_Xpos                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STDFOOYP"      ; DbVarComment="STANDARDFOOT_YPOS                       " ; NameVar = "FEET_StdFooYp_StandardFoot_Ypos                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSFEE1"      ; DbVarComment="ZPOSFEE1                                " ; NameVar = "FEET_ZposFee1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSFEE2"      ; DbVarComment="ZPOSFEE2                                " ; NameVar = "FEET_ZposFee2                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSFEE3"      ; DbVarComment="ZPOSFEE3                                " ; NameVar = "FEET_ZposFee3                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSFEE4"      ; DbVarComment="ZPOSFEE4                                " ; NameVar = "FEET_ZposFee4                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSFEE5"      ; DbVarComment="ZPOSFEE5                                " ; NameVar = "FEET_ZposFee5                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIRDYHEI"      ; DbVarComment="YHEIGHTGIRDER                           " ; NameVar = "FEET_GirdYHei_YHeightGirder                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIRDXPOS"      ; DbVarComment="XPOSGIRDER                              " ; NameVar = "FEET_GirdXpos_XposGirder                               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLAHCEC"      ; DbVarComment="FEETMAINPLATE_HCEC                      " ; NameVar = "FEET_MPlaHcec_FeetMainPlate_Hcec                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "DUMZ"          ; DbVarComment="DUMZ                                    " ; NameVar = "FEET_dumz                                              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "DUMPHI"        ; DbVarComment="DUMPHI                                  " ; NameVar = "FEET_dumphi                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXA"      ; DbVarComment="MAINPLXA                                " ; NameVar = "FEET_MainPlXa                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYA"      ; DbVarComment="MAINPLYA                                " ; NameVar = "FEET_MainPlYa                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXB"      ; DbVarComment="MAINPLXB                                " ; NameVar = "FEET_MainPlXb                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYB"      ; DbVarComment="MAINPLYB                                " ; NameVar = "FEET_MainPlYb                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXC"      ; DbVarComment="MAINPLXC                                " ; NameVar = "FEET_MainPlXc                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYC"      ; DbVarComment="MAINPLYC                                " ; NameVar = "FEET_MainPlYc                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAPLRCXC"      ; DbVarComment="MAINPLATEROUNDCUT_XC                    " ; NameVar = "FEET_MaPlRCXc_MainPlateRoundCut_XC                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAPLRCYC"      ; DbVarComment="MAINPLATEROUNDCUT_YC                    " ; NameVar = "FEET_MaPlRCYc_MainPlateRoundCut_YC                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLXC3"      ; DbVarComment="MAIPLXC3                                " ; NameVar = "FEET_MaiPlXc3                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLYC3"      ; DbVarComment="MAIPLYC3                                " ; NameVar = "FEET_MaiPlYc3                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLXEXTR"      ; DbVarComment="FEETMAINPLATE_XEXTR                     " ; NameVar = "FEET_MPlXextr_FeetMainPlate_Xextr                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLYEXTR"      ; DbVarComment="FEETMAINPLATE_YEXTR                     " ; NameVar = "FEET_MPlYextr_FeetMainPlate_Yextr                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLREXTR"      ; DbVarComment="FEETMAINPLATE_REXTR                     " ; NameVar = "FEET_MPlRextr_FeetMainPlate_Rextr                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLXC2"      ; DbVarComment="MAIPLXC2                                " ; NameVar = "FEET_MaiPlXc2                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLYC2"      ; DbVarComment="MAIPLYC2                                " ; NameVar = "FEET_MaiPlYc2                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLXCEC1"      ; DbVarComment="MPLXCEC1                                " ; NameVar = "FEET_MPlXcec1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLYCEC1"      ; DbVarComment="MPLYCEC1                                " ; NameVar = "FEET_MPlYcec1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLXCEC2"      ; DbVarComment="MPLXCEC2                                " ; NameVar = "FEET_MPlXcec2                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MPLYCEC2"      ; DbVarComment="MPLYCEC2                                " ; NameVar = "FEET_MPlYcec2                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "A"             ; DbVarComment="A                                       " ; NameVar = "FEET_a                                                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "B"             ; DbVarComment="B                                       " ; NameVar = "FEET_b                                                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "APRIME"        ; DbVarComment="APRIME                                  " ; NameVar = "FEET_aprime                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BPRIME"        ; DbVarComment="BPRIME                                  " ; NameVar = "FEET_bprime                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ASECONDE"      ; DbVarComment="ASECONDE                                " ; NameVar = "FEET_aseconde                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BSECONDE"      ; DbVarComment="BSECONDE                                " ; NameVar = "FEET_bseconde                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXD"      ; DbVarComment="MAINPLXD                                " ; NameVar = "FEET_MainPlXd                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYD"      ; DbVarComment="MAINPLYD                                " ; NameVar = "FEET_MainPlYd                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXE"      ; DbVarComment="MAINPLXE                                " ; NameVar = "FEET_MainPlXe                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYE"      ; DbVarComment="MAINPLYE                                " ; NameVar = "FEET_MainPlYe                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLXC1"      ; DbVarComment="MAIPLXC1                                " ; NameVar = "FEET_MaiPlXc1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAIPLYC1"      ; DbVarComment="MAIPLYC1                                " ; NameVar = "FEET_MaiPlYc1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXF"      ; DbVarComment="MAINPLXF                                " ; NameVar = "FEET_MainPlXf                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYF"      ; DbVarComment="MAINPLYF                                " ; NameVar = "FEET_MainPlYf                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXG"      ; DbVarComment="MAINPLXG                                " ; NameVar = "FEET_MainPlXg                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYG"      ; DbVarComment="MAINPLYG                                " ; NameVar = "FEET_MainPlYg                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXH"      ; DbVarComment="MAINPLXH                                " ; NameVar = "FEET_MainPlXh                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYH"      ; DbVarComment="MAINPLYH                                " ; NameVar = "FEET_MainPlYh                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXI"      ; DbVarComment="MAINPLXI                                " ; NameVar = "FEET_MainPlXi                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYI"      ; DbVarComment="MAINPLYI                                " ; NameVar = "FEET_MainPlYi                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLXJ"      ; DbVarComment="MAINPLXJ                                " ; NameVar = "FEET_MainPlXj                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MAINPLYJ"      ; DbVarComment="MAINPLYJ                                " ; NameVar = "FEET_MainPlYj                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ALPHA1"        ; DbVarComment="ALPHA1                                  " ; NameVar = "FEET_Alpha1                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COAL1"         ; DbVarComment="COAL1                                   " ; NameVar = "FEET_CoAl1                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SIAL1"         ; DbVarComment="SIAL1                                   " ; NameVar = "FEET_SiAl1                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MICPDIAG"      ; DbVarComment="MINICONNPLATE_DIAG                      " ; NameVar = "FEET_MiCPDiag_MiniConnPlate_DIAG                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MICPXPOS"      ; DbVarComment="MINICONNPLATE_X                         " ; NameVar = "FEET_MiCPxpos_MiniConnPlate_X                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MICPYPOS"      ; DbVarComment="MINICONNPLATE_Y                         " ; NameVar = "FEET_MiCPypos_MiniConnPlate_Y                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ALPHA2"        ; DbVarComment="ALPHA2                                  " ; NameVar = "FEET_Alpha2                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COAL2"         ; DbVarComment="COAL2                                   " ; NameVar = "FEET_CoAl2                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SIAL2"         ; DbVarComment="SIAL2                                   " ; NameVar = "FEET_SiAl2                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLCPDIAG"      ; DbVarComment="SLANTEDCONNPLATE_DIAG                   " ; NameVar = "FEET_SlCPDiag_SlantedConnPlate_DIAG                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLCPXPOS"      ; DbVarComment="SLANTEDCONNPLATE_X                      " ; NameVar = "FEET_SlCPxpos_SlantedConnPlate_X                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SLCPYPOS"      ; DbVarComment="SLANTEDCONNPLATE_Y                      " ; NameVar = "FEET_SlCPypos_SlantedConnPlate_Y                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GRNPXWID"      ; DbVarComment="GROUNDPLATE_XWIDTH                      " ; NameVar = "FEET_GrnPxwid_GroundPlate_Xwidth                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GRNPYHEI"      ; DbVarComment="GROUNDPLATE_YHEIGHT                     " ; NameVar = "FEET_GrnPyhei_GroundPlate_Yheight                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GRNPXPOS"      ; DbVarComment="GROUNDPLATE_X                           " ; NameVar = "FEET_GrnPxpos_GroundPlate_X                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GRNPYPOS"      ; DbVarComment="GROUNDPLATE_Y                           " ; NameVar = "FEET_GrnPypos_GroundPlate_Y                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VECPXPOS"      ; DbVarComment="VERTICALCONNPLATE_X                     " ; NameVar = "FEET_VeCPxpos_VerticalConnPlate_X                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VECPYPOS"      ; DbVarComment="VERTICALCONNPLATE_Y                     " ; NameVar = "FEET_VeCPypos_VerticalConnPlate_Y                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "VECPZLEN"      ; DbVarComment="VERTICALCONNPLATE_ZLENGTH               " ; NameVar = "FEET_VeCPzlen_VerticalConnPlate_Zlength                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UPCPXPOS"      ; DbVarComment="UPPERCONNPLATE_X                        " ; NameVar = "FEET_UpCPxpos_UpperConnPlate_X                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UPCPYPOS"      ; DbVarComment="UPPERCONNPLATE_Y                        " ; NameVar = "FEET_UpCPypos_UpperConnPlate_Y                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "UPCPZLEN"      ; DbVarComment="UPPERCONNPLATE_ZLENGTH                  " ; NameVar = "FEET_UpCPzlen_UpperConnPlate_Zlength                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "MIDYPOSI"      ; DbVarComment="RAILSUPPORTMIDDLE_Y                     " ; NameVar = "FEET_Midyposi_RailSupportMiddle_Y                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "POSX"          ; DbVarComment="POSX                                    " ; NameVar = "FEET_posx                                              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "POSY"          ; DbVarComment="POSY                                    " ; NameVar = "FEET_posy                                              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOXP"      ; DbVarComment="CONNFEETVOUSS_X                         " ; NameVar = "FEET_CnFeVoXp_ConnFeetVouss_X                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CNFEVOYP"      ; DbVarComment="CONNFEETVOUSS_Y                         " ; NameVar = "FEET_CnFeVoYp_ConnFeetVouss_Y                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXA"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XA                 " ; NameVar = "FEET_G12BoPxa_Girder12_BottomPlate_XA                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXB"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XB                 " ; NameVar = "FEET_G12BoPxb_Girder12_BottomPlate_XB                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXC"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XC                 " ; NameVar = "FEET_G12BoPxc_Girder12_BottomPlate_XC                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXD"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XD                 " ; NameVar = "FEET_G12BoPxd_Girder12_BottomPlate_XD                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XE                 " ; NameVar = "FEET_G12BoPxe_Girder12_BottomPlate_XE                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPXF"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XF                 " ; NameVar = "FEET_G12BoPxf_Girder12_BottomPlate_XF                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYA"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YA                 " ; NameVar = "FEET_G12BoPya_Girder12_BottomPlate_YA                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYB"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YB                 " ; NameVar = "FEET_G12BoPyb_Girder12_BottomPlate_YB                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYC"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YC                 " ; NameVar = "FEET_G12BoPyc_Girder12_BottomPlate_YC                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYD"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YD                 " ; NameVar = "FEET_G12BoPyd_Girder12_BottomPlate_YD                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YE                 " ; NameVar = "FEET_G12BoPye_Girder12_BottomPlate_YE                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12BOPYF"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YF                 " ; NameVar = "FEET_G12BoPyf_Girder12_BottomPlate_YF                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPZLE"      ; DbVarComment="GIRDER12_INNERPLATE_ZLENGTH             " ; NameVar = "FEET_G12IPzle_Girder12_InnerPlate_Zlength              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLYH"      ; DbVarComment="GIRDER12_INNERPLATELARGE_YHEIGHT        " ; NameVar = "FEET_G12IPLyh_Girder12_InnerPlateLarge_Yheight         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLYP"      ; DbVarComment="GIRDER12_INNERPLATELARGE_YPOS           " ; NameVar = "FEET_G12IPLyp_Girder12_InnerPlateLarge_Ypos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSYH"      ; DbVarComment="GIRDER12_INNERPLATESMALL_YHEIGHT        " ; NameVar = "FEET_G12IPSyh_Girder12_InnerPlateSmall_Yheight         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSYP"      ; DbVarComment="GIRDER12_INNERPLATESMALL_YPOS           " ; NameVar = "FEET_G12IPSyp_Girder12_InnerPlateSmall_Ypos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSX1"      ; DbVarComment="GIRDER12_INNERPLATESMALL1_XPOS          " ; NameVar = "FEET_G12IPSx1_Girder12_InnerPlateSmall1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLX1"      ; DbVarComment="GIRDER12_INNERPLATELARGE1_XPOS          " ; NameVar = "FEET_G12IPLx1_Girder12_InnerPlateLarge1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPLX2"      ; DbVarComment="GIRDER12_INNERPLATELARGE2_XPOS          " ; NameVar = "FEET_G12IPLx2_Girder12_InnerPlateLarge2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12IPSX2"      ; DbVarComment="GIRDER12_INNERPLATESMALL2_XPOS          " ; NameVar = "FEET_G12IPSx2_Girder12_InnerPlateSmall2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UPXWI"      ; DbVarComment="GIRDER12_UPPERPLATE_XWIDTH              " ; NameVar = "FEET_G12UPxwi_Girder12_UpperPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UPYHE"      ; DbVarComment="GIRDER12_UPPERPLATE_YHEIGHT             " ; NameVar = "FEET_G12UPyhe_Girder12_UpperPlate_Yheight              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UPYPO"      ; DbVarComment="GIRDER12_UPPERPLATE_YPOS                " ; NameVar = "FEET_G12UPypo_Girder12_UpperPlate_Ypos                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPXA"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XA          " ; NameVar = "FEET_G12RePxa_Girder12_ReinforcementPlate_XA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPXB"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XB          " ; NameVar = "FEET_G12RePxb_Girder12_ReinforcementPlate_XB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPXC"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XC          " ; NameVar = "FEET_G12RePxc_Girder12_ReinforcementPlate_XC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPXD"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XD          " ; NameVar = "FEET_G12RePxd_Girder12_ReinforcementPlate_XD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPYA"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YA          " ; NameVar = "FEET_G12RePya_Girder12_ReinforcementPlate_YA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPYB"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YB          " ; NameVar = "FEET_G12RePyb_Girder12_ReinforcementPlate_YB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPYC"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YC          " ; NameVar = "FEET_G12RePyc_Girder12_ReinforcementPlate_YC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPYD"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YD          " ; NameVar = "FEET_G12RePyd_Girder12_ReinforcementPlate_YD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12REPZP"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_ZPOS        " ; NameVar = "FEET_G12RePzp_Girder12_ReinforcementPlate_Zpos         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UMPYP"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_YPOS            " ; NameVar = "FEET_G12UMPyp_Girder12_UpperMiniPlate_Ypos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12UMPXP"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_XPOS            " ; NameVar = "FEET_G12UMPxp_Girder12_UpperMiniPlate_Xpos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPYH"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_YHEIGHT          " ; NameVar = "FEET_G12SMPyh_Girder12_SideMiniPlate_Yheight           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPYP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_YPOS             " ; NameVar = "FEET_G12SMPyp_Girder12_SideMiniPlate_Ypos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPXP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_XPOS             " ; NameVar = "FEET_G12SMPxp_Girder12_SideMiniPlate_Xpos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G12SMPZP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_ZPOS             " ; NameVar = "FEET_G12SMPzp_Girder12_SideMiniPlate_Zpos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXA"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XA                 " ; NameVar = "FEET_G23BoPxa_Girder23_BottomPlate_XA                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXB"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XB                 " ; NameVar = "FEET_G23BoPxb_Girder23_BottomPlate_XB                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXC"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XC                 " ; NameVar = "FEET_G23BoPxc_Girder23_BottomPlate_XC                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXD"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XD                 " ; NameVar = "FEET_G23BoPxd_Girder23_BottomPlate_XD                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXE"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XE                 " ; NameVar = "FEET_G23BoPxe_Girder23_BottomPlate_XE                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23BOPXF"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XF                 " ; NameVar = "FEET_G23BoPxf_Girder23_BottomPlate_XF                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPSX1"      ; DbVarComment="GIRDER23_INNERPLATESMALL1_XPOS          " ; NameVar = "FEET_G23IPSx1_Girder23_InnerPlateSmall1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPLX1"      ; DbVarComment="GIRDER23_INNERPLATELARGE1_XPOS          " ; NameVar = "FEET_G23IPLx1_Girder23_InnerPlateLarge1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPLX2"      ; DbVarComment="GIRDER23_INNERPLATELARGE2_XPOS          " ; NameVar = "FEET_G23IPLx2_Girder23_InnerPlateLarge2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23IPSX2"      ; DbVarComment="GIRDER23_INNERPLATESMALL2_XPOS          " ; NameVar = "FEET_G23IPSx2_Girder23_InnerPlateSmall2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23UPXWI"      ; DbVarComment="GIRDER23_UPPERPLATE_XWIDTH              " ; NameVar = "FEET_G23UPxwi_Girder23_UpperPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23UPYHE"      ; DbVarComment="GIRDER23_UPPERPLATE_YHEIGHT             " ; NameVar = "FEET_G23UPyhe_Girder23_UpperPlate_Yheight              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPXA"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XA          " ; NameVar = "FEET_G23RePxa_Girder23_ReinforcementPlate_XA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPXB"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XB          " ; NameVar = "FEET_G23RePxb_Girder23_ReinforcementPlate_XB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPXC"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XC          " ; NameVar = "FEET_G23RePxc_Girder23_ReinforcementPlate_XC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPXD"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XD          " ; NameVar = "FEET_G23RePxd_Girder23_ReinforcementPlate_XD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPYA"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YA          " ; NameVar = "FEET_G23RePya_Girder23_ReinforcementPlate_YA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPYB"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YB          " ; NameVar = "FEET_G23RePyb_Girder23_ReinforcementPlate_YB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPYC"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YC          " ; NameVar = "FEET_G23RePyc_Girder23_ReinforcementPlate_YC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23REPYD"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YD          " ; NameVar = "FEET_G23RePyd_Girder23_ReinforcementPlate_YD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23UMPXP"      ; DbVarComment="GIRDER23_UPPERMINIPLATE_XPOS            " ; NameVar = "FEET_G23UMPxp_Girder23_UpperMiniPlate_Xpos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G23SMPXP"      ; DbVarComment="GIRDER23_SIDEMINIPLATE_XPOS             " ; NameVar = "FEET_G23SMPxp_Girder23_SideMiniPlate_Xpos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXA"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XA                 " ; NameVar = "FEET_G34BoPxa_Girder34_BottomPlate_XA                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXB"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XB                 " ; NameVar = "FEET_G34BoPxb_Girder34_BottomPlate_XB                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXC"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XC                 " ; NameVar = "FEET_G34BoPxc_Girder34_BottomPlate_XC                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXD"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XD                 " ; NameVar = "FEET_G34BoPxd_Girder34_BottomPlate_XD                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXE"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XE                 " ; NameVar = "FEET_G34BoPxe_Girder34_BottomPlate_XE                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34BOPXF"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XF                 " ; NameVar = "FEET_G34BoPxf_Girder34_BottomPlate_XF                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPSX1"      ; DbVarComment="GIRDER34_INNERPLATESMALL1_XPOS          " ; NameVar = "FEET_G34IPSx1_Girder34_InnerPlateSmall1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPLX1"      ; DbVarComment="GIRDER34_INNERPLATELARGE1_XPOS          " ; NameVar = "FEET_G34IPLx1_Girder34_InnerPlateLarge1_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPLX2"      ; DbVarComment="GIRDER34_INNERPLATELARGE2_XPOS          " ; NameVar = "FEET_G34IPLx2_Girder34_InnerPlateLarge2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34IPSX2"      ; DbVarComment="GIRDER34_INNERPLATESMALL2_XPOS          " ; NameVar = "FEET_G34IPSx2_Girder34_InnerPlateSmall2_Xpos           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34UPXWI"      ; DbVarComment="GIRDER34_UPPERPLATE_XWIDTH              " ; NameVar = "FEET_G34UPxwi_Girder34_UpperPlate_Xwidth               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34UPYHE"      ; DbVarComment="GIRDER34_UPPERPLATE_YHEIGHT             " ; NameVar = "FEET_G34UPyhe_Girder34_UpperPlate_Yheight              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPXA"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XA          " ; NameVar = "FEET_G34RePxa_Girder34_ReinforcementPlate_XA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPXB"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XB          " ; NameVar = "FEET_G34RePxb_Girder34_ReinforcementPlate_XB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPXC"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XC          " ; NameVar = "FEET_G34RePxc_Girder34_ReinforcementPlate_XC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPXD"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XD          " ; NameVar = "FEET_G34RePxd_Girder34_ReinforcementPlate_XD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPYA"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YA          " ; NameVar = "FEET_G34RePya_Girder34_ReinforcementPlate_YA           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPYB"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YB          " ; NameVar = "FEET_G34RePyb_Girder34_ReinforcementPlate_YB           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPYC"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YC          " ; NameVar = "FEET_G34RePyc_Girder34_ReinforcementPlate_YC           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34REPYD"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YD          " ; NameVar = "FEET_G34RePyd_Girder34_ReinforcementPlate_YD           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34UMPXP"      ; DbVarComment="GIRDER34_UPPERMINIPLATE_XPOS            " ; NameVar = "FEET_G34UMPxp_Girder34_UpperMiniPlate_Xpos             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "G34SMPXP"      ; DbVarComment="GIRDER34_SIDEMINIPLATE_XPOS             " ; NameVar = "FEET_G34SMPxp_Girder34_SideMiniPlate_Xpos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "STDFVOYP"      ; DbVarComment="STANDARDFOOTVOUSSOIR_YPOS               " ; NameVar = "FEET_StdFVoYp_StandardFootVoussoir_Ypos                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPRCXC"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_XC           " ; NameVar = "FEET_ExMPRCxc_ExtremityMainPlateRoundCut_XC            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPRCYC"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_YC           " ; NameVar = "FEET_ExMPRCyc_ExtremityMainPlateRoundCut_YC            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXC3"      ; DbVarComment="EXTREMITYMAINPLATE_XC3                  " ; NameVar = "FEET_ExMPlxc3_ExtremityMainPlate_XC3                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYC3"      ; DbVarComment="EXTREMITYMAINPLATE_YC3                  " ; NameVar = "FEET_ExMPlyc3_ExtremityMainPlate_YC3                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXC4"      ; DbVarComment="EXTREMITYMAINPLATE_XC4                  " ; NameVar = "FEET_ExMPlxc4_ExtremityMainPlate_XC4                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYC4"      ; DbVarComment="EXTREMITYMAINPLATE_YC4                  " ; NameVar = "FEET_ExMPlyc4_ExtremityMainPlate_YC4                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXC5"      ; DbVarComment="EXTREMITYMAINPLATE_XC5                  " ; NameVar = "FEET_ExMPlxc5_ExtremityMainPlate_XC5                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYC5"      ; DbVarComment="EXTREMITYMAINPLATE_YC5                  " ; NameVar = "FEET_ExMPlyc5_ExtremityMainPlate_YC5                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "A2"            ; DbVarComment="A2                                      " ; NameVar = "FEET_a2                                                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "B2"            ; DbVarComment="B2                                      " ; NameVar = "FEET_b2                                                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "A2PRIME"       ; DbVarComment="A2PRIME                                 " ; NameVar = "FEET_a2prime                                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "B2PRIME"       ; DbVarComment="B2PRIME                                 " ; NameVar = "FEET_b2prime                                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAXD"      ; DbVarComment="EXTREMITYMAINPLATE_XD                   " ; NameVar = "FEET_ExMPlaxd_ExtremityMainPlate_XD                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAYD"      ; DbVarComment="EXTREMITYMAINPLATE_YD                   " ; NameVar = "FEET_ExMPlayd_ExtremityMainPlate_YD                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPC2C1"      ; DbVarComment="EXTREMITYMAINPLATE_LENGTHC2C1           " ; NameVar = "FEET_ExMPc2c1_ExtremityMainPlate_LengthC2C1            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXC1"      ; DbVarComment="EXTREMITYMAINPLATE_XC1                  " ; NameVar = "FEET_ExMPlxc1_ExtremityMainPlate_XC1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYC1"      ; DbVarComment="EXTREMITYMAINPLATE_YC1                  " ; NameVar = "FEET_ExMPlyc1_ExtremityMainPlate_YC1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYC1A"      ; DbVarComment="EXTREMITYMAINPLATE_YC1A                 " ; NameVar = "FEET_ExMPyc1a_ExtremityMainPlate_YC1a                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXC1A"      ; DbVarComment="EXTREMITYMAINPLATE_XC1A                 " ; NameVar = "FEET_ExMPxc1a_ExtremityMainPlate_XC1a                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPXC1B"      ; DbVarComment="EXTREMITYMAINPLATE_XC1B                 " ; NameVar = "FEET_ExMPxc1b_ExtremityMainPlate_XC1b                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPYC1B"      ; DbVarComment="EXTREMITYMAINPLATE_YC1B                 " ; NameVar = "FEET_ExMPyc1b_ExtremityMainPlate_YC1b                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ALPHA3"        ; DbVarComment="ALPHA3                                  " ; NameVar = "FEET_Alpha3                                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "COAL3"         ; DbVarComment="COAL3                                   " ; NameVar = "FEET_CoAl3                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SIAL3"         ; DbVarComment="SIAL3                                   " ; NameVar = "FEET_SiAl3                                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMCPDIA"      ; DbVarComment="EXTREMITYMINICONNPLATE_DIAG             " ; NameVar = "FEET_ExMCPDia_ExtremityMiniConnPlate_DIAG              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMCPXPO"      ; DbVarComment="EXTREMITYMINICONNPLATE_X                " ; NameVar = "FEET_ExMCPxpo_ExtremityMiniConnPlate_X                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMCPYPO"      ; DbVarComment="EXTREMITYMINICONNPLATE_Y                " ; NameVar = "FEET_ExMCPypo_ExtremityMiniConnPlate_Y                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAXF"      ; DbVarComment="EXTREMITYMAINPLATE_XF                   " ; NameVar = "FEET_ExMPlaxf_ExtremityMainPlate_XF                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAYF"      ; DbVarComment="EXTREMITYMAINPLATE_YF                   " ; NameVar = "FEET_ExMPlayf_ExtremityMainPlate_YF                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF1"      ; DbVarComment="EXTREMITYMAINPLATE_XF1                  " ; NameVar = "FEET_ExMPlxf1_ExtremityMainPlate_XF1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF1"      ; DbVarComment="EXTREMITYMAINPLATE_YF1                  " ; NameVar = "FEET_ExMPlyf1_ExtremityMainPlate_YF1                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF2"      ; DbVarComment="EXTREMITYMAINPLATE_XF2                  " ; NameVar = "FEET_ExMPlxf2_ExtremityMainPlate_XF2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF2"      ; DbVarComment="EXTREMITYMAINPLATE_YF2                  " ; NameVar = "FEET_ExMPlyf2_ExtremityMainPlate_YF2                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF3"      ; DbVarComment="EXTREMITYMAINPLATE_YF3                  " ; NameVar = "FEET_ExMPlyf3_ExtremityMainPlate_YF3                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF3"      ; DbVarComment="EXTREMITYMAINPLATE_XF3                  " ; NameVar = "FEET_ExMPlxf3_ExtremityMainPlate_XF3                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF4"      ; DbVarComment="EXTREMITYMAINPLATE_XF4                  " ; NameVar = "FEET_ExMPlxf4_ExtremityMainPlate_XF4                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF4"      ; DbVarComment="EXTREMITYMAINPLATE_YF4                  " ; NameVar = "FEET_ExMPlyf4_ExtremityMainPlate_YF4                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF5"      ; DbVarComment="EXTREMITYMAINPLATE_XF5                  " ; NameVar = "FEET_ExMPlxf5_ExtremityMainPlate_XF5                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF5"      ; DbVarComment="EXTREMITYMAINPLATE_YF5                  " ; NameVar = "FEET_ExMPlyf5_ExtremityMainPlate_YF5                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF6"      ; DbVarComment="EXTREMITYMAINPLATE_XF6                  " ; NameVar = "FEET_ExMPlxf6_ExtremityMainPlate_XF6                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF6"      ; DbVarComment="EXTREMITYMAINPLATE_YF6                  " ; NameVar = "FEET_ExMPlyf6_ExtremityMainPlate_YF6                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLXF7"      ; DbVarComment="EXTREMITYMAINPLATE_XF7                  " ; NameVar = "FEET_ExMPlxf7_ExtremityMainPlate_XF7                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLYF7"      ; DbVarComment="EXTREMITYMAINPLATE_YF7                  " ; NameVar = "FEET_ExMPlyf7_ExtremityMainPlate_YF7                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAXI"      ; DbVarComment="EXTREMITYMAINPLATE_XI                   " ; NameVar = "FEET_ExMPlaxi_ExtremityMainPlate_XI                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAXH"      ; DbVarComment="EXTREMITYMAINPLATE_XH                   " ; NameVar = "FEET_ExMPlaxh_ExtremityMainPlate_XH                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXMPLAYH"      ; DbVarComment="EXTREMITYMAINPLATE_YH                   " ; NameVar = "FEET_ExMPlayh_ExtremityMainPlate_YH                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXUCPXPO"      ; DbVarComment="EXTREMITYUPPERCONNPLATE_X               " ; NameVar = "FEET_ExUCPxpo_ExtremityUpperConnPlate_X                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXUCPZLE"      ; DbVarComment="EXTREMITYUPPERCONNPLATE_ZLENGTH         " ; NameVar = "FEET_ExUCPzle_ExtremityUpperConnPlate_Zlength          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPZLE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_ZLENGTH      " ; NameVar = "FEET_ExVCPzle_ExtremityVerticalConnPlate_Zlength       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXA"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XA           " ; NameVar = "FEET_ExVCPlxa_ExtremityVerticalConnPlate_XA            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYA"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YA           " ; NameVar = "FEET_ExVCPlya_ExtremityVerticalConnPlate_YA            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXB"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XB           " ; NameVar = "FEET_ExVCPlxb_ExtremityVerticalConnPlate_XB            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYB"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YB           " ; NameVar = "FEET_ExVCPlyb_ExtremityVerticalConnPlate_YB            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXC"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XC           " ; NameVar = "FEET_ExVCPlxc_ExtremityVerticalConnPlate_XC            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYC"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YC           " ; NameVar = "FEET_ExVCPlyc_ExtremityVerticalConnPlate_YC            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXD"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XD           " ; NameVar = "FEET_ExVCPlxd_ExtremityVerticalConnPlate_XD            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYD"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YD           " ; NameVar = "FEET_ExVCPlyd_ExtremityVerticalConnPlate_YD            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XE           " ; NameVar = "FEET_ExVCPlxe_ExtremityVerticalConnPlate_XE            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YE           " ; NameVar = "FEET_ExVCPlye_ExtremityVerticalConnPlate_YE            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXF"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XF           " ; NameVar = "FEET_ExVCPlxf_ExtremityVerticalConnPlate_XF            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYF"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YF           " ; NameVar = "FEET_ExVCPlyf_ExtremityVerticalConnPlate_YF            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLXP"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XPOS         " ; NameVar = "FEET_ExVCPlxp_ExtremityVerticalConnPlate_Xpos          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLYP"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YPOS         " ; NameVar = "FEET_ExVCPlyp_ExtremityVerticalConnPlate_Ypos          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLAX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_X            " ; NameVar = "FEET_ExVCPlax_ExtremityVerticalConnPlate_X             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXVCPLAY"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_Y            " ; NameVar = "FEET_ExVCPlay_ExtremityVerticalConnPlate_Y             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXCNFEVX"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_X                " ; NameVar = "FEET_ExCnFeVx_ExtremityConnFeetVouss_X                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXCNFEVY"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_Y                " ; NameVar = "FEET_ExCnFeVy_ExtremityConnFeetVouss_Y                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSLZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_ZLENGTH       " ; NameVar = "FEET_ExRSLzle_ExtremityRailSupportLower_Zlength        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_YHEIGHT    " ; NameVar = "FEET_ExRSVyhe_ExtremityRailSupportVertical_Yheight     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMXPO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_XPOS         " ; NameVar = "FEET_ExRSMxpo_ExtremityRailSupportMiddle_Xpos          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVXPO"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_XPOS       " ; NameVar = "FEET_ExRSVxpo_ExtremityRailSupportVertical_Xpos        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSEYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_Y              " ; NameVar = "FEET_ExRSEypo_ExtremityRailSupportExtr_Y               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSUYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTUPPER_Y             " ; NameVar = "FEET_ExRSUypo_ExtremityRailSupportUpper_Y              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSLYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_Y             " ; NameVar = "FEET_ExRSLypo_ExtremityRailSupportLower_Y              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSCYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_Y           " ; NameVar = "FEET_ExRSCypo_ExtremityRailSupportCentral_Y            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_Y          " ; NameVar = "FEET_ExRSVypo_ExtremityRailSupportVertical_Y           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVZP1"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL1_ZPOS      " ; NameVar = "FEET_ExRSVzp1_ExtremityRailSupportVertical1_Zpos       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSVZP2"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL2_ZPOS      " ; NameVar = "FEET_ExRSVzp2_ExtremityRailSupportVertical2_Zpos       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMZPO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_ZPOS         " ; NameVar = "FEET_ExRSMzpo_ExtremityRailSupportMiddle_Zpos          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSEZPO"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZPOS           " ; NameVar = "FEET_ExRSEzpo_ExtremityRailSupportExtr_Zpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1YE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_YEXT           " ; NameVar = "FEET_ExRSC1ye_ExtremityRailSupportCut1_Yext            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC2YE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_YEXT           " ; NameVar = "FEET_ExRSC2ye_ExtremityRailSupportCut2_Yext            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_XPOS           " ; NameVar = "FEET_ExRSC1xp_ExtremityRailSupportCut1_Xpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC2XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_XPOS           " ; NameVar = "FEET_ExRSC2xp_ExtremityRailSupportCut2_Xpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC1ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZPOS           " ; NameVar = "FEET_ExRSC1zp_ExtremityRailSupportCut1_Zpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC3XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT3_XPOS           " ; NameVar = "FEET_ExRSC3xp_ExtremityRailSupportCut3_Xpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC3ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT3_ZPOS           " ; NameVar = "FEET_ExRSC3zp_ExtremityRailSupportCut3_Zpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC4Z1"      ; DbVarComment="EXRSC4Z1                                " ; NameVar = "FEET_ExRSC4z1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSC4ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT4_ZPOS           " ; NameVar = "FEET_ExRSC4zp_ExtremityRailSupportCut4_Zpos            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCXA"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XA        " ; NameVar = "FEET_ExRSMCxa_ExtremityRailSupportMiddleCut_XA         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCYA"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YA        " ; NameVar = "FEET_ExRSMCya_ExtremityRailSupportMiddleCut_YA         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCXB"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XB        " ; NameVar = "FEET_ExRSMCxb_ExtremityRailSupportMiddleCut_XB         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCYB"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YB        " ; NameVar = "FEET_ExRSMCyb_ExtremityRailSupportMiddleCut_YB         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCXC"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XC        " ; NameVar = "FEET_ExRSMCxc_ExtremityRailSupportMiddleCut_XC         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCYC"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YC        " ; NameVar = "FEET_ExRSMCyc_ExtremityRailSupportMiddleCut_YC         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCXP"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_CUT_XPOS     " ; NameVar = "FEET_ExRSMCxp_ExtremityRailSupportMiddle_cut_Xpos      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCZ1"      ; DbVarComment="EXRSMCZ1                                " ; NameVar = "FEET_ExRSMCz1                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSMCZP"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_CUT_ZPOS     " ; NameVar = "FEET_ExRSMCzp_ExtremityRailSupportMiddle_cut_Zpos      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPOSX"      ; DbVarComment="EXTREMITYRAILSUPPORT_X                  " ; NameVar = "FEET_ExRSposx_ExtremityRailSupport_X                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPOSY"      ; DbVarComment="EXTREMITYRAILSUPPORT_Y                  " ; NameVar = "FEET_ExRSposy_ExtremityRailSupport_Y                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPOSZ"      ; DbVarComment="EXTREMITYRAILSUPPORT_Z                  " ; NameVar = "FEET_ExRSposz_ExtremityRailSupport_Z                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPXAP"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_PLUS_X   " ; NameVar = "FEET_ExRSpxap_ExtremityRailSupport_assembled_plus_X    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPXAM"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_MINUS_X  " ; NameVar = "FEET_ExRSpxam_ExtremityRailSupport_assembled_minus_X   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EXRSPYAS"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_Y        " ; NameVar = "FEET_ExRSpyas_ExtremityRailSupport_assembled_Y         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIR12ZPO"      ; DbVarComment="GIR12ZPO                                " ; NameVar = "FEET_Gir12zpo                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIR12YPO"      ; DbVarComment="GIR12YPO                                " ; NameVar = "FEET_Gir12ypo                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIR23ZPO"      ; DbVarComment="GIR23ZPO                                " ; NameVar = "FEET_Gir23zpo                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "GIR34ZPO"      ; DbVarComment="GIR34ZPO                                " ; NameVar = "FEET_Gir34zpo                                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::GENV(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "GENV";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "PI"          ; DbVarComment="PI     " ; NameVar = "GENV_Pi    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "EPS"         ; DbVarComment="EPS    " ; NameVar = "GENV_Eps   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PIS180"      ; DbVarComment="PIS180 " ; NameVar = "GENV_PiS180" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SI5"         ; DbVarComment="SI5    " ; NameVar = "GENV_Si5   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CO5"         ; DbVarComment="CO5    " ; NameVar = "GENV_Co5   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA15"        ; DbVarComment="TA15   " ; NameVar = "GENV_Ta15  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA20"        ; DbVarComment="TA20   " ; NameVar = "GENV_Ta20  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SI225"       ; DbVarComment="SI225  " ; NameVar = "GENV_Si225 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CO225"       ; DbVarComment="CO225  " ; NameVar = "GENV_Co225 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA225"       ; DbVarComment="TA225  " ; NameVar = "GENV_Ta225 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA375"       ; DbVarComment="TA375  " ; NameVar = "GENV_Ta375 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SI45"        ; DbVarComment="SI45   " ; NameVar = "GENV_Si45  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CO45"        ; DbVarComment="CO45   " ; NameVar = "GENV_Co45  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA45"        ; DbVarComment="TA45   " ; NameVar = "GENV_Ta45  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SI525"       ; DbVarComment="SI525  " ; NameVar = "GENV_Si525 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CO525"       ; DbVarComment="CO525  " ; NameVar = "GENV_Co525 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA55"        ; DbVarComment="TA55   " ; NameVar = "GENV_Ta55  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SI675"       ; DbVarComment="SI675  " ; NameVar = "GENV_Si675 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CO675"       ; DbVarComment="CO675  " ; NameVar = "GENV_Co675 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA675"       ; DbVarComment="TA675  " ; NameVar = "GENV_Ta675 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TA77"        ; DbVarComment="TA77   " ; NameVar = "GENV_Ta77  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::JDSH(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JDSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "XYZREFPT"           ; DbVarComment="XYZREFERENCEPOINT         " ; NameVar = "JDSH_XYZrefpt_XYZReferencePoint         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ROTA180D"           ; DbVarComment="180DEGROTATION            " ; NameVar = "JDSH_rota180d_180degRotation            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "NOMIZPOS"           ; DbVarComment="NOMINAL_ZPOS              " ; NameVar = "JDSH_NomiZpos_Nominal_zpos              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "FDINNRAD"           ; DbVarComment="FRONTDISK_INNERRADIUS     " ; NameVar = "JDSH_FDinnRad_FrontDisk_innerRadius     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "FDOUTRAD"           ; DbVarComment="FRONTDISK_OUTERRADIUS     " ; NameVar = "JDSH_FDoutRad_FrontDisk_outerRadius     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "FDTHICKN"           ; DbVarComment="FRONTDISK_THICKNESS       " ; NameVar = "JDSH_FDthickn_FrontDisk_thickness       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BDINNRAD"           ; DbVarComment="BACKDISK_INNERRADIUS      " ; NameVar = "JDSH_BDinnRad_BackDisk_innerRadius      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BDOUTRAD"           ; DbVarComment="BACKDISK_OUTERRADIUS      " ; NameVar = "JDSH_BDoutRad_BackDisk_outerRadius      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BDTHICKN"           ; DbVarComment="BACKDISK_THICKNESS        " ; NameVar = "JDSH_BDthickn_BackDisk_thickness        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCINRA"           ; DbVarComment="BRASSCONE_INNERRADIUS     " ; NameVar = "JDSH_CABCinRa_BrassCone_innerRadius     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCOURA"           ; DbVarComment="BRASSCONE_OUTERRADIUS     " ; NameVar = "JDSH_CABCouRa_BrassCone_outerRadius     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCTHI1"           ; DbVarComment="BRASSCONE_THICKNESS_INTER1" ; NameVar = "JDSH_CABCthi1_BrassCone_thickness_inter1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCTHIC"           ; DbVarComment="BRASSCONE_THICKNESS       " ; NameVar = "JDSH_CABCthic_BrassCone_thickness       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCSLAN"           ; DbVarComment="BRASSCONE_SLOPEANGLE      " ; NameVar = "JDSH_CABCslAn_BrassCone_slopeAngle      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCSLLE"           ; DbVarComment="BRASSCONE_SLOPELENGTH     " ; NameVar = "JDSH_CABCslLe_BrassCone_slopeLength     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CALCTHIC"           ; DbVarComment="LEADCONE_THICKNESS        " ; NameVar = "JDSH_CALCthic_LeadCone_thickness        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CALCLENG"           ; DbVarComment="LEADCONE_LENGTH           " ; NameVar = "JDSH_CALCleng_LeadCone_length           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CALCWIDT"           ; DbVarComment="LEADCONE_WIDTH            " ; NameVar = "JDSH_CALCwidt_LeadCone_width            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAPCTHIC"           ; DbVarComment="POLYCONE_THICKNESS        " ; NameVar = "JDSH_CAPCthic_PolyCone_thickness        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAPCLENG"           ; DbVarComment="POLYCONE_LENGTH           " ; NameVar = "JDSH_CAPCleng_PolyCone_length           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAPCWIDT"           ; DbVarComment="POLYCONE_WIDTH            " ; NameVar = "JDSH_CAPCwidt_PolyCone_width            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOPRAN"           ; DbVarComment="CONE_PROJECTIVEANGLE      " ; NameVar = "JDSH_CACOprAn_Cone_projectiveAngle      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_0"         ; DbVarComment="ARRAY                     " ; NameVar = "JDSH_CACOangz%0                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_1"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%1                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_2"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%2                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_3"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%3                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_4"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%4                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_5"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%5                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_6"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%6                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_7"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%7                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_8"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%8                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_9"         ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%9                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_10"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%10                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_11"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%11                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_12"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%12                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_13"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%13                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_14"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%14                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGZ_15"        ; DbVarComment="                          " ; NameVar = "JDSH_CACOangz%15                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CACOANGX"           ; DbVarComment="CONEASSEMBLY_ANGX         " ; NameVar = "JDSH_CACOangx_ConeAssembly_angx         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBEINRA"           ; DbVarComment="TUBE_INNERRADIUS          " ; NameVar = "JDSH_TUBEinRa_Tube_innerRadius          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBEOUR1"           ; DbVarComment="TUBE_OUTERRADIUS1         " ; NameVar = "JDSH_TUBEOuR1_Tube_OuterRadius1         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBEOUR2"           ; DbVarComment="TUBE_OUTERRADIUS2         " ; NameVar = "JDSH_TUBEOuR2_Tube_OuterRadius2         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBESLEN"           ; DbVarComment="TUBE_SHORTLENGTH          " ; NameVar = "JDSH_TUBEslen_Tube_shortlength          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBELENG"           ; DbVarComment="TUBE_LENGTH               " ; NameVar = "JDSH_TUBEleng_Tube_length               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGINR1"           ; DbVarComment="PLUG_INNERRADIUS1         " ; NameVar = "JDSH_PLUGinR1_Plug_innerRadius1         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGOURA"           ; DbVarComment="PLUG_OUTERRADIUS          " ; NameVar = "JDSH_PLUGouRa_Plug_OuterRadius          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGLENG"           ; DbVarComment="PLUG_LENGTH               " ; NameVar = "JDSH_PLUGleng_Plug_length               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGTIAN"           ; DbVarComment="PLUG_TILTANGLE            " ; NameVar = "JDSH_PLUGtiAn_Plug_tiltAngle            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SWHINNRA"           ; DbVarComment="SMALLWHEELHUB_INNERRADIUS " ; NameVar = "JDSH_SWHinnRa_SmallWheelHub_innerRadius " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SWHOUTR1"           ; DbVarComment="SMALLWHEELHUB_OUTERRADIUS1" ; NameVar = "JDSH_SWHoutR1_SmallWheelHub_outerRadius1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SWHOUTR2"           ; DbVarComment="SMALLWHEELHUB_OUTERRADIUS2" ; NameVar = "JDSH_SWHoutR2_SmallWheelHub_outerRadius2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SWHLENGT"           ; DbVarComment="SMALLWHEELHUB_LENGTH      " ; NameVar = "JDSH_SWHlengt_SmallWheelHub_length      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "FDZPOSIT"           ; DbVarComment="FRONTDISK_Z               " ; NameVar = "JDSH_FDzposit_FrontDisk_Z               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "BDZPOSIT"           ; DbVarComment="BACKDISK_Z                " ; NameVar = "JDSH_BDzposit_BackDisk_Z                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAZPOSIT"           ; DbVarComment="CONEASSEMBLY_Z            " ; NameVar = "JDSH_CAzposit_ConeAssembly_Z            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TUBEZPOS"           ; DbVarComment="TUBE_Z                    " ; NameVar = "JDSH_TUBEzpos_Tube_Z                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGZPOS"           ; DbVarComment="PLUG_Z                    " ; NameVar = "JDSH_PLUGzpos_Plug_Z                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "SWHZPOSI"           ; DbVarComment="SMALLWHEELHUB_Z           " ; NameVar = "JDSH_SWHzposi_SmallWheelHub_Z           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCOUR1"           ; DbVarComment="BRASSCONE_OUTERRADIUS1    " ; NameVar = "JDSH_CABCouR1_BrassCone_outerRadius1    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCTHI2"           ; DbVarComment="BRASSCONE_THICKNESS2      " ; NameVar = "JDSH_CABCthi2_BrassCone_thickness2      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CABCOUR2"           ; DbVarComment="BRASSCONE_OUTERRADIUS2    " ; NameVar = "JDSH_CABCouR2_BrassCone_outerRadius2    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAXBPOS"           ; DbVarComment="LEADCONEASSEMBLY_XB       " ; NameVar = "JDSH_LCAxbpos_LeadConeAssembly_XB       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAYBPOS"           ; DbVarComment="LEADCONEASSEMBLY_YB       " ; NameVar = "JDSH_LCAybpos_LeadConeAssembly_YB       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAXAPOS"           ; DbVarComment="LEADCONEASSEMBLY_XA       " ; NameVar = "JDSH_LCAxapos_LeadConeAssembly_XA       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAYAPOS"           ; DbVarComment="LEADCONEASSEMBLY_YA       " ; NameVar = "JDSH_LCAyapos_LeadConeAssembly_YA       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAXBPOS"           ; DbVarComment="POLYCONEASSEMBLY_XB       " ; NameVar = "JDSH_PCAxbpos_PolyConeAssembly_XB       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAYBPOS"           ; DbVarComment="POLYCONEASSEMBLY_YB       " ; NameVar = "JDSH_PCAybpos_PolyConeAssembly_YB       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAXAPOS"           ; DbVarComment="POLYCONEASSEMBLY_XA       " ; NameVar = "JDSH_PCAxapos_PolyConeAssembly_XA       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAYAPOS"           ; DbVarComment="POLYCONEASSEMBLY_YA       " ; NameVar = "JDSH_PCAyapos_PolyConeAssembly_YA       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CALCRADI"           ; DbVarComment="LEADCONE_RADIUS           " ; NameVar = "JDSH_CALCradi_LeadCone_Radius           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CALCPOSZ"           ; DbVarComment="LEADCONE_POSZ             " ; NameVar = "JDSH_CALCposz_LeadCone_posz             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAPCRADI"           ; DbVarComment="POLYCONE_RADIUS           " ; NameVar = "JDSH_CAPCradi_PolyCone_Radius           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "CAPCPOSZ"           ; DbVarComment="POLYCONE_POSZ             " ; NameVar = "JDSH_CAPCposz_PolyCone_posz             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "I1"                 ; DbVarComment="I1                        " ; NameVar = "JDSH_i1                                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_0"            ; DbVarComment="ARRAY                     " ; NameVar = "JDSH_LCApx%0                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_1"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%1                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_2"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%2                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_3"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%3                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_4"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%4                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_5"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%5                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_6"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%6                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_7"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%7                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_8"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%8                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_9"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%9                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_10"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%10                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_11"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%11                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_12"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%12                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_13"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%13                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_14"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%14                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPX_15"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApx%15                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_0"            ; DbVarComment="ARRAY                     " ; NameVar = "JDSH_LCApy%0                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_1"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%1                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_2"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%2                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_3"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%3                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_4"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%4                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_5"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%5                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_6"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%6                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_7"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%7                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_8"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%8                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_9"            ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%9                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_10"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%10                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_11"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%11                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_12"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%12                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_13"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%13                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_14"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%14                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "LCAPY_15"           ; DbVarComment="                          " ; NameVar = "JDSH_LCApy%15                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "I2"                 ; DbVarComment="I2                        " ; NameVar = "JDSH_i2                                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_0"            ; DbVarComment="ARRAY                     " ; NameVar = "JDSH_PCApx%0                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_1"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%1                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_2"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%2                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_3"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%3                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_4"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%4                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_5"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%5                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_6"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%6                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_7"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%7                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_8"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%8                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_9"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%9                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_10"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%10                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_11"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%11                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_12"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%12                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_13"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%13                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_14"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%14                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPX_15"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApx%15                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_0"            ; DbVarComment="ARRAY                     " ; NameVar = "JDSH_PCApy%0                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_1"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%1                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_2"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%2                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_3"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%3                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_4"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%4                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_5"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%5                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_6"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%6                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_7"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%7                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_8"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%8                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_9"            ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%9                            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_10"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%10                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_11"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%11                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_12"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%12                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_13"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%13                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_14"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%14                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PCAPY_15"           ; DbVarComment="                          " ; NameVar = "JDSH_PCApy%15                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGINR2"           ; DbVarComment="PLUG_INNERRADIUS2         " ; NameVar = "JDSH_PLUGinR2_Plug_innerRadius2         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::JFSH(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JFSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "XYZREF"             ; DbVarComment="XYZREF                                      " ; NameVar = "JFSH_XYZref                                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ROT180"             ; DbVarComment="ROT180                                      " ; NameVar = "JFSH_rot180                                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBBLENG"           ; DbVarComment="AFRAME_BOTTOMBAR_LENGTH                     " ; NameVar = "JFSH_AFBBleng_AFrame_BottomBar_length               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBHEIGH"           ; DbVarComment="AFRAME_BAR_HEIGHT                           " ; NameVar = "JFSH_AFBheigh_AFrame_Bar_height                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFRWIDTH"           ; DbVarComment="AFRAME_WIDTH                                " ; NameVar = "JFSH_AFrwidth_AFrame_width                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBTHIC1"           ; DbVarComment="AFRAME_BAR_THICKNESS1                       " ; NameVar = "JFSH_AFBthic1_AFrame_Bar_thickness1                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBTHIC2"           ; DbVarComment="AFRAME_BAR_THICKNESS2                       " ; NameVar = "JFSH_AFBthic2_AFrame_Bar_thickness2                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLANGLE"           ; DbVarComment="AFRAME_LEG_ANGLE                            " ; NameVar = "JFSH_AFLangle_AFrame_Leg_angle                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSLENGT"           ; DbVarComment="AFRAME_SPACER_LENGTH                        " ; NameVar = "JFSH_AFSlengt_AFrame_Spacer_length                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSHEIGH"           ; DbVarComment="AFRAME_SPACER_HEIGHT                        " ; NameVar = "JFSH_AFSheigh_AFrame_Spacer_height                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBBX1"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_X1             " ; NameVar = "JFSH_AFWEBBx1_AFrame_Web_element_BottomBar_X1       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBBX2"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_X2             " ; NameVar = "JFSH_AFWEBBx2_AFrame_Web_element_BottomBar_X2       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELENG"           ; DbVarComment="AFRAME_WEB_ELEMENT_LENGTH                   " ; NameVar = "JFSH_AFWEleng_AFrame_Web_element_length             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBLX1"           ; DbVarComment="AFRAME_WEB_ELEMENT_LEG_DX1                  " ; NameVar = "JFSH_AFWEBLx1_AFrame_Web_element_Leg_DX1            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBLX2"           ; DbVarComment="AFRAME_WEB_ELEMENT_LEG_DX2                  " ; NameVar = "JFSH_AFWEBLx2_AFrame_Web_element_Leg_DX2            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFTHICK"           ; DbVarComment="AFRAME_FLANGE_THICKNESS                     " ; NameVar = "JFSH_AFFthick_AFrame_Flange_thickness               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFLENGT"           ; DbVarComment="AFRAME_FLANGE_LENGTH                        " ; NameVar = "JFSH_AFFlengt_AFrame_Flange_length                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPLENG"           ; DbVarComment="AFRAME_TOPPLATE_LENGTH                      " ; NameVar = "JFSH_AFTPleng_AFrame_TopPlate_length                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPHEIG"           ; DbVarComment="AFRAME_TOPPLATE_HEIGHT                      " ; NameVar = "JFSH_AFTPheig_AFrame_TopPlate_height                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWHEIGH"           ; DbVarComment="AFRAME_WEDGE_HEIGHT                         " ; NameVar = "JFSH_AFWheigh_AFrame_Wedge_height                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEXTEN"           ; DbVarComment="AFRAME_WEDGE_EXTENSION                      " ; NameVar = "JFSH_AFWexten_AFrame_Wedge_extension                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWTHICK"           ; DbVarComment="AFRAME_WEDGE_THICKNESS                      " ; NameVar = "JFSH_AFWthick_AFrame_Wedge_thickness                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWZOFFS"           ; DbVarComment="AFRAME_WEDGE_ZOFFSET                        " ; NameVar = "JFSH_AFWzoffs_AFrame_Wedge_zoffset                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEELEN"           ; DbVarComment="AFRAME_FEET_LENGTH                          " ; NameVar = "JFSH_AFFEElen_AFrame_Feet_length                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETHI"           ; DbVarComment="AFRAME_FEET_THICKNESS                       " ; NameVar = "JFSH_AFFEEthi_AFrame_Feet_thickness                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEESLE"           ; DbVarComment="AFRAME_FEET_SHORTLENGTH                     " ; NameVar = "JFSH_AFFEEsle_AFrame_Feet_shortlength               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEEHEI"           ; DbVarComment="AFRAME_FEET_HEIGHT                          " ; NameVar = "JFSH_AFFEEhei_AFrame_Feet_height                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEEEXT"           ; DbVarComment="AFRAME_FEET_X_EXTENSION                     " ; NameVar = "JFSH_AFFEEext_AFrame_Feet_X_extension               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTOTHEI"           ; DbVarComment="AFRAME_TOTAL_HEIGHT                         " ; NameVar = "JFSH_AFtothei_AFrame_Total_height                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFECTCLE"           ; DbVarComment="CLEARANCE_ECT_AFRAME                        " ; NameVar = "JFSH_AFECTcle_Clearance_ECT_AFrame                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ECTNOMIZ"           ; DbVarComment="ECT_NOMINAL_ZPOS                            " ; NameVar = "JFSH_ECTnomiz_ECT_Nominal_zpos                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFYPOSIT"           ; DbVarComment="AFRAME_YPOS                                 " ; NameVar = "JFSH_AFyposit_AFrame_Ypos                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFZPOSIT"           ; DbVarComment="AFRAME_ZPOS                                 " ; NameVar = "JFSH_AFzposit_AFrame_Zpos                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBLHPXA"           ; DbVarComment="AFBLHPXA                                    " ; NameVar = "JFSH_AFBLHPxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBLHPYA"           ; DbVarComment="AFBLHPYA                                    " ; NameVar = "JFSH_AFBLHPya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBLHPYB"           ; DbVarComment="AFBLHPYB                                    " ; NameVar = "JFSH_AFBLHPyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBLHPXB"           ; DbVarComment="AFBLHPXB                                    " ; NameVar = "JFSH_AFBLHPxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBBVPXA"           ; DbVarComment="AFBBVPXA                                    " ; NameVar = "JFSH_AFBBVPxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBBVPYA"           ; DbVarComment="AFBBVPYA                                    " ; NameVar = "JFSH_AFBBVPya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBBVPYB"           ; DbVarComment="AFBBVPYB                                    " ; NameVar = "JFSH_AFBBVPyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBBVPXB"           ; DbVarComment="AFBBVPXB                                    " ; NameVar = "JFSH_AFBBVPxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBUHPXA"           ; DbVarComment="AFBUHPXA                                    " ; NameVar = "JFSH_AFBUHPxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBUHPYA"           ; DbVarComment="AFBUHPYA                                    " ; NameVar = "JFSH_AFBUHPya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBUHPYB"           ; DbVarComment="AFBUHPYB                                    " ; NameVar = "JFSH_AFBUHPyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFBUHPXB"           ; DbVarComment="AFBUHPXB                                    " ; NameVar = "JFSH_AFBUHPxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPXA"           ; DbVarComment="AFLLHPXA                                    " ; NameVar = "JFSH_AFLLHPxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPYA"           ; DbVarComment="AFLLHPYA                                    " ; NameVar = "JFSH_AFLLHPya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPXB"           ; DbVarComment="AFLLHPXB                                    " ; NameVar = "JFSH_AFLLHPxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPYB"           ; DbVarComment="AFLLHPYB                                    " ; NameVar = "JFSH_AFLLHPyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPXC"           ; DbVarComment="AFLLHPXC                                    " ; NameVar = "JFSH_AFLLHPxc                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPYC"           ; DbVarComment="AFLLHPYC                                    " ; NameVar = "JFSH_AFLLHPyc                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPXD"           ; DbVarComment="AFLLHPXD                                    " ; NameVar = "JFSH_AFLLHPxd                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLLHPYD"           ; DbVarComment="AFLLHPYD                                    " ; NameVar = "JFSH_AFLLHPyd                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPXA"            ; DbVarComment="AFLVPXA                                     " ; NameVar = "JFSH_AFLVPxa                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPYA"            ; DbVarComment="AFLVPYA                                     " ; NameVar = "JFSH_AFLVPya                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPXB"            ; DbVarComment="AFLVPXB                                     " ; NameVar = "JFSH_AFLVPxb                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPYB"            ; DbVarComment="AFLVPYB                                     " ; NameVar = "JFSH_AFLVPyb                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPXC"            ; DbVarComment="AFLVPXC                                     " ; NameVar = "JFSH_AFLVPxc                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPYC"            ; DbVarComment="AFLVPYC                                     " ; NameVar = "JFSH_AFLVPyc                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPXE"            ; DbVarComment="AFLVPXE                                     " ; NameVar = "JFSH_AFLVPxe                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPYE"            ; DbVarComment="AFLVPYE                                     " ; NameVar = "JFSH_AFLVPye                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPYD"            ; DbVarComment="AFLVPYD                                     " ; NameVar = "JFSH_AFLVPyd                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLVPXD"            ; DbVarComment="AFLVPXD                                     " ; NameVar = "JFSH_AFLVPxd                                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPXA"           ; DbVarComment="AFLUHPXA                                    " ; NameVar = "JFSH_AFLUHPxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPYA"           ; DbVarComment="AFLUHPYA                                    " ; NameVar = "JFSH_AFLUHPya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPXB"           ; DbVarComment="AFLUHPXB                                    " ; NameVar = "JFSH_AFLUHPxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPYB"           ; DbVarComment="AFLUHPYB                                    " ; NameVar = "JFSH_AFLUHPyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPXC"           ; DbVarComment="AFLUHPXC                                    " ; NameVar = "JFSH_AFLUHPxc                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPYC"           ; DbVarComment="AFLUHPYC                                    " ; NameVar = "JFSH_AFLUHPyc                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPXD"           ; DbVarComment="AFLUHPXD                                    " ; NameVar = "JFSH_AFLUHPxd                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFLUHPYD"           ; DbVarComment="AFLUHPYD                                    " ; NameVar = "JFSH_AFLUHPyd                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPLAXA"           ; DbVarComment="AFTPLAXA                                    " ; NameVar = "JFSH_AFTPLAxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPLAYA"           ; DbVarComment="AFTPLAYA                                    " ; NameVar = "JFSH_AFTPLAya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPLAXB"           ; DbVarComment="AFTPLAXB                                    " ; NameVar = "JFSH_AFTPLAxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFTPLAYB"           ; DbVarComment="AFTPLAYB                                    " ; NameVar = "JFSH_AFTPLAyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSPACXA"           ; DbVarComment="AFSPACXA                                    " ; NameVar = "JFSH_AFSPACxa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSPACYA"           ; DbVarComment="AFSPACYA                                    " ; NameVar = "JFSH_AFSPACya                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSPACXB"           ; DbVarComment="AFSPACXB                                    " ; NameVar = "JFSH_AFSPACxb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFSPACYB"           ; DbVarComment="AFSPACYB                                    " ; NameVar = "JFSH_AFSPACyb                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBWID"           ; DbVarComment="AFRAME_WEB_ELEMENT_WIDTH                    " ; NameVar = "JFSH_AFWEBwid_AFrame_Web_element_width              " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBHEI"           ; DbVarComment="AFRAME_WEB_ELEMENT_HEIGHT                   " ; NameVar = "JFSH_AFWEBhei_AFrame_Web_element_height             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBBBY"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_Y              " ; NameVar = "JFSH_AFWEBBBy_AFrame_Web_element_BottomBar_Y        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWEBZPO"           ; DbVarComment="AFRAME_WEB_ELEMENT_Z                        " ; NameVar = "JFSH_AFWEBzpo_AFrame_Web_element_Z                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXI1"           ; DbVarComment="AFWELXI1                                    " ; NameVar = "JFSH_AFWELxi1                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYI1"           ; DbVarComment="AFWELYI1                                    " ; NameVar = "JFSH_AFWELyi1                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXP1"           ; DbVarComment="AFWELXP1                                    " ; NameVar = "JFSH_AFWELxp1                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYP1"           ; DbVarComment="AFWELYP1                                    " ; NameVar = "JFSH_AFWELyp1                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXI2"           ; DbVarComment="AFWELXI2                                    " ; NameVar = "JFSH_AFWELxi2                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYI2"           ; DbVarComment="AFWELYI2                                    " ; NameVar = "JFSH_AFWELyi2                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXP2"           ; DbVarComment="AFWELXP2                                    " ; NameVar = "JFSH_AFWELxp2                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYP2"           ; DbVarComment="AFWELYP2                                    " ; NameVar = "JFSH_AFWELyp2                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXI3"           ; DbVarComment="AFWELXI3                                    " ; NameVar = "JFSH_AFWELxi3                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYI3"           ; DbVarComment="AFWELYI3                                    " ; NameVar = "JFSH_AFWELyi3                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELXP3"           ; DbVarComment="AFWELXP3                                    " ; NameVar = "JFSH_AFWELxp3                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWELYP3"           ; DbVarComment="AFWELYP3                                    " ; NameVar = "JFSH_AFWELyp3                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFWIDTH"           ; DbVarComment="AFRAME_FLANGE_WIDTH                         " ; NameVar = "JFSH_AFFwidth_AFrame_Flange_width                   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFZPOSI"           ; DbVarComment="AFRAME_FLANGE_Z                             " ; NameVar = "JFSH_AFFzposi_AFrame_Flange_Z                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFPOSX1"           ; DbVarComment="AFRAME_FLANGE_X1                            " ; NameVar = "JFSH_AFFposx1_AFrame_Flange_X1                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFPOSY1"           ; DbVarComment="AFRAME_FLANGE_Y1                            " ; NameVar = "JFSH_AFFposy1_AFrame_Flange_Y1                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFPOSX2"           ; DbVarComment="AFRAME_FLANGE_X2                            " ; NameVar = "JFSH_AFFposx2_AFrame_Flange_X2                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFPOSY2"           ; DbVarComment="AFRAME_FLANGE_Y2                            " ; NameVar = "JFSH_AFFposy2_AFrame_Flange_Y2                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSXA"           ; DbVarComment="AFRAME_WEDGE_XA                             " ; NameVar = "JFSH_AFWposxa_AFrame_Wedge_XA                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSYA"           ; DbVarComment="AFRAME_WEDGE_YA                             " ; NameVar = "JFSH_AFWposya_AFrame_Wedge_YA                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSXB"           ; DbVarComment="AFRAME_WEDGE_XB                             " ; NameVar = "JFSH_AFWposxb_AFrame_Wedge_XB                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSYB"           ; DbVarComment="AFRAME_WEDGE_YB                             " ; NameVar = "JFSH_AFWposyb_AFrame_Wedge_YB                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSXC"           ; DbVarComment="AFRAME_WEDGE_XC                             " ; NameVar = "JFSH_AFWposxc_AFrame_Wedge_XC                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSYC"           ; DbVarComment="AFRAME_WEDGE_YC                             " ; NameVar = "JFSH_AFWposyc_AFrame_Wedge_YC                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSXD"           ; DbVarComment="AFRAME_WEDGE_XD                             " ; NameVar = "JFSH_AFWposxd_AFrame_Wedge_XD                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSYD"           ; DbVarComment="AFRAME_WEDGE_YD                             " ; NameVar = "JFSH_AFWposyd_AFrame_Wedge_YD                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSX0"           ; DbVarComment="AFRAME_WEDGE_X0                             " ; NameVar = "JFSH_AFWposx0_AFrame_Wedge_X0                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSY0"           ; DbVarComment="AFRAME_WEDGE_Y0                             " ; NameVar = "JFSH_AFWposy0_AFrame_Wedge_Y0                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSIX"           ; DbVarComment="AFRAME_WEDGE_X                              " ; NameVar = "JFSH_AFWposix_AFrame_Wedge_X                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSIY"           ; DbVarComment="AFRAME_WEDGE_Y                              " ; NameVar = "JFSH_AFWposiy_AFrame_Wedge_Y                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFWPOSIZ"           ; DbVarComment="AFRAME_WEDGE_Z                              " ; NameVar = "JFSH_AFWposiz_AFrame_Wedge_Z                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXA"           ; DbVarComment="AFRAME_FEET_XA                              " ; NameVar = "JFSH_AFFeetxa_AFrame_Feet_XA                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYA"           ; DbVarComment="AFRAME_FEET_YA                              " ; NameVar = "JFSH_AFFeetya_AFrame_Feet_YA                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXB"           ; DbVarComment="AFRAME_FEET_XB                              " ; NameVar = "JFSH_AFFeetxb_AFrame_Feet_XB                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYB"           ; DbVarComment="AFRAME_FEET_YB                              " ; NameVar = "JFSH_AFFeetyb_AFrame_Feet_YB                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXC"           ; DbVarComment="AFRAME_FEET_XC                              " ; NameVar = "JFSH_AFFeetxc_AFrame_Feet_XC                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYC"           ; DbVarComment="AFRAME_FEET_YC                              " ; NameVar = "JFSH_AFFeetyc_AFrame_Feet_YC                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXD"           ; DbVarComment="AFRAME_FEET_XD                              " ; NameVar = "JFSH_AFFeetxd_AFrame_Feet_XD                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYD"           ; DbVarComment="AFRAME_FEET_YD                              " ; NameVar = "JFSH_AFFeetyd_AFrame_Feet_YD                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXE"           ; DbVarComment="AFRAME_FEET_XE                              " ; NameVar = "JFSH_AFFeetxe_AFrame_Feet_XE                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYE"           ; DbVarComment="AFRAME_FEET_YE                              " ; NameVar = "JFSH_AFFeetye_AFrame_Feet_YE                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETXF"           ; DbVarComment="AFRAME_FEET_XF                              " ; NameVar = "JFSH_AFFeetxf_AFrame_Feet_XF                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETYF"           ; DbVarComment="AFRAME_FEET_YF                              " ; NameVar = "JFSH_AFFeetyf_AFrame_Feet_YF                        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFFEETPX"           ; DbVarComment="AFRAME_FEET_X                               " ; NameVar = "JFSH_AFFeetpx_AFrame_Feet_X                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFASPOSY"           ; DbVarComment="AFRAME_ASSEMBLY_Y                           " ; NameVar = "JFSH_AFASposy_AFrame_Assembly_Y                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFASPOSZ"           ; DbVarComment="AFRAME_ASSEMBLY_Z                           " ; NameVar = "JFSH_AFASposz_AFrame_Assembly_Z                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCLEN"           ; DbVarComment="JFCMAINCYLINDER_LENGTH                      " ; NameVar = "JFSH_JFCMClen_JFCMainCylinder_length                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCIR1"           ; DbVarComment="JFCMAINCYLINDER_INNERRADIUS1                " ; NameVar = "JFSH_JFCMCir1_JFCMainCylinder_innerRadius1          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCOUR"           ; DbVarComment="JFCMAINCYLINDER_OUTERRADIUS                 " ; NameVar = "JFSH_JFCMCouR_JFCMainCylinder_outerRadius           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCHSL"           ; DbVarComment="JFCMAINCYLINDER_HOLESLOPE                   " ; NameVar = "JFSH_JFCMChSl_JFCMainCylinder_HoleSLope             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCZOF"           ; DbVarComment="JFCMAINCYLINDER_ZOFFSET                     " ; NameVar = "JFSH_JFCMCzof_JFCMainCylinder_zoffset               " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGLENG"           ; DbVarComment="PLUG_LENGTH                                 " ; NameVar = "JFSH_PLUGleng_Plug_length                           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGINRA"           ; DbVarComment="PLUG_INNERRADIUS                            " ; NameVar = "JFSH_PLUGinRa_Plug_innerRadius                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGOURA"           ; DbVarComment="PLUG_OUTERRADIUS                            " ; NameVar = "JFSH_PLUGouRa_Plug_outerRadius                      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCMRA"           ; DbVarComment="JFSOCTOGON_MAINRADIUS                       " ; NameVar = "JFSH_JFSOCmRa_JFSOctogon_mainRadius                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCLEN"           ; DbVarComment="JFSOCTOGON_LENGTH                           " ; NameVar = "JFSH_JFSOClen_JFSOctogon_length                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCZOF"           ; DbVarComment="JFSOCTOGON_ZOFFSET                          " ; NameVar = "JFSH_JFSOCzof_JFSOctogon_zoffset                    " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFOCONLE"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_LENGTH      " ; NameVar = "JFSH_AFOconle_AFrame_to_JFSOctogon_connection_length" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFOCONWI"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_WIDTH       " ; NameVar = "JFSH_AFOconwi_AFrame_to_JFSOctogon_connection_width " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFOCONHE"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_HEIGHT      " ; NameVar = "JFSH_AFOconhe_AFrame_to_JFSOctogon_connection_height" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E1ORA"           ; DbVarComment="TX1E1ORA                                    " ; NameVar = "JFSH_TX1e1oRa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E1IRA"           ; DbVarComment="TX1E1IRA                                    " ; NameVar = "JFSH_TX1e1iRa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E1ILE"           ; DbVarComment="TX1E1ILE                                    " ; NameVar = "JFSH_TX1e1iLe                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E2ORA"           ; DbVarComment="TX1E2ORA                                    " ; NameVar = "JFSH_TX1e2oRa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E2IRA"           ; DbVarComment="TX1E2IRA                                    " ; NameVar = "JFSH_TX1e2iRa                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E2ILE"           ; DbVarComment="TX1E2ILE                                    " ; NameVar = "JFSH_TX1e2iLe                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1STZOF"           ; DbVarComment="TX1STZOF                                    " ; NameVar = "JFSH_TX1STzof                                       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCIR2"           ; DbVarComment="JFCMAINCYLINDER_INNERRADIUS2                " ; NameVar = "JFSH_JFCMCir2_JFCMainCylinder_innerRadius2          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFCMCZPO"           ; DbVarComment="JFCMAINCYLINDER_Z                           " ; NameVar = "JFSH_JFCMCzpo_JFCMainCylinder_Z                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCZPO"           ; DbVarComment="JFSOCTOGON_Z                                " ; NameVar = "JFSH_JFSOCzpo_JFSOctogon_Z                          " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E1ZPO"           ; DbVarComment="TX1STM_ELEMENT1_Z                           " ; NameVar = "JFSH_TX1e1zpo_tx1stm_element1_Z                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "TX1E2ZPO"           ; DbVarComment="TX1STM_ELEMENT2_Z                           " ; NameVar = "JFSH_TX1e2zpo_tx1stm_element2_Z                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLUGZPOS"           ; DbVarComment="PLUG_POSZ                                   " ; NameVar = "JFSH_PLUGzpos_Plug_posz                             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFMCCONY"           ; DbVarComment="AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Y      " ; NameVar = "JFSH_AFMCcony_AFrame_to_JFCMainCylinder_connection_Y" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "AFMCCONZ"           ; DbVarComment="AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Z      " ; NameVar = "JFSH_AFMCconz_AFrame_to_JFCMainCylinder_connection_Z" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCTXA"           ; DbVarComment="JFSOCTOGON_XA                               " ; NameVar = "JFSH_JFSOctxa_JFSOctogon_XA                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCTYA"           ; DbVarComment="JFSOCTOGON_YA                               " ; NameVar = "JFSH_JFSOctya_JFSOctogon_YA                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCTXB"           ; DbVarComment="JFSOCTOGON_XB                               " ; NameVar = "JFSH_JFSOctxb_JFSOctogon_XB                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "JFSOCTYB"           ; DbVarComment="JFSOCTOGON_YB                               " ; NameVar = "JFSH_JFSOctyb_JFSOctogon_YB                         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromAmdc::JTSH(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JTSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "OPLENGTH"             ; DbVarComment="OUTERPLUGS_LENGTH            " ; NameVar = "JTSH_OPlength_OuterPlugs_length            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "OPINNRAD"             ; DbVarComment="OUTERPLUGS_INNERRADIUS       " ; NameVar = "JTSH_OPinnRad_OuterPlugs_innerRadius       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "OPOUTRAD"             ; DbVarComment="OUTERPLUGS_OUTERRADIUS       " ; NameVar = "JTSH_OPoutRad_OuterPlugs_outerRadius       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPLENGTH"             ; DbVarComment="INNERPLUGS_LENGTH            " ; NameVar = "JTSH_IPlength_InnerPlugs_length            " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPINNRA1"             ; DbVarComment="INNERPLUGS_INNERRADIUS1      " ; NameVar = "JTSH_IPinnRa1_InnerPlugs_innerRadius1      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPOUTRAD"             ; DbVarComment="INNERPLUGS_OUTERRADIUS       " ; NameVar = "JTSH_IPoutRad_InnerPlugs_outerRadius       " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPTILTAN"             ; DbVarComment="INNERPLUGS_TILTANGLE         " ; NameVar = "JTSH_IPtiltAn_InnerPlugs_tiltAngle         " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPZOFFSE"             ; DbVarComment="INNERPLUGS_ZOFFSET           " ; NameVar = "JTSH_IPzoffse_InnerPlugs_zoffset           " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLENGTH"             ; DbVarComment="POLYRINGS_LENGTH             " ; NameVar = "JTSH_PRlength_PolyRings_length             " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRINNRAD"             ; DbVarComment="POLYRINGS_INNERRADIUS        " ; NameVar = "JTSH_PRinnRad_PolyRings_innerRadius        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PROUTRAD"             ; DbVarComment="POLYRINGS_OUTERRADIUS        " ; NameVar = "JTSH_PRoutRad_PolyRings_outerRadius        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PREXTTAN"             ; DbVarComment="POLYRINGS_EXTENSION_TILTANGLE" ; NameVar = "JTSH_PRexttAn_PolyRings_extension_tiltAngle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRIPEXLE"             ; DbVarComment="POLYRINGS_IPEXTENSION_LENGTH " ; NameVar = "JTSH_PRipexle_PolyRings_IPextension_length " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "ZPOSITIO"             ; DbVarComment="SHIELD_Z                     " ; NameVar = "JTSH_zpositio_Shield_Z                     " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "YROTATIO"             ; DbVarComment="SHIELD_ROTY                  " ; NameVar = "JTSH_yrotatio_Shield_rotY                  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPINNRA2"             ; DbVarComment="INNERPLUGS_INNERRADIUS2      " ; NameVar = "JTSH_IPinnRa2_InnerPlugs_innerRadius2      " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PREXTZPO"             ; DbVarComment="POLYRINGS_EXTENSION_Z        " ; NameVar = "JTSH_PRextzpo_PolyRings_extension_z        " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "OPZPOSI1"             ; DbVarComment="OUTERPLUGS_Z1                " ; NameVar = "JTSH_OPzposi1_OuterPlugs_z1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "OPZPOSI2"             ; DbVarComment="OUTERPLUGS_Z2                " ; NameVar = "JTSH_OPzposi2_OuterPlugs_z2                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPZPOSI1"             ; DbVarComment="INNERPLUGS_Z1                " ; NameVar = "JTSH_IPzposi1_InnerPlugs_z1                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "IPZPOSI2"             ; DbVarComment="INNERPLUGS_Z2                " ; NameVar = "JTSH_IPzposi2_InnerPlugs_z2                " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRZPOSI1"             ; DbVarComment="POLYRINGS_Z1                 " ; NameVar = "JTSH_PRzposi1_PolyRings_z1                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRZPOSI2"             ; DbVarComment="POLYRINGS_Z2                 " ; NameVar = "JTSH_PRzposi2_PolyRings_z2                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRZPOSI3"             ; DbVarComment="POLYRINGS_Z3                 " ; NameVar = "JTSH_PRzposi3_PolyRings_z3                 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::MBAP(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "MBAP";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string NameVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  m_UniversalIdKounter = m_UniversalIdKounter + 1;
  AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);
  
    DbVar = "P12S1DZ1"             ; DbVarComment="1353 P12S1DZ1  " ; NameVar = "MBAP_P12s1dz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P12S1DZ2"             ; DbVarComment="1354 P12S1DZ2  " ; NameVar = "MBAP_P12s1dz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P23S1DZ1"             ; DbVarComment="1355 P23S1DZ1  " ; NameVar = "MBAP_P23s1dz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P23S1DZ2"             ; DbVarComment="1356 P23S1DZ2  " ; NameVar = "MBAP_P23s1dz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P34S1DZ1"             ; DbVarComment="1357 P34S1DZ1  " ; NameVar = "MBAP_P34s1dz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P34S1DZ2"             ; DbVarComment="1358 P34S1DZ2  " ; NameVar = "MBAP_P34s1dz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3DZ"              ; DbVarComment="1359 P14S3DZ   " ; NameVar = "MBAP_P14s3dz " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3DX"              ; DbVarComment="1360 P14S3DX   " ; NameVar = "MBAP_P14s3dx " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLAS1DX1"             ; DbVarComment="1361 PLAS1DX1  " ; NameVar = "MBAP_PLAs1dx1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLAS1DX2"             ; DbVarComment="1362 PLAS1DX2  " ; NameVar = "MBAP_PLAs1dx2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFLHE"                ; DbVarComment="1363 PFLHE     " ; NameVar = "MBAP_PFLhe   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS01POSX"             ; DbVarComment="1364 PS01POSX  " ; NameVar = "MBAP_Ps01posx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS01POSY"             ; DbVarComment="1365 PS01POSY  " ; NameVar = "MBAP_Ps01posy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS11POSX"             ; DbVarComment="1366 PS11POSX  " ; NameVar = "MBAP_Ps11posx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS11POSY"             ; DbVarComment="1367 PS11POSY  " ; NameVar = "MBAP_Ps11posy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS14POSX"             ; DbVarComment="1368 PS14POSX  " ; NameVar = "MBAP_Ps14posx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS14POSY"             ; DbVarComment="1369 PS14POSY  " ; NameVar = "MBAP_Ps14posy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PS14ANGZ"             ; DbVarComment="1370 PS14ANGZ  " ; NameVar = "MBAP_Ps14angz" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P12POSZ"              ; DbVarComment="1371 P12POSZ   " ; NameVar = "MBAP_P12posz " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P23POSZ"              ; DbVarComment="1372 P23POSZ   " ; NameVar = "MBAP_P23posz " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P34POSZ"              ; DbVarComment="1373 P34POSZ   " ; NameVar = "MBAP_P34posz " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "XYZREF"               ; DbVarComment="1374 XYZREF    " ; NameVar = "MBAP_xyzref  " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL12XA"              ; DbVarComment="1375 PFL12XA   " ; NameVar = "MBAP_PFL12xa " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL23XA"              ; DbVarComment="1376 PFL23XA   " ; NameVar = "MBAP_PFL23xa " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL34XA"              ; DbVarComment="1377 PFL34XA   " ; NameVar = "MBAP_PFL34xa " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFLYA"                ; DbVarComment="1378 PFLYA     " ; NameVar = "MBAP_PFLya   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL12XB"              ; DbVarComment="1379 PFL12XB   " ; NameVar = "MBAP_PFL12xb " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL23XB"              ; DbVarComment="1380 PFL23XB   " ; NameVar = "MBAP_PFL23xb " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL34XB"              ; DbVarComment="1381 PFL34XB   " ; NameVar = "MBAP_PFL34xb " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFLYB"                ; DbVarComment="1382 PFLYB     " ; NameVar = "MBAP_PFLyb   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL12XC"              ; DbVarComment="1383 PFL12XC   " ; NameVar = "MBAP_PFL12xc " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL23XC"              ; DbVarComment="1384 PFL23XC   " ; NameVar = "MBAP_PFL23xc " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL34XC"              ; DbVarComment="1385 PFL34XC   " ; NameVar = "MBAP_PFL34xc " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFLYC"                ; DbVarComment="1386 PFLYC     " ; NameVar = "MBAP_PFLyc   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL12XD"              ; DbVarComment="1387 PFL12XD   " ; NameVar = "MBAP_PFL12xd " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL23XD"              ; DbVarComment="1388 PFL23XD   " ; NameVar = "MBAP_PFL23xd " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFL34XD"              ; DbVarComment="1389 PFL34XD   " ; NameVar = "MBAP_PFL34xd " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PFLYD"                ; DbVarComment="1390 PFLYD     " ; NameVar = "MBAP_PFLyd   " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRARO"             ; DbVarComment="1391 PLATRARO  " ; NameVar = "MBAP_PlatRaRo" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRARI"             ; DbVarComment="1392 PLATRARI  " ; NameVar = "MBAP_PlatRaRi" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAHE"             ; DbVarComment="1393 PLATRAHE  " ; NameVar = "MBAP_PlatRaHe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RAZE"             ; DbVarComment="1394 PL12RAZE  " ; NameVar = "MBAP_Pl12RaZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RAZE"             ; DbVarComment="1395 PL23RAZE  " ; NameVar = "MBAP_Pl23RaZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RAZE"             ; DbVarComment="1396 PL34RAZE  " ; NameVar = "MBAP_Pl34RaZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRHYP"             ; DbVarComment="1397 PLATRHYP  " ; NameVar = "MBAP_PlatRHyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATBTLE"             ; DbVarComment="1398 PLATBTLE  " ; NameVar = "MBAP_PlatBTle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01DX"             ; DbVarComment="1399 PRLS01DX  " ; NameVar = "MBAP_PRLs01dx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11DX"             ; DbVarComment="1400 PRLS11DX  " ; NameVar = "MBAP_PRLs11dx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01DY"             ; DbVarComment="1401 PRLS01DY  " ; NameVar = "MBAP_PRLs01dy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11DY"             ; DbVarComment="1402 PRLS11DY  " ; NameVar = "MBAP_PRLs11dy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DY1"             ; DbVarComment="1403 PSSS3DY1  " ; NameVar = "MBAP_PSSs3dy1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DY2"             ; DbVarComment="1404 PSSS3DY2  " ; NameVar = "MBAP_PSSs3dy2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DX1"             ; DbVarComment="1405 PSSS3DX1  " ; NameVar = "MBAP_PSSs3dx1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DX2"             ; DbVarComment="1406 PSSS3DX2  " ; NameVar = "MBAP_PSSs3dx2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DX3"             ; DbVarComment="1407 PSSS3DX3  " ; NameVar = "MBAP_PSSs3dx3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3AN"              ; DbVarComment="1408 PSSS3AN   " ; NameVar = "MBAP_PSSs3an " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP1"             ; DbVarComment="1409 PSSS3ZP1  " ; NameVar = "MBAP_PSSs3zp1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP2"             ; DbVarComment="1410 PSSS3ZP2  " ; NameVar = "MBAP_PSSs3zp2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP3"             ; DbVarComment="1411 PSSS3ZP3  " ; NameVar = "MBAP_PSSs3zp3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP4"             ; DbVarComment="1412 PSSS3ZP4  " ; NameVar = "MBAP_PSSs3zp4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP5"             ; DbVarComment="1413 PSSS3ZP5  " ; NameVar = "MBAP_PSSs3zp5" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3ZP6"             ; DbVarComment="1414 PSSS3ZP6  " ; NameVar = "MBAP_PSSs3zp6" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAZ1"             ; DbVarComment="1415 PLATRAZ1  " ; NameVar = "MBAP_PlatRaz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAY1"             ; DbVarComment="1416 PLATRAY1  " ; NameVar = "MBAP_PlatRay1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAZ2"             ; DbVarComment="1417 PLATRAZ2  " ; NameVar = "MBAP_PlatRaz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAY2"             ; DbVarComment="1418 PLATRAY2  " ; NameVar = "MBAP_PlatRay2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RAZ3"             ; DbVarComment="1419 PL12RAZ3  " ; NameVar = "MBAP_Pl12Raz3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RAZ3"             ; DbVarComment="1420 PL23RAZ3  " ; NameVar = "MBAP_Pl23Raz3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RAZ3"             ; DbVarComment="1421 PL34RAZ3  " ; NameVar = "MBAP_Pl34Raz3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAY3"             ; DbVarComment="1422 PLATRAY3  " ; NameVar = "MBAP_PlatRay3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RAZ4"             ; DbVarComment="1423 PL12RAZ4  " ; NameVar = "MBAP_Pl12Raz4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RAZ4"             ; DbVarComment="1424 PL23RAZ4  " ; NameVar = "MBAP_Pl23Raz4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RAZ4"             ; DbVarComment="1425 PL34RAZ4  " ; NameVar = "MBAP_Pl34Raz4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAY4"             ; DbVarComment="1426 PLATRAY4  " ; NameVar = "MBAP_PlatRay4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RAZ0"             ; DbVarComment="1427 PL12RAZ0  " ; NameVar = "MBAP_Pl12Raz0" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RAZ0"             ; DbVarComment="1428 PL23RAZ0  " ; NameVar = "MBAP_Pl23Raz0" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RAZ0"             ; DbVarComment="1429 PL34RAZ0  " ; NameVar = "MBAP_Pl34Raz0" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRAY0"             ; DbVarComment="1430 PLATRAY0  " ; NameVar = "MBAP_PlatRay0" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RHZP"             ; DbVarComment="1431 PL12RHZP  " ; NameVar = "MBAP_Pl12RHzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RHZP"             ; DbVarComment="1432 PL23RHZP  " ; NameVar = "MBAP_Pl23RHzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RHZP"             ; DbVarComment="1433 PL34RHZP  " ; NameVar = "MBAP_Pl34RHzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRVZE"             ; DbVarComment="1434 PLATRVZE  " ; NameVar = "MBAP_PlatRVZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATRVYP"             ; DbVarComment="1435 PLATRVYP  " ; NameVar = "MBAP_PlatRVyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATCRYP"             ; DbVarComment="1436 PLATCRYP  " ; NameVar = "MBAP_PlatCRyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12CRZ1"             ; DbVarComment="1437 PL12CRZ1  " ; NameVar = "MBAP_Pl12CRz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12CRZ2"             ; DbVarComment="1438 PL12CRZ2  " ; NameVar = "MBAP_Pl12CRz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23CRZ1"             ; DbVarComment="1439 PL23CRZ1  " ; NameVar = "MBAP_Pl23CRz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23CRZ2"             ; DbVarComment="1440 PL23CRZ2  " ; NameVar = "MBAP_Pl23CRz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34CRZ1"             ; DbVarComment="1441 PL34CRZ1  " ; NameVar = "MBAP_Pl34CRz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34CRZ2"             ; DbVarComment="1442 PL34CRZ2  " ; NameVar = "MBAP_Pl34CRz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12CRAN"             ; DbVarComment="1443 PL12CRAN  " ; NameVar = "MBAP_Pl12CRan" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23CRAN"             ; DbVarComment="1444 PL23CRAN  " ; NameVar = "MBAP_Pl23CRan" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34CRAN"             ; DbVarComment="1445 PL34CRAN  " ; NameVar = "MBAP_Pl34CRan" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12CRLE"             ; DbVarComment="1446 PL12CRLE  " ; NameVar = "MBAP_Pl12CRle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23CRLE"             ; DbVarComment="1447 PL23CRLE  " ; NameVar = "MBAP_Pl23CRle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34CRLE"             ; DbVarComment="1448 PL34CRLE  " ; NameVar = "MBAP_Pl34CRle" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLAT2RAX"             ; DbVarComment="1449 PLAT2RAX  " ; NameVar = "MBAP_Plat2Rax" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12BTZ1"             ; DbVarComment="1450 PL12BTZ1  " ; NameVar = "MBAP_Pl12BTz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23BTZ1"             ; DbVarComment="1451 PL23BTZ1  " ; NameVar = "MBAP_Pl23BTz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34BTZ1"             ; DbVarComment="1452 PL34BTZ1  " ; NameVar = "MBAP_Pl34BTz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12BTZ2"             ; DbVarComment="1453 PL12BTZ2  " ; NameVar = "MBAP_Pl12BTz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23BTZ2"             ; DbVarComment="1454 PL23BTZ2  " ; NameVar = "MBAP_Pl23BTz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34BTZ2"             ; DbVarComment="1455 PL34BTZ2  " ; NameVar = "MBAP_Pl34BTz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATBTXP"             ; DbVarComment="1456 PLATBTXP  " ; NameVar = "MBAP_PlatBTxp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01XP"             ; DbVarComment="1457 PRLS01XP  " ; NameVar = "MBAP_PRLs01xp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11XP"             ; DbVarComment="1458 PRLS11XP  " ; NameVar = "MBAP_PRLs11xp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01YP"             ; DbVarComment="1459 PRLS01YP  " ; NameVar = "MBAP_PRLs01yp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11YP"             ; DbVarComment="1460 PRLS11YP  " ; NameVar = "MBAP_PRLs11yp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RLZ1"             ; DbVarComment="1461 PL12RLZ1  " ; NameVar = "MBAP_Pl12RLz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RLZ1"             ; DbVarComment="1462 PL23RLZ1  " ; NameVar = "MBAP_Pl23RLz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RLZ1"             ; DbVarComment="1463 PL34RLZ1  " ; NameVar = "MBAP_Pl34RLz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12RLZ2"             ; DbVarComment="1464 PL12RLZ2  " ; NameVar = "MBAP_Pl12RLz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23RLZ2"             ; DbVarComment="1465 PL23RLZ2  " ; NameVar = "MBAP_Pl23RLz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34RLZ2"             ; DbVarComment="1466 PL34RLZ2  " ; NameVar = "MBAP_Pl34RLz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01AN"             ; DbVarComment="1467 PRLS01AN  " ; NameVar = "MBAP_PRLs01an" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11AN"             ; DbVarComment="1468 PRLS11AN  " ; NameVar = "MBAP_PRLs11an" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS01LE"             ; DbVarComment="1469 PRLS01LE  " ; NameVar = "MBAP_PRLs01le" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRLS11LE"             ; DbVarComment="1470 PRLS11LE  " ; NameVar = "MBAP_PRLs11le" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATBSXP"             ; DbVarComment="1471 PLATBSXP  " ; NameVar = "MBAP_PlatBSxp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATBTLS"             ; DbVarComment="1472 PLATBTLS  " ; NameVar = "MBAP_PlatBTls" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL12FRZP"             ; DbVarComment="1473 PL12FRZP  " ; NameVar = "MBAP_Pl12FRzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL23FRZP"             ; DbVarComment="1474 PL23FRZP  " ; NameVar = "MBAP_Pl23FRzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL34FRZP"             ; DbVarComment="1475 PL34FRZP  " ; NameVar = "MBAP_Pl34FRzp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATFRYP"             ; DbVarComment="1476 PLATFRYP  " ; NameVar = "MBAP_PlatFRyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PLATFRXP"             ; DbVarComment="1477 PLATFRXP  " ; NameVar = "MBAP_PlatFRxp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DYT"             ; DbVarComment="1478 PSSS3DYT  " ; NameVar = "MBAP_PSSs3dyt" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3DXT"             ; DbVarComment="1479 PSSS3DXT  " ; NameVar = "MBAP_PSSs3dxt" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X0"              ; DbVarComment="1480 PSSS3X0   " ; NameVar = "MBAP_PSSs3x0 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y0"              ; DbVarComment="1481 PSSS3Y0   " ; NameVar = "MBAP_PSSs3y0 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X1"              ; DbVarComment="1482 PSSS3X1   " ; NameVar = "MBAP_PSSs3x1 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y1"              ; DbVarComment="1483 PSSS3Y1   " ; NameVar = "MBAP_PSSs3y1 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X2"              ; DbVarComment="1484 PSSS3X2   " ; NameVar = "MBAP_PSSs3x2 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y2"              ; DbVarComment="1485 PSSS3Y2   " ; NameVar = "MBAP_PSSs3y2 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X3"              ; DbVarComment="1486 PSSS3X3   " ; NameVar = "MBAP_PSSs3x3 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y3"              ; DbVarComment="1487 PSSS3Y3   " ; NameVar = "MBAP_PSSs3y3 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X4"              ; DbVarComment="1488 PSSS3X4   " ; NameVar = "MBAP_PSSs3x4 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y4"              ; DbVarComment="1489 PSSS3Y4   " ; NameVar = "MBAP_PSSs3y4 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X5"              ; DbVarComment="1490 PSSS3X5   " ; NameVar = "MBAP_PSSs3x5 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y5"              ; DbVarComment="1491 PSSS3Y5   " ; NameVar = "MBAP_PSSs3y5 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3X6"              ; DbVarComment="1492 PSSS3X6   " ; NameVar = "MBAP_PSSs3x6 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSS3Y6"              ; DbVarComment="1493 PSSS3Y6   " ; NameVar = "MBAP_PSSs3y6 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSRAXP"              ; DbVarComment="1494 PSSRAXP   " ; NameVar = "MBAP_PSSRaxp " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSRAYP"              ; DbVarComment="1495 PSSRAYP   " ; NameVar = "MBAP_PSSRayp " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP1"              ; DbVarComment="1496 PSSZRP1   " ; NameVar = "MBAP_PSSzrp1 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP2"              ; DbVarComment="1497 PSSZRP2   " ; NameVar = "MBAP_PSSzrp2 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP3"              ; DbVarComment="1498 PSSZRP3   " ; NameVar = "MBAP_PSSzrp3 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP4"              ; DbVarComment="1499 PSSZRP4   " ; NameVar = "MBAP_PSSzrp4 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP5"              ; DbVarComment="1500 PSSZRP5   " ; NameVar = "MBAP_PSSzrp5 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PSSZRP6"              ; DbVarComment="1501 PSSZRP6   " ; NameVar = "MBAP_PSSzrp6 " ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP1"             ; DbVarComment="1502 PRHS3ZP1  " ; NameVar = "MBAP_PRHs3zp1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP2"             ; DbVarComment="1503 PRHS3ZP2  " ; NameVar = "MBAP_PRHs3zp2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP3"             ; DbVarComment="1504 PRHS3ZP3  " ; NameVar = "MBAP_PRHs3zp3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP4"             ; DbVarComment="1505 PRHS3ZP4  " ; NameVar = "MBAP_PRHs3zp4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP5"             ; DbVarComment="1506 PRHS3ZP5  " ; NameVar = "MBAP_PRHs3zp5" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP6"             ; DbVarComment="1507 PRHS3ZP6  " ; NameVar = "MBAP_PRHs3zp6" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PRHS3ZP7"             ; DbVarComment="1508 PRHS3ZP7  " ; NameVar = "MBAP_PRHs3zp7" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RAZE"             ; DbVarComment="1509 PL14RAZE  " ; NameVar = "MBAP_Pl14RaZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RAZP"             ; DbVarComment="1510 PL14RAZP  " ; NameVar = "MBAP_Pl14Razp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RAY1"             ; DbVarComment="1511 PL14RAY1  " ; NameVar = "MBAP_Pl14Ray1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RAY2"             ; DbVarComment="1512 PL14RAY2  " ; NameVar = "MBAP_Pl14Ray2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RVZE"             ; DbVarComment="1513 PL14RVZE  " ; NameVar = "MBAP_Pl14RVZe" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14RVYP"             ; DbVarComment="1514 PL14RVYP  " ; NameVar = "MBAP_Pl14RVyp" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRY1"             ; DbVarComment="1515 PL14CRY1  " ; NameVar = "MBAP_Pl14CRy1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRY2"             ; DbVarComment="1516 PL14CRY2  " ; NameVar = "MBAP_Pl14CRy2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P12S3CRA"             ; DbVarComment="1517 P12S3CRA  " ; NameVar = "MBAP_P12s3CRa" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P12S3CRL"             ; DbVarComment="1518 P12S3CRL  " ; NameVar = "MBAP_P12s3CRl" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P23S3CRA"             ; DbVarComment="1519 P23S3CRA  " ; NameVar = "MBAP_P23s3CRa" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P23S3CRL"             ; DbVarComment="1520 P23S3CRL  " ; NameVar = "MBAP_P23s3CRl" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P34S3CRA"             ; DbVarComment="1521 P34S3CRA  " ; NameVar = "MBAP_P34s3CRa" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P34S3CRL"             ; DbVarComment="1522 P34S3CRL  " ; NameVar = "MBAP_P34s3CRl" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ1"             ; DbVarComment="1523 PL14CRZ1  " ; NameVar = "MBAP_Pl14CRz1" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ2"             ; DbVarComment="1524 PL14CRZ2  " ; NameVar = "MBAP_Pl14CRz2" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ3"             ; DbVarComment="1525 PL14CRZ3  " ; NameVar = "MBAP_Pl14CRz3" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ4"             ; DbVarComment="1526 PL14CRZ4  " ; NameVar = "MBAP_Pl14CRz4" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ5"             ; DbVarComment="1527 PL14CRZ5  " ; NameVar = "MBAP_Pl14CRz5" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ6"             ; DbVarComment="1528 PL14CRZ6  " ; NameVar = "MBAP_Pl14CRz6" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ7"             ; DbVarComment="1529 PL14CRZ7  " ; NameVar = "MBAP_Pl14CRz7" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ8"             ; DbVarComment="1530 PL14CRZ8  " ; NameVar = "MBAP_Pl14CRz8" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZ9"             ; DbVarComment="1531 PL14CRZ9  " ; NameVar = "MBAP_Pl14CRz9" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "PL14CRZA"             ; DbVarComment="1532 PL14CRZA  " ; NameVar = "MBAP_Pl14CRzA" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3BFL"             ; DbVarComment="1533 P14S3BFL  " ; NameVar = "MBAP_P14s3BFl" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3BFA"             ; DbVarComment="1534 P14S3BFA  " ; NameVar = "MBAP_P14s3BFa" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3BFX"             ; DbVarComment="1535 P14S3BFX  " ; NameVar = "MBAP_P14s3BFx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3BFY"             ; DbVarComment="1536 P14S3BFY  " ; NameVar = "MBAP_P14s3BFy" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3FLA"             ; DbVarComment="1537 P14S3FLA  " ; NameVar = "MBAP_P14s3Fla" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3FLX"             ; DbVarComment="1538 P14S3FLX  " ; NameVar = "MBAP_P14s3Flx" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3FLY"             ; DbVarComment="1539 P14S3FLY  " ; NameVar = "MBAP_P14s3Fly" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;
    DbVar = "P14S3FLZ"             ; DbVarComment="1540 P14S3FLZ  " ; NameVar = "MBAP_P14s3Flz" ; AddXmlDble(pIMboySvc,DbVar,DbVarComment,NameVar,Iok,pAmdcDbRecordset,pAmdcDbRecord,LocalEpsLengthCM) ; if (Iok != 1) return;

  pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromAmdc::AgddXMLVariables(IMboySvc* pIMboySvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AgddXMLVariables";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 
  int Iok;

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  int Ivar_Max;  pIMboySvc->GetAgddXmlVarNb(Ivar_Max);
  for (int Ivar=1 ; Ivar<=Ivar_Max ; Ivar++){

    m_UniversalIdKounter = m_UniversalIdKounter + 1;
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(m_UniversalIdKounter,NameOfTheSet);

    DbVar = "NameOfVariable"  ; DbVarComment="Name Of Variable"  ; pIMboySvc->GetAgddXmlVarName(Ivar,sDbVal)        ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "ValueOfVariable" ; DbVarComment="Value Of Variable" ; pIMboySvc->GetAgddXmlVarValue(sDbVal,Iok,dDbVal) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

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


void AmdcDbSvcMakerFromAmdc::AddXmlDble(
                   IMboySvc* pIMboySvc,
                   std::string DbVar,
                   std::string DbVarComment,
                   std::string NameVar,
                   int& Iok, 
                   AmdcDbRecordset* pAmdcDbRecordset , 
                   AmdcDbRecord*    pAmdcDbRecord,
                   int TheEps 
){

  double      dDbVal	   = 0.;

  pIMboySvc->GetAgddXmlVarValue(NameVar,Iok,dDbVal) ; 

  if (Iok != 1) { 
    std::cout 
      << " AmdcDbSvcMakerFromAmdc::AddXmlDble "
      << " PROBLEM IOK ne 1 WITH  "
      << NameVar
      << std::endl ;
    delete pAmdcDbRecordset ; 
    delete pAmdcDbRecord  ; 
    return ;
  }
  pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,TheEps);

}
