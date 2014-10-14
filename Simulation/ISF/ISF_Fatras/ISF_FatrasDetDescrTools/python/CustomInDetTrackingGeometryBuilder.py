# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# CustomInDetTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__RobustTrackingGeometryBuilder

from AthenaCommon.SystemOfUnits import *

# define the class
class CustomInDetTrackingGeometryBuilder( InDet__RobustTrackingGeometryBuilder ):
    # constructor
    def __init__(self,name = 'InDetTrackingGeometryBuilder',
                      namePrefix = '',
                      setLayerAssociation = True,
                      buildTrtStrawLayers = False):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
       
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.InDetBuildingOutputLevel()
        ToolSvc += BeamPipeBuilder

        print BeamPipeBuilder
                
        # the layer builders
        layerbuilders = []
        binnings      = []
        colors        = []
                        
        # @TODO put DetFlags isOn here, but make sure that this is really when the detector is not built        
        
        createXML = False
        if 'Dummy' in namePrefix:
            createXML = True

        
        from ISF_FatrasDetDescrTools.ISF_FatrasDetDescrToolsConf import iFatras__InputLayerMaterialProvider
        ISF_InputLayerMaterialProvider = iFatras__InputLayerMaterialProvider( name = 'ISF_InputLayerMaterialProvider')
        ISF_InputLayerMaterialProvider.OutputLevel = 1
        ToolSvc += ISF_InputLayerMaterialProvider
        print ISF_InputLayerMaterialProvider
        
        # PIXEL building
        #from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
        from ISF_FatrasDetDescrTools.ISF_FatrasDetDescrToolsConf import iFatras__PlanarDetLayerBuilder
        PixelLayerBuilder = iFatras__PlanarDetLayerBuilder(name=namePrefix+'PixelLayerBuilder')
        PixelLayerBuilder.PixelCase 	       = True
        PixelLayerBuilder.Identification       = 'Pixel'
        PixelLayerBuilder.CheckGeo = False
        PixelLayerBuilder.OutputLevel = 1

        PixelLayerBuilder.InputLayerMaterialProvider = ISF_InputLayerMaterialProvider
        # Pixel barrel specifications
        
        # Noemi's configuration
        
        # Assign custom material
        PixelLayerBuilder.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
        PixelLayerBuilder.BarrelLayerBinsPhi   = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
        PixelLayerBuilder.EndcapLayerBinsR     = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
        PixelLayerBuilder.EndcapLayerBinsPhi   = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
        PixelLayerBuilder.CustomMaterial          = False
        PixelLayerBuilder.CustomMaterialThickness = 0.250
        PixelLayerBuilder.CustomMaterialX0        = 8.333
        PixelLayerBuilder.CustomMaterialL0        = 100
        PixelLayerBuilder.CustomMaterialA         = 14
        PixelLayerBuilder.CustomMaterialZ         = 28.0855
        PixelLayerBuilder.CustomMaterialRho       = 0.00233 
           
        # BARREL
        PixelLayerBuilder.BarrelLayers = 3
        PixelLayerBuilder.LayersZsectors = [ 13, 13, 13]
        PixelLayerBuilder.LayerPhiSectors = [ 22, 38, 52 ]
        PixelLayerBuilder.LayerTilt = [ -20.0, -20.0, -20.0 ] #degree
        PixelLayerBuilder.LayerMinPhi = [ -180.0, -180.0, -180.0 ] #degree
        PixelLayerBuilder.LayerMaxPhi = [ 180.0, 180.0, 180.0 ] #degree
        PixelLayerBuilder.LayerMinZ = [ -400.5, -400.5, -400.5 ]
        PixelLayerBuilder.LayerMaxZ = [ 400.5, 400.5, 400.5 ]
        PixelLayerBuilder.LayerRadius = [ 50.5, 88.5, 122.5]
        PixelLayerBuilder.LayerThickness = [0.250, 0.250, 0.250]
        PixelLayerBuilder.LayerLengthY = [ 60.8, 60.8, 60.8 ]
        PixelLayerBuilder.LayerLengthXmin = [ 16.4 , 16.4 , 16.4]
        PixelLayerBuilder.LayerPitchX = [ 0.010, 0.010, 0.010 ]
        PixelLayerBuilder.LayerPitchY = [ 0.055, 0.055, 0.055 ]
        #PixelLayerBuilder.LayerPitchX = [ 0.05, 0.05, 0.05 ]
        #PixelLayerBuilder.LayerPitchY = [ 0.4, 0.4, 0.4 ]
        PixelLayerBuilder.LayerRotation = [1, 1, 1] #degree
        PixelLayerBuilder.LayerStereo = [0, 0, 0]
        #PixelLayerBuilder.LayerStereoSeparation = [0, 0, 0]
        PixelLayerBuilder.AdditionalLayerRadius = [] #45.5, 83.5, 117.5 ]
        
        # ENDCAPS
        PixelLayerBuilder.EndcapDiscs = 3
        PixelLayerBuilder.DiscPhiSectors = [[48], [48], [48]]
        PixelLayerBuilder.DiscZpos = [-650.0, -580.0, -495.0, 495.0, 580.0, 650.0]
        PixelLayerBuilder.DiscRingMinR = [[88.8], [88.8], [88.8]]
        PixelLayerBuilder.DiscRingMaxR = [[149.6], [149.6], [149.6]]
        PixelLayerBuilder.DiscMinPhi = [[-180.0], [-180.0], [-180.0]]
        PixelLayerBuilder.DiscMaxPhi = [ [180.0],  [180.0],  [180.0]]
        PixelLayerBuilder.DiscThickness = [0.250, 0.250, 0.250]
        PixelLayerBuilder.DiscLengthY = [[60.8], [60.8], [60.8]]
        PixelLayerBuilder.DiscLengthXmin = [[16.4], [16.4], [16.4]]
        PixelLayerBuilder.DiscPitchX = [[0.010], [0.010], [0.010]]
        PixelLayerBuilder.DiscPitchY = [[0.055], [0.055], [0.055]]
        PixelLayerBuilder.DiscSeparation = [[0.500], [0.500], [0.500]]
        #PixelLayerBuilder.DiscStereo = [[0], [0], [0]]
        #PixelLayerBuilder.DiscStereoSeparation = [[0], [0], [0]]
        PixelLayerBuilder.AdditionalDiscZpos = [ -1900. , 1900. ] #-645.0, -575.0, -445.0, 445.0, 575.0, 645.0]
        
        # ENDCAPS
        #PixelLayerBuilder.EndcapDiscs = 7
        #PixelLayerBuilder.DiscPhiSectors = [[48], [48], [48], [48], [48], [48], [48]]
        #PixelLayerBuilder.DiscZpos = [-850, -800, -750, -700, -650.0, -580.0, -450.0, 450.0, 580.0, 650.0, 700, 750, 800, 850]
        #PixelLayerBuilder.DiscRingMinR = [[88.8], [88.8], [88.8], [88.8], [88.8], [88.8], [88.8] ]
        #PixelLayerBuilder.DiscRingMaxR = [[149.6], [149.6], [149.6], [149.6], [149.6], [149.6], [149.6]]
        #PixelLayerBuilder.DiscMinPhi = [[-180.0], [-180.0], [-180.0], [-180.0], [-180.0], [-180.0], [-180.0]]
        #PixelLayerBuilder.DiscMaxPhi = [[180.0],  [180.0],  [180.0], [180.0],  [180.0],  [180.0], [180.0]]
        #PixelLayerBuilder.DiscThickness = [0.250, 0.250, 0.250, 0.250, 0.250, 0.250, 0.250]
        #PixelLayerBuilder.DiscLengthY = [[60.8], [60.8], [60.8], [60.8], [60.8], [60.8], [60.8]]
        #PixelLayerBuilder.DiscLengthXmin = [[16.4], [16.4], [16.4], [16.4], [16.4], [16.4], [16.4]]
        #PixelLayerBuilder.DiscPitchX = [[0.8], [0.8], [0.8], [0.8], [0.8], [0.8], [0.8]]
        #PixelLayerBuilder.DiscPitchY = [[0.8], [0.8], [0.8], [0.8], [0.8], [0.8], [0.8]]
        ##PixelLayerBuilder.DiscStereo = [[0], [0], [0]]
        ##PixelLayerBuilder.DiscStereoSeparation = [[0], [0], [0]]
        #PixelLayerBuilder.AdditionalDiscZpos = [] #-645.0, -575.0, -445.0, 445.0, 575.0, 645.0]

        # BARREL
        #PixelLayerBuilder.BarrelLayers = 1
        #PixelLayerBuilder.LayersZsectors = [ 13 ]
        #PixelLayerBuilder.LayerPhiSectors = [ 22 ]
        #PixelLayerBuilder.LayerTilt = [ -20.0 ] #degree
        #PixelLayerBuilder.LayerMinPhi = [ -180.0 ] #degree
        #PixelLayerBuilder.LayerMaxPhi = [ 180.0 ] #degree
        #PixelLayerBuilder.LayerMinZ = [ -400.5 ]
        #PixelLayerBuilder.LayerMaxZ = [ 400.5 ]
        #PixelLayerBuilder.LayerRadius = [ 50.5 ]
        #PixelLayerBuilder.LayerThickness = [ 0.250 ]
        #PixelLayerBuilder.LayerLengthY = [ 60.8 ]
        #PixelLayerBuilder.LayerLengthXmin = [ 16.4]
        #PixelLayerBuilder.LayerPitchX = [ 0.010 ]
        #PixelLayerBuilder.LayerPitchY = [ 0.055 ]
        #PixelLayerBuilder.LayerRotation = [ 1 ] #degree
        #PixelLayerBuilder.LayerStereo = [ 0 ]
        #PixelLayerBuilder.AdditionalLayerRadius = [] #45.5, 83.5, 117.5 ]

        #PixelLayerBuilder.BarrelLayers = 2
        #PixelLayerBuilder.LayersZsectors = [ 13 , 13]
        #PixelLayerBuilder.LayerPhiSectors = [ 22, 38 ]
        #PixelLayerBuilder.LayerTilt = [ -20.0 , -20.0]
        #PixelLayerBuilder.LayerMinPhi = [ -180.0, -180.0]
        #PixelLayerBuilder.LayerMaxPhi = [ 180.0, 180.0]
        #PixelLayerBuilder.LayerMinZ = [ -400.5, -400.5 ]
        #PixelLayerBuilder.LayerMaxZ = [ 400.5, 400.5]
        #PixelLayerBuilder.LayerRadius = [ 50.5, 88.5]
        #PixelLayerBuilder.LayerThickness = [0.250, 0.250]
        #PixelLayerBuilder.LayerPitchX = [ 0.4, 0.4 ]
        #PixelLayerBuilder.LayerPitchY = [ 0.4, 0.4 ]
        #PixelLayerBuilder.LayerLengthY = [ 60.8, 60.8 ]
        #PixelLayerBuilder.LayerLengthXmin = [ 16.4, 16.4 ]
        #PixelLayerBuilder.LayerRotation = [1, 1 ]
        #PixelLayerBuilder.LayerStereo = [0, 0]
        ##PixelLayerBuilder.LayerStereoSeparation = [0]
        #PixelLayerBuilder.AdditionalLayerRadius = []

        #PixelLayerBuilder.BarrelLayers = 4
        #PixelLayerBuilder.LayersZsectors = [ 13, 13, 13, 13]
        #PixelLayerBuilder.LayerPhiSectors = [ 22, 38, 52 , 80]
        #PixelLayerBuilder.LayerTilt = [ -20.0, -20.0, -20.0 , -20.0] #degree
        #PixelLayerBuilder.LayerMinPhi = [ -180.0, -180.0, -180.0 , -180.0] #degree
        #PixelLayerBuilder.LayerMaxPhi = [ 180.0, 180.0, 180.0 , 180.0] #degree
        #PixelLayerBuilder.LayerMinZ = [ -400.5, -400.5, -400.5 , -400.5]
        #PixelLayerBuilder.LayerMaxZ = [ 400.5, 400.5, 400.5, 400.5]
        #PixelLayerBuilder.LayerRadius = [ 50.5, 85.5, 110.5, 135.5]
        #PixelLayerBuilder.LayerThickness = [0.250, 0.250, 0.250, 0.250]
        #PixelLayerBuilder.LayerLengthY = [ 60.8, 60.8, 60.8, 60.8]
        #PixelLayerBuilder.LayerLengthXmin = [ 16.4 , 16.4 , 16.4, 16.4]
        #PixelLayerBuilder.LayerPitchX = [ 0.4, 0.4, 0.4, 0.4 ]
        #PixelLayerBuilder.LayerPitchY = [ 0.4, 0.4, 0.4, 0.4 ]
        #PixelLayerBuilder.LayerRotation = [1, 1, 1, 1] #degree
        #PixelLayerBuilder.LayerStereo = [0, 0, 0, 0]
        ##PixelLayerBuilder.LayerStereoSeparation = [0, 0, 0]
        #PixelLayerBuilder.AdditionalLayerRadius = [] #45.5, 83.5, 117.5 ]

        # set the layer association
        PixelLayerBuilder.SetLayerAssociation  = setLayerAssociation
        # output level
        TrkDetFlags.InDetBuildingOutputLevel = 0

        PixelLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
        # the binning type of the layers   
        PixelLayerBinning = 2
        # add it to tool service
        
        ToolSvc += PixelLayerBuilder
        print PixelLayerBuilder

        # put them to the caches
        layerbuilders += [ PixelLayerBuilder ]
        binnings      += [ PixelLayerBinning ]
        colors        += [ 3 ]
        
        # SCT building
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__SiLayerBuilder
        SCT_LayerBuilder = InDet__SiLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
        SCT_LayerBuilder.PixelCase                       = False
        SCT_LayerBuilder.Identification                  = 'SCT'
        SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
        # additionall layers - handle with care !
        SCT_LayerBuilder.EndcapAdditionalLayerPositionsZ = [ -2850 , 2850 ] 
        SCT_LayerBuilder.EndcapAdditionalLayerType       = [  0 , 0 ] 
        # SCT barrel specifications
        SCT_LayerBuilder.BarrelLayerBinsZ                = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
        SCT_LayerBuilder.BarrelLayerBinsPhi              = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
        # SCT endcap specifications                          
        SCT_LayerBuilder.EndcapLayerBinsR                = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
        SCT_LayerBuilder.EndcapLayerBinsPhi              = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
        SCT_LayerBuilder.EndcapComplexRingBinning        = TrkDetFlags.SCT_EndcapLayerDynamicRings()
        # set the layer association                   
        SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        
        # output level                                 
        SCT_LayerBuilder.OutputLevel                     = TrkDetFlags.SCT_BuildingOutputLevel()
        # the binning type of the layer     
        SCT_LayerBinning = 2
        # SCT -> ToolSvc                             
        ToolSvc += SCT_LayerBuilder                       
        # put them to the caches
        layerbuilders += [ SCT_LayerBuilder ]
        binnings      += [ SCT_LayerBinning ]
        colors        += [ 4 ]
                                                          
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__TRT_LayerBuilder
        TRT_LayerBuilder = InDet__TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder')
        # TRT barrel specifications
        TRT_LayerBuilder.ModelBarrelLayers  = TrkDetFlags.TRT_BarrelModelLayers()
        TRT_LayerBuilder.BarrelLayerBinsZ   = TrkDetFlags.TRT_BarrelLayerMaterialBinsZ()
        TRT_LayerBuilder.BarrelLayerBinsPhi = TrkDetFlags.TRT_BarrelLayerMaterialBinsPhi()
        # SCT endcap specifications                     
        TRT_LayerBuilder.ModelEndcapLayers  = TrkDetFlags.TRT_EndcapModelLayers()
        TRT_LayerBuilder.EndcapLayerBinsR   = TrkDetFlags.TRT_EndcapLayerMaterialBinsR()
        TRT_LayerBuilder.EndcapLayerBinsPhi = TrkDetFlags.TRT_EndcapLayerMaterialBinsPhi()                
        # set the binning from bi-aequidistant to arbitrary for complex TRT volumes
        TRT_LayerBinning = 1        
        if buildTrtStrawLayers or TrkDetFlags.TRT_BuildStrawLayers() :
           TRT_LayerBinning = 2
           TRT_LayerBuilder.ModelLayersOnly = False
        # output level
        TRT_LayerBuilder.OutputLevel        = TrkDetFlags.TRT_BuildingOutputLevel()
        # TRT -> ToolSvc                      
        ToolSvc += TRT_LayerBuilder
        # put them to the caches
        layerbuilders += [ TRT_LayerBuilder ]
        binnings      += [ TRT_LayerBinning ]
        colors        += [ 5 ]
        
        
        # helpers for the InDetTrackingGeometry Builder : layer array creator

        if createXML:
            from ISF_FatrasDetDescrTools import XMLDumper
            InDetXMLDumper = XMLDumper.XMLDumper(name = 'InDetXMLDumper', fileName = 'test.xml')
            InDetXMLDumper.dump(PixelLayerBuilder, SCT_LayerBuilder, TRT_LayerBuilder)
            InDetXMLDumper.closeFile()
            
        # layer && tracking volume array creator

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
        InDet__RobustTrackingGeometryBuilder.__init__(self,namePrefix+name,\
                                                      BeamPipeBuilder   = BeamPipeBuilder,\
                                                      LayerBuilders     = layerbuilders,
                                                      LayerBinningType  = binnings,
                                                      ColorCodes        = colors,
                                                      BarrelEntryLayers = [ 2, 2, 2 ],
                                                      EndcapEntryLayers = [ 1, 0, 1 ],
                                                      EnvelopeDefinitionSvc = AtlasEnvelopeSvc,
                                                      VolumeEnclosureDiscPositions = [ 3000., 3450. ],
                                                      TrackingVolumeCreator = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator = InDetLayerArrayCreator,
                                                      BuildBoundaryLayers       = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      ReplaceAllJointBoundaries = TrkDetFlags.InDetBuildJointBoundaries(),
                                                      OutputLevel = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode = TrkDetFlags.MagneticFieldMode())


