/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MCTruth/TruthController.h"
#include "SimHelpers/TrackVisualizationHelper.h"

const TruthController* TruthController::getTruthController()
{
  static const TruthController tc;
  return &tc;
}

TruthController::TruthController()
{
  m_theVisHelper=new TrackVisualizationHelper;
}

TruthController::~TruthController()
{
  if (m_theVisHelper) delete m_theVisHelper;
}
void TruthController::setVisualizationHelper(TrackVisualizationHelper *h)
{
  if (m_theVisHelper==h) return;
  delete m_theVisHelper;
  m_theVisHelper=h;
}
