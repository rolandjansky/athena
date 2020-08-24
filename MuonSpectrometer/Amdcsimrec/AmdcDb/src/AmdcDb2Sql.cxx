/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "RDBAccessSvc/IRDBAccessSvc.h"
#include "RDBAccessSvc/IRDBRecord.h"
#include "RDBAccessSvc/IRDBRecordset.h"

/////////////////////////////////////////////////////////
#include "AmdcDb/AmdcDb2Sql.h"

#include "AmdcDb/AmdcDbRecord.h"

AmdcDb2Sql::AmdcDb2Sql(){ 

 m_AsNewTableOn = 0 ;
 m_UseKeysOn    = 1 ;

}

AmdcDb2Sql::~AmdcDb2Sql(){}

void AmdcDb2Sql::SetAsNewTableOn(int AsNewTableOn){ m_AsNewTableOn  = AsNewTableOn ;}
void AmdcDb2Sql::SetUseKeysOn(int UseKeysOn){ m_UseKeysOn  = UseKeysOn ;}

void AmdcDb2Sql::DoIt(std::string TagFileName,std::string tag,IRDBAccessSvc* pIRDBAccessSvc){

  DoSql(TagFileName,"AMDC",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"AGDD",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ATYP",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ACUT",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WRPC",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"AWLN",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WTGC",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"GGLN",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ATLN",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WCSC",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"DBAM",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ASMP",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WDED",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WLBI",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WCRO",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WCMI",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WCHV",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WSUP",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WSPA",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"WMDT",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ALIN",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ALMN",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"APTP",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ASZT",tag,pIRDBAccessSvc);
  DoSql(TagFileName,"ISZT",tag,pIRDBAccessSvc);
 
  DoSql(TagFileName,"HwSwIdMapping",tag,pIRDBAccessSvc);

  DoSql(TagFileName,"XtomoData",tag,pIRDBAccessSvc);

}

void AmdcDb2Sql::DoSql(std::string TagFileName,std::string NameOfTheSet,std::string tag, IRDBAccessSvc* pIRDBAccessSvc) {

  std::ofstream OutFile;
  std::string FileName = TagFileName + ".";
  FileName = FileName+ NameOfTheSet;
  FileName = FileName + ".PREsql";
  OutFile.open(FileName.c_str());
  OutFile << setiosflags(std::ios::fixed);

  IRDBRecordset_ptr pIRDBRecordset = pIRDBAccessSvc->getRecordsetPtr(NameOfTheSet,tag);
  if (pIRDBRecordset->size()==0){
    OutFile << "No " << NameOfTheSet << " in AmdcDb " << std::endl;
    return;
  } 

  std::vector<IRDBRecord*>::const_iterator it = pIRDBRecordset->begin();
  const AmdcDbRecord* pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
  if (pAmdcDbRecord == 0){
    OutFile << "No way to cast in AmdcDbRecord for " << NameOfTheSet << std::endl;
    return;
  } 
  int ThereIsABLOB = 0 ;
  if (pAmdcDbRecord->ThereIsABLOB() == 1) ThereIsABLOB = 1 ;
  
  if ( m_AsNewTableOn == 1) {
  if ( ThereIsABLOB == 1 ){
    OutFile << "PROMPT 07_R.01_01/"<< NameOfTheSet <<".sql" << std::endl;
  }
  }
  
  if ( m_AsNewTableOn == 1) {
  OutFile << "SET echo OFF;" << std::endl;
  OutFile << "set linesize 132;" << std::endl;
  OutFile << "drop   table "<< NameOfTheSet <<"_data2tag cascade constraints;" << std::endl;
  OutFile << "drop   table "<< NameOfTheSet <<"_data cascade constraints;" << std::endl;
  }


  if ( m_AsNewTableOn == 1) {
  OutFile << "create table "<< NameOfTheSet <<"_data (" << std::endl;
  OutFile << "  "<< NameOfTheSet <<"_data_id   number(10)," << std::endl;
  }
  
  it = pIRDBRecordset->begin();
  pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
  if (pAmdcDbRecord == 0){
    OutFile << "No way to cast in AmdcDbRecord for " << NameOfTheSet << std::endl;
    return;
  } 
  std::vector< std::string> VariableList = pAmdcDbRecord->getVariableList();
  if ( m_AsNewTableOn == 1) {
  int ItemTot = VariableList.size() ;
  for (int Item=0 ; Item<ItemTot ; Item++){
    std::string LaQueue = ",";
    int Itemtest = Item + 1;
    int ItemTot2 = VariableList.size() ;
    if ( Itemtest == ItemTot2 )LaQueue = "";
    std::string DbVar = VariableList[Item] ;
    int DbVarType = pAmdcDbRecord->getVariableType(DbVar) ;
    if ( DbVarType == 1 )  OutFile << DbVar << "       "<< "number(10)"  << LaQueue << std::endl;
    if ( DbVarType == 2 )  OutFile << DbVar << "       "<< "float(63)"   << LaQueue << std::endl;
    if ( DbVarType == 3 )  OutFile << DbVar << "       "<< "varchar2(4)" << LaQueue << std::endl;
    if ( DbVarType == 5 )  OutFile << "data CLOB default empty_clob()"   << LaQueue << std::endl;
  }
  OutFile << ") ;" << std::endl;
  OutFile << "" << std::endl;
  }
  
  
  if ( m_AsNewTableOn == 1) {
  OutFile << "alter table "<< NameOfTheSet <<"_data add constraint "<< NameOfTheSet <<"_data_pk" 					                   << std::endl;
  OutFile << "primary key ("<< NameOfTheSet <<"_data_id);"								                                   << std::endl;
  OutFile << ""												                                                   << std::endl;
  OutFile << "create table "<< NameOfTheSet <<"_data2tag ("								                                   << std::endl;
  OutFile << "  "<< NameOfTheSet <<"_vers      varchar2(255),"								                                   << std::endl;
  OutFile << "  "<< NameOfTheSet <<"_data_id   number(10)"								                                   << std::endl;
  OutFile << ") ;"											                                                   << std::endl;
  OutFile << ""												                                                   << std::endl;
  OutFile << "alter table "<< NameOfTheSet <<"_data2tag add constraint "<< NameOfTheSet <<"_data2tag_pk" 				                   << std::endl;
  OutFile << "primary key ("<< NameOfTheSet <<"_vers, "<< NameOfTheSet <<"_data_id);"							                   << std::endl;
  OutFile << "alter table "<< NameOfTheSet <<"_data2tag add constraint "<< NameOfTheSet <<"_data2tag_vers_fk"				                   << std::endl;
  OutFile << "foreign key ("<< NameOfTheSet <<"_vers) references hvs_nodevers (vers_name);"				                                   << std::endl;
  OutFile << "alter table "<< NameOfTheSet <<"_data2tag add constraint "<< NameOfTheSet <<"_data2tag_data_fk"				                   << std::endl;
  OutFile << "foreign key ("<< NameOfTheSet <<"_data_id) references "<< NameOfTheSet <<"_data ("<< NameOfTheSet <<"_data_id);"				   << std::endl;
  OutFile << ""												                                                   << std::endl;
  OutFile << "delete from hvs_relation where CHILD_NODENAME = '"<< NameOfTheSet <<"';"					                                   << std::endl;
  OutFile << "delete from hvs_nodevers where VERS_NAME = 'MuonSpectrometer-07';" 			                                                   << std::endl;
  OutFile << "delete from hvs_nodevers where NODE_NAME = '"<< NameOfTheSet <<"';"					                                   << std::endl;
  OutFile << "delete from hvs_node     where NODE_NAME = '"<< NameOfTheSet <<"';"					                                   << std::endl;
  OutFile << ""												                                                   << std::endl;
  OutFile << "insert into hvs_node     values ('"<< NameOfTheSet <<"','MuonSpectrometer', '');"				                                   << std::endl;
  OutFile << "insert into hvs_nodevers values ('"<< NameOfTheSet <<"', '"<< NameOfTheSet <<"-07', '');"					                   << std::endl;
  OutFile << "insert into hvs_nodevers values ('MuonSpectrometer','MuonSpectrometer-07','');"		                                                   << std::endl;
  OutFile << "insert into hvs_relation values ('MuonSpectrometer','MuonSpectrometer-07','"<< NameOfTheSet <<"','"<< NameOfTheSet <<"-07');"                << std::endl;
  }
  

  int ElementKounter = -1;
  it = pIRDBRecordset->begin();
  for ( ; it<pIRDBRecordset->end(); it++){
    pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
    if (pAmdcDbRecord == 0){
      OutFile << "No way to cast in AmdcDbRecord for " << NameOfTheSet << std::endl;
      return;
    } 
    
    ElementKounter = ElementKounter + 1;
    
    OutFile << "insert into "<< NameOfTheSet <<"_data (" << std::endl;
    OutFile << ""<< NameOfTheSet <<"_data_id,"	         << std::endl;

    VariableList = pAmdcDbRecord->getVariableList();
    int ItemTot3 = VariableList.size() ;
    for (int Item=0 ; Item<ItemTot3 ; Item++){
      std::string DbVar = VariableList[Item] ;
      int DbVarType = pAmdcDbRecord->getVariableType(DbVar) ;
      std::string LaQueue = ",";
      int Itemtest = Item + 1;
      int ItemTot4 = VariableList.size() ;
      if ( Itemtest == ItemTot4 )LaQueue = "";
      if (DbVarType != 5){
        OutFile << DbVar << LaQueue << std::endl;
      }else{
        OutFile << "data" << LaQueue << std::endl;
      }
    }

    if ( m_UseKeysOn == 0) {
    OutFile << ") values (" <<  std::setw(5)<<ElementKounter << ","<< std::endl;
    }else{
    OutFile << ") values (" <<  "XXX_DATA_ID_KOUNTER" << ","<< std::endl;
    }
    
    std::vector< std::string> TheData = pAmdcDbRecord->getAllData();
    int ItemTot5 = TheData.size() ;
    for (int Item=0; Item<ItemTot5; Item++){
      OutFile << TheData[Item] << std::endl;
    }
    OutFile << ");" << std::endl;
    if ( m_UseKeysOn == 0) {
    OutFile << "insert into "<< NameOfTheSet <<"_data2tag values ('"<< NameOfTheSet <<"-07',"<< std::setw(5)<<ElementKounter << ");" << std::endl;
    }else{
    OutFile << "insert into "<< NameOfTheSet <<"_data2tag values ("
      << "XXX_DATA2TAG_KOUNTER" << ","
      << "XXX_DATA_ID_KOUNTER" << ");" << std::endl;
    }

  }

  if ( ThereIsABLOB == 1 ){
    OutFile << "DECLARE" << std::endl;
    OutFile << "  lobloc  CLOB;" << std::endl;
    OutFile << "  req     utl_http.req;" << std::endl;
    OutFile << "  resp    utl_http.resp;" << std::endl;
    OutFile << "  text    VARCHAR2(32767);" << std::endl;
    OutFile << "  amount  INTEGER(10) := 0;" << std::endl;
    OutFile << "  offset  INTEGER(10) := 0;" << std::endl;
    OutFile << "  TRUE    BOOLEAN;" << std::endl;
    OutFile << "BEGIN" << std::endl;
    OutFile << "  SELECT data INTO lobloc" << std::endl;
    OutFile << "  FROM   "<< NameOfTheSet <<"_data" << std::endl;
    if ( m_UseKeysOn == 0) {
    OutFile << "  WHERE  "<< NameOfTheSet <<"_data_id = 0 FOR UPDATE;" << std::endl;
    }else{
    OutFile << "  WHERE  "<< NameOfTheSet <<"_data_id = " 
    << " XXX_DATA_ID_KOUNTER "
    << "FOR UPDATE;" << std::endl;
    }
    OutFile << "  offset := DBMS_LOB.GETLENGTH(lobloc)+2;" << std::endl;
    OutFile << "     req := utl_http.begin_request(" << std::endl;
    if ( m_UseKeysOn == 0) {
    OutFile << "     'http://baranov.home.cern.ch/baranov/sql/07_R.01_01.Light/"<< NameOfTheSet <<"temp.data');" << std::endl;
    }else{
    OutFile << "     'WEB_ADDRESS_FOR_TEMP_DATA_FILE"
    << NameOfTheSet <<"temp.data');" << std::endl;
    }
    OutFile << "  resp := utl_http.get_response(req);" << std::endl;
    OutFile << "  LOOP" << std::endl;
    OutFile << "     text := ' ';" << std::endl;
    OutFile << "     UTL_HTTP.READ_TEXT(resp, text, NULL);" << std::endl;
    OutFile << "     /* DBMS_OUTPUT.PUT_LINE(text); */" << std::endl;
    OutFile << "     amount := length(text);" << std::endl;
    OutFile << "     DBMS_LOB.WRITEAPPEND(lobloc,amount,text);" << std::endl;
    OutFile << "  END LOOP;" << std::endl;
    OutFile << "    utl_http.end_response(resp);" << std::endl;
    OutFile << "  EXCEPTION" << std::endl;
    OutFile << "    WHEN utl_http.end_of_body" << std::endl;
    OutFile << "    THEN utl_http.end_response(resp);" << std::endl;
    OutFile << "END;" << std::endl;
    OutFile << "/" << std::endl;

    std::ofstream OutFileTemp;
    std::string FileNameTemp = TagFileName + ".";
    FileNameTemp = FileNameTemp+ NameOfTheSet;
    FileNameTemp = FileNameTemp + "temp.data";
    OutFileTemp.open(FileNameTemp.c_str());
    int Idone = 0 ;
    it = pIRDBRecordset->begin();
    pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
    if (pAmdcDbRecord == 0){
      OutFile << "No way to cast in AmdcDbRecord for " << NameOfTheSet << std::endl;
      return;
    } 
    std::vector< std::string> VariableList = pAmdcDbRecord->getVariableList();
    int ItemTot6 = VariableList.size() ;
    for (int Item=0 ; Item<ItemTot6 ; Item++){
      if (Idone == 1) continue;
      std::string LaQueue = ",";
      std::string DbVar = VariableList[Item] ;
      int DbVarType = pAmdcDbRecord->getVariableType(DbVar) ;
      if ( DbVarType == 5 ){
        Idone = 1 ;
        OutFileTemp << pAmdcDbRecord->getBlob(DbVar)<< std::endl;
      }
    }
    
  } 

  if ( m_AsNewTableOn == 1) {
  OutFile << "" << std::endl;
  OutFile << "comment on column "<< NameOfTheSet <<"_data."<< NameOfTheSet <<"_data_id is 'Unique identifier';" << std::endl;
  it = pIRDBRecordset->begin();
  pAmdcDbRecord = dynamic_cast<const AmdcDbRecord*>((*it));
  if (pAmdcDbRecord == 0){
    OutFile << "No way to cast in AmdcDbRecord for " << NameOfTheSet << std::endl;
    return;
  } 
  VariableList = pAmdcDbRecord->getVariableList();
  int ItemTot7 = VariableList.size() ;
  for (int Item=0 ; Item<ItemTot7 ; Item++){
    std::string DbVar = VariableList[Item] ;
    std::string DbVarComment = pAmdcDbRecord->getVariableComment(DbVar);
    if ( DbVarComment.size() != 0 ){
      std::string DbVarToBePrinted = DbVar ;
      for (int Item=DbVar.size(); Item<10 ; Item++){
       DbVarToBePrinted = DbVarToBePrinted+" ";
      }
      std::string DbVarCommentToBePrinted = DbVarComment ;
      for (int Item=DbVarComment.size(); Item<48 ; Item++){
       DbVarCommentToBePrinted = DbVarCommentToBePrinted+" ";
      }
      OutFile << "comment on column "<< NameOfTheSet <<"_data."
              << DbVarToBePrinted.substr(0,10)
	      << " is '" 
	      << DbVarCommentToBePrinted.substr(0,48)
	        << "';"
	    << std::endl;
    }
  }
  }

 
}
