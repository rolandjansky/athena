/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerProviderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERPROVIDERCOND_H
#define TRKDETDESCRINTERFACES_ILAYERPROVIDERCOND_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/EventIDRange.h"
#include "GaudiKernel/EventContext.h"
// STL
#include <vector>
#include <string>

namespace Trk {

  class Layer;

  /** Interface ID for ILayerProviderConds*/  
  static const InterfaceID IID_ILayerProviderCond("ILayerProviderCond", 1, 0);
  
  /** @class ILayerProviderCond
  
    Interface class ILayerProviderConds
    it feeds into the StagedGeometryBuilder
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerProviderCond : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerProviderCond(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerProviderCond; }

      /** LayerBuilder interface method - returning the layers at negative side */
      virtual std::pair<EventIDRange, const std::vector< const Layer* > > negativeLayers(const EventContext& ctx) const = 0; 
      
      /** LayerBuilder interface method - returning the central layers */
      virtual std::pair<EventIDRange, const std::vector< const Layer* > > centralLayers(const EventContext& ctx) const = 0; 
      
      /** LayerBuilder interface method - returning the layers at negative side */
      virtual std::pair<EventIDRange, const std::vector< const Layer* > > positiveLayers(const EventContext& ctx) const = 0; 

      /** Name identification */
      virtual const std::string& identification() const = 0;
             
  };

} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERPROVIDER_H


