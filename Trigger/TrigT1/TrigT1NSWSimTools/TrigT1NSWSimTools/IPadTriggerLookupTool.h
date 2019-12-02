//  -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef NSWL1_IPADTRIGGERLOOKUPTOOL_H
#define NSWL1_IPADTRIGGERLOOKUPTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "TrigT1NSWSimTools/TriggerTypes.h"
#include <vector>

namespace NSWL1 {

    class PadData;
    class PadTrigger;

    class IPadTriggerLookupTool: public virtual IAlgTool {

    public:
        virtual ~IPadTriggerLookupTool() {}

        virtual StatusCode lookup_pad_triggers(const std::vector<std::shared_ptr<PadData>>& pads, std::vector<std::unique_ptr<PadTrigger>> &triggers) = 0;

        static const InterfaceID& interfaceID() {
            static const InterfaceID IID_IPadTriggerLookupTool("NSWL1::IPadTriggerLookupTool", 1 ,0);
            return IID_IPadTriggerLookupTool;
        }
    };

}

#endif
