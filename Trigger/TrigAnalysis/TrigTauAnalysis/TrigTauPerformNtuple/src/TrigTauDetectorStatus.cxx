/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauPerformNtuple/TrigTauDetectorStatus.h"

ClassImp(TrigTauDetectorStatus )

TrigTauDetectorStatus::TrigTauDetectorStatus()
  : TObject()
{
  reset();
}

TrigTauDetectorStatus::TrigTauDetectorStatus(const TrigTauDetectorStatus & other)
  : TObject()
  , m_DetectorStatus(other.m_DetectorStatus)
  , m_DetectorElement(other.m_DetectorElement)
{}

TrigTauDetectorStatus::~TrigTauDetectorStatus() {}

void TrigTauDetectorStatus::reset()
{
  m_DetectorElement.clear();
  m_DetectorStatus.clear();
}

