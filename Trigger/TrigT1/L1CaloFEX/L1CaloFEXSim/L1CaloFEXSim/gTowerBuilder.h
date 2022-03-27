/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gTowerBuilder - Builds gTowers (readout elements of gFEX)
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef GTOWERBUILDER_H
#define GTOWERBUILDER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgTowerBuilder.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gTowerContainer.h"

class CaloIdManager;

namespace LVL1 {

class gTowerBuilder: public AthAlgTool, virtual public IgTowerBuilder {

 public:
  gTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~gTowerBuilder() = default;

  virtual void init(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) override ;
  virtual void execute(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) override ;
  virtual void reset() override ;

 private:

  virtual void BuildEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildTransEMBgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildEMEgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildFwdEMECgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildFCALgTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildAllTowers(std::unique_ptr<gTowerContainer> & gTowerContainerRaw) const override ;
  virtual void BuildSingleTower(std::unique_ptr<gTowerContainer> & gTowerContainerRaw, int eta, int phi, int nphi, int keybase, int posneg) const override ;

};

} // end of LVL1 namespace
#endif
