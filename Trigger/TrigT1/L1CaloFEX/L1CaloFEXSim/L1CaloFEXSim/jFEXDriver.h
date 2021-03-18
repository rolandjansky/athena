#ifndef JFEXDRIVER_H
#define JFEXDRIVER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jTowerBuilder.h"
#include "L1CaloFEXSim/jSuperCellTowerMapper.h"
#include "L1CaloFEXToolInterfaces/IjFEXSysSim.h"
#include "L1CaloFEXSim/jFEXSim.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"

class CaloIdManager;

namespace LVL1 {

class jFEXDriver : public AthAlgorithm
{
 public:
  //using AthReentrantAlgorithm::AthReentrantAlgorithm;

  jFEXDriver(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~jFEXDriver();

  virtual StatusCode initialize();
  virtual StatusCode execute(/*const EventContext& ctx*/);// const;
  StatusCode finalize();

 private:

  int m_numberOfEvents = 0;

  SG::WriteHandleKey<LVL1::jTowerContainer> m_jTowerContainerSGKey {this, "MyETowers", "jTowerContainer", "MyETowers"};

  SG::WriteHandleKey<jFEXOutputCollection> m_jFEXOutputCollectionSGKey {this, "MyOutputs", "jFEXOutputCollection", "MyOutputs"};

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

  ToolHandle<IjTowerBuilder> m_jTowerBuilderTool {this, "jTowerBuilderTool", "LVL1::jTowerBuilder", "Tool that builds jTowers for simulation"};
  ToolHandle<IjSuperCellTowerMapper> m_jSuperCellTowerMapperTool {this, "jSuperCellTowerMapperTool", "LVL1::jSuperCellTowerMapper", "Tool that maps supercells to jTowers"};
  ToolHandle<IjFEXSysSim> m_jFEXSysSimTool {this, "jFEXSysSimTool", "LVL1::jFEXSysSim", "Tool that creates the jFEX System Simulation"};

  std::map<Identifier, std::pair<int,int> > m_cell_to_tower_map;

};

} // end of LVL1 namespace
#endif
