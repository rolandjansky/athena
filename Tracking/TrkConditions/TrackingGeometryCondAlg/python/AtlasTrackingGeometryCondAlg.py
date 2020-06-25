# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

##################################################################################
# The AtlasTrackingGeometryCondAlg fragment
#
# usage: 
#   from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlg import ConfiguredTrackingGeometryCondAlg
#   TrkGeoCondAlg = ConfiguredTrackingGeometryCondAlg('AtlasTrackingGeometryCondAlg')
#   from AthenaCommon.AlgSequence import AthSequencer
#   condSeq = AthSequencer("AthCondSeq")
#   condSeq+= TrkGeoCondAlg
#
##################################################################################

from __future__ import print_function

# import the DetFlags for the setting
from AthenaCommon.DetFlags import DetFlags

#################################################################################
# Material for the Geometry comes from COOL or local database
#################################################################################

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags


from TrackingGeometryCondAlg.TrackingGeometryCondAlgConf import Trk__TrackingGeometryCondAlg
   
class ConfiguredTrackingGeometryCondAlg( Trk__TrackingGeometryCondAlg ) :   

    # constructor
    def __init__(self,name = 'AtlasTrackingGeometryCondAlg'):
        
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
          print ('[ Configuration : start ] *** '+name+' ********************************')
          print ('[ TrackingGeometryCondAlg ]')
        
        from AthenaCommon.AppMgr import ToolSvc 

        #################################################################################
        # The Geometry Builder
        #################################################################################
        
        # the geometry builder alg tool
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilderCond
        AtlasGeometryBuilder = Trk__GeometryBuilderCond(name = 'AtlasGeometryBuilder')
        # switch the building outputlevel on 
        AtlasGeometryBuilder.OutputLevel = TrkDetFlags.ConfigurationOutputLevel()
        
        # the envelope definition service
        from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
           getService,getServiceClone,getAlgorithm,getAlgorithmClone
        AtlasEnvelopeSvc = getService('AtlasGeometry_EnvelopeDefSvc')
           
        # (ID) 
        if DetFlags.ID_on() :
#          FIXME: removed to get the migration to CondAlg started
#          # get hand on the ID Tracking Geometry Builder
#          if TrkDetFlags.ISF_FatrasCustomGeometry() :
#              if hasattr(ToolSvc, TrkDetFlags.ISF_FatrasCustomGeometryBuilderName()):
#                  InDetTrackingGeometryBuilder = getattr(ToolSvc, TrkDetFlags.ISF_FatrasCustomGeometryBuilderName())
#          elif TrkDetFlags.XMLFastCustomGeometry() :
#              if hasattr(ToolSvc, TrkDetFlags.InDetTrackingGeometryBuilderName()):
#                  InDetTrackingGeometryBuilder = getattr(ToolSvc, TrkDetFlags.InDetTrackingGeometryBuilderName())
#          else:
          if not TrkDetFlags.InDetStagedGeometryBuilder():
              from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilderCond import ConfiguredInDetTrackingGeometryBuilderCond as IDGeometryBuilder
          else:
              from InDetTrackingGeometry.ConfiguredStagedTrackingGeometryBuilderCond import ConfiguredStagedTrackingGeometryBuilderCond as IDGeometryBuilder
          InDetTrackingGeometryBuilder = IDGeometryBuilder(name ='InDetTrackingGeometryBuilder')
                
          InDetTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
          InDetTrackingGeometryBuilder.OutputLevel = TrkDetFlags.InDetBuildingOutputLevel()
          # and give it to the Geometry Builder
          AtlasGeometryBuilder.InDetTrackingGeometryBuilder = InDetTrackingGeometryBuilder
          # 
        # (Calo)
        if DetFlags.Calo_on() :
           from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
           CaloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
           CaloVolumeCreator.OutputLevel = TrkDetFlags.CaloBuildingOutputLevel()
           ToolSvc += CaloVolumeCreator

           from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilderCond import ConfiguredCaloTrackingGeometryBuilderCond as ConfiguredCaloGeo 
           CaloTrackingGeometryBuilder = ConfiguredCaloGeo(name='CaloTrackingGeometryBuilder');
           CaloTrackingGeometryBuilder.TrackingVolumeCreator = CaloVolumeCreator
           CaloTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
           CaloTrackingGeometryBuilder.OutputLevel           = TrkDetFlags.CaloBuildingOutputLevel()
           CaloTrackingGeometryBuilder.GeometryName          = 'Calo'
           # and give it to the Geometry Builder
           AtlasGeometryBuilder.CaloTrackingGeometryBuilder = CaloTrackingGeometryBuilder
        
        # (Muon)
        if DetFlags.Muon_on() :
           from MuonTrackingGeometry.ConfiguredMuonTrackingGeometryCond import MuonTrackingGeometryBuilderCond
           MuonTrackingGeometryBuilderCond.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
           # and give it to the Geometry Builder
           AtlasGeometryBuilder.MuonTrackingGeometryBuilder = MuonTrackingGeometryBuilderCond
           
        # processors
        AtlasGeometryProcessors = []   
           
        # check whether the material retrieval is ment to be from COOL
        if TrkDetFlags.MaterialSource() is 'COOL':
            # the material provider
            from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialProvider as LayerMaterialProvider
            AtlasMaterialProvider = LayerMaterialProvider('AtlasMaterialProvider')
            AtlasMaterialProvider.OutputLevel           = TrkDetFlags.ConfigurationOutputLevel()
            AtlasMaterialProvider.LayerMaterialMapName  = TrkDetFlags.MaterialStoreGateKey()
        
            AtlasGeometryProcessors += [ AtlasMaterialProvider ]
        
            # the tag names
            CoolDataBaseFolder = TrkDetFlags.MaterialStoreGateKey()
            AtlasMaterialTag = TrkDetFlags.MaterialTagBase()+str(TrkDetFlags.MaterialVersion())+TrkDetFlags.MaterialSubVersion()+'_'
        
            if TrkDetFlags.ConfigurationOutputLevel() < 3 :
               print ('[ TrackingGeometryCondAlg ] Associating DB folder : ', CoolDataBaseFolder)
        
            # we need the conditions interface
            from IOVDbSvc.CondDB import conddb
            # use a local database
            if TrkDetFlags.MaterialDatabaseLocal():
                # specify the local database
                DataBasePath  = TrkDetFlags.MaterialDatabaseLocalPath() 
                DataBaseName  = TrkDetFlags.MaterialDatabaseLocalName()
                MagicTag      = TrkDetFlags.MaterialMagicTag()
                DataBaseConnection = '<dbConnection>sqlite://X;schema='+DataBasePath+DataBaseName+';dbname=OFLP200</dbConnection>'
                conddb.blockFolder('/GLOBAL/TrackingGeo/LayerMaterialV2')
                conddb.addFolderWithTag('',DataBaseConnection+CoolDataBaseFolder,AtlasMaterialTag+MagicTag,force=True)
                if TrkDetFlags.ConfigurationOutputLevel() < 3 :
                    print ('[ TrackingGeometryCondAlg ] Using Local Database: '+DataBaseConnection)
                # make sure that the pool files are in the catalog
            elif TrkDetFlags.SLHC_Geometry() :
                # set the folder to the SLHC location        
                CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialITK'
                ctag = AtlasMaterialTag+TrkDetFlags.MaterialMagicTag()
                cfoldertag = CoolDataBaseFolder+' <tag>'+ctag+'</tag>'
                conddb.addFolderSplitMC('GLOBAL',cfoldertag,cfoldertag)
            else :
                print ('[ TrackingGeometryCondAlg ]     base material tag : ', AtlasMaterialTag)
                cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
                print ('[ TrackingGeometryCondAlg ]     translated to COOL: ', cfolder)
                # load the right folders (preparation for calo inclusion)
                conddb.addFolderSplitMC('GLOBAL',cfolder,cfolder)

        elif TrkDetFlags.MaterialSource() is 'Input' :
            # the material provider
            from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__InputLayerMaterialProvider
            AtlasMaterialProvider = Trk__InputLayerMaterialProvider('AtlasMaterialProvider')
            AtlasMaterialProvider.OutputLevel           = TrkDetFlags.ConfigurationOutputLevel()
            AtlasGeometryProcessors += [ AtlasMaterialProvider ]            

        # material validation
        if TrkDetFlags.MaterialValidation() :
            # load the material inspector
            from TrkDetDescrTestTools.TrkDetDescrTestToolsConf import Trk__LayerMaterialInspector
            AtlasLayerMaterialInspector = Trk__LayerMaterialInspector('AtlasLayerMaterialInspector')
            AtlasLayerMaterialInspector.OutputLevel = TrkDetFlags.ConfigurationOutputLevel()
            
            AtlasGeometryProcessors += [ AtlasLayerMaterialInspector ]

        # call the base class constructor : sets the tools
        Trk__TrackingGeometryCondAlg.__init__(self,name,\
                                          GeometryBuilder = AtlasGeometryBuilder,\
                                          GeometryProcessors = AtlasGeometryProcessors, \
                                          OutputLevel = TrkDetFlags.ConfigurationOutputLevel())
        
        # screen output of the configuration
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
           print (self)
           print ('* [ GeometryBuilder       ]')
           print (AtlasGeometryBuilder)
           print ('* [ Configuration : end   ] ***'+name+'********************************')
        
##################################################################################    

# now create the instance
#AtlasTrackingGeometryCondAlg = ConfiguredTrackingGeometryCondAlg('AtlasTrackingGeometryCondAlg')
# add it to the ServiceManager
#from AthenaCommon.AlgSequence import AthSequencer
#condSeq = AthSequencer("AthCondSeq")
#condSeq+= AtlasTrackingGeometryCondAlg
