/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETTAGTOOLS_LIKELIHOODCOMPONENTS_H
#define JETTAGTOOLS_LIKELIHOODCOMPONENTS_H
#include <vector>
#include <string>

/******************************************************
 @class LikelihoodComponents 
********************************************************/

namespace Analysis
{

  class AtomicProperty {
    public:
      inline AtomicProperty(double v, const std::string& n) { value = v; name = n; }
      double value;
      std::string name;
  };
  class Composite {
    public:
      inline Composite(const std::string& n) { name = n; }
      std::vector<AtomicProperty> atoms;
      std::string name;
  };
  class Slice {
    public:
      inline Slice(const std::string& n) { name = n; }
      std::vector<Composite> composites;
      std::string name;
  };

}
#endif
