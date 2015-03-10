/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBSCINTREC
#define TBREC_TBSCINTREC
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>

class StoreGateSvc;

class TBScintillatorRec : public AthAlgorithm
{
 public:
  TBScintillatorRec(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBScintillatorRec();

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

  // Scintillators
  std::vector<std::string> m_scint_names;
  //  std::vector<float> m_scint_calib;
  std::vector<float> m_scint_timecalib;
  std::vector<float> m_scint_ped;


};

#endif
