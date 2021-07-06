/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBSCINTREC_H
#define TBREC_TBSCINTREC_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>

class TBScintillatorRec : public AthAlgorithm
{
 public:
  TBScintillatorRec(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBScintillatorRec();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


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
  std::vector<float> m_scint_timecalib;
  std::vector<float> m_scint_ped;


};

#endif
