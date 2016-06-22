/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//Author: Russell Smith
//Email : rsmith@cern.ch
//Date  : April 2016

// Make sure that asserts work:
#undef NDEBUG

// System include(s):
#include <cassert>
#include <ctime>
#include <utility>

// Local include(s):
#include "xAODMissingET/MissingETAuxContainer.h"
#include "xAODMissingET/MissingETAssociationMap.h"
#include "xAODMissingET/MissingETContainer.h"


static const std::map<int, std::string>  metStringsMap = {
  {0, "RefEle"},
  {1, "RefGamma"},
  {2, "RefJet"},
  {3, "RefTau"},
  {4, "Muons"},
  {5, "SoftTrk"},
  {6, "Final"},
};

int testStringComparisonVersion(xAOD::MissingETContainer & cont){
  //std::cout << __PRETTY_FUNCTION__ << std::endl;
  for(size_t j = 0; j < 10000; ++j){
    for(size_t i = 0; i < cont.size(); ++i ){
      xAOD::MissingET * met = *(cont.find(metStringsMap.at(i)));
      assert(met);
    }
  }
  return 0;
}

int testHashComparisonVersion(xAOD::MissingETContainer & cont){
  //std::cout << __PRETTY_FUNCTION__ << std::endl;
  for(size_t j = 0; j < 10000; ++j){
    for(size_t i = 0; i < cont.size(); ++i ){
      //      xAOD::MissingET * met = *(cont.findByHash(metStringsMap.at(i)));
      //      assert(met);
    }
  }
  return 0;
}

double calcMean(std::vector<double> & times){
  //std::cout << __PRETTY_FUNCTION__ << std::endl;
  double sum = 0;
  for (auto i : times ){
    //std::cout << sum << std::endl;
    sum += i;
  }

  return sum/static_cast<double>(times.size());
}

int main(){std::cout << __PRETTY_FUNCTION__ << std::endl;

  std::pair<xAOD::MissingETContainer, xAOD::MissingETAuxContainer> cont;
  cont.first.setStore(& cont.second);

  std::vector<double> stringComparisonTimes;
  std::vector<double> hashComparisonTimes;

  for(size_t i = 0; i < 1000; ++i){
    for(size_t j = 0; j < 7; ++j){
      xAOD::MissingET * met = new xAOD::MissingET(metStringsMap.at(j));
      cont.first.push_back(met);
    }

    clock_t begin = clock();
    testStringComparisonVersion(cont.first);
    clock_t end = clock();

    cont.first.clear();

    clock_t begin2 = clock();
    testHashComparisonVersion(cont.first);
    clock_t end2 = clock();

    cont.first.clear();

    double stringTime = double(end   - begin );
    double hashTime   = double(end2  - begin2);
    // std::cout << "stringTime : " << stringTime << std::endl;
    // std::cout << "hashTime   : " << hashTime   << std::endl;
    stringComparisonTimes.push_back(stringTime);
    hashComparisonTimes  .push_back(hashTime);
  }

  std::cout << "Avg time to run testStringComparisonVersion : " << calcMean(stringComparisonTimes)/CLOCKS_PER_SEC << std::endl;
  std::cout << "Avg time to run testHashComparisonVersion   : " << calcMean(hashComparisonTimes  )/CLOCKS_PER_SEC << std::endl;

  return 0;
 }
