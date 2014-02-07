/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ISurfaceBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERBUILDER_H
#define TRKDETDESCRINTERFACES_ILAYERBUILDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <string>

namespace Trk {

    class Surface;

  /** Interface ID for ISurfaceBuilders*/  
  static const InterfaceID IID_ISurfaceBuilder("ISurfaceBuilder", 1, 0);
  
  /** @class ISurfaceBuilder
  
    Interface class ISurfaceBuilders
    It inherits from IAlgTool. The actual implementation of the AlgTool depends on the SubDetector,
    more detailed information should be found there.
    
    @author Andreas.Salzburger@cern.ch
    */
  class ISurfaceBuilder : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ISurfaceBuilder(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ISurfaceBuilder; }

      /** SurfaceBuilder interface method - provide a vector of surfaces - */
      virtual const std::vector< const Surface* >* surfaces() const = 0; 
      
      /** SurfaceBuilder interface method - provice a single surface */
      virtual const Surface* surface( ) const = 0;
      
  };


} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERBUILDER_H


