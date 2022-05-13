/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
//***************************************************************************
//    gFEXaltMetAlgo - Noise cut and Rho+RMS algorithm for gFEX MET
//                              -------------------
//     begin                : 31 03 2022
//     email                : cecilia.tosciri@cern.ch
//***************************************************************************

#ifndef gFEXaltMetAlgo_H
#define gFEXaltMetAlgo_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "L1CaloFEXToolInterfaces/IgFEXaltMetAlgo.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "L1CaloFEXSim/gTowerContainer.h"

#include "AthenaBaseComps/AthAlgorithm.h"
#include "StoreGate/StoreGateSvc.h"
#include "L1CaloFEXSim/FEXAlgoSpaceDefs.h"



namespace LVL1 {

  class gFEXaltMetAlgo : public extends<AthAlgTool, IgFEXaltMetAlgo> {

  public:
    /** Constructor */
    gFEXaltMetAlgo(const std::string& type, const std::string& name, const IInterface* parent);

    /** standard Athena-Algorithm method */
    virtual StatusCode initialize() override;


    virtual void setAlgoConstant(std::vector<int>&& A_thr,
                         std::vector<int>&& B_thr,
                         const int rhoPlusThr) override;
                                 

    virtual void altMetAlgo(const gTowersCentral &Atwr, const gTowersCentral &Btwr,
                            std::array<uint32_t, 4> & outTOB) override;



  private:

    std::array<std::vector<int>, 2> m_etaThr;
    int m_rhoPlusThr;
    
    
    void metFPGA(const gTowersCentral &twrs, int & MET_x, int & MET_y, const unsigned short FPGA_NO);

    void metTotal(const int A_MET_x, const int A_MET_y,
                  const int B_MET_x, const int B_MET_y,
                  int & MET_x, int & MET_y, int & MET);

    int get_rho(const gTowersCentral &twrs);

    int get_sigma(const gTowersCentral &twrs, const int rho);

    void rho_MET(const gTowersCentral &twrs, int & MET_x, int & MET_y, const int rho, const int sigma);

    int sumEtFPGAnc(const gTowersCentral &twrs, const unsigned short FPGA_NO);

    int sumEtFPGArms(const gTowersCentral &twrs, const int sigma);

    int sumEt(const int A_sumEt, const int  B_sumEt);

    float sinLUT(const unsigned int phiIDX, const unsigned int aw);

    float cosLUT(const unsigned int phiIDX, const unsigned int aw);

  };

} // end of namespace


#endif

