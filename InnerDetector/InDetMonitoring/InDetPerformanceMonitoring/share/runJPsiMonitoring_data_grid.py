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
#PoolInput = ["/home/wdic/data15_comm.00264034.physics_MinBias.recon.ESD.x322._lb0805._SFO-1._0001.1"]
#   2) Feed files when executing the script
if 'inputFiles' in dir():
  print inputFiles
PoolInput = inputFiles

# number of event to process
EvtMax=-1
SkipEvents = 0

NoBeamConstraint=True

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
from PerfMonComps.PerfMonFlags import jobproperties as pmjp
pmjp.PerfMonFlags.doFastMon=True


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer


from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-REPPST-007-08")
#globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-16-00-01")

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
#rec.doPerfMon.set_Value_and_Lock(True)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(True)
rec.doTile.set_Value_and_Lock(False)
rec.doLArg.set_Value_and_Lock(False)
rec.doCalo.set_Value_and_Lock(False)
#rec.doMuon.set_Value_and_Lock(True)
#rec.doMuonCombined.set_Value_and_Lock(True)
#rec.doEgamma.set_Value_and_Lock(True)
#rec.doJetMissingETTag.set_Value_and_Lock(True)
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

#with beamconstraint
if NoBeamConstraint:
  from InDetRecExample.InDetJobProperties import InDetFlags
  InDetFlags.useBeamConstraint.set_Value_and_Lock(False)

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

include ("RecExCond/RecExCommon_flags.py")
# switch off ID, calo, or muons
DetFlags.ID_setOn()
#DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()

DetFlags.makeRIO.Calo_setOff()
DetFlags.detdescr.Calo_setOn()


#USE temporary to DEBUG
#from AthenaCommon.AppMgr import theApp
#theApp.ReflexPluginDebugLevel=1



from GaudiSvc.GaudiSvcConf import THistSvc

# add LumiBlockMetaDataTool to ToolSvc and configure
from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
LumiBlockMetaDataTool.calcLumi = True # False by default

# add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
from AthenaServices.AthenaServicesConf import MetaDataSvc
svcMgr += MetaDataSvc( "MetaDataSvc" )
svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

# Configure the goodrunslist selector tool
from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool()
GoodRunsListSelectorTool.GoodRunsListVec = [ './data15_13TeV.periodAllYear_DetStatus-v63-pro18-01_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml' ]

## This Athena job consists of algorithms that loop over events;
## here, the (default) top sequence is used:
from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
seq = AthSequencer("AthMasterSeq")

## AthMasterSeq is always executed before the top sequence, and is configured such that
## any follow-up sequence (eg. top sequence) is not executed in case GRLTriggerAlg1 does
## not pass the event
## In short, the sequence AthMasterSeq makes sure that all algs in the job sequence
## are skipped when an event gets rejects
from GoodRunsListsUser.GoodRunsListsUserConf import *
seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
seq.GRLTriggerAlg1.GoodRunsListArray = ['PHYS_StandardGRL_All_Good']        ## pick up correct name from inside xml file!
#seq.GRLTriggerAlg1.TriggerSelectionRegistration = 'L1_MBTS_1' ## set this to your favorite trigger, eg. L1_MBTS_1_1

## Add the ntuple dumper to the top sequence, as usual
## DummyDumperAlg1 is run in the top sequence, but is not executed in case GRLTriggerAlg1 rejects the event.
job += DummyDumperAlg('DummyDumperAlg1')
# job.DummyDumperAlg1.RootFileName = 'selection1.root'
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["new DATAFILE='selection1.root' TYP='ROOT' OPT='RECREATE'"];
job.DummyDumperAlg1.GRLNameVec = [ 'LumiBlocks_GoodDQ0', 'IncompleteLumiBlocks_GoodDQ0' ]



readPool = False

from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/Indet/Align', 'InDetAlign_EoverP_189598_189845v0')
#conddb.addOverride('/TRT/Align',   'TRTAlign_EoverP_189598_189845v0')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverP_189598_189845v0')

#conddb.addOverride('/Indet/Align', 'InDetAlign_EoverP_190608_191239v0')
#conddb.addOverride('/TRT/Align',   'TRTAlign_EoverP_190608_191239v0')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverP_189598_189845v0')

#conddb.addOverride('/Indet/Align', 'InDetAlign_Nominal_RDeltaPhi_01')
#conddb.addOverride('/TRT/Align',   'TRTAlign_Nominal_RDeltaPhi_01')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_nominal')

#conddb.addOverride('/Indet/Align', 'InDetAlign_Nominal_ZDeltaPhi_01')
#conddb.addOverride('/TRT/Align',   'TRTAlign_Nominal_ZDeltaPhi_01')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_nominal')

#conddb.addOverride('/Indet/TrkErrorScaling', 'IndetTrkErrorScaling_nominal')

if not conddb.folderRequested('PIXEL/PixReco'):
  conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

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

from InDetDiMuonMonitoring.InDetDiMuonMonitoringConf import DiMuMon
varsVSmeanJpsi = ["eta","etaAll","etaPos","etaNeg","phi","phiAll","phiPos","phiNeg","pt","ptAll","ptPos","ptNeg","etaDiff","etaSumm","phiDiff","phiSumm","crtDiff"]
varsVSwidthJpsi = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg","etaDiff","phiDiff","crtDiff"]
varsDistrJpsi = ["etaAll","etaPos","etaNeg","phiAll","phiPos","phiNeg","ptAll","ptPos","ptNeg"]
JpsiMon = DiMuMon(name = "JpsiMon_NoTrig",
                  resonName = "Jpsi",
                  minInvmass = 2.5,
                  maxInvmass = 3.5,
                  nMassBins = 50,
                  triggerChainName = "NoTrig",
                  regions = ["All","BB","EAEA","ECEC"],
                  varsVSmean = varsVSmeanJpsi,
                  varsVSwidth = varsVSwidthJpsi,
                  varsDistr = varsDistrJpsi,
                  doFits = True,
                  doSaveFits = False,
                  OutputLevel = VERBOSE)

#ToolSvc += JpsiMon

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
IDPerfMonManager.AthenaMonTools += [ JpsiMon ]

ServiceMgr.THistSvc.Output += ["DiMuMon DATAFILE='./DiMuMon.root' OPT='RECREATE'"]
IDPerfMonManager.FileKey = "DiMuMon"

topSequence += IDPerfMonManager
