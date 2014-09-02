/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/HadronUtils.h"
#include <algorithm>
#include <string>
#include <sstream>

std::pair<short,short> HadronClass::type(int pdg) {
  short typeP = 0; // Flavor
  short typeH = 2; // 0 : meson ; 1 : baryon ; 2 : other
  std::ostringstream o; o << abs(pdg);
  std::string s = o.str();
  int ns = s.size();
  if (ns >= 4) {   
    if (s.at(ns-4) != '0') {
      typeH = 1;
      typeP = atoi(s.substr(ns-4,1).c_str());
    } else {
      typeH = 0;
      typeP = atoi(s.substr(ns-3,1).c_str());
    }  
  } else if ( ns == 3 ) {
    typeH = 0;    
    typeP = atoi(s.substr(0,1).c_str());  
  }

  return std::make_pair(typeH,typeP);
}

bool HadronClass::HeavyHadron(int pdg) {
  short typ  = type(pdg).second;
  short typh = type(pdg).first;
  return (typh < 2 && (typ == 4 || typ == 5));
}
