/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// LayerMaterialProvider.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef ISF_FATRASDETDESCRTOOLS_LAYERMATERIALPROVIDER_H
#define ISF_FATRASDETDESCRTOOLS_LAYERMATERIALPROVIDER_H

// Trk
#include "TrkDetDescrInterfaces/IGeometryProcessor.h"
// Gaudi & Athena
#include "AthenaBaseComps/AthAlgTool.h"


namespace Trk {

  class TrackingGeometry;
  class TrackingVolume;    
  class Layer;
  class Surface;
  class LayerMaterialMap;

}

namespace iFatras {

  /** @class LayerMaterialProvider

      AlgTool that retrieves the LayerMaterial from COOL and 
      loads it onto the geometry.
      In case the layer has already a material layer it is not overwritten.

      @author Noemi.Calace@cern.ch
  */

  class LayerMaterialProvider :  public AthAlgTool, virtual public Trk::IGeometryProcessor {
     
  public:
    /** Constructor */
    LayerMaterialProvider(const std::string&,const std::string&,const IInterface*);

    /** Destructor */
    virtual ~LayerMaterialProvider();

    /** Processor Action to work on TrackingGeometry& tgeo */
    virtual StatusCode process(const Trk::TrackingGeometry& tgeo);
       
    /** Processor Action to work on TrackingVolumes - the level is for the hierachy tree*/
    virtual StatusCode process(const Trk::TrackingVolume& tvol, size_t level = 0);   
       
    /** Processor Action to work on Layers */
    virtual StatusCode process(const Trk::Layer& lay, size_t level = 0);
       
    /** Processor Action to work on Surfaces */
    virtual StatusCode process(const Trk::Surface& surf, size_t level = 0);

  private:
          
    StatusCode loadMaterialMap();               //!< reatrieve the Material map from the detector store

    //!< boolean switch for assignLayerMaterial
    mutable const Trk::LayerMaterialMap*         m_layerMaterialMap;
    std::string                                  m_layerMaterialMapName;
                        
        
  };

} // end of namespace

#endif // ISF_FATRASDETDESCRTOOLS_LAYERMATERIALPROVIDER_H

