/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBBPCREC
#define TBREC_TBBPCREC
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBBPCRawCont.h"


class StoreGateSvc;

class TBBPCRec : public AthAlgorithm
{



 public:

  TBBPCRec(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBBPCRec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode getnewcalib();

 private:


  ////////////////
  // Properties //
  ////////////////

  std::string m_SGkey,m_SGrecordkey;
  std::string m_calib_filename;

  unsigned int m_runnumber;

  //////////////////
  // Calib consts //
  //////////////////  
  
  std::vector<std::string> m_bpc_names;

  std::vector<float> m_bpc_calibX;
  std::vector<float> m_bpc_calibY;
  std::vector<float> m_bpc_leftright;
  std::vector<float> m_bpc_updown;
  std::vector<float> m_bpc_udoffset;
  std::vector<float> m_bpc_lroffset;

  std::vector<float> m_bpc_invX;
  std::vector<float> m_bpc_invY;

  std::vector<float> m_tdccutmin;
  std::vector<float> m_tdccutmax;
  std::vector<float> m_tdccutlr;
  std::vector<float> m_tdccutud;

  std::vector<float> m_rotation;

};

#endif
