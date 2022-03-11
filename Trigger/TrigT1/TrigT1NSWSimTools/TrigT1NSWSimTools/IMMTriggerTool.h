/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef IMMTRIGGERTOOL_H
#define IMMTRIGGERTOOL_H

//basic includes
#include "GaudiKernel/IAlgTool.h"
#include "MuonRDO/NSW_TrigRawDataContainer.h"

//local includes
#include <vector>


// namespace for the NSW LVL1 related classes
namespace NSWL1 {

  class IMMTriggerTool: public virtual IAlgTool {

  public:
    virtual ~IMMTriggerTool() {}

    virtual StatusCode runTrigger(Muon::NSW_TrigRawDataContainer* rdo, const bool do_MMDiamonds) = 0;

    static const InterfaceID& interfaceID() {
        static const InterfaceID IID_IMMTriggerTool("NSWL1::IMMTriggerTool", 1 ,0);
        return IID_IMMTriggerTool;
    }

  };  // end of IMMTriggerTools class

} // namespace NSWL1

#endif
