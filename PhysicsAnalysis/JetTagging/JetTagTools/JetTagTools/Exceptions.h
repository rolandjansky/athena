/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// WARNING: this code was copied automatically from
// https://github.com/lwtnn/lwtnn.git (rev v2.0)
// Please don't edit it! To get the latest version, run
// > ./update-lwtnn.sh
// from JetTagTools/share

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

#include <stdexcept>

namespace lwt {
  // ______________________________________________________________________
  // exceptions

  // base exception
  class LightweightNNException: public std::logic_error {
  public:
    LightweightNNException(std::string problem);
  };

  // thrown by the constructor if something goes wrong
  class NNConfigurationException: public LightweightNNException {
  public:
    NNConfigurationException(std::string problem);
  };

  // thrown by `compute`
  class NNEvaluationException: public LightweightNNException {
  public:
    NNEvaluationException(std::string problem);
  };
}

#endif // EXCEPTIONS_HH



