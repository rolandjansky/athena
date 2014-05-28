/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DQM_ALGORITHMS_ITERATIVEGAUSSIANFIT_H
#define DQM_ALGORITHMS_ITERATIVEGAUSSIANFIT_H

#include "dqm_core/Algorithm.h"
#include <string>

namespace dqm_core { class AlgorithmConfig; }
class TObject;

namespace dqm_algorithms
{

class IterativeGaussianFit: public dqm_core::Algorithm
{
public:
    explicit IterativeGaussianFit(const std::string &name);
    virtual ~IterativeGaussianFit();

public:
    virtual IterativeGaussianFit *clone();
    virtual dqm_core::Result *execute(const std::string &name, const TObject &object, const dqm_core::AlgorithmConfig &config);
    using dqm_core::Algorithm::printDescription;
    virtual void printDescription(std::ostream& out);

private:
    const std::string m_name;
};

} // namespace

#endif // DQM_ALGORITHMS_ITERATIVEGAUSSIANFIT_H
