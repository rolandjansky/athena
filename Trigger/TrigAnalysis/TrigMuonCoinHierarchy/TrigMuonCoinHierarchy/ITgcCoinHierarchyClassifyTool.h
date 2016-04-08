/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_ITGCCOINHIERARCHYCLASSIFYTOOL_H 
#define TRIGGER_ITGCCOINHIERARCHYCLASSIFYTOOL_H 

#include "GaudiKernel/IAlgTool.h"

#include "TrigMuonCoinHierarchy/TgcCoinHierarchyTriggerSector.h"

class Identifier;

namespace Trigger {

  class ITgcCoinHierarchyClassifyTool : virtual public IAlgTool {
  public:
    /** Provide InterfaceID */
    static const InterfaceID& interfaceID() { 
      static const InterfaceID IID_ITgcCoinHierarchyClassifyTool("Trigger::ITgcCoinHierarchyClassifyTool", 1, 0);
      return IID_ITgcCoinHierarchyClassifyTool; 
    };
    
    /** Make TgcCoinHierarchyTriggerSector's from TgcCoinDataContainer and TgcPrepDataContainer */
    virtual StatusCode classify(std::vector<TgcCoinHierarchyTriggerSector*>* pTrigSectorCombs) = 0;

    /** Get TgcCoinHierarchy::STATION enum from an Indentifier */
    virtual TgcCoinHierarchy::STATION getSTATION(const Identifier identify) const = 0 ;
    /** Get trigger sector from an Identifier */
    virtual bool getTriggerSector(const Identifier identify, unsigned int& isAside, unsigned int& isForward, 
				  unsigned int& phi) const = 0;
    /** Get isStrip from an Identifier */
    virtual bool isStrip(const Identifier identify) const = 0;
  };

} // end of namespace Trigger 

#endif // TRIGGER_ITGCCOINHIERARCHYCLASSIFYTOOL_H
