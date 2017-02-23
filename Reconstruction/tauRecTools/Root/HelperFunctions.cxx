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

#ifdef XAODTAU_VERSIONS_TAUJET_V3_H
xAOD::TauJetParameters::PanTauDetails PANTAU_DECAYMODE=xAOD::TauJetParameters::PanTau_DecayMode;
#else
xAOD::TauJetParameters::PanTauDetails PANTAU_DECAYMODE=xAOD::TauJetParameters::pantau_CellBasedInput_DecayMode;
#endif

//______________________________________________________________________________
void tauRecTools::createPi0Vectors(const xAOD::TauJet* xTau, std::vector<TLorentzVector>& vPi0s)
{
  // reset the pi0s
  vPi0s.clear();

  // Since the PFO links as they come out of reconstruction, only correspond to
  // calorimeter clusters, whereas we want the consts_pi0 vectors to correspond
  // to real pi0s, we need to be careful to collect the PFOs correctly to pi0s
  // for the cases where number of pi0s does not match to the decay mode:
  size_t iNumPi0PFO = xTau->nPi0PFOs();

  int iDecayMode = -1;
  if (!(xTau->panTauDetail(PANTAU_DECAYMODE, iDecayMode)))
  {
    std::cerr << "Failed to retrieve panTauDetail decay mode\n";
    return;
  }
  
  if (iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1p1n && iNumPi0PFO > 1)
  {
    // TODO: find out if the pi0 mass is defined elsewhere in atlas code!
    // float fMassPi0 = 134.98;
    float fMassPi0Squared = 18219.6004;

    // combine both photons (with 0 mass from Pantau) to one pi0 vector:
    const xAOD::PFO* xPfo1 = xTau->pi0PFO(0);
    const xAOD::PFO* xPfo2 = xTau->pi0PFO(1);
    vPi0s.push_back(xPfo1->p4() + xPfo2->p4());

    // re-set the mass to one pi0:
    double dNewMomentum = std::sqrt(vPi0s[0].E() * vPi0s[0].E() - fMassPi0Squared);
    vPi0s[0].SetPxPyPzE(vPi0s[0].Vect().Unit().Px() * dNewMomentum,
                        vPi0s[0].Vect().Unit().Py() * dNewMomentum,
                        vPi0s[0].Vect().Unit().Pz() * dNewMomentum,
                        vPi0s[0].E());
  }
  else if (iDecayMode == xAOD::TauJetParameters::DecayMode::Mode_1pXn && iNumPi0PFO == 1)
  {
    // make a single pi0 from a PFO that contains two pi0s:
    const xAOD::PFO* xPfo = xTau->pi0PFO(0);
    // add the 2-pi0 vector preliminarily to the pi0vector:
    vPi0s.push_back(xPfo->p4());

    // re-set the mass back to one pi0:
    double dNewMomentum = std::sqrt(vPi0s[0].E() * vPi0s[0].E() - vPi0s[0].M() / 2. * vPi0s[0].M() / 2.);
    vPi0s[0].SetVectM(vPi0s[0].Vect() * (dNewMomentum / vPi0s[0].P()), vPi0s[0].M() / 2.);

    // create another pi0 from the same vector:
    vPi0s.push_back(vPi0s[0]);
  }
  else
  {
    // if it's not any of the special cases above then just collect the PFOs:
    for (size_t iPFO = 0; iPFO < iNumPi0PFO; iPFO++)
    {
      vPi0s.push_back(xTau->pi0PFO(iPFO)->p4());
    }
  }
}

xAOD::TauTrack::TrackFlagType tauRecTools::isolateClassifiedBits(xAOD::TauTrack::TrackFlagType flag){
  static const int flagsize=sizeof(flag)*8;
  flag=flag<<(flagsize-xAOD::TauJetParameters::classifiedFake-1);
  flag=flag>>(flagsize-xAOD::TauJetParameters::classifiedCharged+1);
  return flag;
}

//bool sortTracks(xAOD::TauTrack* xTrack1, xAOD::TauTrack* xTrack2)
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
  
  //this somehow causes a crash
  /* static uint16_t flag1 = xTrack1->flagSet() >> (xAOD::TauJetParameters::classifiedCharged - 1); */
  /* static uint16_t flag2 = xTrack2->flagSet() >> (uint16_t(xAOD::TauJetParameters::classifiedCharged) - 1); */
  /* return (flag1<flag2) ||                            // sort by type, true tracks first */
  /*   ((flag1==flag2) && (xTrack1->pt()>xTrack2->pt())); // sort by pt if equal types */
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
