/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBMWPCREC
#define TBREC_TBMWPCREC
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MWPC Reconstruction algorithm. Based on code for 2002 TestBeam            //
// (see LArCnv/LArHECTBCnv/hectb/Alg_mwpc.cxx)                               //
//  author : Pierre-Antoine Delsart                                          //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include "TBEvent/TBMWPCCont.h"
#include "TBEvent/TBMWPCRawCont.h"


class StoreGateSvc;

class TBMWPCRec : public AthAlgorithm
{
 public:

  TBMWPCRec(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBMWPCRec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();



 private:


  ////////////////
  // Properties //
  ////////////////

  std::string m_SGkey,m_SGrecordkey;

  //////////////////
  // Calib consts //
  //////////////////  
  
  std::string m_mwpc_names[8];

  std::vector<float> m_mwpc_wirestep;
  std::vector<float> m_mwpc_halfsize;

  std::vector<int> m_mwpc_invX;
  std::vector<int> m_mwpc_invY;

};

#endif
