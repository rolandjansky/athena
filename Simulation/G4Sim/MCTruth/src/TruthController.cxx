/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TruthController.h"
#include "SimHelpers/TrackVisualizationHelper.h"

TruthController* TruthController::thePointer=TruthController::GetTruthController() ;

TruthController* TruthController::GetTruthController()
{
	if (!thePointer) thePointer=new TruthController;
	return thePointer;
}

TruthController::TruthController()
{
	theVisHelper=new TrackVisualizationHelper;
}

TruthController::~TruthController()
{
        if (theVisHelper) delete theVisHelper;
}
void TruthController::SetVisualizationHelper(TrackVisualizationHelper *h)
{
	if (theVisHelper==h) return;
	delete theVisHelper;
	theVisHelper=h;
}
