/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// C/C++
#include <cmath>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

// Local
#include "TrigCostRate/RateABC.h"
#include "TrigCostRate/RateBand.h"

using namespace std;

//-----------------------------------------------------------------------------
Anp::RateBand::RateBand()
  :fDebug(false)
{
}

//-----------------------------------------------------------------------------
Anp::RateBand::~RateBand()
{
}

//-----------------------------------------------------------------------------
void Anp::RateBand::ConfigReg(const Registry &reg)
{
  //
  // Configure self
  //
  reg.Get("RateBand", "Debug", fDebug);
  if(reg.KeyExists("RateBand::OverlapKey")) {
    reg.Get("RateBand::OverlapKey", fOlapKey);
  }
  else {    
    fOlapKey = "EF_BW";
  }
  
  fReg = reg;

  cout << "RateBand::OverlapKey = " << fOlapKey << endl;
}

//-----------------------------------------------------------------------------
void Anp::RateBand::SetRateAlg(RateABC &rate)
{
  //
  // Process input rate groups
  //
  vector<Handle<RateGroup> > groups  = rate.GetRateGroups();
  fUnions = rate.GetRateUnions();
  cout << "Got " << fUnions.size() << " unions" << endl;
  
  vector<pair<int,Handle<RateGroup> > >bwGroups;
  for(unsigned int i = 0; i < groups.size(); ++i) {
    Handle<RateGroup> group = groups[i];
    if(group->GetName().find(fOlapKey) != string::npos) { 
      fGroups.push_back(group);
    }
  }
  
  for(unsigned int i = 1; i <= fGroups.size(); ++i) {
    GroupMap tempOlaps;
    MakeOverlaps(i, tempOlaps);
    fbwOlaps.insert(tempOlaps.begin(), tempOlaps.end());
  } 


  if(fDebug) {
    for(GroupMap::const_iterator itr = fbwOlaps.begin(); itr != fbwOlaps.end(); ++itr) {
      cout << "RateBand::SetRateAlg - made overlap group " << itr->first  << endl;
    } 
  }
  
}


//-----------------------------------------------------------------------------
void Anp::RateBand::Finalize(Registry &)
{
  cout << "In RateBand::Finalize" << endl;
  //
  // Compute and write results
  //
  map<string, double> BWrates;
  
  for(unsigned i = 0; i < fGroups.size(); ++i) {
    BWrates.insert(make_pair(fGroups[i]->GetName(), fGroups[i]->GetPassRate().first));
  }

  for(unsigned i = 0; i < fUnions.size(); ++i) {
    Handle<RateGroup> urate = fUnions[i];

    if(urate->GetLevel() != 3) {
      continue;
    }
    string name1 = urate->GetGroup1()->GetName();
    string name2 = urate->GetGroup2()->GetName();

    if(name1.find("BW") == string::npos ||
       name2.find("BW") == string::npos ) {
      continue;
    }
    if( BWrates.find(name1) == BWrates.end() ) {
      cout << "RateBand::Finalize - " << name1 << " not found in bandwidth groups" << endl;
      continue;
    }
    if( BWrates.find(name2) == BWrates.end() ) {
      cout << "RateBand::Finalize - " << name2 << " not found in bandwidth groups" << endl;
      continue;
    }
   
    float overlapCorr = GetCorrFrac(name1, name2); 
    double rate1 = (*std::find(fGroups.begin(), fGroups.end(), name1))->GetPassRate().first;
    double rate2 = (*std::find(fGroups.begin(), fGroups.end(), name2))->GetPassRate().first;
    double overlapRate = rate1 + rate2 - urate->GetPassRate().first; 

    if(fDebug) {
      cout << "Overlap chain, chain1, chain 2, rate1, rate2, OR = " << urate->GetName() << ", " << name1 << ", " << name2 << ", " << rate1 << ", " << rate2 << ", " << urate->GetPassRate().first << endl;
      cout << "Correcting " << name1 << ", " << BWrates[name1] << " to " << BWrates[name1]-overlapCorr*overlapRate << endl;
      cout << "Correcting " << name2 << ", " << BWrates[name2] << " to " << BWrates[name2]-(1-overlapCorr)*overlapRate << endl;
    }
    //do the correction
    BWrates[name1] -= overlapCorr*overlapRate;
    BWrates[name2] -= (1-overlapCorr)*overlapRate;


  }
    
  for(unsigned i = 0; i < fGroups.size(); ++i) {
    Handle<RateGroup> group = fGroups[i];

    if(BWrates.find(group->GetName()) == BWrates.end()) {
      continue;
    }

    const pair<double,double> rawRate = group->GetPassRate();
    const pair<string,string> roundrate = Anp::Round2Pair(rawRate.first,rawRate.second);
    const pair<string,string> roundrate_corr = Anp::Round2Pair(BWrates[group->GetName()],rawRate.second);
  fbwCorr.push_back(pair<string,string>(group->GetName(), roundrate.first));
  fbwCorr.push_back(pair<string,string>(group->GetName()+"_Corr", roundrate_corr.first));
  }   
  
  int maxOlapOrder = fGroups.size();
  for(int order = maxOlapOrder; order > 1; --order) { //start with the highest order of overlap, and go down
    for(unsigned int j = 0; j < fOlapGroups.size(); ++j) { //loop over the overlap groups
      const Handle<RateGroup> & group1 = fOlapGroups[j];
      string name = group1->GetName();
      
      if(FindOrder(name) != order) continue;
      
      int parity = 0;
      if(order%2==0) parity = -1;
      else       parity = 1;
      
      set<string> calc_keys;
      Tokenize(name, calc_keys);
     
       
      double olap_rate = parity*(group1->GetPassRate().first);
      double olap_err2 = (group1->GetPassRate().second)*(group1->GetPassRate().second);
      for(unsigned int k = 0; k < fOlapGroups.size(); ++k) { //second loop over the overlap groups
        const Handle<RateGroup> &group2 = fOlapGroups[k];
        int order2 = FindOrder(group2->GetName());
        if(order2 >= order) continue; //only groups of lower order are needed
        
        set<string> lower_keys;
        Tokenize(group2->GetName(),lower_keys);
        
        //if second overlap group has all groups of the first, include it
        if(std::includes(calc_keys.begin(), calc_keys.end(), lower_keys.begin(), lower_keys.end())) {
          
          int sign = 0;
          if((order2 + order)%2==0) { //if orders are both even or both odd, they have the same parity
            sign = parity;
          }
          else { //if one order is odd and the other is even, they have opposite parity
            sign = -1*parity;
          }
          
          olap_rate += sign*(group2->GetPassRate().first);
          olap_err2 += (group2->GetPassRate().second)*(group2->GetPassRate().second);
        }
      }
      const pair<string,string> rrate = Anp::Round2Pair(olap_rate, sqrt(olap_err2));
      fOlapCalc.push_back(make_pair(vector<string>(calc_keys.begin(), calc_keys.end()), rrate.first));
    }
  } 
  cout << "RateBand::Finalize - size of overlaps = " << fbwOlaps.size() << endl;  
}


//
// Recursive funciton to make all unique overlap groups consisting of order number of groups
// 
void Anp::RateBand::MakeOverlaps(int order, 
				 GroupMap & OlapGroups, 
				 vector<Handle<RateChain> > chains, 
				 const string & name, 
				 int index)
{
  for(unsigned int i = index; i < fGroups.size(); ++i) {
    if(order == 1) {
      //make group
      
      string new_name = name + fGroups[i]->GetName();
      vector<Handle<RateChain> > new_chains = fGroups[i]->GetChains();
      new_chains.insert(new_chains.end(), chains.begin(), chains.end());
      OlapGroups.insert(make_pair(new_name, new_chains));
    }
    else { //call another instance
      string new_name = name + fGroups[i]->GetName() + "_AND_";
      vector<Handle<RateChain> > new_chains = fGroups[i]->GetChains();
      new_chains.insert(new_chains.end(), chains.begin(), chains.end());
      MakeOverlaps(order-1, OlapGroups, new_chains, new_name, i+1);
    }
  }

}

void Anp::RateBand::Tokenize(const string &name, set<string> & tok) const
{
  stringstream ss;
  ss.str(name);
  string temp;
  while (getline(ss, temp, '_')) {
    if(temp != "AND" && fOlapKey.find(temp)==string::npos) {
      tok.insert(temp);
    }
  }
}

int Anp::RateBand::FindOrder(const string & name) const {
  unsigned order = 0;
  size_t index = 0;
  while( index != string::npos) {
    index = name.find("AND", index + (index == 0 ? 0 : 3));
    order++;
  }

  return order;
}

double Anp::RateBand::GetCorrFrac(const std::string &gr1, const std::string &gr2) const {

  float overlap = 0.5;

  if(gr1.find("Egamma") != string::npos) {
    if(gr2.find("Jet")       != string::npos || 
       gr2.find("BJet")      != string::npos || 
       gr2.find("MissingEt") != string::npos ||
       gr2.find("Tau")       != string::npos) {
      overlap = 0.666666;
    }
    else if(gr2.find("Muon") != string::npos) {
      overlap = 0.5;
    }
  }
  else if(gr1.find("Muon") != string::npos) {
    if(gr2.find("Jet")       != string::npos || 
       gr2.find("BJet")      != string::npos || 
       gr2.find("MissingEt") != string::npos ||
       gr2.find("Tau")       != string::npos) {
      overlap = 0.666666;
    }
    else if(gr2.find("EGamma")  != string::npos) {
      overlap = 0.5;
    }
  }
  else if(gr1.find("BJet") != string::npos) {
    if(gr2.find("Jet") != string::npos) {
      overlap = 0.333333;
    }
    else if(gr2.find("Muon")    != string::npos ||
            gr2.find("EGamma")  != string::npos) {
      overlap = 0.333333;
    }
  }
  else if(gr1.find("Jet") != string::npos && gr1.find("BJet") == string::npos) {
    if(gr2.find("BJet") != string::npos) {
      overlap = 0.666666;
    }
    else if(gr2.find("Muon")    != string::npos ||
            gr2.find("EGamma")  != string::npos) {
      overlap = 0.333333;
    }
  } 
  return overlap;
}
