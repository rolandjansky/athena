/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
      MultipleScatterUpdator.h  -  description
      ----------------------------------------
begin                : Tuesday 22nd January 2005
author               : atkinson
email                : Tom.Atkinson@cern.ch
decription           : Class for describing multiple scattering effects only.
                       More or less duplication from Andi
*********************************************************************************/

#ifndef Trk_MultipleScatteringUpdator_H
#define Trk_MultipleScatteringUpdator_H

#include "TrkExInterfaces/IMaterialEffectsUpdator.h"
#include "TrkExUtils/MaterialUpdateMode.h"
#include "TrkEventPrimitives/PropDirection.h"
#include "TrkParameters/TrackParameters.h"
#include "AthenaBaseComps/AthAlgTool.h"

#include "GaudiKernel/ToolHandle.h"


namespace Trk{

class Layer;
class MaterialProperties;
class IMultipleScatteringUpdator;


class MultipleScatterUpdator : public AthAlgTool, virtual public IMaterialEffectsUpdator {

 public:

  /** Constructor with AlgTool parameters */
  MultipleScatterUpdator(const std::string&, const std::string&, const IInterface*);

  /** Virtual destructor */
  virtual ~MultipleScatterUpdator();

  /** AlgTool initialise method */
  StatusCode initialize();
  
  /** AlgTool finalise method */
  StatusCode finalize();

  /** Layer based material effects update - track parameters given by pointer */
  const TrackParameters* update( const TrackParameters* parameters,
																 const Layer& layer,
																 PropDirection direction = anyDirection,
																 ParticleHypothesis particleHypothesis = nonInteracting,
																 MaterialUpdateMode matmode = Trk::addNoise ) const;

  /** Material properties based effects update - track parameters are given by reference */
  const TrackParameters* update( const TrackParameters&,
																 const MaterialProperties&,
																 double,
																 PropDirection direction = anyDirection,
																 ParticleHypothesis particleHypothesis = nonInteracting,
																 MaterialUpdateMode matmode = Trk::addNoise) const;

  /** User updator interface (full update for a layer):
  The parmeters are given as a pointer, they are deleted inside the update method.
  Update occurs on the place where the parameters parm are according to the specified MaterialEffectsOnTrack
  */
  const TrackParameters*      update( const TrackParameters* parm,
                                      const MaterialEffectsOnTrack&,
                                      ParticleHypothesis particle=pion,
                                      MaterialUpdateMode matupmode=addNoise) const 
	{ (void)particle; 
		(void)matupmode;
		 return parm; }         
	
  /** Pre-Update */
  const TrackParameters* preUpdate( const TrackParameters*,
																		const Layer&,
																		PropDirection,
																		ParticleHypothesis,
																		MaterialUpdateMode ) const
	{ return 0; };

  /** Post-Update */
  const TrackParameters* postUpdate( const TrackParameters&,
                                     const Layer&,
                                     PropDirection,
                                     ParticleHypothesis,
                                     MaterialUpdateMode ) const
	{ return 0; };


 private:
  int                     m_outputlevel;                      //!< to cache current output level
  ToolHandle< IMultipleScatteringUpdator > m_msUpdator; //!< AlgoTool for MultipleScatterin effects  
 
 protected:
  static ParticleMasses   s_particleMasses;
  bool                    m_multipleScatterLogTermOn;
  static double           s_multipleScatterMainFactor;
  static double           s_multipleScatterLogFactor;
  static double           s_main_RossiGreisen;
  static double           s_log_RossiGreisen;

};

}

#endif
