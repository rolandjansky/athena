theApp.setup( NO_EVSEL )

include( "LArDetMgrDetDescrCnv/LArDetMgrDetDescrCnv_joboptions.py" )

#--------------------------------------------------------------
# Application manager options
#--------------------------------------------------------------
theApp.Dlls += ["LArCalibUtils","LArBookkeeping","LArTBCnv","HbookCnv" ]
theApp.TopAlg += ["LArAutoCorrMaker/AutoCorr","LArAutoCorr2Ntuple"]
Applicationmgr = Service( "Applicationmgr" )
Applicationmgr.ExtSvc += [ "LArTBCnvSvc","LArTBEventSelector/EventSelector" ]
theApp.EvtSel = "EventSelector"
# Number of events to be processed  : default is 10 
theApp.EvtMax = 100
#--------------------------------------------------------------
# Persistency services
#--------------------------------------------------------------0
EventPersistencySvc.CnvServices     += [ "LArTBCnvSvc" ]; 
StoreGateSvc = Service( "StoreGateSvc" )
StoreGateSvc.dump=TRUE
#--------------------------------------------------------------
# Message services options
#--------------------------------------------------------------
# Output levelt threshold : default is INFO
# 2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL
MessageSvc.OutputLevel      = 3
#--------------------------------------------------------------
# Event Selector options
#--------------------------------------------------------------
EventSelector.InputFiles=["/afs/cern.ch/user/m/maharrou/public/emtb/data/RUN217744.dat" ]
#--------------------------------------------------------------
# Properties for LArAutoCorr*
#--------------------------------------------------------------
AutoCorr = Algorithm( "AutoCorr" )
AutoCorr.events_ref = 50
AutoCorr.nsigma =5
LArAutoCorr2Ntuple = Algorithm( "LArAutoCorr2Ntuple" )
LArAutoCorr2Ntuple.Nsamples= 7
LArAutoCorr2Ntuple.ObjectKey= "AutoCorr"
theApp.HistogramPersistency="HBOOK"
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output  = [ "FILE1 DATAFILE='autocorr.ntp' OPT='NEW'"]
# To output ROOT file
#ApplicationMgr.Dlls += { "RootHistCnv" };
#HistogramPersistencySvc.OutputFile  = "";
#ApplicationMgr.HistogramPersistency="ROOT";
#NTupleSvc.Output  = { "FILE1 DATAFILE='autocorr.root' OPT='NEW'"};
