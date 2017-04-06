/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TruthController.h"
#include "SimHelpers/TrackVisualizationHelper.h"

TruthController* TruthController::s_thePointer=TruthController::GetTruthController() ;

TruthController* TruthController::GetTruthController()
{
	if (!s_thePointer) s_thePointer=new TruthController;
	return s_thePointer;
}

TruthController::TruthController()
{
	m_theVisHelper=new TrackVisualizationHelper;
}

TruthController::~TruthController()
{
        if (m_theVisHelper) delete m_theVisHelper;
}
void TruthController::SetVisualizationHelper(TrackVisualizationHelper *h)
{
	if (m_theVisHelper==h) return;
	delete m_theVisHelper;
	m_theVisHelper=h;
}
