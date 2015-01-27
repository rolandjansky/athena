/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERPROVIDER_H
#define TRKDETDESCRINTERFACES_ILAYERPROVIDER_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
// STL
#include <vector>
#include <string>

namespace Trk {

  class Layer;

  /** Interface ID for ILayerProviders*/  
  static const InterfaceID IID_ILayerProvider("ILayerProvider", 1, 0);
  
  /** @class ILayerProvider
  
    Interface class ILayerProviders
    it feeds into the StagedGeometryBuilder
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerProvider : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerProvider(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerProvider; }

      /** LayerBuilder interface method - returning the layers at negative side */
      virtual const std::vector< const Layer* > negativeLayers() const = 0; 
      
      /** LayerBuilder interface method - returning the central layers */
      virtual const std::vector< const Layer* > centralLayers() const = 0; 
      
      /** LayerBuilder interface method - returning the layers at negative side */
      virtual const std::vector< const Layer* > positiveLayers() const = 0; 

      /** Name identification */
      virtual const std::string& identification() const = 0;
             
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERPROVIDER_H


