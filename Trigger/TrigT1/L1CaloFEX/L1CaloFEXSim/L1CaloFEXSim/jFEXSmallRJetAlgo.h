/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXSmallRJetAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 03 11 2020
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#ifndef jFEXSmallRJetAlgo_H
#define jFEXSmallRJetAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXSmallRJetAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"

#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h" 


namespace LVL1 {

  class jFEXSmallRJetAlgo : public AthAlgTool, virtual public IjFEXSmallRJetAlgo {

  public:
    /** Constructors */
    jFEXSmallRJetAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    /** Destructor */
    virtual ~jFEXSmallRJetAlgo();

    virtual StatusCode safetyTest() override;
    virtual void setup(int inputTable[5][5]) override;
    virtual void setupCluster(int inputTable[4][5]) override;
    virtual unsigned int getRealPhi() override;
    virtual unsigned int getRealEta() override;   
    virtual unsigned int getTTowerET() override;
    virtual void buildSeeds() override; 
    virtual bool isSeedLocalMaxima() override; 
    virtual unsigned int getClusterET() override;
    virtual std::unique_ptr<jFEXSmallRJetTOB> getSmallRJetTOBs() override;
  //  virtual jFEXSmallRJetTOB* getSmallRJetTOBs() override;
//LVL1::jFEXSmallRJetAlgoTOB * LVL1::jFEXSmallRJetAlgo::getSmallRJetTOB()
    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jFEXSmallRJetAlgo_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        int m_jFEXalgoTowerID[5][5];
        int m_jFEXalgoSearchWindowSeedET[5][5];
        int m_smallRJetClusterIDs[4][5];
	bool m_seedSet;
        bool m_clusterSet;
  };


}//end of namespace
#endif
