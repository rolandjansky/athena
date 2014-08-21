/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// SiLayerBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_SILAYERBUILDERN_H
#define INDETTRACKINGGEOMETRY_SILAYERBUILDERN_H

// Athena
#include "AthenaBaseComps/AthAlgTool.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrUtils/SharedObject.h"
// STL
#include <vector>

class PixelID;
class SCT_ID;


namespace InDetDD {
  class SiDetectorManager;
}

namespace Trk {
  class Surface;
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class LayerMaterialProperties;
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

namespace InDet {
 
  /** @class SiLayerBuilder
  
     The SiLayerBuilder parses the senstive detector elments and orders them onto a
     Layer surface.
  
     It also uses the SiNumerology to construct the BinUtility and then orders the representing
     detector surfaces on the layers.
     
     It performs an automated detector if an equidistant or non-equidistant binning
     is to be used for the barrel case.
     
     There is an option to run in split mode for multiple pixel systems of different 
     layer / endcap dimensions. In such a case, a cache is filled at the first time 
     running for the pixel system of greater dimensions and just return in the second pass.

     @author Andreas.Salzburger@cern.ch
    */
  class SiLayerBuilder : public AthAlgTool, virtual public Trk::ILayerBuilder {
    
    public:
    
      /** AlgTool style constructor */
      SiLayerBuilder(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~SiLayerBuilder();
      
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
      const Trk::LayerMaterialProperties* barrelLayerMaterial(double r, double hz) const;  //!< helper method to construct barrel material
      const Trk::LayerMaterialProperties* endcapLayerMaterial(double rMin, double rMax) const; //!< helper method to construct endcap material
        
      void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association

      bool                                           m_pixelCase;                      //!< flag for pixel/sct
                                                     
      const InDetDD::SiDetectorManager*              m_siMgr;                          //!< the Si Detector Manager
      std::string                                    m_siMgrLocation;                  //!< the location of the Pixel Manager
      const PixelID*                                 m_pixIdHelper;                    //!< pixel Id Helper 
      const SCT_ID*                                  m_sctIdHelper;                    //!< sct Id Helper
                                                     
      bool                                           m_setLayerAssociation;            //!< Set Layer Association
                                                     
      // barrel layer section                        
      std::vector<double>                            m_barrelAdditionalLayerR;         //!< Create an additional layer at these radii
      size_t                                         m_barrelLayerBinsZ;               //!< Barrel bins for the material in z 
      size_t                                         m_barrelLayerBinsPhi;             //!< Barrel bins for the material in phi
    
      bool                                           m_barrelOverlapLocCheck;          //!< check the overlap in the barrel on local parameters 
      double                                         m_barrelOverlapFractionLoc1;      //!< overlap fraction in local coordinate for the barrel (loc1)   
      double                                         m_barrelOverlapFractionLoc2;      //!< overlap fraction in local coordinate for the barrel (loc2)

      double                                         m_barrelEdbTolerance;             //!< tolerance in percent how much the bin sizes can change

                                                     
      std::vector<double>                            m_endcapAdditionalLayerPosZ;      //!< Create additional endcaps at these z positions
      size_t                                         m_endcapLayerBinsR;               //!< Barrel bins for the material in r
      size_t                                         m_endcapLayerBinsPhi;             //!< Barrel bins for the material in phi
      bool                                           m_endcapComplexRingBinning;       //!< make std::vector<R> rings, could be different for layers
      
      bool                                           m_endcapOverlapLocCheck;          //!< check the overlap in the barrel on local parameters 
      double                                         m_endcapOverlapFractionLoc1;      //!< overlap fraction in local coordinate for the barrel (loc1)   
      double                                         m_endcapOverlapFractionLoc2;      //!< overlap fraction in local coordinate for the barrel (loc2)                                             
                                                     
                                                     
      std::string                                    m_identification;                  //!< string identification  
      
      bool                                           m_splitMode;                       //!< Check for the SLHC/split case 
      mutable double                                 m_splitRadius;                     //!< Split radius for multiple pixel systems
      double                                         m_splitTolerance;                  //!< difference in layer half length to provoke the split
           
      static std::vector<const Trk::CylinderLayer*>  s_splitCylinderLayers;             //!< cached SLHC/split cylinder layers for projective layout
      static std::vector<const Trk::DiscLayer*>      s_splitDiscLayers;                 //!< cached SLHC/split disc layers for projective layout

      bool                                           m_customMaterial;                  //!< is there custom material assigned ?
      double                                         m_customMaterialThickness;         //!< give a custom material : thickness
      double                                         m_customMaterialX0;                //!< give a custom material : radiation length
      double                                         m_customMaterialL0;                //!< give a custom material : interaction length
      double                                         m_customMaterialA;                 //!< give a custom material : average A
      double                                         m_customMaterialZ;                 //!< give a custom material : average Z
      double                                         m_customMaterialRho;               //!< give a custom material : average density
                                                     
      bool                                           m_runGeometryValidation;           //!< run the validation of the geometry ( no empty bins)
      bool                                           m_robustOverlaps;                 //!< check for possible overlap modules in a more robust way
            
                      
  };

 inline const std::vector< const Trk::PlaneLayer* >* SiLayerBuilder::planarLayers() const
 { return 0; }
 
 inline const std::string& SiLayerBuilder::identification() const
 { return m_identification; }
   
} // end of namespace


#endif // INDETTRACKINGGEOMETRY_SILAYERBUILDERN_H


