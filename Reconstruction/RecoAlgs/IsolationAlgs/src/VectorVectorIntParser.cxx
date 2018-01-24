///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Add a parser for vector<vector<int>>
/////////////////////////////////////////////////////////////////// 

// Includes needed for the custom type
#include <vector>
#include <string>
// A typedef may save a lot of mistakes
typedef std::vector<std::vector<int> > MyCustomType;

// Define the parser
#include "GaudiKernel/ParsersFactory.h"

namespace Gaudi
{
  namespace Parsers
  {

    // Parse function... nothing special, but it must be done explicitely.
    StatusCode parse( MyCustomType& result, const std::string& input ) { return parse_( result, input ); }
  }
}

// We also need to be able to print an object of our type as a string that both
// Python and our parser can understand,
#include "GaudiKernel/ToStream.h"
namespace std
{
  // This is an example valid for any mapping type.
  ostream& operator<<( ostream& s, const MyCustomType& vecvec )
  {
    s << '{';
    for ( const auto& vec : vecvec ) {
      Gaudi::Utils::toStream( vec, s );
    }
    s << '}';
    return s;
  }
}
