// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: LinkDef.h 784257 2016-11-15 11:59:07Z krasznaa $
#ifndef REWEIGHTUTILS_LINKDEF_H
#define REWEIGHTUTILS_LINKDEF_H

// Local include(s):
#include <ReweightUtils/APWeightEntry.h>
#include <ReweightUtils/APReweightBase.h>
#include <ReweightUtils/APReweight.h>
#include <ReweightUtils/APReweight2D.h>
#include <ReweightUtils/APReweight3D.h>
#include <ReweightUtils/APReweightND.h>
#include <ReweightUtils/APWeightHist.h>
#include <ReweightUtils/APEvtWeight.h>
#include <ReweightUtils/APWeightSum.h>
#include <ReweightUtils/APWeightSumEnsemble.h>

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;
#pragma link C++ nestedclass;

#pragma link C++ class APWeightEntry+;
#pragma link C++ class APReweightBase+;
#pragma link C++ class APReweight+;
#pragma link C++ class APReweight2D+;
#pragma link C++ class APReweight3D+;
#pragma link C++ class APReweightND+;
#pragma link C++ class APWeightHist+;
#pragma link C++ class APEvtWeight+;
#pragma link C++ class APWeightSum+;
#pragma link C++ class APWeightSumEnsemble+;

#endif // __CINT__
#endif // not REWEIGHTUTILS_LINKDEF_H
