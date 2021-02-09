/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXLargeRJetAlgo - Algorithm for large R jet Algorithm in jFEX
//                              -------------------
//     begin                : 22 01 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#ifndef jFEXLargeRJetAlgo_H
#define jFEXLargeRJetAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/CLASS_DEF.h" 
#include "L1CaloFEXToolInterfaces/IjFEXLargeRJetAlgo.h"
#include "L1CaloFEXToolInterfaces/IjFEXSmallRJetAlgo.h"
#include "L1CaloFEXSim/jFEXLargeRJetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h" 


namespace LVL1 {

  class jFEXLargeRJetAlgo : public AthAlgTool, virtual public IjFEXLargeRJetAlgo {

  public:
    /** Constructors */
    jFEXLargeRJetAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    /** Destructor */
    virtual ~jFEXLargeRJetAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setupCluster(int inputTable[15][15]) override;
    virtual unsigned int getRingET() override;
    virtual unsigned int getLargeClusterET(unsigned int smallClusterET, unsigned int largeRingET) override;
    virtual std::unique_ptr<jFEXLargeRJetTOB> getLargeRJetTOBs() override;

  protected:

  private:

    SG::ReadHandleKey<LVL1::jTowerContainer> m_jFEXLargeRJetAlgo_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
    ToolHandle<IjFEXSmallRJetAlgo> m_jFEXSmallRJetAlgoTool {this, "jFEXSmallRJetAlgoTool", "LVL1::jFEXSmallRJetAlgo", "Tool that runs the jFEX Small R Jet algorithm"};

    
    //int inputTable[15][15];
    int m_largeRJetEtRing_IDs[15][15];
  };


}//end of namespace
#endif

