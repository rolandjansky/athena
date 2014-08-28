/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IPAUprimaryVertexFinder.h, (c) ATLAS Detector software 2005
///////////////////////////////////////////////////////////////////

#ifndef PRIMARYVERTEXFINDER_IPRIMARYVERTEXFINDER_H
#define PRIMARYVERTEXFINDER_IPRIMARYVERTEXFINDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// Trk
//#include "TrkEventPrimitives/GlobalPosition.h"
//#include "TrkParameters/TrackParameters.h"

namespace Rec { class TrackParticleContainer; } 
namespace Trk { class VxCandidate ; } 

static const InterfaceID IID_IPAUprimaryVertexFinder("IPAUprimaryVertexFinder", 1, 0);

class IPAUprimaryVertexFinder : virtual public IAlgTool {
  
 public:

  // This subclass store all quantities
  class likelihoodPV
  {
  public:
    likelihoodPV();
    const Trk::VxCandidate* vertex;
    double zvertex;
    double likelihood_ratio;
  };

  /** Virtual destructor */
  virtual ~IPAUprimaryVertexFinder(){};
  
  /** AlgTool interface methods */
  static const InterfaceID& interfaceID() { 
    return IID_IPAUprimaryVertexFinder; };
  
  virtual const std::vector<double> PrimaryVxFinder(const double z, const double sigma_z) const =0;
  virtual likelihoodPV RooPrimaryVxFinder(const double z, const double sigma_z) = 0;

};

#endif // PRIMARYVERTEXFINDER_IPRIMARYVERTEXFINDER_H
