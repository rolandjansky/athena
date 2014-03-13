/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETGoodness/TEasyFormula.h"
#include "MissingETGoodness/TMsgLogger.h"

#include "TString.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>


ClassImp(MET::TEasyFormula)


//_____________________________________________________________________________
MET::TEasyFormula::TEasyFormula()
  : TFormula()
{
}


//_____________________________________________________________________________
MET::TEasyFormula::TEasyFormula(const char* expression)
  : TFormula()
{
  (void) this->SetFormula(expression);
}


//_____________________________________________________________________________
MET::TEasyFormula::TEasyFormula(const char* name,const char* expression) 
  : TFormula(name,"1") 
{
  (void) this->SetFormula(expression);
}


//_____________________________________________________________________________
MET::TEasyFormula::~TEasyFormula()
{
}


//_____________________________________________________________________________
MET::TEasyFormula::TEasyFormula(const MET::TEasyFormula& other)
  : TFormula(other)
  , m_expr(other.m_expr)
  , m_par(other.m_par)
  , m_stod(other.m_stod)
  , m_itos(other.m_itos)
  , m_stoi(other.m_stoi)
{
}


//_____________________________________________________________________________
MET::TEasyFormula&
MET::TEasyFormula::operator=(const MET::TEasyFormula& other)
{
  if (&other==this) {
    return *this;
  }

  TFormula::operator=(other) ;
  m_expr = other.m_expr;
  m_par  = other.m_par;
  m_stod = other.m_stod;
  m_itos = other.m_itos;
  m_stoi = other.m_stoi;

  return *this ;
}


//_____________________________________________________________________________
Int_t 
MET::TEasyFormula::SetFormula(const char *expression)
{
  m_par.clear();
  m_stod.clear();
  m_itos.clear();
  m_stoi.clear();
  m_expr = expression;

  return this->Compile(m_expr.Data());
}


//_____________________________________________________________________________
void
MET::TEasyFormula::SetValue(const TString& name, const double& value)
{
  if ( m_stod.find(name)==m_stod.end() ) {
    TString newname(name);
    (void) this->DefinedVariable(newname); // define new variable
  }
  m_stod[name] = value; 
}


//_____________________________________________________________________________
Double_t
MET::TEasyFormula::DefinedValue(Int_t code) 
{
  // Interface to TFormula, return value defined by object with id 'code'
  // Object ids are mapped from object names by method DefinedVariable()
  // Return current value for variable indicated by internal reference code

  if ( m_itos.find(code)==m_itos.end() ) { return 0.; }
  TString name = m_itos[code];
  if ( m_stod.find(name)!=m_stod.end() ) { return m_stod[name]; }

  return 0.;
}


//_____________________________________________________________________________
Int_t 
MET::TEasyFormula::DefinedVariable(TString &name, int& action)
{
  // Interface to TFormula. If name passed by TFormula is recognized
  // as one of our RooAbsArg servers, return a unique id integer
  // that represent this variable.

  Int_t ret = DefinedVariable(name) ;
  if (ret>=0) { action = kDefinedVariable; }
  return ret ;
}


//_____________________________________________________________________________
Int_t 
MET::TEasyFormula::DefinedVariable(TString &name) 
{
  // Interface to TFormula. If name passed by TFormula is recognized
  // as one of our RooAbsArg servers, return a unique id integer
  // that represent this variable.

  if ( m_stod.find(name)==m_stod.end() ) {
    // variable not known, add it
    m_stod[name] = 0.;
    m_par.push_back(name);
    int uid = static_cast<int>(m_stod.size())-1 ; 
    m_itos[uid]  = name;
    m_stoi[name] = uid;
    return uid ;
  } else {
    // variable known, return code
    return m_stoi[name];
  }

  // variable does not exist
  return -1 ;
}


//_____________________________________________________________________________
void
MET::TEasyFormula::Summary() const
{
  MET::TMsgLogger m_logger("TEasyFormula");
  m_logger << kINFO << "Formula : " << m_expr << GEndl;
}


//_____________________________________________________________________________
Bool_t 
MET::TEasyFormula::Contains(const TString& parname) const
{
  return (m_stod.find(parname)!=m_stod.end());
}


//_____________________________________________________________________________
Bool_t
MET::TEasyFormula::Contains(const std::vector<TString>& parList) const
{
  if (parList.empty()) { return kTRUE; }

  Bool_t found(kFALSE);
  std::vector<TString>::const_iterator itr = parList.begin();
  for (; itr!=parList.end(); ++itr) {
    found = this->Contains(*itr);
    if (found) break;
  }
  return found;
}

