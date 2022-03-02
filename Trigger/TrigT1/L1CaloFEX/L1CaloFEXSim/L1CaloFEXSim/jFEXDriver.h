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
#include "xAODTrigL1Calo/TriggerTowerContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/jFEXOutputCollection.h"
#include "xAODTrigger/jFexSRJetRoIContainer.h" 
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h" 


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

  virtual StatusCode testSRJetEDM();
  virtual StatusCode testLRJetEDM();
  virtual StatusCode testTauEDM();

 private: 

  int m_numberOfEvents = 0;

  SG::WriteHandleKey<LVL1::jTowerContainer> m_jTowerContainerSGKey {this, "MyETowers", "jTowerContainer", "MyETowers"};

  SG::WriteHandleKey<jFEXOutputCollection> m_jFEXOutputCollectionSGKey {this, "MyOutputs", "jFEXOutputCollection", "MyOutputs"};

  SG::ReadHandleKey<xAOD::jFexSRJetRoIContainer> m_jFexSRJetEDMKey {this, "myEDMSR", "L1_jFexSRJetRoI", "Reading container of jFexSRRoIs"};
  SG::ReadHandleKey<xAOD::jFexLRJetRoIContainer> m_jFexLRJetEDMKey {this, "myEDMLR", "L1_jFexLRJetRoI", "Reading container of jFexLRRoIs"};
  SG::ReadHandleKey<xAOD::jFexTauRoIContainer> m_jFexTauEDMKey {this, "myEDMTau", "L1_jFexTauRoI", "Reading container of jFexTauRoIs"};


  ToolHandle<IjTowerBuilder> m_jTowerBuilderTool {this, "jTowerBuilderTool", "LVL1::jTowerBuilder", "Tool that builds jTowers for simulation"};
  ToolHandle<IjSuperCellTowerMapper> m_jSuperCellTowerMapperTool {this, "jSuperCellTowerMapperTool", "LVL1::jSuperCellTowerMapper", "Tool that maps supercells to jTowers"};
  ToolHandle<IjFEXSysSim> m_jFEXSysSimTool {this, "jFEXSysSimTool", "LVL1::jFEXSysSim", "Tool that creates the jFEX System Simulation"};

  std::unordered_map<Identifier, std::pair<int,int> > m_cell_to_tower_map;
  std::string m_PileupWeigthFile;
  
  // This string is used to convert the m_PileupweigthFile file in a readable file for jFEX
  std::string m_PileupHelperFile;
  
    //histograms need to set coordinates and noise subtraction
    TH1F* m_jTowerArea_hist;
    TH1I* m_Firmware2BitwiseID;
    TH1I* m_BinLayer;
    TH1F* m_EtaCoords;
    TH1F* m_PhiCoords;


};

} // end of LVL1 namespace
#endif
