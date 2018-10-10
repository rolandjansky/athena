###############################################################
#
# Job options file to run PileUp. Need Digitization.py
#
#==============================================================
#--------------------------------------------------------------
# AthenaCommon configuration
#--------------------------------------------------------------
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = 3
athenaCommonFlags.SkipEvents=10

athenaCommonFlags.PoolHitsInput=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"]
athenaCommonFlags.PoolRDOOutput="PileUpDigiTest.root"

#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.IOVDbGlobalTag='OFLCOND-MC16-SDR-16'
digitizationFlags.simRunNumber=284500
digitizationFlags.dataRunNumber=284500
#inputs
digitizationFlags.overrideMetadata=['ALL'] #True
digitizationFlags.doHighPtMinBias=False
digitizationFlags.doLowPtMinBias=True
digitizationFlags.numberOfLowPtMinBias=2.3
digitizationFlags.LowPtMinBiasInputCols=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000003.pool.root.1",
"/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/HITS.10501925._000024.pool.root.1"]

#time structure
#for now use old flags...
digitizationFlags.bunchSpacing = 25
digitizationFlags.BeamIntensityPattern.createConstBunchSpacingPattern(75) #FIXME This runArg should probably in
digitizationFlags.initialBunchCrossing=-5
digitizationFlags.finalBunchCrossing=3
digitizationFlags.doXingByXingPileUp=True
#digitizationFlags.FixedT0BunchCrossing=0 # redundant as only position 0 is non-zero

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 75 # Set this to the spacing between filled bunch-crossings within the train.

#--------------------------------------------------------------
# Set some of the global flags. Like eg the DD version:
#--------------------------------------------------------------
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion = "ATLAS-R2-2016-01-00-01"

#FIXME should come from AthenaCommon
from AthenaCommon.Resilience import treatException,protectedInclude
athenaCommonFlags.AllowIgnoreConfigError=True

# for Paolo/Weiming: doing the imports below at this time eventually leaks to problem with PixelCalibSvc!! We had a similar problem in the past...
#from AtlasGeoModel import SetGeometryVersion
#from AtlasGeoModel import GeoModelInit

#select detectors
from AthenaCommon.DetFlags import DetFlags
DetFlags.all_setOff()
#DetFlags.pixel_setOn()
DetFlags.BCM_setOn()
DetFlags.TGC_setOn()
DetFlags.Truth_setOn()

#print job configuration
DetFlags.Print()

include("Digitization/Digitization.py")

#--------------------------------------------------------------
#test the IPileUpTool call back mechanism
#--------------------------------------------------------------
#this only works in XingByXing mode
if digitizationFlags.doXingByXingPileUp():
    job = AlgSequence()
    puAlg = job.StandardPileUpToolsAlg
    #puAlg = job.StandardInTimeOnlyTruthPileUpToolsAlg
    puAlg.OutputLevel=DEBUG
    from AthenaCommon.CfgGetter import getPublicTool
    puAlg.PileUpTools += [ getPublicTool("TestPileUpTool", tryDefaultConfigurable=True) ]
    puAlg.PileUpTools["TestPileUpTool"].OutputLevel=DEBUG
    #xing times in ns
    puAlg.PileUpTools["TestPileUpTool"].FirstXing=-300
    puAlg.PileUpTools["TestPileUpTool"].LastXing=+300

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
from AthenaCommon.AppMgr import ServiceMgr
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.setVerbose += [ "PileUpStream" ]
pupElm = ServiceMgr.PileUpEventLoopMgr
pupElm.OutputLevel = VERBOSE
if hasattr(ServiceMgr,"ArrayBM") :
    ServiceMgr.ArrayBM.OutputLevel=VERBOSE
if hasattr(ServiceMgr,"FixedArrayBM") :
    ServiceMgr.FixedArrayBM.OutputLevel=VERBOSE
if digitizationFlags.doLowPtMinBias:
    pupElm.bkgCaches["LowPtMinBiasCache"].OutputLevel = VERBOSE
##     pupElm.bkgCaches["LowPtMinBiasCache"].NonIntersectingRings = ["-1:1","-3:5", "-2:1"]
##     pupElm.bkgCaches["LowPtMinBiasCache"].AllowRingMigration = True
if digitizationFlags.doCavern:
    pupElm.bkgCaches["CavernCache"].OutputLevel = VERBOSE
##     pupElm.bkgCaches["CavernCache"].NonIntersectingRings = ["-1:1","-3:5", "-2:1"]
##     pupElm.bkgCaches["CavernCache"].AllowRingMigration = True
#ServiceMgr.PileUpMergeSvc.OutputLevel=VERBOSE


#subtract from central bunch xing one event, typically to compensate for
# the fact that also the OriginalEvent stream contains MinBias
#pupElm.bkgCaches["LowPtMinBiasCache"].SubtractBC0 = 1


#--------------------------------------------------------------
#monitor floating point exceptions
#--------------------------------------------------------------
try:
    theApp.CreateSvc += ["FPEControlSvc"]
except Exception:
    treatException("Could not load FPEControlSvc" )

#--------------------------------------------------------------
#print resource usage summary at the end of the job
#--------------------------------------------------------------
theApp.AuditServices=True
theApp.AuditAlgorithms=True
from AthenaCommon.AppMgr import theAuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
#theAuditorSvc += getConfigurable("ChronoAuditor")()
#theAuditorSvc += getConfigurable("MemStatAuditor")()
theAuditorSvc += getConfigurable("NameAuditor")()

#--------------------------------------------------------------
#run perfmon
#--------------------------------------------------------------
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.OutputFile = "ntuple.root"
# by default, PerfMon is disabled: enable it
# see https://twiki.cern.ch/twiki/bin/view/Atlas/PerfMonComps
jobproperties.PerfMonFlags.doMonitoring = True
protectedInclude( "PerfMonComps/PerfMonSvc_jobOptions.py" )
#instead of doMonitoring may also do
#jobproperties.PerfMonFlags.doFastMon = True


#use a wide name field to "see" the tools in use
svcMgr.MessageSvc.Format = "% F%32W%S%7W%R%T %0W%M"
svcMgr.MessageSvc.enableSuppression =False

#remove some of POOL annoying warnings
svcMgr.PoolSvc.AttemptCatalogPatch = True
#--------------------------------------------------------------
#print configuration flags
#--------------------------------------------------------------
jobproperties.print_JobProperties()
from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence()
print ServiceMgr
