#include "TrigHTTObjects/HTTOfflineTrack.h"
#include <iostream>

ClassImp(HTTOfflineTrack)


std::ostream& operator<<(std::ostream& s, const HTTOfflineTrack& h) {
 
  s << "pt "       << h.pt()
    << " \t eta "  << h.eta()
    << " \t phi "  << h.phi() <<std::endl;

  std::vector<HTTOfflineHit> hits=h.hits();
  for ( int j=0 ; j<h.nHits() ; j++ ) { 
    s << " " << j << "  " << hits[j] << "\n";
  }
  s<<std::endl;
  
  return s;
}
