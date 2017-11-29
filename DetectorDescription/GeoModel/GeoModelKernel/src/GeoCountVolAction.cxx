/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoCountVolAction.h"

GeoCountVolAction::GeoCountVolAction ()
 : m_count (0)
{
  setDepthLimit (1);
}

GeoCountVolAction::~GeoCountVolAction()
{
}

void GeoCountVolAction::handlePhysVol (const GeoPhysVol *)
{
  //    
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    m_count++;
}

void GeoCountVolAction::handleFullPhysVol (const GeoFullPhysVol *)
{
  //   
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    m_count++;
}

void GeoCountVolAction::handleSerialTransformer (const GeoSerialTransformer  *st)
{
  m_count += st->getNCopies ();
}
