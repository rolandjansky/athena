/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigT1NSWSimTools/MMCandidateData.h"

std::ostream& operator<<(std::ostream& stream, NSWL1::MMCandidateData& candidate) {
    stream << "MM Trigger candidate: Delta Theta " << candidate.deltaTheta()
           << ", eta "   << candidate.eta()
           << "(sigma: " << candidate.etaSigma() << ")"
           << ", phi "   << candidate.phi()
           << "(sigma: " << candidate.phiSigma() << ")";

    return stream;
}
