# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
## @file TrkDetDescrSvc/python/TrkDetDescrJobProperties.py
## @purpose Python module to hold common flags to configure JobOptions
##

""" TrkDetDescrJobProperties
    Python module to hold storegate keys of InDet objects.

"""

__author__ = "A. Salzburger"
__version__= "$Revision: 1.4 $"
__doc__    = "TrkDetDescrJobProperties"
__all__    = [ "TrkDetDescrJobProperties" ]

# kindly stolen from AthenaCommonFlags from S. Binet and M. Gallas

##-----------------------------------------------------------------------------
## Import

from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer
from AthenaCommon.JobProperties import jobproperties

## definitions
mm = 1

##-----------------------------------------------------------------------------
## 1st step: define JobProperty classes

######################### TRKDET SECTION #######################################
# General TrkDetDescr properties
# Material section
class MaterialSource(JobProperty) :
    """ Material Steering """
    statusOn      = True
    allowedTypes  = ['str']
    allowedValues = ['COOL','Input','None']
    StoredValue   = 'COOL'

class MaterialStoreGateKey(JobProperty) :
    """ Material Steering """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue = '/GLOBAL/TrackingGeo/LayerMaterialV2'
  
class MaterialTagBase(JobProperty) :          
    """ Material Steering """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue = 'AtlasLayerMat_v' 

class MaterialMagicTag(JobProperty) :          
    """ Material Steering """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue = 'DEFAULT' 

class MaterialVersion(JobProperty) :          
    """ Material Steering """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue = 21

class MaterialSubVersion(JobProperty) :          
    """ Material Steering """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue = ''

# Magnetic field section
class MagneticFieldMode(JobProperty) :
    """ Magnetic field Steering """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 6
    
# Magnetic field section
class MagneticFieldSynchronisation(JobProperty) :
    """ Magnetic field Steering """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

# Magnetic field section
class MagneticFieldCallbackEnforced(JobProperty) :
    """ Magnetic field Steering """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

# for the local database
class MaterialDatabaseLocal(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
# for the local database
class MaterialDatabaseLocalPath(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = './'    
    
# for the local database, part 2: the db name
class MaterialDatabaseLocalName(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'mylocaldb.db'

# Material Validation
class MaterialValidation(JobProperty) :
    """Material Validation output """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
# switch to sLHC version
class SLHC_Geometry(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# switch to FastGeoModel geometry version
class XMLFastCustomGeometry(JobProperty) :
    """ Boolean switch to use an XML FastGeoModel custom geometry """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# for the FastGeoModel geometry builder name
class InDetTrackingGeometryBuilderName(JobProperty) :
    """ Name of GeoModel geometry builder """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDetTrackingGeometryBuilder'

# switch to ISF_Fatras custom geometry version
class ISF_FatrasCustomGeometry(JobProperty) :
    """ Boolean switch to a fatras custom geometry """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

# for the ISF_Fatras custom geometry builder name
class ISF_FatrasCustomGeometryBuilderName(JobProperty) :
    """ Name of fatras custom geometry builder """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'CustomGeometryBuilder'

# configuration output level
class ConfigurationOutputLevel(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

# compactify memory by owner stamping
class AssignSurfaceOwnership(JobProperty) :
    """ Owner Steering """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

######################### INDET SECTION ########################################
# Beam Pipe --------------------------------------------------
class BeamPipeFromGeoModel(JobProperty):
    """ Beam Pipe from GeoModel directly """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
    
class BeamPipeEnvelope(JobProperty):
    """ Beam Pipe envelope when using the TopVolume """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.2*mm
    
class BeamPipeOffsetX(JobProperty):
    """ Beam Pipe Offset in X direction """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*mm
    
class BeamPipeOffsetY(JobProperty):
    """ Beam Pipe Offset in Y direction """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.0*mm
            
class BeamPipeRadius(JobProperty):
    """ Beam Pipe Radius """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 33.1*mm

class BeamPipeThickness(JobProperty):
    """ Beam Pipe Thickness"""
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.*mm

class BeamPipeLayerMaterialBinsZ(JobProperty):
    """ Beam Pipe Thickness"""
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class BeamPipeBuildingOutputLevel(JobProperty):
    """ OutputLevel for the Beam Pipe Layer Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

# PIXEL Detector ---------------------------------------------   
class PixelBarrelLayerDynamicExtend(JobProperty):
    """ Dynamic Symmetric Barrel Layer Extend in Z """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PixelBarrelLayerExtend(JobProperty):
    """ Symmetric Barrel Layer Extend in Z """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 420.*mm
         
class PixelBarrelLayerMaterialBinsZ(JobProperty):
    """ Material Binning Pixel Barrel in Z """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class PixelBarrelLayerMaterialBinsPhi(JobProperty):
    """ Material Binning Pixel Barrel in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class PixelBarrelOverlapLocalCoordsCheck(JobProperty):
    """ Use local coordinates to check the overlap in the pixels """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PixelBarrelOverlapFractionLocal1(JobProperty):
    """ Local (1) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class PixelBarrelOverlapFractionLocal2(JobProperty):
    """ Local (2) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1
    
class PixelEndcapLayerDynamicExtend(JobProperty):
    """ Dynamic Pixel Endcap Layer Extend in R estimation """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PixelEndcapLayerInnerR(JobProperty):
    """ Barrel Disc inner R """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 40.*mm

class PixelEndcapLayerOuterR(JobProperty):
    """ Barrel Disc outer R """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 160.*mm
            
class PixelEndcapLayerMaterialBinsR(JobProperty):
    """ Material Binning Pixel Endcap in R """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class PixelEndcapLayerMaterialBinsPhi(JobProperty):
    """ Material Binning Pixel Endcap in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class PixelEndcapOverlapLocalCoordsCheck(JobProperty):
    """ Use local coordinates to check the overlap in the pixels """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class PixelEndcapOverlapFractionLocal1(JobProperty):
    """ Local (1) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class PixelEndcapOverlapFractionLocal2(JobProperty):
    """ Local (2) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class PixelSplitMode(JobProperty):
    """ Configuring in how many steps the pixel detector is built """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0
    
class PixelRingLayout(JobProperty):
    """ The ring layout for ITK """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False
    
class PixelBuildingOutputLevel(JobProperty):
    """ OutputLevel for the Pixel Layer Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3
    
# SCT Detector ---------------------------------------------    
class SCT_BarrelLayerDynamicExtend(JobProperty):
    """ SCT Symmetric Barrel Layer Extend in Z """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class SCT_BarrelLayerExtend(JobProperty):
    """ Symmetric Barrel Layer Extend in Z """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 750.*mm

class SCT_BarrelLayerMaterialBinsZ(JobProperty):
    """ Material SCT Pixel Barrel in Z """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class SCT_BarrelLayerMaterialBinsPhi(JobProperty):
    """ Material Binning SCT Barrel in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class SCT_BarrelOverlapLocalCoordsCheck(JobProperty):
    """ Use local coordinates to check the overlap in the pixels """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class SCT_BarrelOverlapFractionLocal1(JobProperty):
    """ Local (1) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class SCT_BarrelOverlapFractionLocal2(JobProperty):
    """ Local (2) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class SCT_EndcapLayerMaterialBinsR(JobProperty):
    """ Material Binning SCT Endcap in R """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class SCT_EndcapLayerMaterialBinsPhi(JobProperty):
    """ Material Binning SCT Endcap in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class SCT_EndcapLayerDynamicExtend(JobProperty):
    """ Dynamic SCT Endcap Layer Extend in R estimation """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class SCT_EndcapLayerInnerR(JobProperty):
    """ Barrel Disc inner R """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 220.*mm

class SCT_EndcapLayerOuterR(JobProperty):
    """ Barrel Disc outer R """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 580.*mm    
    
class SCT_EndcapLayerDynamicRings(JobProperty):
    """ Barrel Disc outer R """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True    
    
class SCT_EndcapOverlapLocalCoordsCheck(JobProperty):
    """ Use local coordinates to check the overlap in the pixels """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class SCT_EndcapOverlapFractionLocal1(JobProperty):
    """ Local (1) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class SCT_EndcapOverlapFractionLocal2(JobProperty):
    """ Local (2) coordinate fraction where to search for overlaps """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 0.1

class SCT_SplitMode(JobProperty):
    """ Configuring in how many steps the SCT detector is built """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 0


class SCT_BuildingOutputLevel(JobProperty):
    """ OutputLevel for the SCT Layer Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

# TRT Detector ---------------------------------------------    
    
class TRT_BuildStrawLayers(JobProperty):
    """Steering of Geometry building: build TRT straw layers """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class TRT_BarrelLayerMaterialBinsZ(JobProperty):
    """ Material TRT Pixel Barrel in Z """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class TRT_BarrelLayerMaterialBinsPhi(JobProperty):
    """ Material Binning TRT Barrel in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TRT_BarrelModelLayers(JobProperty):
    """ Material Binning TRT Endcap in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 7  
                
class TRT_EndcapLayerMaterialBinsR(JobProperty):
    """ Material Binning TRT Endcap in R """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class TRT_EndcapLayerMaterialBinsPhi(JobProperty):
    """ Material Binning TRT Endcap in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
      
class TRT_EndcapModelLayers(JobProperty):
    """ Material Binning TRT Endcap in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 14  
    
class TRT_BuildingOutputLevel(JobProperty):
    """ OutputLevel for the TRT Layer Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

# General ID stuff

class InDetPassiveLayerThickness(JobProperty):
    """ Material Thicknes ID passive """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 1.*mm

class InDetPassiveLayerMaterialBinsPhi(JobProperty):
    """ Material Binning ID passive in Phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class InDetPassiveLayerMaterialBinsRz(JobProperty):
    """ Material Binning ID passive in Z / eta """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class InDetBuildMaterialBoundaries(JobProperty):
    """ Boolean steering to build material boundary layers """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class InDetBuildJointBoundaries(JobProperty):
    """ Boolean steering to build joint boundary layers """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class InDetContainerName(JobProperty) :
    """Name of the InnerDetector Envelope """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDet::Containers::InnerDetector'


# use the newly developed StagedGeometruBuilder
class InDetStagedGeometryBuilder(JobProperty) :
    """ Boolean switch to a local material database """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = False

class InDetBuildingHelperOutputLevel(JobProperty):
    """ OutputLevel for the InDet Geometry Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3
        
class InDetBuildingOutputLevel(JobProperty):
    """ OutputLevel for the InDet Geometry Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3


######################### CALORIMETER SECTION ##################################

# LAr    
    
class LArUseCaloSurfBuilder(JobProperty):
    """Envelope steering """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True
        
class LArBuildingOutputLevel(JobProperty):
    """ OutputLevel for the LAr Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class LArBarrelEnvelopeCover(JobProperty):
    """ OutputLevel for the LAr Volume Builder """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 5.

class LArEndcapEnvelopeCover(JobProperty):
    """ OutputLevel for the LAr Volume Builder """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 5.

# Tile
class TileUseCaloSurfBuilder(JobProperty):
    """Envelope steering """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class TileBuildingOutputLevel(JobProperty):
    """ OutputLevel for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

class TileBarrelEnvelopeCover(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 5.

class TileBarrelLayerMaterialBinsRz(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
      
class TileBarrelLayerMaterialBinsPhi(JobProperty):
    """ Material Bin in Phi for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
    
class TileGirderLayerMaterialBinsRz(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileGirderLayerMaterialBinsPhi(JobProperty):
    """ Material Bin in Phi for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileFingerLayerMaterialBinsRz(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileFingerLayerMaterialBinsPhi(JobProperty):
    """ Material Bin in Phi for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileExtendedBarrelLayerMaterialBinsRz(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileExtendedBarrelLayerMaterialBinsPhi(JobProperty):
    """ Material Bin in Phi for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileGapLayerMaterialBinsRz(JobProperty):
    """ Material Bin in R/Z for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class TileGapLayerMaterialBinsPhi(JobProperty):
    """ Material Bin in Phi for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

# Calo general
class CaloRecordLayerIndexCaloSampleMap(JobProperty):
    """ OutputLevel for the Tile Volume Builder """
    statusOn     = True
    allowedTypes = ['bool']
    StoredValue  = True

class CaloLayerIndexCaloSampleMapName(JobProperty):
    """ LayerIndex/SampleName map  """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'LayerIndexCaloSampleMap'
    
class CaloEnvelopeCover(JobProperty) :    
    """ Cover for Gap volumes """
    statusOn     = True
    allowedTypes = ['float']
    StoredValue  = 5.
    
class CaloLayerMaterialBinsRz(JobProperty):
    """ Material bins layers in calorimeter in rz """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1
            
class CaloLayerMaterialBinsPhi(JobProperty):
    """ Material bins layers in calorimeter in phi """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 1

class CaloEntryVolumeName(JobProperty) :
    """Name of the Calo Entry volume """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'InDet::Containers::EntryVolume'

class CaloContainerName(JobProperty) :
    """Name of the Calo Envelope """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Calo::Container'
        
    
class CaloBuildingOutputLevel(JobProperty):
    """ OutputLevel for the LAr Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3


######################### MUON SECTION ##################################
                             
# Muon system    
class MuonSystemEntryVolumeName(JobProperty) :
    """Name of the MS Entry volume """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'MuonSpectrometerEntrance'
    
# Muon system    
class MuonSystemContainerName(JobProperty) :
    """Name of the MS container volume """
    statusOn     = True
    allowedTypes = ['str']
    StoredValue  = 'Muon::Containers::MuonSystem'

    
class MuonBuildingOutputLevel(JobProperty):
    """ OutputLevel for the MS Volume Builder """
    statusOn     = True
    allowedTypes = ['int']
    StoredValue  = 3

           
##-----------------------------------------------------------------------------
## 2nd step
## Definition of the InDet flag container
class TrkDetDescrJobProperties(JobPropertyContainer):
    """Container for the Fatras key flags
    """
    pass


##-----------------------------------------------------------------------------
## 3rd step
## adding the container to the general top-level container
jobproperties.add_Container(TrkDetDescrJobProperties)

##-----------------------------------------------------------------------------
## 4th step
## adding Trk single flags to the TrkDetFlags container

# general trk det descr
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialSource)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialVersion)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialSubVersion)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialTagBase)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialMagicTag)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialStoreGateKey)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialDatabaseLocal)          
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialDatabaseLocalPath)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialDatabaseLocalName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MaterialValidation)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MagneticFieldMode)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MagneticFieldSynchronisation)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MagneticFieldCallbackEnforced)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SLHC_Geometry)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(XMLFastCustomGeometry)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetTrackingGeometryBuilderName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(ISF_FatrasCustomGeometry)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(ISF_FatrasCustomGeometryBuilderName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(ConfigurationOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(AssignSurfaceOwnership)
# the sub detector specifications
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeFromGeoModel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeEnvelope)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeOffsetX)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeOffsetY)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeRadius)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeThickness)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeLayerMaterialBinsZ)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(BeamPipeBuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelLayerDynamicExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelLayerExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelLayerMaterialBinsZ)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelOverlapLocalCoordsCheck)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelOverlapFractionLocal1)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBarrelOverlapFractionLocal2)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapLayerDynamicExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapLayerInnerR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapLayerOuterR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapLayerMaterialBinsR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapOverlapLocalCoordsCheck)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapOverlapFractionLocal1)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelEndcapOverlapFractionLocal2)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelRingLayout)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelSplitMode)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(PixelBuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelLayerDynamicExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelLayerExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelLayerMaterialBinsZ)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelOverlapLocalCoordsCheck)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelOverlapFractionLocal1)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BarrelOverlapFractionLocal2)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerDynamicExtend)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerMaterialBinsR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerInnerR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerOuterR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapLayerDynamicRings)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapOverlapLocalCoordsCheck)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapOverlapFractionLocal1)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_EndcapOverlapFractionLocal2)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_SplitMode)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(SCT_BuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_BuildStrawLayers)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_BarrelLayerMaterialBinsZ)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_BarrelLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_BarrelModelLayers)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_EndcapLayerMaterialBinsR)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_EndcapLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_EndcapModelLayers)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TRT_BuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetPassiveLayerThickness)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetPassiveLayerMaterialBinsPhi)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetPassiveLayerMaterialBinsRz)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetBuildMaterialBoundaries)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetBuildJointBoundaries)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetContainerName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetStagedGeometryBuilder)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetBuildingHelperOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(InDetBuildingOutputLevel)
# Calorimeter setting
jobproperties.TrkDetDescrJobProperties.add_JobProperty(LArUseCaloSurfBuilder)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(LArBuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(LArBarrelEnvelopeCover)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(LArEndcapEnvelopeCover)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TileUseCaloSurfBuilder)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TileBuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(TileBarrelEnvelopeCover)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(CaloLayerIndexCaloSampleMapName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(CaloEnvelopeCover)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(CaloEntryVolumeName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(CaloContainerName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(CaloBuildingOutputLevel)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MuonSystemEntryVolumeName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MuonSystemContainerName)
jobproperties.TrkDetDescrJobProperties.add_JobProperty(MuonBuildingOutputLevel)
##-----------------------------------------------------------------------------
## 5th step
## short-cut for lazy people
## carefull: do not select TrkDetDescrJobProperties as a short name as well. 
## otherwise problems with pickle
## Note: you still have to import it:
## >>> from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags = jobproperties.TrkDetDescrJobProperties
