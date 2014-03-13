/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __GOODRUNSLISTS__
#define __GOODRUNSLISTS__

#ifdef __CINT__

#include <vector>
#include <map>
#include "TString.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace MET ;

#pragma link C++ enum MET::Quality;

#pragma link C++ function MET::SummaryEtmissGoodnessConfig;
#pragma link C++ function MET::GetEtmissGoodnessConfig(const TString& qualifier);

#pragma link C++ class std::vector<TString> ;
//#pragma link C++ class std::vector<TString>::iterator ;

#pragma link C++ class std::map<TString,int>+;
#pragma link C++ class std::map<TString,double>+;
#pragma link C++ class std::map<TString,int>::iterator ;
#pragma link C++ class std::map<TString,double>::iterator ;

#pragma link C++ class MET::Goodies- ;
#pragma link C++ class MET::NtupleGoodiesFiller ;

#pragma link C++ class MET::TEasyFormula ;
#pragma link C++ class MET::TRunRange ;
#pragma link C++ class MET::TSelection ;
#pragma link C++ class MET::TSelectionSet ;
#pragma link C++ class MET::TSelectionSetArchive ;

#pragma link C++ class MET::EtmissGoodnessManager- ;
#pragma link C++ class MET::EtmissGoodness ;

#pragma link C++ class MET::EtmissGoodness_Loose_20091117;
#pragma link C++ class MET::EtmissGoodness_Medium_20091117;
#pragma link C++ class MET::EtmissGoodness_Tight_20091117;

#pragma link C++ class MET::EtmissGoodness_Loose_20091217;

#pragma link C++ class MET::EtmissGoodness_Loose_20100115;
#pragma link C++ class MET::EtmissGoodness_Loose_20100217;
#pragma link C++ class MET::EtmissGoodness_Loose_20100408;

#pragma link C++ class MET::EtmissGoodness_Loose_20100415;

#endif

#endif

