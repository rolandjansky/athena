#ifndef EFEXDRIVER_H
#define EFEXDRIVER_H

// STL
#include <string>

// Athena/Gaudi
#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "CaloEvent/CaloCellContainer.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXSim/eTowerBuilder.h"
#include "L1CaloFEXSim/eSuperCellTowerMapper.h"
#include "L1CaloFEXToolInterfaces/IeFEXSysSim.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"

class CaloIdManager;

namespace LVL1 {

class eFEXDriver : public AthAlgorithm
{
 public:
  //using AthReentrantAlgorithm::AthReentrantAlgorithm;

  eFEXDriver(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~eFEXDriver();

  virtual StatusCode initialize();
  virtual StatusCode execute(/*const EventContext& ctx*/);// const;
  StatusCode finalize();

 private:

  int m_numberOfEvents = 0;

  SG::WriteHandleKey<LVL1::eTowerContainer> m_eTowerContainerSGKey {this, "MyETowers", "eTowerContainer", "MyETowers"};

  //SG::WriteHandleKey<eFEXOutputCollection> m_eFEXOutputCollectionSGKey {this, "MyOutputs", "eFEXOutputCollection", "MyOutputs"};

  SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

  ToolHandle<IeTowerBuilder> m_eTowerBuilderTool {this, "eTowerBuilderTool", "LVL1::eTowerBuilder", "Tool that builds eTowers for simulation"};
  ToolHandle<IeSuperCellTowerMapper> m_eSuperCellTowerMapperTool {this, "eSuperCellTowerMapperTool", "LVL1::eSuperCellTowerMapper", "Tool that maps supercells to eTowers"};
  ToolHandle<IeFEXSysSim> m_eFEXSysSimTool {this, "eFEXSysSimTool", "LVL1::eFEXSysSim", "Tool that creates the eFEX System Simulation"};

  std::map<Identifier, std::pair<int,int> > m_cell_to_tower_map;

};

} // end of LVL1 namespace
#endif
