/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "xAODPrimitives/IsolationType.h"
#include "xAODPrimitives/IsolationConeSize.h"
#include "xAODPrimitives/IsolationFlavour.h"
#include "xAODPrimitives/IsolationCorrection.h"
#include "xAODPrimitives/IsolationCorrectionHelper.h"
#include "xAODPrimitives/IsolationHelpers.h"
#include <iostream>
#include <algorithm>
#include <vector>

unsigned int encode();
unsigned int decode();
unsigned int testCaloBitset();
unsigned int testTrackBitset();

using namespace xAOD;

int main() {
  
  std::cout << " testing encoding " << std::endl;
  unsigned int nfailures = encode();

  nfailures += decode();
  
  std::cout << " total number of failures " << nfailures << std::endl;

  std::cout << " testing calo bitset " << std::endl; 
  unsigned int nfailures2 = testCaloBitset();
  std::cout << " total number of failures " << nfailures2 << std::endl;

  std::cout << " testing track bitset " << std::endl; 
  unsigned int nfailures3 = testTrackBitset();
  std::cout << " total number of failures " << nfailures3 << std::endl;

  return ( nfailures+nfailures2+nfailures3 != 0 ) ? -1 : 0;
}

bool check( bool result, const std::string& message, Iso::IsolationType type, unsigned int& nfailures ){
  if( !result ) {
    ++nfailures;
    std::cout << "WARNING " << message << " for type " << type << std::endl;
  }
  return result;
}


unsigned int encode() {
  unsigned int nfailures = 0;
  for( int i = 0 ; i <  static_cast<int>(Iso::IsolationType::numIsolationTypes) ; ++i ){
    // current number of failures
    unsigned int startFailures = nfailures;

    // skip invalid entries
    if( i%10 >= static_cast<int>(Iso::IsolationConeSize::numIsolationConeSizes) ) continue;
    Iso::IsolationType type = static_cast<Iso::IsolationType>(i);

    // decode and check cone size
    Iso::IsolationConeSize coneSize = Iso::enumconeSize(type);
    check( coneSize <= Iso::IsolationConeSize::numIsolationConeSizes, "invalid cone size",type,nfailures);

    float fsize = Iso::coneSize(coneSize);
    check (Iso::coneSize(fsize)==coneSize, "Cone size conversion to float & back failed",type,nfailures);

    // decode and check flavour
    Iso::IsolationFlavour flavour = Iso::isolationFlavour(type);
    check( flavour <= Iso::IsolationFlavour::numIsolationFlavours, "invalid flavour",type,nfailures);
 
    // encode isolation type
    Iso::IsolationType typeE = Iso::isolationType(flavour,coneSize);
    check( typeE == type, "inconsistent conversion of type",type,nfailures);
    
    if( startFailures != nfailures ){
      std::cout << "   detected a failure with type " << static_cast<int>(type) << " typeE " << static_cast<int>(typeE)
                << "   coneSize " << static_cast<int>(coneSize) << " flavour " << static_cast<int>(flavour) << std::endl;
    }
  }
  return nfailures;
}

unsigned int decode() {
  return 0;
}

bool compareBitsets( const std::vector<Iso::IsolationCaloCorrection>& vec1, const std::vector<Iso::IsolationCaloCorrection>& vec2 ){
  if( vec1.size() != vec2.size() ){
    std::cout << " incompatible sizes " << vec1.size() << " " << vec2.size() << std::endl;
    return false;
  }
  unsigned int nfound = 0;
  for( auto item : vec1 ){
    auto pos = std::find(vec2.begin(),vec2.end(),item);
    if( pos != vec2.end() ) ++nfound;
  }
  if( nfound != vec1.size() ){
    std::cout << " not all elements found " << nfound << " total " << vec1.size() <<std::endl;
    return false;
  }
  return true;
}

bool compareBitsets( const std::vector<Iso::IsolationTrackCorrection>& vec1, const std::vector<Iso::IsolationTrackCorrection>& vec2 ){
  if( vec1.size() != vec2.size() ){
    std::cout << " incompatible sizes " << vec1.size() << " " << vec2.size() << std::endl;
    return false;
  }
  unsigned int nfound = 0;
  for( auto item : vec1 ){
    auto pos = std::find(vec2.begin(),vec2.end(),item);
    if( pos != vec2.end() ) ++nfound;
  }
  if( nfound != vec1.size() ){
    std::cout << " not all elements found " << nfound << " total " << vec1.size() <<std::endl;
    return false;
  }
  return true;
}

unsigned int testCaloBitset() {
  unsigned int nfailures = 0;
  for( int i=0;i<static_cast<int>(Iso::IsolationCaloCorrection::numIsolationCaloCorrections);++i ){
    Iso::IsolationCaloCorrection cori = static_cast<Iso::IsolationCaloCorrection>(i);
    
    std::vector<Iso::IsolationCaloCorrection> vec1 = {cori};
    std::vector<Iso::IsolationCaloCorrection> results;

    Iso::IsolationCaloCorrectionBitset bitset;
    Iso::IsolationCaloCorrectionBitsetHelper::encode(vec1,bitset);
    Iso::IsolationCaloCorrectionBitsetHelper::decode(bitset,results);
    if(!compareBitsets(vec1,results)) ++nfailures;
    for( int j=i+1;j<static_cast<int>(Iso::IsolationCaloCorrection::numIsolationCaloCorrections);++j ){
      Iso::IsolationCaloCorrection corj = static_cast<Iso::IsolationCaloCorrection>(j);
      std::vector<Iso::IsolationCaloCorrection> vec2 = vec1;
      vec2.push_back(corj);
      Iso::IsolationCaloCorrectionBitsetHelper::encode(vec2,bitset);
      Iso::IsolationCaloCorrectionBitsetHelper::decode(bitset,results);
      if(!compareBitsets(vec2,results)) ++nfailures;
      
      for( int k=j+1;k<static_cast<int>(Iso::IsolationCaloCorrection::numIsolationCaloCorrections);++k ){
        Iso::IsolationCaloCorrection cork = static_cast<Iso::IsolationCaloCorrection>(k);
        std::vector<Iso::IsolationCaloCorrection> vec3 = vec2;
        vec3.push_back(cork);
        Iso::IsolationCaloCorrectionBitsetHelper::encode(vec3,bitset);
        Iso::IsolationCaloCorrectionBitsetHelper::decode(bitset,results);
        if(!compareBitsets(vec3,results)) ++nfailures;
      }
    }
  }
  return nfailures;
}

unsigned int testTrackBitset() {
  unsigned int nfailures = 0;
  for( int i=0;i<static_cast<int>(Iso::IsolationTrackCorrection::numIsolationTrackCorrections);++i ){
    Iso::IsolationTrackCorrection cori = static_cast<Iso::IsolationTrackCorrection>(i);
    
    std::vector<Iso::IsolationTrackCorrection> vec1 = {cori};
    std::vector<Iso::IsolationTrackCorrection> results;

    Iso::IsolationTrackCorrectionBitset bitset;
    Iso::IsolationTrackCorrectionBitsetHelper::encode(vec1,bitset);
    Iso::IsolationTrackCorrectionBitsetHelper::decode(bitset,results);
    if(!compareBitsets(vec1,results)) ++nfailures;
    for( int j=i+1;j<static_cast<int>(Iso::IsolationTrackCorrection::numIsolationTrackCorrections);++j ){
      Iso::IsolationTrackCorrection corj = static_cast<Iso::IsolationTrackCorrection>(j);
      std::vector<Iso::IsolationTrackCorrection> vec2 = vec1;
      vec2.push_back(corj);
      Iso::IsolationTrackCorrectionBitsetHelper::encode(vec2,bitset);
      Iso::IsolationTrackCorrectionBitsetHelper::decode(bitset,results);
      if(!compareBitsets(vec2,results)) ++nfailures;
      
      for( int k=j+1;k<static_cast<int>(Iso::IsolationTrackCorrection::numIsolationTrackCorrections);++k ){
        Iso::IsolationTrackCorrection cork = static_cast<Iso::IsolationTrackCorrection>(k);
        std::vector<Iso::IsolationTrackCorrection> vec3 = vec2;
        vec3.push_back(cork);
        Iso::IsolationTrackCorrectionBitsetHelper::encode(vec3,bitset);
        Iso::IsolationTrackCorrectionBitsetHelper::decode(bitset,results);
        if(!compareBitsets(vec3,results)) ++nfailures;
      }
    }
  }
  return nfailures;
}


