/*
 Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//              jFEXForwardJetsAlgo - Algorithm for small R jet Algorithm in jFEX
//                              -------------------
//     begin                : 07 06 2021
//     email                : varsiha.sothilingam@cern.ch
//***************************************************************************

#ifndef jFEXForwardJetsAlgo_H
#define jFEXForwardJetsAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IjFEXForwardJetsAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
//#include "L1CaloFEXSim/jFEXSmallRJetTOB.h"
#include "L1CaloFEXSim/jTowerContainer.h"
#include "L1CaloFEXSim/jFEXForwardJetsInfo.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"
#include "CaloEvent/CaloCellContainer.h"
#include "CaloIdentifier/CaloIdManager.h" 
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "AthenaBaseComps/AthAlgorithm.h" 
#include "StoreGate/StoreGateSvc.h" 


namespace LVL1 {

  class jFEXForwardJetsAlgo : public AthAlgTool, virtual public IjFEXForwardJetsAlgo {

  public:
    /** Constructors */
    jFEXForwardJetsAlgo(const std::string& type, const std::string& name, const IInterface* parent);
   
    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;

    /** Destructor */
    virtual ~jFEXForwardJetsAlgo();

    virtual StatusCode safetyTest() override;

    virtual void setup(int inputTable[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width]) override;
    //virtual int realValue(int ID, int eta) override;
    virtual float globalPhi(int nphi, int neta) override;
    virtual float globalEta(int nphi, int neta) override;   
    virtual unsigned int localPhi(int nphi, int neta) override;
    virtual unsigned int localEta(int nphi, int neta) override;
    virtual unsigned int getTTowerET(int nphi, int neta) override; 
    virtual std::map<int, jFEXForwardJetsInfo> FcalJetsTowerIDLists() override;
    virtual std::map<int, jFEXForwardJetsInfo> isSeedLocalMaxima() override;
    virtual std::map<int, jFEXForwardJetsInfo> calculateJetETs() override;
    virtual void setFPGAEnergy(std::map<int,std::vector<int> > et_map)  override;

  protected:

  private:
        SG::ReadHandleKey<LVL1::jTowerContainer> m_jFEXForwardJetsAlgo_jTowerContainerKey {this, "MyjTowers", "jTowerContainer", "Input container for jTowers"};
        int m_jFEXalgoTowerID[FEXAlgoSpaceDefs::jFEX_algoSpace_height][FEXAlgoSpaceDefs::jFEX_wide_algoSpace_width];
        bool m_storeEnergyRingTTIDs; 
        std::map<int,std::vector<int> > m_map_Etvalues;
        int m_lowerEM_eta;
        int m_upperEM_eta;
  };
}//end of namespace
#endif
