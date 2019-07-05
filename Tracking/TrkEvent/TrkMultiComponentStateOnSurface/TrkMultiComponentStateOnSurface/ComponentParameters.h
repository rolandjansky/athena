/*
 Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  ComponentParameters():
      std::pair<const Trk::TrackParameters*, double>()
  {}


  /** Constructor with pointer to track parameters and weighting double */
  ComponentParameters(const Trk::TrackParameters* trackParameters, double weight):
      std::pair<const Trk::TrackParameters*, double>(trackParameters, weight)
  {}

  /** Copy constructor */
  ComponentParameters(const Trk::ComponentParameters& componentParameters):
      std::pair<const Trk::TrackParameters*, double>(componentParameters.first, componentParameters.second)
  {}

  /** Default assignment **/
  ComponentParameters & operator=(const ComponentParameters&) = default;

  /** Destructor */
  ~ComponentParameters()
  {}

  /** Clone method */
  const Trk::ComponentParameters* clone() const
  {
    return new ComponentParameters( (this->first)->clone(), this->second );
  }

};

} // end Trk namespace

#endif


