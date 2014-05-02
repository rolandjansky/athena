/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoSelClearAbsPosAction.h"
#include "GeoModelKernel/GeoClearAbsPosAction.h"
#include "GeoModelKernel/GeoAlignableTransform.h"
#include <sstream>

GeoSelClearAbsPosAction::GeoSelClearAbsPosAction(const GeoAlignableTransform* alignTrans)
  : m_alignTrans(alignTrans)
  , m_needToClearCache(false)
{
  setDepthLimit(1);
}

GeoSelClearAbsPosAction::~GeoSelClearAbsPosAction()
{
}

void GeoSelClearAbsPosAction::handleTransform(const GeoTransform *xform)
{
  if(xform==m_alignTrans)
    m_needToClearCache = true;
}

void GeoSelClearAbsPosAction::handlePhysVol(const GeoPhysVol *vol)
{
  if(getPath()->getLength()>1 && m_needToClearCache) {
    GeoClearAbsPosAction cc;
    vol->exec(&cc);
    m_needToClearCache = false;
  }
}

void GeoSelClearAbsPosAction::handleFullPhysVol(const GeoFullPhysVol *vol)
{
  if(getPath()->getLength()>1 && m_needToClearCache) {
    GeoClearAbsPosAction cc;
    vol->exec(&cc);
    m_needToClearCache = false;
  }
}



