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
if not digitizationFlags.doXingByXingPileUp():
   ## Check PixelDigitization is defined in the AlgSequence
   if hasattr(job,'PixelDigitization'):
      pix = job.PixelDigitization
   else:
      ## return an error here?
       print "PixelDigitization not found in AlgSequence"
else:
   ## Check for PixelDigitizationTool in list of PileUpTools
   try:
      print "trying to setup PixelDigitizationTool..."
      pix = job.PileUpToolsAlg.PileUpTools[ "PixelDigitizationTool" ]
   except:
      ## return an error here?
       print "PixelDigitizationTool not found in list of PileUpTools"

from AthenaCommon import CfgGetter
pix=CfgGetter.getPublicTool("PixelDigitizationTool",checkType=True)
CalibSvc = CfgGetter.getService("CalibSvc")

CalibSvc.UseCalibCondDB = False
CalibSvc.UsePixMapCondDB = False
pix.EnableSpecialPixels = False
#pix.DisableDistortions = True

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
