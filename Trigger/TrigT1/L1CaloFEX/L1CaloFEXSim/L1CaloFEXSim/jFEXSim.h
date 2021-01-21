/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           jFEXSim.h  -  
//                              -------------------
//     begin                : 22 08 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef jFEXSim_H
#define jFEXSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXSim.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jTower.h"
#include "L1CaloFEXSim/jFEXFPGA.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The jFEXSim class defines the structure of a single jFEX
      Its purpose is:
      - to emulate the steps taken in processing data for a single jFEX in hardware and firmware
      - It will need to interact with jTowers and produce the eTOBs.  It will be created and handed data by jFEXSysSim
  */
  
  class jFEXSim : public AthAlgTool, virtual public IjFEXSim {
    
  public:

    /** Constructors */
    jFEXSim(const std::string& type,const std::string& name,const IInterface* parent);

    /** Destructor */
    virtual ~jFEXSim();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  () override;

    virtual void init (int id) override ;

    virtual void reset () override ;

    virtual void execute() override ;

    virtual int ID() override {return m_id;}
    
    virtual void SetTowersAndCells_SG(int tmp[16*2][17]) override;
    virtual void SetTowersAndCells_SG(int tmp[16*2][24]) override;

    virtual StatusCode NewExecute(int tmp[16*4][17]) override;
    virtual StatusCode NewExecute(int tmp[16*4][24]) override;
    virtual std::vector<uint32_t> getSmallRJetTOBs() override;


    /** Internal data */
  private:
    static bool etSort(uint32_t i, uint32_t j){ return (((i >> 0 ) & 0x7ff)> ((j >> 0) & 0x7ff));}

    int m_id;

    int m_jTowersIDs_Wide [16*4][17];
    int m_jTowersIDs_Thin [16*4][24];

    std::map<int,jTower> m_jTowersColl;
    CaloCellContainer m_sCellsCollection;
    std::vector<jFEXFPGA*> m_jFEXFPGACollection;
   
    std::vector<std::vector<uint32_t>> m_tobWords;
    ToolHandle<IjFEXFPGA> m_jFEXFPGATool {this, "jFEXFPGATool", "LVL1::jFEXFPGA", "Tool that simulates the FPGA hardware"};

    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::jFEXSim , 246128035 , 1 )


#endif
