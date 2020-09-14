/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/eFEXSim.h"
#include "L1CaloFEXSim/eTower.h"
#include "L1CaloFEXSim/eTowerContainer.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"

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

    virtual StatusCode execute() override ;

    virtual void init() override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int mod) override ;

    /** Internal data */
  private:

    std::vector<eFEXSim*>  m_eFEXCollection;
    
    ToolHandle<IeFEXSim> m_eFEXSimTool       {this, "eFEXSimTool",    "LVL1::eFEXSim",    "Tool that creates the eFEX Simulation"};

    SG::ReadHandleKey<LVL1::eTowerContainer> m_eTowerContainerSGKey {this, "MyETowers", "eTowerContainer", "Input container for eTowers"};
    SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

    std::map<int,eTower> m_eTowersColl;
    
  };
  
} // end of namespace

//CLASS_DEF( LVL1::eFEXSysSim , 32201258 , 1 )


#endif
