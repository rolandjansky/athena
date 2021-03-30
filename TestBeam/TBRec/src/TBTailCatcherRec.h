/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBTAILCATCHERREC_H
#define TBREC_TBTAILCATCHERREC_H
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBTailCatcherRaw.h"


class TBTailCatcherRec : public AthAlgorithm
{
 public:

  TBTailCatcherRec(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TBTailCatcherRec();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;


 private:
  ////////////////
  // Properties //
  ////////////////

  std::string m_SGkey;

  //////////////////
  // Calib consts //
  //////////////////

  std::vector<float> m_tailcatch_calib;
  std::vector<float> m_tailcatch_ped;

};

#endif
