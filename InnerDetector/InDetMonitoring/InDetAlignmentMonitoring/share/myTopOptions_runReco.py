#example of personal topOptions
#
# see RecExCommon/share/RecExCommon_flags.py for more available flags
# and https://twiki.cern.ch/twiki/bin/view/Atlas/RecExCommonFlags
# for more complete documentation.

# will use list of input files if filelist is defined
# otherwise will use a castorDataSet if defined
# otherwise will use file defined by dataPath + fileName (see defaults below)



if 'fileList' in dir():
    print "BDC: fileList = ",fileList

if 'castorDataSet' in dir():
    print "BDC: castorDataSet = ",castorDataSet
    
if 'dataPath' in dir():
    print "BDC: dataPath = ",dataPath
else:
    dataPath = '/afs/cern.ch/atlas/www/GROUPS/DATAPREPARATION/DataSkims/Zmumu/user.varoucas.data10_7TeV.periodD4.physics_MuonswBeam.PhysCont.ZtomumuSkim.ESD.t0repro04_v01.cern/'
    if not 'fileList' in dir():
        print "BDC: setting dataPath to: ",dataPath
    
if 'fileName' in dir():
    print "BDC: fileName = ",fileName
else:
    fileName = 'user.varoucas.data10_7TeV.periodD4.physics_MuonswBeam.PhysCont.ZtomumuSkim.ESD.t0repro04_v01.cern.ESD._00001.pool.root'
    if not 'fileList' in dir():
        print "BDC: setting fileName to: ",fileName
    
if 'nEvents' in dir():
    print "BDC: nEvents = ",nEvents
else:
    nEvents = -1
    print "BDC: setting nEvents to: ",nEvents

if 'runReco' in dir():
    print "BDC: runReco = ",runReco
else:
    runReco = False
    print "BDC: setting runReco to: ",runReco

if 'cosmicData' in dir():
    print "BDC: cosmicData = ",cosmicData
else:
    cosmicData = False
    print "BDC: setting cosmicData to: ",cosmicData

if 'IDRecoOnly' in dir():
    print "BDC: IDRecoOnly = ",IDRecoOnly
else:
    IDRecoOnly = False
    print "BDC: setting IDRecoOnly to: ",IDRecoOnly    

# if we don't specify the alignment tags then these do not get overridden in reco
if 'siAlignTag' in dir():
    print "BDC: siAlignTag = ",siAlignTag
if 'trtAlignTag' in dir():    
    print "BDC: trtAlignTag = ",trtAlignTag
if 'esTag' in dir():    
    print "BDC: esTag = ",esTag

if 'rootAlignFile' in dir():
    print "BDC: rootAlignFile = ",rootAlignFile

if 'goodRunList' in dir():
    print "BDC: goodRunList = ",goodRunList
else :
    goodRunList = ''

if 'goodRunListArray' in dir():
    print "BDC: goodRunListArray = ",goodRunListArray

if 'applyReweight' in dir():
    InDetAlignMonDoReweight = applyReweight
    print "BDC: applyReweight = ", applyReweight
else :
    InDetAlignMonDoReweight = False
    print "BDC: applyReweight = ", InDetAlignMonDoReweight

if 'runOnESD' in dir():
    InDetAlignMonRunOnESD = runOnESD
    print "BDC: runOnESD = ", InDetAlignMonRunOnESD
else :
    InDetAlignMonRunOnESD = False
    print "BDC: runOnESD = ", InDetAlignMonRunOnESD
    
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from RecExConfig.RecFlags import rec
from RecExConfig.RecAlgsFlags import recAlgs


#set input file. Note that this triggers autoconfiguration automatically (see RecExCommonAutoConfiguration wiki)
# here a MC RDO is specified, but this can also be a data BS, an ESD AOD or TAG
if athenaCommonFlags.FilesInput.isDefault(): # check if not already set upstream
    #athenaCommonFlags.FilesInput=["/tmp/bdcooper/CosmicsBField_160387_00.ESD.root"] # on lxplus304
    #athenaCommonFlags.FilesInput=["/castor/cern.ch/user/a/atlidali/calibration_IDTracks/Collision_160472_19.ESD.root"]

    if 'fileList' in dir() and fileList!='':
        include(fileList)
    elif 'castorDataSet' in dir() and castorDataSet!='':
        include("castorDataSets.py")
    elif "castor" in dataPath:
        athenaCommonFlags.FilesInput=[dataPath + fileName] #castor files has to be run on like this
    else:
        DATAPATH = dataPath # with this method the fileName can include a wildcard e.g. '*.root'
        from glob import glob # however, this won't work on castor (wildcards don't work on castor)
        INPUT = glob(DATAPATH + fileName)
        print INPUT
        athenaCommonFlags.FilesInput = INPUT

athenaCommonFlags.EvtMax=nEvents  # number of events to process run on all file



# switch on detectors
rec.doForwardDet=True
rec.doInDet=True
rec.doCalo=True
rec.doMuon=True

if runReco and IDRecoOnly:
    #these settings mean only ID reconstruction is done
    rec.doForwardDet=False
    rec.doInDet=True
    rec.doMuon=False
    rec.doCalo=False
    rec.doEgamma=False
    rec.doMuonCombined=False
    rec.doJetMissingETTag=False
    rec.doTau=False
   
#need some special settings for cosmics
if cosmicData:
    if runReco==False:
        #if we are looking at cosmics we don't care about the other detectors
        rec.doForwardDet=False
        rec.doCalo=False
        rec.doMuon=False

if runReco and IDRecoOnly or cosmicData:        
    rec.doTrigger = False; recAlgs.doTrigger=False


# autoconfiguration might trigger undesired feature
rec.doESD.set_Value_and_Lock(runReco) # uncomment if do not run ESD making algorithms
rec.doWriteESD.set_Value_and_Lock(False) # uncomment if do not write ESD
rec.doAOD.set_Value_and_Lock(False) # uncomment if do not run AOD making algorithms
rec.doWriteAOD.set_Value_and_Lock(False) # uncomment if do not write AOD
rec.doWriteTAG.set_Value_and_Lock(False) # uncomment if do not write TAG
rec.doCBNT.set_Value_and_Lock(False) # uncomment if do not make CBNT

# to enable monitoring that works on ESDs (i.e. InDetPerformanceMonitoring)
rec.doMonitoring.set_Value_and_Lock(True)
from AthenaMonitoring.DQMonFlags import DQMonFlags
#DQMonFlags.monManEnvironment.set_Value_and_Lock('tier0ESD')#produces run directories
DQMonFlags.monManEnvironment.set_Value_and_Lock('user')#doesn't produce run directories

DQMonFlags.doInDetAlignMon.set_Value_and_Lock(True)

#disable all monitoring other than alignment (too susceptible to crashes)
DQMonFlags.doPixelMon.set_Value_and_Lock(False)
DQMonFlags.doSCTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTMon.set_Value_and_Lock(False)
DQMonFlags.doInDetGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doGlobalMon.set_Value_and_Lock(False)
DQMonFlags.doLVL1CaloMon.set_Value_and_Lock(False)
DQMonFlags.doCTPMon.set_Value_and_Lock(False)
DQMonFlags.doHLTMon.set_Value_and_Lock(False)
DQMonFlags.doTRTElectronMon.set_Value_and_Lock(False)
DQMonFlags.doTileMon.set_Value_and_Lock(False)
DQMonFlags.doLArMon.set_Value_and_Lock(False)
DQMonFlags.doCaloMon.set_Value_and_Lock(False)
DQMonFlags.doEgammaMon.set_Value_and_Lock(False)
DQMonFlags.doMissingEtMon.set_Value_and_Lock(False)
DQMonFlags.doJetMon.set_Value_and_Lock(False)
DQMonFlags.doTauMon.set_Value_and_Lock(False)
DQMonFlags.doJetTagMon.set_Value_and_Lock(False)
DQMonFlags.doMuonRawMon.set_Value_and_Lock(False)
DQMonFlags.doMuonSegmentMon.set_Value_and_Lock(False)
DQMonFlags.doMuonTrackMon.set_Value_and_Lock(False)
DQMonFlags.doMuonAlignMon.set_Value_and_Lock(False)
DQMonFlags.doMuonPhysicsMon.set_Value_and_Lock(False)
DQMonFlags.doMuonCombinedMon.set_Value_and_Lock(False)
DQMonFlags.doLucidMon.set_Value_and_Lock(False)

if IDRecoOnly:
    DQMonFlags.doInDetPerfMon.set_Value_and_Lock(False)#makes no sense to run PerfMon if ID only reco
    
if cosmicData: 
    DQMonFlags.doInDetPerfMon.set_Value_and_Lock(False)#makes no sense to run PerfMon on cosmics
    from AthenaCommon.BeamFlags import jobproperties
    jobproperties.Beam.beamType.set_Value_and_Lock("cosmics")#needed so that monitoring runs with cosmic configuration

from AthenaCommon.DetFlags import DetFlags
DetFlags.ID_setOn()

from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.doStatistics.set_Value_and_Lock(False)
InDetFlags.doNewTracking.set_Value_and_Lock(True)
#InDetFlags.doSlimming.set_Value_and_Lock(False)
#InDetFlags.doSiSPSeededTrackFinder.set_Value_and_Lock(True)

if  InDetAlignMonRunOnESD :
    InDetFlags.doPRDFormation.set_Value_and_Lock(False)
else :
    InDetFlags.doPRDFormation.set_Value_and_Lock(True)


#print "PP PP PP PP   Config"
#InDetNewTrackingCuts      = ConfiguredNewTrackingCuts("Offline")
#InDetNewTrackingCuts.printInfo()

if not InDetAlignMonRunOnESD :
    from AthenaCommon.GlobalFlags import globalflags
    globalflags.ConditionsTag.set_Value_and_Lock( 'COMCOND-BLKPST-004-07' ) 


#main jobOptions
include ("RecExCommon/RecExCommon_topOptions.py")



if not InDetAlignMonRunOnESD :
    conddb.setGlobalTag(globalflags.ConditionsTag())

addLumiBlockTools = False
if InDetAlignMonRunOnESD :
    if not 'IS_SIMULATION' in inputFileSummary['evt_type'] and 'goodRunList' in dir() and goodRunList!='' and 'goodRunListArray' in dir() and goodRunListArray!='':
        addLumiBlockTools = True
else :
    if goodRunList!='' and 'goodRunListArray' in dir() and goodRunListArray!='':
        addLumiBlockTools = True
    
if  addLumiBlockTools :
    # add LumiBlockMetaDataTool to ToolSvc and configure
    from LumiBlockComps.LumiBlockCompsConf import LumiBlockMetaDataTool
    ToolSvc += LumiBlockMetaDataTool( "LumiBlockMetaDataTool" )
    LumiBlockMetaDataTool.calcLumi = False # False by default
    LumiBlockMetaDataTool.storeXMLFiles = True
    LumiBlockMetaDataTool.applyDQCuts = True
    LumiBlockMetaDataTool.OutputLevel = INFO

    # add ToolSvc.LumiBlockMetaDataTool to MetaDataSvc
    from AthenaServices.AthenaServicesConf import MetaDataSvc
    svcMgr += MetaDataSvc( "MetaDataSvc" )
    svcMgr.MetaDataSvc.MetaDataTools += [ ToolSvc.LumiBlockMetaDataTool ]

    # Configure the goodrunslist selector tool
    from GoodRunsLists.GoodRunsListsConf import *
    ToolSvc += GoodRunsListSelectorTool() 
    GoodRunsListSelectorTool.OutputLevel = INFO
    GoodRunsListSelectorTool.GoodRunsListVec = [ goodRunList ] 
    GoodRunsListSelectorTool.PassThrough = False
    
    # This Athena job consists of algorithms that loop over events;
    # here, the (default) top sequence is used:
    from AthenaCommon.AlgSequence import AlgSequence, AthSequencer
    job = AlgSequence()
    seq = AthSequencer("AthMasterSeq")

    from GoodRunsListsUser.GoodRunsListsUserConf import *
    seq += GRLTriggerSelectorAlg('GRLTriggerAlg1')
    # In the next line, pick up correct name from inside xml file!
    seq.GRLTriggerAlg1.GoodRunsListArray = [ goodRunListArray ]

if InDetAlignMonRunOnESD :
    if cosmicData and not 'IS_SIMULATION' in inputFileSummary['evt_type']:
            #apply the TRT cosmics calibrations to cosmics data runs
        conddb.addOverride('/TRT/Calib/T0','TrtCalibT0-AlignmentJune2010-00') 
        conddb.addOverride('/TRT/Calib/RT','TrtCalibRt-AlignmentJune2010-00') 
else :
    if cosmicData:
        conddb.addOverride('/TRT/Calib/T0','TrtCalibT0-AlignmentJune2010-00') 
        conddb.addOverride('/TRT/Calib/RT','TrtCalibRt-AlignmentJune2010-00') 

# use alignment constants specified in pool.root file if specified
# pool.root file needs to be inserted into PoolFileCatalog
if 'rootAlignFile' in dir() and rootAlignFile!='':
        print "BDC: using pool.root file alignment constants"
        conddb.blockFolder("/Indet/Align")
        conddb.blockFolder("/TRT/Align")
        from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr += CondProxyProvider()
        ServiceMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
        ServiceMgr.CondProxyProvider.InputCollections += [ rootAlignFile ]
        ServiceMgr.CondProxyProvider.OutputLevel=INFO
        print ServiceMgr.CondProxyProvider
        ServiceMgr.IOVSvc.preLoadData=True 

# overriding alignment constants if specified
if 'siAlignTag' in dir() and siAlignTag!='':
    conddb.addOverride('/Indet/Align', siAlignTag)

if 'trtAlignTag' in dir() and trtAlignTag!='':
    conddb.addOverride('/TRT/Align', trtAlignTag)

if 'trtCalibTag' in dir() and trtCalibTag!='':
    conddb.addOverride('/TRT/Calib/DX', trtCalibTag)

if 'esTag' in dir() and esTag!='':    
    conddb.addOverride('/Indet/TrkErrorScaling', esTag)


# user modifier of properties should come here
#topSequence.myAlg.someProperty=3.14
