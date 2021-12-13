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

    unsigned int m_aFPGA_A;
    unsigned int m_bFPGA_A;
    unsigned int m_aFPGA_B;
    unsigned int m_bFPGA_B;
    int m_gBlockthresholdA;
    int m_gBlockthresholdB;


    virtual void gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum);

    virtual void metFPGA(gTowersCentral twrs, gTowersCentral & gBlkSum, int gBlockthreshold,
                           unsigned int & MHT_x, unsigned int & MHT_y,
                           unsigned int & MST_x, unsigned int & MST_y,
                           unsigned int & MET_x, unsigned int & MET_y);

    virtual void metTotal(unsigned int A_MET_x, unsigned int A_MET_y,
                          unsigned int B_MET_x, unsigned int B_MET_y,
                          unsigned int & MET_x, unsigned int & MET_y, unsigned int & MET);

    virtual void sumEtFPGA(gTowersCentral twrs, unsigned int & partial_sumEt);

    virtual void sumEt(unsigned int  A_sumEt, unsigned int  B_sumEt, unsigned int & total_sumEt);

    virtual unsigned int sinLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw);

    virtual unsigned int cosLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw);


  };

} // end of namespace


#endif
