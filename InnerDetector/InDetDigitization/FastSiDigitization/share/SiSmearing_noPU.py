
#--------------------------------------------------------------------
# Perfmon configuration
#--------------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doMallocMonitoring = True
jobproperties.PerfMonFlags.doPostProcessing = True

#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.SkipEvents=0
athenaCommonFlags.EvtMax=-1
athenaCommonFlags.PoolHitsInput=["Hits.pool.root"]
athenaCommonFlags.PoolRDOOutput="DigitizationOutput.pool.root"

# the magnetic field
from MagFieldServices import SetupField
from IOVDbSvc.CondDB import conddb
conddb.addOverride('/GLOBAL/BField/Map','BFieldMap-FullAsym-09-solTil')

from TrkDetDescrSvc.TrkDetDescrJobProperties import TrkDetFlags
TrkDetFlags.MaterialSource           = 'Input'

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-SIM-00-00-00'

#--------------------------------------------------------------
# Global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion='ATLAS-GEO-20-00-01'
globalflags.DetDescrVersion='ATLAS-PX-ITK-00-00-00'

#--------------------------------------------------------------------
# DetFlags. Use to turn on/off individual subdetector or LVL1 trigger
#--------------------------------------------------------------------
from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
DetFlags.LVL1_setOff()
DetFlags.Truth_setOn()

include("FastSiDigitization/SiSmearing_jobOptions.py")

DetFlags.pixel_setOff()
DetFlags.detdescr.pixel_setOn()

DetFlags.SCT_setOff() # setOn to use the standard digi
DetFlags.detdescr.SCT_setOn()

#DetFlags.SCT_setOff()
DetFlags.TRT_setOff() # setOn to use the standard digi

include("Digitization/Digitization.py")

StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True

from InDetRecExample.InDetKeys import InDetKeys
InDetKeys.PixelManager = "Pixel"

from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.enableSuppression = False
ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
OutputNameString = 'CheckSmearing'

if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()
 
#ServiceMgr.THistSvc.Output  = ["val DATAFILE='"+OutputNameString+".root' TYPE = 'ROOT' OPT='RECREATE'"];


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.SystemOfUnits import *
import math

topSequence = AlgSequence()

from AtlasGeoModel import InDetGM

from FastSiDigitization.FastSiDigitizationConf import SiSmearedDigitizationTool
topSequence.PixelSmearedDigitization.DigitizationTool = SiSmearedDigitizationTool(CheckSmear=True,
                                                                                  OutputLevel=VERBOSE,
                                                                                  # pitch_X=10.0*micrometer,
                                                                                  # pitch_X=0*micrometer,
                                                                                  # pitch_Y=55.0*micrometer,
                                                                                  # pitch_Y=0*micrometer,
                                                                                  UseCustomGeometry = False, # @TODO was True, which causes many errors. Why ?
                                                                                  DetectorElementMapName="Pixel_IdHashDetElementMap",
                                                                                  MergeClusters = False
                                                                                  # , Nsigma = 1.0
                                                                                  )


topSequence.SCTSmearedDigitization.DigitizationTool = SiSmearedDigitizationTool(CheckSmear=True,
                                                                                SmearPixel = False,
                                                                                OutputLevel=VERBOSE,
                                                                                # pitch_X= 70.0*micrometer,
                                                                                # pitch_X= 0*micrometer,
                                                                                # pitch_X= 0*micrometer,
                                                                                UseCustomGeometry = False, # @TODO was True, which causes many errors. Why ?
                                                                                DetectorElementMapName="SCT_IdHashDetElementMap",
                                                                                MergeClusters = False)

rdo_stream = theApp.getOutputStream( "StreamRDO" )

print (rdo_stream)
#rdo_stream.ItemList += ["iFatras::PlanarClusterContainer#Pixel_PlanarClusters"]
##rdo_stream.ItemList += ["InDet::SiClusterContainer#PixelClusters"]
#rdo_stream.ItemList += ["iFatras::PlanarClusterContainer#SCT_PlanarClusters"]
#rdo_stream.ItemList += ["InDet::SiClusterContainer#SCT_Clusters"]

#rdo_stream.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarPixel"]
#rdo_stream.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPlanarSCT"]
##rdo_stream.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthPixel"]
#rdo_stream.ItemList += ["PRD_MultiTruthCollection#PRD_MultiTruthSCT"]

#from VP1Algs.VP1AlgsConf import VP1Alg 
#topSequence += VP1Alg()

