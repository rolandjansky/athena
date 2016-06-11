/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MuonStationBuilder.h, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#ifndef MUONTRACKINGGEOMETRY_MUONSTATIONBUILDER_H
#define MUONTRACKINGGEOMETRY_MUONSTATIONBUILDER_H

//Trk
#include "TrkDetDescrInterfaces/IDetachedTrackingVolumeBuilder.h"
#include "TrkGeometry/DetachedTrackingVolume.h"
#include "TrkGeometry/TrackingVolume.h"
// Gaudi
#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "GeoModelKernel/GeoVPhysVol.h"
#include "TrkDetDescrGeoModelCnv/GeoMaterialConverter.h"
#include "TrkDetDescrGeoModelCnv/GeoShapeConverter.h"

class MdtIdHelper;
class RpcIdHelper;
class CscIdHelper;
class TgcIdHelper;
class sTgcIdHelper;
class MmIdHelper;

namespace Trk {
 class TrackingGeometry;
 class TrackingVolume;
 class Volume;
 class Layer;
 class ITrackingVolumeBuilder;
 class ITrackingVolumeArrayCreator;
 class ITrackingVolumeHelper;
 class ILayerBuilder;
 class ILayerArrayCreator;
 class MaterialProperties;
}

namespace MuonGM {
  class MuonDetectorManager;
  class MuonStation;
}
 
namespace Muon {

  class MuonStationTypeBuilder;      
  
  /** @class MuonStationBuilder
  
      The Muon::MuonStationBuilder retrieves muon stations from Muon Geometry Tree
      prototypes built with help of Muon::MuonStationTypeBuilder
      
      by Sarka.Todorova@cern.ch
    */
    
  class MuonStationBuilder : public AthAlgTool,
                             virtual public Trk::IDetachedTrackingVolumeBuilder {
  public:
      /** Constructor */
      MuonStationBuilder(const std::string&,const std::string&,const IInterface*);
      /** Destructor */
      ~MuonStationBuilder();
      /** AlgTool initailize method.*/
      StatusCode initialize();
      /** AlgTool finalize method */
      StatusCode finalize();

      const std::vector<const Trk::DetachedTrackingVolume*>* buildDetachedTrackingVolumes(bool blend=false) const; 
      const std::vector<const Trk::DetachedTrackingVolume*>* buildDetachedTrackingVolumeTypes(bool blend) const; 

    private:

      void glueComponents(const Trk::DetachedTrackingVolume* ) const;    
      void encloseLayers( const Trk::DetachedTrackingVolume* ) const; 
      void identifyLayers(const Trk::DetachedTrackingVolume*, int, int ) const;
      void identifyPrototype(const Trk::TrackingVolume*, int, int, Amg::Transform3D ) const;
      void getObjsForTranslation(const GeoVPhysVol* pv, std::string name, Amg::Transform3D , std::vector<std::pair<std::pair<const GeoLogVol*,Trk::MaterialProperties*>,std::vector<Amg::Transform3D> > >& vols, std::vector<std::string>& volNames ) const;
  
      const MuonGM::MuonDetectorManager*  m_muonMgr;               //!< the MuonDetectorManager
      const MdtIdHelper*            m_mdtIdHelper;           //!< 
      const RpcIdHelper*            m_rpcIdHelper;           //!< 
      const CscIdHelper*            m_cscIdHelper;           //!< 
      const TgcIdHelper*            m_tgcIdHelper;           //!< 
      const sTgcIdHelper*           m_stgcIdHelper;           //!< 
      const MmIdHelper*             m_mmIdHelper;           //!< 
      std::string                   m_muonMgrLocation;       //!< the location of the Muon Manager

      ToolHandle<Muon::MuonStationTypeBuilder>  m_muonStationTypeBuilder; //!< Helper Tool to create TrackingVolume Arrays

      ToolHandle<Trk::ITrackingVolumeHelper>    m_trackingVolumeHelper;   //!< Helper Tool to create TrackingVolumes

      mutable Trk::Material                   m_muonMaterial;               //!< the material
      //mutable std::pair<bool,Trk::Material>   m_sTgcMaterial;  //!< the material
      //mutable std::pair<bool,Trk::Material>   m_mmMaterial;    //!< the material
      Trk::GeoShapeConverter*             m_geoShapeConverter;          //!< shape converter
      Trk::GeoMaterialConverter*          m_materialConverter;          //!< material converter
      bool                                m_buildBarrel;
      bool                                m_buildEndcap;
      bool                                m_buildCsc;
      bool                                m_buildTgc;  
      bool                                m_resolveActiveLayers;
    };


} // end of namespace


#endif //MUONTRACKINGGEOMETRY_MUONSTATIONBUILDER_H


