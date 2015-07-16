/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_LAYERPROVIDER_H
#define TRKDETDESCRTOOLS_LAYERPROVIDER_H

// Trk
#include "TrkDetDescrInterfaces/ILayerProvider.h"
// Gaudi & Athena
#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

    class Layer;
    class ILayerBuilder;


    /** @class LayerProvider

      Wrapper around an ILayerBuilder to feed into the StagedGeometryBuilder

      @author Andreas.Salzburger@cern.ch
     */

    class LayerProvider :  public AthAlgTool, virtual public ILayerProvider {
     
      public:
        /** Constructor */
        LayerProvider(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~LayerProvider();
        
        /** initialize */
        StatusCode initialize();
        
        /** finalize */
        StatusCode finalize();

        /** LayerBuilder interface method - returning the layers at negative side */
        const std::vector< const Layer* > negativeLayers() const; 
      
        /** LayerBuilder interface method - returning the central layers */
        const std::vector< const Layer* > centralLayers() const; 
      
        /** LayerBuilder interface method - returning the layers at negative side */
        const std::vector< const Layer* > positiveLayers() const; 

        /** Name identification */
        const std::string& identification() const;

      private:
        /** LayerBuilder interface method - returning the layers at negative side */
        const std::vector< const Layer* > discLayers(int posneg) const;   
      
        ToolHandle<ILayerBuilder>               m_layerBuilder;
        mutable std::vector<const Trk::Layer*>  m_layerCache;
        
    };


} // end of namespace

#endif // TRKDETDESCRTOOLS_LAYERPROVIDER_H

