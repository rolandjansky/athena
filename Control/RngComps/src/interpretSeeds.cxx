/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "interpretSeeds.h"

//get the luxury level, if specified. In that case moves token past it
inline
void getLuxury( boost::tokenizer<boost::char_separator<char> >::iterator& token, short& luxLevel) {
  if ((*token) == "LUXURY") {
    luxLevel = boost::lexical_cast<uint32_t>(*(++token));
    ++token; 
  }
}

//get the luxury level, if specified. In that case moves token past it
inline
void getOffset( boost::tokenizer<boost::char_separator<char> >::iterator& token, uint32_t& offset) {
  if ((*token) == "OFFSET") {
    offset = boost::lexical_cast<uint32_t>(*(++token));
    ++token; 
  }
}

bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, uint32_t& seed1, uint32_t& seed2, short& luxury, uint32_t& offset)
{
  //split the space-separated string in 3 or 5 or 7 words:	
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ");
  tokenizer tokens(buffer, sep);
  int nToks(distance(tokens.begin(), tokens.end()));
  bool status = (nToks == 3 || nToks == 5 || nToks == 7);
  if (status) {
    tokenizer::iterator token(tokens.begin());
    stream = *token++;
    //FIXME, try permutations by hand. With more than two we'd need a parser
    getOffset(token, offset);
    getLuxury(token, luxury);
    getOffset(token, offset);

    try {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
      seed1 = boost::lexical_cast<uint32_t>(*token++);
      seed2 = boost::lexical_cast<uint32_t>(*token++);
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
    } catch (const boost::bad_lexical_cast& e) {
      status = false;
    }
  }
  return status;
}

bool interpretSeeds(const std::string& buffer, 
		    std::string& stream, std::vector<uint32_t>& seeds) 
{
  //split the space-separated string in 31 or 33 words:	
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep(" ");
  tokenizer tokens(buffer, sep);
  int nToks(distance(tokens.begin(), tokens.end()));
  bool status = (nToks == 31 || nToks == 33 || nToks == 771);
  if (status) {
    tokenizer::iterator token(tokens.begin());
    stream = *token++;
    --nToks;
    try {
      if (nToks == 32) nToks=30; //ranlux (FIXME NEEDED?)
      for (int i=0; i<nToks; i++) {
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
	seeds.push_back(boost::lexical_cast<uint32_t>(*token++));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
      }
    } catch (const boost::bad_lexical_cast& e) {
      status = false;
    }
  }
  return status;
}
