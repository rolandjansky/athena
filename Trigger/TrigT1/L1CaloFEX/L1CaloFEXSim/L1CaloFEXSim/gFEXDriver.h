#ifndef GFEXDRIVER_H
#define GFEXDRIVER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gTowerContainer.h"
#include "L1CaloFEXSim/gTowerBuilder.h"
#include "L1CaloFEXSim/gSuperCellTowerMapper.h"
#include "L1CaloFEXToolInterfaces/IgFEXSysSim.h"
#include "L1CaloFEXSim/gFEXSim.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"

class CaloIdManager;

namespace LVL1 {

class gFEXDriver : public AthAlgorithm
{
 public:
  //using AthReentrantAlgorithm::AthReentrantAlgorithm;

  gFEXDriver(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~gFEXDriver();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

 private:

  int m_numberOfEvents = 0;

  //Declare that gFEXDriver class will write an object of type LVL1::gTowerContainer, one of type gFEXOutputCollection
  SG::WriteHandleKey<LVL1::gTowerContainer> m_gTowerContainerSGKey {this, "MyGTowers", "gTowerContainer", "MyGTowers"};
  SG::WriteHandleKey<gFEXOutputCollection> m_gFEXOutputCollectionSGKey {this, "MyOutputs", "gFEXOutputCollection", "MyOutputs"};

  //Declare that gFEXDriver class will read an object of type CaloCellContainer, xAOD::gFexJetRoIContainer, xAOD::gFexGlobalRoIContainer
  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

  ToolHandle<IgTowerBuilder> m_gTowerBuilderTool {this, "gTowerBuilderTool", "LVL1::gTowerBuilder", "Tool that builds gTowers for simulation"};
  ToolHandle<IgSuperCellTowerMapper> m_gSuperCellTowerMapperTool {this, "gSuperCellTowerMapperTool", "LVL1::gSuperCellTowerMapper", "Tool that maps supercells to gTowers"};
  ToolHandle<IgFEXSysSim> m_gFEXSysSimTool {this, "gFEXSysSimTool", "LVL1::gFEXSysSim", "Tool that creates the gFEX System Simulation"};

  std::map<Identifier, std::pair<int,int> > m_cell_to_tower_map;

};

} // end of LVL1 namespace
#endif
