/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXFPGA.h  -  
//                              -------------------
//     begin                : 15 10 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef eFEXFPGA_H
#define eFEXFPGA_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXToolInterfaces/IeFEXFPGA.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXToolInterfaces/IeFEXtauAlgo.h"
#include "L1CaloFEXToolInterfaces/IeFEXegAlgo.h"
#include "L1CaloFEXToolInterfaces/IeFEXFormTOBs.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"
#include "TrigConfData/L1Menu.h"


#include <vector>

namespace LVL1 {
  
  //Doxygen class description below:
  /** The eFEXFPGA class defines the structure of a single eFEX FPGA
      Its purpose is:
      - to emulate the steps taken in processing data for a single eFEX FPGA in hardware and firmware
      - It will need to interact with eTowers and produce the eTOBs.  It will be created and handed data by eFEXSim
  */
  
  class eFEXFPGA : public AthAlgTool, virtual public IeFEXFPGA {
    
  public:
    /** Constructors */
    eFEXFPGA(const std::string& type,const std::string& name,const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** Destructor */
    virtual ~eFEXFPGA();

    virtual StatusCode init(int id, int efexid) override ;
    virtual StatusCode execute(eFEXOutputCollection* inputOutputCollection) override ;
    virtual void reset() override ;
    virtual int getID() override {return m_id;}

    virtual void SetTowersAndCells_SG( int [][6] ) override ;
    virtual void SetIsoWP(std::vector<unsigned int> &, std::vector<unsigned int> &, unsigned int &, unsigned int &) override ;

    virtual std::vector <uint32_t> getEmTOBs() override ;
    virtual std::vector <uint32_t> getTauTOBs() override ;

    /** Internal data */
  private:
    const unsigned int m_eFexStep = 25;

    static bool etSort (uint32_t i,uint32_t j) { return (((i >> 0 ) & 0xfff)>((j >> 0 ) & 0xfff)); }

    int m_id;
    int m_efexid;
    std::vector< uint32_t > m_emTobwords;
    std::vector< uint32_t > m_tauTobwords;
    int m_eTowersIDs [10][6];

    SG::ReadHandleKey<TrigConf::L1Menu> m_l1MenuKey{
      this, "L1TriggerMenu", "DetectorStore+L1TriggerMenu",
      "Name of the L1Menu object to read configuration from"};

    SG::ReadHandleKey<LVL1::eTowerContainer> m_eFEXFPGA_eTowerContainerKey {
      this, "MyETowers", "eTowerContainer", 
	"Input container for eTowers"};

    ToolHandle<IeFEXtauAlgo> m_eFEXtauAlgoTool {
      this, "eFEXtauAlgoTool", "LVL1::eFEXtauAlgo", 
	"Tool that runs the eFEX tau algorithm"};
    ToolHandle<IeFEXegAlgo> m_eFEXegAlgoTool {
      this, "eFEXegAlgoTool", "LVL1::eFEXegAlgo", 
	"Tool that runs the eFEX e/gamma algorithm"};
    
    ToolHandle<IeFEXFormTOBs> m_eFEXFormTOBsTool {this, "eFEXFormTOBs", "LVL1::eFEXFormTOBs", "Tool that creates eFEX TOB words"};
  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXFPGA , 32201201 , 1 )


#endif
