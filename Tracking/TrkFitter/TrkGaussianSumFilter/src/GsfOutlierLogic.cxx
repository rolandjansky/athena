/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* *******************************************************************************
      GsfOutlierLogic.cxx  -  description
      -----------------------------------
begin                : Tuesday 15th March 2005
author               : amorley, atkinson
email                : Anthony.Morley@cern.ch, Tom.Atkinson@cern.ch
decription           : Implementation code for GsfOutlierLogic class
********************************************************************************** */

#include "TrkGaussianSumFilter/GsfOutlierLogic.h"

#include "TrkMultiComponentStateOnSurface/MultiComponentStateOnSurface.h"
#include "TrkEventPrimitives/FitQuality.h"

Trk::GsfOutlierLogic::GsfOutlierLogic(const std::string& type, const std::string& name, const IInterface* parent)
  :
  AthAlgTool(type, name, parent),  
  m_outputlevel(0)
{

  declareInterface<IGsfOutlierLogic>(this);

}

StatusCode Trk::GsfOutlierLogic::initialize()
{

  m_outputlevel = msg().level()-MSG::DEBUG;   // save the threshold for debug printout in private member
  
  msg(MSG::INFO) << "Initialisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

StatusCode Trk::GsfOutlierLogic::finalize()
{

  msg(MSG::INFO) << "Finalisation of " << name() << " was successful" << endmsg;

  return StatusCode::SUCCESS;

}

const Trk::FitQuality* Trk::GsfOutlierLogic::fitQuality(const Trk::SmoothedTrajectory& smoothedTrajectory) const
{

  msg(MSG::VERBOSE) << "GsfOutlierLogic fitQuality" << endmsg;

  double chiSquared = 0.;
  int    numberDoF  = -5;

  // Loop over all TrackStateOnSurface objects in trajectory
  SmoothedTrajectory::const_iterator stateOnSurface = smoothedTrajectory.begin();

  for ( ; stateOnSurface != smoothedTrajectory.end(); ++stateOnSurface ) {

    if ( !(*stateOnSurface)->type(TrackStateOnSurface::Measurement) ) continue;
    if ( (*stateOnSurface)->fitQualityOnSurface() == 0 ) continue;

    chiSquared += (*stateOnSurface)->fitQualityOnSurface()->chiSquared();
    numberDoF  += (*stateOnSurface)->fitQualityOnSurface()->numberDoF();

  }

  if ( std::isnan( chiSquared ) || chiSquared <= 0. ) return 0;

  const FitQuality* fitQuality = new FitQuality( chiSquared, numberDoF );

  return fitQuality;

}
