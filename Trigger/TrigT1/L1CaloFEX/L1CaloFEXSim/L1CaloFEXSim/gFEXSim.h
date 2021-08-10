/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXSim - Simulation of the gFEX module
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXSim_H
#define gFEXSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgFEXSim.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gTower.h"
#include "L1CaloFEXSim/gFEXFPGA.h"
#include "L1CaloFEXSim/gFEXJetAlgo.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/gFEXOutputCollection.h"


namespace LVL1 {

  //Doxygen class description below:
  /** The gFEXSim class defines the structure of the gFEX
      Its purpose is:
      - to emulate the steps taken in processing data for gFEX in hardware and firmware
      - It will need to interact with gTowers and produce the gTOBs. It will be created and handed data by gFEXSysSim
  */

  class gFEXSim : public AthAlgTool, virtual public IgFEXSim {

  public:

    /** Constructors */
    gFEXSim(const std::string& type,const std::string& name,const IInterface* parent);

    /** Destructor */
    virtual ~gFEXSim();

    //virtual void init (int id) override ;

    virtual void reset () override ;

    virtual void execute() override ;

    //virtual int ID() override {return m_id;}

    virtual StatusCode executegFEXSim(gTowersIDs tmp) override;

    virtual std::vector<uint32_t> getgRhoTOBs() const override;

    virtual std::vector<uint32_t> getgBlockTOBs() const override;

    virtual std::vector<uint32_t> getgJetTOBs() const override;

    /** Internal data */
  private:

    gTowersIDs m_gTowersIDs;

    CaloCellContainer m_sCellsCollection;

    std::vector<uint32_t>  m_gRhoTobWords;

    std::vector<uint32_t>  m_gBlockTobWords;

    std::vector<uint32_t>  m_gJetTobWords;

    // std::array<uint32_t>  m_gRhoTobWords;

    // std::array<uint32_t>  m_gBlockTobWords;

    // std::array<uint32_t>  m_gJetTobWords;

    ToolHandle<IgFEXFPGA> m_gFEXFPGA_Tool {this, "gFEXFPGATool", "LVL1::gFEXFPGA", "Tool that simulates the FPGA hardware"};

    ToolHandle<IgFEXJetAlgo> m_gFEXJetAlgoTool {this, "gFEXJetAlgoTool", "LVL1::gFEXJetAlgo", "Tool that runs the gFEX jet algorithm"};
  };

} // end of namespace


#endif
