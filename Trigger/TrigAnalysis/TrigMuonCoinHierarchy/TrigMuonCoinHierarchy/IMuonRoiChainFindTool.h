/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGGER_IMUONROICHAINFINDTOOL_H 
#define TRIGGER_IMUONROICHAINFINDTOOL_H 

#include "GaudiKernel/IAlgTool.h"

#include "TrigMuonCoinHierarchy/MuonRoiChain.h"

namespace Trigger {

  class IMuonRoiChainFindTool : virtual public IAlgTool {
  public:
    /** Provide InterfaceID */
    static const InterfaceID& interfaceID() { 
      static const InterfaceID IID_IMuonRoiChainFindTool("Trigger::IMuonRoiChainFindTool", 1, 0);
      return IID_IMuonRoiChainFindTool; 
    };

    /** Find MuonRoiChains */
    virtual StatusCode find(std::vector<MuonRoiChain*>* chains) = 0;
  };
  
} // end of namespace Trigger

#endif // TRIGGER_IMUONROICHAINFINDTOOL_H
