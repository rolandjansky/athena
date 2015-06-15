// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: Helpers.py 530000 2012-12-11 15:45:53Z krasznaa $
#ifdef __CINT__

#include "../TrigCostD3PD/D3PDObjectBase.h"
//#include "../TrigCostD3PD/D3PDPerfStats.h"
//#include "../TrigCostD3PD/D3PDReadStats.h"
#include "../TrigCostD3PD/NTUP_TRIGCOST.h"
#include "../TrigCostD3PD/TrigCostD3PDObject.h"
#include "../TrigCostD3PD/TrigDBKeysD3PDObject.h"
#include "../TrigCostD3PD/UserD3PDObject.h"
#include "../TrigCostD3PD/Utils.h"

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;

#pragma link C++ class std::map<std::string,int>+;
#pragma link C++ class std::map<std::string,float>+;

#pragma link C++ class D3PDReader::VarHandleBase+;
#pragma link C++ class D3PDReader::VarProxyBase+;
//#pragma link C++ class D3PDReader::VariableStats+;
//#pragma link C++ class D3PDReader::D3PDReadStats+;
//#pragma link C++ class map<TString,D3PDReader::VariableStats>+;
//#pragma link C++ class pair<TString,D3PDReader::VariableStats>+;
#pragma link C++ class map<TString,D3PDReader::VarHandleBase*>+;
#pragma link C++ class pair<TString,D3PDReader::VarHandleBase*>+;
#pragma link C++ class D3PDReader::UserD3PDObjectElement+;
#pragma link C++ class D3PDReader::UserD3PDObject+;
#pragma link C++ class D3PDReader::VarHandle<unsigned int>+;
#pragma link C++ class D3PDReader::D3PDObjectBase+;
//#pragma link C++ class D3PDReader::D3PDPerfStats+;
#pragma link C++ class D3PDReader::VarHandle<float>+;
#pragma link C++ class D3PDReader::NTUP_TRIGCOST+;
#pragma link C++ class D3PDReader::VarHandle<vector<unsigned short>*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<unsigned char>*>+;
#pragma link C++ class D3PDReader::VarHandle<unsigned short>+;
#pragma link C++ class D3PDReader::VarHandle<vector<float>*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<vector<unsigned int> >*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<vector<unsigned char> >*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<unsigned int>*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<double>*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<vector<double> >*>+;
#pragma link C++ class D3PDReader::VarHandle<vector<vector<unsigned short> >*>+;
#pragma link C++ class D3PDReader::TrigCostD3PDObject+;
#pragma link C++ class D3PDReader::TrigDBKeysD3PDObject+;
#pragma link C++ class D3PDReader::VarHandle<vector<short>*>+;
#pragma link C++ class D3PDReader::VarHandle<bool>+;
// You can disable the remaining lines if you don't
// plan to use the library in CINT or PyROOT.
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<bool>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<short>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<unsigned short>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<int>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<unsigned int>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<long long>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<unsigned long long>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<float>;
#pragma link C++ function D3PDReader::UserD3PDObject::DeclareVariable<double>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<bool>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<short>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<unsigned short>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<int>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<unsigned int>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<long long>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<unsigned long long>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<float>;
#pragma link C++ function D3PDReader::UserD3PDObject::Variable<double>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<short>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<unsigned short>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<int>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<unsigned int>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<long long>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<unsigned long long>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<float>;
#pragma link C++ function D3PDReader::UserD3PDObjectElement::Variable<double>;

#endif // __CINT__
