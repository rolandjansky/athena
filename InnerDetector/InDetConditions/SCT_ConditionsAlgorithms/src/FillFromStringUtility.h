// -*- C++ -*-

/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FillFromStringUtility_h
#define FillFromStringUtility_h

// Include STL stuff
#include <limits>
#include <string>

// Include boost stuff
#include "boost/tokenizer.hpp"
#include "boost/lexical_cast.hpp"

namespace {
  template <typename C> 
  bool fillArrayFromString(const std::string& source, C& userContainer) {
    if (source.empty()) return false;
    typedef typename C::value_type V_t;
    V_t errVal{std::numeric_limits<V_t>::has_quiet_NaN ? (std::numeric_limits<V_t>::quiet_NaN()) : 0};
    boost::char_separator<char> sep{" "};
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    Tokenizer tok{source, sep};
    bool noNan{true};
    const Tokenizer::iterator end{tok.end()};
    int j{0};
    for (Tokenizer::iterator i{tok.begin()}; i!=end; ++i) {
      try {
        userContainer[j]=boost::lexical_cast<V_t>(*i);
      } catch (const boost::bad_lexical_cast&) {
        userContainer[j]=errVal;
        noNan=false;
      }
      ++j;
    }
    return noNan;
  }
  template <typename C> 
  bool fillEmptyVectorFromString(const std::string& source, C& userContainer) {
    if (source.empty()) return false;
    typedef typename C::value_type V_t;
    V_t errVal{std::numeric_limits<V_t>::has_quiet_NaN ? (std::numeric_limits<V_t>::quiet_NaN()) : 0};
    boost::char_separator<char> sep{" "};
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    Tokenizer tok{source, sep};
    bool noNan{true};
    const Tokenizer::iterator end{tok.end()};
    for (Tokenizer::iterator i{tok.begin()}; i!=end; ++i) {
      try {
        userContainer.push_back(boost::lexical_cast<V_t>(*i));
      } catch (const boost::bad_lexical_cast&) {
        userContainer.push_back(errVal);
        noNan=false;
      }
    }
    return noNan;
  }
} // end of anonymous namespace

#endif // FillFromStringUtility_h
