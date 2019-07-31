/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/RegularFormula.h"
#include "GoodRunsLists/TMsgLogger.h"

#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TROOT.h"

#include <iostream>
#include <stdlib.h>
#include <algorithm>

ClassImp(Root::RegularFormula)


Root::RegularFormula::RegularFormula()
  : TFormula()
{
}


Root::RegularFormula::RegularFormula(const char* name,const char* expression) 
  : TFormula(name,"1") 
{
  (void) setFormula(expression);

  //m_expr = parseExpression(expression);
  //this->Compile(m_expr.Data());
}

Root::RegularFormula::~RegularFormula()
{
  //this->TFormula::~TFormula();
}


Root::RegularFormula::RegularFormula(const Root::RegularFormula& other)
  : TFormula(other)
  , m_expr(other.m_expr)
  , m_par(other.m_par)
{
}


Root::RegularFormula&
Root::RegularFormula::operator=(const Root::RegularFormula& other)
{
  if (&other==this) {
    return *this;
  }

  TFormula::operator=(other) ;
  m_expr = other.m_expr;
  m_par  = other.m_par;

  return *this ;
}


void 
Root::RegularFormula::parseExpression(const char* expression, TString& expr) 
{ 
  expr = expression;
  TString parStr(expression);

  const int nsign(13);
  const char* sign = "+-*/%&|=!<>()";
  for (int i=0; i<nsign; ++i)
    parStr = parStr.ReplaceAll(sign[i]," ");

  TMsgLogger mylogger( "RegularFormula" );
  mylogger << kINFO << "Now parsing regular expression : " << expression << GEndl;
  mylogger << kINFO << "Please be aware that Cling errors are expected and aren't inherently a problem" << GEndl;

  TFormula analyzer("analyzer","1");
  TObjArray* parArr = parStr.Tokenize(" ");
  for (int count(0), i(0); i<parArr->GetEntries(); ++i) {
    TString myPar = ((TObjString*)parArr->At(i))->GetString();   
    if ( 0==analyzer.Compile(myPar.Data()) || 
         0==analyzer.Compile(Form("%s(1)",myPar.Data())) ) {
      continue;
    } else {
      std::list<TString>::iterator itrF = std::find(m_par.begin(),m_par.end(),myPar);
      if (itrF==m_par.end()) {
        expr = expr.ReplaceAll(myPar,Form("[%d]",count));
        m_par.push_back(myPar);
        count++;
      }
    }
  }
  delete parArr;

  mylogger << kINFO << "Number of interpreted input parameters : " << m_par.size() << GEndl;
  mylogger << kINFO << "Parsed regular expression : " << expr << GEndl;  
}


Int_t 
Root::RegularFormula::setFormula(const char *expression)
{
  m_par.clear();
  this->parseExpression(expression,m_expr);
  return this->Compile(m_expr.Data());
}

