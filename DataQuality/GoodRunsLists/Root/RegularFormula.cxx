/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "GoodRunsLists/RegularFormula.h"
#include "GoodRunsLists/TMsgLogger.h"

#include "TRegexp.h"
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
}

Root::RegularFormula::~RegularFormula()
{
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

  TRegexp specialChars("[+-/*%&=!><()|]+");
  int lastIdx = -1;
  do {
    //search for special characters and determine if they're escaped
    lastIdx = parStr.Index(specialChars, lastIdx+1);
    if (lastIdx >= 0) {
      bool needsReplacement = false;
      if (lastIdx > 0) {
	//if a match is found and it's not the first character of the string
	char isEscape = parStr[lastIdx-1];
	if (isEscape == '\\') {
	  //this is an escpaed special character, keep it but remove escape
	  // both on parStr as well as in expr
	  parStr = parStr.Remove(lastIdx-1, 1);
	  expr = expr.Remove(lastIdx-1, 1);
	  needsReplacement = false;
	  lastIdx--; //we removed one character from the string
	} else {
	  needsReplacement = true;
	}
      } else {
	//matched the first character
	needsReplacement = true;
      }

      if (needsReplacement) {
	//not escaped, just replace with a space to allow parameters parsing below
	parStr = parStr.Replace(lastIdx, 1, " ");
      }
    }
  } while (lastIdx >= 0);

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

