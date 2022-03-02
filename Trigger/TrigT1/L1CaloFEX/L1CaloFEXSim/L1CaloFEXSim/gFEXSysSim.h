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

#include "xAODTrigger/gFexGlobalRoI.h"
#include "xAODTrigger/gFexGlobalRoIContainer.h"
#include "xAODTrigger/gFexGlobalRoIAuxContainer.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"

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

    virtual StatusCode execute(gFEXOutputCollection* gFEXOutputs) override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int nphi, int mod) override ;

    /**Create and fill a new gFexJetRoI object, and return a pointer to it*/
    virtual StatusCode fillgRhoEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgBlockEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgJetEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgScalarEJwojEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgMETComponentsJwojEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgMHTComponentsJwojEDM(uint32_t tobWord) override ;

    virtual StatusCode fillgMSTComponentsJwojEDM(uint32_t tobWord) override ;


    /** Internal data */
  private:
    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gRhoContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gRhoAuxContainer;

    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gBlockContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gBlockAuxContainer;

    std::unique_ptr< xAOD::gFexJetRoIContainer > m_gJetContainer;
    std::unique_ptr< xAOD::gFexJetRoIAuxContainer > m_gJetAuxContainer;

    std::unique_ptr< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojContainer;
    std::unique_ptr< xAOD::gFexGlobalRoIAuxContainer > m_gScalarEJwojAuxContainer;

    std::unique_ptr< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojContainer;
    std::unique_ptr< xAOD::gFexGlobalRoIAuxContainer > m_gMETComponentsJwojAuxContainer;

    std::unique_ptr< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojContainer;
    std::unique_ptr< xAOD::gFexGlobalRoIAuxContainer > m_gMHTComponentsJwojAuxContainer;

    std::unique_ptr< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojContainer;
    std::unique_ptr< xAOD::gFexGlobalRoIAuxContainer > m_gMSTComponentsJwojAuxContainer;


    std::vector<gFEXSim*> m_gFEXCollection;

    ToolHandle<IgFEXSim> m_gFEXSimTool       {this, "gFEXSimTool",    "LVL1::gFEXSim",    "Tool that creates the gFEX Simulation"};

    SG::ReadHandleKey<LVL1::gTowerContainer> m_gTowerContainerSGKey {this, "MyGTowers", "gTowerContainer", "Input container for gTowers"};

    SG::WriteHandleKey< xAOD::gFexJetRoIContainer > m_gFexRhoOutKey {this,"Key_gFexRhoOutputContainer","L1_gFexRhoRoI","Output gFexRho (energy density) container"};
    SG::WriteHandleKey< xAOD::gFexJetRoIContainer > m_gFexBlockOutKey {this,"Key_gFexSRJetOutputContainer","L1_gFexSRJetRoI","Output gFexBlock (small-R jet) container"};
    SG::WriteHandleKey< xAOD::gFexJetRoIContainer > m_gFexJetOutKey {this,"Key_gFexLRJetOutputContainer","L1_gFexLRJetRoI","Output gFexJet (large-R jet) container"};
    
    SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gScalarEJwojOutKey {this,"Key_gScalarEJwojOutputContainer","L1_gScalarEJwoj","Output Scalar MET and SumET (from Jets without Jets algo) container"};
    SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMETComponentsJwojOutKey {this,"Key_gMETComponentsJwojOutputContainer","L1_gMETComponentsJwoj","Output total MET components (from Jets without Jets algo) container"};
    SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMHTComponentsJwojOutKey {this,"Key_gMHTComponentsJwojOutputContainer","L1_gMHTComponentsJwoj","Output hard MET components (from Jets without Jets algo) container"};
    SG::WriteHandleKey< xAOD::gFexGlobalRoIContainer > m_gMSTComponentsJwojOutKey {this,"Key_gMSTComponentsJwojOutputContainer","L1_gMSTComponentsJwoj","Output soft MET components (from Jets without Jets algo) container"};



    std::vector<uint32_t>  m_allgRhoTobs;
    std::vector<uint32_t>  m_allgBlockTobs;
    std::vector<uint32_t>  m_allgJetTobs;

    std::vector<uint32_t>  m_allgScalarEJwojTobs;
    std::vector<uint32_t>  m_allgMETComponentsJwojTobs;
    std::vector<uint32_t>  m_allgMHTComponentsJwojTobs;
    std::vector<uint32_t>  m_allgMSTComponentsJwojTobs;

  };

} // end of namespace


#endif
