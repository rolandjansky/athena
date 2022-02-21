if not 'CafJobInputs' in dir():
    CafJobInputs=[["rfio:/castor/cern.ch/grid/atlas/tzero/prod1/perm/data08_1beammag/physics_L1Calo/0087764/data08_1beammag.00087764.physics_L1Calo.recon.ESD.o4_f54/data08_1beammag.00087764.physics_L1Calo.recon.ESD.o4_f54._lb0000._sfo01._0001.1"]]
    print "No input file given, use ",CafJobInputs[0]

if not 'CafJobOutputs' in dir():
    CafJobOutputs=["LArTiming.root"]

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()


from AthenaCommon.GlobalFlags import GlobalFlags
GlobalFlags.DetGeo.set_commis()
GlobalFlags.Luminosity.set_zero()
GlobalFlags.DataSource.set_data()
GlobalFlags.InputFormat.set_pool()

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = "ATLAS-GEO-03-00-00"

from AthenaCommon.DetFlags import DetFlags
DetFlags.Calo_setOff()  #Switched off to avoid geometry
DetFlags.ID_setOff()
DetFlags.Muon_setOff()
DetFlags.Truth_setOff()
DetFlags.LVL1_setOff()
DetFlags.digitize.all_setOff()
#DetFlags.Print()

#Set up GeoModel (not really needed but crashes without)
from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit

svcMgr.GeoModelSvc.IgnoreTagDifference = True
 
#Get identifier mapping
include( "LArConditionsCommon/LArIdMap_comm_jobOptions.py" )
include( "LArIdCnv/LArIdCnv_joboptions.py" )

import AthenaPoolCnvSvc.ReadAthenaPool

from LArConditionsCommon.LArCondFlags import larCondFlags 
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

svcMgr.IOVDbSvc.GlobalTag="COMCOND-ES1C-001-00"

#Specify the input file(s)
svcMgr.EventSelector.InputCollections = CafJobInputs[0]


from LArROD.LArRODConf import LArRawChannelBuilderDriver
theLArRawChannelBuilder=LArRawChannelBuilderDriver("LArRawChannelBuilder")
theLArRawChannelBuilder.DataLocation="LArDigitContainer_Thinned"

topSequence += theLArRawChannelBuilder
# The first tool filters out bad channels
from LArROD.LArRODConf import LArRawChannelBuilderToolBadChannelTool
theLArRawChannelBuilderToolBadChannel=LArRawChannelBuilderToolBadChannelTool()

theLArRawChannelBuilderToolBadChannel.ProblemsToMask=[
    "deadReadout","deadPhys","almostDead","short",
    "lowNoiseHG","highNoiseHG","unstableNoiseHG",
    "lowNoiseMG","highNoiseMG","unstableNoiseMG",
    "lowNoiseLG","highNoiseLG","unstableNoiseLG"
    ]
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolBadChannel.getFullName()]
ToolSvc+=theLArRawChannelBuilderToolBadChannel

from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter
theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolOFCIter.getFullName()]

## # look for max in this range
theLArRawChannelBuilderToolOFCIter.minSample = 2 
theLArRawChannelBuilderToolOFCIter.maxSample = 12
theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=0 # ADCmax at least 3 sigma above noise for iteration
theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
theLArRawChannelBuilderToolOFCIter.defaultPhase=12  # starting delay, also the fixed delay for ADC below min.
theLArRawChannelBuilderToolOFCIter.TimingContainerKey="LArOFIterResult"
ToolSvc += theLArRawChannelBuilderToolOFCIter


from LArROD.LArRODConf import LArRawChannelBuilderPedestalDataBase
theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"
theLArRawChannelBuilder.PedestalTools  = [theLArRawChannelBuilderPedestalDataBase.getFullName()]
ToolSvc += theLArRawChannelBuilderPedestalDataBase

from LArRecUtils.LArADC2MeVCondAlgDefault import LArADC2MeVCondAlgDefault
LArADC2MeVCondAlgDefault()

from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
theLArRawChannelBuilder.ADCtoEnergyTools  = [theLArRawChannelBuilderADC2EDataBase.getFullName()]
ToolSvc += theLArRawChannelBuilderADC2EDataBase

from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
theLArOFPeakRecoTool=LArOFPeakRecoTool()
theLArOFPeakRecoTool.UseShape=False #Shoudl eventually be switched to true
ToolSvc += theLArOFPeakRecoTool




#Setting up the CBNTAA
from CBNT_Athena.CBNT_AthenaAwareCfg import CBNT_AthenaAware
theCBNT_AthenaAware= CBNT_AthenaAware()
CBNT_AthenaAware = topSequence.CBNT_AthenaAware
include( "CBNT_Athena/CBNT_EventInfo_jobOptions.py" )

from LArROD.LArRODConf import CBNTAA_LArDigits
theCBNTAA_LArDigits = CBNTAA_LArDigits("CBNTAA_LArDigits")
theCBNTAA_LArDigits.MaxChannels = 200000
#theCBNTAA_LArDigits.NEvents=1000
theCBNTAA_LArDigits.DumpIterResults=True
theCBNTAA_LArDigits.SaveId=True
theCBNTAA_LArDigits.SaveSCAAddress = False
theCBNTAA_LArDigits.DumpCut=1000
theCBNTAA_LArDigits.ContainerKey = "LArDigitContainer_Thinned"
#theCBNTAA_LArDigits.DumpDisconnected=False
#theCBNTAA_LArDigits.SaveSCAAddress=False
theCBNTAA_LArDigits.OutputLevel=VERBOSE
CBNT_AthenaAware +=theCBNTAA_LArDigits
     

#Setting up the CBNTAA Output Stream
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence +=AANTupleStream()
AANTupleStream = topSequence.AANTupleStream
AANTupleStream.ExtraRefNames = [ "" ]
AANTupleStream.OutputName = CafJobOutputs[0]
AANTupleStream.ExistDataHeader = False

theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc()
svcMgr.THistSvc.Output += ["AANT DATAFILE='"+CafJobOutputs[0]+"' OPT='RECREATE'"]


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
 
#svcMgr.ChronoStatSvc.ChronoDestinationCout = True

svcMgr.ChronoStatSvc.PrintUserTime     = True
svcMgr.ChronoStatSvc.PrintSystemTime   = True
svcMgr.ChronoStatSvc.PrintEllapsedTime = True
#svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1

#svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

theApp.AuditAlgorithms = True


#Hack to load the right dict to write vector< vector < X > > to TTRee
import AthenaPython.PyAthena as PyAthena
PyAthena.load_library('AtlasSTLAddReflexDict')
