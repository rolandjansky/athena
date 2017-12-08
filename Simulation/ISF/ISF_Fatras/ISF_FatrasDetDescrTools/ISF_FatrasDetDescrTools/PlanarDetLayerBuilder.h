/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// PlanarDetLayerBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////
 
#ifndef ISF_DETDESCRTOOLS_PLANARDETLAYERBUILDER_H
#define ISF_DETDESCRTOOLS_PLANARDETLAYERBUILDER_H

// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
// Amg
#include "GeoPrimitives/GeoPrimitives.h"
// Trk
#include "TrkDetDescrInterfaces/ILayerBuilder.h"
#include "TrkDetDescrUtils/SharedObject.h"
#include "TrkSurfaces/SurfaceCollection.h"

// ISF
#include "ISF_FatrasDetDescrModel/IdHashDetElementCollection.h"

// STL
#include <vector>
#include <string>
#include "GaudiKernel/ITHistSvc.h"

class TTree;
class TFile;
  
class PixelID;
class SCT_ID;

namespace Trk {
  class Surface;
  class CylinderLayer;
  class DiscLayer;
  class PlaneLayer;
  class LayerMaterialProperties;
  typedef std::pair< SharedObject<const Surface>, Amg::Vector3D > SurfaceOrderPosition;
}

namespace InDetDD {
  class SiDetectorManager;
}

namespace iFatras {

  class PlanarDetElement;
  class IInputLayerMaterialProvider;
  
  /** @class IPlanarDetLayerBuilder
      
      The PlanarDetLayerBuilder parses the senstive detector elments and orders them onto a Layer surface.
      It inherits from IAlgTool. 
      
      @author Noemi.Calace@cern.ch
  */
  class PlanarDetLayerBuilder : virtual public AthAlgTool, virtual public Trk::ILayerBuilder {
    
  public:
    /** AlgTool style constructor */
    PlanarDetLayerBuilder(const std::string&,const std::string&,const IInterface*);
    
    /** Destructor*/
    virtual ~PlanarDetLayerBuilder();
    
    /** AlgTool initialize and finalize methods */
    StatusCode initialize();

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
    /**  Private methods to create the detector elements*/
    const std::vector<const iFatras::PlanarDetElement*>* CreateElements(unsigned int layerCounter, bool isCylinder = true) const;
    iFatras::PlanarDetElement* CylinderDetElement(unsigned int layerCounter, unsigned int Phi, unsigned int Z) const;
    std::vector< const Trk::CylinderLayer* >* dressCylinderLayers(const std::vector< const Trk::CylinderLayer* >& detectionLayers ) const;

    iFatras::PlanarDetElement* DiscDetElement(unsigned int discCounter, unsigned int iring, unsigned int Phi) const;
    
    const Trk::LayerMaterialProperties* barrelLayerMaterial(double r, double hz, bool isActive = true) const;  //!< helper method to construct barrel material
    const Trk::LayerMaterialProperties* endcapLayerMaterial(double rMin, double rMax, bool isActive = true) const; //!< helper method to construct endcap material
    
    void registerSurfacesToLayer(const std::vector<const Trk::Surface*>& surfaces, const Trk::Layer& layer) const; //!< layer association

    void layerToSurfaceCollection(Trk::Layer *layer) const;
    
    void computeRadiusMinMax(Amg::Transform3D trf, iFatras::PlanarDetElement* moduleTmp, double &rMin, double &rMax) const;
    
    bool                                           m_pixelCase;                      //!< flag for pixel/sct
    const InDetDD::SiDetectorManager*              m_siMgr;                          //!< the Si Detector Manager
    std::string                                    m_siMgrLocation;                  //!< the location of the Pixel Manager
    const PixelID*                                 m_pixIdHelper;                    //!< pixel Id Helper 
    const SCT_ID*                                  m_sctIdHelper;                    //!< sct Id Helper

    iFatras::IdHashDetElementCollection*           m_detElementMap;
    std::string                                    m_detElementMapName;

    bool                                           m_setLayerAssociation;  //!< Set Layer Association
    std::string                                    m_identification;       //!< string identification  
    bool                                           m_moreSurfacesToOverlapDescriptor;

    // barrel layer section                        
    size_t                                                         m_barrelLayerBinsZ;               //!< Barrel bins for the material in z 
    size_t                                                         m_barrelLayerBinsPhi;             //!< Barrel bins for the material in phi
    size_t                                                         m_barrelLayers;
    size_t                                                         m_layerOffset; // for identifier you need to know how many layers of the same type you already have
    bool                                                           m_layerEquidistantBinning;
    mutable std::vector< std::vector<float> >                      m_layerZboundaries;
    std::vector<size_t>                                            m_layerZsectors;
    std::vector<size_t>                                            m_layerPhiSectors;
    std::vector<double>                                            m_layerTilt;
    std::vector<double>                                            m_layerTiltedTilt;
    std::vector<double>                                            m_layerMinPhi;
    std::vector<double>                                            m_layerMaxPhi;
    std::vector<double>                                            m_layerPhiOffset;
    std::vector<double>                                            m_layerTiltedPhiOffset;
    std::vector<double>                                            m_layerMinZ;
    std::vector<double>                                            m_layerMaxZ;
    std::vector<double>                                            m_layerRadius;
    std::vector<double>                                            m_layerTiltedRadius;
    mutable std::vector<double>                                    m_layerRmin;
    mutable std::vector<double>                                    m_layerRmax;
    std::vector<double>                                            m_layerRseparation; // distance in R between elements on the same stave (this means same phi and different eta)
    std::vector<double>                                            m_layerTiltedRseparation;
    std::vector<double>                                            m_layerZseparation; // distance in Z (space between consecutive elements)
    std::vector<double>                                            m_layerTiltedZseparation; // distance in Z (space between consecutive elements on the same ring) for tilted elements
    std::vector<double>                                            m_layerTiltedZgroups;
    std::vector< std::vector<double> >                             m_layerTiltedZposition;
    std::vector< double >                                          m_layerBarrelZposition;
    std::vector<double>                                            m_layerThickness;
    std::vector<double>                                            m_layerLengthY;
    std::vector<double>                                            m_layerTiltedLengthY;
    std::vector<double>                                            m_layerLengthXmin;
    std::vector<double>                                            m_layerLengthXmax;
    std::vector<double>                                            m_layerTiltedLengthXmin;
    std::vector<double>                                            m_layerTiltedLengthXmax;
    std::vector<double>                                            m_layerPitchX;
    std::vector<double>                                            m_layerPitchY;
    std::vector<double>                                            m_layerRotation;
    std::vector< std::vector<double> >                             m_layerMultipleRotation;
    bool                                                           m_layerSCTlike;
    //bool                                                           m_layerInvertOtherSurface;
    std::vector<double>                                            m_layerStereo;
    std::vector<double>                                            m_layerTiltedStereo;
    std::vector<double>                                            m_layerStereoSeparation; 
    std::vector<double>                                            m_layerTiltedStereoZSeparation;
    std::vector<double>                                            m_layerAdditionalLayerR;
    mutable std::vector<const std::vector<const PlanarDetElement*>* >      m_barrelElements;
    mutable std::vector< std::vector<Trk::SurfaceOrderPosition> >          m_layerSurfaces;
    mutable PlanarDetElement*                                              m_firstLayerElement;
    mutable PlanarDetElement*                                              m_previousLayerElement; 
    mutable std::vector<PlanarDetElement*>*                                m_previousSliceElements;

    // endcap layer section                                                           
    size_t                                                         m_endcapLayerBinsR;               //!< Endcap bins for the material in r
    size_t                                                         m_endcapLayerBinsPhi;             //!< Endcap bins for the material in phi
    size_t                                                         m_endcapDiscs;
    size_t                                                         m_discOffset; // for identifier you need to know how many discs of the same type you already have
    bool                                                           m_useTrapezoidalBounds;
    std::vector< std::vector<double> >                             m_discPhiSectors; // need to be std::vector< std::vector<double> > because of the parser
    std::vector<double>                                            m_discZpos;
    std::vector<double>                                            m_discThickness;
    std::vector<double>                                            m_discRmin;
    std::vector<double>                                            m_discRmax;
    std::vector< std::vector<double> >                             m_discMinPhi;
    std::vector< std::vector<double> >                             m_discMaxPhi;  
    std::vector< std::vector<double> >                             m_discRingMinR;
    std::vector< std::vector<double> >                             m_discRingMaxR; 
    std::vector< std::vector<double> >                             m_discLengthY; 
    std::vector< std::vector<double> >                             m_discLengthXmin; 
    std::vector< std::vector<double> >                             m_discLengthXmax; 
    std::vector< std::vector<double> >                             m_discPitchX;
    std::vector< std::vector<double> >                             m_discPitchY;
    std::vector< std::vector<double> >                             m_discSeparation; //separation between elements on the same ring in the same disk
    std::vector< std::vector<double> >                             m_ringDisplacement; //displacement of one ring wrt middle position
    bool                                                           m_discSCTlike;
    std::vector< std::vector<double> >                             m_discStereo;
    std::vector< std::vector<double> >                             m_discStereoSeparation; 
    std::vector<double>                                            m_discAdditionalLayerPosZ;
    mutable std::vector<const std::vector<const PlanarDetElement*>* >          m_endcapElements;
    mutable std::vector< std::vector<Trk::SurfaceOrderPosition> >          m_discSurfaces;
    mutable PlanarDetElement*                                              m_firstDiscElement;
    mutable PlanarDetElement*                                              m_previousDiscElement;  

    ToolHandle<IInputLayerMaterialProvider>        m_inputLayerMaterialProvider;
    Trk::LayerMaterialProperties *                 m_layerMaterial;
    
    bool                                           m_customMaterial;                  //!< is there custom material assigned ?
    double                                         m_customMaterialThickness;         //!< give a custom material : thickness
    double                                         m_customMaterialX0;                //!< give a custom material : radiation length
    double                                         m_customMaterialL0;                //!< give a custom material : interaction length
    double                                         m_customMaterialA;                 //!< give a custom material : average A
    double                                         m_customMaterialZ;                 //!< give a custom material : average Z
    double                                         m_customMaterialRho;               //!< give a custom material : average density
       
    double                                         m_barrelEnvelope;                 //!< envelope around rMin/rMax
    double                                         m_endcapEnvelope;                 //!< envelope around zMin/zMax
    
    // For Debugging
    bool                m_checkGeo;
    ITHistSvc*          m_thistSvc;
    TFile*              m_outputFile;  //!< the root file 
    TTree*              m_currentTree; //!< the tree to store information from pixel and SCT 
    mutable double      m_x_local;
    mutable double      m_y_local;
    mutable double      m_x_global;
    mutable double      m_y_global;
    mutable double      m_z_global;

    mutable SurfaceCollection*   m_surfcoll;
    
  };
  
  inline const std::vector< const Trk::PlaneLayer* >* PlanarDetLayerBuilder::planarLayers() const
  { return 0; }

  inline const std::string& PlanarDetLayerBuilder::identification() const
 { return m_identification; }

   
} // end of namespace


#endif // ISF_DETDESCRTOOLS_PLANARDETLAYERBUILDER_H
