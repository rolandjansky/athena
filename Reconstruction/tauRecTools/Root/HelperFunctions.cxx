/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// local include(s)
#include "tauRecTools/HelperFunctions.h"

#include <TObjString.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>

#include <iostream>

xAOD::TauTrack::TrackFlagType tauRecTools::isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag){
  const int flagsize=sizeof(flag)*8;
  flag=flag<<(flagsize-xAOD::TauJetParameters::classifiedFake-1);
  flag=flag>>(flagsize-xAOD::TauJetParameters::classifiedCharged+1);
  return flag;
}

bool tauRecTools::sortTracks(const ElementLink<xAOD::TauTrackContainer> &l1, const ElementLink<xAOD::TauTrackContainer> &l2)
{
  //should we be safe and ask if the links are available?
  const xAOD::TauTrack* xTrack1 = *l1;
  const xAOD::TauTrack* xTrack2 = *l2;

  //return classified charged, then isolation (wide tracks), else by pt
  xAOD::TauTrack::TrackFlagType f1 = isolateClassifiedBits(xTrack1->flagSet());
  xAOD::TauTrack::TrackFlagType f2 = isolateClassifiedBits(xTrack2->flagSet());

  if(f1==f2)
    return xTrack1->pt()>xTrack2->pt();
  return f1<f2;
}

//________________________________________________________________________________
std::vector<TString> tauRecTools::parseString(const TString& str, const TString& delim/*=","*/){
  std::vector<TString> parsed_strings;
  TObjArray* varList_ar = str.Tokenize(delim);
  for(int i = 0; i != varList_ar->GetEntries(); ++i){
    TString var = dynamic_cast<TObjString*> (varList_ar->At(i))->String();
    if(var.Length()==0) continue;
    parsed_strings.push_back(var);
  }
  delete varList_ar;
  return parsed_strings;
}

//________________________________________________________________________________
std::vector<TString> tauRecTools::parseStringMVAUtilsBDT(const TString& str, const TString& delim/*=","*/){
  std::vector<TString> parsed_strings = parseString(str, delim);
  for( TString& str : parsed_strings ){
    str.ReplaceAll(" ", "");
    if(str.Contains(":=")){
      str=str(str.Index(":=")+2, str.Length()-str.Index(":=")-2);
    }
  }
  return parsed_strings;
}

//________________________________________________________________________________
MVAUtils::BDT* tauRecTools::configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile){
  TFile* fBDT = TFile::Open( weightFile );
  if(!fBDT){
    std::cerr << "ERROR Cannot find tau input BDT file: " << weightFile << std::endl;
    return 0;
  }
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    delete fBDT;
    std::cerr << "ERROR Cannot find tau input BDT tree" << std::endl;
    return 0;
  }    

  std::cout << "tauRecTools::configureMVABDT opened file: " << weightFile << std::endl;

  std::vector<float*> vars;

  //parsing of variables done here from TNamed object
  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    std::cerr << "ERROR no Variable List in file : " << weightFile << std::endl;
    delete fBDT;
    return 0;
  }
  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  for(const TString& str : varList_ar){
    if(str.Length()==0) continue;
    std::map<TString, float*>::iterator itr = availableVars.find(str);
    if(itr==availableVars.end()){
      std::cerr << "ERROR Variable : " << str << " is not available" << std::endl;
      delete fBDT;
      delete n_varList;
      return 0;
    }
    vars.push_back( itr->second );
  }
  
  MVAUtils::BDT* reader = new MVAUtils::BDT(tBDT);
  reader->SetPointers( vars );

  delete n_varList;
  delete fBDT;
  return reader;

}

//________________________________________________________________________________
tauRecTools::TRTBDT::TRTBDT(const char* weightFile){
  this->bdt = 0;
  init(weightFile);
}

//________________________________________________________________________________
bool tauRecTools::TRTBDT::init(const char* weightFile){
  TFile* fBDT = TFile::Open( weightFile );
  if(!fBDT){
    std::cerr << "ERROR Cannot find tau input BDT file: " << weightFile << std::endl;
    return 0;
  }
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    delete fBDT;
    std::cerr << "ERROR Cannot find tau input BDT tree" << std::endl;
    return 0;
  }    

  std::cout << "tauRecTools::configureMVABDT opened file: " << weightFile << std::endl;
  // tBDT->Print();
  // std::cout << "CHECK 123" << std::endl;

  std::vector<float*> vars;

  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    std::cerr << "ERROR no Variable List in file : " << weightFile << std::endl;
    delete fBDT;
    return 0;
  }

  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  for(TString str : varList_ar){
    if(str.Length()==0) continue;
    
    if(str.Index(".")>=0){
      str = str(str.Last('.')+1, str.Length()-str.Last('.')-1);
    }

    float* fval = new float(0);
    DummyAccessor* facc = new DummyAccessor(str);//second argument if int

    m_data.insert(std::make_pair(fval, facc));
    vars.push_back(fval);
  }

  this->bdt = new MVAUtils::BDT(tBDT);
  this->bdt->SetPointers( vars );

  delete n_varList;
  delete fBDT;
  return true;
}

//________________________________________________________________________________
tauRecTools::TRTBDT::~TRTBDT(){
  for( std::pair<float*, DummyAccessor*> p : m_data ){
    delete p.first;
    delete p.second;
  }
  m_data.clear();
}

//________________________________________________________________________________
bool tauRecTools::TRTBDT::updateVariables(const xAOD::TauJet& tau){
  for( std::pair<float*, DummyAccessor*> p : m_data ){
    //check availability first and return false if not avail
    *(p.first) = (*p.second)(tau);
  }
  return true;
}

//________________________________________________________________________________
float tauRecTools::TRTBDT::GetGradBoostMVA(){
  return this->bdt->GetGradBoostMVA(this->bdt->GetPointers());
}

//________________________________________________________________________________
float tauRecTools::TRTBDT::GetClassification(){
  return this->bdt->GetClassification();
}

//________________________________________________________________________________
float tauRecTools::TRTBDT::GetResponse(){
  return this->bdt->GetResponse();
}
