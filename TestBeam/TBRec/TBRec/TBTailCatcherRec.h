/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBTAILCATCHERREC
#define TBREC_TBTAILCATCHERREC
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/Algorithm.h"

#include <string>
#include <vector>
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBTailCatcherRaw.h"


class StoreGateSvc;

class TBTailCatcherRec : public Algorithm
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

  StoreGateSvc* m_StoreGate;
  std::string m_SGkey;

  //////////////////
  // Calib consts //
  //////////////////

  std::vector<float> m_tailcatch_calib;
  std::vector<float> m_tailcatch_ped;

};

#endif
