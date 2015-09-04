/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "VnHypoHelper.h"


int VnHypoHelper::getCentBin( double et ) const {
  for( std::size_t i = 0; i < centcuts.size(); i++ ) {
    if( et > centcuts[i]) return i;
  }
  return -1;
}

float VnHypoHelper::getThreshold( int centBin ) const {
  if ( centBin < 0 ) return 1.e9; // theshold impossible to pass  
  return thresholds.at(centBin);
}



bool VnHypoHelper::valid() const {
  // size check for now but can be extend wiht some other helth checks
  return centcuts.size() == thresholds.size() and centcuts.size() == qxshifts.size() and centcuts.size() == qyshifts.size();
}

float VnHypoHelper::getQxShift( int centBin ) const {
  if ( centBin < 0 ) return 0;
  return qxshifts.at(centBin);
  
}

float VnHypoHelper::getQyShift( int centBin ) const {
  if ( centBin < 0 ) return 0;
  return qyshifts.at(centBin);
}



