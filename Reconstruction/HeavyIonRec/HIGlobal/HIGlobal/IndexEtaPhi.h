/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef INDEXETAPHI_H
#define INDEXETAPHI_H

class IndexEtaPhi {
 public:
  int eta;
  int phi;
  IndexEtaPhi() {
    eta = 0;
    phi = 0;
  };
  IndexEtaPhi(int eta0, int phi0) {
    eta = eta0;
    phi = phi0;
  };

  ~IndexEtaPhi(){};
  
  bool operator ==(const IndexEtaPhi & indexEtaPhi0) {
    if( eta!=indexEtaPhi0.eta ) return false;
    if( phi==indexEtaPhi0.phi ) return true;
    if( (phi==153||phi==160) && (indexEtaPhi0.phi==153||indexEtaPhi0.phi==160) ) return true;
    if( (phi==155||phi==161) &&(indexEtaPhi0.phi==155||indexEtaPhi0.phi==161) ) return true;
    if( (phi==157||phi==162) &&(indexEtaPhi0.phi==157||indexEtaPhi0.phi==162) ) return true;
    if( (phi==159||phi==163) &&(indexEtaPhi0.phi==159||indexEtaPhi0.phi==163) ) return true;
    if( (phi==168||phi==164) &&(indexEtaPhi0.phi==168||indexEtaPhi0.phi==164) ) return true;
    if( (phi==170||phi==165) &&(indexEtaPhi0.phi==170||indexEtaPhi0.phi==165) ) return true;
    if( (phi==172||phi==166) &&(indexEtaPhi0.phi==172||indexEtaPhi0.phi==166) ) return true;
    if( (phi==174||phi==167) &&(indexEtaPhi0.phi==174||indexEtaPhi0.phi==167) ) return true;
    
    return false;
  }
  
  bool operator <(const IndexEtaPhi & indexEtaPhi0) {
    if( eta<indexEtaPhi0.eta ) return true;
    else if(eta==indexEtaPhi0.eta ) return phi<indexEtaPhi0.phi;
    return false;
  }
  
};

#endif 
