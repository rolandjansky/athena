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

namespace tauRecTools {
  ANA_MSG_SOURCE(msgHelperFunction, "HelperFunction")
}

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
std::unique_ptr<MVAUtils::BDT> tauRecTools::configureMVABDT( std::map<TString, float*> &availableVars, const TString& weightFile){
  std::unique_ptr<TFile> fBDT(TFile::Open(weightFile));
  if(!fBDT){
    std::cerr << "ERROR Cannot find tau input BDT file: " << weightFile << std::endl;
    return nullptr;
  }
  
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    std::cerr << "ERROR Cannot find tau input BDT tree" << std::endl;
    return nullptr;
  }    

  std::cout << "tauRecTools::configureMVABDT opened file: " << weightFile << std::endl;

  std::vector<float*> vars;

  //parsing of variables done here from TNamed object
  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    std::cerr << "ERROR no Variable List in file : " << weightFile << std::endl;
    return nullptr;
  }
  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  delete n_varList;

  for(const TString& str : varList_ar){
    if(str.Length()==0) continue;
    std::map<TString, float*>::iterator itr = availableVars.find(str);
    if(itr==availableVars.end()){
      std::cerr << "ERROR Variable : " << str << " is not available" << std::endl;
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
tauRecTools::TRTBDT::TRTBDT(const char* weightFile){
  this->bdt = nullptr;
  init(weightFile);
}

//________________________________________________________________________________
bool tauRecTools::TRTBDT::init(const char* weightFile){
  std::unique_ptr<TFile> fBDT(TFile::Open(weightFile));
  if(!fBDT){
    std::cerr << "ERROR Cannot find tau input BDT file: " << weightFile << std::endl;
    return 0;
  }
  
  TTree* tBDT = dynamic_cast<TTree*> (fBDT->Get("BDT"));
  if(!tBDT){
    std::cerr << "ERROR Cannot find tau input BDT tree" << std::endl;
    return 0;
  }    

  std::cout << "tauRecTools::configureMVABDT opened file: " << weightFile << std::endl;

  std::vector<float*> vars;

  TNamed* n_varList = dynamic_cast<TNamed*> (fBDT->Get("varList"));
  if(!n_varList) {
    std::cerr << "ERROR no Variable List in file : " << weightFile << std::endl;
    return 0;
  }

  std::vector<TString> varList_ar = tauRecTools::parseStringMVAUtilsBDT(n_varList->GetTitle());
  delete n_varList;
  
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

  this->bdt = std::make_unique<MVAUtils::BDT>(tBDT);
  this->bdt->SetPointers( vars );
  
  fBDT->Close();

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

const StatusCode tauRecTools::GetJetClusterList(const xAOD::Jet* jet, std::vector<const xAOD::CaloCluster*> &clusterList, bool incShowerSubtracted, TLorentzVector dRVector, double dRCut){

  using namespace tauRecTools::msgHelperFunction;
  // If using subtracted clusters, need to store unmodified to check if charged are duplicates
  std::vector<const xAOD::CaloCluster*> dupList;

  // Loop over jet constituents
  xAOD::JetConstituentVector jVec = jet->getConstituents();
  for(auto jCon : jVec){
    ANA_MSG_DEBUG("JetConstituent: ");
    ANA_MSG_DEBUG("eta: " << jCon->eta() << " phi: " << jCon->phi() << " e: " << jCon->e()); 

    // do deltaR check against jet constituent
    bool PassdR = true;
    if (dRCut > 0){
      TLorentzVector tempClusterVector;
      tempClusterVector.SetPtEtaPhiE( jCon->pt(), jCon->eta(), jCon->phi(), jCon->e() );
      ANA_MSG_DEBUG("Apply dR cut on JetConstituent: " << dRCut );
      ANA_MSG_DEBUG("JetConstituent Pt: " << tempClusterVector.Pt() << ", Eta: " << tempClusterVector.Eta() << ", Phi: " << tempClusterVector.Phi());
      ANA_MSG_DEBUG("dR " << dRVector.DeltaR(tempClusterVector));
      if (dRVector.DeltaR(tempClusterVector) > dRCut){
	ANA_MSG_DEBUG("Failed dR Cut ");
	PassdR = false;
      }
    }

    if (PassdR){
      if( jCon->type() == xAOD::Type::CaloCluster ) {
	const xAOD::CaloCluster* cluster = static_cast<const xAOD::CaloCluster*>( jCon->rawConstituent() );
	ANA_MSG_DEBUG("CaloCluster: ");
	ANA_MSG_DEBUG("eta: " << cluster->eta() << " phi: " << cluster->phi() << " e: " << cluster->e());
	ANA_MSG_DEBUG("rawEta: " << cluster->rawEta() << " rawPhi: " << cluster->rawPhi() << " rawE: " << cluster->rawE());
	ANA_MSG_DEBUG("calEta: " << cluster->calEta() << " calPhi: " << cluster->calPhi() << " calE: " << cluster->calE());

	clusterList.push_back(cluster);
      }
      else if( jCon->type() == xAOD::Type::ParticleFlow ) {
	const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( jCon->rawConstituent() );
	if( !pfo->isCharged() ){
	  if( pfo->nCaloCluster()==1 ){

	    if (incShowerSubtracted){
	      ElementLink<xAOD::CaloClusterContainer> subClusLink;
	      pfo->attribute("PFOShowerSubtractedClusterLink", subClusLink);
	      if ( !subClusLink.isValid() ){
		ANA_MSG_ERROR("Tau HelperFunctions: Found invalid link to shower subtracted cluster");
		return StatusCode::FAILURE;
	      }
	      else {
		clusterList.push_back( (*subClusLink) );
		dupList.push_back( pfo->cluster(0) );
	      }
	    }
	    else {
	      clusterList.push_back(pfo->cluster(0));
	    }

	  }
	  else ANA_MSG_WARNING("Neutral PFO has " << std::to_string(pfo->nCaloCluster()) << " clusters, expected exactly 1!\n");

	}// neutral PFO check
      }
      else{
	ANA_MSG_ERROR("GetJetConstCluster: Seed jet constituent type not supported!");
	return StatusCode::FAILURE;
      }
    }// dR check
  }

  // Get clusters from charged PFOs
  std::vector<const xAOD::CaloCluster*> checkList;
  if (incShowerSubtracted) checkList = dupList;
  else checkList = clusterList;

  for(auto jCon : jVec){

    if( jCon->type() == xAOD::Type::ParticleFlow ) {

      bool PassdR = true;
      if (dRCut > 0){
	TLorentzVector tempClusterVector;
	tempClusterVector.SetPtEtaPhiE( jCon->pt(), jCon->eta(), jCon->phi(), jCon->e() );
	ANA_MSG_DEBUG("Apply dR cut on JetConstituent: " << dRCut );
	ANA_MSG_DEBUG("JetConstituent Pt: " << tempClusterVector.Pt() << ", Eta: " << tempClusterVector.Eta() << ", Phi: " << tempClusterVector.Phi());
	ANA_MSG_DEBUG("dR " << dRVector.DeltaR(tempClusterVector));
	if (dRVector.DeltaR(tempClusterVector) > dRCut){
	  ANA_MSG_DEBUG("Failed dR Cut ");
	  PassdR = false;
	}
      }

      if (PassdR){
	const xAOD::PFO* pfo = static_cast<const xAOD::PFO*>( jCon->rawConstituent() );
	if( pfo->isCharged() ){

	  // loop through clusters linked to charged PFO
	  for (u_int cc=0; cc<pfo->nCaloCluster(); cc++){
	    const xAOD::CaloCluster* cluster = pfo->cluster(cc);
	    // check it is not duplicate of one in neutral list
	    if ( std::find(checkList.begin(), checkList.end(), cluster) == checkList.end() ){
	      clusterList.push_back(cluster);
	      checkList.push_back(cluster);
	    }
	  }

	}
      }// dR check
    }
  }// loop through jet constituents

  return StatusCode::SUCCESS;
}
