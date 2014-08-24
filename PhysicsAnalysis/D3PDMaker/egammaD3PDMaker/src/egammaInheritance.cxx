/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file egammaD3PDMaker/src/egammaInheritance.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Oct, 2009
 * @brief Declare inheritance relationships for egamma classes.
 *
 * Eventually, these should be moved to the EDM classes.
 */

#include "egammaEvent/Electron.h"
#include "egammaEvent/Photon.h"
#include "egammaEvent/EMTrackFit.h"
#include "egammaEvent/EMBremFit.h"
#include "egammaEvent/EMConvert.h"
#include "egammaEvent/EMErrorDetail.h"
#include "egammaEvent/EMTrackMatch.h"
#include "AnalysisTriggerEvent/EmTau_ROI.h"
#include "EventKernel/INavigable4Momentum.h"
#include "Navigation/IAthenaBarCode.h"
#include "SGTools/BaseInfo.h"
#include "xAODBase/IParticle.h"
#include "xAODEgamma/Electron.h"

SG_ADD_BASE (EmTau_ROI, SG_VIRTUAL (INavigable4Momentum));
SG_ADD_BASE (Analysis::Electron, SG_VIRTUAL (IAthenaBarCode));
SG_ADD_BASE (Analysis::Photon, SG_VIRTUAL (IAthenaBarCode));
SG_ADD_BASE (egamma, SG_VIRTUAL (IAthenaBarCode));

SG_ADD_BASE (EMTrackFit,    egDetail);
SG_ADD_BASE (EMBremFit,     egDetail);
SG_ADD_BASE (EMConvert,     egDetail);
SG_ADD_BASE (EMErrorDetail, egDetail);
SG_ADD_BASE (EMTrackMatch,  egDetail);


