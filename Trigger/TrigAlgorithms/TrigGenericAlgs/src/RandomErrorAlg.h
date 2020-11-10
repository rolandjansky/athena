/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGENERICALGS_RandomError_h
#define TRIGGENERICALGS_RandomError_h

#include "AthenaBaseComps/AthReentrantAlgorithm.h"

/**
 *  @class RandomError
 *  @brief Throw an error with given probability
 **/
class RandomErrorAlg : public AthReentrantAlgorithm {
  public:

    RandomErrorAlg(const std::string& name, ISvcLocator* svcLoc);

    virtual StatusCode execute(const EventContext& eventContext) const override;

  private:
    Gaudi::Property<double> m_errorProbability {this, "ErrorProbability", 0.5, "Probablility od throwing an error"};
};

#endif // TRIGGENERICALGS_RandomError_h
