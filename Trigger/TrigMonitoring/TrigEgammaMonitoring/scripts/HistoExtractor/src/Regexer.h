/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Regexer_h
#define Regexer_h

#include <string>
#include <vector>
#include <boost/regex.hpp>

using std::string;
using std::vector;

class Regexer {

  public:
    Regexer(std::string incl="", std::string excl="");
    ~Regexer(void);
  
    ///Extracts runnumber from string that encloses it in dots and pads it with leading zeros to 8 characters
    std::string ExtractRunNumber(std::string input);

    bool Match(std::string input);

    std::vector<std::string> SplitPathIntoDirs(std::string path); 
    

  private:
    bool doIncl;
    bool doExcl;
    boost::regex expr_incl;
    boost::regex expr_excl;
};
#endif
