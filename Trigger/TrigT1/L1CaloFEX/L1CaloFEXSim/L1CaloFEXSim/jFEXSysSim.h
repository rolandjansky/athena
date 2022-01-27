/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXSysSim.h  -  
//                              -------------------
//     begin                : 12 07 2019
//     email                : alison.elliot@cern.ch, jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef jFEXSysSim_H
#define jFEXSysSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXSysSim.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jFEXSim.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

#include "xAODTrigger/jFexSRJetRoIContainer.h"
#include "xAODTrigger/jFexSRJetRoIAuxContainer.h"
#include "xAODTrigger/jFexLRJetRoIContainer.h"
#include "xAODTrigger/jFexLRJetRoIAuxContainer.h"
#include "xAODTrigger/jFexTauRoIContainer.h"
#include "xAODTrigger/jFexTauRoIAuxContainer.h"
#include "xAODTrigger/jFexMETRoIContainer.h"
#include "xAODTrigger/jFexMETRoIAuxContainer.h"
#include "xAODTrigger/jFexSumETRoIContainer.h"
#include "xAODTrigger/jFexSumETRoIAuxContainer.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The jFEXSysSim class defines the structure of the jFEX system
      Its purpose is:
      - to follow the structure of the 24 jFEXes and their FPGAs in as much
      detail as necessary to simulate the output of the system
      It will need to interact with jTowers and produce the eTOBs
  */

  class jFEXSysSim : public AthAlgTool, virtual public IjFEXSysSim {
    
  public:
    
    /** Constructors */

    jFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent);
    /** Destructor */
    jFEXSysSim&& operator= (const jFEXSysSim& ) = delete;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  () override;

    virtual StatusCode execute(jFEXOutputCollection* inputOutputCollection) override ;

    virtual void init() override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int mod) override ;

    /**Create and fill a new eFexEMRoI object (corresponding to this window), and return a pointer to it*/
    virtual StatusCode fillSRJetEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexSRJetRoIContainer > &jContainer) override ;
    virtual StatusCode fillLRJetEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexLRJetRoIContainer > &jContainer) override ;
    virtual StatusCode fillTauEDM  (uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, float_t eta, float_t phi, std::unique_ptr< xAOD::jFexTauRoIContainer   > &jContainer) override ;  
    virtual StatusCode fillSumEtEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, std::unique_ptr< xAOD::jFexSumETRoIContainer > &jContainer) override ;  
    virtual StatusCode fillMetEDM(uint8_t jFexNum, uint8_t fpgaNumber, uint32_t tobWord, std::unique_ptr< xAOD::jFexMETRoIContainer > &jContainer) override ;  
      
  /** Internal data */
  private:
    std::vector<jFEXSim*>  m_jFEXCollection;
    
    ToolHandle<IjFEXSim> m_jFEXSimTool       {this, "jFEXSimTool",    "LVL1::jFEXSim",    "Tool that creates the jFEX Simulation"};

    SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerSGKey {this, "MyETowers", "jTowerContainer", "Input container for jTowers"};
    SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

    SG::WriteHandleKey< xAOD::jFexSRJetRoIContainer> m_jFexSRJetOutKey {this,"Key_jFexSRJetOutputContainer","L1_jFexSRJetRoI","Output jFexEM container"};
    SG::WriteHandleKey< xAOD::jFexLRJetRoIContainer> m_jFexLRJetOutKey {this,"Key_jFexLRJetOutputContainer","L1_jFexLRJetRoI","Output jFexEM container"};
    SG::WriteHandleKey< xAOD::jFexTauRoIContainer>   m_jFexTauOutKey   {this,"Key_jFexTauOutputContainer","L1_jFexTauRoI","Output jFexEDM tau container"};
    SG::WriteHandleKey< xAOD::jFexSumETRoIContainer> m_jFexSumETOutKey {this,"Key_jFexSumETOutputContainer","L1_jFexSumETRoI","Output jFexEDM SumET container"};
    SG::WriteHandleKey< xAOD::jFexMETRoIContainer>   m_jFexMETOutKey   {this,"Key_jFexMETOutputContainer","L1_jFexMETRoI","Output jFexEDM Met container"};

    std::unordered_map<int,jTower> m_jTowersColl;
    std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> > m_allSmallRJetTobs; 
    std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> > m_allLargeRJetTobs;
    std::unordered_map<uint8_t, std::vector<std::vector<std::vector<uint32_t>>> > m_alltauTobs;
    std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> > m_allsumEtTobs;
    std::unordered_map<uint8_t, std::vector<std::vector<uint32_t>> > m_allMetTobs;
  };
  
} // end of namespace

//CLASS_DEF( LVL1::jFEXSysSim , 141823245 , 1 )


#endif
