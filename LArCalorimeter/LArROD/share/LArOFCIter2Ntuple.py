if not 'CafJobInputs' in dir():
    CafJobInputs=[["/scratch/wlampl/data12_8TeV.00215091.physics_JetTauEtmiss.merge.RAW._lb0092._SFO-1._0001.1"]]
    print "No input file given, use ",CafJobInputs[0]


if not 'CafJobOutputs' in dir():
    CafJobOutputs=["LArTiming.root"]

from AthenaCommon.AppMgr import (theApp, ServiceMgr as svcMgr,ToolSvc)
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

include("LArConditionsCommon/LArMinimalSetup.py")

from LArConditionsCommon.LArCondFlags import larCondFlags 
include("LArConditionsCommon/LArConditionsCommon_comm_jobOptions.py")

svcMgr.IOVDbSvc.GlobalTag="COMCOND-BLKPA-RUN1-06"

#Specify the input file(s)
svcMgr.EventSelector.Input=CafJobInputs[0]

# Specify the object you want to read from ByteStream
theByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc
theByteStreamAddressProviderSvc.TypeNames += ["LArDigitContainer/FREE"]
theByteStreamAddressProviderSvc.TypeNames += ["LArFebHeaderContainer/LArFebHeader"]


#from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
#topSequence+=xAODMaker__EventInfoCnvAlg()
#rec.doTruth=False

from LArROD.LArRODConf import LArRawChannelBuilderDriver
theLArRawChannelBuilder=LArRawChannelBuilderDriver("LArRawChannelBuilder")
topSequence += theLArRawChannelBuilder
# The first tool filters out bad channels
from LArROD.LArRODConf import LArRawChannelBuilderToolBadChannelTool
theLArRawChannelBuilderToolBadChannel=LArRawChannelBuilderToolBadChannelTool()
from LArBadChannelTool.LArBadChannelToolConf import LArBadChannelMasker
theLArRCBMasker=LArBadChannelMasker("LArRCBMasker")
theLArRCBMasker.DoMasking=True
theLArRCBMasker.ProblemsToMask=[
    "deadReadout","deadPhys","almostDead","short",
    "lowNoiseHG","highNoiseHG","unstableNoiseHG",
    "lowNoiseMG","highNoiseMG","unstableNoiseMG",
    "lowNoiseLG","highNoiseLG","unstableNoiseLG"
    ]
ToolSvc+=theLArRCBMasker
theLArRawChannelBuilderToolBadChannel.BadChannelMask=theLArRCBMasker
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolBadChannel]
ToolSvc+=theLArRawChannelBuilderToolBadChannel

from LArROD.LArRODConf import LArRawChannelBuilderToolOFCIter
theLArRawChannelBuilderToolOFCIter=LArRawChannelBuilderToolOFCIter()

## # look for max in this range
theLArRawChannelBuilderToolOFCIter.minSample = 2 
theLArRawChannelBuilderToolOFCIter.maxSample = 12
theLArRawChannelBuilderToolOFCIter.minADCforIterInSigma=3 # ADCmax at least 3 sigma above noise for iteration
theLArRawChannelBuilderToolOFCIter.minADCforIter=15 # min adc for iteration (only if no pedestalRMS found)
theLArRawChannelBuilderToolOFCIter.defaultPhase=12  # starting delay, also the fixed delay for ADC below min.
theLArRawChannelBuilderToolOFCIter.StoreTiming=True
theLArRawChannelBuilder.BuilderTools += [theLArRawChannelBuilderToolOFCIter]
theLArRawChannelBuilder += theLArRawChannelBuilderToolOFCIter


from LArROD.LArRODConf import LArRawChannelBuilderPedestalDataBase
theLArRawChannelBuilderPedestalDataBase=LArRawChannelBuilderPedestalDataBase()
theLArRawChannelBuilderPedestalDataBase.LArPedestalKey = "Pedestal"
theLArRawChannelBuilder.PedestalTools  = [theLArRawChannelBuilderPedestalDataBase]
theLArRawChannelBuilder += theLArRawChannelBuilderPedestalDataBase

from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theLArADC2MeVTool=LArADC2MeVToolDefault()
ToolSvc+=theLArADC2MeVTool

from LArROD.LArRODConf import LArRawChannelBuilderADC2EDataBase
theLArRawChannelBuilderADC2EDataBase=LArRawChannelBuilderADC2EDataBase()
theLArRawChannelBuilder.ADCtoEnergyTools  = [theLArRawChannelBuilderADC2EDataBase]
theLArRawChannelBuilderADC2EDataBase.ADC2MeVTool = theLArADC2MeVTool
theLArRawChannelBuilder += theLArRawChannelBuilderADC2EDataBase

from LArRecUtils.LArRecUtilsConf import LArOFPeakRecoTool
theLArOFPeakRecoTool=LArOFPeakRecoTool()
theLArOFPeakRecoTool.UseShape=True
ToolSvc += theLArOFPeakRecoTool



#"ESD" writing
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr
StreamESD_Augmented=MSMgr.NewPoolStream("streamESD","esdout.pool.root",asAlg=True)
StreamESD=StreamESD_Augmented.GetEventStream()
StreamESD.ItemList+=["EventInfo#*","LArRawChannelContainer#*"]


#CaloD3PD writing
from CaloD3PDMaker.LArDigitD3PDObject import LArDigitD3PDObject
from EventCommonD3PDMaker.EventInfoD3PDObject     import EventInfoD3PDObject
alg = MSMgr.NewRootStream("caloD3PD",CafJobOutputs[0] )
#alg += EventInfoD3PDObject(10)
alg += LArDigitD3PDObject(2,sgkey ="FREE")

#topSequence.caloD3PDAANTStream.ExistDataHeader=False

svcMgr.StoreGateSvc.Dump=True

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
theAuditorSvc = svcMgr.AuditorSvc
theAuditorSvc.Auditors  += [ "ChronoAuditor"]
 
#svcMgr.ChronoStatSvc.ChronoDestinationCout = True

#svcMgr.ChronoStatSvc.PrintUserTime     = True
#svcMgr.ChronoStatSvc.PrintSystemTime   = True
#svcMgr.ChronoStatSvc.PrintEllapsedTime = True
#svcMgr.ChronoStatSvc.NumberOfSkippedEventsForMemStat = 1

#svcMgr.ChronoStatSvc.AsciiStatsOutputFile = "chronoStats.ascii"

#svcMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

#theApp.AuditAlgorithms = True


theApp.EvtMax=10



