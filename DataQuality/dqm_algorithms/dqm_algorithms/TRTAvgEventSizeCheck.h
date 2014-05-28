/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifdef ONLINE // can only be built in an online environment

#ifndef DQM_ALGORITHMS_TRTAVGEVENTSIZECHECK_H
#define DQM_ALGORITHMS_TRTAVGEVENTSIZECHECK_H

#include "dqm_core/Algorithm.h"

namespace dqm_algorithms {

class TRTAvgEventSizeCheck : public dqm_core::Algorithm
{
public:
    TRTAvgEventSizeCheck();
    TRTAvgEventSizeCheck *clone();
    dqm_core::Result *execute(const std::string &, const TObject &, const dqm_core::AlgorithmConfig &);
    using dqm_core::Algorithm::printDescription;
    void printDescription(std::ostream& out);

private:
    boost::posix_time::ptime lastCollisionsSeen_;
};

} // namespace

#endif // DQM_ALGORITHMS_TRTAVGEVENTSIZECHECK_H
#endif // ONLINE
