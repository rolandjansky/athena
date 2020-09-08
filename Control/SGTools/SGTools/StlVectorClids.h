///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// StlVectorClids.h 
// Header file for CLIDs for various std::vector<T>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SGTOOLS_STLVECTORCLIDS_H 
#define SGTOOLS_STLVECTORCLIDS_H 

#include "AthenaKernel/CLASS_DEF.h"
#include <vector>
#include <string>
#include <stdint.h>

CLASS_DEF( std::vector<bool> ,            45822813 , 1 )
CLASS_DEF( std::vector<short> ,           35706084 , 1 )
CLASS_DEF( std::vector<int> ,             22592129 , 1 )
CLASS_DEF( std::vector<long> ,             6675975 , 1 )
CLASS_DEF( std::vector<float> ,          202242136 , 1 )
CLASS_DEF( std::vector<double> ,         219400222 , 1 )
CLASS_DEF( std::vector<std::string> ,     25884436 , 1 )
CLASS_DEF( std::vector<char> , 206378829 , 1 )

CLASS_DEF( std::vector<unsigned short> , 238793770 , 1 )
CLASS_DEF( std::vector<unsigned int> ,   177480351 , 1 )
CLASS_DEF( std::vector<unsigned long> ,      18868981 , 1 )
CLASS_DEF( std::vector<unsigned long long> , 35119468 , 1 )

CLASS_DEF( std::vector<std::vector<bool> > ,              255356807 , 1 )
CLASS_DEF( std::vector<std::vector<char> > ,               70698075 , 1 )
CLASS_DEF( std::vector<std::vector<short> > ,              24204906 , 1 )
CLASS_DEF( std::vector<std::vector<int> > ,                23262399 , 1 )
CLASS_DEF( std::vector<std::vector<uint8_t> > ,           126048273 , 1 )
CLASS_DEF( std::vector<std::vector<unsigned short> > ,    247252788 , 1 )
CLASS_DEF( std::vector<std::vector<unsigned int> > ,      213985193 , 1 )
CLASS_DEF( std::vector<std::vector<unsigned long> > ,     205717523 , 1 )
CLASS_DEF( std::vector<std::vector<unsigned long long> > , 76977490 , 1 )
CLASS_DEF( std::vector<std::vector<float> > ,        258753314 , 1 )
CLASS_DEF( std::vector<std::vector<double> > ,       154043868 , 1 )
CLASS_DEF( std::vector<std::vector<std::string> > ,  101141770 , 1 )

CLASS_DEF( std::vector<std::vector<std::vector<int> > > ,   85220325 , 1 )
CLASS_DEF( std::vector<std::vector<std::vector<float> > > , 82469196 , 1 )

#endif // !SGTOOLS_STLVECTORCLIDS_H
