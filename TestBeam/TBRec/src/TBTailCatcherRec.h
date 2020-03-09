/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  ~TBTailCatcherRec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();



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
