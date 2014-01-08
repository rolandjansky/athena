/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonboyCore/AgddXml.h"

// Fortran routines -------------------------------------------------------------

extern "C" void getamdcxmlvarnb_( int& NbVar );

extern "C" void f1getamdcxmlvarname_( int& IVar, int& Lvar,  char* CVAR );

extern "C" void f1getamdcxmlvarvalue_( int& Long, char* VARNAME, int& Iok, double& Value );


//*Class implementation
AgddXml::AgddXml(){}
 
AgddXml::~AgddXml(){}
 

void AgddXml::GetAgddXmlVariableNb    (int& NbVar ){
  getamdcxmlvarnb_(NbVar);
}

void AgddXml::GetAgddXmlVariableName  (int  IVar , std::string& NameVar ){
  int  Lvar;
  char CVAR[512];
  f1getamdcxmlvarname_(IVar,Lvar,CVAR);
  NameVar.resize(Lvar);
  for(int I=0; I<Lvar; I++){ NameVar[I] = CVAR[I]; }
}

void AgddXml::GetAgddXmlVariableValue (std::string  NameVar, int& Iok, double& Value){
  char VARNAME[512];
  int Long = NameVar.size();
  if( Long > 512 ){ Long = 512; }
  for(int I=0; I<Long; I++){ VARNAME[I] = NameVar[I]; }
  f1getamdcxmlvarvalue_(Long,VARNAME,Iok,Value);
}
