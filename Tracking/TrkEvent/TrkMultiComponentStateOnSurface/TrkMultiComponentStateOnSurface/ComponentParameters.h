/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*********************************************************************************
			ComponentParameters.h  -  description
			-------------------------------------
begin                : Sunday 8th May 2005
author               : atkinson, amorley
email                : Anthony.Morley@cern.ch
decription           : Definition of component parameters for use in a mixture
		       						 of many components. In this reigme each track parameters
                       object comes with a weighting (double) attached
*********************************************************************************/

#ifndef TrkComponentParameters
#define TrkComponentParameters

#include "TrkParameters/TrackParameters.h" //typedef

namespace Trk{
    
class ComponentParameters : public std::pair<const TrackParameters*, double>{
 public:

  /** Default constructor */
  ComponentParameters();
  
  /** Constructor with pointer to track parameters and weighting double */
  ComponentParameters(const TrackParameters*, double);
  
  /** Copy constructor */
  ComponentParameters(const ComponentParameters&);
  
  /** Default assignment **/
  ComponentParameters & operator=(const ComponentParameters&) = default;

  /** Virtual destructor */
  virtual ~ComponentParameters();

  /** Clone method */
  virtual const ComponentParameters* clone() const;

};

} // end Trk namespace

#endif
