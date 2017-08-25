/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SiClusterizationToolNormalizationException_h
#define SiClusterizationToolNormalizationException_h

/**
 * 
 *
 *
**/ 
#include <stdexcept>

class SiClusterizationToolNormalizationException: public std::runtime_error{
public:
SiClusterizationToolNormalizationException () : std::runtime_error ("Input to function is out of range"){
  //nop
}






};
#endif

