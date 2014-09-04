###############################################################
#
# TOP Job options file for LArCellRec
#    it includes other jobOptions files
#   
#==============================================================
theApp.setup( ZEBRA )

theApp.Dlls += [ "GaudiAud" ] 
theAuditorSvc = AuditorSvc()
theAuditorSvc.Auditors =  [ "ChronoAuditor" ] 
#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Cells from MC hits (bypass Digitization, ROD) 
include( "LArCellRec/LArCellFromLArHit_MC_jobOptions.py" )

# replace the above with the following three lines for full path.
# #include "LArCellRec/LArCellFromLArRaw_MC_jobOptions.txt"
# digitmaker1.NoiseOnOff = true ;
# --- CBNT 
include( "LArCellRec/CBNT_LArCell_jobOptions.py" )

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
MessageSvc.OutputLevel               = 3
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#  Ntuple output 
NTupleSvc = Service( "NTupleSvc" )
NTupleSvc.Output =  [ "FILE1 DATAFILE='larcell.hbook' OPT='NEW'" ]
theApp.HistogramPersistency="HBOOK"
# Histogram output, if any. 
HistogramPersistencySvc = Service( "HistogramPersistencySvc" )
HistogramPersistencySvc.OutputFile  = "histo.hbook"
EventSelector.firstRun=1; 
EventSelector.lastRun=100000; 
EventSelector.calos = FALSE
EventSelector.muons = FALSE
EventSelector.indet = FALSE
