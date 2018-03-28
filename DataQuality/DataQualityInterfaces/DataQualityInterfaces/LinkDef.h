/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace dqi ;

#pragma link C++ class dqi::MiniConfig ;
#pragma link C++ class dqi::MiniConfigTreeNode ;

#pragma link C++ class dqi::HanApp ;
#pragma link C++ class dqi::HanConfig ;
#pragma link C++ class dqi::HanConfigAlgLimit+ ;
#pragma link C++ class dqi::HanConfigAlgPar+ ;
#pragma link C++ class dqi::HanConfigParMap+ ;
#pragma link C++ class dqi::HanConfigAssessor- ;
#pragma link C++ class dqi::HanConfigCompAlg- ;
#pragma link C++ class dqi::HanConfigGroup+ ;
#pragma link C++ class dqi::HanConfigMetadata+ ;
#pragma link C++ class dqi::ConditionsSingleton- ;

//Working around some brokenness in tdaq code
#pragma link C++ class std::map<TString,double>+ ;

#endif
