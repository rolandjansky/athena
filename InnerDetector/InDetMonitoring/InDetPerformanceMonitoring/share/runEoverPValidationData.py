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


#from glob import glob
#PoolInput = ["root://eosatlas.cern.ch//eos/atlas/atlastier0/rucio/data15_13TeV/physics_Main/00270588/data15_13TeV.00270588.physics_Main.merge.DESDM_EGZ.f610_m1453_f610_m1462/data15_13TeV.00270588.physics_Main.merge.DESDM_EGZ.f610_m1453_f610_m1462._0001.1"]
PoolInput = []
for line in open("data15_13TeV.00271595.DESDM_EGZ.txt"):#"data15_13TeV.00267639.AOD.txt"):
  line = line.strip()
  PoolInput.append(line)

# number of event to process
EvtMax= -1
SkipEvents = 0



# DetFlags modifications are best set here (uncomment RecExCommon_flags first)


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

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag.set_Value_and_Lock("CONDBR2-BLKPA-2015-09")
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


from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn() # test
DetFlags.Calo_setOff()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()
#DetFlags.TRT_setOff()
# AOD fix?
#DetFlags.makeRIO.Calo_setOff()
#DetFlags.detdescr.Calo_setOn()


from IOVDbSvc.CondDB import conddb
inputCollections = []#["Iter4_AlignmentConstants.root"]
#inputCollections = ["/afs/cern.ch/user/m/mdanning/hias/public/13TeV/20.1.5.8/run_DCSfix/DCS_L2fixedThenL27_andL3/Iter3/Iter0_AlignmentConstants.root"]
readPool  = False
conddb.addOverride('/Indet/Align', 'InDetAlign-RUN2-25NS')
conddb.addOverride('/TRT/Align',   'TRTAlign-RUN2-25NS')

if readPool :
  conddb.blockFolder("/Indet/Align")
  conddb.blockFolder("/TRT/Align")
#	conddb.blockFolder("/TRT/Calib/DX")
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


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


print svcMgr.IOVDbSvc


#print SkipEvents
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["eoverpValidation DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["eoverpValidation2 DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]
include ("ElectronEoverPTracking.py")
from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonEoverP
funIDPerfMonEoverP = IDPerfMonEoverP(name = 'IDPerfMonEoverP',
                                     ReFitterTool = ElectronRefitterTool,
                                     ReFitterTool2 = ElectronRefitterTool2,
                                     InputElectronContainerName = "Electrons",
                                     InputJetContainerName = "AntiKt4LCTopoJets",
                                     RefittedElectronTrackContainer1 = GSFTrackCollection,
                                     RefittedElectronTrackContainer2 = DNATrackCollection,
                                     RefitTracks = True,
                                     isDATA = True,
                                     FillDetailedTree = True,
                                     OutputLevel = INFO)




job += funIDPerfMonEoverP
