/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TrigInfo_h__
#define __TrigInfo_h__

//::: STL includes
#include <string>

//::: ROOT includes
#include <TTree.h>

//::: Framework includes
#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"

//::: Local includes
#include "MuonPtCalibNtupleMaker/TrigItem.h"

class TrigInfo { 
  
private:
  std::map< std::string, TrigItem* > m_Info;

public:
  TrigInfo();
  ~TrigInfo() {}

  void Reset();
  void Register( TTree* t );
  void Fill( ToolHandle<Trig::TrigDecisionTool>& tdt );
  void FillMatching( ToolHandle< Trig::ITrigMuonMatching >& mt, const xAOD::Muon* mu1, const xAOD::Muon* mu2 );

};

#endif
