# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from TrkDetDescrSvc.TrkDetDescrSvcConf import Trk__TrackingGeometrySvc
from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilder
from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilder import ConfiguredCaloTrackingGeometryBuilder
from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilder import ConfiguredInDetTrackingGeometryBuilder
from MuonTrackingGeometry.ConfiguredMuonTrackingGeometry import MuonTrackingGeometryBuilder
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg, addFolders, addFoldersSplitOnline
from SubDetectorEnvelopes.SubDetectorEnvelopesConfig import EnvelopeDefSvc

def _setupCondDB(flags, CoolDataBaseFolder):
    result=ComponentAccumulator()
    
    # the tag names
    materialTagBase = 'AtlasLayerMat_v'
    version = 21
    sub_version = ''
    
    AtlasMaterialTag = materialTagBase+str(version)+sub_version+'_'
    cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
    # load the right folders (preparation for calo inclusion)
    result.merge( addFoldersSplitOnline(flags,'GLOBAL',[cfolder],[cfolder]) )
    return result
    
def TrackingGeometrySvcCfg( flags ) :   
    result = ComponentAccumulator()

    AtlasTrackingGeometryName = 'AtlasTrackingGeometry'
    atlas_geometry_builder = Trk__GeometryBuilder(name = 'AtlasGeometryBuilder')
    result.addPublicTool(atlas_geometry_builder) #TODO remove one day
    
    AtlasEnvelopeSvc = EnvelopeDefSvc( flags )
    result.addService(AtlasEnvelopeSvc)
    
    # No Det Flags right now.
    
    # Inner Detector
    if (False):
        InDetTrackingGeometryBuilder = ConfiguredInDetTrackingGeometryBuilder(name ='InDetTrackingGeometryBuilder')
        result.addPublicTool(InDetTrackingGeometryBuilder)
        # and give it to the Geometry Builder
        atlas_geometry_builder.InDetTrackingGeometryBuilder = InDetTrackingGeometryBuilder
    
    # (Calo)
    if (False):
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
        CaloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
        result.addPublicTool(CaloVolumeCreator)

        CaloTrackingGeometryBuilder = ConfiguredCaloTrackingGeometryBuilder(name='CaloTrackingGeometryBuilder');
        CaloTrackingGeometryBuilder.TrackingVolumeCreator = CaloVolumeCreator
        CaloTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
        CaloTrackingGeometryBuilder.GeometryName          = 'Calo'
        result.addPublicTool(CaloTrackingGeometryBuilder)
        # and give it to the Geometry Builder
        atlas_geometry_builder.CaloTrackingGeometryBuilder = CaloTrackingGeometryBuilder

    # (Muon)
    if (True):
        MuonTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
        result.addPublicTool(MuonTrackingGeometryBuilder)
        # and give it to the Geometry Builder
        atlas_geometry_builder.MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilder   
   
    # the material provider
    from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialProvider
    AtlasMaterialProvider = Trk__LayerMaterialProvider('AtlasMaterialProvider')
    CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialV2'
    
    AtlasMaterialProvider.LayerMaterialMapName    = CoolDataBaseFolder
    result.addPublicTool(AtlasMaterialProvider)

    AtlasGeometryProcessors = [ AtlasMaterialProvider ]

    # Setup DBs
    result.merge(_setupCondDB(flags, CoolDataBaseFolder))
    
    tracking_geometry_service = Trk__TrackingGeometrySvc(name = 'AtlasTrackingGeometrySvc', GeometryBuilder=atlas_geometry_builder)
    result.addService(tracking_geometry_service)
    
    return result, tracking_geometry_service
    
if __name__ == '__main__':
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    result = ComponentAccumulator()

    acc, tracking_geometry_service = TrackingGeometrySvcCfg(ConfigFlags )
    result.merge(acc)

    f=open('TrackingGeometrySvcCfg.pkl','w')
    acc.store(f)
    f.close()
