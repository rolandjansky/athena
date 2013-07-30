/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __GOODRUNSLISTS__
#define __GOODRUNSLISTS__

#include <vector>
#include <string>
#include <map>
#include <TString.h>
#include <GoodRunsLists/DQHelperFunctions.h>
#include <GoodRunsLists/RegularFormula.h>
#include <GoodRunsLists/TGoodRunsListReader.h>
#include <GoodRunsLists/TGoodRunsListWriter.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace Root ;

#pragma link C++ class Root::RegularFormula+;

#pragma link C++ class Root::TGoodRunsListReader+;
#pragma link C++ class Root::TGoodRunsListWriter+;

#pragma link C++ class Root::TLumiBlockRange+;
#pragma link C++ class vector<Root::TLumiBlockRange>+;
#pragma link C++ class vector<Root::TLumiBlockRange>::iterator;

#pragma link C++ class Root::TGoodRun+;
#pragma link C++ class vector<Root::TGoodRun>+;
#pragma link C++ class vector<Root::TGoodRun>::iterator;

#pragma link C++ class pair<int,Root::TGoodRun>+;
//#pragma link C++ class std::pair<Int_t,Root::TGoodRun>::iterator ;
#pragma link C++ class map<int,Root::TGoodRun>+;
#pragma link C++ class map<int,Root::TGoodRun>::iterator;

#pragma link C++ class map<TString,TString>+;
#pragma link C++ class pair<TString,TString>+;

#pragma link C++ class Root::TGoodRunsList+;
#pragma link C++ class vector<Root::TGoodRunsList>+;
#pragma link C++ class vector<Root::TGoodRunsList>::iterator;

#pragma link C++ class Root::TGRLCollection+;

#pragma link C++ namespace DQ;
#pragma link C++ function DQ::SetXMLFile( const TString& );
#pragma link C++ function DQ::PassRunLB( int,int );

#endif

#endif
