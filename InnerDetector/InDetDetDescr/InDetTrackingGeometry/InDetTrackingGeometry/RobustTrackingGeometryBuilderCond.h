/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// RobustTrackingGeometryBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef INDETTRACKINGGEOMETRY_ROBUSTTRACKINGGEOMETRYBUILDERCOND_H
#define INDETTRACKINGGEOMETRY_ROBUSTTRACKINGGEOMETRYBUILDERCOND_H

//Trk
#include "TrkDetDescrInterfaces/IGeometryBuilderCond.h"
#include "TrkDetDescrUtils/BinningType.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"
// Athena
#include "AthenaBaseComps/AthAlgTool.h"
#include "CxxUtils/CachedUniquePtr.h"
#include "CxxUtils/checker_macros.h"
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
#define takeSmallerBigger(cSmallest, cBiggest, test) takeSamller(cSmallest, cBiggest, test); takeBigger(cSmallest, cBiggest,test)
#endif


namespace Trk {
 class TrackingGeometry;
 class ILayerBuilderCond;
 class ITrackingVolumeCreator;
 class ILayerArrayCreator;
 class IMagneticFieldTool;
 class Layer;
 class Material;
}
 
class IEnvelopeDefSvc; 
 
namespace InDet {
     

  /** @class RobustTrackingGeometryBuilderCond

      New Geometry builder that adapts to different layer setups
      
      Only a few parameters are not automated:
       - m_outwardsFraction: this defines how much you orient yourself on the next bigger layer
                             if you wrap an outer volume around an inner 0.5 would lead to a boundary fully in bewteen
                            1. at the outer boundary, 0. at the inner boundary
      
      @author Andreas.Salzburger@cern.ch
    
    */
    
  class RobustTrackingGeometryBuilderCond : public AthAlgTool, 
                                        public Trk::TrackingVolumeManipulator,
                                        virtual public Trk::IGeometryBuilderCond {
    
    
    public:
      /** Constructor */
      RobustTrackingGeometryBuilderCond(const std::string&,const std::string&,const IInterface*);
      
      /** Destructor */
      virtual ~RobustTrackingGeometryBuilderCond();
        
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();
      /** TrackingGeometry Interface methode */
      std::pair<EventIDRange, const Trk::TrackingGeometry*> trackingGeometry ATLAS_NOT_THREAD_SAFE (const EventContext& ctx, std::pair<EventIDRange, const Trk::TrackingVolume*> tVolPair) const; 

      /** The unique signature */
      Trk::GeometrySignature geometrySignature() const { return Trk::ID; }
      
    private:
        
      /** Private method, creates and packs a triple containing of NegEndcap-Barrel-PosEndcap layers */
      const Trk::TrackingVolume* packVolumeTriple ATLAS_NOT_THREAD_SAFE
                                                 (const std::vector<const Trk::Layer*>& negLayers,
                                                  const std::vector<const Trk::Layer*>& centralLayers,
                                                  const std::vector<const Trk::Layer*>& posLayers,
                                                  double rMin, double rMax,
                                                  double zMin, double zPosCentral,
                                                  const std::string& baseName="UndefinedVolume",
                                                  int colorCode = 21,
                                                  Trk::BinningType bintype=Trk::arbitrary) const;      
      
      /** Private method, creates and packs a triple containing of NegEndcap-Barrel-PosEndcap volumes */
      const Trk::TrackingVolume* packVolumeTriple(const std::vector<const Trk::TrackingVolume*>& negVolumes,
                                                  const std::vector<const Trk::TrackingVolume*>& centralVolumes,
                                                  const std::vector<const Trk::TrackingVolume*>& posVolumes,
                                                  const std::string& baseName="UndefinedVolume") const;

      // helper tools for the geometry building
      ToolHandle<Trk::ILayerBuilderCond>             m_beamPipeBuilder;          //!< BeamPipe builder (is different from layers)
      ToolHandleArray<Trk::ILayerBuilderCond>        m_layerBuilders;            //!< Helper Tools for the Layer creation   
      ToolHandle<Trk::ITrackingVolumeCreator>        m_trackingVolumeCreator;    //!< Helper Tool to create TrackingVolumes
      ToolHandle<Trk::ILayerArrayCreator>            m_layerArrayCreator;        //!< Helper Tool to create BinnedArrays

      // configurations for the layer builders
      std::vector<int>                               m_layerBinningType;         //!< binning type for the provided layers      
      std::vector<int>                               m_colorCodesConfig;         //!< Color codes    

      // enclosing endcap/cylinder layer 
      ServiceHandle<IEnvelopeDefSvc>                 m_enclosingEnvelopeSvc;                //!< the service to provide the ID envelope size
      std::vector<double>                            m_enclosingCylinderRadius;             //!< the cylinder layer inside the enclosing volume
      std::vector<double>                            m_enclosingDiscPositionZ;              //!< the disc position inside the enclosing volume
      
      double                                         m_layerEnvelopeCover;       //!< innermost - outermost 
      bool                                           m_buildBoundaryLayers;      //!< create boundary layers 
      bool                                           m_replaceJointBoundaries;   //!< run with replacement of all joint boundaries 
      
      // material configuration
      CxxUtils::CachedUniquePtrT<Trk::Material>      m_materialProperties;       //!< overal material properties of the ID
      // outer envelope        
      double                                         m_outwardsFraction;         //!< defines how much you orient yourself in an outwards way (see above)                                    
      // robust layer indexing                                                   
      bool                                           m_indexStaticLayers;        //!< forces robust indexing for layers
      // naming schema                                                           
      std::string                                    m_namespace;                //!< identificaton namespace 
      // ID container                                                            
      std::string                                    m_exitVolume;                //!< the final ID container             
      bool                                           m_isSLHC;                   //!< changes volume boundary calculation for SLHC layouts      
  };

} // end of namespace

#endif //INDETTRACKINGGEOMETRY_ROBUSTTRACKINGGEOMETRYBUILDERCOND_H


