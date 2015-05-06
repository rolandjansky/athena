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


from glob import glob
PoolInput = glob("/afs/cern.ch/work/a/amorley/testarea/eOverP/data12_8TeV.00208631.physics_Egamma.merge.AOD.f472_m1208/*")
PoolInput = ["/tmp/amorley/data12_8TeV.00204954.physics_Egamma.recon.DESD_ZEE.r4065_p1346_r4065_tid01110148_00/DESD_ZEE.01110148._000010.pool.root.1"]


# number of event to process
EvtMax= 10
SkipEvents = 0



# DetFlags modifications are best set here (uncomment RecExCommon_flags first)


from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer

if not rec.doTruth():
  from GoodRunsLists.GoodRunsListsConf import *
  grlTool =  GoodRunsListSelectorTool("GoodRunsTool")
  grlTool.GoodRunsListVec  =   ['data11_7TeV.periodAllYear_DetStatus-v18-pro08-04_CoolRunQuery-00-03-98_Eg_standard.xml']
  grlTool.EventSelectorMode = True
  grlTool.PassThrough     = False
  grlTool.OutputLevel = DEBUG
  print grlTool
#    GoodRunsListSelectorTool.OutputLevel = DEBUG
#
#    ## use grlb algorithm for event selection
  #AthenaEventLoopMgr=Service("AthenaEventLoopMgr")
  #AthenaEventLoopMgr.PreSelectTools+=[grlTool]
#  ToolSvc += grlTool
  #print AthenaEventLoopMgr
  #seq = AthSequencer("AthFilterSeq")
  #from GoodRunsListsUser.GoodRunsListsUserConf import *
  #seq += GRLTriggerSelectorAlg( "GRLFilterInWenuStream",
  #                                      OutputLevel       = INFO,
  #                                      GoodRunsListArray = [ 'eg_standard_7TeV' ] )




from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput
athenaCommonFlags.EvtMax = EvtMax
athenaCommonFlags.SkipEvents = SkipEvents

from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-REPPST-007-08")
globalflags.ConditionsTag.set_Value_and_Lock("COMCOND-BLKPA-006-07")


#globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-16-00-01")
#from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
#GeoModelSvc = GeoModelSvc()
#GeoModelSvc.IgnoreTagDifference = True

from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']
rec.doAOD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
rec.doInDet.set_Value_and_Lock(False)
rec.doTile.set_Value_and_Lock(True)
rec.doLArg.set_Value_and_Lock(True)
rec.doCalo.set_Value_and_Lock(True)
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
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()
#DetFlags.TRT_setOff()

inputCollections = ["Iter4_AlignmentConstants.root"]

readPool  = False

from IOVDbSvc.CondDB import conddb
#conddb.addOverride('/Indet/Align', 'InDetAlign_Repro2012_d0z0p_constrained')
#conddb.addOverride('/TRT/Align',   'TRTAlign_Repro2012_d0z0p_constrained')


#conddb.addOverride('/Indet/Align', 'InDetAlign_EoverPv2.3_2011_E')
#conddb.addOverride('/TRT/Align', 'TRTAlign_EoverPv2_2011_E')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverPv3.1_2011_E')


#conddb.addOverride('/GLOBAL/InsituPerf/EGamma/Resolutions','EGammaRes-0-0-2')
conddb.addOverride('/Indet/TrkErrorScaling','IndetTrkErrorScaling_nominal')
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
                                     InputElectronContainerName = "ElectronCollection",
                                     InputJetContainerName = "AntiKt4TopoEMJets",
                                     RefittedElectronTrackContainer1 = GSFTrackCollection,
                                     RefittedElectronTrackContainer2 = DNATrackCollection,
                                     RefitTracks = True,
                                     isDATA = True,
                                     OutputLevel =3)




job += funIDPerfMonEoverP


