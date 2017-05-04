/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbSvcMakerFromRDB.h"
#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"
#include "AmdcDb/AmdcDbSvc.h"
#include "AmdcDb/AmdcDbRecordset.h"
#include "AmdcDb/AmdcDbRecord.h"

AmdcDbSvcMakerFromRDB::AmdcDbSvcMakerFromRDB(){

 m_EpsLengthMM = 4 ;
 m_EpsLengthCM = 5 ;
 m_EpsAngle    = 6 ;

 m_Map_Set_EpsLengthMM["Dummy"] = 0 ; m_Map_Set_EpsLengthMM.clear() ;
 m_Map_Set_EpsLengthCM["Dummy"] = 0 ; m_Map_Set_EpsLengthCM.clear() ;
 m_Map_Set_EpsAngle   ["Dummy"] = 0 ; m_Map_Set_EpsAngle.clear()    ;
  
}

AmdcDbSvcMakerFromRDB::~AmdcDbSvcMakerFromRDB(){}

// Set a AmdcDbSvc
void AmdcDbSvcMakerFromRDB::SetEpsLengthMM(int EpsLengthMM){ m_EpsLengthMM  = EpsLengthMM ;}
void AmdcDbSvcMakerFromRDB::SetEpsLengthCM(int EpsLengthCM){ m_EpsLengthCM  = EpsLengthCM ;}
void AmdcDbSvcMakerFromRDB::SetEpsAngle(int EpsAngle){ m_EpsAngle  = EpsAngle ;}
void AmdcDbSvcMakerFromRDB::SetEpsLengthMM(std::string NameOfTheSet, int EpsLengthMM) { m_Map_Set_EpsLengthMM[NameOfTheSet] = EpsLengthMM ; }
void AmdcDbSvcMakerFromRDB::SetEpsLengthCM(std::string NameOfTheSet, int EpsLengthCM) { m_Map_Set_EpsLengthCM[NameOfTheSet] = EpsLengthCM ; }
void AmdcDbSvcMakerFromRDB::SetEpsAngle   (std::string NameOfTheSet, int EpsAngle   ) { m_Map_Set_EpsAngle   [NameOfTheSet] = EpsAngle    ; }
void AmdcDbSvcMakerFromRDB::Set(
            std::string detectorKey  ,
            std::string detectorNode ,
            IRDBAccessSvc* pIRDBAccessSvc,
	    AmdcDbSvc* pAmdcDbSvc
){ 

  m_detectorKey  = detectorKey  ;
  m_detectorNode = detectorNode ;
  
  pIRDBAccessSvc->connect();

  AMDC(pIRDBAccessSvc,pAmdcDbSvc);
  AGDD(pIRDBAccessSvc,pAmdcDbSvc);
  
  ATYP(pIRDBAccessSvc,pAmdcDbSvc);
  ACUT(pIRDBAccessSvc,pAmdcDbSvc);
  
  WRPC(pIRDBAccessSvc,pAmdcDbSvc);
  AWLN(pIRDBAccessSvc,pAmdcDbSvc);
  WTGC(pIRDBAccessSvc,pAmdcDbSvc);
  GGLN(pIRDBAccessSvc,pAmdcDbSvc);
  ATLN(pIRDBAccessSvc,pAmdcDbSvc);
  WCSC(pIRDBAccessSvc,pAmdcDbSvc);
  DBAM(pIRDBAccessSvc,pAmdcDbSvc);
  ASMP(pIRDBAccessSvc,pAmdcDbSvc);
  WDED(pIRDBAccessSvc,pAmdcDbSvc);
  WLBI(pIRDBAccessSvc,pAmdcDbSvc);
  WCRO(pIRDBAccessSvc,pAmdcDbSvc);
  WCMI(pIRDBAccessSvc,pAmdcDbSvc);
  WCHV(pIRDBAccessSvc,pAmdcDbSvc);
  WSUP(pIRDBAccessSvc,pAmdcDbSvc);
  WSPA(pIRDBAccessSvc,pAmdcDbSvc);
  WMDT(pIRDBAccessSvc,pAmdcDbSvc);
  ALIN(pIRDBAccessSvc,pAmdcDbSvc);
  ALMN(pIRDBAccessSvc,pAmdcDbSvc);
  APTP(pIRDBAccessSvc,pAmdcDbSvc);
  
  ASZT(pIRDBAccessSvc,pAmdcDbSvc);
  ISZT(pIRDBAccessSvc,pAmdcDbSvc);

//  ABRT(pIRDBAccessSvc,pAmdcDbSvc);
//  AECT(pIRDBAccessSvc,pAmdcDbSvc);
//  ECST(pIRDBAccessSvc,pAmdcDbSvc);
//  FEET(pIRDBAccessSvc,pAmdcDbSvc);
//  GENV(pIRDBAccessSvc,pAmdcDbSvc);
//  JDSH(pIRDBAccessSvc,pAmdcDbSvc);
//  JFSH(pIRDBAccessSvc,pAmdcDbSvc);
//  JTSH(pIRDBAccessSvc,pAmdcDbSvc);
//  MBAP(pIRDBAccessSvc,pAmdcDbSvc);

//   AgddXMLVariables(pIRDBAccessSvc,pAmdcDbSvc);

  HwSwIdMapping(pIRDBAccessSvc,pAmdcDbSvc);

  XtomoData(pIRDBAccessSvc,pAmdcDbSvc);

  pIRDBAccessSvc->shutdown();

}

int AmdcDbSvcMakerFromRDB::GetEpsLengthMM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthMM.find(NameOfTheSet) != m_Map_Set_EpsLengthMM.end()) return m_Map_Set_EpsLengthMM.find(NameOfTheSet)->second ; return m_EpsLengthMM; }
int AmdcDbSvcMakerFromRDB::GetEpsLengthCM(std::string NameOfTheSet){ if (m_Map_Set_EpsLengthCM.find(NameOfTheSet) != m_Map_Set_EpsLengthCM.end()) return m_Map_Set_EpsLengthCM.find(NameOfTheSet)->second ; return m_EpsLengthCM; }
int AmdcDbSvcMakerFromRDB::GetEpsAngle   (std::string NameOfTheSet){ if (m_Map_Set_EpsAngle.find(NameOfTheSet)    != m_Map_Set_EpsAngle.end()   ) return m_Map_Set_EpsAngle.find(NameOfTheSet)->second    ; return m_EpsAngle   ; }

void AmdcDbSvcMakerFromRDB::AMDC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AMDC";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"         ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "VNAME"   ; DbVarComment="NAME"            ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "LENAMDC" ; DbVarComment="STRING LENGTH"   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "NLINE"   ; DbVarComment="CHAR4 NUMBER"    ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "DATA"    ; DbVarComment="(NLINE)-ASCII"   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addBlob(DbVar,DbVarComment,sDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::AGDD(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AGDD";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"         ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "VNAME"   ; DbVarComment="NAME"            ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "LENAGDD" ; DbVarComment="STRING LENGTH"   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "NLINE"   ; DbVarComment="CHAR4 NUMBER"    ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "DATA"    ; DbVarComment="(NLINE)-ASCII"   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addBlob(DbVar,DbVarComment,sDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ATYP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ATYP";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  		          ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JTYP"    ; DbVarComment="AMDB STATION TYPE"                  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"     ; DbVarComment="AMDB STATION NAME"	          ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

//DRING: What is it?       						        
    DbVar = "NSTA"    ; DbVarComment="NUMBER OF STATIONS OF THIS TYPE"    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ACUT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ACUT";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  			     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "I"       ; DbVarComment="INDEX"  			             ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ICUT"    ; DbVarComment="CUT-OUT INDEX"  			     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "N"       ; DbVarComment="NUMBER OF SUB-CUTS IN THE STATION"     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WRPC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WRPC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  			     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?       						        
    DbVar = "NVRS"    ; DbVarComment="VERSION OF RPC TECHNOLOGY"	     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?       						        
    DbVar = "LAYRPC"  ; DbVarComment="LAYERS NUMBER"			     ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TCKRLA"  ; DbVarComment="THICK. OF AN RPC LAYER"		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);  
    DbVar = "TOTTCK"  ; DbVarComment="TOTAL THICKNESS"  		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKFSP"  ; DbVarComment="THICK. OF FOAM SPACER"		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ACKFSP"  ; DbVarComment="THICK. OF AL PLATE OF FOAM SPACER"     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TLOHCB"  ; DbVarComment="THICK. OF LOWER HONEYCOMB"	     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ALOHCB"  ; DbVarComment="THICK. OF AL PLATE OF LOWER HONEYCOMB" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKBAK"  ; DbVarComment="THICK. OF BAKELITE"		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKGAS"  ; DbVarComment="THICK. OF GAS GAP "		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKSSU"  ; DbVarComment="THICK. OF STRIPS SUPPORT" 	     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKSTR"  ; DbVarComment="THICK. OF STRIPS" 		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SDEDMI"  ; DbVarComment="S INTERNAL MID-CHBER DEAD REGION"      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZDEDMI"  ; DbVarComment="Z INTERNAL MID-CHBER DEAD REGION"      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SPDIAM"  ; DbVarComment="SPACER DIAMETER"  		     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SPPITC"  ; DbVarComment="SPACER PITCH"			     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STROFF_0"; DbVarComment="STRIP OFFSET S, FIRST Z, SECOND Z"     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "STROFF_1"; DbVarComment="" 				     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "STROFF_2"; DbVarComment="" 				     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);


    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::AWLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AWLN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  		      ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"    ; DbVarComment="JSTA TYPE NUMBER" 	      ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "SPITCH"  ; DbVarComment="S-STRIPS PITCH"		      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPITCH"  ; DbVarComment="Z-STRIPS PITCH"		      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DEDSTR"  ; DbVarComment="DEAD REAGION BETWEEN STRIP"     ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "NSREST"  ; DbVarComment="NBER OF S STRIPS READOUTS"      ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NZREST"  ; DbVarComment="NBER OF S GAS GAPS"	      ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "SFIRST"  ; DbVarComment="S-PHI STRIP OFFSET"	      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "ZFIRST"  ; DbVarComment="Z-ETA STRIP OFFSET"	      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "DEDSEP"  ; DbVarComment="DEAD SEPARATION"  	      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    DbVar = "NSROST"  ; DbVarComment="NUMBER OF S-PHI READOUT STRIPS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NZROST"  ; DbVarComment="NUMBER OF Z-ETA READOUT STRIPS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WTGC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WTGC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  		  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"    ; DbVarComment="JSTA INDEX"		  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NBEVOL"  ; DbVarComment="NUMBER OF DETAILS"	  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "X0"      ; DbVarComment="X0"			  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "WIDCHB"  ; DbVarComment="WIDTH OF THE CHBER ALONG Z" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FWIRCH"  ; DbVarComment="FRAME WIDTH IN R" 	  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FWIXCH"  ; DbVarComment="FRAME WIDTH IN X" 	  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    for (int DB_Item=1; DB_Item<=9 ; DB_Item++){
      DbVar = "ALLNAME" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      sDbVal =(*it)->getString(DbVar);
      DbVarComment="";
      if (DB_Item == 1) DbVarComment="MATERIAL";

      pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    }

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::GGLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "GGLN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  				; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "JSTA"    ; DbVarComment="JSTA INDEX"				; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NWGS_0"  ; DbVarComment="N OF GROUPS OF WIRES (GANG) PER CH. IN E" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NWGS_1"  ; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NWGS_2"  ; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NSPS_0"  ; DbVarComment="N OF STRIPS PER CHAMBER"  		; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NSPS_1"  ; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NSPS_2"  ; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ROFFST_0"; DbVarComment="OFFSET OF WIRE GROUP ADDRESS 1"		; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ROFFST_1"; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ROFFST_2"; DbVarComment="" 					; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?  From SB reserved but never used   
    DbVar = "POFFST_0"; DbVarComment="" 					; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

//DRING: What is it?  From SB reserved but never used   
    DbVar = "POFFST_1"; DbVarComment="" 					; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

//DRING: What is it?  From SB reserved but never used   
    DbVar = "POFFST_2"; DbVarComment="" 					; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    DbVar = "WIRESP"  ; DbVarComment="WIRE SPACING [MM]"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "STRIPSP" ; DbVarComment="STRIP SPACING [MM]"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

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
    	iDbVal =(*it)->getInt(DbVar);
        DbVarComment="";
        if ( DB_IWGS==1 && DB_Item==1 ) DbVarComment="IWGS1 WIRE GANGING";
        if ( DB_IWGS==2 && DB_Item==1 ) DbVarComment="IWGS2 WIRE GANGING";
        if ( DB_IWGS==3 && DB_Item==1 ) DbVarComment="IWGS3 WIRE GANGING";

    	pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

      }
    }

    DbVar = "PDIST"   ; DbVarComment="! PHYSICAL DISTANCE OF STRIPS W.R.T. BAS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "S1PP"    ; DbVarComment="WIRE SUPPORT : WIDTH"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "S2PP"    ; DbVarComment="WIRE SUPPORT : WIDTH OF GAS CHANNEL"	; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "WSEP"    ; DbVarComment="WIRE SUPPORT : SEPARATION"		; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP1WI"   ; DbVarComment="WIRE SUPPORT : OFFSET OF WIRE SUPPORT LA" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP2WI"   ; DbVarComment="WIRE SUPPORT : LAYER 2"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP3WI"   ; DbVarComment="WIRE SUPPORT : LAYER 3"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "TILT"    ; DbVarComment="WIRE SUPPORT : TILT ANGLE (DEG)"  	; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP1BU"   ; DbVarComment="BUTTON SUPPORT : RADIUS"  		; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP2BU"   ; DbVarComment="BUTTON SUPPORT :SEPARATION"		; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP3BU"   ; DbVarComment="BUTTON SUPPORT : PITCH"			; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "SP4BU"   ; DbVarComment="BUTTON SUPPORT : ANGLE IN TRAPEZOID REGI" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
  
    for (int DB_Item=1; DB_Item<=33 ; DB_Item++){
      DbVar = "SLARGE" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      dDbVal =(*it)->getFloat(DbVar);
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
      dDbVal =(*it)->getFloat(DbVar);
      DbVarComment="";
      if (DB_Item==1) DbVarComment="START OF STRIP ON LARGE BASE 1ST PLANE";

      pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    }

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ATLN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ATLN";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  	   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "I"       ; DbVarComment="INDEX"		   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "ICOVOL"  ; DbVarComment="MATERIAL CODE"	   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "ZPOVOL"  ; DbVarComment="RELATIVE Z POSITION" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WIDVOL"  ; DbVarComment="Z WIDTH"  	   ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  
    DbVar = "NAMVOL"  ; DbVarComment="MATERIAL NAME"       ; sDbVal =(*it)->getString(DbVar); pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
  
    DbVar = "JSTA"    ; DbVarComment="JSTA INDEX "         ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WCSC(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCSC";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  			 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "JSTA"    ; DbVarComment="JSTA TYPE NUMBER" 		 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "LAYCSC"  ; DbVarComment="NBER OF CSCS LAYERS"		 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TTOTAL"  ; DbVarComment="TOTAL THICKNESS"  		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TNOMEX"  ; DbVarComment="NOMEX HONEYCOMB THICKNESS"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TLAG10"  ; DbVarComment="G10 LAMINATES THICKNESS"  	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WISPA"   ; DbVarComment="WIRE SPACING"			 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DANCAT"  ; DbVarComment="ANODE-CATHODE DISTANCE"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCATRE"  ; DbVarComment="CATHODE READOUT PITCH"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "AZCAT"   ; DbVarComment="CATHODE READOUT PITCH, AZIMUTAL"   ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "GSTRIP"  ; DbVarComment="GAP BETWEEN CATHODE STRIPS"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WRESTR"  ; DbVarComment="WIDTH OF READOUT STRIPS"  	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WFLSTR"  ; DbVarComment="WIDTH OF FLOATING STRIPS" 	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRRWAS"  ; DbVarComment="RIGIT RECTANGULAR WASHER THICKNES" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WROXA"   ; DbVarComment="ROXACELL WIDTH"			 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GROXWI"  ; DbVarComment="ROXACELL AND WIRE BAR GAP"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WGASBA"  ; DbVarComment="FULL GAS GAP BAR WIDTH"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TGASBA"  ; DbVarComment="FULL GAS GAP BAR THICK."  	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WGASCU"  ; DbVarComment="CUTS GAS GAP BAR WIDTH"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TGASCU"  ; DbVarComment="CUTS GAS GAP BAR THICK."  	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WFIXWI"  ; DbVarComment="FULL WIRE FIX. BAR WID."  	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TFIXWI"  ; DbVarComment="FULL WIRE FIX. BAR THICK."	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PBA1WI"  ; DbVarComment="WIRE BAR POSITION"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PBA2WI"  ; DbVarComment="WIRE BAR POSITION"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PBA3WI"  ; DbVarComment="WIRE BAR POSITION"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

//DRING: What is it?  From SB reserved but never used    						    
    DbVar = "PSNDCO"  ; DbVarComment="2ND COORDINATE PITCH"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::DBAM(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "DBAM";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  	      ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "NVRS"    ; DbVarComment="VERSION OF READING"     ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "AMDB"    ; DbVarComment="AMDB NAME"	      ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
  
    DbVar = "TEST"    ; DbVarComment="TEST NAME"	      ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
  
    DbVar = "MTYP"    ; DbVarComment="MAXIMUM STATION NUMBER" ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    int DB_MTYP = iDbVal = (*it)->getInt(DbVar)  ;
    for (int DB_Item=1; DB_Item<=DB_MTYP ; DB_Item++){
      DbVar = "NAME" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      sDbVal =(*it)->getString(DbVar);
      DbVarComment="" ;
      if (DB_Item==1) DbVarComment="STATION NAME" ;

      pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    }
  
//DRING: I do not know what is the meaning of this variable
    DbVar = "NUMBOX"  ; DbVarComment="FILE INDEX" ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ASMP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ASMP";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"    ; DbVarComment="VERSION"  		    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "INDX"    ; DbVarComment="STATION NUMBER (INSIDE TYPE)" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "N"       ; DbVarComment="NUMBER OF ELEMENTS"	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JTYP"    ; DbVarComment="AMDB STATION TYPE"	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WDED(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WDED";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"   ; DbVarComment="VERSION"		  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"   ; DbVarComment="INDEX"		  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NB"     ; DbVarComment="NUMBER OF DETAILS"   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "X0"     ; DbVarComment="X0"		  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "AUPHCB" ; DbVarComment="HONEYCOMB THICKNESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TCKDED" ; DbVarComment="ALUMINIUM THICKNESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WLBI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WLBI";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"      ; DbVarComment="VERSION"	   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"      ; DbVarComment="INDEX"  	   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NUM"	; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "HEIGHT"	; DbVarComment="HEIGHT" 	   ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "THICKNESS" ; DbVarComment="WALL THICKNESS"    ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "LOWERTHICK"    ; DbVarComment="LOWER THICK"   ; dDbVal = 0. ; if (!(*it)->isFieldNull(DbVar)) dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SHIFTYSTATION" ; DbVarComment="SHIFTY STATION"; dDbVal = 0. ; if (!(*it)->isFieldNull(DbVar)) dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WCRO(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCRO";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"       ; DbVarComment="VERSION"	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"       ; DbVarComment="INDEX" 	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NUM"	 ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"	    ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WCMI(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCMI";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"       ; DbVarComment="VERSION"	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"       ; DbVarComment="INDEX" 	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NUM"	 ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"	    ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WCHV(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WCHV";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"	 ; DbVarComment="VERSION"	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"	 ; DbVarComment="INDEX" 	    ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NUM"	 ; DbVarComment="NUMBER OF OBJECTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "HEIGHTNESS" ; DbVarComment="HEIGHT"	    ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LARGENESS"  ; DbVarComment="T-SHAPE LARGENESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "THICKNESS"  ; DbVarComment="T-SHAPE THICKNESS" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WSUP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WSUP";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"   ; DbVarComment="VERSION"                 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"   ; DbVarComment="INDEX"                   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NXXSUP" ; DbVarComment="MAX NB. FOR X FRAGMENTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "NZZSUP" ; DbVarComment="MAX NB. FOR Z FRAGMENTS" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "X0"     ; DbVarComment="X0"                      ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,4);
    DbVar = "THICKN" ; DbVarComment="THICKNESS"               ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,4);
  
    for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
      DbVar = "XXSUP" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      dDbVal =(*it)->getFloat(DbVar);
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
      dDbVal =(*it)->getFloat(DbVar);
      DbVarComment="" ;
      if (DB_Item==1) DbVarComment="Z DIMENSION" ; 

      pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    }

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WSPA(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WSPA";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"   ; DbVarComment="VERSION"		  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "JSTA"   ; DbVarComment="JSTA INDEX"	  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "NB"     ; DbVarComment="NUMBER OF DETAILS"   ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "X0"     ; DbVarComment="X0"		  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "TCKSPA" ; DbVarComment="THICKNESS OF SPACER" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::WMDT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "WMDT";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"   ; DbVarComment="VERSION"		   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"    ; DbVarComment="NAME"		   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "IW"     ; DbVarComment="INDEX"		   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "X0"     ; DbVarComment="X0"		   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    DbVar = "LAYMDT" ; DbVarComment="MAXIMUM LAYER NUMBER" ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "TUBPIT" ; DbVarComment="PITCH BETWEEN TUBE"   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBRAD" ; DbVarComment="RADIUS OF TUBE"	   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBSTA" ; DbVarComment="THICKNESS OF TUBE"    ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBDEA" ; DbVarComment="DEAD LENGTH IN TUBES" ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
  
    for (int DB_Item=1; DB_Item<=4 ; DB_Item++){
      DbVar = "TUBXCO" ;
      DbVar = DbVar+"_";
      std::ostringstream Aostringstream;
      int DB_Item_Shifted = DB_Item - 1 ;
      Aostringstream << DB_Item_Shifted ;
      DbVar = DbVar+Aostringstream.str();
      dDbVal =(*it)->getFloat(DbVar);
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
      dDbVal =(*it)->getFloat(DbVar);
      DbVarComment="" ;
      if (DB_Item==1) DbVarComment="X TUBE POSITION" ;

      pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    }

    DbVar = "TUBWAL" ; DbVarComment="TUBE WALL THICKNESS" ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ALIN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ALIN";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"     ; DbVarComment="VERSION" 				 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE SUB-CUT"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE SUB-CUT"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "I"        ; DbVarComment="SERIAL NB. OF THE OBJECT IN WHICH THE SU" ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "WIDTH_XS" ; DbVarComment="S DIMENSIONS OF THE SUB-CUT"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WIDTH_XL" ; DbVarComment="L DIMENSIONS OF THE SUB-CUT"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LENGTH_Y" ; DbVarComment="Y DIMENSIONS OF THE SUB-CUT"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCENT"   ; DbVarComment="EXC DIMENSIONS OF THE SUB-CUT"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DEAD1"    ; DbVarComment="D1 DIMENSIONS OF THE SUB-CUT"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    DbVar = "JTYP"     ; DbVarComment="STATION TYPE"				 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "INDX"     ; DbVarComment="INDEX"					 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ICUT"     ; DbVarComment="CUT-OUT INDEX"				 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ALMN(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ALMN";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"     ; DbVarComment="VERSION"                                  ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "I"        ; DbVarComment="ELEMENT NUMBER"                           ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "DX"       ; DbVarComment="X RELATIVE POSITION OF THE OBJECT"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DY"       ; DbVarComment="Y RELATIVE POSITION OF THE OBJECT"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DZ"       ; DbVarComment="Z RELATIVE POSITION OF THE OBJECT"	 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "JOB"      ; DbVarComment="OBJECT SERIAL NUMBER IN THE STATION"	 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TEC"      ; DbVarComment="TYPE OF TECHNOLOGY USED FOR THE OBJECT"   ; sDbVal =(*it)->getString(DbVar); pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "IW"       ; DbVarComment="INNER STRUCTURE TYPE INDEX, ISTA"	 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "ISPLIT_X" ; DbVarComment="NUMBER OF SECTIONS IN X" 		 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ISPLIT_Y" ; DbVarComment="NUMBER OF SECTIONS IN Y" 		 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "ISHAPE"   ; DbVarComment="TYPE OF GEOMETRICAL SHAPE 0-TRAPEZOIDAL," ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "WIDTH_XS" ; DbVarComment="S WIDTH, WS"				 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "WIDTH_XL" ; DbVarComment="L WIDTH, WL"				 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LENGTH_Y" ; DbVarComment="Y LENGTH, LE"				 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCENT"   ; DbVarComment="ADDITIONAL INFORMATION, EX"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DEAD1"    ; DbVarComment="FIRST DEAD MATERIAL, D1" 		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DEAD2"    ; DbVarComment="SECOND DEAD MATERIAL, D2"		 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DEAD3"    ; DbVarComment="STEPS, D3"				 ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "JTYP"     ; DbVarComment="STATION TYPE"				 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "INDX"     ; DbVarComment="ELEMENT NUMBER"  			 ; iDbVal = (*it)->getInt(DbVar)  ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::APTP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "APTP";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"  ; DbVarComment="VERSION"			   ; iDbVal = (*it)->getInt(DbVar)     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
//DRING: What is it?						    	
    DbVar = "LINE"  ; DbVarComment="LINE NUMBER"		   ; iDbVal = (*it)->getInt(DbVar)     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    DbVar = "TYP"   ; DbVarComment="STATION TYPE"		   ; sDbVal = (*it)->getString(DbVar)  ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "I"     ; DbVarComment="STATION AMDB INDEX" 	   ; iDbVal = (*it)->getInt(DbVar)     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "ICUT"  ; DbVarComment="CUT-OUT INDEX,ZERO IF MISSING" ; iDbVal = (*it)->getInt(DbVar)     ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
  
    for (int DB_JFF=1; DB_JFF<=8 ; DB_JFF++){
      int DB_JFF_Shifted = DB_JFF - 1 ;
      std::ostringstream Aostringstream;
      Aostringstream << DB_JFF_Shifted ;
      DbVar = "IPHI_"+Aostringstream.str();
      int iDbVal =(*it)->getInt(DbVar);
      DbVarComment="" ;
      if (DB_JFF==1) DbVarComment="PHI INDICATES OF OCTANTS" ;
      pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    }
    
    DbVar = "IZ"    ; DbVarComment="Z (FOR BARREL) OR R (FOR END-CAPS) POS."  ; iDbVal = (*it)->getInt(DbVar)	; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "DPHI"  ; DbVarComment="RELATIVE PHI POSITION OF THE STATION IN"  ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

    DbVar = "Z"     ; DbVarComment="Z POSITION OF THE LOWEST Z EDGE OF THE S" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "R"     ; DbVarComment="RADIAL POSITION OF ITS INNERMOST EDGE"    ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "S"     ; DbVarComment="ORTHO-RADIAL POSITION OF THE CENTER OF T" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "ALFA"  ; DbVarComment="ALFA ANGLE DEFINING THE DEVIATION [GRAD]" ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "BETA"  ; DbVarComment="BETA ANGLE DEFINING THE DEVIATION"        ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "GAMMA" ; DbVarComment="GAMMA ANGLE DEFINING THE DEVIATION"       ; dDbVal = (*it)->getFloat(DbVar); pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);


    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ASZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ASZT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"  ; DbVarComment="VERSION"		; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

//DRING: What is it?	
//  DbVar = "LINE"  ; DbVarComment="LINE NUMBER"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "LINE"  ; DbVarComment="LINE NUMBER"	; iDbVal = 0                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"   ; DbVarComment="STATION TYPE"	; sDbVal = (*it)->getString(DbVar); pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
    DbVar = "JFF"   ; DbVarComment="PHI POSITION"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JZZ"   ; DbVarComment="Z POSITION" 	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JOB"   ; DbVarComment="JOB POSITION"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TRAS"  ; DbVarComment="S TRANSLATION [MM]" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAZ"  ; DbVarComment="Z TRANSLATION"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAT"  ; DbVarComment="T TRANSLATION"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "ROTS"  ; DbVarComment="S ROTATION [RAD]"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTZ"  ; DbVarComment="Z ROTATION" 	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTT"  ; DbVarComment="T ROTATION" 	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

//DRING: What is it? From SB error	
//  DbVar = "I"     ; DbVarComment="STATION AMDB INDEX" ; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "I"     ; DbVarComment="STATION AMDB INDEX" ; iDbVal = 0                      ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);


    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::ISZT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ISZT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "VERS"  ; DbVarComment="VERSION"		; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TYP"   ; DbVarComment="STATION TYPE"	; sDbVal = (*it)->getString(DbVar); pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);
    DbVar = "JFF"   ; DbVarComment="PHI POSITION"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JZZ"   ; DbVarComment="Z POSITION" 	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JOB"   ; DbVarComment="JOB POSITION"	; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);
    DbVar = "JLAY"  ; DbVarComment="JLAY POSITION"      ; iDbVal = (*it)->getInt(DbVar)   ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "TRAS"  ; DbVarComment="S TRANSLATION [MM]" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAZ"  ; DbVarComment="Z TRANSLATION"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TRAT"  ; DbVarComment="T TRANSLATION"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    DbVar = "ROTS"  ; DbVarComment="S ROTATION [RAD]"	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTZ"  ; DbVarComment="Z ROTATION" 	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);
    DbVar = "ROTT"  ; DbVarComment="T ROTATION" 	; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsAngle);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::ABRT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ABRT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "CRYORMIN"  ; DbVarComment="CRYORMIN                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORMAX"  ; DbVarComment="CRYORMAX                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOZMAX"  ; DbVarComment="CRYOZMAX                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORCUR"  ; DbVarComment="CRYORCUR                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORADI"  ; DbVarComment="RADIUS                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORADT"  ; DbVarComment="RADTHICK                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTD0"  ; DbVarComment="CRYOATTDIAM0                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTD1"  ; DbVarComment="CRYOATTDIAM1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTAL"  ; DbVarComment="CRYOATTALFA                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTXH"  ; DbVarComment="CRYOATTXHIGH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTTH"  ; DbVarComment="CRYOATTTHICK                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTRX"  ; DbVarComment="CRYOATTRMAX                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWIY"  ; DbVarComment="CRYOATTWINGY                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWXP"  ; DbVarComment="CRYOATTWINGXP               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWXN"  ; DbVarComment="CRYOATTWINGXN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWBO"  ; DbVarComment="CRYOATTWINGBOSS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWTH"  ; DbVarComment="CRYOATTWINGTHIC             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWZE"  ; DbVarComment="CRYOATTWINGZESP             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWRA"  ; DbVarComment="CRYOATTWINGRAD              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATWYC"  ; DbVarComment="CRYOATTWINGYCUT             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIBYW"  ; DbVarComment="CRYORIBYWID                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIBZL"  ; DbVarComment="CRYORIBZLEN                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIBTH"  ; DbVarComment="CRYORIBTHICK                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIWYP"  ; DbVarComment="CRYORIBWINGYP               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIWYN"  ; DbVarComment="CRYORIBWINGYN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIWXP"  ; DbVarComment="CRYORIBWINGXP               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIWXN"  ; DbVarComment="CRYORIBWINGXN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIWTH"  ; DbVarComment="CRYORIBWINGTHIC             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRNGRM"  ; DbVarComment="CRYORINGRMED                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRNGZM"  ; DbVarComment="CRYORINGZMED                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRNGRA"  ; DbVarComment="CRYORINGRADIUS              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTRMAX"  ; DbVarComment="STRTRMAX                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTRMIN"  ; DbVarComment="STRTRMIN                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTYLEN"  ; DbVarComment="STRTYLEN                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTZWID"  ; DbVarComment="STRUTZWIDTH                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTRTHI"  ; DbVarComment="STRUTRTHICK                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTZTHI"  ; DbVarComment="STRUTZTHICK                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWRMAX"  ; DbVarComment="STRUTWINGRMAX               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWYMAX"  ; DbVarComment="STRUTWINGYMAX               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWRMIN"  ; DbVarComment="STRUTWINGRMIN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWYMIN"  ; DbVarComment="STRUTWINGYMIN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWZTHI"  ; DbVarComment="STRUTWINGZTHICK             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWYTHI"  ; DbVarComment="STRUTWINGYTHICK             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRWZLEN"  ; DbVarComment="STRUTWINGZLEN               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_0"  ; DbVarComment="ARRAY                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_1"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_2"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_3"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_4"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_5"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_6"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZVOUSS_7"  ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_0"    ; DbVarComment="ARRAY                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_1"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_2"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_3"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_4"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_5"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZRIB_6"    ; DbVarComment="                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYRIXHI"  ; DbVarComment="CRYORIBXHIGH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTRMOY"  ; DbVarComment="STRTRMOY                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STRTYLNP"  ; DbVarComment="STRUTYLENP                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYATTXS"  ; DbVarComment="CRYOATTXSEC                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARMIN"  ; DbVarComment="COLDMASSRMIN                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARMAX"  ; DbVarComment="COLDMASSRMAX                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMAZMAX"  ; DbVarComment="COLDMASSZMAX                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARCUI"  ; DbVarComment="COLDMASSRCURVI              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARTHI"  ; DbVarComment="COLDMASSRTHICK              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMAYTHI"  ; DbVarComment="COLDMASSYTHICK              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARIBY"  ; DbVarComment="COLDMASSRIBY                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARIBZ"  ; DbVarComment="COLDMASSRIBZ                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMABOSS"  ; DbVarComment="COLDMASSBOSS                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COMARIBX"  ; DbVarComment="COLDMASSRIBX                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELYHEI"  ; DbVarComment="BIELLYHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELYSO1"  ; DbVarComment="BIELLYSOLE1                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELYSO2"  ; DbVarComment="BIELLYSOLE2                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELXSOL"  ; DbVarComment="BIELLXSOLE                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELXTH1"  ; DbVarComment="BIELLXTHICK1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELXTH2"  ; DbVarComment="BIELLXTHICK2                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELZTH1"  ; DbVarComment="BIELLZTHICK1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELRHOL"  ; DbVarComment="BIELLRADHOLE                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELYCHO"  ; DbVarComment="BIELLYCENHOLE               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELATL1"  ; DbVarComment="BIELLATTLENG1               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELATL2"  ; DbVarComment="BIELLATTLENG2               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELATTH"  ; DbVarComment="BIELLATTTHICK               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BIELATHE"  ; DbVarComment="BIELLATTHEIGHT              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSBLXH"  ; DbVarComment="VOUSSBLOCKXHEIGHT           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSPLIX"  ; DbVarComment="VOUSSPLATEINNERXWID         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSPLOX"  ; DbVarComment="VOUSSPLATEOUTERXWID         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSBLYW"  ; DbVarComment="VOUSSBLOCKYWID              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUBLYWS"  ; DbVarComment="VOUSSBLOCKYWIDSUB           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOURCYWI"  ; DbVarComment="VOUSSRECTCUTYWID            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOURPYWI"  ; DbVarComment="VOUSSREINFORCEPLATYWID      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUBLZLE"  ; DbVarComment="VOUSSBLOCKZLEN              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUBLZLS"  ; DbVarComment="VOUSSBLOCKZLENSUB           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUBZWTH"  ; DbVarComment="VOUSSBLOCKZWALLTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUCUTZE"  ; DbVarComment="VOUSSCUTZEXT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOURCUTR"  ; DbVarComment="VOUSSROUNDCUTRAD            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOURECSL"  ; DbVarComment="VOUSSRECTCUTXYSLANT         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCOYEX"  ; DbVarComment="CONBOXCOREYEXTENSION        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCOXEX"  ; DbVarComment="CONBOXCOREXEXTENSION        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCOXSU"  ; DbVarComment="CONBOXCOREXSUP              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCOXIN"  ; DbVarComment="CONBOXCOREXINF              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEAHEI"  ; DbVarComment="CONBOXEARHEIGHT             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACXI"  ; DbVarComment="CONBOXEARCUTXIN             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACTL"  ; DbVarComment="CONBOXEARCUTPLATETHICKLOWOUT" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACTU"  ; DbVarComment="CONBOXEARCUTPLATETHICKUP    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACZW"  ; DbVarComment="CONBOXEARCUTZWALL           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACZE"  ; DbVarComment="CONBOXEARCUTZEXTR           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEACPL"  ; DbVarComment="CONBOXINNEREARCUTPLATE      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCADMA"  ; DbVarComment="CONBOXCAVITYDMAX            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCADMI"  ; DbVarComment="CONBOXCAVITYDMIN            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCAZEX"  ; DbVarComment="CONBOXCAVITYZEXT            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBOXZEX"  ; DbVarComment="CONBOXZEXTENSION            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSMBRA"  ; DbVarComment="VOUSSOIRMAINBLOCKRADIUS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXMBRA"  ; DbVarComment="CONBOXMAINBLOCKRADIUS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUCRCYR"  ; DbVarComment="VOUSSCENTRRECTCUTYREACH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSRCXW"  ; DbVarComment="VOUSSRECTCUTXWID            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSCXOF"  ; DbVarComment="VOUSSCUTXOFFSET             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSCYPO"  ; DbVarComment="VOUSSOIRCUTYPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSCZPO"  ; DbVarComment="VOUSSOIRCUTZPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEAXTP"  ; DbVarComment="CONBOXEARXTOP               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEAYTP"  ; DbVarComment="CONBOXEARYTOP               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEAXBT"  ; DbVarComment="CONBOXEARXBOT               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBEAYBT"  ; DbVarComment="CONBOXEARYBOT               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECXOL"  ; DbVarComment="CONBOXEARCUTXOUTLOW         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECYOL"  ; DbVarComment="CONBOXEARCUTYOUTLOW         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECYIL"  ; DbVarComment="CONBOXEARCUTYINLOW          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECOHE"  ; DbVarComment="CONBOXEARCUTOUTHEIGHT       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECXOU"  ; DbVarComment="CONBOXEARCUTXOUTUP          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECYOU"  ; DbVarComment="CONBOXEARCUTYOUTUP          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECYIU"  ; DbVarComment="CONBOXEARCUTYINUP           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBECZPO"  ; DbVarComment="CONBOXEARCUTZPOS            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBIECZP"  ; DbVarComment="CONBOXINNEREARCUTZPOS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBCAZIN"  ; DbVarComment="CONBOXCAVITYZINTER          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VOUSSRAD"  ; DbVarComment="VOUSSOIRRADIUS              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBOXRAD"  ; DbVarComment="CONBOXRADIUS                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::AECT(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AECT";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "CRYOT1"      ; DbVarComment="CRYOT1           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOT2"      ; DbVarComment="CRYOT2           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOS1"      ; DbVarComment="CRYOS1           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOS2"      ; DbVarComment="CRYOS2           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOEDGE"    ; DbVarComment="CRYOEDGE         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOFLTY"    ; DbVarComment="FLATY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOR0"      ; DbVarComment="CRYOR0           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOZMIN"    ; DbVarComment="CRYOZMIN         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOZEXT"    ; DbVarComment="CRYOZEXT         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOTHI1"    ; DbVarComment="THICK1           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOTHI2"    ; DbVarComment="THICK2           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOTHI3"    ; DbVarComment="THICK3           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOTTU0"    ; DbVarComment="CRYOTTU0         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORTU0"    ; DbVarComment="CRYORTU0         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYORTU1"    ; DbVarComment="CRYORTU1         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYODPHT"    ; DbVarComment="DPHITU           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTL0"    ; DbVarComment="CRYOSTOPLENGTH0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTL1"    ; DbVarComment="CRYOSTOPLENGTH1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTH0"    ; DbVarComment="CRYOSTOPHEIGHT0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTW0"    ; DbVarComment="CRYOSTOPWIDTH0   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTW1"    ; DbVarComment="CRYOSTOPWIDTH1   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTTH"    ; DbVarComment="CRYOSTOPTHICKNESS" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTH1"    ; DbVarComment="CRYOSTOPHEIGHT1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTH2"    ; DbVarComment="CRYOSTOPHEIGHT2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSTPZ"    ; DbVarComment="CRYOSTOPPOSIZ    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSUPL"    ; DbVarComment="CRYOSUPL         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSUPH"    ; DbVarComment="CRYOSUPH         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSUPW"    ; DbVarComment="CRYOSUPW         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOSUPX"    ; DbVarComment="CRYOSUPX         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXZMOF"    ; DbVarComment="CONDBOXZMINOFFSET" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXZEXT"    ; DbVarComment="CONDBOXZEXT      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXTMIN"    ; DbVarComment="CONDBOXTMIN      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXTMAX"    ; DbVarComment="CONDBOXTMAX      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXTHIC"    ; DbVarComment="CONDBOXTHICK     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXEDGE"    ; DbVarComment="CONDBOXEDGE      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLZOF"    ; DbVarComment="SUPPLATZOFFSET   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLZEX"    ; DbVarComment="SUPPLATZEXT      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLTMI"    ; DbVarComment="SUPPLATTMIN      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLTMA"    ; DbVarComment="SUPPLATTMAX      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLTHZ"    ; DbVarComment="SUPPLATTHICKZ    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLTHT"    ; DbVarComment="SUPPLATTHICKT    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLRHO"    ; DbVarComment="SUPPLATRHOLE     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU1DZ"    ; DbVarComment="SERVTUR1DZ       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU1HE"    ; DbVarComment="SERVTUR1HEIGHT   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU1OW"    ; DbVarComment="SERVTUR1OUTWIDTH " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU1IW"    ; DbVarComment="SERVTUR1INNWIDTH " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU1ED"    ; DbVarComment="SERVTUR1EDGE     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU2DZ"    ; DbVarComment="SERVTUR2DZ       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU2HE"    ; DbVarComment="SERVTUR2HEIGHT   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU2OR"    ; DbVarComment="SERVTUR2OUTRAD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU2IR"    ; DbVarComment="SERVTUR2INNRAD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU3DZ"    ; DbVarComment="SERVTUR3DZ       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SRVTU3HE"    ; DbVarComment="SERVTUR3HEIGHT   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SIFITU"      ; DbVarComment="SIFITU           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COFITU"      ; DbVarComment="COFITU           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOZMAX"    ; DbVarComment="CRYOZMAX         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXZMIN"    ; DbVarComment="CONDBOXZMIN      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNBXZMAX"    ; DbVarComment="CONDBOXZMAX      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLZMI"    ; DbVarComment="SUPPLATZMIN      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLZMA"    ; DbVarComment="SUPPLATZMAX      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOZMOY"    ; DbVarComment="CRYOZMOY         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOALFA"    ; DbVarComment="CRYOALFA         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOBETA"    ; DbVarComment="CRYOBETA         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOT1P"     ; DbVarComment="CRYOT1P          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOT2P"     ; DbVarComment="CRYOT2P          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOS1P"     ; DbVarComment="CRYOS1P          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOS2P"     ; DbVarComment="CRYOS2P          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CRYOFLYP"    ; DbVarComment="FLATYP           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLZME"    ; DbVarComment="SUPPLATZMOY      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTEMP_0"     ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTEMP_1"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTEMP_2"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTEMP_0"     ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTEMP_1"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTEMP_2"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DTEMP01"     ; DbVarComment="DTEMP01          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTIMP_0"     ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTIMP_1"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XTIMP_2"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTIMP_0"     ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTIMP_1"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YTIMP_2"     ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DTIMP01"     ; DbVarComment="DTIMP01          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UTEMP61"     ; DbVarComment="UTEMP61          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UTEMP62"     ; DbVarComment="UTEMP62          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UTIMP61"     ; DbVarComment="UTIMP61          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UTIMP62"     ; DbVarComment="UTIMP62          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DTEMP"       ; DbVarComment="DTEMP            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATY_0"  ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATY_1"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATY_2"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATY_3"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATX_0"  ; DbVarComment="ARRAY            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATX_1"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATX_2"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SUPPLATX_3"  ; DbVarComment="                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "X1TEMP"      ; DbVarComment="X1TEMP           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "X0TEMP"      ; DbVarComment="X0TEMP           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::ECST(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "ECST";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "STOTHIC1"      ; DbVarComment="STOTHIC1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOTHIC2"      ; DbVarComment="STOTHIC2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOTHIC3"      ; DbVarComment="STOTHIC3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOTHIC4"      ; DbVarComment="STOTHIC4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOLENGT"      ; DbVarComment="STOLENGT  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOLENGA"      ; DbVarComment="STOLENGA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOXPREF"      ; DbVarComment="STOXPREF  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOYPREF"      ; DbVarComment="STOYPREF  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOHEIGH"      ; DbVarComment="STOHEIGH  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOHEIG1"      ; DbVarComment="STOHEIG1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOHEIG2"      ; DbVarComment="STOHEIG2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOHEIG3"      ; DbVarComment="STOHEIG3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOHEIG4"      ; DbVarComment="STOHEIG4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZBOT"      ; DbVarComment="STODZBOT  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZIN1"      ; DbVarComment="STODZIN1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZIN2"      ; DbVarComment="STODZIN2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZIN3"      ; DbVarComment="STODZIN3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZIN4"      ; DbVarComment="STODZIN4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZIN5"      ; DbVarComment="STODZIN5  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STODZTOP"      ; DbVarComment="STODZTOP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOANGLE"      ; DbVarComment="STOANGLE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOYPOS"       ; DbVarComment="STOYPOS   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOGECXZ"      ; DbVarComment="STOGECXZ  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOGECHE"      ; DbVarComment="STOGECHE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCLCZP"      ; DbVarComment="STOCLCZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOANGD2"      ; DbVarComment="STOANGD2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOANGD4"      ; DbVarComment="STOANGD4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOLENG1"      ; DbVarComment="STOLENG1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFXPOS"      ; DbVarComment="STOFXPOS  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOZPOS"       ; DbVarComment="STOZPOS   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEYP"       ; DbVarComment="STOUEYP   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOTEYP"       ; DbVarComment="STOTEYP   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOTEZP"       ; DbVarComment="STOTEZP   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXA"       ; DbVarComment="STOFLXA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYA"       ; DbVarComment="STOFLYA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXB"       ; DbVarComment="STOFLXB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYB"       ; DbVarComment="STOFLYB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXC"       ; DbVarComment="STOFLXC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYC"       ; DbVarComment="STOFLYC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXD"       ; DbVarComment="STOFLXD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYD"       ; DbVarComment="STOFLYD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXE"       ; DbVarComment="STOFLXE   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYE"       ; DbVarComment="STOFLYE   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXF"       ; DbVarComment="STOFLXF   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYF"       ; DbVarComment="STOFLYF   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXG"       ; DbVarComment="STOFLXG   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYG"       ; DbVarComment="STOFLYG   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXH"       ; DbVarComment="STOFLXH   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYH"       ; DbVarComment="STOFLYH   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLXI"       ; DbVarComment="STOFLXI   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOFLYI"       ; DbVarComment="STOFLYI   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXA"       ; DbVarComment="STOCOXA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYA"       ; DbVarComment="STOCOYA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXA1"      ; DbVarComment="STOCOXA1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYA1"      ; DbVarComment="STOCOYA1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXB"       ; DbVarComment="STOCOXB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYB"       ; DbVarComment="STOCOYB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXB1"      ; DbVarComment="STOCOXB1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYB1"      ; DbVarComment="STOCOYB1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXC"       ; DbVarComment="STOCOXC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYC"       ; DbVarComment="STOCOYC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXC1"      ; DbVarComment="STOCOXC1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYC1"      ; DbVarComment="STOCOYC1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXD"       ; DbVarComment="STOCOXD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYD"       ; DbVarComment="STOCOYD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXD1"      ; DbVarComment="STOCOXD1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYD1"      ; DbVarComment="STOCOYD1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXE"       ; DbVarComment="STOCOXE   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYE"       ; DbVarComment="STOCOYE   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXE1"      ; DbVarComment="STOCOXE1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYE1"      ; DbVarComment="STOCOYE1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXF"       ; DbVarComment="STOCOXF   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYF"       ; DbVarComment="STOCOYF   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXF1"      ; DbVarComment="STOCOXF1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYF1"      ; DbVarComment="STOCOYF1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXG"       ; DbVarComment="STOCOXG   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYG"       ; DbVarComment="STOCOYG   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXG1"      ; DbVarComment="STOCOXG1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYG1"      ; DbVarComment="STOCOYG1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXH"       ; DbVarComment="STOCOXH   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYH"       ; DbVarComment="STOCOYH   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXH1"      ; DbVarComment="STOCOXH1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYH1"      ; DbVarComment="STOCOYH1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXI"       ; DbVarComment="STOCOXI   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYI"       ; DbVarComment="STOCOYI   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOXI1"      ; DbVarComment="STOCOXI1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCOYI1"      ; DbVarComment="STOCOYI1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCO3DX"      ; DbVarComment="STOCO3DX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCO3DY"      ; DbVarComment="STOCO3DY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCO3DZ"      ; DbVarComment="STOCO3DZ  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCO3ZP"      ; DbVarComment="STOCO3ZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOCO3YP"      ; DbVarComment="STOCO3YP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEXA"       ; DbVarComment="STOUEXA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEYA"       ; DbVarComment="STOUEYA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEXB"       ; DbVarComment="STOUEXB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEYB"       ; DbVarComment="STOUEYB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEXC"       ; DbVarComment="STOUEXC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUEYC"       ; DbVarComment="STOUEYC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECXA"      ; DbVarComment="STOUECXA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECYA"      ; DbVarComment="STOUECYA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECXB"      ; DbVarComment="STOUECXB  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECYB"      ; DbVarComment="STOUECYB  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECZP"      ; DbVarComment="STOUECZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STOUECYP"      ; DbVarComment="STOUECYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::FEET(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "FEET";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "MAINPLXO"      ; DbVarComment="MAINPLATEXORIGIN                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLGXWI"      ; DbVarComment="MAINPLATE_GROUNDXWIDTH                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLRCDX"      ; DbVarComment="MAINPLATEROUNDCUTDX                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLDXEX"      ; DbVarComment="MAINPLATEDXEXTR                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLUPDX"      ; DbVarComment="MAINPLATEUPPERDX                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLUPXW"      ; DbVarComment="MAINPLATEUPPERXWIDTH                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLMIDX"      ; DbVarComment="MAINPLATEMIDDLEDX                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYO"      ; DbVarComment="MAINPLATEYORIGIN                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLHE"      ; DbVarComment="MAINPLATEHEIGHT                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLH1"      ; DbVarComment="MAINPLATEHEIGHT_INTERMEDIATE1           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLRCYP"      ; DbVarComment="MAINPLATEROUNDCUT_Y                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLSRCY"      ; DbVarComment="MAINPLATESECONDARYROUNDCUT_Y            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLUPDY"      ; DbVarComment="MAINPLATEUPPERDY                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLMIDY"      ; DbVarComment="MAINPLATEMIDDLEDY                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLPEHE"      ; DbVarComment="MAINPLATE_PEDESTALHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLDZ"      ; DbVarComment="MAINPLATEDZ                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLZSEP"      ; DbVarComment="MAINPLATEZSEP                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLRCRA"      ; DbVarComment="MAINPLATEROUNDCUT_R                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MNPLSRCR"      ; DbVarComment="MAINPLATESECONDARYROUNDCUT_R            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MINCPLXW"      ; DbVarComment="MINICONNPLATE_XWIDTH                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MINCPLYH"      ; DbVarComment="MINICONNPLATE_YHEIGHT                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MINCPLTZ"      ; DbVarComment="MINICONNPLATE_THETAZ                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MINCPLZL"      ; DbVarComment="MINICONNPLATE_ZLENGTH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLACPLXW"      ; DbVarComment="SLANTEDCONNPLATE_XWIDTH                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLACPLYH"      ; DbVarComment="SLANTEDCONNPLATE_YHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLACPLTZ"      ; DbVarComment="SLANTEDCONNPLATE_THETAZ                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLACPLZL"      ; DbVarComment="SLANTEDCONNPLATE_ZLENGTH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GRNDPLZL"      ; DbVarComment="GROUNDPLATE_ZLENGTH                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VCNPLXWI"      ; DbVarComment="VERTICALCONNPLATE_XWIDTH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VCNPLYHE"      ; DbVarComment="VERTICALCONNPLATE_YHEIGHT               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VCNPLDXE"      ; DbVarComment="VERTICALCONNPLATE_DX                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VCNPLDYE"      ; DbVarComment="VERTICALCONNPLATE_DY                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UCNPLXWI"      ; DbVarComment="UPPERCONNPLATE_XWIDTH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UCNPLYHE"      ; DbVarComment="UPPERCONNPLATE_YHEIGHT                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XWIDTH"        ; DbVarComment="XWIDTH                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UPPERHEY"      ; DbVarComment="RAILSUPPORTUPPER_YHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UPPERLEZ"      ; DbVarComment="RAILSUPPORTUPPER_ZLENGTH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LOWERHEY"      ; DbVarComment="RAILSUPPORTLOWER_YHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LOWERLEZ"      ; DbVarComment="RAILSUPPORTLOWER_ZLENGTH                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TOTALHEY"      ; DbVarComment="RAILSUPPORTTOTAL_YHEIGHT                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CENTRLEZ"      ; DbVarComment="RAILSUPPORTCENTRAL_ZLENGTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CENTRXTH"      ; DbVarComment="RAILSUPPORTCENTRAL_XTHICK               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MIDLLYTH"      ; DbVarComment="RAILSUPPORTMIDDLE_YTHICK                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VERTIZTH"      ; DbVarComment="RAILSUPPORTVERTICAL_ZTHICK              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MIDLLYPO"      ; DbVarComment="RAILSUPPORTMIDDLE_YPOS                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXTREHEY"      ; DbVarComment="RAILSUPPORTEXTR_YHEIGHT                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXTRELEZ"      ; DbVarComment="RAILSUPPORTEXTR_ZLENGTH                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CENTRHEY"      ; DbVarComment="RAILSUPPORTCENTRAL_YHEIGHT              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOXW"      ; DbVarComment="CONNFEETVOUSS_XWIDTH                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOYH"      ; DbVarComment="CONNFEETVOUSS_YHEIGHT                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOZL"      ; DbVarComment="CONNFEETVOUSS_ZLENGTH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOXT"      ; DbVarComment="CONNFEETVOUSS_XTHICK                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOYT"      ; DbVarComment="CONNFEETVOUSS_YTHICK                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOZT"      ; DbVarComment="CONNFEETVOUSS_ZTHICK                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPXWT"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_TOTAL      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSXO"      ; DbVarComment="GIRDER12_INNERPLATESMALLOFFSET_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSLX"      ; DbVarComment="GIRDER12_INNERPLATESMALL2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLLX"      ; DbVarComment="GIRDER12_INNERPLATELARGE2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLSX"      ; DbVarComment="GIRDER12_INNERPLATELARGE2SMALL_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12RPXWI"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XWIDTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPXW1"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_INTER1     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPX12"      ; DbVarComment="GIRDER12_LATERALPLATE_XWIDTH_INTER1INTER" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPXWI"      ; DbVarComment="GIRDER12_INNERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UMPWI"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_XWIDTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPWI"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_XWIDTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12YORIG"      ; DbVarComment="GIRDER12_YORIGIN                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPYHT"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_TOTAL     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPYH1"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_INTER1    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPYH2"      ; DbVarComment="GIRDER12_LATERALPLATE_YHEIGHT_INTER2    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPYTH"      ; DbVarComment="GIRDER12_LOWERPLATE_YTHICK              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UPYTH"      ; DbVarComment="GIRDER12_UPPERPLATE_YTHICK              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12RPYDI"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YDIST       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UMPYH"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_YHEIGHT         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UPZLE"      ; DbVarComment="GIRDER12_UPPERPLATE_ZLENGTH             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UMPZL"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_ZLENGTH         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BPZLE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_ZLENGTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12LPZLE"      ; DbVarComment="GIRDER12_LATERALPLATE_ZLENGTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12RPZLE"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_ZLENGTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPZL"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_ZLENGTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23LPXWT"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_TOTAL      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPSXO"      ; DbVarComment="GIRDER23_INNERPLATESMALLOFFSET_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPSLX"      ; DbVarComment="GIRDER23_INNERPLATESMALL2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPLLX"      ; DbVarComment="GIRDER23_INNERPLATELARGE2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPLSX"      ; DbVarComment="GIRDER23_INNERPLATELARGE2SMALL_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23RPXWI"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XWIDTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23LPXW1"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_INTER1     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23LPX12"      ; DbVarComment="GIRDER23_LATERALPLATE_XWIDTH_INTER1INTER" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPXWI"      ; DbVarComment="GIRDER23_INNERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23UMPWI"      ; DbVarComment="GIRDER23_UPPERMINIPLATE_XWIDTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23SMPWI"      ; DbVarComment="GIRDER23_SIDEMINIPLATE_XWIDTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34LPXWT"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_TOTAL      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPSXO"      ; DbVarComment="GIRDER34_INNERPLATESMALLOFFSET_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPSLX"      ; DbVarComment="GIRDER34_INNERPLATESMALL2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPLLX"      ; DbVarComment="GIRDER34_INNERPLATELARGE2LARGE_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPLSX"      ; DbVarComment="GIRDER34_INNERPLATELARGE2SMALL_XDIST    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34RPXWI"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XWIDTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34LPXW1"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_INTER1     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34LPX12"      ; DbVarComment="GIRDER34_LATERALPLATE_XWIDTH_INTER1INTER" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPXWI"      ; DbVarComment="GIRDER34_INNERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34UMPWI"      ; DbVarComment="GIRDER34_UPPERMINIPLATE_XWIDTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34SMPWI"      ; DbVarComment="GIRDER34_SIDEMINIPLATE_XWIDTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPHEIG"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPRCRA"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_R            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPRCDX"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_DX           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPRCYP"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_Y            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMCPYHE"      ; DbVarComment="EXTREMITYMINICONNPLATE_YHEIGHT          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMCPZLE"      ; DbVarComment="EXTREMITYMINICONNPLATE_ZLENGTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXGPZLEN"      ; DbVarComment="EXTREMITYGROUNDPLATE_ZLENGTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXSCPZLE"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATE_ZLENGTH       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSZOFF"      ; DbVarComment="EXTREMITYRAILSUPPORT_ZOFFSET            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPHEI1"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE1 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPHEI2"      ; DbVarComment="EXTREMITYMAINPLATE_HEIGHT_INTERMEDIATE2 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXWID"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXWI1"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH_INTER1        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXWI2"      ; DbVarComment="EXTREMITYMAINPLATE_XWIDTH_INTER2        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYHI1"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER1       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYHI2"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER2       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYHI3"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER3       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYHI4"      ; DbVarComment="EXTREMITYMAINPLATE_YHEIGHT_INTER4       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXDI1"      ; DbVarComment="EXTREMITYMAINPLATE_XDIST_INTER1         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXDI2"      ; DbVarComment="EXTREMITYMAINPLATE_XDIST_INTER2         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYDI2"      ; DbVarComment="EXTREMITYMAINPLATE_YDIST_INTER1         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLDX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_DX           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLDY"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_DY           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPZSEP"      ; DbVarComment="EXTREMITYMAINPLATE_ZSEP                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXSPRCRA"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATEROUNDCUT_RADIUS" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXSPRCOF"      ; DbVarComment="EXTREMITYSLANTEDCONNPLATEROUNDCUT_OFFSET" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPTHI"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_THICKNESS    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPXEX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XEXT         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPHEI"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_HEIGHT       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPYCU"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_CUTY         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPXCU"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_CUTX         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCFVXWI"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_XWIDTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCFVYHE"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_YHEIGHT          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCFVZLE"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_ZLENGTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSUZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTUPPER_ZLENGTH       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSLDZL"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_DZLENGTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSTYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTTOTAL_YHEIGHT       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSLYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_YHEIGHT       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSEYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_YHEIGHT        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSCYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_YHEIGHT     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSCZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_ZLENGTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_ZLENGTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSCXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_XWIDTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_XWIDTH       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVXWI"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_XWIDTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVZI1"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_ZPOS_INTER1" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSEZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZLENGTH        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSEZOF"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZOFFSET        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1DE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_DEPTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1WI"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_WIDTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1XE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_DUMXEXT        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC2XE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_DUMXEXT        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC2Z1"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZOFFSET1       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC2Z2"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZOFFSET2       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCDE"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DEPTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCWI"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_WIDTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCZO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_ZOFFSET   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCY1"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DUMY1     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCY2"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_DUMY2     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STDFOOXP"      ; DbVarComment="STANDARDFOOT_XPOS                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STDFOOYP"      ; DbVarComment="STANDARDFOOT_YPOS                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSFEE1"      ; DbVarComment="ZPOSFEE1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSFEE2"      ; DbVarComment="ZPOSFEE2                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSFEE3"      ; DbVarComment="ZPOSFEE3                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSFEE4"      ; DbVarComment="ZPOSFEE4                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSFEE5"      ; DbVarComment="ZPOSFEE5                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIRDYHEI"      ; DbVarComment="YHEIGHTGIRDER                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIRDXPOS"      ; DbVarComment="XPOSGIRDER                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLAHCEC"      ; DbVarComment="FEETMAINPLATE_HCEC                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DUMZ"          ; DbVarComment="DUMZ                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "DUMPHI"        ; DbVarComment="DUMPHI                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXA"      ; DbVarComment="MAINPLXA                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYA"      ; DbVarComment="MAINPLYA                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXB"      ; DbVarComment="MAINPLXB                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYB"      ; DbVarComment="MAINPLYB                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXC"      ; DbVarComment="MAINPLXC                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYC"      ; DbVarComment="MAINPLYC                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAPLRCXC"      ; DbVarComment="MAINPLATEROUNDCUT_XC                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAPLRCYC"      ; DbVarComment="MAINPLATEROUNDCUT_YC                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLXC3"      ; DbVarComment="MAIPLXC3                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLYC3"      ; DbVarComment="MAIPLYC3                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLXEXTR"      ; DbVarComment="FEETMAINPLATE_XEXTR                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLYEXTR"      ; DbVarComment="FEETMAINPLATE_YEXTR                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLREXTR"      ; DbVarComment="FEETMAINPLATE_REXTR                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLXC2"      ; DbVarComment="MAIPLXC2                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLYC2"      ; DbVarComment="MAIPLYC2                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLXCEC1"      ; DbVarComment="MPLXCEC1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLYCEC1"      ; DbVarComment="MPLYCEC1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLXCEC2"      ; DbVarComment="MPLXCEC2                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MPLYCEC2"      ; DbVarComment="MPLYCEC2                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "A"             ; DbVarComment="A                                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "B"             ; DbVarComment="B                                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "APRIME"        ; DbVarComment="APRIME                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BPRIME"        ; DbVarComment="BPRIME                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ASECONDE"      ; DbVarComment="ASECONDE                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BSECONDE"      ; DbVarComment="BSECONDE                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXD"      ; DbVarComment="MAINPLXD                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYD"      ; DbVarComment="MAINPLYD                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXE"      ; DbVarComment="MAINPLXE                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYE"      ; DbVarComment="MAINPLYE                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLXC1"      ; DbVarComment="MAIPLXC1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAIPLYC1"      ; DbVarComment="MAIPLYC1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXF"      ; DbVarComment="MAINPLXF                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYF"      ; DbVarComment="MAINPLYF                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXG"      ; DbVarComment="MAINPLXG                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYG"      ; DbVarComment="MAINPLYG                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXH"      ; DbVarComment="MAINPLXH                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYH"      ; DbVarComment="MAINPLYH                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXI"      ; DbVarComment="MAINPLXI                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYI"      ; DbVarComment="MAINPLYI                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLXJ"      ; DbVarComment="MAINPLXJ                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MAINPLYJ"      ; DbVarComment="MAINPLYJ                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ALPHA1"        ; DbVarComment="ALPHA1                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COAL1"         ; DbVarComment="COAL1                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SIAL1"         ; DbVarComment="SIAL1                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MICPDIAG"      ; DbVarComment="MINICONNPLATE_DIAG                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MICPXPOS"      ; DbVarComment="MINICONNPLATE_X                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MICPYPOS"      ; DbVarComment="MINICONNPLATE_Y                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ALPHA2"        ; DbVarComment="ALPHA2                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COAL2"         ; DbVarComment="COAL2                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SIAL2"         ; DbVarComment="SIAL2                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLCPDIAG"      ; DbVarComment="SLANTEDCONNPLATE_DIAG                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLCPXPOS"      ; DbVarComment="SLANTEDCONNPLATE_X                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SLCPYPOS"      ; DbVarComment="SLANTEDCONNPLATE_Y                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GRNPXWID"      ; DbVarComment="GROUNDPLATE_XWIDTH                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GRNPYHEI"      ; DbVarComment="GROUNDPLATE_YHEIGHT                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GRNPXPOS"      ; DbVarComment="GROUNDPLATE_X                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GRNPYPOS"      ; DbVarComment="GROUNDPLATE_Y                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VECPXPOS"      ; DbVarComment="VERTICALCONNPLATE_X                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VECPYPOS"      ; DbVarComment="VERTICALCONNPLATE_Y                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "VECPZLEN"      ; DbVarComment="VERTICALCONNPLATE_ZLENGTH               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UPCPXPOS"      ; DbVarComment="UPPERCONNPLATE_X                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UPCPYPOS"      ; DbVarComment="UPPERCONNPLATE_Y                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "UPCPZLEN"      ; DbVarComment="UPPERCONNPLATE_ZLENGTH                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "MIDYPOSI"      ; DbVarComment="RAILSUPPORTMIDDLE_Y                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "POSX"          ; DbVarComment="POSX                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "POSY"          ; DbVarComment="POSY                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOXP"      ; DbVarComment="CONNFEETVOUSS_X                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CNFEVOYP"      ; DbVarComment="CONNFEETVOUSS_Y                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXA"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XA                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXB"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XB                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXC"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XC                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXD"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XD                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XE                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPXF"      ; DbVarComment="GIRDER12_BOTTOMPLATE_XF                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYA"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YA                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYB"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YB                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYC"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YC                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYD"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YD                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYE"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YE                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12BOPYF"      ; DbVarComment="GIRDER12_BOTTOMPLATE_YF                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPZLE"      ; DbVarComment="GIRDER12_INNERPLATE_ZLENGTH             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLYH"      ; DbVarComment="GIRDER12_INNERPLATELARGE_YHEIGHT        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLYP"      ; DbVarComment="GIRDER12_INNERPLATELARGE_YPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSYH"      ; DbVarComment="GIRDER12_INNERPLATESMALL_YHEIGHT        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSYP"      ; DbVarComment="GIRDER12_INNERPLATESMALL_YPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSX1"      ; DbVarComment="GIRDER12_INNERPLATESMALL1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLX1"      ; DbVarComment="GIRDER12_INNERPLATELARGE1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPLX2"      ; DbVarComment="GIRDER12_INNERPLATELARGE2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12IPSX2"      ; DbVarComment="GIRDER12_INNERPLATESMALL2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UPXWI"      ; DbVarComment="GIRDER12_UPPERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UPYHE"      ; DbVarComment="GIRDER12_UPPERPLATE_YHEIGHT             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UPYPO"      ; DbVarComment="GIRDER12_UPPERPLATE_YPOS                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPXA"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPXB"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPXC"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPXD"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_XD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPYA"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPYB"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPYC"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPYD"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_YD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12REPZP"      ; DbVarComment="GIRDER12_REINFORCEMENTPLATE_ZPOS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UMPYP"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_YPOS            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12UMPXP"      ; DbVarComment="GIRDER12_UPPERMINIPLATE_XPOS            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPYH"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_YHEIGHT          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPYP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_YPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPXP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_XPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G12SMPZP"      ; DbVarComment="GIRDER12_SIDEMINIPLATE_ZPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXA"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XA                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXB"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XB                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXC"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XC                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXD"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XD                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXE"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XE                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23BOPXF"      ; DbVarComment="GIRDER23_BOTTOMPLATE_XF                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPSX1"      ; DbVarComment="GIRDER23_INNERPLATESMALL1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPLX1"      ; DbVarComment="GIRDER23_INNERPLATELARGE1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPLX2"      ; DbVarComment="GIRDER23_INNERPLATELARGE2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23IPSX2"      ; DbVarComment="GIRDER23_INNERPLATESMALL2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23UPXWI"      ; DbVarComment="GIRDER23_UPPERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23UPYHE"      ; DbVarComment="GIRDER23_UPPERPLATE_YHEIGHT             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPXA"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPXB"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPXC"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPXD"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_XD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPYA"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPYB"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPYC"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23REPYD"      ; DbVarComment="GIRDER23_REINFORCEMENTPLATE_YD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23UMPXP"      ; DbVarComment="GIRDER23_UPPERMINIPLATE_XPOS            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G23SMPXP"      ; DbVarComment="GIRDER23_SIDEMINIPLATE_XPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXA"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XA                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXB"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XB                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXC"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XC                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXD"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XD                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXE"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XE                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34BOPXF"      ; DbVarComment="GIRDER34_BOTTOMPLATE_XF                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPSX1"      ; DbVarComment="GIRDER34_INNERPLATESMALL1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPLX1"      ; DbVarComment="GIRDER34_INNERPLATELARGE1_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPLX2"      ; DbVarComment="GIRDER34_INNERPLATELARGE2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34IPSX2"      ; DbVarComment="GIRDER34_INNERPLATESMALL2_XPOS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34UPXWI"      ; DbVarComment="GIRDER34_UPPERPLATE_XWIDTH              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34UPYHE"      ; DbVarComment="GIRDER34_UPPERPLATE_YHEIGHT             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPXA"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPXB"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPXC"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPXD"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_XD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPYA"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YA          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPYB"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YB          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPYC"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YC          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34REPYD"      ; DbVarComment="GIRDER34_REINFORCEMENTPLATE_YD          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34UMPXP"      ; DbVarComment="GIRDER34_UPPERMINIPLATE_XPOS            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "G34SMPXP"      ; DbVarComment="GIRDER34_SIDEMINIPLATE_XPOS             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "STDFVOYP"      ; DbVarComment="STANDARDFOOTVOUSSOIR_YPOS               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPRCXC"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_XC           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPRCYC"      ; DbVarComment="EXTREMITYMAINPLATEROUNDCUT_YC           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXC3"      ; DbVarComment="EXTREMITYMAINPLATE_XC3                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYC3"      ; DbVarComment="EXTREMITYMAINPLATE_YC3                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXC4"      ; DbVarComment="EXTREMITYMAINPLATE_XC4                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYC4"      ; DbVarComment="EXTREMITYMAINPLATE_YC4                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXC5"      ; DbVarComment="EXTREMITYMAINPLATE_XC5                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYC5"      ; DbVarComment="EXTREMITYMAINPLATE_YC5                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "A2"            ; DbVarComment="A2                                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "B2"            ; DbVarComment="B2                                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "A2PRIME"       ; DbVarComment="A2PRIME                                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "B2PRIME"       ; DbVarComment="B2PRIME                                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAXD"      ; DbVarComment="EXTREMITYMAINPLATE_XD                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAYD"      ; DbVarComment="EXTREMITYMAINPLATE_YD                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPC2C1"      ; DbVarComment="EXTREMITYMAINPLATE_LENGTHC2C1           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXC1"      ; DbVarComment="EXTREMITYMAINPLATE_XC1                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYC1"      ; DbVarComment="EXTREMITYMAINPLATE_YC1                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYC1A"      ; DbVarComment="EXTREMITYMAINPLATE_YC1A                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXC1A"      ; DbVarComment="EXTREMITYMAINPLATE_XC1A                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPXC1B"      ; DbVarComment="EXTREMITYMAINPLATE_XC1B                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPYC1B"      ; DbVarComment="EXTREMITYMAINPLATE_YC1B                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ALPHA3"        ; DbVarComment="ALPHA3                                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "COAL3"         ; DbVarComment="COAL3                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SIAL3"         ; DbVarComment="SIAL3                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMCPDIA"      ; DbVarComment="EXTREMITYMINICONNPLATE_DIAG             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMCPXPO"      ; DbVarComment="EXTREMITYMINICONNPLATE_X                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMCPYPO"      ; DbVarComment="EXTREMITYMINICONNPLATE_Y                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAXF"      ; DbVarComment="EXTREMITYMAINPLATE_XF                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAYF"      ; DbVarComment="EXTREMITYMAINPLATE_YF                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF1"      ; DbVarComment="EXTREMITYMAINPLATE_XF1                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF1"      ; DbVarComment="EXTREMITYMAINPLATE_YF1                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF2"      ; DbVarComment="EXTREMITYMAINPLATE_XF2                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF2"      ; DbVarComment="EXTREMITYMAINPLATE_YF2                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF3"      ; DbVarComment="EXTREMITYMAINPLATE_YF3                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF3"      ; DbVarComment="EXTREMITYMAINPLATE_XF3                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF4"      ; DbVarComment="EXTREMITYMAINPLATE_XF4                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF4"      ; DbVarComment="EXTREMITYMAINPLATE_YF4                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF5"      ; DbVarComment="EXTREMITYMAINPLATE_XF5                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF5"      ; DbVarComment="EXTREMITYMAINPLATE_YF5                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF6"      ; DbVarComment="EXTREMITYMAINPLATE_XF6                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF6"      ; DbVarComment="EXTREMITYMAINPLATE_YF6                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLXF7"      ; DbVarComment="EXTREMITYMAINPLATE_XF7                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLYF7"      ; DbVarComment="EXTREMITYMAINPLATE_YF7                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAXI"      ; DbVarComment="EXTREMITYMAINPLATE_XI                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAXH"      ; DbVarComment="EXTREMITYMAINPLATE_XH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXMPLAYH"      ; DbVarComment="EXTREMITYMAINPLATE_YH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXUCPXPO"      ; DbVarComment="EXTREMITYUPPERCONNPLATE_X               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXUCPZLE"      ; DbVarComment="EXTREMITYUPPERCONNPLATE_ZLENGTH         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPZLE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_ZLENGTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXA"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XA           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYA"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YA           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXB"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XB           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYB"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YB           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXC"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XC           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYC"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YC           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXD"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XD           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYD"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YD           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XE           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYE"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YE           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXF"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XF           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYF"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YF           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLXP"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_XPOS         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLYP"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_YPOS         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLAX"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_X            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXVCPLAY"      ; DbVarComment="EXTREMITYVERTICALCONNPLATE_Y            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCNFEVX"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_X                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXCNFEVY"      ; DbVarComment="EXTREMITYCONNFEETVOUSS_Y                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSLZLE"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_ZLENGTH       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVYHE"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_YHEIGHT    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMXPO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_XPOS         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVXPO"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_XPOS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSEYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_Y              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSUYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTUPPER_Y             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSLYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTLOWER_Y             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSCYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTCENTRAL_Y           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVYPO"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL_Y          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVZP1"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL1_ZPOS      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSVZP2"      ; DbVarComment="EXTREMITYRAILSUPPORTVERTICAL2_ZPOS      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMZPO"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_ZPOS         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSEZPO"      ; DbVarComment="EXTREMITYRAILSUPPORTEXTR_ZPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1YE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_YEXT           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC2YE"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_YEXT           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_XPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC2XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT2_XPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC1ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT1_ZPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC3XP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT3_XPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC3ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT3_ZPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC4Z1"      ; DbVarComment="EXRSC4Z1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSC4ZP"      ; DbVarComment="EXTREMITYRAILSUPPORTCUT4_ZPOS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCXA"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XA        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCYA"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YA        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCXB"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XB        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCYB"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YB        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCXC"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_XC        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCYC"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLECUT_YC        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCXP"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_CUT_XPOS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCZ1"      ; DbVarComment="EXRSMCZ1                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSMCZP"      ; DbVarComment="EXTREMITYRAILSUPPORTMIDDLE_CUT_ZPOS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPOSX"      ; DbVarComment="EXTREMITYRAILSUPPORT_X                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPOSY"      ; DbVarComment="EXTREMITYRAILSUPPORT_Y                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPOSZ"      ; DbVarComment="EXTREMITYRAILSUPPORT_Z                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPXAP"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_PLUS_X   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPXAM"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_MINUS_X  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EXRSPYAS"      ; DbVarComment="EXTREMITYRAILSUPPORT_ASSEMBLED_Y        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIR12ZPO"      ; DbVarComment="GIR12ZPO                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIR12YPO"      ; DbVarComment="GIR12YPO                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIR23ZPO"      ; DbVarComment="GIR23ZPO                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "GIR34ZPO"      ; DbVarComment="GIR34ZPO                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::GENV(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "GENV";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "PI"          ; DbVarComment="PI     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "EPS"         ; DbVarComment="EPS    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PIS180"      ; DbVarComment="PIS180 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SI5"         ; DbVarComment="SI5    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CO5"         ; DbVarComment="CO5    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA15"        ; DbVarComment="TA15   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA20"        ; DbVarComment="TA20   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SI225"       ; DbVarComment="SI225  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CO225"       ; DbVarComment="CO225  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA225"       ; DbVarComment="TA225  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA375"       ; DbVarComment="TA375  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SI45"        ; DbVarComment="SI45   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CO45"        ; DbVarComment="CO45   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA45"        ; DbVarComment="TA45   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SI525"       ; DbVarComment="SI525  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CO525"       ; DbVarComment="CO525  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA55"        ; DbVarComment="TA55   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SI675"       ; DbVarComment="SI675  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CO675"       ; DbVarComment="CO675  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA675"       ; DbVarComment="TA675  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TA77"        ; DbVarComment="TA77   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::JDSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JDSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "XYZREFPT"           ; DbVarComment="XYZREFERENCEPOINT         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ROTA180D"           ; DbVarComment="180DEGROTATION            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "NOMIZPOS"           ; DbVarComment="NOMINAL_ZPOS              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FDINNRAD"           ; DbVarComment="FRONTDISK_INNERRADIUS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FDOUTRAD"           ; DbVarComment="FRONTDISK_OUTERRADIUS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FDTHICKN"           ; DbVarComment="FRONTDISK_THICKNESS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BDINNRAD"           ; DbVarComment="BACKDISK_INNERRADIUS      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BDOUTRAD"           ; DbVarComment="BACKDISK_OUTERRADIUS      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BDTHICKN"           ; DbVarComment="BACKDISK_THICKNESS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCINRA"           ; DbVarComment="BRASSCONE_INNERRADIUS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCOURA"           ; DbVarComment="BRASSCONE_OUTERRADIUS     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCTHI1"           ; DbVarComment="BRASSCONE_THICKNESS_INTER1" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCTHIC"           ; DbVarComment="BRASSCONE_THICKNESS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCSLAN"           ; DbVarComment="BRASSCONE_SLOPEANGLE      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCSLLE"           ; DbVarComment="BRASSCONE_SLOPELENGTH     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CALCTHIC"           ; DbVarComment="LEADCONE_THICKNESS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CALCLENG"           ; DbVarComment="LEADCONE_LENGTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CALCWIDT"           ; DbVarComment="LEADCONE_WIDTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAPCTHIC"           ; DbVarComment="POLYCONE_THICKNESS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAPCLENG"           ; DbVarComment="POLYCONE_LENGTH           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAPCWIDT"           ; DbVarComment="POLYCONE_WIDTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOPRAN"           ; DbVarComment="CONE_PROJECTIVEANGLE      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_0"         ; DbVarComment="ARRAY                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_1"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_2"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_3"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_4"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_5"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_6"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_7"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_8"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_9"         ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_10"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_11"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_12"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_13"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_14"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGZ_15"        ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CACOANGX"           ; DbVarComment="CONEASSEMBLY_ANGX         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBEINRA"           ; DbVarComment="TUBE_INNERRADIUS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBEOUR1"           ; DbVarComment="TUBE_OUTERRADIUS1         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBEOUR2"           ; DbVarComment="TUBE_OUTERRADIUS2         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBESLEN"           ; DbVarComment="TUBE_SHORTLENGTH          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBELENG"           ; DbVarComment="TUBE_LENGTH               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGINR1"           ; DbVarComment="PLUG_INNERRADIUS1         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGOURA"           ; DbVarComment="PLUG_OUTERRADIUS          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGLENG"           ; DbVarComment="PLUG_LENGTH               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGTIAN"           ; DbVarComment="PLUG_TILTANGLE            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SWHINNRA"           ; DbVarComment="SMALLWHEELHUB_INNERRADIUS " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SWHOUTR1"           ; DbVarComment="SMALLWHEELHUB_OUTERRADIUS1" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SWHOUTR2"           ; DbVarComment="SMALLWHEELHUB_OUTERRADIUS2" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SWHLENGT"           ; DbVarComment="SMALLWHEELHUB_LENGTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "FDZPOSIT"           ; DbVarComment="FRONTDISK_Z               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "BDZPOSIT"           ; DbVarComment="BACKDISK_Z                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAZPOSIT"           ; DbVarComment="CONEASSEMBLY_Z            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TUBEZPOS"           ; DbVarComment="TUBE_Z                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGZPOS"           ; DbVarComment="PLUG_Z                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "SWHZPOSI"           ; DbVarComment="SMALLWHEELHUB_Z           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCOUR1"           ; DbVarComment="BRASSCONE_OUTERRADIUS1    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCTHI2"           ; DbVarComment="BRASSCONE_THICKNESS2      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CABCOUR2"           ; DbVarComment="BRASSCONE_OUTERRADIUS2    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAXBPOS"           ; DbVarComment="LEADCONEASSEMBLY_XB       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAYBPOS"           ; DbVarComment="LEADCONEASSEMBLY_YB       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAXAPOS"           ; DbVarComment="LEADCONEASSEMBLY_XA       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAYAPOS"           ; DbVarComment="LEADCONEASSEMBLY_YA       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAXBPOS"           ; DbVarComment="POLYCONEASSEMBLY_XB       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAYBPOS"           ; DbVarComment="POLYCONEASSEMBLY_YB       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAXAPOS"           ; DbVarComment="POLYCONEASSEMBLY_XA       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAYAPOS"           ; DbVarComment="POLYCONEASSEMBLY_YA       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CALCRADI"           ; DbVarComment="LEADCONE_RADIUS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CALCPOSZ"           ; DbVarComment="LEADCONE_POSZ             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAPCRADI"           ; DbVarComment="POLYCONE_RADIUS           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "CAPCPOSZ"           ; DbVarComment="POLYCONE_POSZ             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "I1"                 ; DbVarComment="I1                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_0"            ; DbVarComment="ARRAY                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_1"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_2"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_3"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_4"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_5"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_6"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_7"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_8"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_9"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_10"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_11"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_12"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_13"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_14"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPX_15"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_0"            ; DbVarComment="ARRAY                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_1"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_2"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_3"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_4"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_5"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_6"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_7"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_8"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_9"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_10"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_11"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_12"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_13"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_14"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "LCAPY_15"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "I2"                 ; DbVarComment="I2                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_0"            ; DbVarComment="ARRAY                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_1"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_2"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_3"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_4"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_5"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_6"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_7"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_8"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_9"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_10"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_11"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_12"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_13"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_14"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPX_15"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_0"            ; DbVarComment="ARRAY                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_1"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_2"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_3"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_4"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_5"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_6"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_7"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_8"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_9"            ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_10"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_11"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_12"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_13"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_14"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PCAPY_15"           ; DbVarComment="                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGINR2"           ; DbVarComment="PLUG_INNERRADIUS2         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::JFSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JFSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "XYZREF"             ; DbVarComment="XYZREF                                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ROT180"             ; DbVarComment="ROT180                                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBBLENG"           ; DbVarComment="AFRAME_BOTTOMBAR_LENGTH                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBHEIGH"           ; DbVarComment="AFRAME_BAR_HEIGHT                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFRWIDTH"           ; DbVarComment="AFRAME_WIDTH                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBTHIC1"           ; DbVarComment="AFRAME_BAR_THICKNESS1                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBTHIC2"           ; DbVarComment="AFRAME_BAR_THICKNESS2                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLANGLE"           ; DbVarComment="AFRAME_LEG_ANGLE                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSLENGT"           ; DbVarComment="AFRAME_SPACER_LENGTH                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSHEIGH"           ; DbVarComment="AFRAME_SPACER_HEIGHT                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBBX1"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_X1             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBBX2"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_X2             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELENG"           ; DbVarComment="AFRAME_WEB_ELEMENT_LENGTH                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBLX1"           ; DbVarComment="AFRAME_WEB_ELEMENT_LEG_DX1                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBLX2"           ; DbVarComment="AFRAME_WEB_ELEMENT_LEG_DX2                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFTHICK"           ; DbVarComment="AFRAME_FLANGE_THICKNESS                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFLENGT"           ; DbVarComment="AFRAME_FLANGE_LENGTH                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPLENG"           ; DbVarComment="AFRAME_TOPPLATE_LENGTH                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPHEIG"           ; DbVarComment="AFRAME_TOPPLATE_HEIGHT                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWHEIGH"           ; DbVarComment="AFRAME_WEDGE_HEIGHT                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEXTEN"           ; DbVarComment="AFRAME_WEDGE_EXTENSION                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWTHICK"           ; DbVarComment="AFRAME_WEDGE_THICKNESS                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWZOFFS"           ; DbVarComment="AFRAME_WEDGE_ZOFFSET                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEELEN"           ; DbVarComment="AFRAME_FEET_LENGTH                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETHI"           ; DbVarComment="AFRAME_FEET_THICKNESS                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEESLE"           ; DbVarComment="AFRAME_FEET_SHORTLENGTH                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEEHEI"           ; DbVarComment="AFRAME_FEET_HEIGHT                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEEEXT"           ; DbVarComment="AFRAME_FEET_X_EXTENSION                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTOTHEI"           ; DbVarComment="AFRAME_TOTAL_HEIGHT                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFECTCLE"           ; DbVarComment="CLEARANCE_ECT_AFRAME                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ECTNOMIZ"           ; DbVarComment="ECT_NOMINAL_ZPOS                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFYPOSIT"           ; DbVarComment="AFRAME_YPOS                                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFZPOSIT"           ; DbVarComment="AFRAME_ZPOS                                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBLHPXA"           ; DbVarComment="AFBLHPXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBLHPYA"           ; DbVarComment="AFBLHPYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBLHPYB"           ; DbVarComment="AFBLHPYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBLHPXB"           ; DbVarComment="AFBLHPXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBBVPXA"           ; DbVarComment="AFBBVPXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBBVPYA"           ; DbVarComment="AFBBVPYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBBVPYB"           ; DbVarComment="AFBBVPYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBBVPXB"           ; DbVarComment="AFBBVPXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBUHPXA"           ; DbVarComment="AFBUHPXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBUHPYA"           ; DbVarComment="AFBUHPYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBUHPYB"           ; DbVarComment="AFBUHPYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFBUHPXB"           ; DbVarComment="AFBUHPXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPXA"           ; DbVarComment="AFLLHPXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPYA"           ; DbVarComment="AFLLHPYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPXB"           ; DbVarComment="AFLLHPXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPYB"           ; DbVarComment="AFLLHPYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPXC"           ; DbVarComment="AFLLHPXC                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPYC"           ; DbVarComment="AFLLHPYC                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPXD"           ; DbVarComment="AFLLHPXD                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLLHPYD"           ; DbVarComment="AFLLHPYD                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPXA"            ; DbVarComment="AFLVPXA                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPYA"            ; DbVarComment="AFLVPYA                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPXB"            ; DbVarComment="AFLVPXB                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPYB"            ; DbVarComment="AFLVPYB                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPXC"            ; DbVarComment="AFLVPXC                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPYC"            ; DbVarComment="AFLVPYC                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPXE"            ; DbVarComment="AFLVPXE                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPYE"            ; DbVarComment="AFLVPYE                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPYD"            ; DbVarComment="AFLVPYD                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLVPXD"            ; DbVarComment="AFLVPXD                                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPXA"           ; DbVarComment="AFLUHPXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPYA"           ; DbVarComment="AFLUHPYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPXB"           ; DbVarComment="AFLUHPXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPYB"           ; DbVarComment="AFLUHPYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPXC"           ; DbVarComment="AFLUHPXC                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPYC"           ; DbVarComment="AFLUHPYC                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPXD"           ; DbVarComment="AFLUHPXD                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFLUHPYD"           ; DbVarComment="AFLUHPYD                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPLAXA"           ; DbVarComment="AFTPLAXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPLAYA"           ; DbVarComment="AFTPLAYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPLAXB"           ; DbVarComment="AFTPLAXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFTPLAYB"           ; DbVarComment="AFTPLAYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSPACXA"           ; DbVarComment="AFSPACXA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSPACYA"           ; DbVarComment="AFSPACYA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSPACXB"           ; DbVarComment="AFSPACXB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFSPACYB"           ; DbVarComment="AFSPACYB                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBWID"           ; DbVarComment="AFRAME_WEB_ELEMENT_WIDTH                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBHEI"           ; DbVarComment="AFRAME_WEB_ELEMENT_HEIGHT                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBBBY"           ; DbVarComment="AFRAME_WEB_ELEMENT_BOTTOMBAR_Y              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWEBZPO"           ; DbVarComment="AFRAME_WEB_ELEMENT_Z                        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXI1"           ; DbVarComment="AFWELXI1                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYI1"           ; DbVarComment="AFWELYI1                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXP1"           ; DbVarComment="AFWELXP1                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYP1"           ; DbVarComment="AFWELYP1                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXI2"           ; DbVarComment="AFWELXI2                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYI2"           ; DbVarComment="AFWELYI2                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXP2"           ; DbVarComment="AFWELXP2                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYP2"           ; DbVarComment="AFWELYP2                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXI3"           ; DbVarComment="AFWELXI3                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYI3"           ; DbVarComment="AFWELYI3                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELXP3"           ; DbVarComment="AFWELXP3                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWELYP3"           ; DbVarComment="AFWELYP3                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFWIDTH"           ; DbVarComment="AFRAME_FLANGE_WIDTH                         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFZPOSI"           ; DbVarComment="AFRAME_FLANGE_Z                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFPOSX1"           ; DbVarComment="AFRAME_FLANGE_X1                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFPOSY1"           ; DbVarComment="AFRAME_FLANGE_Y1                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFPOSX2"           ; DbVarComment="AFRAME_FLANGE_X2                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFPOSY2"           ; DbVarComment="AFRAME_FLANGE_Y2                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSXA"           ; DbVarComment="AFRAME_WEDGE_XA                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSYA"           ; DbVarComment="AFRAME_WEDGE_YA                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSXB"           ; DbVarComment="AFRAME_WEDGE_XB                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSYB"           ; DbVarComment="AFRAME_WEDGE_YB                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSXC"           ; DbVarComment="AFRAME_WEDGE_XC                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSYC"           ; DbVarComment="AFRAME_WEDGE_YC                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSXD"           ; DbVarComment="AFRAME_WEDGE_XD                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSYD"           ; DbVarComment="AFRAME_WEDGE_YD                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSX0"           ; DbVarComment="AFRAME_WEDGE_X0                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSY0"           ; DbVarComment="AFRAME_WEDGE_Y0                             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSIX"           ; DbVarComment="AFRAME_WEDGE_X                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSIY"           ; DbVarComment="AFRAME_WEDGE_Y                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFWPOSIZ"           ; DbVarComment="AFRAME_WEDGE_Z                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXA"           ; DbVarComment="AFRAME_FEET_XA                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYA"           ; DbVarComment="AFRAME_FEET_YA                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXB"           ; DbVarComment="AFRAME_FEET_XB                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYB"           ; DbVarComment="AFRAME_FEET_YB                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXC"           ; DbVarComment="AFRAME_FEET_XC                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYC"           ; DbVarComment="AFRAME_FEET_YC                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXD"           ; DbVarComment="AFRAME_FEET_XD                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYD"           ; DbVarComment="AFRAME_FEET_YD                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXE"           ; DbVarComment="AFRAME_FEET_XE                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYE"           ; DbVarComment="AFRAME_FEET_YE                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETXF"           ; DbVarComment="AFRAME_FEET_XF                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETYF"           ; DbVarComment="AFRAME_FEET_YF                              " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFFEETPX"           ; DbVarComment="AFRAME_FEET_X                               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFASPOSY"           ; DbVarComment="AFRAME_ASSEMBLY_Y                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFASPOSZ"           ; DbVarComment="AFRAME_ASSEMBLY_Z                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCLEN"           ; DbVarComment="JFCMAINCYLINDER_LENGTH                      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCIR1"           ; DbVarComment="JFCMAINCYLINDER_INNERRADIUS1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCOUR"           ; DbVarComment="JFCMAINCYLINDER_OUTERRADIUS                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCHSL"           ; DbVarComment="JFCMAINCYLINDER_HOLESLOPE                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCZOF"           ; DbVarComment="JFCMAINCYLINDER_ZOFFSET                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGLENG"           ; DbVarComment="PLUG_LENGTH                                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGINRA"           ; DbVarComment="PLUG_INNERRADIUS                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGOURA"           ; DbVarComment="PLUG_OUTERRADIUS                            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCMRA"           ; DbVarComment="JFSOCTOGON_MAINRADIUS                       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCLEN"           ; DbVarComment="JFSOCTOGON_LENGTH                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCZOF"           ; DbVarComment="JFSOCTOGON_ZOFFSET                          " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFOCONLE"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_LENGTH      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFOCONWI"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_WIDTH       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFOCONHE"           ; DbVarComment="AFRAME_TO_JFSOCTOGON_CONNECTION_HEIGHT      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E1ORA"           ; DbVarComment="TX1E1ORA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E1IRA"           ; DbVarComment="TX1E1IRA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E1ILE"           ; DbVarComment="TX1E1ILE                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E2ORA"           ; DbVarComment="TX1E2ORA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E2IRA"           ; DbVarComment="TX1E2IRA                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E2ILE"           ; DbVarComment="TX1E2ILE                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1STZOF"           ; DbVarComment="TX1STZOF                                    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCIR2"           ; DbVarComment="JFCMAINCYLINDER_INNERRADIUS2                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFCMCZPO"           ; DbVarComment="JFCMAINCYLINDER_Z                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCZPO"           ; DbVarComment="JFSOCTOGON_Z                                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E1ZPO"           ; DbVarComment="TX1STM_ELEMENT1_Z                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "TX1E2ZPO"           ; DbVarComment="TX1STM_ELEMENT2_Z                           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLUGZPOS"           ; DbVarComment="PLUG_POSZ                                   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFMCCONY"           ; DbVarComment="AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Y      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "AFMCCONZ"           ; DbVarComment="AFRAME_TO_JFCMAINCYLINDER_CONNECTION_Z      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCTXA"           ; DbVarComment="JFSOCTOGON_XA                               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCTYA"           ; DbVarComment="JFSOCTOGON_YA                               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCTXB"           ; DbVarComment="JFSOCTOGON_XB                               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "JFSOCTYB"           ; DbVarComment="JFSOCTOGON_YB                               " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::JTSH(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "JTSH";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "OPLENGTH"             ; DbVarComment="OUTERPLUGS_LENGTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "OPINNRAD"             ; DbVarComment="OUTERPLUGS_INNERRADIUS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "OPOUTRAD"             ; DbVarComment="OUTERPLUGS_OUTERRADIUS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPLENGTH"             ; DbVarComment="INNERPLUGS_LENGTH            " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPINNRA1"             ; DbVarComment="INNERPLUGS_INNERRADIUS1      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPOUTRAD"             ; DbVarComment="INNERPLUGS_OUTERRADIUS       " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPTILTAN"             ; DbVarComment="INNERPLUGS_TILTANGLE         " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPZOFFSE"             ; DbVarComment="INNERPLUGS_ZOFFSET           " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLENGTH"             ; DbVarComment="POLYRINGS_LENGTH             " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRINNRAD"             ; DbVarComment="POLYRINGS_INNERRADIUS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PROUTRAD"             ; DbVarComment="POLYRINGS_OUTERRADIUS        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PREXTTAN"             ; DbVarComment="POLYRINGS_EXTENSION_TILTANGLE" ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRIPEXLE"             ; DbVarComment="POLYRINGS_IPEXTENSION_LENGTH " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "ZPOSITIO"             ; DbVarComment="SHIELD_Z                     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "YROTATIO"             ; DbVarComment="SHIELD_ROTY                  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPINNRA2"             ; DbVarComment="INNERPLUGS_INNERRADIUS2      " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PREXTZPO"             ; DbVarComment="POLYRINGS_EXTENSION_Z        " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "OPZPOSI1"             ; DbVarComment="OUTERPLUGS_Z1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "OPZPOSI2"             ; DbVarComment="OUTERPLUGS_Z2                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPZPOSI1"             ; DbVarComment="INNERPLUGS_Z1                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "IPZPOSI2"             ; DbVarComment="INNERPLUGS_Z2                " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRZPOSI1"             ; DbVarComment="POLYRINGS_Z1                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRZPOSI2"             ; DbVarComment="POLYRINGS_Z2                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRZPOSI3"             ; DbVarComment="POLYRINGS_Z3                 " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}
void AmdcDbSvcMakerFromRDB::MBAP(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "MBAP";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSet+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "P12S1DZ1"             ; DbVarComment="1353 P12S1DZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P12S1DZ2"             ; DbVarComment="1354 P12S1DZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P23S1DZ1"             ; DbVarComment="1355 P23S1DZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P23S1DZ2"             ; DbVarComment="1356 P23S1DZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P34S1DZ1"             ; DbVarComment="1357 P34S1DZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P34S1DZ2"             ; DbVarComment="1358 P34S1DZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3DZ"              ; DbVarComment="1359 P14S3DZ   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3DX"              ; DbVarComment="1360 P14S3DX   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLAS1DX1"             ; DbVarComment="1361 PLAS1DX1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLAS1DX2"             ; DbVarComment="1362 PLAS1DX2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFLHE"                ; DbVarComment="1363 PFLHE     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS01POSX"             ; DbVarComment="1364 PS01POSX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS01POSY"             ; DbVarComment="1365 PS01POSY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS11POSX"             ; DbVarComment="1366 PS11POSX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS11POSY"             ; DbVarComment="1367 PS11POSY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS14POSX"             ; DbVarComment="1368 PS14POSX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS14POSY"             ; DbVarComment="1369 PS14POSY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PS14ANGZ"             ; DbVarComment="1370 PS14ANGZ  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P12POSZ"              ; DbVarComment="1371 P12POSZ   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P23POSZ"              ; DbVarComment="1372 P23POSZ   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P34POSZ"              ; DbVarComment="1373 P34POSZ   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "XYZREF"               ; DbVarComment="1374 XYZREF    " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL12XA"              ; DbVarComment="1375 PFL12XA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL23XA"              ; DbVarComment="1376 PFL23XA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL34XA"              ; DbVarComment="1377 PFL34XA   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFLYA"                ; DbVarComment="1378 PFLYA     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL12XB"              ; DbVarComment="1379 PFL12XB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL23XB"              ; DbVarComment="1380 PFL23XB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL34XB"              ; DbVarComment="1381 PFL34XB   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFLYB"                ; DbVarComment="1382 PFLYB     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL12XC"              ; DbVarComment="1383 PFL12XC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL23XC"              ; DbVarComment="1384 PFL23XC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL34XC"              ; DbVarComment="1385 PFL34XC   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFLYC"                ; DbVarComment="1386 PFLYC     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL12XD"              ; DbVarComment="1387 PFL12XD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL23XD"              ; DbVarComment="1388 PFL23XD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFL34XD"              ; DbVarComment="1389 PFL34XD   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PFLYD"                ; DbVarComment="1390 PFLYD     " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRARO"             ; DbVarComment="1391 PLATRARO  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRARI"             ; DbVarComment="1392 PLATRARI  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAHE"             ; DbVarComment="1393 PLATRAHE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RAZE"             ; DbVarComment="1394 PL12RAZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RAZE"             ; DbVarComment="1395 PL23RAZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RAZE"             ; DbVarComment="1396 PL34RAZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRHYP"             ; DbVarComment="1397 PLATRHYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATBTLE"             ; DbVarComment="1398 PLATBTLE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01DX"             ; DbVarComment="1399 PRLS01DX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11DX"             ; DbVarComment="1400 PRLS11DX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01DY"             ; DbVarComment="1401 PRLS01DY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11DY"             ; DbVarComment="1402 PRLS11DY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DY1"             ; DbVarComment="1403 PSSS3DY1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DY2"             ; DbVarComment="1404 PSSS3DY2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DX1"             ; DbVarComment="1405 PSSS3DX1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DX2"             ; DbVarComment="1406 PSSS3DX2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DX3"             ; DbVarComment="1407 PSSS3DX3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3AN"              ; DbVarComment="1408 PSSS3AN   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP1"             ; DbVarComment="1409 PSSS3ZP1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP2"             ; DbVarComment="1410 PSSS3ZP2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP3"             ; DbVarComment="1411 PSSS3ZP3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP4"             ; DbVarComment="1412 PSSS3ZP4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP5"             ; DbVarComment="1413 PSSS3ZP5  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3ZP6"             ; DbVarComment="1414 PSSS3ZP6  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAZ1"             ; DbVarComment="1415 PLATRAZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAY1"             ; DbVarComment="1416 PLATRAY1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAZ2"             ; DbVarComment="1417 PLATRAZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAY2"             ; DbVarComment="1418 PLATRAY2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RAZ3"             ; DbVarComment="1419 PL12RAZ3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RAZ3"             ; DbVarComment="1420 PL23RAZ3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RAZ3"             ; DbVarComment="1421 PL34RAZ3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAY3"             ; DbVarComment="1422 PLATRAY3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RAZ4"             ; DbVarComment="1423 PL12RAZ4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RAZ4"             ; DbVarComment="1424 PL23RAZ4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RAZ4"             ; DbVarComment="1425 PL34RAZ4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAY4"             ; DbVarComment="1426 PLATRAY4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RAZ0"             ; DbVarComment="1427 PL12RAZ0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RAZ0"             ; DbVarComment="1428 PL23RAZ0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RAZ0"             ; DbVarComment="1429 PL34RAZ0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRAY0"             ; DbVarComment="1430 PLATRAY0  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RHZP"             ; DbVarComment="1431 PL12RHZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RHZP"             ; DbVarComment="1432 PL23RHZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RHZP"             ; DbVarComment="1433 PL34RHZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRVZE"             ; DbVarComment="1434 PLATRVZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATRVYP"             ; DbVarComment="1435 PLATRVYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATCRYP"             ; DbVarComment="1436 PLATCRYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12CRZ1"             ; DbVarComment="1437 PL12CRZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12CRZ2"             ; DbVarComment="1438 PL12CRZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23CRZ1"             ; DbVarComment="1439 PL23CRZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23CRZ2"             ; DbVarComment="1440 PL23CRZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34CRZ1"             ; DbVarComment="1441 PL34CRZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34CRZ2"             ; DbVarComment="1442 PL34CRZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12CRAN"             ; DbVarComment="1443 PL12CRAN  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23CRAN"             ; DbVarComment="1444 PL23CRAN  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34CRAN"             ; DbVarComment="1445 PL34CRAN  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12CRLE"             ; DbVarComment="1446 PL12CRLE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23CRLE"             ; DbVarComment="1447 PL23CRLE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34CRLE"             ; DbVarComment="1448 PL34CRLE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLAT2RAX"             ; DbVarComment="1449 PLAT2RAX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12BTZ1"             ; DbVarComment="1450 PL12BTZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23BTZ1"             ; DbVarComment="1451 PL23BTZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34BTZ1"             ; DbVarComment="1452 PL34BTZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12BTZ2"             ; DbVarComment="1453 PL12BTZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23BTZ2"             ; DbVarComment="1454 PL23BTZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34BTZ2"             ; DbVarComment="1455 PL34BTZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATBTXP"             ; DbVarComment="1456 PLATBTXP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01XP"             ; DbVarComment="1457 PRLS01XP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11XP"             ; DbVarComment="1458 PRLS11XP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01YP"             ; DbVarComment="1459 PRLS01YP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11YP"             ; DbVarComment="1460 PRLS11YP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RLZ1"             ; DbVarComment="1461 PL12RLZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RLZ1"             ; DbVarComment="1462 PL23RLZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RLZ1"             ; DbVarComment="1463 PL34RLZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12RLZ2"             ; DbVarComment="1464 PL12RLZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23RLZ2"             ; DbVarComment="1465 PL23RLZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34RLZ2"             ; DbVarComment="1466 PL34RLZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01AN"             ; DbVarComment="1467 PRLS01AN  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11AN"             ; DbVarComment="1468 PRLS11AN  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS01LE"             ; DbVarComment="1469 PRLS01LE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRLS11LE"             ; DbVarComment="1470 PRLS11LE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATBSXP"             ; DbVarComment="1471 PLATBSXP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATBTLS"             ; DbVarComment="1472 PLATBTLS  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL12FRZP"             ; DbVarComment="1473 PL12FRZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL23FRZP"             ; DbVarComment="1474 PL23FRZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL34FRZP"             ; DbVarComment="1475 PL34FRZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATFRYP"             ; DbVarComment="1476 PLATFRYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PLATFRXP"             ; DbVarComment="1477 PLATFRXP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DYT"             ; DbVarComment="1478 PSSS3DYT  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3DXT"             ; DbVarComment="1479 PSSS3DXT  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X0"              ; DbVarComment="1480 PSSS3X0   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y0"              ; DbVarComment="1481 PSSS3Y0   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X1"              ; DbVarComment="1482 PSSS3X1   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y1"              ; DbVarComment="1483 PSSS3Y1   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X2"              ; DbVarComment="1484 PSSS3X2   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y2"              ; DbVarComment="1485 PSSS3Y2   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X3"              ; DbVarComment="1486 PSSS3X3   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y3"              ; DbVarComment="1487 PSSS3Y3   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X4"              ; DbVarComment="1488 PSSS3X4   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y4"              ; DbVarComment="1489 PSSS3Y4   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X5"              ; DbVarComment="1490 PSSS3X5   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y5"              ; DbVarComment="1491 PSSS3Y5   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3X6"              ; DbVarComment="1492 PSSS3X6   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSS3Y6"              ; DbVarComment="1493 PSSS3Y6   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSRAXP"              ; DbVarComment="1494 PSSRAXP   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSRAYP"              ; DbVarComment="1495 PSSRAYP   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP1"              ; DbVarComment="1496 PSSZRP1   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP2"              ; DbVarComment="1497 PSSZRP2   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP3"              ; DbVarComment="1498 PSSZRP3   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP4"              ; DbVarComment="1499 PSSZRP4   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP5"              ; DbVarComment="1500 PSSZRP5   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PSSZRP6"              ; DbVarComment="1501 PSSZRP6   " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP1"             ; DbVarComment="1502 PRHS3ZP1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP2"             ; DbVarComment="1503 PRHS3ZP2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP3"             ; DbVarComment="1504 PRHS3ZP3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP4"             ; DbVarComment="1505 PRHS3ZP4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP5"             ; DbVarComment="1506 PRHS3ZP5  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP6"             ; DbVarComment="1507 PRHS3ZP6  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PRHS3ZP7"             ; DbVarComment="1508 PRHS3ZP7  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RAZE"             ; DbVarComment="1509 PL14RAZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RAZP"             ; DbVarComment="1510 PL14RAZP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RAY1"             ; DbVarComment="1511 PL14RAY1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RAY2"             ; DbVarComment="1512 PL14RAY2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RVZE"             ; DbVarComment="1513 PL14RVZE  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14RVYP"             ; DbVarComment="1514 PL14RVYP  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRY1"             ; DbVarComment="1515 PL14CRY1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRY2"             ; DbVarComment="1516 PL14CRY2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P12S3CRA"             ; DbVarComment="1517 P12S3CRA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P12S3CRL"             ; DbVarComment="1518 P12S3CRL  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P23S3CRA"             ; DbVarComment="1519 P23S3CRA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P23S3CRL"             ; DbVarComment="1520 P23S3CRL  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P34S3CRA"             ; DbVarComment="1521 P34S3CRA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P34S3CRL"             ; DbVarComment="1522 P34S3CRL  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ1"             ; DbVarComment="1523 PL14CRZ1  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ2"             ; DbVarComment="1524 PL14CRZ2  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ3"             ; DbVarComment="1525 PL14CRZ3  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ4"             ; DbVarComment="1526 PL14CRZ4  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ5"             ; DbVarComment="1527 PL14CRZ5  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ6"             ; DbVarComment="1528 PL14CRZ6  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ7"             ; DbVarComment="1529 PL14CRZ7  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ8"             ; DbVarComment="1530 PL14CRZ8  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZ9"             ; DbVarComment="1531 PL14CRZ9  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "PL14CRZA"             ; DbVarComment="1532 PL14CRZA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3BFL"             ; DbVarComment="1533 P14S3BFL  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3BFA"             ; DbVarComment="1534 P14S3BFA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3BFX"             ; DbVarComment="1535 P14S3BFX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3BFY"             ; DbVarComment="1536 P14S3BFY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3FLA"             ; DbVarComment="1537 P14S3FLA  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3FLX"             ; DbVarComment="1538 P14S3FLX  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3FLY"             ; DbVarComment="1539 P14S3FLY  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);
    DbVar = "P14S3FLZ"             ; DbVarComment="1540 P14S3FLZ  " ; dDbVal = (*it)->getFloat(DbVar) ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::AgddXMLVariables(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "AgddXMLVariables";
//  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
  int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//  int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
//int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::string NameOfTheSetKAPITAL = NameOfTheSet ;
  std::transform(NameOfTheSet.begin(), NameOfTheSet.end(), NameOfTheSetKAPITAL.begin(), (int(*)(int)) toupper);

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSetKAPITAL+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "NameOfVariable"  ; DbVarComment="Name Of Variable"  ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)              ;

    DbVar = "ValueOfVariable" ; DbVarComment="Value Of Variable" ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthCM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

void AmdcDbSvcMakerFromRDB::HwSwIdMapping(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "HwSwIdMapping";
//   int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
//double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::string NameOfTheSetKAPITAL = NameOfTheSet ;
  std::transform(NameOfTheSet.begin(), NameOfTheSet.end(), NameOfTheSetKAPITAL.begin(), (int(*)(int)) toupper);

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSetKAPITAL+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "HARDNAME"   ; DbVarComment="Hard name"   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "SOFTNAME"   ; DbVarComment="Soft name"   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal);

    DbVar = "SOFTOCTANT" ; DbVarComment="Soft octant" ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    DbVar = "SOFTIZ"     ; DbVarComment="Soft Iz"     ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}


void AmdcDbSvcMakerFromRDB::XtomoData(IRDBAccessSvc* pIRDBAccessSvc,AmdcDbSvc* pAmdcDbSvc){

  std::string NameOfTheSet = "XtomoData";
  int LocalEpsLengthMM = GetEpsLengthMM(NameOfTheSet) ;
//   int LocalEpsLengthCM = GetEpsLengthCM(NameOfTheSet) ;
//   int LocalEpsAngle    = GetEpsAngle   (NameOfTheSet) ;

  const IRDBRecordset* pIRDBRecordset = pIRDBAccessSvc->getRecordset(NameOfTheSet,m_detectorKey,m_detectorNode);
  if (pIRDBRecordset->size()==0){
    std::cout << "AmdcDbSvcMakerFromRDB: No " << NameOfTheSet << " in pIRDBAccessSvc " << std::endl;
    std::cout << " for m_detectorKey  " << m_detectorKey  << std::endl;
    std::cout << " and m_detectorNode " << m_detectorNode << std::endl;
    return;
  } 

  std::string DbVar	   = "";
  std::string DbVarComment = "";
  int	      iDbVal	   = 0 ;
  double      dDbVal	   = 0.;
  std::string sDbVal	   = ""; 

  AmdcDbRecordset* pAmdcDbRecordset = new AmdcDbRecordset();

  std::string NameOfTheSetKAPITAL = NameOfTheSet ;
  std::transform(NameOfTheSet.begin(), NameOfTheSet.end(), NameOfTheSetKAPITAL.begin(), (int(*)(int)) toupper);

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    std::string StrDbId = NameOfTheSetKAPITAL+"_DATA_ID" ;
    long DbId	  = (*it)->getLong(StrDbId);    
 
    AmdcDbRecord* pAmdcDbRecord = new AmdcDbRecord(DbId,NameOfTheSet);

    DbVar = "XtomoChberName"  ; DbVarComment="XtomoChberName"   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoSite"       ; DbVarComment="XtomoSite     "   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoSiteId"     ; DbVarComment="XtomoSiteId   "   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoTime"       ; DbVarComment="XtomoTime     "   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ; 
    DbVar = "XtomoPassed"     ; DbVarComment="XtomoPassed   "   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoSide"       ; DbVarComment="XtomoSide     "   ; sDbVal = (*it)->getString(DbVar) ; pAmdcDbRecord->addString(DbVar,DbVarComment,sDbVal)                 ;
    DbVar = "XtomoNberTube1"  ; DbVarComment="XtomoNberTube1"   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberTube2"  ; DbVarComment="XtomoNberTube2"   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberML"     ; DbVarComment="XtomoNberML   "   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoNberLayer"  ; DbVarComment="XtomoNberLayer"   ; iDbVal = (*it)->getInt(DbVar)    ; pAmdcDbRecord->addInt(DbVar,DbVarComment,iDbVal)                    ;
    DbVar = "XtomoML1NYtub"   ; DbVarComment="XtomoML1NYtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NZtub"   ; DbVarComment="XtomoML1NZtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDely"   ; DbVarComment="XtomoML1NDely "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDelz"   ; DbVarComment="XtomoML1NDelz "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NDela"   ; DbVarComment="XtomoML1NDela "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NYpit"   ; DbVarComment="XtomoML1NYpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1NZpit"   ; DbVarComment="XtomoML1NZpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PYtub"   ; DbVarComment="XtomoML1PYtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PZtub"   ; DbVarComment="XtomoML1PZtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDely"   ; DbVarComment="XtomoML1PDely "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDelz"   ; DbVarComment="XtomoML1PDelz "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PDela"   ; DbVarComment="XtomoML1PDela "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PYpit"   ; DbVarComment="XtomoML1PYpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML1PZpit"   ; DbVarComment="XtomoML1PZpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NYtub"   ; DbVarComment="XtomoML2NYtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NZtub"   ; DbVarComment="XtomoML2NZtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDely"   ; DbVarComment="XtomoML2NDely "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDelz"   ; DbVarComment="XtomoML2NDelz "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NDela"   ; DbVarComment="XtomoML2NDela "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NYpit"   ; DbVarComment="XtomoML2NYpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2NZpit"   ; DbVarComment="XtomoML2NZpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PYtub"   ; DbVarComment="XtomoML2PYtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PZtub"   ; DbVarComment="XtomoML2PZtub "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDely"   ; DbVarComment="XtomoML2PDely "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDelz"   ; DbVarComment="XtomoML2PDelz "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PDela"   ; DbVarComment="XtomoML2PDela "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PYpit"   ; DbVarComment="XtomoML2PYpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);
    DbVar = "XtomoML2PZpit"   ; DbVarComment="XtomoML2PZpit "   ; dDbVal = (*it)->getFloat(DbVar)  ; pAmdcDbRecord->addDouble(DbVar,DbVarComment,dDbVal,LocalEpsLengthMM);

    pAmdcDbRecordset->addIRDBRecord(pAmdcDbRecord);

  }

  pAmdcDbSvc->addIRDBRecordset(NameOfTheSet,pAmdcDbRecordset);

}

