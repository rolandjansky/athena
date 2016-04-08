/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonInertMaterialBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDER_H
#define MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDER_H

//Trk
#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilder.h"
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
#
#include "GeoModelKernel/GeoVPhysVol.h"
//mw
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkVolumes/TrapezoidVolumeBounds.h"
#include "TrkVolumes/CuboidVolumeBounds.h"
#include "TrkVolumes/BevelledCylinderVolumeBounds.h"
#include "TrkVolumes/CylinderVolumeBounds.h"

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
  class MuonDetectorManager;
  class MuonStation;
}
 
namespace Muon {
        
  /** @class MuonInertMaterialBuilder
  
      The Muon::MuonInertMaterialBuilder retrieves muon stations from Muon Geometry Tree
      
      by Sarka.Todorova@cern.ch, Marcin.Wolter@cern.ch
    */
    
  class MuonInertMaterialBuilder : public AthAlgTool,
                                   public Trk::TrackingVolumeManipulator,
                             virtual public Trk::IDetachedTrackingVolumeBuilder {
  public:
      /** Constructor */
      MuonInertMaterialBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      ~MuonInertMaterialBuilder();
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      /** Method returning cloned and positioned material objects */
      const std::vector<const Trk::DetachedTrackingVolume*>* buildDetachedTrackingVolumes(bool blend=false) const; 

    private:

      /** Method creating material object prototypes */
      const std::vector<std::pair<const Trk::DetachedTrackingVolume*,std::vector<Amg::Transform3D> > >* buildDetachedTrackingVolumeTypes(bool blend) const; 
      /** Method extracting material objects from GeoModel tree */
      void getObjsForTranslation(const GeoVPhysVol* pv,Amg::Transform3D , std::vector<std::pair<const GeoLogVol*,std::vector<Amg::Transform3D> > >& vols ) const;
      /** Dump from GeoModel tree  */
      void printInfo(const GeoVPhysVol* pv) const;
      void printChildren(const GeoVPhysVol* pv) const;
      /** Simplification of GeoModel object + envelope */
      const Trk::TrackingVolume* simplifyShape(const Trk::TrackingVolume* tr, bool blend) const;
      /** Envelope creation & material fraction calculation */
      const Trk::Volume* createEnvelope(const Amg::Transform3D transf, std::vector<std::pair<const Trk::Volume*,std::pair<float,float> > > ) const;
      /** Simplification of objects, material fraction calculation */
      std::vector<std::pair<const Trk::Volume*,std::pair<float,float> > > splitComposedVolume(const Trk::Volume*,bool) const; 
      /** Scan point generation for 'hit&miss' sampling */
      Amg::Vector3D getScanPoint(const Trk::Volume* vol) const;
      /** Volume calculation */
      double calculateVolume( const Trk::Volume* envelope) const;

      const MuonGM::MuonDetectorManager*  m_muonMgr;               //!< the MuonDetectorManager
      std::string                         m_muonMgrLocation;       //!< the location of the Muon Manager
      bool                                m_simplify;              // switch geometry simplification on/off 
      bool                                m_simplifyToLayers;      // switch geometry simplification to layers on/off 
      bool                                m_debugMode;             // build layers & dense volumes in parallel - double counting material !!! 
      bool                                m_buildBT;                    // build barrel toroids 
      bool                                m_buildECT;                   // build endcap toroids 
      bool                                m_buildFeets;                 // build feets 
      int                                 m_buildRails;                 // build rails 
      bool                                m_buildShields;               // build shieldings 
      bool                                m_buildSupports;              // build other 
      double                              m_blendLimit;                 // volume limit for blending (except shields) 
      mutable Trk::Material               m_muonMaterial;               //!< the material
//mw
      Trk::GeoMaterialConverter*           m_materialConverter;          //!< material converter
      Trk::GeoShapeConverter*              m_geoShapeConverter;          //!< shape converter
      ServiceHandle<IRndmGenSvc>           m_rndmGenSvc;                 //!< Random number generator
      Rndm::Numbers*                       m_flatDist;
      
      mutable std::vector<Trk::Volume*>    m_garbage;      
      mutable std::vector<std::vector<std::pair<const Trk::Volume*,float> >* >   m_constituents;

      bool                                 m_extraMaterial;
      float                                m_extraX0;
      float                                m_extraFraction;
      float                                m_extraPos1;
      float                                m_extraPos2;
    };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONINERTMATERIALBUILDER_H


