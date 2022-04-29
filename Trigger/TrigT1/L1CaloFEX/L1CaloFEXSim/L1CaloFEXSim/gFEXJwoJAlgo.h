/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXJwoJAlgo - Jets without jets algorithm for gFEX
//                              -------------------
//     begin                : 10 08 2021
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXJwoJAlgo_H
#define gFEXJwoJAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgFEXJwoJAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gFEXJwoJTOB.h"
#include "L1CaloFEXSim/gTowerContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"



namespace LVL1 {

  class gFEXJwoJAlgo : public AthAlgTool, virtual public IgFEXJwoJAlgo {

  public:
    /** Constructors */
    gFEXJwoJAlgo(const std::string& type, const std::string& name, const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;


    virtual void setAlgoConstant(float aFPGA_A, float bFPGA_A,
                                 float aFPGA_B, float bFPGA_B,
                                 float aFPGA_C, float bFPGA_C,
                                 int gXE_seedThrA, int gXE_seedThrB, int gXE_seedThrC) override;

    virtual std::vector<std::unique_ptr<gFEXJwoJTOB>> jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                               gTowersForward CNtwr, gTowersForward CPtwr,
                                                               std::array<uint32_t, 4> & outJwojTOB) override;



  private:

    float m_aFPGA_A;
    float m_bFPGA_A;
    float m_aFPGA_B;
    float m_bFPGA_B;
    float m_aFPGA_C;
    float m_bFPGA_C;
    float m_gBlockthresholdA;
    float m_gBlockthresholdB;
    float m_gBlockthresholdC;
 
    void makeFPGAC(gTowersForward twrsCN, gTowersForward twrsCP, gTowersCentral & twrsC);

    void gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum);

    void metFPGA(gTowersCentral twrs, gTowersCentral & gBlkSum, int gBlockthreshold,
                           float aFPGA, float bFPGA,
                           int & MHT_x, int & MHT_y,
                           int & MST_x, int & MST_y,
                           int & MET_x, int & MET_y);

    void metTotal(int A_MET_x, int A_MET_y,
                          int B_MET_x, int B_MET_y,
                          int C_MET_x, int C_MET_y,
                          int & MET_x, int & MET_y, int & MET);

    void sumEtFPGA(gTowersCentral twrs, int & partial_sumEt);

    void sumEt(int  A_sumEt, int  B_sumEt, int  C_sumEt, int & total_sumEt);

    float sinLUT(unsigned int phiIDX, unsigned int aw);

    float cosLUT(unsigned int phiIDX, unsigned int aw);


  };

} // end of namespace


#endif
