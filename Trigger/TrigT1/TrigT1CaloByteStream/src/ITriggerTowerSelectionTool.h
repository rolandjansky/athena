/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOBYTESTREAM_ITRIGGERTOWERSELECTIONTOOL_H
#define TRIGT1CALOBYTESTREAM_ITRIGGERTOWERSELECTIONTOOL_H

#include <vector>

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IInterface.h"


namespace LVL1BS {

static const InterfaceID IID_ITriggerTowerSelectionTool("LVL1BS::ITriggerTowerSelectionTool", 1, 0);

class ITriggerTowerSelectionTool : virtual public IAlgTool {

 public:
   static const InterfaceID& interfaceID();

   virtual void channelIDs(double etaMin, double etaMax,
                           double phiMin, double phiMax,
			   std::vector<unsigned int>& chanIds) = 0;
   virtual void robIDs(const std::vector<unsigned int>& chanIds,
                             std::vector<unsigned int>& robs) = 0;
      
};

inline const InterfaceID& ITriggerTowerSelectionTool::interfaceID()
{ 
  return IID_ITriggerTowerSelectionTool;
}

} // end of namespace

#endif 
