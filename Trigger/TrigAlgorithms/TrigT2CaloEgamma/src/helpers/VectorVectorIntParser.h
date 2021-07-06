///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// VectorVectorIntParser.h
// Forward declarations for vector<vector<int>> parser
/////////////////////////////////////////////////////////////////// 
#ifndef TRIGT2CALOEGAMMA_VECTORVECTORINTPARSER_H
#define TRIGT2CALOEGAMMA_VECTORVECTORINTPARSER_H 1

// STL includes
#include <string>
#include <vector>


#include "GaudiKernel/StatusCode.h"
namespace Gaudi
{
  namespace Parsers
  {

    // A typedef may save a lot of mistakes
    typedef std::vector<std::vector<int> > VecVecInt_t;

    // Parse function... nothing special, but it must be done explicitely.
    StatusCode parse( VecVecInt_t & result, const std::string& input );
  }
}

#endif
