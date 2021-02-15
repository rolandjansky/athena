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

    virtual StatusCode execute() override ;

    virtual void init() override ;

    virtual void cleanup() override;

    virtual int calcTowerID(int eta, int phi, int mod) override ;

    /** Internal data */
  private:

    std::vector<jFEXSim*>  m_jFEXCollection;
    
    ToolHandle<IjFEXSim> m_jFEXSimTool       {this, "jFEXSimTool",    "LVL1::jFEXSim",    "Tool that creates the jFEX Simulation"};

    SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerSGKey {this, "MyETowers", "jTowerContainer", "Input container for jTowers"};
    SG::ReadHandleKey<CaloCellContainer> m_scellsCollectionSGKey {this, "SCell", "SCell", "SCell"};

    std::map<int,jTower> m_jTowersColl;
    std::map<int, std::vector<uint32_t> > m_allSmallRJetTobs; 
    std::map<int, std::vector<uint32_t> > m_allLargeRJetTobs;
  };
  
} // end of namespace

//CLASS_DEF( LVL1::jFEXSysSim , 141823245 , 1 )


#endif
