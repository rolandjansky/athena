#example of personal topOptions
#
# to use it
# athena >! athena.log
#  ( myTopOptions.py is defaulted through jobOptions.py soft link)
#
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.
#
# doESD, DetDescrVersion and other flags
# needs be set before the include, since several
# secondary flags are configured according to that one
#
#   1) Specify the input in here
#      - One file
#PoolInput = ["/afs/cern.ch/work/s/sthenkel/work/testarea/20.1.0.6/MCsets/valid1.167824.Sherpa_CT10_ZmumuMassiveCBPt280_500_BFilter.recon.ESD.e3099_s1982_s1964_r6006_tid04628773_00/ESD.04628773._000033.pool.root.1"]
#PoolInput = ["/afs/cern.ch/user/s/sthenkel/eos/atlas/user/s/sthenkel/MC/valid3.147407.PowhegPythia8_AZNLO_Zmumu.recon.ESD.e3099_s2578_r6588_tid05292497_00/ESD.05292497._000150.pool.root.1"]
#   2) Feed files when executing the script
if 'inputFiles' in dir():
  print inputFiles
PoolInput = inputFiles


# number of event to process
EvtMax=-1
SkipEvents = 0


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

job = AlgSequence()
seq = AthSequencer("AthMasterSeq")

###############
# GRL
###############

useGRL = False

if useGRL:
  include("InDetPerformanceMonitoring/jobOptions_useGRL.py")

###############
# END GRL
###############

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
#from PerfMonComps.PerfMonFlags import jobproperties as pmjp
#pmjp.PerfMonFlags.doFastMon=True

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2015-14")
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-R2-2015-03-01-00")

from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
rec.doMuon.set_Value_and_Lock(False)
rec.doMuonCombined.set_Value_and_Lock(False)
rec.doEgamma.set_Value_and_Lock(False)
rec.doJetMissingETTag.set_Value_and_Lock(False)
rec.doTau.set_Value_and_Lock(False)
rec.doTrigger.set_Value_and_Lock(False)
rec.doTruth.set_Value_and_Lock(False)

#rec.doMonitoring.set_Value_and_Lock(True)
#from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.doInDetPerfMon.set_Value_and_Lock(True)


from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()

#DetFlags.makeRIO.Calo_setOff()
#DetFlags.detdescr.Calo_setOn()



from IOVDbSvc.CondDB import conddb
inputCollections = ["/afs/cern.ch/user/s/sthenkel/work/Performance/Alignment/WORK_ATHENA/20.1.X-VAL_WeakMode/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/Iter1_AlignmentConstants.root"]
readPool = True
conddb.addOverride('/Indet/Align', 'InDetAlign-RUN2-25NS')
conddb.addOverride('/TRT/Align',   'TRTAlign-RUN2-25NS')


if readPool :
  conddb.blockFolder("/Indet/Align")
  conddb.blockFolder("/TRT/Align")
  from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
  from AthenaCommon.AppMgr import ServiceMgr
  ServiceMgr += CondProxyProvider()
  ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
  # set this to the file containing AlignableTransform objects
  ServiceMgr.CondProxyProvider.InputCollections += inputCollections
  ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
  print ServiceMgr.CondProxyProvider
  # this preload causes callbacks for read in objects to be activated,
  # allowing GeoModel to pick up the transforms
  ServiceMgr.IOVSvc.preLoadData=True
  ServiceMgr.IOVSvc.OutputLevel=INFO

include ("InDetRecExample/InDetRecConditionsAccess.py")

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

print svcMgr.IOVDbSvc

from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon
varsVSmeanZmumu = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
varsVSwidthZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
varsDistrZmumu = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]

ZmumuMon = DiMuMon (name = "ZmumuMon_NoTrig",
                               resonName = "Zmumu",
                               minInvmass = 60.,
                               maxInvmass = 120.,
                               nMassBins = 60,
                               triggerChainName = "NoTrig",
                               regions = ["All","BB","EAEA","ECEC"],
                               varsVSmean = varsVSmeanZmumu,
                               varsVSwidth = varsVSwidthZmumu,
                               varsDistr = varsDistrZmumu,
                               doFits = True,
                               doSaveFits = False,
                               OutputLevel = DEBUG)

#ToolSvc += ZmumuMon

from AthenaMonitoring.DQMonFlags import DQMonFlags
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
IDPerfMonManager = AthenaMonManager(name                = "IDPerfMonManager",
                                    FileKey             = DQMonFlags.monManFileKey(),
                                    ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                    DataType            = DQMonFlags.monManDataType(),
                                    Environment         = "user",
                                    ManualRunLBSetup    = True,
                                    Run                 = 1,
                                    LumiBlock           = 1)
IDPerfMonManager.AthenaMonTools += [ ZmumuMon ]



ServiceMgr.THistSvc.Output += ["DiMuMon DATAFILE='./DiMuMon.root' OPT='RECREATE'"]
IDPerfMonManager.FileKey = "DiMuMon"

topSequence += IDPerfMonManager
