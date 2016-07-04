/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HeavyMass2016Condition.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/CombinationsGen.h"


#include <sstream>
#include <cmath>
#include <TLorentzVector.h>
#include <set>
#include <vector>

bool in_mass_window(const std::vector<int>& indices,
                    const HypoJetVector& jets,
                    double mass_lo,
                    double mass_hi){

  std::vector<TLorentzVector> selected;
  auto first = jets.begin();

  TLorentzVector sumVector;
  for(auto i : indices){sumVector += (*(first+i))->p4();}

  auto mass = sumVector.M();
  return mass >= mass_lo and mass < mass_hi;
}

HeavyMass2016Condition::HeavyMass2016Condition(double innerMassMin0, 
                                                   double innerMassMax0,
                                                   double innerMassMin1, 
                                                   double innerMassMax1,
                                                   double outerMassMin,
                                                   double outerMassMax):
  m_innerMassMin0(innerMassMin0),
  m_innerMassMax0(innerMassMax0),
  m_innerMassMin1(innerMassMin1),
  m_innerMassMax1(innerMassMax1),
  m_outerMassMin(outerMassMin),
  m_outerMassMax(outerMassMax){
  }
  
bool HeavyMass2016Condition::isSatisfied(const HypoJetVector& ips) const {

  auto njets = ips.size();
  
  CombinationsGen combgen0(njets, 2);
  
  while(true){
    auto combs0 = combgen0.next();
    if(!combs0.second){break;} // combination indices are  invalid
    auto daughters0 = combs0.first;

    if(!in_mass_window(daughters0, ips, m_innerMassMin0, m_innerMassMax0)){
      continue;
    }

    CombinationsGen combgen1(njets, 2);
    while(true){
      auto combs1 = combgen1.next();
      if(!combs1.second){break;} // combination indices are  invalid
      auto daughters1 = combs1.first;
      
      if(!in_mass_window(daughters1, ips, m_innerMassMin1, m_innerMassMax1)){
        continue;
      }
        
      std::vector<int> all_indices(daughters0.begin(), daughters0.end());
      all_indices.insert(all_indices.end(),
                         daughters1.begin(),
                         daughters1.end());

      // remove daughter collections that share jets
      std::set<int> unique(all_indices.begin(), all_indices.end());
      if (unique.size() != all_indices.size()){continue;}

      return in_mass_window(all_indices, ips, m_outerMassMin, m_outerMassMax);
    }
  }
  return false;
}


std::string HeavyMass2016Condition::toString() const noexcept {
  std::stringstream ss;
  ss << " HeavyMass2016Condition:"
     << " inner mass window0 min "
     <<  m_innerMassMin0
     << " inner mass window0 max "
     <<  m_innerMassMax0
     << " inner mass window1 min "
     <<  m_innerMassMin1
     << " inner mass window1 max "
     <<  m_innerMassMax1
     << " outer mass window min "
     <<  m_outerMassMin
     << " outer mass window max "
     <<  m_outerMassMax
     <<'\n';

  return ss.str();
}


double HeavyMass2016Condition::orderingParameter() const noexcept {return 0.0;}
