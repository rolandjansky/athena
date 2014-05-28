/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*! \file TRTCheckPeakSimple.h file declares the dqm_algorithms::TRTCheckPeakSimple class.
 * \author Peter Cwetanski
*/

#ifndef DQM_ALGORITHMS_TRTCHECKPEAKSIMPLE_H
#define DQM_ALGORITHMS_TRTCHECKPEAKSIMPLE_H

#include "dqm_core/Algorithm.h"

#include <string>

namespace dqm_algorithms
{

class TRTCheckPeakSimple: public dqm_core::Algorithm
{
public:
    TRTCheckPeakSimple();

    virtual ~TRTCheckPeakSimple();
    virtual dqm_core::Algorithm *clone();
    virtual dqm_core::Result *execute(const std::string &, const TObject &, const dqm_core::AlgorithmConfig &);
    using dqm_core::Algorithm::printDescription;
    virtual void printDescription(std::ostream& out);

private:
    std::string name;
};

} //namespace dqm_algorithms

#endif // DQM_ALGORITHMS_TRTCHECKPEAKSIMPLE_H
