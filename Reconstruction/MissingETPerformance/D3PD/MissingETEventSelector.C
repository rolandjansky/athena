/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MissingETEventSelector.h"
#include <iostream>
#include <fstream>
#include "TLorentzVector.h"
using namespace std;

MissingETEventSelector::MissingETEventSelector() {
  _data=0;
}

MissingETEventSelector::MissingETEventSelector(const MissingETEventSelector &other) {
  _data                  = other._data;
}

bool MissingETEventSelector::passesL1J15() {
  return (_data->L1_J15 == 1);
}

bool MissingETEventSelector::passes_hecf_quality1() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if (_data->jet_AntiKt4LCTopo_hecf->at(i) > (1.-_data->jet_AntiKt4LCTopo_LArQuality->at(i)) )  {return false;}
  }
  return true;
}

bool MissingETEventSelector::passes_hecf_quality2() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if ((_data->jet_AntiKt4LCTopo_hecf->at(i) > 0.3) && (_data->jet_AntiKt4LCTopo_LArQuality->at(i) > 0.3))  {return false;}
  }
  return true;
}


bool MissingETEventSelector::passes_fbadQCutAndEmFracCut() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if ((_data->jet_AntiKt4LCTopo_emfrac->at(i) >0.90) && (_data->jet_AntiKt4LCTopo_LArQuality->at(i) > 0.6)) {return false;}
  }
  return true;
}

bool MissingETEventSelector::passes_EmFracCut() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if (_data->jet_AntiKt4LCTopo_emfrac->at(i) <0.10)  {return false;}
  }
  return true;
}

bool MissingETEventSelector::passes_FmaxAndEtaCut() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if ((_data->jet_AntiKt4LCTopo_fracSamplingMax->at(i)>0.95) && (_data->jet_AntiKt4LCTopo_emscale_eta->at(i) < 2 )) {return false;}
  }
  return true;
}

//no more used in new cleaning 
bool MissingETEventSelector::passesN90AndHECFracCut() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if ((_data->jet_AntiKt4LCTopo_n90->at(i) < 6) && (_data->jet_AntiKt4LCTopo_hecf->at(i) > 0.8)) {return false;}
  }
  return true;
}
//no more used in new cleaning 
bool MissingETEventSelector::passes_JetTime() {
  for (Int_t i = 0; i < _data->jet_AntiKt4LCTopo_n; ++i) {
    if (_data->jet_AntiKt4LCTopo_emscale_pt->at(i)/1000. < 10.) {continue;}
    if (fabs(_data->jet_AntiKt4LCTopo_Timing->at(i)) > 50.) {return false;}
  }
  return true;
}

bool MissingETEventSelector::hasExactlyOneVertex() {
  unsigned int ngoodvtx = 0;
  int primary = 0;
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_type->at(i)==1 && _data->vxp_nTracks->at(i) > 4) {
      primary=1;
    }
  }
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_nTracks->at(i) > 4 && primary ==1){
    ngoodvtx++;
    }
  }
  return (ngoodvtx==1);
}

bool MissingETEventSelector::hasExactlyTwoVertex() {
  unsigned int ngoodvtx = 0;
  int primary = 0;
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_type->at(i)==1 && _data->vxp_nTracks->at(i) > 4) {
      primary=1;
    }
  }
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_nTracks->at(i) > 4 && primary ==1){
    ngoodvtx++;
    }
  }  
  return (ngoodvtx==2);
}

bool MissingETEventSelector::hasThreeAndMoreVertex() {
  unsigned int ngoodvtx = 0;
  int primary = 0;
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_type->at(i)==1 && _data->vxp_nTracks->at(i) > 4) {
      primary=1;
    }
  }
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_nTracks->at(i) > 4 && primary ==1){
    ngoodvtx++;
    }
  }  
  return (ngoodvtx>2); 
 
}



bool MissingETEventSelector::passesVertex() {
  unsigned int ngoodvtx = 0;
  for (Int_t i = 0; i < _data->vxp_n; ++i) {
    if (_data->vxp_type->at(i)==1 && _data->vxp_nTracks->at(i) > 4) {
      ngoodvtx++;
    }
  }
  return (ngoodvtx==1);
}

bool MissingETEventSelector::passesMBTSTime() {
  return (_data->collcand_passMBTSTime==1);
}
bool MissingETEventSelector::passesCaloTime() {
  return (_data->collcand_passCaloTime==1);
}

bool MissingETEventSelector::hasNoJets() {
  return (_data->jet_AntiKt6LCTopo_n == 0);
}

bool MissingETEventSelector::hasOneJet() {
  return (_data->jet_AntiKt6LCTopo_n > 0);
}

bool MissingETEventSelector::hasOneJetAbove20() {  
//cout << "njet antikt6 " <<_data->jet_AntiKt6LCTopo_n  << endl;
  for (Int_t i = 0; i < _data->jet_AntiKt6LCTopo_n; ++i) {
    if (_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000. > 20.) {return true;}
  //cout << "pt antikt6 " <<_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000.  << endl;
  }

  return false;
}

bool MissingETEventSelector::hasOnlyOneJetAbove20() {
unsigned int ngoodjet = 0;
  for (Int_t i = 0; i < _data->jet_AntiKt6LCTopo_n; ++i) {
  
    if (_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000. > 20.) {ngoodjet++;}
  }

  return (ngoodjet==1);
}
bool MissingETEventSelector::hasTwoJetAbove20() {

  unsigned int count = 0;

  for (Int_t i = 0; i < _data->jet_AntiKt6LCTopo_n; ++i) {
    if (_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000. > 20.) {count++;}
  }

  return (count==2);
}

bool MissingETEventSelector::hasTwoJetAbove30() {

  unsigned int count = 0;

  for (Int_t i = 0; i < _data->jet_AntiKt6LCTopo_n; ++i) {
    if (_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000. > 30.) {count++;}
  }

  return (count>1);
}

bool MissingETEventSelector::hasNoJetAbove20() {

  for (Int_t i = 0; i < _data->jet_AntiKt6LCTopo_n; ++i) {
    if (_data->jet_AntiKt6LCTopo_emscale_pt->at(i)/1000. > 20.) {return false;}
  }

  return true;
}
/*
bool MissingETEventSelector::hasWmunu(){
  float MMU = 105.658;
  for (int i=0; i<_data->mu_staco_n; i++){
   
  TLorentzVector mu1;
  mu1.SetXYZM(_data->mu_staco_px->at(i),_data->mu_staco_py->at(i),
  _data->mu_staco_pz->at(i),MMU);
  double mupt1 = mu1.Pt();
  double mueta1 = mu1.Eta();
  double muphi1=mu1.Phi();
 // cout << "sono qui pt " << mupt1<< " eta "<< mueta1<< endl;
  if (_data->mu_staco_isCombinedMuon->at(i)==0) return false;
  if (mupt1<20000) return false;
  if (fabs(mueta1)>2.4) return false;
  double met = sqrt (pow( _data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_etx,2)+
  pow( _data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_ety,2)); 
  if (_data->mu_staco_ptcone40->at(i)/mupt1 > 0.2) return false;
  
  double mT=sqrt(2*mupt1*met*(1-cos(muphi1-_data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_phi)));
  //cout << "sono qui pt " << mupt1<< " eta "<< mueta1<< "MET "<< met<< endl;
  //if (met<25000) return false;
  if (mT<40000)return false;
  return true;
  
  }
  return false;
}

bool MissingETEventSelector::hasWenu(){
  
  for (int i=0; i<_data->el_n; i++){
   
 
  double elpt1 = _data->el_pt->at(i);
  double eleta1 = _data->el_eta->at(i);
  double elphi1 = _data->el_phi->at(i);
 // cout << "sono qui pt " << elpt1<< " eta "<< eleta1<< endl;
  if (_data->el_author->at(i)!=1 &&_data->el_author->at(i)!=3 ) return false;
  if (elpt1<20000) return false;
  if (fabs(eleta1)>2.47) return false;
  if (fabs(_data->el_charge->at(i))!=1) return false;
  if (_data->el_tight->at(i)!=0) return false;
  double met = sqrt (pow( _data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_etx,2)+
  pow( _data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_ety,2)); 
 // if (_data->el_staco_ptcone40->at(i)/elpt1 > 0.2) return false;
  double mT=sqrt(2*elpt1*met*(1-cos(elphi1-_data->MET_RefFinal_LCW_NI_pt20_noSoftJets_eflow_phi)));
  if (mT<40000)return false;
  //cout << "sono qui pt ele" << elpt1<< " eta "<< eleta1<< "MET "<< met<< endl;
  //if (met<25000) return false;
  
  return true;
  
  }
  return false;
}
*/
