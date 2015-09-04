# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

######################################################
# CustomInDetTrackingGeometry module
#
# it inherits from RobustTrackingGeometryBuilderConf and performs 
# standard configuration
#
######################################################

# import the Extrapolator configurable
from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__StagedTrackingGeometryBuilder

from AthenaCommon.SystemOfUnits import *

# define the class
class CustomInDetTrackingGeometryBuilder( InDet__StagedTrackingGeometryBuilder ):
    # constructor
    def __init__(self, name          = 'InDetTrackingGeometryBuilder',
                 namePrefix          = '',
                 setLayerAssociation = True,
                 buildTrtStrawLayers = False,
                 buildCustomPixel    = False,
                 buildCustomSCT      = False,
                 buildCustomTRT      = False,
                 createXML           = True,
                 dictionaryFileName  = 'InDetTrackingGeometry.xml'
                 ):

        # get the ToolSvc
        from AthenaCommon.AppMgr import ToolSvc, ServiceMgr           
        
        # the Detector flags to be imported
        from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags

        from ISF_FatrasDetDescrTools.ISF_FatrasDetDescrToolsConf import iFatras__InputLayerMaterialProvider
        ISF_InputLayerMaterialProvider = iFatras__InputLayerMaterialProvider( name = namePrefix+'ISF_InputLayerMaterialProvider')
        #ISF_InputLayerMaterialProvider.OutputLevel = 1
        ToolSvc += ISF_InputLayerMaterialProvider
        print ISF_InputLayerMaterialProvider
        
        # beampipe        
        from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__BeamPipeBuilder
        BeamPipeBuilder = InDet__BeamPipeBuilder(name=namePrefix+'BeamPipeBuilder')
        BeamPipeBuilder.BeamPipeRadius 	        = TrkDetFlags.BeamPipeRadius()
        BeamPipeBuilder.BeamPipeThickness       = TrkDetFlags.BeamPipeThickness() 
        BeamPipeBuilder.BeamPipeMaterialBinsZ   = TrkDetFlags.BeamPipeLayerMaterialBinsZ()
        BeamPipeBuilder.OutputLevel             = TrkDetFlags.InDetBuildingOutputLevel()
        ToolSvc += BeamPipeBuilder
        print BeamPipeBuilder
        
        # the layer providers 
        from TrkDetDescrTools.TrkDetDescrToolsConf import Trk__LayerProvider 
        BeamPipeProvider = Trk__LayerProvider(name=namePrefix+'BeamPipeProvider') 
        BeamPipeProvider.LayerBuilder = BeamPipeBuilder 
        ToolSvc += BeamPipeProvider 
        
        # binning of the beam pipe
        BeamPipeBinning = 2
        
        # the layer builders
        layerProviders  = [ BeamPipeProvider ] 
        binningsCenter  = [ BeamPipeBinning ] 
        binningsEndcap  = [ BeamPipeBinning ]
        colors          = [ 2 ]
                        
        IBLLayerBuilder = None
        PixelLayerBuilder = None
        if buildCustomPixel:
            from ISF_FatrasDetDescrTools.ISF_FatrasDetDescrToolsConf import iFatras__PlanarDetLayerBuilder
            ## EXT IBL building
            #IBLLayerBuilder = iFatras__PlanarDetLayerBuilder(name=namePrefix+'IBLLayerBuilder')
            #IBLLayerBuilder.PixelCase 	                = True
            #IBLLayerBuilder.Identification              = 'Pixel'
            #IBLLayerBuilder.InputLayerMaterialProvider  = ISF_InputLayerMaterialProvider
        
            ## Assign custom material
            #IBLLayerBuilder.BarrelLayerBinsZ        = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
            #IBLLayerBuilder.BarrelLayerBinsPhi      = TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
            #IBLLayerBuilder.EndcapLayerBinsR        = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
            #IBLLayerBuilder.EndcapLayerBinsPhi      = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
            #IBLLayerBuilder.CustomMaterial          = False
            #IBLLayerBuilder.CustomMaterialThickness = 0.250
            #IBLLayerBuilder.CustomMaterialX0        = 8.333
            #IBLLayerBuilder.CustomMaterialL0        = 100
            #IBLLayerBuilder.CustomMaterialA         = 14
            #IBLLayerBuilder.CustomMaterialZ         = 28.0855
            #IBLLayerBuilder.CustomMaterialRho       = 0.00233 
            
            ## INNER PIXEL BARREL
            #INPB_NR = 1
            #IBLLayerBuilder.BarrelLayers = INPB_NR
            #IBLLayerBuilder.LayersZsectors = [40]  # [58, 58]
            #IBLLayerBuilder.LayerPhiSectors = [16]
            #IBLLayerBuilder.LayerTilt = [-14.0 ] * INPB_NR   #degrees
            #IBLLayerBuilder.LayerMinPhi = [-180.0 ] * INPB_NR   #degrees
            #IBLLayerBuilder.LayerMaxPhi = [ 180.0 ] * INPB_NR   #degrees
            #IBLLayerBuilder.LayerMinZ = [-40.5 * z/2. for z in IBLLayerBuilder.LayersZsectors] #mm
            #IBLLayerBuilder.LayerMaxZ = [40.5 * z/2. for z in IBLLayerBuilder.LayersZsectors] #mm
            #IBLLayerBuilder.LayerRadius = [39.]
            #IBLLayerBuilder.LayerThickness = [0.150 + 0.150 + 0.182] * INPB_NR   # chip + sensor + hybrid   # CHECK HYBRID MATERIAL
            #IBLLayerBuilder.LayerLengthY = [ 40.5 ]
            #IBLLayerBuilder.LayerLengthXmin = [ 16.81 ]
            #IBLLayerBuilder.LayerPitchX = [0.050] * INPB_NR
            #IBLLayerBuilder.LayerPitchY = [0.050] * INPB_NR
            #IBLLayerBuilder.LayerRotation = [0.] * INPB_NR
            #IBLLayerBuilder.AdditionalLayerRadius = []
        
            #IBLLayerBuilder.EndcapDiscs = 0

            ## set the layer association
            #IBLLayerBuilder.SetLayerAssociation  = setLayerAssociation
            #IBLLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
            
            ## the binning type of the layers   
            #IBLLayerBinning = 2

            ## add it to tool service
            #ToolSvc += IBLLayerBuilder
            #print IBLLayerBuilder

            ## the layer providers 
            #IBLLayerProvider = Trk__LayerProvider(name=namePrefix+'IBLLayerProvider') 
            #IBLLayerProvider.LayerBuilder = IBLLayerBuilder 
            #ToolSvc += IBLLayerProvider 

            ## put them to the caches
            #layerProviders  += [ IBLLayerProvider ] 
            #binningsCenter  += [ IBLLayerBinning ] 
            #binningsEndcap  += [ IBLLayerBinning ] 
            #colors          += [ 3 ]

            # PIXEL building
            PixelLayerBuilder = iFatras__PlanarDetLayerBuilder(name=namePrefix+'PixelLayerBuilder')
            PixelLayerBuilder.PixelCase 	       = True
            PixelLayerBuilder.Identification       = 'Pixel'
            
            PixelLayerBuilder.InputLayerMaterialProvider = ISF_InputLayerMaterialProvider
        
            # Pixel barrel specifications
        
            # Noemi's configuration
        
            # Assign custom material
            PixelLayerBuilder.BarrelLayerBinsZ     = TrkDetFlags.PixelBarrelLayerMaterialBinsZ()
            PixelLayerBuilder.BarrelLayerBinsPhi   = 100 #TrkDetFlags.PixelBarrelLayerMaterialBinsPhi()
            PixelLayerBuilder.EndcapLayerBinsR     = TrkDetFlags.PixelEndcapLayerMaterialBinsR()
            PixelLayerBuilder.EndcapLayerBinsPhi   = TrkDetFlags.PixelEndcapLayerMaterialBinsPhi()
            PixelLayerBuilder.CustomMaterial          = True
            PixelLayerBuilder.CustomMaterialThickness = 0.250
            PixelLayerBuilder.CustomMaterialX0        = 8.333
            PixelLayerBuilder.CustomMaterialL0        = 100
            PixelLayerBuilder.CustomMaterialA         = 14
            PixelLayerBuilder.CustomMaterialZ         = 28.0855
            PixelLayerBuilder.CustomMaterialRho       = 0.00233 
                        
            ## BARREL
            PixelLayerBuilder.BarrelLayers = 3
            if IBLLayerBuilder:
                PixelLayerBuilder.LayerOffset = IBLLayerBuilder.BarrelLayers
            PixelLayerBuilder.LayersZsectors = [ 13, 13, 13]
            PixelLayerBuilder.LayerPhiSectors = [ 22, 38, 52 ]
            PixelLayerBuilder.LayerTilt = [ -20.0, -20.0, -20.0 ] #degree
            PixelLayerBuilder.LayerPhiOffset = [ 0., 0., 360./52.*0.4 ] #degree
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
            #PixelLayerBuilder.LayerPitchY = [ 6.08, 6.08, 6.08 ]
            #PixelLayerBuilder.LayerPitchX = [ 1.64, 1.64, 1.64 ]
            PixelLayerBuilder.LayerRotation = [1., 1., 1.] #degree
            PixelLayerBuilder.LayerSCTlike = False
            PixelLayerBuilder.AdditionalLayerRadius = [] #45.5, 83.5, 117.5 ]

            # ENDCAPS
            PixelLayerBuilder.EndcapDiscs = 3
            if IBLLayerBuilder:
                PixelLayerBuilder.DiscOffset = IBLLayerBuilder.EndcapDiscs
            PixelLayerBuilder.DiscPhiSectors = [[48], [48], [48]]
            PixelLayerBuilder.DiscZpos = [-649.0, -579.0, -493.5, 496.5, 581.0, 651.0]
            PixelLayerBuilder.DiscRingMinR = [[88.8], [88.8], [88.8]]
            PixelLayerBuilder.DiscRingMaxR = [[149.6], [149.6], [149.6]]
            PixelLayerBuilder.DiscMinPhi = [[-180.0], [-180.0], [-180.0]]
            PixelLayerBuilder.DiscMaxPhi = [ [180.0],  [180.0],  [180.0]]
            PixelLayerBuilder.DiscThickness = [0.250, 0.250, 0.250]
            PixelLayerBuilder.DiscLengthY = [[60.8], [60.8], [60.8]]
            PixelLayerBuilder.DiscLengthXmin = [[16.4], [16.4], [16.4]]
            PixelLayerBuilder.DiscPitchX = [[0.010], [0.010], [0.010]]
            PixelLayerBuilder.DiscPitchY = [[0.055], [0.055], [0.055]]
            #PixelLayerBuilder.DiscPitchY = [ [6.08], [6.08], [6.08] ]
            #PixelLayerBuilder.DiscPitchX = [ [1.64], [1.64], [1.64] ]
            PixelLayerBuilder.DiscSeparation = [[4.], [4.], [4.]]
            PixelLayerBuilder.AdditionalDiscZpos = [ -1900. , 1900. ] #-645.0, -575.0, -445.0, 445.0, 575.0, 645.0]

            #ENDCAPS
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
            #PixelLayerBuilder.AdditionalLayerRadius = [ 45.5 ] #, 83.5, 117.5 ]

            #PixelLayerBuilder.BarrelLayers = 0
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
            #TrkDetFlags.InDetBuildingOutputLevel = 0

            PixelLayerBuilder.OutputLevel         = TrkDetFlags.PixelBuildingOutputLevel()
            # the binning type of the layers   
            PixelLayerBinning = 2
            # add it to tool service

            ToolSvc += PixelLayerBuilder
            print PixelLayerBuilder

            # the layer providers 
            PixelLayerProvider = Trk__LayerProvider(name=namePrefix+'PixelLayerProvider') 
            PixelLayerProvider.LayerBuilder = PixelLayerBuilder 
            ToolSvc += PixelLayerProvider 

            # put them to the caches
            layerProviders  += [ PixelLayerProvider ] 
            binningsCenter  += [ PixelLayerBinning ] 
            binningsEndcap  += [ PixelLayerBinning ] 
            colors          += [ 3 ]

        
        SCT_LayerBuilder = None
        if buildCustomSCT:
            # SCT building
            from ISF_FatrasDetDescrTools.ISF_FatrasDetDescrToolsConf import iFatras__PlanarDetLayerBuilder
            SCT_LayerBuilder = iFatras__PlanarDetLayerBuilder(name=namePrefix+'SCT_LayerBuilder')
            SCT_LayerBuilder.PixelCase                       = False
            SCT_LayerBuilder.Identification                  = 'SCT'
            SCT_LayerBuilder.InputLayerMaterialProvider      = ISF_InputLayerMaterialProvider  
            SCT_LayerBuilder.SiDetManagerLocation            = 'SCT'
            # SCT barrel specifications
            SCT_LayerBuilder.BarrelLayerBinsZ                = TrkDetFlags.SCT_BarrelLayerMaterialBinsZ()
            SCT_LayerBuilder.BarrelLayerBinsPhi              = TrkDetFlags.SCT_BarrelLayerMaterialBinsPhi()
            # SCT endcap specifications                          
            SCT_LayerBuilder.EndcapLayerBinsR                = TrkDetFlags.SCT_EndcapLayerMaterialBinsR()
            SCT_LayerBuilder.EndcapLayerBinsPhi              = TrkDetFlags.SCT_EndcapLayerMaterialBinsPhi()
            SCT_LayerBuilder.CustomMaterial                  = False
            SCT_LayerBuilder.CustomMaterialThickness         = 0.250
            SCT_LayerBuilder.CustomMaterialX0                = 8.333
            SCT_LayerBuilder.CustomMaterialL0                = 100
            SCT_LayerBuilder.CustomMaterialA                 = 14
            SCT_LayerBuilder.CustomMaterialZ                 = 28.0855
            SCT_LayerBuilder.CustomMaterialRho               = 0.00233 
            SCT_LayerBuilder.DetElementMapName               = "SCT_IdHashDetElementMap"

            SCT_LayerBuilder.BarrelLayers = 4
            SCT_LayerBuilder.LayerSCTlike = True
            #SCT_LayerBuilder.LayerInvertOtherSurface = True
            SCT_LayerBuilder.LayersZsectors = [12, 12, 12, 12]
            SCT_LayerBuilder.LayerPhiSectors = [32, 40, 48, 56]
            SCT_LayerBuilder.LayerPhiOffset = [(-360./phi*0.2+180./phi) for phi in SCT_LayerBuilder.LayerPhiSectors]   #degrees
            SCT_LayerBuilder.LayerTilt = [11.0, 11.0, 11.25, 11.25] #degree
            SCT_LayerBuilder.LayerMinPhi = [ -180.0, -180.0, -180.0, -180.0] #degree
            SCT_LayerBuilder.LayerMaxPhi = [ 180.0, 180.0, 180.0, 180.0] #degree
            SCT_LayerBuilder.LayerMinZ = [ -742.095, -742.095, -742.095, -742.095]
            SCT_LayerBuilder.LayerMaxZ = [ 742.095, 742.095, 742.095, 742.095]
            SCT_LayerBuilder.LayerRadius = [ 299., 371., 443., 514.]
            SCT_LayerBuilder.LayerRSeparation = [ -3., -3., -3., -3.]
            SCT_LayerBuilder.LayerThickness =  [0.2850, 0.2850, 0.2850, 0.2850]
            SCT_LayerBuilder.LayerLengthY = [ 126.09, 126.09, 126.09, 126.09]
            SCT_LayerBuilder.LayerLengthXmin = [ 61.44 , 61.44 , 61.44 , 61.44]
            SCT_LayerBuilder.LayerPitchX = [ 0.080, 0.080, 0.080, 0.080 ]
            #SCT_LayerBuilder.LayerPitchX = [ 6.144 , 6.144 , 6.144 , 6.144]
            SCT_LayerBuilder.LayerPitchY = [ 126.09, 126.09, 126.09, 126.09]
            SCT_LayerBuilder.LayerRotation = [0., 0., 0., 0.] #degree
            SCT_LayerBuilder.LayerStereo = [-1.15, -1.15, -1.15, -1.15]
            SCT_LayerBuilder.LayerStereoSeparation = [1., 1., 1., 1.]
            SCT_LayerBuilder.AdditionalLayerRadius = []
      
            SCT_LayerBuilder.EndcapDiscs = 9
            SCT_LayerBuilder.DiscSCTlike = True
            #SCT_LayerBuilder.DiscSCTlike = False
            # discs are ordered from the smaller z to the bigger one
            # from the smallest ring to the biggest
            SCT_LayerBuilder.DiscPhiSectors = [[40, 52], [40, 40, 52], [40, 40, 52], [40, 40, 52], [40, 40, 52], [40, 40, 52], [40, 52], [40, 52], [48]]
            #SCT_LayerBuilder.DiscZpos = [-2735.45, -2520.25, -2130.45, -1786.65, -1415.95, -1315.15, -1106.75, -949.25, -869.05, 
            #                              869.05, 949.25, 1106.75, 1315.15, 1415.95, 1786.65, 2130.45, 2520.25, 2735.45]
            SCT_LayerBuilder.DiscZpos = [-2742.45, -2511.25, -2123.45, -1778.55, -1406.95, -1307.15, -1098.75, -941.25, -861.05, 
                                          860.05, 940.25, 1097.75, 1306.15, 1405.95, 1777.65, 2121.45, 2511.25, 2741.45]
            SCT_LayerBuilder.DiscRingMinR = [[337.6, 438.77], [275.0, 337.6, 438.77], [275.0, 337.6, 438.77], [275.0, 337.6, 438.77], [275.0, 337.6, 438.77], [275.0, 337.6, 438.77], [337.6, 438.77], [402.8, 438.77], [438.77]]
            SCT_LayerBuilder.DiscRingMaxR = [[455.3, 560.0], [334.1, 455.3, 560.0], [334.1, 455.3, 560.0], [334.1, 455.3, 560.0], [334.1, 455.3, 560.0], [334.1, 455.3, 560.0], [455.3, 560.0], [455.3, 560.0], [560.0]]
            SCT_LayerBuilder.DiscMinPhi = [[-180.0, -180.0], [-180.0, -180.0, -180], [-180.0, -180.0, -180], [-180.0, -180.0, -180], [-180.0, -180.0, -180], [-180.0, -180.0, -180], [-180.0, -180.0], [-180.0, -180.0], [-180.0]]
            SCT_LayerBuilder.DiscMaxPhi = [[180.0, 180.0], [180.0, 180.0, 180], [180.0, 180.0, 180], [180.0, 180.0, 180], [180.0, 180.0, 180], [180.0, 180.0, 180], [180.0, 180.0], [180.0, 180.0], [180.0]]
            SCT_LayerBuilder.DiscThickness = [0.2850, 0.2850, 0.2850, 0.2850, 0.2850, 0.2850, 0.2850, 0.2850, 0.2850]
            #SCT_LayerBuilder.DiscLengthY = [[117.7, 117.7], [117.7, 117.7, 117.7], [117.7, 117.7, 117.7], [117.7, 117.7, 117.7], [117.7, 117.7, 117.7], [117.7, 117.7, 117.7], [117.7, 117.7], [117.7, 117.7], [117.7]]
            #SCT_LayerBuilder.DiscLengthXmin = [[53.78, 53.78], [53.78, 53.78, 53.78], [53.78, 53.78, 53.78], [53.78, 53.78, 53.78], [53.78, 53.78, 53.78], [53.78, 53.78, 53.78], [53.78, 53.78], [53.78, 53.78], [53.78]]
            #SCT_LayerBuilder.DiscLengthXmax = [[72.53, 72.53], [72.53, 72.53, 72.53], [72.53, 72.53, 72.53], [72.53, 72.53, 72.53], [72.53, 72.53, 72.53], [72.53, 72.53, 72.53], [72.53, 72.53], [72.53, 72.53], [72.53]]
            #SCT_LayerBuilder.DiscLengthXmin = [[83.78, 83.78], [83.78, 83.78, 83.78], [83.78, 83.78, 83.78], [83.78, 83.78, 83.78], [83.78, 83.78, 83.78], [83.78, 83.78, 83.78], [83.78, 83.78], [83.78, 83.78], [83.78]]
            #SCT_LayerBuilder.DiscLengthXmax = [[92.53, 92.53], [92.53, 92.53, 92.53], [92.53, 92.53, 92.53], [92.53, 92.53, 92.53], [92.53, 92.53, 92.53], [92.53, 92.53, 92.53], [92.53, 92.53], [92.53, 92.53], [92.53]]
            SCT_LayerBuilder.DiscLengthXmin = [[53.784, 54.490], [43.810, 53.784, 54.490], [43.810, 53.784, 54.490], [43.810, 53.784, 54.490], [43.810, 53.784, 54.490], [43.810, 53.784, 54.490], [53.784, 54.490], [64.174, 54.490], [54.490]]
            SCT_LayerBuilder.DiscLengthXmax = [[72.534, 69.548], [53.226, 72.534, 69.548], [53.226, 72.534, 69.548], [53.226, 72.534, 69.548], [53.226, 72.534, 69.548], [53.226, 72.534, 69.548], [72.534, 69.548], [72.534, 69.548], [69.548]]
            SCT_LayerBuilder.DiscLengthY = [[117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [117.7, 121.236], [52.474, 121.236], [121.236]]
            #SCT_LayerBuilder.DiscPitchX = [[0.080, 0.080], [0.080, 0.080, 0.080], [0.080, 0.080, 0.080], [0.080, 0.080, 0.080], [0.080, 0.080, 0.080], [0.080, 0.080, 0.080], [0.080, 0.080], [0.080, 0.080], [0.080]]
            #SCT_LayerBuilder.DiscPitchY = [[117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [117.7, 121.236], [52.474, 121.236], [121.236]]
            SCT_LayerBuilder.DiscPitchX = [[207.0e-06, 161.5e-06], [207.0e-06, 207.0e-06, 161.5e-06], [207.0e-06, 207.0e-06, 161.5e-06], [207.0e-06, 207.0e-06, 161.5e-06], [207.0e-06, 207.0e-06, 161.5e-06], [207.0e-06, 207.0e-06, 161.5e-06], [207.0e-06, 161.5e-06], [207.0e-06, 161.5e-06], [161.5e-06]] #in rad for DiscSurfaces
            SCT_LayerBuilder.DiscPitchY = [[117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [117.7, 121.236], [52.474, 121.236], [121.236]]
            #SCT_LayerBuilder.DiscPitchX = [[5.3784, 5.4490], [4.3810, 5.3784, 5.4490], [4.3810, 5.3784, 5.4490], [4.3810, 5.3784, 5.4490], [4.3810, 5.3784, 5.4490], [4.3810, 5.3784, 5.4490], [5.3784, 5.4490], [6.4174, 5.4490], [5.4490]]
            #SCT_LayerBuilder.DiscPitchY = [[117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.700, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [59.100, 117.7, 121.236], [117.7, 121.236], [52.474, 121.236], [121.236]]
            SCT_LayerBuilder.DiscSeparation = [[5.3, 5.3], [5.3, 5.3, 5.3], [5.1, 5.1, 5.1], [5.3, 5.3, 5.3], [5.3, 5.3, 5.3], [5.2, 5.2, 5.2], [5.2, 5.2],  [5.2, 5.2], [5.1]]
            SCT_LayerBuilder.RingDisplacement = [[15.1, 15.1], [15.1, 15.1, 15.1], [15.2, 15.2, 15.2], [15.1, 15.1, 15.1], [15.1, 15.1, 15.1], [15.15, 15.15, 15.15], [15.15, 15.15],  [15.15, 15.15], [0.]]
            SCT_LayerBuilder.DiscStereo = [[2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3, 2.3], [2.3, 2.3], [2.3, 2.3], [2.3]]
            #SCT_LayerBuilder.DiscStereo = [[10.0, 10.0], [10., 10., 10.], [10., 10., 10.], [10., 10., 10.], [10., 10., 10.], [10., 10., 10.], [10., 10., 10.], [10., 10.], [10., 10.], [10.]]
            SCT_LayerBuilder.DiscStereoSeparation = [[1., 1.], [1., 1., 1.], [1., 1., 1.], [1., 1., 1.], [1., 1., 1.], [1., 1., 1.], [1., 1.], [1., 1.], [1.]]

            #SCT_LayerBuilder.EndcapDiscs = 1
            #SCT_LayerBuilder.DiscSCTlike = True
            ##SCT_LayerBuilder.DiscSCTlike = False
            ## discs are ordered from the smaller z to the bigger one
            ## from the smallest ring to the biggest
            #SCT_LayerBuilder.DiscPhiSectors = [[16]]
            #SCT_LayerBuilder.DiscZpos = [-1000, 1000]
            #SCT_LayerBuilder.DiscRingMinR = [[337.6]]
            #SCT_LayerBuilder.DiscRingMaxR = [[455.3]]
            #SCT_LayerBuilder.DiscMinPhi = [[-180.0]]
            #SCT_LayerBuilder.DiscMaxPhi = [[180.0]]
            #SCT_LayerBuilder.DiscThickness = [0.2850]
            #SCT_LayerBuilder.DiscLengthXmin = [[40]]
            #SCT_LayerBuilder.DiscLengthXmax = [[80]]
            #SCT_LayerBuilder.DiscLengthY = [[117.70]]
            #SCT_LayerBuilder.DiscPitchX = [[0.080]]
            #SCT_LayerBuilder.DiscPitchY = [[117.70]]
            #SCT_LayerBuilder.DiscStereo = [[90.]]
            #SCT_LayerBuilder.DiscStereoSeparation = [[1.]]
            
            #SCT_LayerBuilder.EndcapDiscs = 1
            #SCT_LayerBuilder.DiscSCTlike = True
            ##SCT_LayerBuilder.DiscSCTlike = False
            ## discs are ordered from the smaller z to the bigger one
            ## from the smallest ring to the biggest
            #SCT_LayerBuilder.DiscPhiSectors = [[16]]
            #SCT_LayerBuilder.DiscZpos = [-900., 900.]
            #SCT_LayerBuilder.DiscRingMinR = [[337.6]]
            #SCT_LayerBuilder.DiscRingMaxR = [[455.3]]
            #SCT_LayerBuilder.DiscMinPhi = [[-180.0]]
            #SCT_LayerBuilder.DiscMaxPhi = [[180.0]]
            #SCT_LayerBuilder.DiscThickness = [0.2850]
            #SCT_LayerBuilder.DiscLengthXmin = [[53.784]]
            #SCT_LayerBuilder.DiscLengthXmax = [[72.534]]
            #SCT_LayerBuilder.DiscLengthY = [[117.700]]
            #SCT_LayerBuilder.DiscPitchX = [[207.0e-06]]
            #SCT_LayerBuilder.DiscPitchX = [[0.0079739*2.]]
            #SCT_LayerBuilder.DiscPitchY = [[117.700]]
            #SCT_LayerBuilder.DiscStereo = [[10.]]
            #SCT_LayerBuilder.DiscStereoSeparation = [[1.]]

            SCT_LayerBuilder.AdditionalDiscZpos = [ -2850 , 2850 ] 
            
            # set the layer association                   
            SCT_LayerBuilder.SetLayerAssociation             = setLayerAssociation        

            # output level                                 
            SCT_LayerBuilder.OutputLevel                     = TrkDetFlags.SCT_BuildingOutputLevel()
           
            # the binning type of the layer     
            SCT_LayerBinning = 2
            # SCT -> ToolSvc                             
            ToolSvc += SCT_LayerBuilder  
            print SCT_LayerBuilder

            # the layer providers 
            SCT_LayerProvider = Trk__LayerProvider(name=namePrefix+'SCT_LayerProvider') 
            SCT_LayerProvider.LayerBuilder = SCT_LayerBuilder 
            ToolSvc += SCT_LayerProvider 
            
            # put them to the caches
            layerProviders  += [ SCT_LayerProvider ] 
            binningsCenter  += [ SCT_LayerBinning ] 
            binningsEndcap  += [ SCT_LayerBinning ] 
            colors          += [ 4 ]
                        
        TRT_LayerBuilder = None
        if buildCustomTRT:
            print "No TRT for the moment"
            # TRT building
        #from InDetTrackingGeometry.InDetTrackingGeometryConf import InDet__TRT_LayerBuilder
            #TRT_LayerBuilder = InDet__TRT_LayerBuilder(name=namePrefix+'TRT_LayerBuilder')
            ## TRT barrel specifications
            #TRT_LayerBuilder.ModelBarrelLayers  = TrkDetFlags.TRT_BarrelModelLayers()
            #TRT_LayerBuilder.BarrelLayerBinsZ   = TrkDetFlags.TRT_BarrelLayerMaterialBinsZ()
            #TRT_LayerBuilder.BarrelLayerBinsPhi = TrkDetFlags.TRT_BarrelLayerMaterialBinsPhi()
            ## SCT endcap specifications                     
            #TRT_LayerBuilder.ModelEndcapLayers  = TrkDetFlags.TRT_EndcapModelLayers()
            #TRT_LayerBuilder.EndcapLayerBinsR   = TrkDetFlags.TRT_EndcapLayerMaterialBinsR()
            #TRT_LayerBuilder.EndcapLayerBinsPhi = TrkDetFlags.TRT_EndcapLayerMaterialBinsPhi()                
            ## set the binning from bi-aequidistant to arbitrary for complex TRT volumes
            #TRT_LayerBinning = 1        
            #if buildTrtStrawLayers or TrkDetFlags.TRT_BuildStrawLayers() :
               #TRT_LayerBinning = 2
               #TRT_LayerBuilder.ModelLayersOnly = False
            ## output level
            #TRT_LayerBuilder.OutputLevel        = TrkDetFlags.TRT_BuildingOutputLevel()
            ## TRT -> ToolSvc                      
            #ToolSvc += TRT_LayerBuilder
            ## put them to the caches
            #layerbuilders += [ TRT_LayerBuilder ]
            #binnings      += [ TRT_LayerBinning ]
            #colors        += [ 5 ]
        
        
        # helpers for the InDetTrackingGeometry Builder : layer array creator

        if createXML:
            from ISF_FatrasDetDescrTools import XMLDumper
            InDetXMLDumper = XMLDumper.XMLDumper(name = 'InDetXMLDumper', fileName = dictionaryFileName)
            if IBLLayerBuilder:
                InDetXMLDumper.addPixelBuilder(IBLLayerBuilder)
            if PixelLayerBuilder:
                InDetXMLDumper.addPixelBuilder(PixelLayerBuilder)
            if SCT_LayerBuilder:
                InDetXMLDumper.addSCTBuilder(SCT_LayerBuilder)
            if TRT_LayerBuilder:
                InDetXMLDumper.addTRTBuilder(TRT_LayerBuilder)
            InDetXMLDumper.dump()
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
        InDet__StagedTrackingGeometryBuilder.__init__(self,namePrefix+name,
                                                      LayerBuilders                 = layerProviders,
                                                      LayerBinningTypeCenter        = binningsCenter, 
                                                      LayerBinningTypeEndcap        = binningsEndcap,                                                       
                                                      ColorCodes                    = colors,
                                                      EnvelopeDefinitionSvc         = AtlasEnvelopeSvc,
                                                      VolumeEnclosureDiscPositions  = [ 3000., 3450. ],
                                                      TrackingVolumeCreator         = InDetCylinderVolumeCreator,
                                                      LayerArrayCreator             = InDetLayerArrayCreator,
                                                      CheckForRingLayout            = False, 
                                                      BuildBoundaryLayers           = TrkDetFlags.InDetBuildMaterialBoundaries(),
                                                      ReplaceAllJointBoundaries     = TrkDetFlags.InDetBuildJointBoundaries(),
                                                      OutputLevel                   = TrkDetFlags.InDetBuildingOutputLevel(),
                                                      ExitVolumeName                = TrkDetFlags.InDetContainerName(),
                                                      MagneticFieldMode             = TrkDetFlags.MagneticFieldMode())


