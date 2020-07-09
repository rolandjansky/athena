/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// StagedTrackingGeometryBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_STAGEDTRACKINGGEOMETRYBUILDER_H
#define INDETTRACKINGGEOMETRY_STAGEDTRACKINGGEOMETRYBUILDER_H

//Trk
#include "TrkDetDescrInterfaces/IGeometryBuilder.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"
// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/CachedUniquePtr.h"
// Gaudi
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/ServiceHandle.h"
// STL
#include <vector>
#include <string>

#ifndef TRKDETDESCR_TAKESMALLERBIGGER
#define TRKDETDESCR_TAKESMALLERBIGGER
#define takeSmaller(current,test) current = current < test ? current : test
#define takeBigger(current,test)  current = current > test ? current : test
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSmaller(cSmallest, test); takeBigger(cBiggest, test)
#endif


namespace Trk {
 class TrackingGeometry;
 class ILayerProvider;
 class ITrackingVolumeCreator;
 class ILayerArrayCreator;
 class IMagneticFieldTool;
 class Layer;
 class Material;
}
 
class IEnvelopeDefSvc; 
 
namespace InDet {
     
  /** @struct LayerSetup 
       - helps understanding how to pack the layers into a volume compound
   */
  struct LayerSetup {

    // the layer cache
    std::vector<const Trk::Layer*> negativeLayers;
    std::vector<const Trk::Layer*> centralLayers;
    std::vector<const Trk::Layer*> positiveLayers;
        
    // center information  
    double minRadiusCenter;
    double maxRadiusCenter;  
    double zExtendCenter;
    int    binningCenter;
    
    // endcap information
    bool buildEndcap;
    double minRadiusEndcap;
    double maxRadiusEndcap;  
    double minZextendEndcap;
    double maxZextendEndcap;
    int binningEndcap;
    
    // full setup information
    double zSector;
    double rMin;
    double rMax;
    double zMax;
    
    std::string identification;
    int         colorCode;
    
    LayerSetup(const std::string& idName,
               int cCode,
               const std::vector<const Trk::Layer*>& negLayers,
               const std::vector<const Trk::Layer*>& cenLayers,
               const std::vector<const Trk::Layer*>& posLayers,
               double minRc, double maxRc, double zC, int binC,
               bool bec=false, double minRe=0., double maxRe=0., double zMinE=0., double zMaxE=0., int binE = 0) :
      negativeLayers(negLayers),
      centralLayers(cenLayers),
      positiveLayers(posLayers),
      minRadiusCenter(minRc),
      maxRadiusCenter(maxRc),
      zExtendCenter(zC),
      binningCenter(binC),
      buildEndcap(bec),            
      minRadiusEndcap(minRe),   
      maxRadiusEndcap(maxRe),
      minZextendEndcap(zMinE),
      maxZextendEndcap(zMaxE),
      binningEndcap(binE),
      identification(idName),
      colorCode(cCode)
    {
        rMin     = minRadiusCenter < minRadiusEndcap ? minRadiusCenter : minRadiusEndcap;
        rMax     = maxRadiusCenter > maxRadiusEndcap ? maxRadiusCenter : maxRadiusEndcap;
        zMax     = zExtendCenter > maxZextendEndcap ? zExtendCenter : maxZextendEndcap;
        zSector  = buildEndcap ? 0.5*(zExtendCenter+minZextendEndcap) : zExtendCenter;
        
    }
             
  };


  /** @class StagedTrackingGeometryBuilder

      New Geometry builder that adapts to different layer setups
      
      Only a few parameters are not automated:
       - m_outwardsFraction: this defines how much you orient yourself on the next bigger layer
                             if you wrap an outer volume around an inner 0.5 would lead to a boundary fully in bewteen
                            1. at the outer boundary, 0. at the inner boundary
      
      @author Andreas.Salzburger@cern.ch
    
    */
    
  class StagedTrackingGeometryBuilder : public AthAlgTool, 
                                        public Trk::TrackingVolumeManipulator,
                                        virtual public Trk::IGeometryBuilder {
    
    
    public:
      /** Constructor */
      StagedTrackingGeometryBuilder(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~StagedTrackingGeometryBuilder();
        
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** TrackingGeometry Interface methode */
      const Trk::TrackingGeometry* trackingGeometry(const Trk::TrackingVolume* tvol = 0) const; 

      /** The unique signature */
      Trk::GeometrySignature geometrySignature() const { return Trk::ID; }
      
    private:
      /** Private helper method, estimates the overal dimensions */
      LayerSetup estimateLayerSetup(const std::string& idName, size_t ils,
                                    const std::vector<const Trk::Layer*>& negLayers,
                                    const std::vector<const Trk::Layer*>& centralLayers,
                                    const std::vector<const Trk::Layer*>& posLayers,
                                    double maxR, double maxZ) const;
                                    
      /** Private helper method to estimate the layer dimensions */
      void estimateLayerDimensions(const std::vector<const Trk::Layer*>& layers,
                                   double& rMin, double& rMax, double& zMin, double& zMax) const;
                                   
      /** Private helper method to check if a sector is compatible with the cache */
      bool setupFitsCache(LayerSetup& layerSetup, std::vector<InDet::LayerSetup>& layerSetupCache) const;
                                   
                                   
      /** Private helper method to flush the cache into the id volumes - return volume is the one to be provided */
      const Trk::TrackingVolume* createFlushVolume(std::vector<InDet::LayerSetup>& layerSetupCache,
                                                   double innerRadius, double& outerRadius, double extendZ) const;                                                         
        
      /** Private helper method, creates a TrackingVolume - and checks if configured - for Ring Layout 
            - in case a ring layout is given, it creates the corresponding sub-volumes and updates the radius                                       
            */
      const Trk::TrackingVolume* createTrackingVolume(const std::vector<const Trk::Layer*>& layers, 
                                                      double innerRadius, double& outerRadius,
                                                      double zMin, double zMax,
                                                      const std::string& volumeName,
                                                      Trk::BinningType btype,
						      bool doAdjustOuterRadius = true) const;                                                  
        
      /** Private helper method, creates and packs a triple containing of NegEndcap-Barrel-PosEndcap layers
          - in case of a ring layout the subvolumes are created and the rMax is adapted                                             
         */
      const Trk::TrackingVolume* packVolumeTriple(const LayerSetup& layerSetup,
                                                  double rMin, double& rMax,
                                                  double zMin, double zPosCentral) const;      
      
      /** Private helper method, creates and packs a triple containing of NegEndcap-Barrel-PosEndcap volumes */
      const Trk::TrackingVolume* packVolumeTriple(const std::vector<const Trk::TrackingVolume*>& negVolumes,
                                                  const std::vector<const Trk::TrackingVolume*>& centralVolumes,
                                                  const std::vector<const Trk::TrackingVolume*>& posVolumes,
                                                  const std::string& baseName="UndefinedVolume") const;
                                                  
      /** Private helper method for detection of Ring layout */
      bool ringLayout(const std::vector<const Trk::Layer*>& layers, std::vector<double>& rmins, std::vector<double>& rmaxs) const;                                              

      /** helper method needed for the Ring layout */
      void checkForInsert(std::vector<double>& radii, double radius) const;

      // helper tools for the geometry building
      ToolHandleArray<Trk::ILayerProvider>           m_layerProviders;          //!< Helper Tools for the Layer creation, includes beam pipe builder   
      ToolHandle<Trk::ITrackingVolumeCreator>        m_trackingVolumeCreator;   //!< Helper Tool to create TrackingVolumes
      ToolHandle<Trk::ILayerArrayCreator>            m_layerArrayCreator;       //!< Helper Tool to create BinnedArrays

      // configurations for the layer builders
      std::vector<int>                               m_layerBinningTypeCenter;  //!< binning type for the provided layers      
      std::vector<int>                               m_layerBinningTypeEndcap;  //!< binning type for the provided layers      
      std::vector<int>                               m_colorCodesConfig;        //!< Color codes    

      // enclosing endcap/cylinder layer 
      ServiceHandle<IEnvelopeDefSvc>                 m_enclosingEnvelopeSvc;     //!< the service to provide the ID envelope size
      std::vector<double>                            m_enclosingCylinderRadius;  //!< the cylinder layer inside the enclosing volume
      std::vector<double>                            m_enclosingDiscPositionZ;   //!< the disc position inside the enclosing volume
      
      double                                         m_layerEnvelopeCover;       //!< innermost - outermost 
      bool                                           m_buildBoundaryLayers;      //!< create boundary layers 
      bool                                           m_replaceJointBoundaries;   //!< run with replacement of all joint boundaries 
      
      // material configuration
      CxxUtils::CachedUniquePtrT<Trk::Material>      m_materialProperties;       //!< overal material properties of the ID
                    
      // robust layer indexing                                                   
      bool                                           m_indexStaticLayers;        //!< forces robust indexing for layers
      
      // check for endcap ring layout
      bool                                           m_checkForRingLayout;        //!< this is to check for the endcap ring layout
      double                                         m_ringTolerance;            //!< the ring tolerance 
      
      // naming schema                                                           
      std::string                                    m_namespace;                //!< identificaton namespace 
      // ID container                                                            
      std::string                                    m_exitVolume;                //!< the final ID container             
  };

  inline void StagedTrackingGeometryBuilder::checkForInsert(std::vector<double>& radii, double radius) const {
      bool exists = false;
      // loop and check
      for (auto& checkr : radii) {
          if ( (checkr-radius)*(checkr-radius) < m_ringTolerance*m_ringTolerance ){
              exists = true; break;
          }
      }
      // insert 
      if (!exists) radii.push_back(radius);
      // re-sort
      std::sort(radii.begin(),radii.end());   
  }


} // end of namespace

#endif //INDETTRACKINGGEOMETRY_STAGEDTRACKINGGEOMETRYBUILDER_H


