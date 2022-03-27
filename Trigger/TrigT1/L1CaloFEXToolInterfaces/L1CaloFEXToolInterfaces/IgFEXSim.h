/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgFEXSim_H
#define IgFEXSim_H

#include "GaudiKernel/IAlgTool.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"

namespace LVL1 {

/*
Interface definition for gFEXSim
*/

  static const InterfaceID IID_IgFEXSim("LVL1::IgFEXSim", 1, 0);
  typedef  std::array<std::array<int, 40>, 32> gTowersIDs;

  class IgFEXSim : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void reset() = 0;

    virtual void execute() = 0;

    virtual StatusCode executegFEXSim(gTowersIDs tmp, gFEXOutputCollection* gFEXOutputs) = 0;

    virtual std::vector<uint32_t> getgRhoTOBs() const =0;

    virtual std::vector<uint32_t> getgBlockTOBs() const =0;

    virtual std::vector<uint32_t> getgJetTOBs() const =0;

    virtual std::vector<uint32_t> getgScalarEJwojTOBs() const =0;
 
    virtual std::vector<uint32_t> getgMETComponentsJwojTOBs() const =0;

    virtual std::vector<uint32_t> getgMHTComponentsJwojTOBs() const =0;

    virtual std::vector<uint32_t> getgMSTComponentsJwojTOBs() const =0;



  private:

  };

  inline const InterfaceID& LVL1::IgFEXSim::interfaceID()
  {
    return IID_IgFEXSim;
  }

} // end of namespace

#endif
