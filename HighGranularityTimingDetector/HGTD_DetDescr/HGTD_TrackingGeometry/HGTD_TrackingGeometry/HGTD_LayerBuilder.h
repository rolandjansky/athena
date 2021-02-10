/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HGTD_LayerBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERN_H
#define HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERN_H

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkDetDescrUtils/ComparisonUtils.h"
// STL
#include <vector>

class HGTD_ID;
class HGTD_DetectorManager;

namespace Trk {
  class Surface;
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class LayerMaterialProperties;
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

namespace HGTDet {
 
  /** @class HGTD_LayerBuilder
  
     The HGTD_LayerBuilder parses the senstive detector elments and orders them onto a
     Disc surface; no cylindrical layers are expected. 
     This implementation is based on what done in the SiLayerBuilder, adapted to the HGTD use case.
     
     It also uses the SiNumerology to construct the BinUtility and then orders the representing
     detector surfaces on the layers.
     
     */
  
  class HGTD_LayerBuilder : public AthAlgTool, virtual public Trk::ILayerBuilder {
    
    public:
    
      /** AlgTool style constructor */
      HGTD_LayerBuilder(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~HGTD_LayerBuilder();
      
      /** AlgTool initialize method */
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
       
      /** LayerBuilder interface method - returning Barrel-like layers */
      const std::vector< const Trk::CylinderLayer* >* cylindricalLayers() const; 
      
      /** LayerBuilder interface method - returning Endcap-like layers */
      const std::vector< const Trk::DiscLayer* >*     discLayers() const; 
      
      /** LayerBuilder interface method - returning Planar-like layers */
      const std::vector< const Trk::PlaneLayer* >*    planarLayers() const; 
             
      /** Name identification */
      const std::string& identification() const;      
        
    private:
      const Trk::LayerMaterialProperties* discLayerMaterial(double rMin, double rMax) const; //!< helper method to construct HGTD material
        
      void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association
      
      void evaluateBestBinning(std::vector<Trk::SurfaceOrderPosition>& surfaces,
                               std::vector<float>& rBins, float& maxRadius,
                               std::vector<std::vector<float>>& phiBins) const;

      const HGTD_DetectorManager*           m_hgtdMgr;                        //!< the HGTD Detector Manager
      const HGTD_ID*                        m_hgtdHelper;                     //!< HGTD Id Helper
                                            
      bool                                  m_setLayerAssociation;            //!< Set Layer Association
                                            
      std::string                           m_identification;                 //!< string identification        
      
      int                                   m_rBins;                          //!< set the number of bins
      int                                   m_phiBins;                        //!< set the number of bins
      
      float                                 m_discEnvelopeR;                  //!< set disc envelope
      float                                 m_discThickness;                  //!< set disc thickness
      
      bool                                  m_runGeometryValidation;          //!< run geometry validation
                      
  };

 inline const std::vector< const Trk::CylinderLayer* >* HGTD_LayerBuilder::cylindricalLayers() const
 { return 0; }
  
 inline const std::vector< const Trk::PlaneLayer* >* HGTD_LayerBuilder::planarLayers() const
 { return 0; }
 
 inline const std::string& HGTD_LayerBuilder::identification() const
 { return m_identification; }
   
} // end of namespace


#endif // HGTD_TRACKINGGEOMETRY_HGTDLAYERBUILDERN_H


