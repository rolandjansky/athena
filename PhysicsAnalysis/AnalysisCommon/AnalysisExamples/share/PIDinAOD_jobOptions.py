# Author: Ketevi A. Assamagan
# BNL, July 16, 2004

# job options for particle identification analysis
# Read in the AOD from POOL 
# this file is produced by running the AOD builders of the RDO raw data
# The user is supposed to modify the input data andset the flags as
# appropriate for his/her analysis

#get a handle on the ServiceManager which holds all the services
from AthenaCommon.AppMgr import ServiceMgr

# import the analysis flags
from AnalysisExamples.AnalysisFlags import AnalysisFlags

# load common options and common DLL
include( "AnalysisExamples/AnalysisCommon_jobOptions.py" )

# Print the flags before running the analysis code
AnalysisFlags.Print()

# particle pre selection job options
if AnalysisFlags.DoElectron:
   include( "AnalysisExamples/DoElectron_jobOptions.py" )
if AnalysisFlags.DoPhoton:	
   include( "AnalysisExamples/DoPhoton_jobOptions.py" )
if AnalysisFlags.DoMuon:
   include( "AnalysisExamples/DoMuon_jobOptions.py" )
if AnalysisFlags.DoTauJet:
   include( "AnalysisExamples/DoTauJet_jobOptions.py" )
if AnalysisFlags.DoParticleJet:
   include( "AnalysisExamples/DoParticleJet_jobOptions.py" )
if AnalysisFlags.DoBJet:
   include( "AnalysisExamples/DoBJet_jobOptions.py" )

# The AOD input file
import AthenaPoolCnvSvc.ReadAthenaPool
ServiceMgr.EventSelector.InputCollections = [ "AOD.pool.root" ]

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = DEBUG

# Number of Events to process
theApp.EvtMax = 10000

#########################################
#Setup TTree registration Service
# save ROOT histograms and NTuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
# Root Ntuple output
ServiceMgr.THistSvc.Output = [ "FILE1 DATAFILE='PIDinAOD.ntuple.root' OPT='NEW'" ]
ServiceMgr.THistSvc.Output += [ "FILE2 DATAFILE='PIDinAOD.hist.root' OPT='NEW'" ]
