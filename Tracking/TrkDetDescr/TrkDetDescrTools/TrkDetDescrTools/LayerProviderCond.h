/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProviderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H
#define TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H

// Trk
#include "TrkDetDescrInterfaces/ILayerProviderCond.h"
// Gaudi & Athena
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

    class Layer;
    class ILayerBuilderCond;


    /** @class LayerProviderCond

      Wrapper around an ILayerBuilderCond to feed into the StagedGeometryBuilder

      @author Andreas.Salzburger@cern.ch
     */

    class LayerProviderCond :  public AthAlgTool, virtual public ILayerProviderCond {
     
      public:
        /** Constructor */
        LayerProviderCond(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~LayerProviderCond();
        
        /** initialize */
        StatusCode initialize();
        
        /** finalize */
        StatusCode finalize();

        /** LayerBuilder interface method - returning the layers at negative side */
        std::pair<EventIDRange, const std::vector< const Layer* > > negativeLayers(const EventContext& ctx) const; 
      
        /** LayerBuilder interface method - returning the central layers */
        std::pair<EventIDRange, const std::vector< const Layer* > > centralLayers(const EventContext& ctx) const; 
      
        /** LayerBuilder interface method - returning the layers at negative side */
        std::pair<EventIDRange, const std::vector< const Layer* > > positiveLayers(const EventContext& ctx) const; 

        /** Name identification */
        const std::string& identification() const;

      private:
        /** LayerBuilder interface method - returning the layers at negative side */
        std::pair<EventIDRange, const std::vector< const Layer* > > discLayers(const EventContext& ctx, int posneg) const;   
      
        ToolHandle<ILayerBuilderCond>           m_layerBuilder;
        mutable std::vector<const Trk::Layer*>  m_layerCache;
        
    };


} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERPROVIDERCOND_H

