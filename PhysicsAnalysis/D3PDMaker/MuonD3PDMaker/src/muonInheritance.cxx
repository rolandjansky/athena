/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file MuonD3PDMaker/src/muonInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2010
 * @brief Declare inheritance relationships for muon classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "muonEvent/MuonSpShowerContainer.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "TrigMuonEvent/CombinedMuonFeature.h"
#include "AnalysisTriggerEvent/Muon_ROI.h"
#include "EventKernel/INavigable4Momentum.h"
#include "SGTools/BaseInfo.h"

SG_ADD_BASE (TrigMuonEF,          SG_VIRTUAL(INavigable4Momentum));
SG_ADD_BASE (CombinedMuonFeature, SG_VIRTUAL(INavigable4Momentum));
SG_ADD_BASE (Muon_ROI,            SG_VIRTUAL(INavigable4Momentum));
SG_ADD_BASE (Rec::MuonSpShowerContainer, DataVector<Rec::MuonSpShower>);
