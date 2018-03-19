/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "PileUpTools/PileUpToolBase.h"

PileUpToolBase::PileUpToolBase(const std::string& type,
                               const std::string& name,
                               const IInterface* parent)
  : base_class(type, name, parent)
  , m_firstXing(-999)
  , m_lastXing(999)
  , m_filterPassed(true)
{
  declareProperty("FirstXing", m_firstXing);
  declareProperty("LastXing", m_lastXing);
}
