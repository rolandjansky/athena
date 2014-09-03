/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArTPCnv/LArRawChannel_p1.h"

// LArRawChannel_p1, implementation of persistent class
// just needed for debug output
// author R.Seuster <seuster@cern.ch>

#include <iostream>
using namespace std;

std::ostream& operator << ( std::ostream& sl, const  LArRawChannel& data)
{
  sl << "LArRawChannel ("<< &data <<") "
     << "ID=" << data.identify()
     << ", energy="  << data.energy()
     << ", time="    << data.time()
     << ", quality=" << data.quality()
     << ", gain="    << (unsigned int)data.gain();
  return sl;
}
