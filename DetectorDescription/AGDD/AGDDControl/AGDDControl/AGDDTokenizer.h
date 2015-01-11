/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDTokenizer_h
#define AGDDTokenizer_h
#include <string>
#include <vector>

class AGDDTokenizer : public std::vector<std::string> {
public:
  AGDDTokenizer(const std::string & sep, const std::string & input);


private:

};


#endif // AGDDTokenizer_h
