/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
  RoIData.cxx
*/
#include "TriggerMenuNtuple/RoIData.h"
#include <algorithm>
#include <iostream>

using namespace std;

bool isPassed(const std::string& chain_name, 
	      const std::vector<TrigStatus>& status_vec) {
  bool x = false;
  std::vector<TrigStatus>::const_iterator p;
  for (p=status_vec.begin(); p!=status_vec.end(); ++p) {
    if (p->name() == chain_name) {
      if (p->isPassed()) x = true;
      break;
    }
  }
  return x;
}

bool isAssociated(const std::string& chain_name, 
		  const std::vector<TrigStatus>& status_vec) {
  bool x = false;
  std::vector<TrigStatus>::const_iterator p;
  for (p=status_vec.begin(); p!=status_vec.end(); ++p) {
    if (p->name() == chain_name) {
      x = true;
      break;
    }
  }
  return x;
}

RoIData::RoIData(int /*create*/) {
  mAssociatedChains.clear();
  mPassedChains.clear();

//   if (create != 0) {
//     mAssociatedChains = new std::vector<std::string>();
//     mPassedChains = new std::vector<std::string>();
//   }
}

RoIData::~RoIData() {
//   if (mAssociatedChains) {
//     delete mAssociatedChains;
//     mAssociatedChains = 0;
//   }
//   if (mPassedChains) {
//     delete mPassedChains;
//     mPassedChains = 0;
//   }
}

bool RoIData::l2exist() const {
  return false;
}
float RoIData::l2pt() const {
  return 0.0;
}
float RoIData::l2eta() const {
  return 0.0;
}
float RoIData::l2phi() const {
  return 0.0;
}

bool RoIData::efexist() const {
  return false;
}
float RoIData::efpt() const {
  return 0.0;
}
float RoIData::efeta() const {
  return 0.0;
}
float RoIData::efphi() const {
  return 0.0;
}

bool RoIData::isAssociated(const std::string& cname) const {
  if (find(mAssociatedChains.begin(), 
	   mAssociatedChains.end(), cname) == mAssociatedChains.end()) {
    return false;
  } else {
    return true;
  }
}

bool RoIData::isPassed(const std::string& cname) const {
  if (find(mPassedChains.begin(), 
	   mPassedChains.end(), cname) == mPassedChains.end()) {
    return false;
  } else {
    return true;
  }
}

bool RoIData::isPassed(const std::string& chain_name, 
	      const std::vector<TrigStatus>& status_vec) {
  bool x = false;
  std::vector<TrigStatus>::const_iterator p;
  for (p=status_vec.begin(); p!=status_vec.end(); ++p) {
    if (p->name() == chain_name) {
      if (p->isPassed()) x = true;
      break;
    }
  }
  return x;
}

bool RoIData::isAssociated(const std::string& chain_name, 
		  const std::vector<TrigStatus>& status_vec) {
  bool x = false;
  std::vector<TrigStatus>::const_iterator p;
  for (p=status_vec.begin(); p!=status_vec.end(); ++p) {
    if (p->name() == chain_name) {
      x = true;
      break;
    }
  }
  return x;
}



std::string RoIData::sChainName="";

void RoIData::setChainToCheck(const string str){
  sChainName = str;
}

string RoIData::getChainToCheck(){
  return sChainName;
}

bool RoIData::isPassed() const{
  return isPassed(sChainName);
}
