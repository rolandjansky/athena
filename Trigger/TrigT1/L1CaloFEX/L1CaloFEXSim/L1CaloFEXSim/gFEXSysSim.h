/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXSysSim - Overall gFEX simulation
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXSysSim_H
#define gFEXSysSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgFEXSysSim.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gFEXSim.h"
#include "L1CaloFEXSim/gTower.h"

#include "xAODTrigger/gFexJetRoI.h"
#include "xAODTrigger/gFexJetRoIContainer.h"
#include "xAODTrigger/gFexJetRoIAuxContainer.h"

namespace LVL1 {

  //Doxygen class description below:
  /** The gFEXSysSim class defines the structure of the gFEX system
      Its purpose is:
      - to follow the structure of the gFEX and its FPGAs in as much
      detail as necessary to simulate the output of the system
      It will need to interact with gTowers and produce the gTOBs
  */

  class gFEXSysSim : public AthAlgTool, virtual public IgFEXSysSim {

  public:

    /** Constructors */

    gFEXSysSim(const std::string& type,const std::string& name,const IInterface* parent);
    /** Destructor */
    gFEXSysSim&& operator= (const gFEXSysSim& ) = delete;

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    virtual StatusCode execute() override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int nphi, int mod) override ;

    /**Create and fill a new gFexJetRoI object, and return a pointer to it*/
    virtual StatusCode fillgRhoEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgBlockEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgJetEDM(uint32_t tobWord) override ;

    /** Internal data */
  private:
    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gRhoContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gRhoAuxContainer;

    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gBlockContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gBlockAuxContainer;

    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gJetContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gJetAuxContainer;

    std::vector<gFEXSim*> m_gFEXCollection;

    ToolHandle<IgFEXSim> m_gFEXSimTool       {this, "gFEXSimTool",    "LVL1::gFEXSim",    "Tool that creates the gFEX Simulation"};

    SG::ReadHandleKey<LVL1::gTowerContainer> m_gTowerContainerSGKey {this, "MyGTowers", "gTowerContainer", "Input container for gTowers"};
    SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

    SG::WriteHandleKey< xAOD::gFexJetRoIContainer > m_gFexJetOutKey {this,"Key_gFexJetOutputContainer","L1_gJetRoI","Output gFexJet container"};

    std::vector<uint32_t>  m_allgRhoTobs;
    std::vector<uint32_t>  m_allgBlockTobs;
    std::vector<uint32_t>  m_allgJetTobs;

  };

} // end of namespace


#endif
