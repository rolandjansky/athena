// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//
// This file is part of MCUtils -- https://bitbucket.org/andybuckley/mcutils
// Copyright (C) 2013-2014 Andy Buckley <andy.buckley@cern.ch>
//
// Embedding of MCUtils code in other projects is permitted provided this
// notice is retained and the MCUtils namespace and include path are changed.
//
#pragma once

/// @file Utility functions
/// @author Andy Buckley <andy.buckley@cern.ch>

// Provide convenience BOOST_FOREACH loop -- particularly nice for HepMC walking combined with the functions below
#include "boost/foreach.hpp"

namespace MC {


  /// @name Container utils
  //@{

  /// Return true if f(x) is true for any x in container c, otherwise false.
  template <typename CONTAINER, typename FN>
  inline bool any(const CONTAINER& c, const FN& f) {
    //for (const typename CONTAINER::value_type& val : c) {
    BOOST_FOREACH (const typename CONTAINER::value_type& val, c) {
      if (f(val)) return true;
    }
    return false;
  }

  /// Return true if f(x) is true for all x in container c, otherwise false.
  template <typename CONTAINER, typename FN>
  inline bool all(const CONTAINER& c, const FN& f) {
    //for (const typename CONTAINER::value_type& val : c) {
    BOOST_FOREACH (const typename CONTAINER::value_type& val, c) {
      if (!f(val)) return false;
    }
    return true;
  }

  /// Return true if x is found in container c, otherwise false.
  template <typename CONTAINER, typename T>
  inline bool contains(const CONTAINER& c, const T& x) {
    BOOST_FOREACH (const typename CONTAINER::value_type& val, c) {
      if (val == x) return true;
    }
    return false;
  }


  //@}


}
