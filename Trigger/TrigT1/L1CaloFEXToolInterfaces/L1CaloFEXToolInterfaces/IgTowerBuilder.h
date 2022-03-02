/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef IgTowerBuilder_H
#define IgTowerBuilder_H

#include "GaudiKernel/IAlgTool.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gTowerContainer.h"

namespace LVL1 {

/*
Interface definition for gTowerBuilder
*/

  static const InterfaceID IID_IgTowerBuilder("LVL1::IgTowerBuilder", 1, 0);

  class IgTowerBuilder : virtual public IAlgTool {
  public:
    static const InterfaceID& interfaceID( ) ;

    virtual void BuildEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildTransEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildEMEgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildFwdEMECgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildFCALgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildAllTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const = 0;
    virtual void BuildSingleTower(std::unique_ptr<gTowerContainer> & gTowerContainerRaw, int eta, int phi, int nphi, int keybase, int posneg) const = 0;


    virtual void init(std::unique_ptr<gTowerContainer> & gTowerContainer) = 0;
    virtual void execute(std::unique_ptr<gTowerContainer> & gTowerContainer) = 0;
    virtual void reset() = 0;




  private:

  };

  inline const InterfaceID& LVL1::IgTowerBuilder::interfaceID()
  {
    return IID_IgTowerBuilder;
  }

} // end of namespace

#endif
