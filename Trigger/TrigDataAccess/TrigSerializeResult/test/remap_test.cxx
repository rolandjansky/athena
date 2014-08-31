/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../src/SerializeCommon.h"

#include <string>
#include <iostream>
int main() {
  using namespace std;
  using namespace TrigSerializeResult;
  // check invariance
  std::string remapped = remapToDictName("WhateverUnknownContainerClass");
  if ( remapped != "WhateverUnknownContainerClass" ) {
    cout << "ERROR remapping unknown class to another name: " << remapped << endl;    
    return -1;
  }
  cout << "OK remapping unknown class" << endl;    
  // check hardcoded
  remapped = remapToDictName("Analysis :: TauDetailsContainer");
  if ( remapped != "Analysis::TauDetailsContainer" ) {
    cout << "ERROR remapping of known hardoded name: " << remapped << endl;    
  }
  cout << "OK remapping of known hardoded name" << endl;

  // xOAD pattern
  remapped = remapToDictName("xAOD::TauJetContainer_v1");
  if ( remapped != "DataVector<xAOD::TauJet_v1>" ) {
    cout << "ERROR remapping of xAOD Container type name: " << remapped << endl;    
  }

  remapped = remapToDictName("xAOD::L2StandAloneMuonContainer_v1");
  if ( remapped != "DataVector<xAOD::L2StandAloneMuon_v1>" ) {
    cout << "ERROR remapping of xAOD Container type name: " << remapped << endl;    
  }
  cout << "OK remapping of xAOD Container type name" << endl;

  remapped = remapToDictName("xAOD::L2StandAloneMuonAuxContainer_v1");
  if ( remapped != "xAOD::L2StandAloneMuonAuxContainer_v1" ) {
    cout << "ERROR remapping of xAOD Aux names: " << remapped << endl;    
  }
  cout << "OK remapping of xAOD Aux type name" << endl;  

  return 0;
}
