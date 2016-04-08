/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Tokenizer_h
#define Tokenizer_h
#include <string>
#include <vector>

namespace FADS {

class Tokenizer : public std::vector<std::string> {
public:
  Tokenizer(const std::string & sep, const std::string & input);


private:

};

}	// end namespace

#endif // Tokenizer_h
