
/**
 **
 **   @file         IReadCards.cxx        
 **
 **                 fortran callable interface routines to the  
 **                 ReadCards parameter file  
 **                   
 ** 
 **   @author       M.Sutton    
 **
 **   @date         Fri Aug 12 17:40:22 CEST 2005
 **
 **   Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
 **                   
 **                   
 **
 **/



#include "ReadCards.h"
#include "IReadCards.h"


ReadCards* rp = NULL;


void initcards_(const char* s) {
  rp = new ReadCards(s);
}

void deletecards_(void) {
  if ( rp ) delete rp;
}



void getstring_(const char* tag, char* s) {
  string val = rp->GetString(tag);
  const char* sp = val.c_str();
  while ( ( (*s++) = (*sp++) ) );
  (*(s-1)) = ' ';
  return;
}



void isdefined_(const char* tag, int& i) {
  i = ( rp->isTagDefined(tag) ? 1 : 0 );
}


void getdvalue_(const char* tag, double& d) {
  d = rp->GetValue(tag);
}

void getfvalue_(const char* tag, float& f) {
  f = rp->GetValue(tag);
}


void getivalue_(const char* tag, int& i) {
  i = int(rp->GetValue(tag));
}

void getbvalue_(const char* tag, bool& b) {
  b = bool(rp->GetValue(tag));
}


void getdvector_(const char* tag, double* d) {
  vector<double> dv = rp->GetVector(tag);
  for ( unsigned i=0 ; i<dv.size() ; i++ ) d[i] = dv[i];
}

void getivector_(const char* tag, int* d) {
  vector<double> dv = rp->GetVector(tag);
  for ( unsigned i=0 ; i<dv.size() ; i++ ) d[i] = int(dv[i]);
}

void printcards_() { rp->print(); }




