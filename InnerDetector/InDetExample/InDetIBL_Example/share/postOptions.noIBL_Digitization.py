##########################################
# Comments
# Requested for IBL Digitization studies
##########################################

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

###########################################################
## Add Pixel and SCT SiHitCollections to outputStream, if required
outStreams = AlgSequence( "Streams" )
if hasattr(outStreams, 'StreamRDO'):
   outStream = outStreams.StreamRDO
elif hasattr(outStreams, 'Stream1'):
   # Legacy only
   # this option will be removed after 15.6.X
   outStream = outStreams.Stream1
if 'outStream' in dir():
    outStream.ItemList+=["SiHitCollection#PixelHits"]
    outStream.ItemList+=["SiHitCollection#SCT_Hits"]

###########################################################
## Disable some Pixel stuff
from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
pixelTool =  PixelDetectorTool()
pixelTool.Alignable = False

from Digitization.DigitizationFlags import digitizationFlags
pixeldigi = None
if not digitizationFlags.doXingByXingPileUp() and hasattr(job, 'PixelDigitization'):
   pixeldigi = job.PixelDigitization
else:
   for alg in job:
      if hasattr(alg, 'PileUpTools'):
         pixeldigi = alg.PileUpTools[ "PixelDigitizationTool" ]
         break
      pass
if None == pixeldigi:
   raise AttributeError("PixelDigitization(Tool) not found.")

if hasattr(pixeldigi,'UseCalibCondDB'):
   pixeldigi.UseCalibCondDB = False
   pixeldigi.UsePixMapCondDB = False
   pixeldigi.UsePixCondSum = False
   pixeldigi.EnableSpecialPixels = False
   pixeldigi.DisableDistortions = True
else:
   #From PixelDigitization-01-00-05 onwards configure tools directly
   from AthenaCommon.CfgGetter import getService, getPublicTool
   calibSvc = getService("CalibSvc")
   calibSvc.UseCalibCondDB = False
   calibSvc.UsePixMapCondDB = False
   pixeldigi.EnableSpecialPixels = False
   getPublicTool("SpecialPixelGenerator").UsePixCondSum = False
   getPublicTool("PixelBarrelChargeTool").DisableDistortions = True
   getPublicTool("PixelECChargeTool").DisableDistortions = True
   getPublicTool("DBMChargeTool").DisableDistortions = True
   getPublicTool("IblPlanarChargeTool").DisableDistortions = True
   getPublicTool("Ibl3DChargeTool").DisableDistortions = True

###########################################################
## SLHC Flags
## Leave commented out unless overriding with text file.
## Default is to use Geom DB only
#from InDetIBL_Example.SLHC_JobProperties import SLHC_Flags
#SLHC_Flags.SLHC_Version = "IBL-01"


###########################################################
## SLHC setup
## Set the path variables consistently
from InDetIBL_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()
