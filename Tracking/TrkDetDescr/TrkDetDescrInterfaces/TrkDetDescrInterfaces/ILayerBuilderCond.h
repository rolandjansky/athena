/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// ILayerBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRINTERFACES_ILAYERBUILDERCOND_H
#define TRKDETDESCRINTERFACES_ILAYERBUILDERCOND_H

// Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "TrkSurfaces/Surface.h"
// STL
#include <vector>
#include <string>

namespace Trk {

  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class Layer;

  /** Interface ID for ILayerBuilderCond*/  
  static const InterfaceID IID_ILayerBuilderCond("ILayerBuilderCond", 1, 0);
  
  /** @class ILayerBuilderCond
    Interface class ILayerBuilderConds
    It inherits from IAlgTool. The actual implementation of the AlgTool depends on the SubDetector,
    more detailed information should be found there.
    
    @author Andreas.Salzburger@cern.ch
    */
  class ILayerBuilderCond : virtual public IAlgTool {
    
    public:
      /**Virtual destructor*/
      virtual ~ILayerBuilderCond(){}
      
      /** AlgTool and IAlgTool interface methods */
      static const InterfaceID& interfaceID() { return IID_ILayerBuilderCond; }

      /** LayerBuilder interface method - returning Barrel-like layers */
      virtual std::pair<EventIDRange, const std::vector< const CylinderLayer* >*> cylindricalLayers(const EventContext& ctx) const = 0; 
      
      /** LayerBuilder interface method - returning Endcap-like layers */
      virtual std::pair<EventIDRange, const std::vector< const DiscLayer* >*>     discLayers(const EventContext& ctx) const = 0; 
      
      /** LayerBuilder interface method - returning Planar-like layers */
      virtual std::pair<EventIDRange, const std::vector< const PlaneLayer* >*>    planarLayers(const EventContext& ctx) const = 0;

      /** Name identification */
      virtual const std::string& identification() const = 0;
      
      /** Validation Action:
          Can be implemented optionally, outside access to internal validation steps */
      virtual void validationAction() const {}

    protected:
      /** Protected method to register the Layer to the Surface */
      void associateLayer(const Layer& lay, const Surface& sf) const { sf.associateLayer(lay); }
       
  };


} // end of namespace


#endif // TRKDETDESCRINTERFACES_ILAYERBUILDERCOND_H


