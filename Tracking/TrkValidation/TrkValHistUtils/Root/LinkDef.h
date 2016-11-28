/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __TRKVALHISTUTILS__
#define __TRKVALHISTUTILS__

#include "TrkValHistUtils/EfficiencyPlots.h"
#include "TrkValHistUtils/EfficiencyPurityCalculator.h"
#include "TrkValHistUtils/HitTypePlots.h"
#include "TrkValHistUtils/IDHitPlots.h"
#include "TrkValHistUtils/ImpactPlots.h"
#include "TrkValHistUtils/MSHitDiffPlots.h"
#include "TrkValHistUtils/MSHitPlots.h"
#include "TrkValHistUtils/ParamPlots.h"
#include "TrkValHistUtils/PlotBase.h"
#include "TrkValHistUtils/RecoInfoPlots.h"
#include "TrkValHistUtils/ResolutionPlots.h"
#include "TrkValHistUtils/TruthInfoPlots.h"
#include "TrkValHistUtils/TruthTrkExtrapolationPlots.h"

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ namespace Trk ;

#pragma link C++ class PlotBase+ ;
#pragma link C++ class EfficiencyPurityCalculator+ ;
#pragma link C++ class Trk::EfficiencyPlots+ ;
#pragma link C++ class Trk::HitTypePlots+ ;
#pragma link C++ class Trk::IDHitTypePlots+ ;
#pragma link C++ class Trk::ImpactPlots+ ;
#pragma link C++ class Trk::MSHitDiffPlots+ ;
#pragma link C++ class Trk::MSHitPlots+ ;
#pragma link C++ class Trk::ParamPlots+ ;
#pragma link C++ class Trk::RecoInfoPlots+ ;
#pragma link C++ class Trk::ResolutionPlots+ ;
#pragma link C++ class Trk::TruthInfoPlots+ ;
#pragma link C++ class Trk::TruthTrkExtrapolationPlots+ ;
#endif

#endif
