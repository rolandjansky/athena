/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "AmdcDb/AmdcDbRecord.h"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <utility>

AmdcDbRecord::AmdcDbRecord(long UniversalId,const std::string& NameOfTheRecord){

  m_MapOfint["Dummy"]    =  0  ; m_MapOfint.clear();
  m_MapOfdouble["Dummy"] =  0. ; m_MapOfdouble.clear();
  m_MapOfdoublePrecision["Dummy"] =  0 ; m_MapOfdoublePrecision.clear();
  m_MapOfstring["Dummy"] = "A" ; m_MapOfstring.clear();
  m_MapOflong["Dummy"] = 0     ; m_MapOflong.clear();

  m_UniversalId     = UniversalId;
  m_NameOfTheRecord = NameOfTheRecord;
  m_ThereIsABLOB    = 0 ;
  
}

AmdcDbRecord::~AmdcDbRecord(){}

// Get
std::string AmdcDbRecord::getNameOfTheRecord() const { return m_NameOfTheRecord ;}

long AmdcDbRecord::getUniversalId() const { return m_UniversalId;}

std::vector< std::string> AmdcDbRecord::getVariableList() const { return m_VectorOfVariableNames;}

std::string AmdcDbRecord::getVariableComment(const std::string& NameToFind) const {
 std::string NoComment="NoComment";
 std::vector< std::string> VariablesList = getVariableList();
 int ItemTot = VariablesList.size() ;
 for (int Item=0 ; Item<ItemTot ; Item++){
   const std::string& DbVar = VariablesList[Item] ;
   if (DbVar == NameToFind)  return m_VectorOfVariableNamesComment[Item] ;
 }
 return NoComment;
}

int AmdcDbRecord::getVariableType(const std::string& NameToFind) const {
 std::vector< std::string> VariablesList = getVariableList();
 int ItemTot = VariablesList.size() ;
 for (int Item=0 ; Item<ItemTot ; Item++){
   const std::string& DbVar = VariablesList[Item] ;
   if (DbVar == NameToFind)  return m_VectorOfVariableNamesType[Item] ;
 }
 return 0;
}
std::vector< std::string> AmdcDbRecord::getAllData() const {
 
 std::vector< std::string> ToBeReturned;

 std::vector< std::string> VariablesList = getVariableList();

 std::string StringCurrent     ="";
 std::string StringCurrentTest ="";
 int ItemTot = VariablesList.size() ;
 for (int Item=0 ; Item<ItemTot ; Item++){
   std::ostringstream Aostringstream;
   Aostringstream << setiosflags(std::ios::fixed);

   const std::string& DbVar     = VariablesList[Item] ;
   int                DbVarType = getVariableType(DbVar) ;
  
   std::string LaQueue = ",";
   int Itemtest = Item + 1;
   int ItemTot2 = VariablesList.size() ;
   if ( Itemtest == ItemTot2 )LaQueue = "";

   if (DbVarType == 1 ) Aostringstream << std::setw(6)<< getInt(DbVar) << LaQueue ;
 
   if (DbVarType == 2 ){
     int ValPrec = getDoublePrecision(DbVar);
     if (ValPrec < 1 )ValPrec = 1 ;
     if (ValPrec > 6 )ValPrec = 6 ;
     if (  ValPrec <= 1 ) Aostringstream  << std::setw(11)<<std::setprecision(1)<< getDouble(DbVar) << LaQueue ;  
     if (  ValPrec == 2 ) Aostringstream  << std::setw(11)<<std::setprecision(2)<< getDouble(DbVar) << LaQueue ;  
     if (  ValPrec == 3 ) Aostringstream  << std::setw(11)<<std::setprecision(3)<< getDouble(DbVar) << LaQueue ;  
     if (  ValPrec == 4 ) Aostringstream  << std::setw(11)<<std::setprecision(4)<< getDouble(DbVar) << LaQueue ;  
     if (  ValPrec == 5 ) Aostringstream  << std::setw(11)<<std::setprecision(5)<< getDouble(DbVar) << LaQueue ;  
     if (  ValPrec >= 6 ) Aostringstream  << std::setw(11)<<std::setprecision(6)<< getDouble(DbVar) << LaQueue ;  
   }

   if (DbVarType == 3 ) Aostringstream << "'" << getString(DbVar) << "'" << LaQueue ;

   if (DbVarType == 4 ) Aostringstream <<  getLong(DbVar) << LaQueue ;
   
   if (DbVarType == 5 ) Aostringstream <<  "empty_clob()" << LaQueue ;
   
   StringCurrentTest = StringCurrent+Aostringstream.str();
   if (StringCurrentTest.size() > 80 || DbVarType == 5 ){
     ToBeReturned.push_back(StringCurrent);
     StringCurrent = Aostringstream.str();
   }else{
     StringCurrent = StringCurrentTest;
   }

 }
 if (StringCurrent.size() != 0 ) ToBeReturned.push_back(StringCurrent);
 
 return ToBeReturned;

}
int AmdcDbRecord::ThereIsABLOB() const {return m_ThereIsABLOB;}

// Get
int AmdcDbRecord::getInt(const std::string& NameToFind) const {

  int notfound = -99999999;
  if (m_MapOfint.find(NameToFind) != m_MapOfint.end()){
    return m_MapOfint.find(NameToFind)->second;
  }
  return notfound ;

}

double AmdcDbRecord::getDouble(const std::string& NameToFind) const {

  double notfound =  999999999999.;
  if (m_MapOfdouble.find(NameToFind) != m_MapOfdouble.end()){
    return m_MapOfdouble.find(NameToFind)->second;
  }
  return notfound ;

}
float AmdcDbRecord::getFloat(const std::string& NameToFind) const {

  return getDouble(NameToFind) ;

}

int AmdcDbRecord::getDoublePrecision(const std::string& NameToFind) const {

  int notfound = -99999999;
  if (m_MapOfdoublePrecision.find(NameToFind) != m_MapOfdoublePrecision.end()){
    return m_MapOfdoublePrecision.find(NameToFind)->second;
  }
  return notfound ;

}

std::string AmdcDbRecord::getString(const std::string& NameToFind) const {

  std::string notfound = "NOT FOUND";
  if (m_MapOfstring.find(NameToFind) != m_MapOfstring.end()){
    return m_MapOfstring.find(NameToFind)->second;
  }
  return notfound ;

}

long AmdcDbRecord::getLong(const std::string& NameToFind) const {

  long notfound = 99999999;
  if (m_MapOflong.find(NameToFind) != m_MapOflong.end()){
    return m_MapOflong.find(NameToFind)->second;
  }
  return notfound ;

}

std::string AmdcDbRecord::getBlob(const std::string& NameToFind) const {

  std::string notfound = "NOT FOUND";
  if (m_MapOfBlob.find(NameToFind) != m_MapOfBlob.end()){
    return m_MapOfBlob.find(NameToFind)->second;
  }
  return notfound ;

}


// Set
void AmdcDbRecord::addInt(std::string NameToSet,std::string Comment,int ValueToSet){
 m_MapOfint[NameToSet] = ValueToSet;
 m_VectorOfVariableNames.push_back(NameToSet);
 m_VectorOfVariableNamesComment.push_back(Comment);
 m_VectorOfVariableNamesType.push_back(1);
}

void AmdcDbRecord::addDouble(std::string NameToSet,std::string Comment,double ValueToSet, int precision){
 m_MapOfdouble[NameToSet] = ValueToSet;
 m_MapOfdoublePrecision[NameToSet] = precision;
 m_VectorOfVariableNames.push_back(NameToSet);
 m_VectorOfVariableNamesComment.push_back(Comment);
 m_VectorOfVariableNamesType.push_back(2);
}

void AmdcDbRecord::addString(std::string NameToSet,std::string Comment,std::string ValueToSet){
 m_MapOfstring[NameToSet] = std::move(ValueToSet);
 m_VectorOfVariableNames.push_back(NameToSet);
 m_VectorOfVariableNamesComment.push_back(Comment);
 m_VectorOfVariableNamesType.push_back(3);
}

void AmdcDbRecord::addLong(std::string NameToSet,std::string Comment,long ValueToSet){
 m_MapOflong[NameToSet] = ValueToSet;
 m_VectorOfVariableNames.push_back(NameToSet);
 m_VectorOfVariableNamesComment.push_back(Comment);
 m_VectorOfVariableNamesType.push_back(4);
}

void AmdcDbRecord::addBlob(std::string NameToSet,std::string Comment,std::string ValueToSet){
 m_MapOfBlob[NameToSet] = std::move(ValueToSet);
 m_VectorOfVariableNames.push_back(NameToSet);
 m_VectorOfVariableNamesComment.push_back(Comment);
 m_VectorOfVariableNamesType.push_back(5);
 m_ThereIsABLOB    = 1 ;
}

// Set
int AmdcDbRecord::doWeMatchForThisVariable(
                                const std::string&  NameToFind,
                                const AmdcDbRecord* pAmdcDbRecord
) const {

 std::vector< std::string> VariablesList = getVariableList();
 
 int ItemTot = VariablesList.size() ;
 for (int Item=0 ; Item<ItemTot ; Item++){
   const std::string& DbVar = VariablesList[Item] ;
   if (DbVar == NameToFind){
     int DbVarType = getVariableType(DbVar) ;

     if (DbVarType == 1 ){
       int DbVal   = pAmdcDbRecord->getInt(DbVar)  ; 
       int AmdcVal = getInt(DbVar);
       if ( DbVal == AmdcVal) return 1;
       return 0 ;
     }

     if (DbVarType == 2 ){
       double DbVal   = pAmdcDbRecord->getDouble(DbVar)  ; 
       double AmdcVal = getDouble(DbVar);
       if ( DbVal == AmdcVal) return 1;
       return 0 ;
     }

     if (DbVarType == 3 ){
       const std::string& DbVal   = pAmdcDbRecord->getString(DbVar)  ; 
       const std::string& AmdcVal = getString(DbVar);
       if ( DbVal == AmdcVal) return 1;
       return 0 ;
     }

     if (DbVarType == 4 ){
       long DbVal   = pAmdcDbRecord->getLong(DbVar)  ; 
       long AmdcVal = getLong(DbVar);
       if ( DbVal == AmdcVal) return 1;
       return 0 ;
     }

     if (DbVarType == 5 ){
       const std::string& DbVal   = pAmdcDbRecord->getBlob(DbVar)  ; 
       const std::string& AmdcVal = getBlob(DbVar);
       if ( DbVal == AmdcVal) return 1;
       return 0 ;
     }


     return 0 ;

   }
 }
 
 return 0;
 
}
void AmdcDbRecord::doIMatch(
                 int            WarningLevel,
                 int&           Kwarn,
                 int&           Kchck,
		 std::ofstream& OutFile,
                 const AmdcDbRecord*  pAmdcDbRecord
) const {

 long DbId = pAmdcDbRecord->getUniversalId();    
 
 std::vector< std::string> VariablesList = getVariableList();

 int ItemTot = VariablesList.size() ;
 for (int Item=0 ; Item<ItemTot ; Item++){
   const std::string& DbVar     = VariablesList[Item] ;
   int                DbVarType = getVariableType(DbVar) ;
  
   if (DbVarType == 1 ){
 
     int         DbVal   = pAmdcDbRecord->getInt(DbVar)  ; 
     int         AmdcVal = getInt(DbVar);

     int DeltaValue =  DbVal - AmdcVal ;
     std::string WARNING_Message	  = " ";
     int Iwarning = 0 ;
     if (DeltaValue != 0) {
       WARNING_Message = "  <=========  WARNING !";
       Iwarning = 1 ;
     }
     Kwarn = Kwarn + Iwarning;
     Kchck = Kchck + 1;

     if (Iwarning >= WarningLevel){
       OutFile 
         << " Unique Id:"     << DbId
         << " Variable:" << std::setw(12) << DbVar
         << "  DB/Am/DB-Am " 
         << std::setw(12)  		    << DbVal     
         << std::setw(12)  		    << AmdcVal	 
         << std::setw(12)  		    << DeltaValue	   
         <<				       WARNING_Message   
         << std::endl;
     }

   }

   if (DbVarType == 2 ){
 
     double      DbVal       = pAmdcDbRecord->getDouble(DbVar)  ; 
     double      AmdcVal     = getDouble(DbVar);
     int         AmdcValPrec = getDoublePrecision(DbVar);
     if (AmdcValPrec < 1 )AmdcValPrec = 1 ;
     if (AmdcValPrec > 6 )AmdcValPrec = 6 ;

     double DeltaValue =  DbVal - AmdcVal ;
     std::string WARNING_Message	  = " ";
     int Iwarning = 0 ;
     double EpsKut = 1./ std::pow(10., AmdcValPrec);
     if ( std::abs(DeltaValue) > EpsKut ) {
       WARNING_Message = "  <=========  WARNING !";
       Iwarning = 1 ;
     }
     Kwarn = Kwarn + Iwarning;
     Kchck = Kchck + 1;

     if (Iwarning >= WarningLevel){
       OutFile 
          << " Unique Id:"     << DbId
          << " Variable:" << std::setw(12) << DbVar
          << "  DB/Am/DB-Am " ;
       if ( AmdcValPrec <= 1 ){
         OutFile 
           << std::setw(12)<<std::setprecision(1)<< DbVal     
           << std::setw(12)<<std::setprecision(1)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(1)<< DeltaValue ;     
       }
       if ( AmdcValPrec == 2 ){
         OutFile 
           << std::setw(12)<<std::setprecision(2)<< DbVal     
           << std::setw(12)<<std::setprecision(2)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(2)<< DeltaValue ;     
       }
       if ( AmdcValPrec == 3 ){
         OutFile 
           << std::setw(12)<<std::setprecision(3)<< DbVal     
           << std::setw(12)<<std::setprecision(3)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(3)<< DeltaValue ;     
       }
       if ( AmdcValPrec == 4 ){
         OutFile 
           << std::setw(12)<<std::setprecision(4)<< DbVal     
           << std::setw(12)<<std::setprecision(4)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(4)<< DeltaValue ;     
       }
       if ( AmdcValPrec == 5 ){
         OutFile 
           << std::setw(12)<<std::setprecision(5)<< DbVal     
           << std::setw(12)<<std::setprecision(5)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(5)<< DeltaValue ;     
       }
       if ( AmdcValPrec >= 6 ){
         OutFile 
           << std::setw(12)<<std::setprecision(6)<< DbVal     
           << std::setw(12)<<std::setprecision(6)<< AmdcVal	 
           << std::setw(12)<<std::setprecision(6)<< DeltaValue ;     
       }
       OutFile 
          <<				       WARNING_Message   
          << std::endl;
     }

   }

   if (DbVarType == 3 ){
 
     const std::string& DbVal   = pAmdcDbRecord->getString(DbVar)  ; 
     const std::string& AmdcVal = getString(DbVar);

     std::string WARNING_Message	= " ";
     int Iwarning = 0 ;
     if ( DbVal != AmdcVal ) {
       WARNING_Message = "  <=========  WARNING !";
       Iwarning = 1 ;
     }
     Kwarn = Kwarn + Iwarning;
     Kchck = Kchck + 1;

     if (Iwarning >= WarningLevel){
       OutFile 
     	 << " Unique Id:"     << DbId
     	 << " Variable:" << std::setw(12) << DbVar
     	 << "  DB/Am       " 
     	 << std::setw(12)		  << DbVal     
     	 << std::setw(12)		  << AmdcVal   
     	 <<				     WARNING_Message   
     	 << std::endl;
     }

   }

   if (DbVarType == 4 ){

     long        DbVal   = pAmdcDbRecord->getLong(DbVar)  ; 
     long        AmdcVal = getLong(DbVar);

     long DeltaValue =  DbVal - AmdcVal ;
     std::string WARNING_Message	= " ";
     int Iwarning = 0 ;
     if (DeltaValue != 0) {
       WARNING_Message = "  <=========  WARNING !";
       Iwarning = 1 ;
     }
     Kwarn = Kwarn + Iwarning;
     Kchck = Kchck + 1;

     if (Iwarning >= WarningLevel){
       OutFile 
     	 << " Unique Id:"     << DbId
     	 << " Variable:" << std::setw(12) << DbVar
     	 << "  DB/Am/DB-Am " 
     	 << std::setw(12)		  << DbVal     
     	 << std::setw(12)		  << AmdcVal   
     	 << std::setw(12)		  << DeltaValue 	 
     	 <<				     WARNING_Message   
     	 << std::endl;
     }

   }

   if (DbVarType == 5 ){
 
     const std::string& DbVal   = pAmdcDbRecord->getBlob(DbVar)  ; 
     const std::string& AmdcVal = getBlob(DbVar);

     std::string WARNING_Message	= " ";
     int Iwarning = 0 ;
     if ( DbVal != AmdcVal ) {
       WARNING_Message = "  <=========  WARNING !";
       Iwarning = 1 ;
     }
     Kwarn = Kwarn + Iwarning;
     Kchck = Kchck + 1;

     if (Iwarning >= WarningLevel){
       OutFile 
     	 << " Unique Id:"     << DbId
     	 << " Variable:" << std::setw(12) << DbVar
     	 << "  BLOB are not printed       " 
     	 <<				     WARNING_Message   
     	 << std::endl;
     }

   }

 }

}
//Functions 
//  of IRDBRecord Not implemented
bool        AmdcDbRecord::isFieldNull(const std::string& /*fieldName*/) const {std::cout << " AmdcDbRecord::isFieldNull Not implemented " << std::endl ;return true;}
int         AmdcDbRecord::getInt     (const std::string& /*fieldName*/, unsigned int /*index*/) const{std::cout << " AmdcDbRecord::getInt(const std::string& fieldName, unsigned int index) Not implemented " << std::endl ;return 1;}
long        AmdcDbRecord::getLong    (const std::string& /*fieldName*/, unsigned int /*index*/) const{std::cout << " AmdcDbRecord::getLong(const std::string& fieldName, unsigned int index) Not implemented " << std::endl ;return 1;}
double      AmdcDbRecord::getDouble  (const std::string& /*fieldName*/, unsigned int /*index*/) const{std::cout << " AmdcDbRecord::getDouble(const std::string& fieldName, unsigned int index) Not implemented " << std::endl ;return 1.;}
float       AmdcDbRecord::getFloat   (const std::string& /*fieldName*/, unsigned int /*index*/) const{std::cout << " AmdcDbRecord::getFloat(const std::string& fieldName, unsigned int index) Not implemented " << std::endl ;return 1.;}
std::string AmdcDbRecord::getString  (const std::string& /*fieldName*/, unsigned int /*index*/) const{std::cout << " AmdcDbRecord::getString(const std::string& fieldName, unsigned int index) Not implemented " << std::endl ;std::string ToBeReturned = "Not Implemented";return ToBeReturned;}
