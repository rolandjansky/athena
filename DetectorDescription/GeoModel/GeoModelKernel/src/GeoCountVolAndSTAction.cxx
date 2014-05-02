/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "GeoModelKernel/GeoCountVolAndSTAction.h"

GeoCountVolAndSTAction::GeoCountVolAndSTAction()
  :count (0)
{
  setDepthLimit (1);
}

GeoCountVolAndSTAction::~GeoCountVolAndSTAction()
{
}

void GeoCountVolAndSTAction::handlePhysVol(const GeoPhysVol*)
{
  //    
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    count++;
}

void GeoCountVolAndSTAction::handleFullPhysVol(const GeoFullPhysVol*)
{
  //   
  // Do not count the top volume, this action counts only children!   
  //   
  if (getPath ()->getLength () > 1)
    count++;
}

void GeoCountVolAndSTAction::handleSerialTransformer(const GeoSerialTransformer *)
{
  count ++;
}





