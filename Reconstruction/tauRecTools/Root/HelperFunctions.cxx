/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "tauRecTools/HelperFunctions.h"

#include <TObjString.h>
#include <TObjArray.h>
#include <TFile.h>
#include <TTree.h>

#include <iostream>

namespace tauRecTools {
  ANA_MSG_SOURCE(msgHelperFunction, "HelperFunction")
}



const xAOD::Vertex* tauRecTools::getJetVertex(const xAOD::Jet& jet) {
  const xAOD::Vertex* jetVertex = nullptr;
  
  bool isAvailable = jet.getAssociatedObject("OriginVertex", jetVertex); 
  if (! isAvailable) {
    return nullptr;
  }

  return jetVertex;
}



const xAOD::Vertex* tauRecTools::getTauVertex(const xAOD::TauJet& tau, bool inTrigger) {
  using namespace tauRecTools::msgHelperFunction;
  
  const xAOD::Vertex* tauVertex = nullptr;
  if (tau.vertexLink().isValid()) {
    tauVertex = tau.vertex();
  }
  else if (! inTrigger) {
    if (! tau.jetLink().isValid()) {
      ANA_MSG_WARNING("Link to seed jet is not valid !");
      return nullptr;
    } 
    const xAOD::Jet* seedJet = tau.jet();
    tauVertex = tauRecTools::getJetVertex(*seedJet);
  }

  return tauVertex;
}



TLorentzVector tauRecTools::getTauAxis(const xAOD::TauJet& tau, bool doVertexCorrection) {
  TLorentzVector tauAxis;
  if (doVertexCorrection) {
    tauAxis = tau.p4(xAOD::TauJetParameters::IntermediateAxis);
  }
  else {
    tauAxis = tau.p4(xAOD::TauJetParameters::DetectorAxis);
  }
 
  return tauAxis;
}

//________________________________________________________________________________
xAOD::TauTrack::TrackFlagType tauRecTools::isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag){
  const int flagsize=sizeof(flag)*8;
  flag=flag<<(flagsize-xAOD::TauJetParameters::classifiedFake-1);
  flag=flag>>(flagsize-xAOD::TauJetParameters::classifiedCharged+1);
  return flag;
}

//________________________________________________________________________________
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
    if (auto tos = dynamic_cast<TObjString*> (varList_ar->At(i))) {
      TString var = tos->String();
      if(var.Length()==0) continue;
      parsed_strings.push_back(var);
    }
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
std::unique_ptr<MVAUtils::BDT> tauRecTools::configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile){
  using namespace tauRecTools::msgHelperFunction;
  std::unique_ptr<TFile> fBDT(TFile::Open(weightFile));
  if(!fBDT){
    ANA_MSG_ERROR("configureMVABDT: Cannot find tau input BDT file: " << weightFile );
    return nullptr;
  }
  
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    ANA_MSG_ERROR("configureMVABDT: Cannot find tau input BDT tree");
    return nullptr;
  }    

  ANA_MSG_INFO("configureMVABDT: opened file: " << weightFile);

  std::vector<float*> vars;

  //parsing of variables done here from TNamed object
  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    ANA_MSG_ERROR("configureMVABDT: no Variable List in file: " << weightFile );
    return nullptr;
  }
  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  delete n_varList;

  for(const TString& str : varList_ar){
    if(str.Length()==0) continue;
    std::map<TString, float*>::iterator itr = availableVars.find(str);
    if(itr==availableVars.end()){
      ANA_MSG_ERROR("configureMVABDT: Variable : " << str << " is not available" );
      return nullptr;
    }
    vars.push_back( itr->second );
  }
  
  auto reader = std::make_unique<MVAUtils::BDT>(tBDT);
  reader->SetPointers( vars );

  fBDT->Close();
  return reader;
}

//________________________________________________________________________________
std::unique_ptr<MVAUtils::BDT> tauRecTools::configureMVABDT(std::vector<TString>& availableVars, const TString& weightFile) {
  using namespace tauRecTools::msgHelperFunction;
  std::unique_ptr<TFile> fBDT(TFile::Open(weightFile));
  if(!fBDT){
    ANA_MSG_ERROR("configureMVABDT: Cannot find tau input BDT file: " << weightFile );
    return nullptr;
  }
  
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    ANA_MSG_ERROR("configureMVABDT: Cannot find tau input BDT tree");
    return nullptr;
  }    

  ANA_MSG_INFO("configureMVABDT: opened file: " << weightFile);

  //parsing of variables done here from TNamed object
  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    ANA_MSG_ERROR("configureMVABDT: no Variable List in file: " << weightFile );
    return nullptr;
  }
  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  delete n_varList;

  availableVars.clear();
  for(const TString& str : varList_ar) {
    availableVars.push_back( str );
  }
  
  auto reader = std::make_unique<MVAUtils::BDT>(tBDT);

  fBDT->Close();
  return reader;
}



TLorentzVector tauRecTools::GetConstituentP4(const xAOD::JetConstituent& constituent) {
  using namespace tauRecTools::msgHelperFunction;

  TLorentzVector constituentP4;

  if( constituent->type() == xAOD::Type::CaloCluster ) {
    const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>( constituent->rawConstituent() );
    constituentP4 = cluster->p4();
  }
  else if ( constituent->type() == xAOD::Type::ParticleFlow ) {
    const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( constituent->rawConstituent() );
    constituentP4 = pfo->p4();
  }
  else {
    ANA_MSG_ERROR("GetConstituentP4: Seed jet constituent type not supported!");
    constituentP4.SetPtEtaPhiE(constituent.pt(), constituent.eta(), constituent.phi(), constituent.e());
  }

  return constituentP4;
}
