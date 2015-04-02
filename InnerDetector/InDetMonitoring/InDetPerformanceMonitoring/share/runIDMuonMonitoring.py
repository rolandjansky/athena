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


#PoolInput = [ "root://eosatlas//eos/atlas/user/s/stahlman/ZmumuESD/data11_7TeV.00180710.physics_Muons.recon.DESD_ZMUMU.f371_m716_f371/data11_7TeV.00180710.physics_Muons.recon.DESD_ZMUMU.f371_m716_f371._0013.1" ]
#PoolInput = ["/afs/cern.ch/user/p/pbutti/spacework/MC_MSstudies/mc12_8TeV.107231.ParticleGenerator_mu_Pt50.recon.ESD.e1717_s1609_s1611_r4076_tid01148449_00/ESD.01148449._004032.pool.root.2"]

#mc10b sample
#PoolInput = [ "root://eosatlas//eos/atlas/user/s/stahlman/ZmumuESD/mc10_7TeV.106047.PythiaZmumu_no_filter.recon.ESD.e574_s933_s946_r2175/ESD.289298._000009.pool.root.1" ]
#mc14 sample
#PoolInput = [ "/afs/cern.ch/user/s/sthenkel/work/QUALTASK/MC/mc14_valid.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e2658_s1967_s2008_r5787_tid04304891_00/AOD.04304891._000359.pool.root.1" ]
#PoolInput = [ "/afs/cern.ch/user/s/sthenkel/work/QUALTASK/MC/mc14_8TeV.147807.PowhegPythia8_AU2CT10_Zmumu.recon.AOD.e1852_s1896_s1912_r5591_tid01495119_00/AOD.01495119._005006.pool.root.1" ]
#PoolInput = ["/afs/cern.ch/user/s/sthenkel/work/QUALTASK/MC/mc14_13TeV.147407.PowhegPythia8_AZNLO_Zmumu.recon.ESD.e3059_s2046_s2008_r5862_tid01612264_00/ESD.01612264._005310.pool.root.1"]
if 'inputFiles' in dir():
  print inputFiles
PoolInput = inputFiles
# number of event to process
EvtMax= 10
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
DetFlags.Calo_setOn()
DetFlags.Muon_setOn()
#DetFlags.Tile_setOff()


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

conddb.addOverride('/Indet/TrkErrorScaling', 'IndetTrkErrorScaling_nominal')

if not conddb.folderRequested('PIXEL/PixReco'):
  conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

#if readPool :
#	conddb.blockFolder("/Indet/Align")
#	conddb.blockFolder("/TRT/Align")
#	from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
#	from AthenaCommon.AppMgr import ServiceMgr
#	ServiceMgr += CondProxyProvider()
#	ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
#	# set this to the file containing AlignableTransform objects
#	ServiceMgr.CondProxyProvider.InputCollections += inputCollections
#	ServiceMgr.CondProxyProvider.OutputLevel=DEBUG
#	print ServiceMgr.CondProxyProvider
#	# this preload causes callbacks for read in objects to be activated,
#	# allowing GeoModel to pick up the transforms
#	ServiceMgr.IOVSvc.preLoadData=True
#	ServiceMgr.IOVSvc.OutputLevel=INFO

include ("InDetRecExample/InDetRecConditionsAccess.py")

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["ZmumuValidation DATAFILE='ZmumuValidationOut.root' OPT='RECREATE'"]

include ("InDetPerformanceMonitoring/ElectronEoverPTracking.py")
#include ("/afs/cern.ch/user/s/sthenkel/work/ProjectArea/InnerDetector/InDetMonitoring/InDetPerformanceMonitoring/share/ElectronEoverPTracking.py")
from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMuonRefitter
funIDPerfMonMuon = IDPerfMuonRefitter(name = 'IDPerfMuonRefitter',
                                     ReFitterTool1 = MuonRefitterTool,
                                     ReFitterTool2 = MuonRefitterTool2,
				     OutputTracksName =  "SelectedMuons",
                                     OutputLevel =0)


#ToolSvc += funIDPerfMonMuon
job += funIDPerfMonMuon

trackCollections = ["SelectedMuonsRefit1","SelectedMuonsRefit2"]

include ("InDetPerformanceMonitoring/TrackMonitoring.py")



StoreGateSvc = Service("StoreGateSvc")
StoreGateSvc.Dump = True
