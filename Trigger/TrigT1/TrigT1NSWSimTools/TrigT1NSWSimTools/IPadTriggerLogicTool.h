//  -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_IPADTDSTOOL_H
#define NSWL1_IPADTDSTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include <vector>

namespace NSWL1 {

    class PadData;
    class PadTrigger;

    /**

    @short interface for the PadTriggerLogic tools

    This class implements the public interface of the TDS Tools.
    It returns a vector of PadTrigger.

    The design of this class is based on the one of IPadTdsTool.

    \todo
    Note to self: Sandro's original design was to have an interface and
    then an "Offline" and an "Online" tool. I don't know whether this
    is still what we want...to be discussed.
    */

    class IPadTriggerLogicTool: public virtual IAlgTool {

    public:
        virtual ~IPadTriggerLogicTool() {}

        virtual StatusCode compute_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads, std::vector<std::unique_ptr<PadTrigger>> &triggers) = 0;

        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IPadTriggerLogicTool("NSWL1::IPadTriggerLogicTool", 1 ,0);
            return IID_IPadTriggerLogicTool;
        }
    };

}

#endif
