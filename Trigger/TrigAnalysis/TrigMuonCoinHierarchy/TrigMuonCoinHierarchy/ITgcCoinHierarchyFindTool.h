/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_ITGCCOINHIERARCHYFINDTOOL_H 
#define TRIGGER_ITGCCOINHIERARCHYFINDTOOL_H 

#include "GaudiKernel/IAlgTool.h"

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyTriggerSector.h"

namespace Trigger {

  class ITgcCoinHierarchyFindTool : virtual public IAlgTool {
  public:
    /** Provide InterfaceID */
    static const InterfaceID& interfaceID() { 
      static const InterfaceID IID_ITgcCoinHierarchyFindTool("Trigger::ITgcCoinHierarchyFindTool", 1, 0);
      return IID_ITgcCoinHierarchyFindTool; 
    };

    /** Find TgcCoinHierarchy's from a TgcCoinHierarchyTriggerSector */
    virtual StatusCode find(TgcCoinHierarchyTriggerSector* pTrigSectorCombs, 
			    std::vector<TgcCoinHierarchy*>* pHierarchies) = 0;
  };
  
} // end of namespace Trigger

#endif // TRIGGER_ITGCCOINHIERARCHYFINDTOOL_H
