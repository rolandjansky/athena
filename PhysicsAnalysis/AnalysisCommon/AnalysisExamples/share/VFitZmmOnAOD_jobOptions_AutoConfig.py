# Author: Shuwei YE
# BNL, September 2009
# Modified by Vivek Jain, Feb 2010

# This file uses AutoConfig to peek at the input file, and set Geometry tag, etc., correctly
# you don't need to set Geometry tags anymore. They are read from the file itself.

# here give the full path of a file that you run over locally
# if you are running on the grid, it will ignore what you set here, and do the right thing
# if you want to run over multiple files locally, use glob as shown below

from AthenaCommon.AthenaCommonFlags import jobproperties as jp
jp.AthenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/maxidisk/d49/AOD.191045._001937.pool.root.1']

#DATAPATH = '/afs/cern.ch/atlas/maxidisk/d49/'
#from glob import glob
#INPUT = glob(DATAPATH + 'AOD*.root*')
#print INPUT
#jp.AthenaCommonFlags.FilesInput = INPUT

#
from RecExConfig.RecFlags import rec

# import the data types 
import EventKernel.ParticleDataType

# get a handle on the ServiceManager which holds all the services 
from AthenaCommon.AppMgr import ServiceMgr

include ("RecExCond/RecExCommon_flags.py")
include( "RecExCond/AllDet_detDescr.py" )
include( "AthenaPoolCnvSvc/ReadAthenaPool_jobOptions.py" )


# The input file already specified near the top of this file
ServiceMgr.EventSelector.InputCollections = jp.AthenaCommonFlags.FilesInput()

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from AnalysisExamples.AnalysisExamplesConf import VFitZmmOnAOD
topSequence += VFitZmmOnAOD()

# The user analysis cuts - default values to changed by the user as needed

# set the names of the particle containers to be retrieved from StoreGate
# must be the same name used to build the AOD container
# do not touch these if you do know what you are doing

# Muon Parameters
topSequence.VFitZmmOnAOD.MuonContainer = "MuidMuonCollection"
topSequence.VFitZmmOnAOD.MuonEtCut  = 6.0*GeV
topSequence.VFitZmmOnAOD.MuonEtaCut = 2.5

# message level for VFitZmmOnAOD
topSequence.VFitZmmOnAOD.OutputLevel = INFO

# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
ServiceMgr.MessageSvc.OutputLevel = INFO
# print out all messages. may need to increase it
ServiceMgr.MessageSvc.defaultLimit = 9999999

# Number of Events to process
theApp.EvtMax = -1

# setup TTree registration Service
# save ROOT histograms and Tuple
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='VFitZmmOnAOD.root' OPT='RECREATE'"]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
topSequence.AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.AANTupleStream.OutputName = 'VFitZmmOnAOD.root'
topSequence.AANTupleStream.WriteInputDataHeader = True
topSequence.AANTupleStream.OutputLevel = WARNING
# Root Ntuple output file and name
#theApp.HistogramPersistency = "ROOT"
#theApp.HistogramPersistencySvc.OutputFile = "VFitZmmOnAOD.root"
