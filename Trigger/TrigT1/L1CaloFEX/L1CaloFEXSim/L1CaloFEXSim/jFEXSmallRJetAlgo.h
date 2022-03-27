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
    virtual void setup(int inputTable[7][7]) override;
    virtual int realValue(int ID, int eta) override;
    virtual unsigned int getRealPhi(unsigned int TTID ) override;
    virtual int getRealEta(unsigned int TTID ) override;   
    virtual unsigned int getTTowerET(unsigned int TTID ) override;
    virtual void buildSeeds() override; 
    virtual bool isSeedLocalMaxima() override; 
    virtual unsigned int getSmallClusterET() override;
    virtual unsigned int getSmallETRing() override;
    virtual bool checkDisplacedLM() override;
    virtual std::unique_ptr<jFEXSmallRJetTOB> getSmallRJetTOBs() override;
    virtual unsigned int getTTIDcentre() override;
    virtual void setFPGAEnergy(std::unordered_map<int,std::vector<int> > et_map)  override;
  //  virtual jFEXSmallRJetTOB* getSmallRJetTOBs() override;
//LVL1::jFEXSmallRJetAlgoTOB * LVL1::jFEXSmallRJetAlgo::getSmallRJetTOB()
    
protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        int m_jFEXalgoTowerID[7][7];
        int m_jFEXalgoSearchWindowSeedET[5][5];
	bool m_seedSet;
        bool m_LMDisplaced;
        std::unordered_map<int,std::vector<int> > m_map_Etvalues;
  };


}//end of namespace
#endif
