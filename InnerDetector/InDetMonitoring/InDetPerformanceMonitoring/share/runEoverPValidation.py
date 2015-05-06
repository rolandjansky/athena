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

# 1)
#PoolInput = glob("/atlas/data3/userdata/amorley/mc10_7TeV.106043.PythiaWenu_no_filter.merge.AOD.e574_s933_s946_r2216_r2260_tid300113_00/*")
#PoolInput = [ "/afs/cern.ch/user/s/sthenkel/work/QUALTASK/MC/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e1852_s1896_s1912_r5591_tid01495119_00/AOD.01495119._005006.pool.root.1" ]
#PoolInput = ["/afs/cern.ch/user/s/sthenkel/work/QUALTASK/MC/mc14_13TeV.147407.PowhegPythia8_AZNLO_Zmumu.recon.ESD.e3059_s2046_s2008_r5862_tid01612264_00/ESD.01612264._005310.pool.root.1"]
PoolInput = ["/afs/cern.ch/user/s/sthenkel/eos/atlas/user/s/sthenkel/MC/mc14_13TeV.147407.PowhegPythia8_AZNLO_Zmumu.recon.ESD.e3059_s2046_s2008_r5862_tid01612263_00/ESD.01612263._003709.pool.root.1"]
#PoolInput = ["/afs/cern.ch/user/s/sthenkel/eos/atlas/user/s/sthenkel/MC/mc12_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.ESD.e1169_s1469_s1470_r3658_tid01085595_00/ESD.01085595._000584.pool.root.1"]

# 2) input through -c
#if 'inputFiles' in dir():
#  print inputFiles
#PoolInput = inputFiles





# number of event to process
EvtMax= -1
SkipEvents = 0



# DetFlags modifications are best set here (uncomment RecExCommon_flags first)


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
rec.doESD.set_Value_and_Lock(False)
rec.doESD.set_Value_and_Lock(False) # uncomment if rec.do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if rec.do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if rec.do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if rec.do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if rec.do not write TAG
rec.doCBNT.set_Value_and_Lock(False)
rec.doPerfMon.set_Value_and_Lock(False)
#rec.doInDet.set_Value_and_Lock(False)
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
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()



inputCollections = ["Z2dX_AlignmentConstants.root"]

readPool = False

from IOVDbSvc.CondDB import conddb


#InDetAlign_Nominal_PhiDeltaPhi_01
#InDetAlign_Nominal_PhiDeltaR_01
#InDetAlign_Nominal_PhiDeltaZ_01
#InDetAlign_Nominal_RDeltaPhi_01
#InDetAlign_Nominal_RDeltaR_01
#InDetAlign_Nominal_RDeltaZ_01
#InDetAlign_Nominal_RadialConst_01
#InDetAlign_Nominal_Sagitta_01
#InDetAlign_Nominal_ZDeltaPhi_01
#InDetAlign_Nominal_ZDeltaR_01
#InDetAlign_Nominal_ZDeltaZ_01


#conddb.addOverride('/Indet/Align', 'InDetAlign_Nominal_Sagitta_01')
#conddb.addOverride('/TRT/Align', 'TRTAlign_Nominal_Sagitta_01')



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


# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

ToolSvc += CfgMgr.JetCleaningTool("MyTool",CutLevel="LooseBad")

from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["eoverpValidation DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["eoverpValidation2 DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]


include ("InDetPerformanceMonitoring/ElectronEoverPTracking.py")
from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonEoverP
funIDPerfMonEoverP = IDPerfMonEoverP(name = 'IDPerfMonEoverP',
                                     ReFitterTool = ElectronRefitterTool,
                                     ReFitterTool2 = ElectronRefitterTool2,
                                     InputElectronContainerName = "ElectronCollection",
                                     RefittedElectronTrackContainer1 = GSFTrackCollection,
                                     RefittedElectronTrackContainer2 = DNATrackCollection,
                                     RefitTracks = True,
                                     isDATA = False,
				     ValidationMode = True,
				     FillDetailedTree = True,
                                     OutputLevel =1)



job += funIDPerfMonEoverP
