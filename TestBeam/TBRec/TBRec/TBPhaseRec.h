/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TBREC_TBPHASEREC
#define TBREC_TBPHASEREC

/*! \brief reconstruction of a suitable event phase
 *
 * \author Michel Lefebvre <lefebvre@uvic.ca>
 * \date August 13, 2004
 *  
 * June 30, 2004 - implementation based on a first draft from Marco Delmastro
 */  

#include "AthenaBaseComps/AthAlgorithm.h"

#include <string>
#include <vector>

class StoreGateSvc;

class TBPhaseRec : public AthAlgorithm
{
 public:
  TBPhaseRec(const std::string& name, ISvcLocator* pSvcLocator);
  ~TBPhaseRec();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
  StatusCode getnewcalib();
  
  static const int unknown = -99999 ;

 private:
  ////////////////
  // Properties //
  ////////////////
  std::vector<std::string> m_tdcNames ;   // the TDC(s) with phase information
  std::vector<float> m_tdcToTime;         // per TDC
  std::vector<float> m_tdcwac;            // wrap around constants
  std::vector<float> m_tdcMin;            // used only for choosing the best TDC if necessary
  float m_delta;                          // ttc clock period
  int m_timeBins;                         // number of time bins in a ttc period
  std::string m_TBPhaseKey;               // key of the TBPhase object that TBPhaseRec tries to create
  bool m_neverReturnFailure;              // controls when StatusCode::FAILURE can be issued in execute()

  std::string m_calib_filename;
  unsigned int m_runnumber;

  float m_guardValue;                     // guard region cut

  ///////////////////////////////
  // member data and functions //
  ///////////////////////////////
  
  int m_nTDC ;
  std::vector<int> m_tdcRaw;
  std::vector<float> m_phaseReco;
  float computePhase(int tdcIndex);
  
};

#endif
