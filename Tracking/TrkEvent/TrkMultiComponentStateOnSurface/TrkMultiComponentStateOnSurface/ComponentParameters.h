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
#include "TrkParameters/TrackParameters.h"


/*
 * Note that this class does not own/delete the Trk::TrackParameters ptr.
 * Deletion happens only if you push it in a MultiComponentState. In which
 * case it takes ownership
 * This needs some care and can be target of furhter refactoring ....
 */

namespace Trk{

typedef std::pair<std::unique_ptr<Trk::TrackParameters>, double> SimpleComponentParameters;
typedef std::vector<SimpleComponentParameters> SimpleMultiComponentState;

class ComponentParameters : public std::pair<const TrackParameters*, double>{
public:

  /** Default constructor */
  ComponentParameters() = default;
  /** Constructor with pointer to track parameters and weighting double */
  ComponentParameters(const Trk::TrackParameters* trackParameters, double weight):
      std::pair<const Trk::TrackParameters*, double>(trackParameters, weight)
  {}
  /** Default constructor */
  ComponentParameters(const Trk::ComponentParameters& componentParameters) = default;
  /** Default assignment **/
  ComponentParameters & operator=(const ComponentParameters&) = default;

  /** Destructor */
  ~ComponentParameters() = default;

  /** Clone method */
  const Trk::ComponentParameters clone() const
  {
    return ComponentParameters( (this->first)->clone(), this->second );
  }

};

} // end Trk namespace

#endif
