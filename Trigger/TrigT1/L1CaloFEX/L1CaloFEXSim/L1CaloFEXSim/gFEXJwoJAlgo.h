/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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


    virtual void setAlgoConstant(unsigned int aFPGA_A, unsigned int bFPGA_A,
                                 unsigned int aFPGA_B, unsigned int bFPGA_B,
                                 int gXE_seedThrA, int gXE_seedThrB) override;

    virtual std::vector<std::unique_ptr<gFEXJwoJTOB>> jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                               std::array<uint32_t, 4> & outTOB) override;



  private:

    int m_aFPGA_A;
    int m_bFPGA_A;
    int m_aFPGA_B;
    int m_bFPGA_B;
    int m_gBlockthresholdA;
    int m_gBlockthresholdB;


    virtual void gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum);

    virtual void metFPGA(gTowersCentral twrs, gTowersCentral & gBlkSum, int gBlockthreshold,
                           int & MHT_x, int & MHT_y,
                           int & MST_x, int & MST_y,
                           int & MET_x, int & MET_y);

    virtual void metTotal(int A_MET_x, int A_MET_y,
                          int B_MET_x, int B_MET_y,
                          int & MET_x, int & MET_y, int & MET);

    virtual void sumEtFPGA(gTowersCentral twrs, int & partial_sumEt);

    virtual void sumEt(int  A_sumEt, int  B_sumEt, int & total_sumEt);

    virtual float sinLUT(unsigned int phiIDX, unsigned int aw);

    virtual float cosLUT(unsigned int phiIDX, unsigned int aw);


  };

} // end of namespace


#endif
