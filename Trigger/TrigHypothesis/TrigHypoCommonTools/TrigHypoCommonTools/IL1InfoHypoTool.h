/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHYPOCOMMONTOOLS_IL1INFOHYPOTOOL_H
#define TRIGHYPOCOMMONTOOLS_IL1INFOHYPOTOOL_H

#include "AsgTools/IAsgTool.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "TrigCompositeUtils/HLTIdentifier.h"


class IL1InfoHypoTool : virtual public::IAlgTool {
    
public:
    struct L1Info {
        L1Info (const ROIB::RoIBResult* r, const TrigCompositeUtils::DecisionIDContainer& p, TrigCompositeUtils::Decision* d) 
        : roib(r), previousDecisionsIDs(p), decision(d) {}

        // RoIB to retrieve L1 before prescale decisions
        const ROIB::RoIBResult* roib;

        // The set of positive decisions made at the previous step 
        const TrigCompositeUtils::DecisionIDContainer previousDecisionsIDs;

        // Place to store DecisionIDs from this step
        TrigCompositeUtils::Decision* decision;
    };


    DeclareInterfaceID(IL1InfoHypoTool, 1, 0);

    virtual ~IL1InfoHypoTool(){};
    virtual StatusCode decide(const std::vector<L1Info>&) const = 0;
    virtual const HLT::Identifier& getId() const = 0;
};

#endif //TRIGHYPOCOMMONTOOLS_IL1INFOHYPOTOOL_H