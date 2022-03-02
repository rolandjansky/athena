/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXSysSim.h  -  
//                              -------------------
//     begin                : 12 07 2019
//     email                : alison.elliot@cern.ch, jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef eFEXSysSim_H
#define eFEXSysSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXSysSim.h"
#include "L1CaloFEXToolInterfaces/IeFEXFillEDM.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eFEXFPGATowerIdProvider.h"

#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexEMRoIAuxContainer.h"

#include "xAODTrigger/eFexTauRoIContainer.h"
#include "xAODTrigger/eFexTauRoIAuxContainer.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The eFEXSysSim class defines the structure of the eFEX system
      Its purpose is:
      - to follow the structure of the 24 eFEXes and their FPGAs in as much
      detail as necessary to simulate the output of the system
      It will need to interact with eTowers and produce the eTOBs
  */

  class eFEXSysSim : public AthAlgTool, virtual public IeFEXSysSim {
    
  public:
    
    /** Constructors */

    eFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent);
    /** Destructor */
    eFEXSysSim&& operator= (const eFEXSysSim& ) = delete;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  () override;

    virtual StatusCode execute(eFEXOutputCollection* inputOutputCollection) override ;

    virtual void init() override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int mod) override;

    /** Internal data */
  private:
    std::unique_ptr< xAOD::eFexEMRoIContainer > m_eContainer;
    std::unique_ptr< xAOD::eFexEMRoIAuxContainer > m_eAuxContainer;

    std::unique_ptr< xAOD::eFexTauRoIContainer > m_tauContainer;
    std::unique_ptr< xAOD::eFexTauRoIAuxContainer > m_tauAuxContainer;

    std::vector<eFEXSim*>  m_eFEXCollection;
    
    ToolHandle<IeFEXSim> m_eFEXSimTool {this, "eFEXSimTool",    "LVL1::eFEXSim",    "Tool that creates the eFEX Simulation"};

    ToolHandle<IeFEXFillEDM> m_eFEXFillEDMTool {this, "eFEXFillEDMTool", "LVL1::eFEXFillEDM", "Tool to fille eFEX EDMs"};

    SG::ReadHandleKey<LVL1::eTowerContainer> m_eTowerContainerSGKey {this, "MyETowers", "eTowerContainer", "Input container for eTowers"};

    SG::WriteHandleKey< xAOD::eFexEMRoIContainer > m_eFexOutKey {this,"Key_eFexEMOutputContainer","L1_eEMRoI","Output eFexEM container"};
    SG::WriteHandleKey< xAOD::eFexTauRoIContainer > m_eFexTauOutKey {this,"Key_eFexTauOutputContainer","L1_eTauRoI","Output eFexTau container"};
    ToolHandle<IeFEXFPGATowerIdProvider> m_eFEXFPGATowerIdProviderTool {this, "eFEXFPGATowerIdProviderTool", "LVL1::eFEXFPGATowerIdProvider", "Tool that provides tower-FPGA mapping"};
    ToolHandle<IeFEXFPGA> m_eFEXFPGATool {this, "eFEXFPGATool", "LVL1::eFEXFPGA", "Tool that simulates the FPGA hardware"};

    //std::map<int,eTower> m_eTowersColl;

    std::map<int, std::vector<uint32_t> > m_allEmTobs;

    std::map<int, std::vector<uint32_t> > m_allTauTobs;

  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXSysSim , 32201258 , 1 )


#endif
