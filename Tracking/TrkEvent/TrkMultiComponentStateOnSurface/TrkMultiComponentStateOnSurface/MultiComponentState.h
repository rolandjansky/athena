/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			MultiComponentState.h  -  description
			-------------------------------------
begin                : Friday 31st December 2004
author               : atkinson, amorley
email                : Anthony.Morley@cern.ch
decription           : Basic definitions for a track state described by more
		       than one set of Track Parameters. The resulting state is
		       a mixture of components. Each component is described by
		       a ComponentParameters object which is of the type
                       std::pair< const TrackParameters*, double> 
                       The double describes the weighting of the component - 
                       or its relative importance in the mixture.			
*********************************************************************************/

#ifndef TrkMultiComponentState
#define TrkMultiComponentState

#include "TrkMultiComponentStateOnSurface/ComponentParameters.h"
#include <list>

class MsgStream;

namespace Trk{

class MultiComponentState : public std::list<ComponentParameters>{
 public:

  /** Default constructor */
  MultiComponentState();

  /** Copy constructor */
  MultiComponentState( const ComponentParameters& );

  /** Virtual destructor */
  virtual ~MultiComponentState();

  /** Clone method */
  virtual const MultiComponentState* clone() const;

  /** Clone with rescaled weight scaling factor */
  virtual const MultiComponentState* cloneWithWeightScaling( double ) const;

  /** Clone with covariance matricies scaled by a factor */
  virtual const MultiComponentState* cloneWithScaledError( double ) const;

  /** Clone with covariance matrix componants scaled by individual factors
      This will only work if there are 5 track parameters in each componant
  */
  virtual const MultiComponentState* cloneWithScaledError( double, double,
                                                           double, double, 
                                                           double ) const;

  /** Check to see if all components in the state have measured track parameters */
  virtual bool isMeasured() const;

  /** Clone state performing renormalisation of total state weighting to one */
  virtual const MultiComponentState* clonedRenormalisedState() const;

  /** Dump methods */
  virtual MsgStream&    dump( MsgStream& ) const;
  virtual std::ostream& dump( std::ostream& ) const;
  
};

/** Overload of << operator for MsgStream and std::ostream */
MsgStream& operator    << ( MsgStream&,    const MultiComponentState& );
std::ostream& operator << ( std::ostream&, const MultiComponentState& );

} // end Trk namespace

#endif
