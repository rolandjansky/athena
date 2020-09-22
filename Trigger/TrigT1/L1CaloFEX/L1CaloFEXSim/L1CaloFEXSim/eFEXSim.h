/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
//                           eFEXSim.h  -  
//                              -------------------
//     begin                : 22 08 2019
//     email                : jacob.julian.kempster@cern.ch
//  ***************************************************************************/


#ifndef eFEXSim_H
#define eFEXSim_H
#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IeFEXSim.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eFEXFPGA.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

namespace LVL1 {
  
  //Doxygen class description below:
  /** The eFEXSim class defines the structure of a single eFEX
      Its purpose is:
      - to emulate the steps taken in processing data for a single eFEX in hardware and firmware
      - It will need to interact with eTowers and produce the eTOBs.  It will be created and handed data by eFEXSysSim
  */
  
  class eFEXSim : public AthAlgTool, virtual public IeFEXSim {
    
  public:

    /** Constructors */
    eFEXSim(const std::string& type,const std::string& name,const IInterface* parent);

    /** Destructor */
    virtual ~eFEXSim();

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;
    /** standard Athena-Algorithm method */
    virtual StatusCode finalize  () override;

    virtual void init (int id) override ;

    virtual void reset () override ;

    virtual void execute() override ;

    virtual int ID() override {return m_id;}
    
    virtual void SetTowersAndCells_SG(int tmp[10][18]) override;

    virtual StatusCode NewExecute(int tmp[10][18]) override;

    /** Internal data */
  private:

    int m_id;

    int m_eTowersIDs [10][18];
    std::map<int,eTower> m_eTowersColl;
    CaloCellContainer m_sCellsCollection;
    std::vector<eFEXFPGA*> m_eFEXFPGACollection;

    ToolHandle<IeFEXFPGA> m_eFEXFPGATool {this, "eFEXFPGATool", "LVL1::eFEXFPGA", "Tool that simulates the FPGA hardware"};

    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXSim , 32201200 , 1 )


#endif
