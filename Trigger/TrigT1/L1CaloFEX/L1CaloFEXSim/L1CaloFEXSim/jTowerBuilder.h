/*
    Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#ifndef JTOWERBUILDER_H
#define JTOWERBUILDER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjTowerBuilder.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"

class CaloIdManager;

namespace LVL1 {

class jTowerBuilder: public AthAlgTool, virtual public IjTowerBuilder {

 public:
  jTowerBuilder(const std::string& type,const std::string& name,const IInterface* parent);
  virtual ~jTowerBuilder() = default;

  virtual void init(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) override ;
  virtual void execute(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) override ;
  virtual void reset() override ;

 private:

  virtual void BuildEMBjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildTRANSjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildEMEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildEMIEjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildFCALjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildHECjTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildAllTowers(std::unique_ptr<jTowerContainer> & jTowerContainerRaw) const override ;
  virtual void BuildSingleTower(std::unique_ptr<jTowerContainer> & jTowerContainerRawRaw,float eta, float phi, float keybase, int posneg, float centre_eta = 0.0, float centre_phi = 0.0, int fcal_layer = -1) const override ;

};

} // end of LVL1 namespace
#endif
