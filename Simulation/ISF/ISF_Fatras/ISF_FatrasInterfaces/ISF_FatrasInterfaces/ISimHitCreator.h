/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISimHitCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASINTERFACES_SIMHITCREATOR_H
#define ISF_FATRASINTERFACES_SIMHITCREATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "DataModel/DataVector.h"

// Trk
#include "TrkEventPrimitives/ParticleHypothesis.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkExInterfaces/ITimedExtrapolator.h"

// STL
#include <utility>

namespace Trk
{
  class Track;
  class PerigeeSurface;
  class TrackStateOnSurface;
  class TrackingGeometry;
}

namespace ISF {
    class ISFParticle;
}
  
namespace iFatras
{
  static const InterfaceID IID_ISimHitCreator("ISimHitCreator", 1, 0);

  /** 
   @class ISimHitCreator

   The sim hit creator recieves a std::vector of Trk::TrackParameters and uses them to create simulated hits.
   Hit timing comes from the extrapolator.

   @author Sarka.Todorova -at- cern.ch
   
   */
      
  class ISimHitCreator : virtual public IAlgTool
  {
  public:
    
    /** Virtual destructor */
    virtual ~ISimHitCreator() {}
    
    /** AlgTool interface methods */
    static const InterfaceID& interfaceID() { return IID_ISimHitCreator; }
    
    /** Loop over the hits, create simhits and store in hit collection */
    virtual void createHits(const ISF::ISFParticle& isp, 
                            const std::vector<Trk::HitInfo>& hits) const = 0;
    
  };
  
} // end of namespace

#endif 

