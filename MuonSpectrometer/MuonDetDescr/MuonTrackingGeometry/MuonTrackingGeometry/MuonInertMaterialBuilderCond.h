/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonInertMaterialBuilderCond.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDERCOND_H
#define MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDERCOND_H

//Trk
#include "TrkGeometry/MaterialProperties.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"
#include "TrkGeometry/TrackingVolumeManipulator.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

#include "GeoModelKernel/GeoVPhysVol.h"
//mw
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"

// CondData
#include "MuonReadoutGeometry/MuonDetectorManager.h"

namespace Trk {
 class TrackingGeometry;
 class TrackingVolume;
 class Volume;
 class Layer;
 class ITrackingVolumeBuilder;
 class ITrackingVolumeArrayCreator;
 class ILayerBuilder;
 class ILayerArrayCreator;

//mw
 class Material;

}

namespace MuonGM {
  class MuonStation;
}
 
namespace Muon {
        
  /** @class MuonInertMaterialBuilderCond
  
      The Muon::MuonInertMaterialBuilderCond retrieves muon stations from Muon Geometry Tree
      
      by Sarka.Todorova@cern.ch, Marcin.Wolter@cern.ch
    */
    
  class MuonInertMaterialBuilderCond : public AthAlgTool,
                                       public Trk::TrackingVolumeManipulator {
  public:
      /** Constructor */
      MuonInertMaterialBuilderCond(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      virtual ~MuonInertMaterialBuilderCond() = default;
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** Method returning cloned and positioned material objects */
      std::tuple<EventIDRange,
           std::unique_ptr<const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume> > >,
           std::unique_ptr<std::vector<std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> > > > >
      buildDetachedTrackingVolumes(const EventContext& ctx, bool blend=false) const; 

    private:

      /** Method creating material object prototypes */
      std::tuple<EventIDRange,
           const std::vector<std::pair<const Trk::DetachedTrackingVolume*,std::vector<Amg::Transform3D> > >*,
           std::unique_ptr<std::vector<std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> > > > >
      buildDetachedTrackingVolumeTypes(const EventContext& ctx, bool blend) const; 
      /** Method extracting material objects from GeoModel tree */
      void getObjsForTranslation(const GeoVPhysVol* pv,Amg::Transform3D , std::vector<std::pair<const GeoLogVol*,std::vector<Amg::Transform3D> > >& vols ) const;
      /** Dump from GeoModel tree  */
      void printInfo(const GeoVPhysVol* pv) const;
      void printChildren(const GeoVPhysVol* pv) const;
      /** Simplification of GeoModel object + envelope */
      const Trk::TrackingVolume* simplifyShape(const Trk::TrackingVolume* tr, bool blend, std::vector<std::vector<std::pair<std::unique_ptr<const Trk::Volume>,float> > >* constituentsVector) const;
      /** Envelope creation & material fraction calculation */
      const Trk::Volume* createEnvelope(const Amg::Transform3D transf, std::vector<std::pair<const Trk::Volume*,std::pair<float,float> > > ) const;
      /** Simplification of objects, material fraction calculation */
      std::vector<std::pair<const Trk::Volume*,std::pair<float,float> > > splitComposedVolume(const Trk::Volume*,bool) const; 
      /** Scan point generation for 'hit&miss' sampling */
      Amg::Vector3D getScanPoint(const Trk::Volume* vol) const;
      /** Volume calculation */
      double calculateVolume( const Trk::Volume* envelope) const;

      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muonMgrReadKey{this, "MuonMgrReadKey", "MuonDetectorManager", "Key of input MuonDetectorMgr"};

      Gaudi::Property<bool>               m_simplify{this,"SimplifyGeometry",false};                 // switch geometry simplification on/off 
      Gaudi::Property<bool>               m_simplifyToLayers{this,"SimplifyGeometryToLayers",false}; // switch geometry simplification to layers on/off 
      Gaudi::Property<bool>               m_debugMode{this,"DebugMode",false};                       // build layers & dense volumes in parallel - double counting material !!! 
      Gaudi::Property<bool>               m_buildBT{this,"BuildBarrelToroids",true};    // build barrel toroids 
      Gaudi::Property<bool>               m_buildECT{this,"BuildEndcapToroids",true};   // build endcap toroids 
      Gaudi::Property<bool>               m_buildFeets{this,"BuildFeets",true};         // build feets 
      Gaudi::Property<int>                m_buildRails{this,"BuildRails",1};            // build rails 
      Gaudi::Property<bool>               m_buildShields{this,"BuildShields",true};     // build shieldings 
      Gaudi::Property<bool>               m_buildSupports{this,"BuildSupports",true};   // build other 
      Gaudi::Property<bool>               m_buildNSWInert{this,"BuildNSWInert",true};              // build NSW inert material
      Gaudi::Property<double>             m_blendLimit{this,"BlendLimit",3e+09};                 // volume limit for blending (except shields) 
      Trk::Material                       m_muonMaterial;               //!< the material
//mw
      std::unique_ptr<Trk::GeoMaterialConverter>          m_materialConverter;          //!< material converter
      std::unique_ptr<Trk::GeoShapeConverter>             m_geoShapeConverter;          //!< shape converter
      ServiceHandle<IRndmGenSvc>          m_rndmGenSvc{this,"randomGen","RndmGenSvc"};                 //!< Random number generator
      std::unique_ptr<Rndm::Numbers>                      m_flatDist;
      
      Gaudi::Property<bool>               m_extraMaterial{this,"AddMaterial",false};
      Gaudi::Property<float>              m_extraX0{this,"AMradLength",0.3};
      Gaudi::Property<float>              m_extraFraction{this,"AMsplit",0.5};
      Gaudi::Property<float>              m_extraPos1{this,"AMlayerPos1",13000.};
      Gaudi::Property<float>              m_extraPos2{this,"AMlayerPos2",15000.};
    };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDERCOND_H


