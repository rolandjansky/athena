/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H

#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilderCond.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonTrackingGeometry/MuonStationTypeBuilder.h"
#include "TrkDetDescrInterfaces/ITrackingVolumeHelper.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "GeoModelKernel/GeoVPhysVol.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"

namespace Trk {
 class MaterialProperties;
}

namespace MuonGM {
  class MuonDetectorManager;
}
 
namespace Muon {

  
  /** @class MuonStationBuilderCond
  
      The Muon::MuonStationBuilderCond retrieves muon stations from Muon Geometry Tree
      prototypes built with help of Muon::MuonStationTypeBuilder
      
      by Sarka.Todorova@cern.ch
    */
    
  class MuonStationBuilderCond : public AthAlgTool, virtual public Trk::IDetachedTrackingVolumeBuilderCond {
  public:
      MuonStationBuilderCond(const std::string&,const std::string&,const IInterface*);
      virtual ~MuonStationBuilderCond() = default;
      StatusCode initialize();

      //FIXME: THIS IS USING const_cast TO ACCESS buildDetachedTrackingVolumeTypes(...)
      std::pair<EventIDRange, std::unique_ptr<const std::vector<std::unique_ptr<const Trk::DetachedTrackingVolume> > > > buildDetachedTrackingVolumes ATLAS_NOT_THREAD_SAFE (const EventContext& ctx, bool blend=false) const;

    private:
      ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

      const std::vector<const Trk::DetachedTrackingVolume*>* buildDetachedTrackingVolumeTypes(bool blend, const MuonGM::MuonDetectorManager* muonMgr);

      void glueComponents(const Trk::DetachedTrackingVolume* ) const;    
      void encloseLayers( const Trk::DetachedTrackingVolume* ) const; 
      void identifyLayers(const Trk::DetachedTrackingVolume*, int, int, const MuonGM::MuonDetectorManager*) const;
      void identifyPrototype(const Trk::TrackingVolume*, int, int, Amg::Transform3D, const MuonGM::MuonDetectorManager*) const;
      void getNSWStationsForTranslation(const GeoVPhysVol* pv, std::string name, Amg::Transform3D , std::vector<std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*>,std::vector<Amg::Transform3D> > >& vols, std::vector<std::string>& volNames ) const;
  
      SG::ReadCondHandleKey<MuonGM::MuonDetectorManager> m_muonMgrReadKey{this, "MuonMgrReadKey", "MuonDetectorManager", "Key of input MuonMgr"};

      ToolHandle<Muon::MuonStationTypeBuilder> m_muonStationTypeBuilder{this,"StationTypeBuilder","Muon::MuonStationTypeBuilder/MuonStationTypeBuilder"}; //!< Helper Tool to create TrackingVolume Arrays
      ToolHandle<Trk::ITrackingVolumeHelper> m_trackingVolumeHelper{this,"TrackingVolumeHelper","Trk::TrackingVolumeHelper/TrackingVolumeHelper"};   //!< Helper Tool to create TrackingVolumes

      Trk::Material m_muonMaterial;               //!< the material
      std::unique_ptr<Trk::GeoShapeConverter> m_geoShapeConverter;          //!< shape converter
      std::unique_ptr<Trk::GeoMaterialConverter> m_materialConverter;          //!< material converter
      Gaudi::Property<bool> m_buildBarrel{this,"BuildBarrelStations",true};
      Gaudi::Property<bool> m_buildEndcap{this,"BuildEndcapStations",true};
      Gaudi::Property<bool> m_buildCsc{this,"BuildCSCStations",true};
      Gaudi::Property<bool> m_buildTgc{this,"BuildTGCStations",true};  
    };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONSTATIONBUILDERCOND_H


