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



const StatusCode tauRecTools::GetJetClusterList(const xAOD::Jet* jet, std::vector<const xAOD::CaloCluster*> &clusterList, bool useSubtractedCluster) {
  using namespace tauRecTools::msgHelperFunction;

  // If using subtracted clusters, need to store unmodified to check if charged are duplicates
  std::vector<const xAOD::CaloCluster*> dupList;

  // Loop over jet constituents
  xAOD::JetConstituentVector constituents = jet->getConstituents();
  for(const xAOD::JetConstituent* constituent : constituents){
    ANA_MSG_DEBUG("JetConstituent: ");
    ANA_MSG_DEBUG("eta: " << constituent->eta() << " phi: " << constituent->phi() << " e: " << constituent->e()); 

    if( constituent->type() == xAOD::Type::CaloCluster ) {
	  const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>( constituent->rawConstituent() );
	  ANA_MSG_DEBUG("CaloCluster: ");
	  ANA_MSG_DEBUG("eta: " << cluster->eta() << " phi: " << cluster->phi() << " e: " << cluster->e());

	  clusterList.push_back(cluster);
    }
    else if ( constituent->type() == xAOD::Type::ParticleFlow ) {
	  const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( constituent->rawConstituent() );
	  
      if (pfo->isCharged()) continue;
	  if (pfo->nCaloCluster()!=1){
	    ANA_MSG_WARNING("Neutral PFO has " << std::to_string(pfo->nCaloCluster()) << " clusters, expected exactly 1!\n");
        continue;
      }

	  if (useSubtractedCluster){
	    ElementLink<xAOD::CaloClusterContainer> subClusLink;
	    pfo->attribute("PFOShowerSubtractedClusterLink", subClusLink);
	    if ( !subClusLink.isValid() ){
	      ANA_MSG_ERROR("Tau HelperFunctions: Found invalid link to shower subtracted cluster");
	      return StatusCode::FAILURE;
	    }
	    else {
	      clusterList.push_back( (*subClusLink) );
	    }
	  }
	  else {
	    clusterList.push_back(pfo->cluster(0));
	  }
	}
    else {
	  ANA_MSG_ERROR("GetJetConstCluster: Seed jet constituent type not supported!");
	  return StatusCode::FAILURE;
    }
  }

  if (useSubtractedCluster) return StatusCode::SUCCESS; 

  for (const xAOD::JetConstituent* constituent : constituents){
    if ( constituent->type() != xAOD::Type::ParticleFlow ) break;
	
    const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( constituent->rawConstituent() );
	if (! pfo->isCharged()) continue;

	for (u_int index=0; index<pfo->nCaloCluster(); index++){
	  const xAOD::CaloCluster* cluster = pfo->cluster(index);
	  // check it is not duplicate of one in neutral list
	  if ( std::find(clusterList.begin(), clusterList.end(), cluster) == clusterList.end() ) {
	    clusterList.push_back(cluster);
	  }
	}
  }

  return StatusCode::SUCCESS;
}
