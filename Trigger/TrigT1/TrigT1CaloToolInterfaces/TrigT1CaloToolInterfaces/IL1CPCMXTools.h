/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IL1CPCMXTools.h, 
///////////////////////////////////////////////////////////////////
#ifndef ILVL1L1CPCMXTOOLS_H
#define ILVL1L1CPCMXTOOLS_H

#include "GaudiKernel/IAlgTool.h"
//#include "TrigT1CaloUtils/CPAlgorithm.h"

#include "xAODTrigL1Calo/CPMTobRoIContainer.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "xAODTrigL1Calo/CMXCPHitsContainer.h"

#include "TrigConfData/L1Menu.h"

namespace LVL1 
{
  class CPMTobRoI;
  class EmTauROI;
  
  /**
     Interface definition for L1CPCMXTools
  */
  static const InterfaceID IID_IL1CPCMXTools("LVL1::IL1CPCMXTools", 1, 0);
  
  class IL1CPCMXTools : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void formCMXCPTob(const xAOD::CPMTobRoIContainer*   cpmRoiVec,
                              xAOD::CMXCPTobContainer* cmxTobVec) const = 0;
    
    virtual void formCMXCPTob(
                 const std::vector<const xAOD::CPMTobRoIContainer*>& cpmRoiColls,
                 xAOD::CMXCPTobContainer* cmxTobVec, uint8_t peak) const = 0;
    

    virtual void formCMXCPHits(const TrigConf::L1Menu* l1menu,
			       const xAOD::CMXCPTobContainer*  cmxTobVec,
                               xAOD::CMXCPHitsContainer* cmxHitsVec) const = 0;
    virtual void formCMXCPHitsCrate(const TrigConf::L1Menu* l1menu,
				    const xAOD::CMXCPTobContainer* cmxTobVec,
				    xAOD::CMXCPHitsContainer* cmxHitsCrate) const = 0;
    virtual void formCMXCPHitsSystem(const xAOD::CMXCPHitsContainer* cmxHitsCrate,
				     xAOD::CMXCPHitsContainer* cmxHitsSys) const = 0;
    virtual void formCMXCPHitsTopo(const xAOD::CMXCPTobContainer* cmxTobVec,
                               xAOD::CMXCPHitsContainer* cmxHitsTopo) const = 0;
      
  };

  inline const InterfaceID& LVL1::IL1CPCMXTools::interfaceID()
    { 
      return IID_IL1CPCMXTools;
    }

} // end of namespace

#endif 
