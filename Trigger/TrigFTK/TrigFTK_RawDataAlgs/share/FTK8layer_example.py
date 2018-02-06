# JobTransform: RAWtoESD

from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.trfSubstepName = 'RAWtoESD' 

runArgs.postcommand = ['include("TriggerRelease/dbmod_BFieldAutoConfig.py")']
runArgs.writeBS = False
runArgs.beamType = 'collisions'
#runArgs.AMITag = 'r10188'
runArgs.runNumber = 341615
runArgs.autoConfiguration = ['everything']
runArgs.skipEvents = 6
runArgs.maxEvents = 2
runArgs.conditionsTag = 'CONDBR2-BLKPA-2017-12'
runArgs.geometryVersion = 'ATLAS-R2-2016-01-00-01'
runArgs.postExec = ['topSequence.FTK_RDO_ReaderAlgo.GetTracks=False;topSequence.FTK_RDO_ReaderAlgo.GetOfflineVertex_Offline=False;topSequence.FTK_RDO_ReaderAlgo.GetRefitTracks=False;topSequence.FTK_RDO_ReaderAlgo.GetTrackParticles=False;topSequence.FTK_RDO_ReaderAlgo.GetRefitTrackParticles=False;topSequence.FTK_RDO_ReaderAlgo.GetVertex=False;topSequence.FTK_RDO_ReaderAlgo.GetRefitVertex=False;from TrigFTK_RawDataAlgs.TrigFTK_RawDataAlgsConf import FTK_RDO_MonitorAlgo;FTK_RDO_Monitor = FTK_RDO_MonitorAlgo( "FTK_RDO_MonitorAlgo");FTK_RDO_Monitor.RDO_CollectionName="FTK_RDO_Tracks";FTK_RDO_Monitor.offlineTracksName="Tracks";FTK_RDO_Monitor.FTK_DataProvider=theFTK_DataProviderSvc;alg+= FTK_RDO_Monitor;topSequence.FTK_RDO_MonitorAlgo.GetHashFromTrack=False;topSequence.FTK_RDO_MonitorAlgo.GetHashFromConstants=True;topSequence.FTK_RDO_MonitorAlgo.Nlayers=8;topSequence.FTK_RDO_MonitorAlgo.PatternsVersion=\"DataAlignment_xm05_ym05_Reb64_v2\";topSequence.FTK_RDO_MonitorAlgo.TowerID=40;topSequence.FTK_RDO_MonitorAlgo.OutputLevel=VERBOSE'] # set TowerID here

#FTK_RDO_Monitor.mineta=-0.1;FTK_RDO_Monitor.maxeta=1.6;FTK_RDO_Monitor.minphi=2.3;FTK_RDO_Monitor.maxphi=2.9;


runArgs.preExec = ['rec.doWriteAOD=False;rec.doWriteESD=False;rec.doTrigger=False;rec.doFTK=True;rec.doCalo=False;rec.doInDet=True;rec.doMuon=False;rec.doJetMissingETTag=False;rec.doEgamma=False;rec.doMuonCombined=False;rec.doTau=False;from TrigFTKByteStream.TrigFTKByteStreamConf import FTK__TrigFTKByteStreamTool as TrigFTKByteStreamTool;ftkbstool=TrigFTKByteStreamTool("FTK::TrigFTKByteStreamTool");ftkbstool.FTKAuxDataFormat=True;ToolSvc+=ftkbstool']

# Input data
runArgs.inputBSFile = ['data17_5TeV.00341184.physics_Main.merge.DRAW_ZMUMU.f903_m1831._0001.1']
runArgs.inputBSFileType = 'BS'

#runArgs.inputBSFileNentries = 240
runArgs.BSFileIO = 'input'

# Output data
#runArgs.outputESDFile = 'ESD.root'
#runArgs.outputESDFileType = 'ESD'
#runArgs.outputHIST_ESD_INTFile = 'tmp.HIST_ESD_INT'
#runArgs.outputHIST_ESD_INTFileType = 'hist_esd_int'

include("RecJobTransforms/skeleton.RAWtoESD_tf.py")

from GaudiSvc.GaudiSvcConf import THistSvc
svcMgr += THistSvc(Output = ["TRACKS DATAFILE='ftk.root', OPT='RECREATE'"])
