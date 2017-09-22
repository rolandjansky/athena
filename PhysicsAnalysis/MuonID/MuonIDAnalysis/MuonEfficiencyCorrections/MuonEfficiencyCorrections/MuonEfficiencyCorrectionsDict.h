/*
 Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

/*
 * MuonEfficiencyCorrectionsDict.h
 *
 *  Created on: Nov 12, 2014
 *      Author: goblirsc
 */

#ifndef MUONEFFICIENCYCORRECTIONSDICT_H_
#define MUONEFFICIENCYCORRECTIONSDICT_H_

#if defined(__GCCXML__) and not defined(EIGEN_DONT_VECTORIZE)
#   define EIGEN_DONT_VECTORIZE
#endif // __GCCXML__

#include "MuonEfficiencyCorrections/DetRegionBinning.h"
#include "MuonEfficiencyCorrections/EfficiencyScaleFactor.h"
#include "MuonEfficiencyCorrections/EffiCollection.h"
//#include "MuonEfficiencyCorrections/errorcheck.h"
#include "MuonEfficiencyCorrections/fineEtaPhiBinning.h"
#include "MuonEfficiencyCorrections/HistHandler.h"

#include "MuonEfficiencyCorrections/KinematicSystHandler.h"
#include "MuonEfficiencyCorrections/MuonEfficiencyScaleFactors.h"

#include "MuonEfficiencyCorrections/MuonSFTestHelper.h"
#include "MuonEfficiencyCorrections/MuonTriggerScaleFactors.h"


#endif /* MUONEFFICIENCYCORRECTIONSDICT_H_ */
