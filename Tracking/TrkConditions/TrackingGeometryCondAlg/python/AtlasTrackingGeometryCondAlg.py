# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

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
def modifyCondAlg(the_name, **props) :
    from AthenaCommon.AlgSequence import AlgSequence
    from AthenaCommon.AlgSequence import AthSequencer
    cond_seq=AthSequencer("AthCondSeq")
    for seq in [AlgSequence(),cond_seq] :
        if hasattr(seq,the_name) :
            alg = getattr(seq,the_name)
            print('Modified %s' % the_name)
            for k,v in props.items() :
                setattr(alg,k,v)
            print (alg)
            return

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags


from TrackingGeometryCondAlg.TrackingGeometryCondAlgConf import Trk__TrackingGeometryCondAlg
   
class ConfiguredTrackingGeometryCondAlg( Trk__TrackingGeometryCondAlg ) :   

    # constructor
    def __init__(self,name = 'AtlasTrackingGeometryCondAlg'):
        nameSuffix='Cond'
        atlas_tracking_geometry_name = 'AtlasTrackingGeometry'
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
          print ('[ Configuration : start ] *** '+name+' ********************************')
          print ('[ TrackingGeometryCondAlg ]')
        
        from AthenaCommon.AppMgr import ToolSvc 

        #################################################################################
        # The Geometry Builder
        #################################################################################
        
        # the geometry builder alg tool
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__GeometryBuilderCond
        AtlasGeometryBuilder = Trk__GeometryBuilderCond(name = 'AtlasGeometryBuilder'+nameSuffix)
        # switch the building outputlevel on 
        AtlasGeometryBuilder.OutputLevel = TrkDetFlags.ConfigurationOutputLevel()
        
        # the envelope definition service
        from AthenaCommon.CfgGetter import getService
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
              from InDetTrackingGeometry.ConfiguredInDetTrackingGeometryBuilderCond import(
                  ConfiguredInDetTrackingGeometryBuilderCond as IDGeometryBuilder)
          else:
              from InDetTrackingGeometry.ConfiguredStagedTrackingGeometryBuilderCond import (
                  ConfiguredStagedTrackingGeometryBuilderCond as IDGeometryBuilder)
          from AthenaCommon.BeamFlags import jobproperties
          InDetTrackingGeometryBuilder = IDGeometryBuilder(
              name='InDetTrackingGeometryBuilder'+nameSuffix,
              nameSuffix=nameSuffix,
              buildTrtStrawLayers=(jobproperties.Beam.beamType == "cosmics"))

          InDetTrackingGeometryBuilder.EnvelopeDefinitionSvc = AtlasEnvelopeSvc
          InDetTrackingGeometryBuilder.OutputLevel = TrkDetFlags.InDetBuildingOutputLevel()
          # and give it to the Geometry Builder
          AtlasGeometryBuilder.InDetTrackingGeometryBuilder = InDetTrackingGeometryBuilder

        # (Calo)
        if DetFlags.Calo_on() :
           from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
           CaloVolumeCreator = Trk__CylinderVolumeCreator("CaloVolumeCreator")
           CaloVolumeCreator.OutputLevel = TrkDetFlags.CaloBuildingOutputLevel()
           ToolSvc += CaloVolumeCreator

           from CaloTrackingGeometry.ConfiguredCaloTrackingGeometryBuilderCond import ConfiguredCaloTrackingGeometryBuilderCond as ConfiguredCaloGeo 
           CaloTrackingGeometryBuilder = ConfiguredCaloGeo(name='CaloTrackingGeometryBuilder',nameSuffix=nameSuffix)
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

        # adding the artificial SiDetEltCondAlg dependencies after everything else has been set up
        if DetFlags.ID_on() :

          # artifical dependencies to Si-DetectorElement conditions algs to ensure that
          # the IOV is identical to the IOV of the tracking geoemtry

          # Edit the conditions algorithm sequence to ensure dependencies are executed in a feasible order.
          # Only necessary for single threaded athena where the AvalancheScheduler is not used.
          pixelDep = "PixelAlignCondAlg/PixelAlignCondAlg"
          sctDep = "SCT_AlignCondAlg/SCT_AlignCondAlg"
          trtDep = "TRTAlignCondAlg/TRTAlignCondAlg"
          muonAlignDep = "MuonAlignmentCondAlg/MuonAlignmentCondAlg"
          muonDetDep = "MuonDetectorCondAlg/MuonDetectorCondAlg"
          condInputLoader = "CondInputLoader/CondInputLoader"
          dependencies = {pixelDep,
                          sctDep,
                          trtDep,
                          muonAlignDep,
                          muonDetDep,
                          condInputLoader}
          from AthenaCommon.ConcurrencyFlags import jobproperties as jp
          if jp.ConcurrencyFlags.NumThreads() <= 0:
              prependList = list()
              appendList = list()
              from AthenaCommon.AlgSequence import AthSequencer
              cond_seq=AthSequencer("AthCondSeq")
              condAlgs = cond_seq._Configurable__children
              for alg in condAlgs:
                  condAlgName = alg.getFullName()
                  if condAlgName in dependencies:
                      prependList.append(alg)
                  else:
                      appendList.append(alg)
              prependList.extend(appendList)
              cond_seq._Configurable__children = prependList
              
          MuonManagerKey  = "MuonDetectorManager"     if DetFlags.Muon_on()  else ""
          TRT_DetEltKey   = "TRT_DetElementContainer" if DetFlags.TRT_on()   else ""
          SCTAlignStore   = "SCTAlignmentStore"       if DetFlags.SCT_on()   else ""
          PixelAlignStore = "PixelAlignmentStore"     if DetFlags.pixel_on() else ""

          modifyCondAlg('SCT_DetectorElementCondAlg', MuonManagerKey      = MuonManagerKey,
                                                      TRT_DetEltContKey   = TRT_DetEltKey,
                                                      PixelAlignmentStore = PixelAlignStore)

          modifyCondAlg('PixelDetectorElementCondAlg', MuonManagerKey     = MuonManagerKey,
                                                       TRT_DetEltContKey  = TRT_DetEltKey,
                                                       SCTAlignmentStore  = SCTAlignStore)
        #end editing cond_seq for artificial SiDetEltCondAlg dependencies
        
        # processors
        AtlasGeometryProcessors = []   
           
        # check whether the material retrieval is ment to be from COOL
        if TrkDetFlags.MaterialSource() == 'COOL':
            # the material provider
            from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerMaterialProvider as LayerMaterialProvider
            AtlasMaterialProvider = LayerMaterialProvider('AtlasMaterialCondProvider')
            AtlasMaterialProvider.OutputLevel           = TrkDetFlags.ConfigurationOutputLevel()
            AtlasMaterialProvider.LayerMaterialMapKey   = TrkDetFlags.MaterialStoreGateKey()
        
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
                conddb.addFolderWithTag('',DataBaseConnection+CoolDataBaseFolder,AtlasMaterialTag+MagicTag,force=True,
                                        className = 'Trk::LayerMaterialMap')
                if TrkDetFlags.ConfigurationOutputLevel() < 3 :
                    print ('[ TrackingGeometryCondAlg ] Using Local Database: '+DataBaseConnection)
                # make sure that the pool files are in the catalog
            elif TrkDetFlags.SLHC_Geometry() :
                # set the folder to the SLHC location        
                CoolDataBaseFolder = '/GLOBAL/TrackingGeo/LayerMaterialITK'
                ctag = AtlasMaterialTag+TrkDetFlags.MaterialMagicTag()
                cfoldertag = CoolDataBaseFolder+' <tag>'+ctag+'</tag>'
                conddb.addFolderSplitMC('GLOBAL',cfoldertag,cfoldertag,
                                        className = 'Trk::LayerMaterialMap')
            else :
                print ('[ TrackingGeometryCondAlg ]     base material tag : ', AtlasMaterialTag)
                cfolder = CoolDataBaseFolder +'<tag>TagInfoMajor/'+AtlasMaterialTag+'/GeoAtlas</tag>'
                print ('[ TrackingGeometryCondAlg ]     translated to COOL: ', cfolder)
                # load the right folders (preparation for calo inclusion)
                conddb.addFolderSplitMC('GLOBAL',cfolder,cfolder,
                                        className = 'Trk::LayerMaterialMap')

        elif TrkDetFlags.MaterialSource() == 'Input' :
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
        Trk__TrackingGeometryCondAlg.__init__(self,name,
                                              GeometryBuilder = AtlasGeometryBuilder,
                                              GeometryProcessors = AtlasGeometryProcessors,
                                              TrackingGeometryWriteKey = atlas_tracking_geometry_name,
                                              OutputLevel = TrkDetFlags.ConfigurationOutputLevel())
        # screen output of the configuration
        if TrkDetFlags.ConfigurationOutputLevel() < 3 :
           print (self)
           print ('* [ GeometryBuilder       ]')
           print (AtlasGeometryBuilder)
           print ('* [ Configuration : end   ] ***'+name+'********************************')
        
