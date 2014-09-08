theApp.setup( NO_EVSEL )

include( "DetDescrCnvSvc/DetStore_joboptions.py" )

DetDescrCnvSvc = Service( "DetDescrCnvSvc" )
DetDescrCnvSvc.DetectorManagers+= ["CaloTTDescrManager"]
DetDescrCnvSvc.DetectorManagers += [ "CaloMgr" ]
include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel =4
#StoreGateSvc.dump=true;
#DetectorStore.dump=true;
#---- Job options to read in EMTB-Files --------------------
theApp.Dlls += ["LArTBCnv"]
theApp.ExtSvc += [ "LArTBCnvSvc" , "LArTBEventSelector/EventSelector"]
theApp.EvtSel = "EventSelector"
EventPersistencySvc.CnvServices     += [ "LArTBCnvSvc" ]
EventSelector.InputFiles = ["/afs/cern.ch/user/w/wlampl/public/emtb/data/RUN217828.dat"]
#----- JobOptions for Ntuple-output
theApp.Dlls += [ "RootHistCnv" ]; 
theApp.HistogramPersistency = "ROOT" 
#HistogramPersistencyService.OutputFile= "histo.root";
#---- JobOptions for TopAlgorithm
theApp.Dlls += ["LArROD"]
theApp.TopAlg += [ "LArCaliWaveBuilder"]
theApp.TopAlg += ["LArWaves2Ntuple"]
dumpdigits = Algorithm( "dumpdigits" )
dumpdigits.DigitKey="HIGH"
LArWaves2Ntuple = Algorithm( "LArWaves2Ntuple" )
LArWaves2Ntuple.ContainerKey="HIGH"
LArCaliWaveBuilder = Algorithm( "LArCaliWaveBuilder" )
LArCaliWaveBuilder.KeyList=["HIGH"]
theApp.EvtMax = 14986
NtupleSvc = Service( "NtupleSvc" )
NtupleSvc.Output=["FILE1 DATAFILE='calibWave.root' TYP='ROOT' OPT='NEW'"]
