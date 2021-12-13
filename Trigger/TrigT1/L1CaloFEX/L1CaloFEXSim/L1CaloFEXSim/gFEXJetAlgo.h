/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJetAlgo - JetFinder algorithm for gFEX
//                              -------------------
//     begin                : 01 04 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXJetAlgo_H
#define gFEXJetAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgFEXJetAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gFEXJetTOB.h"
#include "L1CaloFEXSim/gTowerContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"



namespace LVL1 {

  typedef  std::array<std::array<int, 6>, 32> gTowersJetEngine;
  typedef  std::array<std::array<int, 4>, 32> gTowersPartialSums;

  class gFEXJetAlgo : public AthAlgTool, virtual public IgFEXJetAlgo {

  public:
    /** Constructors */
    gFEXJetAlgo(const std::string& type, const std::string& name, const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;


virtual std::vector<std::unique_ptr<gFEXJetTOB>> largeRfinder(gTowersCentral Atwr, gTowersCentral Btwr, 
                                                              gTowersForward CNtwr, gTowersForward CPtwr, 
                                                              int pucA, int pucB, int gLJ_seedThrA, int gLJ_seedThrB, 
                                                              int gJ_ptMinToTopoCounts1, int gJ_ptMinToTopoCounts2, 
                                                              int jetThreshold, int gLJ_ptMinToTopoCounts1, int gLJ_ptMinToTopoCounts2,
                                                              std::array<uint32_t, 7> & ATOB1_dat, std::array<uint32_t, 7> & ATOB2_dat,
                                                              std::array<uint32_t, 7> & BTOB1_dat, std::array<uint32_t, 7> & BTOB2_dat) override;

  private:

    virtual void RemotePartialAB(gTowersCentral twrs, gTowersPartialSums & lps, gTowersPartialSums & rps);

    virtual void RemotePartialCN(gTowersForward twrs, gTowersPartialSums & rps);

    virtual void RemotePartialCP(gTowersForward twrs, gTowersPartialSums & lps);

    virtual void singleAB(gTowersCentral twrs, gTowersCentral & FPGAsum);

    virtual void gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum);

    virtual void blkOutAB(gTowersCentral blocks,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> jetOutL, std::array<int, FEXAlgoSpaceDefs::ABCrows> etaIndL,
                           std::array<int, FEXAlgoSpaceDefs::ABCrows> jetOutR, std::array<int, FEXAlgoSpaceDefs::ABCrows> etaIndR );

    virtual void gBlockMax2(gTowersCentral gBlkSum, int BjetColumn, std::array<int, 3> & gBlockV, std::array<int, 3> & gBlockEta, std::array<int, 3> & gBlockPhi);

    virtual void gBlockMax192(gTowersJetEngine gBlkSum, std::array<int, 3> & gBlockVp, std::array<int, 3> & gBlockEtap, std::array<int, 3> & gBlockPhip, int index);

    virtual void addRemoteRin(gTowersCentral &jets, const gTowersPartialSums &partial);

    virtual void addRemoteLin(gTowersCentral &jets, const gTowersPartialSums &partial);

    virtual void pileUpCorrectionAB(gTowersCentral &jets, int puc);

    virtual void gBlockVetoAB(gTowersCentral &twrs, gTowersCentral blocks, int seed_threshold) ;

    virtual void jetOutAB(gTowersCentral jets, gTowersCentral blocks, int seedThreshold,
                           std::array<int, 32> &jetOutL, std::array<int, 32> &etaIndL,
                           std::array<int, 32> &jetOutR, std::array<int, 32> &etaIndR);

    virtual void gJetTOBgen(std::array<int, FEXAlgoSpaceDefs::ABCrows> jetOut,
                             std::array<int, FEXAlgoSpaceDefs::ABCrows> etaInd,
                             int TOBnum, int jetThreshold, std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBs,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBv,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBeta,
                             std::array<int, FEXAlgoSpaceDefs::gJetTOBfib> & gJetTOBphi );

    SG::ReadHandleKey<LVL1::gTowerContainer> m_gFEXJetAlgo_gTowerContainerKey {this, "MyGTowers", "gTowerContainer", "Input container for gTowers"};
    
  };

} // end of namespace


#endif
