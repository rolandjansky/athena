/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*************************************************************************************
      MultiStateMaterialEffectsAdapter.h  -  description
      --------------------------------------------------
begin                : Tuesday 22nd February 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Converter class for single component material effects to
                       multi-component material effects
************************************************************************************/

#ifndef TrkMultiStateMaterialEffectsAdapter_H
#define TrkMultiStateMaterialEffectsAdapter_H

#include "TrkGaussianSumFilter/IMultiStateMaterialEffects.h"
#include "TrkGaussianSumFilter/MultiStateMaterialEffects.h"

//class ISvcLocator;

namespace Trk{

class IMaterialEffectsUpdator;

class MultiStateMaterialEffectsAdapter :  public MultiStateMaterialEffects, virtual public IMultiStateMaterialEffects 
{

 public:

  MultiStateMaterialEffectsAdapter(const std::string&, const std::string&, const IInterface*);

  virtual ~MultiStateMaterialEffectsAdapter();

  /** Service initialise method */
  StatusCode initialize();

  /** Service finalise method */
  StatusCode finalize();

 private:
  //int                                m_outputlevel;                      //!< to cache current output level
  
  virtual void compute ( const ComponentParameters&, 
       const MaterialProperties&,
       double,
       PropDirection = anyDirection,
       ParticleHypothesis = nonInteracting ) const;

  double extractDeltaP ( const TrackParameters& updatedParameters, const TrackParameters& originalParameters ) const;

  const AmgSymMatrix(5)* extractDeltaCovariance ( const TrackParameters& updatedParameters, const TrackParameters& originalParameters ) const;

};

} // end namespace Trk

#endif
