///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// StlMapClids.h 
// Header file for CLIDs for various std::map<T,U>
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef SGTOOLS_STLMAPCLIDS_H 
#define SGTOOLS_STLMAPCLIDS_H 

#include "AthenaKernel/CLASS_DEF.h"
#include <map>
#include <string>
#include <vector>

CLASS_DEF2( std::map<int,int> ,    103402598 , 1 )
CLASS_DEF2( std::map<int,float> ,  256222847 , 1 )
CLASS_DEF2( std::map<int,double> , 235483215 , 1 )

CLASS_DEF2( std::map<std::string,int> ,          170542827 , 1 )
CLASS_DEF2( std::map<std::string,unsigned int> , 113556225 , 1 )
CLASS_DEF2( std::map<std::string,float> ,        193758082 , 1 )
CLASS_DEF2( std::map<std::string,double> ,       252602412 , 1 )
CLASS_DEF2( std::map<std::string,std::string> , 143489258 , 1 )

CLASS_DEF2( std::map<std::string,std::vector<int> > ,          181049313 , 1 )
CLASS_DEF2( std::map<std::string,std::vector<unsigned int> > , 199376827 , 1 )
CLASS_DEF2( std::map<std::string,std::vector<float> > ,        259112316 , 1 )
CLASS_DEF2( std::map<std::string,std::vector<double> > ,        78422682 , 1 )

#endif // !SGTOOLS_STLMAPCLIDS_H
