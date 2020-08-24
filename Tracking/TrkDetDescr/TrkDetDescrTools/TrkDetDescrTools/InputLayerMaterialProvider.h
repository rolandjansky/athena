/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// InputLayerMaterialProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef TRKDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H
#define TRKDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H

// Trk
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
#include "TrkGeometry/MaterialProperties.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

    class TrackingGeometry;
    class TrackingVolume;    
    class Layer;
    class Surface;
    class LayerMaterialMap;


    /** @class InputLayerMaterialProvider

      AlgTool that assigns LayerMaterial as specified to the TrackingGeometry

      @author Andreas.Salzburger@cern.ch
     */

    class InputLayerMaterialProvider :  public AthAlgTool, virtual public IGeometryProcessor {
     
      public:
        /** Constructor */
        InputLayerMaterialProvider(const std::string&,const std::string&,const IInterface*);

        /** Destructor */
        virtual ~InputLayerMaterialProvider();

        /** Framework initialize() */
        virtual StatusCode initialize();

        /** Processor Action to work on TrackingGeometry& tgeo */
        virtual StatusCode process(const TrackingGeometry& tgeo) const;
       
        /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
        virtual StatusCode process(const TrackingVolume& tvol, size_t level = 0) const;
       
        /** Processor Action to work on Layers */
        virtual StatusCode process(const Layer& lay, size_t level = 0) const;
       
        /** Processor Action to work on Surfaces */
        virtual StatusCode process(const Surface& surf, size_t level = 0) const;

      private:
         bool               m_constantMaterialToAllLayers;  //!< just assign a dummy material to all layers
         float              m_constantThicknessInX0;        //!< constant material specifications
         float              m_constantThicknessInL0;        //!< constant material specifications
         float              m_constantAverageA;             //!< constant material specifications
         float              m_constantAverageZ;             //!< constant material specifications
         float              m_constantAverageRho;           //!< constant material specifications
         MaterialProperties m_constantMaterialProperties;   //!< the set together material

                        
        
    };

} // end of namespace

#endif // TRKDETDESCRTOOLS_INPUTLAYERMATERIALPROVIDER_H

