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
                                 int gblockThreshold) override;

    virtual std::vector<std::unique_ptr<gFEXJwoJTOB>> jwojAlgo(gTowersCentral Atwr, gTowersCentral Btwr,
                                                               std::array<uint32_t, 4> & outTOB) override;



  private:

    unsigned int m_aFPGA_A;
    unsigned int m_bFPGA_A;
    unsigned int m_aFPGA_B;
    unsigned int m_bFPGA_B;
    int m_gBlockthreshold;


    virtual void gBlockAB(gTowersCentral twrs, gTowersCentral & gBlkSum);

    virtual void metFPGA(gTowersCentral twrs, gTowersCentral & gBlkSum,
                           unsigned short & MHT_x, unsigned short & MHT_y,
                           unsigned short & MST_x, unsigned short & MST_y,
                           unsigned short & MET_x, unsigned short & MET_y);

    virtual void metTotal(unsigned short A_MET_x, unsigned short A_MET_y,
                          unsigned short B_MET_x, unsigned short B_MET_y,
                          unsigned short & MET_x, unsigned short & MET_y, unsigned short & MET);

    virtual void sumEtFPGA(gTowersCentral twrs, unsigned int & partial_sumEt);

    virtual void sumEt(unsigned int  A_sumEt, unsigned int  B_sumEt, unsigned int & total_sumEt);

    virtual unsigned short sinLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw);

    virtual unsigned short cosLUT(unsigned int phiIDX, unsigned int aw, unsigned int dw);


  };

} // end of namespace


#endif
