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
loadInDetRec_Options = {"detectorDescription" : ""
                        ,"doReadBS":True
                        ,"globalTag" : ""
                        ,"siAlignmentTag":""
                        ,"trtAlignmentTag":""
                        ,"trtAlignmentTagL3":""
                        ,"alignmentPoolFile":""
                        ,"beamSpotTag":""
                        ,"errorScalingTag":""
                        ,"numberOfEvents":30
                        ,"GoodRunList":""
                        ,"inputPoolFiles":""
                        # 7 TeV Simulation Data
                        ,"inputFiles":["rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/DESD_SGLEL/f396_m946/data11_7TeV.00187815.physics_Egamma.merge.DESD_SGLEL.f396_m946/data11_7TeV.00187815.physics_Egamma.merge.DESD_SGLEL.f396_m946._lb0005-lb0011._0001.1"]
                        }
for var in loadInDetRec_Options:
  if var in dir():
    loadInDetRec_Options[var] = eval(var)

from glob import glob


PoolInput =  loadInDetRec_Options["inputFiles"]


#PoolRDOInput = ["rfio:/castor/cern.ch/grid/atlas/atlasdatadisk/data11_7TeV/ESD/v51/data11_7TeV.00187815.express_express.recon.ESD.v51/data11_7TeV.00187815.express_express.recon.ESD.v51._lb0005._SFO-ALL._0001.1"]
# number of event to process
Events= loadInDetRec_Options["numberOfEvents"]
SkipEvents = 0

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AlgSequence import AthSequencer



from GoodRunsLists.GoodRunsListsConf import *
ToolSvc += GoodRunsListSelectorTool()
GoodRunsListSelectorTool.GoodRunsListVec  = [loadInDetRec_Options["GoodRunList"]]

from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
job = AlgSequence()
seq = AthSequencer("AthMasterSeq")

from GoodRunsListsUser.GoodRunsListsUserConf import *
seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
seq.GRLTriggerAlg1.GoodRunsListArray = ['Eg_standard']        ## pick up correct name from inside xml file!

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput=PoolInput

athenaCommonFlags.EvtMax = Events
athenaCommonFlags.SkipEvents = SkipEvents
athenaCommonFlags.PoolRDOInput.set_Value_and_Lock(PoolInput)
#athenaCommonFlags.PoolRDOInput = PoolInput
#from AthenaCommon.GlobalFlags import globalflags
#globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-16-00-01")
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.IgnoreTagDifference = True

from AthenaCommon.GlobalFlags import globalflags
if len(loadInDetRec_Options["globalTag"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.ConditionsTag.set_Value_and_Lock(loadInDetRec_Options["globalTag"])

if len(loadInDetRec_Options["detectorDescription"])!=0:
	from AthenaCommon.GlobalFlags import globalflags
	globalflags.DetDescrVersion.set_Value_and_Lock(loadInDetRec_Options["detectorDescription"])

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
#DetFlags.TRT_setOff()

if len(loadInDetRec_Options["inputPoolFiles"][0]) > 0:
	readPool  = True 
	inputCollections = loadInDetRec_Options["inputPoolFiles"]
else:
	readPool = False
from IOVDbSvc.CondDB import conddb

#conddb.addOverride('/Indet/Align', 'InDetAlign_EoverPv2.3_2011_E')
#conddb.addOverride('/TRT/Align', 'TRTAlign_EoverPv2_2011_E')
#conddb.addOverride('/TRT/Calib/DX','TRTCalibDX_EoverPv3.1_2011_E')

from IOVDbSvc.CondDB import conddb
if not conddb.folderRequested('PIXEL/PixReco'):
    conddb.addFolder('PIXEL_OFL','/PIXEL/PixReco')

if len(loadInDetRec_Options["beamSpotTag"]) > 0:
  conddb.addOverride('/Indet/Beampos',loadInDetRec_Options["beamSpotTag"])
  
if readPool :
	conddb.blockFolder("/Indet/Align")
	conddb.blockFolder("/TRT/Align")
	#conddb.blockFolder("/TRT/Calib/DX")
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
else:
    if loadInDetRec_Options["siAlignmentTag"]:
        conddb.addOverride('/Indet/Align',loadInDetRec_Options["siAlignmentTag"])
    if loadInDetRec_Options["trtAlignmentTag"]:
        conddb.addOverride('/TRT/Align',loadInDetRec_Options["trtAlignmentTag"])
##     if loadInDetRec_Options["trtAlignmentTagL3"]:
##         conddb.addOverride('/TRT/Calib/DX',loadInDetRec_Options["trtAlignmentTagL3"])
if loadInDetRec_Options["trtAlignmentTagL3"]:
        conddb.addOverride('/TRT/Calib/DX',loadInDetRec_Options["trtAlignmentTagL3"])
# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


print svcMgr.IOVDbSvc


#print SkipEvents
from GaudiSvc.GaudiSvcConf import THistSvc
#ServiceMgr += THistSvc()
#ServiceMgr.THistSvc.Output += ["egTestOutPut DATAFILE='ElectronExtrapolationOut.root' OPT='RECREATE'"]
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output += ["eoverpValidation DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]
ServiceMgr.THistSvc.Output += ["eoverpValidation2 DATAFILE='eoverpValidationOut.root' OPT='RECREATE'"]


include ("InDetAlignExample/ElectronEoverPTracking.py")
from InDetPerformanceMonitoring.InDetPerformanceMonitoringConf import IDPerfMonEoverP
funIDPerfMonEoverP = IDPerfMonEoverP(name = 'IDPerfMonEoverP',
                                     ReFitterTool = ElectronRefitterTool,
                                     ReFitterTool2 = ElectronRefitterTool2,
                                     InputElectronContainerName = "ElectronAODCollection",
                                     RefittedElectronTrackContainer1 = GSFTrackCollection,
                                     RefittedElectronTrackContainer2 = DNATrackCollection,
                                     InputJetContainerName = "AntiKt4LCTopoJets", #AntiKt4TopoJets
                                     RefitTracks = True,
                                     isDATA = True,
                                     OutputLevel =INFO)

job += funIDPerfMonEoverP
#from ElectronRefit.ElectronRefitConf import ElectronExtrapolation
#ElectronRefitter = ElectronExtrapolation(name = 'ElectronRefitter',
#                                    ReFitterTool = ElectronRefitterTool,
#																		ReFitterTool2 = ElectronRefitterTool2,
#																		InputElectronContainerName = "ElectronAODCollection",
#																		RefittedElectronTrackContainer1 = GSFTrackCollection,
#																		RefittedElectronTrackContainer2 = DNATrackCollection,
#																		BremBuilder = bremBuilder,
#																		RefitTracks = True,
#																		isDATA = True,
#																		OutputLevel =4)



#job += ElectronRefitter


