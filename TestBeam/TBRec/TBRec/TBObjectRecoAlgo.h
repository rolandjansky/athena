/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBOBJECTRECOALGO
#define TBREC_TBOBJECTRECOALGO
///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This algorithm controls event flow and reconstruction and monitoring      //
// tasks.                                                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/Algorithm.h"

#include <string>
#include <vector>
#include "TBEvent/TBBPCCont.h"
#include "TBEvent/TBBPCRawCont.h"
#include "TBEvent/TBMWPCCont.h"
#include "TBEvent/TBMWPCRawCont.h"
#include "TBEvent/TBScintillatorCont.h"
#include "TBEvent/TBScintillatorRawCont.h"
#include "TBEvent/TBTailCatcher.h"
#include "TBEvent/TBTailCatcherRaw.h"

class StoreGateSvc;

class TBObjectRecoAlgo : public Algorithm
{



 public:

  TBObjectRecoAlgo(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBObjectRecoAlgo();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();



 private:

  TBTailCatcher *  BuildTailCatch(TBTailCatcherRaw tailcatchraw);
  TBScintillatorCont *  BuildScint(TBScintillatorRawCont scintrawCont);
  TBBPCCont *  BuildBPC(TBBPCRawCont bpcrawCont);
  TBMWPCCont *  BuildMWPC(TBMWPCRawCont mwpcrawCont);
  void BuildHit();

  ////////////////
  // Properties //
  ////////////////


  //////////////////
  // Calib consts //
  //////////////////

  // BPC
  std::vector<std::string> m_bpc_names;

  std::vector<float> m_bpc_calibX;
  std::vector<float> m_bpc_calibY;
  std::vector<float> m_bpc_leftright;
  std::vector<float> m_bpc_updown;
  std::vector<float> m_bpc_udoffset;
  std::vector<float> m_bpc_lroffset;

  std::vector<float> m_bpc_errX;
  std::vector<float> m_bpc_errY;

  std::vector<float> m_bpc_invX;
  std::vector<float> m_bpc_invY;

  // MWPC
  std::vector<std::string> m_mwpc_names;
  std::vector<float> m_mwpc_invX;
  std::vector<float> m_mwpc_invY;


  // TailCatcher
  std::vector<float> m_tailcatch_calib;

  // Scintillators
  std::vector<std::string> m_scint_names;
  //  std::vector<float> m_scint_calib;
  std::vector<float> m_scint_timecalib;


  StoreGateSvc* m_StoreGate;
};
#endif
