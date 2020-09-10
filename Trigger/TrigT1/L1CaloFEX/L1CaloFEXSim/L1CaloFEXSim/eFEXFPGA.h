/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "L1CaloFEXToolInterfaces/IeFEXtauAlgo.h"
#include "L1CaloFEXToolInterfaces/IeFEXegAlgo.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "L1CaloFEXSim/eFEXOutputCollection.h"

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
    virtual StatusCode initialize() ;
    /** Destructor */
    virtual ~eFEXFPGA();

    virtual StatusCode init(int id, int efexid) override ;

    virtual StatusCode execute() override ;

    virtual void reset() override ;

    virtual int ID() override {return m_id;}

    virtual void SetTowersAndCells_SG( int [][6] ) override ;

    /** Internal data */
  private:

    int m_id;
    int m_efexid;

    int m_eTowersIDs [10][6];
    std::map<int,eTower> m_eTowersColl;

    CaloCellContainer m_sCellsCollection;

    SG::ReadHandleKey<LVL1::eTowerContainer> m_eFEXFPGA_eTowerContainerKey {this, "MyETowers", "eTowerContainer", "Input container for eTowers"};

    //SG::ReadHandleKey<eFEXOutputCollection> m_eFEXFPGA_eFEXOutputCollectionKey {this, "MyOutputs", "eFEXOutputCollection", "Input container for eFEXOutputCollection"};

    ToolHandle<IeFEXtauAlgo> m_eFEXtauAlgoTool {this, "eFEXtauAlgoTool", "LVL1::eFEXtauAlgo", "Tool that runs the eFEX tau algorithm"};
    ToolHandle<IeFEXegAlgo> m_eFEXegAlgoTool {this, "eFEXegAlgoTool", "LVL1::eFEXegAlgo", "Tool that runs the eFEX e/gamma algorithm"};
    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXFPGA , 32201201 , 1 )


#endif
