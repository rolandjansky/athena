# RunCSC_Digitization.py 
#
# Job options for testing CSC digitization.
#
# Woochun Park
# April 2011
#
#--------------------------------------------------------------
myname = "RunCSC_Digitization: "

if not vars().has_key("NewDigitEDM"):
    NewDigitEDM = True

if not vars().has_key("InputFile"):
    InputFile ="/atlasdisk1/wpark/MonteCarlo/sim_dig/mc10/mc10_7TeV.106047.PythiaZmumu_no_filter.simul.HITS.e574_s1022_tid210462_00/HITS.210462._000001.pool.root.1"


if not vars().has_key("InputFiles"):
    if len(InputFile): InputFiles = [InputFile]
    else: InputFiles = []

if not vars().has_key("DetDescrVersion"):
    DetDescrVersion = "ATLAS-GEO-16-00-00"



from AthenaCommon.AlgSequence import AlgSequence
TopAlg = AlgSequence()



#include( "AthenaCommon/Atlas_Gen.UnixStandardJob.py" )

#--------------------------------------------------------------
# Detector description.
#--------------------------------------------------------------
print myname + "Setting geometry " + DetDescrVersion
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock( DetDescrVersion )
      
include ("RecExCommon/RecExCommon_flags.py")
DetFlags.ID_setOff()
DetFlags.Calo_setOff()
DetFlags.Tile_setOff()
DetFlags.LAr_setOff()
include ("RecExCommon/AllDet_detDescr.py")
#GeoModelSvc.MuonVersionOverride = "MuonSpectrometer-R.01.01.Initial.Light"
#GeoModelSvc.IgnoreTagDifference = True

#from IOVDbSvc.CondDB import conddb
#conddb.setGlobalTag('OFLCOND-SIM-00-00-06')
#conddb.setGlobalTag('OFLCOND-CSC-01-00-00')  
#------------
# To use CscCalibTool... this is special version to bypass DB folder loading
include ("CSC_Digitization/LocalCscCalibTool_jobOptions.py")

#--------------------------------------------------------------
# Digitzation.
#--------------------------------------------------------------
include("RecJobTransforms/UseOracle.py")

include("CSC_Digitization/CSC_Digitization_jobOptions.py")
CscDigitBuilder.noiseLevel = 0.0
CscDigitBuilder.pedestal = 0
CscDigitBuilder.OutputLevel = DEBUG

#--------------------------------------------------------------
# Define the input data stream.
#--------------------------------------------------------------

# Event selector
from AthenaCommon.AppMgr import ServiceMgr
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = InputFiles

#from AthenaCommon.AthenaCommonFlags import jobproperties
#jobproperties.AthenaCommonFlags.PoolHitsInput=[
#    "/raid01/woochun/DATA/HIT/mc08.106051.PythiaZmumu_1Lepton.simul.HITS.e347_s522_tid046058/HITS.046058._00006.pool.root.1"
#    ,"/raid01/woochun/DATA/HIT/mc08.106051.PythiaZmumu_1Lepton.simul.HITS.e347_s522_tid046058/HITS.046058._00035.pool.root.1"
#    ,"/raid01/woochun/DATA/HIT/mc08.106051.PythiaZmumu_1Lepton.simul.HITS.e347_s522_tid046058/HITS.046058._00040.pool.root.1"
#    ,"/raid01/woochun/DATA/HIT/mc08.106051.PythiaZmumu_1Lepton.simul.HITS.e347_s522_tid046058/HITS.046058._00052.pool.root.1"
#    ]
#jobproperties.AthenaCommonFlags.PoolRDOOutput="Valgrind.PileUpTools.RDO.pool.root"

ServiceMgr.MessageSvc.Format       = "% F%50W%S%7W%R%T %0W%M"
# to change the default limit on number of message
ServiceMgr.MessageSvc.defaultLimit = 9999999  # all messages

#--------------------------------------------------------------
# ItemList for writing out POOL RDO file
#--------------------------------------------------------------
#include( "EventAthenaPool/EventAthenaPoolItemList_joboptions.py" )
#fullItemList+=["CscRawDataContainer#*"]
#fullItemList+=["EventInfo#*"]

#from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
#CscDigitOutput = AthenaPoolOutputStream( "CscDigitOutput" )

# List of DO's to write out
#CscDigitOutput.ItemList   += fullItemList
# Run OutputStream as an algorithm
#topAlg += CscDigitOutput
#CscDigitOutput.OutputFile =   "R01V1S16_00_000.25TS.RDO.pool.root"
#print CscDigitOutput
#--------------------------------------------------------------
