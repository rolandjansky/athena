/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**    @Afile HLTMuonMonTool.cxx
 *   
 *    authors: 
 */

#include "AthenaMonitoring/AthenaMonManager.h"
#include "AthenaMonitoring/ManagedMonitorToolTest.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"


#include "TrigMuonEvent/TrigMuonEFContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfoTrackContainer.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "TrigMuonEvent/TrigMuonEFTrack.h"
#include "TrigMuonEvent/TrigMuonEFCbTrack.h"

#include "TROOT.h"
#include "TH1I.h"
#include "TH1F.h"
#include "TH2I.h"
#include "TH2F.h"

#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

#include "TrigMuonMonitoring/HLTMuonMonTool.h"

//for GetKalmanUpdator
#include "GaudiKernel/ListItem.h"

using namespace std;
StatusCode HLTMuonMonTool::initMuGirlDQA()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::bookMuGirlDQA()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::fillMuGirlDQA()
{
  return StatusCode::SUCCESS;
}

StatusCode HLTMuonMonTool::procMuGirlDQA()
{
  return StatusCode::SUCCESS;
}
