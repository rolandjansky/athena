/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventAthenaPool/CscRawData_p1.h"
#include <iostream>

std::ostream& operator << ( std::ostream& sl, const  CscRawData_p1& data)
{
   sl << "CscRawData ("<< &data <<") "
      << "ID=" << data.identify()
      << ", address=" << data.getAddress()
      << ", dpuID=" <<  data.dpuID()
      << ", amps=[";
   for( unsigned i=0; i<data.getSamples().size(); ++i ) {
      sl << data.getSamples()[i] << ",";
   }
   sl << "]";
   return sl;
}




