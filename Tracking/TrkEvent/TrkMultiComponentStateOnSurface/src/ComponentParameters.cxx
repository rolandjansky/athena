/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			ComponentParameters.cxx  -  description
			---------------------------------------
begin                : Sunday 8th May 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Implementation code for ComponentParameters class			
*********************************************************************************/

#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"

Trk::ComponentParameters::ComponentParameters()
  :
  std::pair<const Trk::TrackParameters*, double>()
{}

Trk::ComponentParameters::ComponentParameters(const Trk::TrackParameters* trackParameters, double weight)
  :
  std::pair<const Trk::TrackParameters*, double>(trackParameters, weight)
{}

Trk::ComponentParameters::ComponentParameters(const Trk::ComponentParameters& componentParameters)
  :
  std::pair<const Trk::TrackParameters*, double>(componentParameters.first, componentParameters.second)
{}

Trk::ComponentParameters::~ComponentParameters()
{}

const Trk::ComponentParameters* 
Trk::ComponentParameters::clone() const
{
  return new ComponentParameters( (this->first)->clone(), this->second );
}

