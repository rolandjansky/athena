/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// IDynamicLayerCreator.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_IDYNAMICLAYERCREATOR_H
#define TRKDETDESCRINTERFACES_IDYNAMICLAYERCREATOR_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include "TrkParameters/TrackParameters.h"

namespace Trk {

  class Layer;
  //class TrackParameters;
  class Surface;
  class IPropagator;

  /** Interface ID for IDynamicLayerCreator*/  
  static const InterfaceID IID_IDynamicLayerCreator("IDynamicLayerCreator", 1, 0);
  
  /** @class IDynamicLayerCreator
    Interface class IDynamicLayerCreator
    It inherits from IAlgTool. The actual implementation of the AlgTool depends on the SubDetector,
    more detailed information should be found there.

    Purpose of this Tool is the creation of material layers according to a given tracking procedure;
    
    @author Andreas.Salzburger@cern.ch
    */
  class IDynamicLayerCreator : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~IDynamicLayerCreator(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_IDynamicLayerCreator; }

      /** DynamicLayerCreator interface method
          Input parameters are:
            - const TrackParameters& parm  - starting track parameters
            - const Surface& sf            - destination surface
            - const IPropagator* prop      - pointer to Propagator used for eventual tracking (optional)

         */
      virtual const std::vector< std::pair<const TrackParameters*, const Layer* > >* materialLayers(const Trk::TrackParameters& parm,
                                                                const Trk::Surface& sf,
                                                                const Trk::IPropagator* prop = 0) const = 0; 
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_IDYNAMICLAYERCREATOR_H


