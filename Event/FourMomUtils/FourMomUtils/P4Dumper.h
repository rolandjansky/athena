///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// P4Dumper.h
// Header file for class P4Dumper
// Author: S.Binet<binet@cern.ch>
///////////////////////////////////////////////////////////////////
#ifndef FOURMOMUTILS_P4DUMPER_H
#define FOURMOMUTILS_P4DUMPER_H

// STL includes
#include <iosfwd>

// FourMomUtils includes

namespace FourMomUtils {

  /// Helper to stream out a range of I4Momentum objects
  template <class I4MomIter>
  inline
  std::ostream& dump( std::ostream& out,
          const I4MomIter iBeg, const I4MomIter iEnd ) {
    for ( I4MomIter i = iBeg; i != iEnd; ++i ) {
      out << (**i) << "\n";
    }
    return out;
  }

  /// Helper to stream out a collection of I4Momentum objects
  template <class Container>
  inline
  std::ostream& dump( std::ostream& out, const Container& c ) {
    return dump( out, c.begin(), c.end() );
  }

} //> namespace FourMomUtils

#endif //> FOURMOMUTILS_P4DUMPER_H
