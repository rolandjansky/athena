######################################################
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# ConfiguredSLHC_InDetTrackingGeometryXML module
#
# it inherits from StagedTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__StagedTrackingGeometryBuilder
from XMLReaderJobProperties import XMLReaderFlags

# define the class
class ConfiguredSLHC_InDetTrackingGeometryXMLBuilder( InDet__StagedTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingXMLGeometryBuilder',
                      namePrefix = '',
                 splitFirstLayer     = False,
                 setLayerAssociation = True):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
        from AthenaCommon.DetFlags import DetFlags
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.InDetBuildingOutputLevel()
        # the binning type of the layers   
        BeamPipeBinning = 2
        # where to build it from
        BeamPipeBuilder.BeamPipeFromGeoModel    = TrkDetFlags.BeamPipeFromGeoModel()
        BeamPipeBuilder.BeamPipeEnvelope        = TrkDetFlags.BeamPipeEnvelope()
        if not TrkDetFlags.BeamPipeFromGeoModel():
            BeamPipeBuilder.BeamPipeOffsetX	        = TrkDetFlags.BeamPipeOffsetX()
            BeamPipeBuilder.BeamPipeOffsetY	        = TrkDetFlags.BeamPipeOffsetY()
            BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()

        ToolSvc += BeamPipeBuilder

        # the layer providers
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerProvider
        BeamPipeProvider = Trk__LayerProvider(name=namePrefix+'BeamPipeProvider')
        BeamPipeProvider.LayerBuilder = BeamPipeBuilder
        ToolSvc += BeamPipeProvider
                
        # the layer builders
        layerProviders   = [ BeamPipeProvider ]
        binningsCenter  = [ BeamPipeBinning ]
        binningsEndcap  = [ BeamPipeBinning ]
        colors          = [ 2 ]



        ###### Flags required to properly build the tracking volumes
        from InDetTrackingGeometryXML.XMLReaderJobProperties import XMLReaderFlags

        isRingLayout       = XMLReaderFlags.isRingLayout()
        splitBarrelLayers  = XMLReaderFlags.splitBarrelLayers()
        InnerLayers        = XMLReaderFlags.InnerLayerIndices()
        InnerDisks         = XMLReaderFlags.InnerDiskIndices()
        doPix              = XMLReaderFlags.doPix()
        doSCT              = XMLReaderFlags.doSCT()
        readXMLfromDB      = XMLReaderFlags.readXMLfromDB()
        
        print "TrackingXML endcap layer  ",isRingLayout," ",splitBarrelLayers," ",InnerLayers, " - ", InnerDisks

        # PIXEL building
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__LayerProviderXML
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__EndcapBuilderXML
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__BarrelBuilderXML
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__StaveBuilderXML
        from InDetTrackingGeometryXML.InDetTrackingGeometryXMLConf import InDet__SiModuleProvider

        InDetModuleProvider = InDet__SiModuleProvider(name=namePrefix+'SiModuleProvider'  )
        InDetEndcapBuilder  = InDet__EndcapBuilderXML(name=namePrefix+'PixelEndcapBuilder')
        InDetBarrelBuilder  = InDet__BarrelBuilderXML(name=namePrefix+'PixelBarrelBuilder')
        InDetStaveBuilder   = InDet__StaveBuilderXML (name=namePrefix+'StaveBuilderXML'   )

        ToolSvc += InDetModuleProvider
        ToolSvc += InDetStaveBuilder
        ToolSvc += InDetBarrelBuilder
        ToolSvc += InDetEndcapBuilder

        InDetStaveBuilder.ModuleProvider        = InDetModuleProvider
        InDetBarrelBuilder.ModuleProvider       = InDetModuleProvider
        InDetBarrelBuilder.StaveBuilder         = InDetStaveBuilder
        InDetBarrelBuilder.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
        InDetBarrelBuilder.BarrelLayerBinsPhi   = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
        InDetBarrelBuilder.OutputLevel          = TrkDetFlags.InDetBuildingOutputLevel()
        
        InDetEndcapBuilder.ModuleProvider       = InDetModuleProvider
        InDetEndcapBuilder.ModuleProvider       = InDetModuleProvider
        InDetEndcapBuilder.EndcapLayerBinR      = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
        InDetEndcapBuilder.EndcapLayerBinPhi    = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
        InDetEndcapBuilder.OutputLevel          = TrkDetFlags.InDetBuildingOutputLevel()
        
	
##         if doSCT :
##             InDetBarrelSCTLayerBuilder = InDet__BarrelBuilderXML(name='SCTBarrelBuilder')
##             InDetBarrelSCTLayerBuilder.StaveBuilder = InDetStaveBuilder
##             InDetBarrelSCTLayerBuilder.ModuleProvider = InDetModuleProvider
##             ToolSvc += InDetBarrelSCTLayerBuilder
            
##             InDetEndcapSCTLayerBuilder = InDet__EndcapBuilderXML(name='SCTEndcapBuilder')
##             InDetEndcapSCTLayerBuilder.ModuleProvider = InDetModuleProvider
##             ToolSvc += InDetEndcapSCTLayerBuilder 


        startLayer = 0
        startEndcap = 0
        if splitBarrelLayers:
            endLayer = InnerLayers[-1]
            # Pixel layer provider
            PixelFirstLayerProvider = InDet__LayerProviderXML(name=namePrefix+'PixelFirstLayerProvider')
            PixelFirstLayerProvider.Identification          = 'Pixel0'
            PixelFirstLayerProvider.OutputLevel             = 1
            PixelFirstLayerProvider.doPix = doPix
            PixelFirstLayerProvider.doSCT = False
            PixelFirstLayerProvider.startLayer = startLayer
            PixelFirstLayerProvider.endLayer = endLayer
            PixelFirstLayerProvider.ModuleProvider = InDetModuleProvider
            PixelFirstLayerProvider.PixelBarrelBuilder = InDetBarrelBuilder
            PixelFirstLayerProvider.PixelEndcapBuilder = InDetEndcapBuilder
             
            if (len(InnerDisks)>0):
                endEndcap = InnerDisks[-1]
                PixelFirstLayerProvider.startEndcap = startEndcap
                PixelFirstLayerProvider.endEndcap   = endEndcap
                startEndcap = endEndcap + 1
            else:
                PixelFirstLayerProvider.startEndcap = startLayer
                PixelFirstLayerProvider.endEndcap   = endLayer
            
            startLayer = endLayer + 1
            ToolSvc += PixelFirstLayerProvider
            PixelFirstLayerBinning = 2
            layerProviders  += [ PixelFirstLayerProvider ]
            binningsCenter  += [ PixelFirstLayerBinning ]
            binningsEndcap  += [ PixelFirstLayerBinning ]
            colors          += [ 3 ]
            #print PixelFirstLayerProvider
            
        if doPix:
            # Pixel layer provider
            PixelLayerProvider = InDet__LayerProviderXML(name=namePrefix+'PixelLayerProvider')
            PixelLayerProvider.Identification          = 'Pixel'
            PixelLayerProvider.OutputLevel             = 1
            PixelLayerProvider.doPix = doPix
            PixelLayerProvider.doSCT = False
            PixelLayerProvider.startLayer = startLayer
            PixelLayerProvider.endLayer = 4

            if (len(InnerDisks)>0):
                PixelLayerProvider.startEndcap = startEndcap
                PixelLayerProvider.endEndcap   = 32
            else:
                PixelLayerProvider.startEndcap = startLayer
                PixelLayerProvider.endEndcap   = 4

            PixelLayerProvider.ModuleProvider = InDetModuleProvider
            PixelLayerProvider.PixelBarrelBuilder = InDetBarrelBuilder
            PixelLayerProvider.PixelEndcapBuilder = InDetEndcapBuilder
            ToolSvc += PixelLayerProvider
            PixelLayerBinning = 2
            layerProviders  += [ PixelLayerProvider ]
            binningsCenter  += [ PixelLayerBinning ]
            binningsEndcap  += [ PixelLayerBinning ]
            colors          += [ 3 ]
            #print PixelLayerProvider


##         if doSCT:
##             # SCT layer provider
##             SCTLayerProvider = InDet__LayerProviderXML(name=namePrefix+'SCTLayerProvider')
##             SCTLayerProvider.Identification          = 'SCT'
##             SCTLayerProvider.OutputLevel             = 1
##             SCTLayerProvider.doPix = False
##             SCTLayerProvider.doSCT = doSCT
##             SCTLayerProvider.ModuleProvider = InDetModuleProvider
##             SCTLayerProvider.SCTBarrelBuilder = InDetBarrelSCTLayerBuilder
##             SCTLayerProvider.SCTEndcapBuilder = InDetEndcapSCTLayerBuilder                                    
##             ToolSvc += SCTLayerProvider
##             SCTLayerBinning = 2
##             layerProviders  += [ SCTLayerProvider ]
##             binningsCenter  += [ SCTLayerBinning ]
##             binningsEndcap  += [ SCTLayerBinning ]
##             colors          += [ 4 ]

            
        if DetFlags.SCT_on() :
            from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
            # SCT building
            SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
            SCT_LayerBuilder.PixelCase                       = False
            SCT_LayerBuilder.Identification                  = 'SCT1'
            SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
            # general steering
            SCT_LayerBuilder.SplitMode                       = TrkDetFlags.SCT_SplitMode()
            # SCT barrel specifications
            SCT_LayerBuilder.BarrelLayerBinsZ                = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
            SCT_LayerBuilder.BarrelLayerBinsPhi              = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
            # SCT endcap specifications                          
            SCT_LayerBuilder.EndcapLayerBinsR                = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
            SCT_LayerBuilder.EndcapLayerBinsPhi              = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
            # SCT_LayerBuilder.EndcapComplexRingBinning        = TrkDetFlags.SCT_EndcapLayerDynamicRings()
            # set the layer association                   
            SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
            # output level                                 
            SCT_LayerBuilder.OutputLevel                     = TrkDetFlags.SCT_BuildingOutputLevel()
            # the binning type of the layer     
            SCT_LayerBinning = 2
            # SCT -> ToolSvc                             
            ToolSvc += SCT_LayerBuilder                       
            # the layer providers
            SCT_LayerProvider = Trk__LayerProvider(name=namePrefix+'SCT_LayerProvider')
            SCT_LayerProvider.LayerBuilder = SCT_LayerBuilder
            ToolSvc += SCT_LayerProvider

            # put them to the caches
            layerProviders  += [ SCT_LayerProvider ]
            binningsCenter += [ SCT_LayerBinning ]
            binningsEndcap += [ SCT_LayerBinning ]
            colors         += [ 4 ]
                                                              
            # the split and the second SCT layer builder is needed if the SCT outer barrel
            # spans over the full width      
            if TrkDetFlags.SCT_SplitMode() is not 0 :
              print "[SLHC_GeometryBuilder] Building SCT layers in split mode"
              from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
              SCT_LayerBuilder2 = InDet__SCT_LayerBuilder(name=namePrefix+'SCT_LayerBuilder2')
            
              # switch of the pixel case
              SCT_LayerBuilder2.PixelCase                = False
              SCT_LayerBuilder2.SiDetManagerLocation     = 'SCT'
              # set the general parameters
              SCT_LayerBuilder2.OutputLevel              = 2
              SCT_LayerBuilder2.SplitMode                = TrkDetFlags.SCT_SplitMode()
              SCT_LayerBuilder2.Identification           = "SCT2"
              # SCT barrel specifications
              SCT_LayerBuilder2.BarrelLayerBinsZ         = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
              SCT_LayerBuilder2.BarrelLayerBinsPhi       = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
              # SCT endcap specifications                     
              SCT_LayerBuilder2.EndcapLayerBinsR         = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
              SCT_LayerBuilder2.EndcapLayerBinsPhi       = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
              # set the layer association
              SCT_LayerBuilder2.SetLayerAssociation = setLayerAssociation        
              # output level
              SCT_LayerBuilder2.OutputLevel         = TrkDetFlags.SCT_BuildingOutputLevel()
              ToolSvc += SCT_LayerBuilder2
              # the layer providers
              SCT_LayerProvider2 = Trk__LayerProvider(name=namePrefix+'SCT_LayerProvider2')
              SCT_LayerProvider2.LayerBuilder = SCT_LayerBuilder2
              ToolSvc += SCT_LayerProvider2
              # put them to the caches
              layerProviders  += [ SCT_LayerProvider2 ] 
              binningsCenter += [ SCT_LayerBinning ]
              binningsEndcap += [ SCT_LayerBinning ]
              colors         += [ 5 ]


        # helpers for the InDetTrackingGeometry Builder : layer array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerArrayCreator
        InDetLayerArrayCreator = Trk__LayerArrayCreator(name = 'InDetLayerArrayCreator')
        InDetLayerArrayCreator.EmptyLayerMode           = 2 # deletes empty material layers from arrays
        InDetLayerArrayCreator.OutputLevel              = TrkDetFlags.InDetBuildingOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetLayerArrayCreator

        # helpers for the InDetTrackingGeometry Builder : volume array creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeArrayCreator
        InDetTrackingVolumeArrayCreator                       = Trk__TrackingVolumeArrayCreator(name = 'InDetTrackingVolumeArrayCreator')
        InDetTrackingVolumeArrayCreator.OutputLevel           = TrkDetFlags.InDetBuildingOutputLevel()
        # add to ToolSvc
        ToolSvc += InDetTrackingVolumeArrayCreator
        
        # helpers for the InDetTrackingGeometry Builder : tracking voluem helper for glueing
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__TrackingVolumeHelper
        InDetTrackingVolumeHelper                             = Trk__TrackingVolumeHelper(name ='InDetTrackingVolumeHelper')
        InDetTrackingVolumeHelper.OutputLevel                 = TrkDetFlags.InDetBuildingOutputLevel()
        # the material bins 
        InDetTrackingVolumeHelper.BarrelLayerBinsZ            = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()
        InDetTrackingVolumeHelper.BarrelLayerBinsPhi          = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi() 
        InDetTrackingVolumeHelper.EndcapLayerBinsR            = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()
        InDetTrackingVolumeHelper.EndcapLayerBinsPhi          = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi() 
        # add to ToolSvc
        ToolSvc += InDetTrackingVolumeHelper
        
        # helpers for the InDetTrackingGeometry Builder : cylinder volume creator
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__CylinderVolumeCreator
        InDetCylinderVolumeCreator = Trk__CylinderVolumeCreator(name = 'InDetCylinderVolumeCreator')
        # give him the layer array creator
        InDetCylinderVolumeCreator.LayerArrayCreator          = InDetLayerArrayCreator
        InDetCylinderVolumeCreator.TrackingVolumeArrayCreator = InDetTrackingVolumeArrayCreator
        InDetCylinderVolumeCreator.TrackingVolumeHelper       = InDetTrackingVolumeHelper
        # specifiy the binning, passive layers, entry layers
        InDetCylinderVolumeCreator.PassiveLayerThickness      = TrkDetFlags.InDetPassiveLayerThickness()  
        InDetCylinderVolumeCreator.PassiveLayerBinsPhi        = TrkDetFlags.InDetPassiveLayerMaterialBinsPhi()  
        InDetCylinderVolumeCreator.PassiveLayerBinsRZ         = TrkDetFlags.InDetPassiveLayerMaterialBinsRz()  
        # output level                                       
        InDetCylinderVolumeCreator.OutputLevel                = TrkDetFlags.InDetBuildingOutputLevel()        
        # add to ToolSvc
        ToolSvc += InDetCylinderVolumeCreator        
        
        # the envelope definition service
        from SubDetectorEnvelopes.SubDetectorEnvelopesConf import DetDescrDBEnvelopeSvc
        AtlasEnvelopeSvc = DetDescrDBEnvelopeSvc('AtlasEnvelopeSvcDefinitionSvc')
        # set the output level for the Envelope service
        AtlasEnvelopeSvc.OutputLevel      = TrkDetFlags.InDetBuildingOutputLevel()  
        # add to SvcMgr
        ServiceMgr += AtlasEnvelopeSvc
        

        # the tracking geometry builder
        InDet__StagedTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      BeamPipeBuilder           = BeamPipeBuilder,\
                                                      LayerBuilders             = layerProviders,
                                                      LayerBinningTypeCenter    = binningsCenter,
                                                      LayerBinningTypeEndcap    = binningsEndcap,
                                                      ColorCodes                = colors,
                                                      EnvelopeDefinitionSvc     = AtlasEnvelopeSvc,
                                                      EnvelopeCover                 = 0.5,
#                                                      VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                      TrackingVolumeCreator     = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator         = InDetLayerArrayCreator,
                                                      CheckForRingLayout        = isRingLayout,
                                                      BuildBoundaryLayers       = False,
                                                      ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      OutputLevel               = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName            = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode         = TrkDetFlags.MagneticFieldMode(),
                                                      isSLHC = True)


##         InDet__StagedTrackingGeometryBuilder.__init__(self,namePrefix+name,\
##                                                       LayerBuilders                 = layerProviders,
##                                                       LayerBinningTypeCenter        = binningsCenter,
##                                                       LayerBinningTypeEndcap        = binningsEndcap,
##                                                       ColorCodes                    = colors,
##                                                       EnvelopeDefinitionSvc         = AtlasEnvelopeSvc,
##                                                       EnvelopeCover                 = 0.5,
##                                                       VolumeEnclosureDiscPositions  = [ 3250., 3450. ],
##                                                       TrackingVolumeCreator         = InDetCylinderVolumeCreator,
##                                                       LayerArrayCreator             = InDetLayerArrayCreator,
##                                                       CheckForRingLayout            = isRingLayout,
##                                                       BuildBoundaryLayers           = TrkDetFlags.InDetBuildMaterialBoundaries(),
##                                                       ReplaceAllJointBoundaries     = TrkDetFlags.InDetBuildJointBoundaries(),
##                                                       OutputLevel                   = TrkDetFlags.InDetBuildingOutputLevel(),
##                                                       ExitVolumeName                = TrkDetFlags.InDetContainerName(),
##                                                       MagneticFieldMode             = TrkDetFlags.MagneticFieldMode())
