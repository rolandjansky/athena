# Author: Ketevi A. Assamagan
# CERN, February 2005
# Modified by Vivek Jain, Mar. 2012

# This file uses AutoConfig to peek at the input file, and set Geometry tag, etc., correctly
# you don't need to set Geometry tags anymore. They are read from the file itself.

# here give the full path of a file that you run over locally
# if you are running on the grid, it will ignore what you set here, and do the right thing
# if you want to run over multiple files locally, use glob as shown below

from AthenaCommon.AthenaCommonFlags import jobproperties as jp
#jp.AthenaCommonFlags.FilesInput = ['/afs/cern.ch/atlas/maxidisk/d49/AOD.191045._001937.pool.root.1']
jp.AthenaCommonFlags.FilesInput = ['root://eosatlas//eos/atlas/atlascerngroupdisk/proj-sit/rtt/prod/tct/rel_2/17.2.X/i686-slc5-gcc43-opt/offline/Tier0ChainTests/Run00183021_JetTauEtmiss0_Collisions/myESD_JetTauEtmiss_0.pool.root']
#jp.AthenaCommonFlags.FilesInput = ['/usatlas/u/vj/vj_bnl_local/datafile/mc11_7TeV.105014.J5_pythia_jetjet.recon.ESD.e815_s1273_s1274_r3060_tid709350_00/ESD.709350._002530.pool.root.1']

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

from AnalysisExamples.AnalysisExamplesConf import ZeeOnESD
topSequence += ZeeOnESD()

# The user analysis cuts - default values to changed by the user as needed

# set the names of the particle containers to be retrieved from StoreGate
# must be the same name used to build the AOD container
# do not touch these if you do know what you are doing

# Electron Parameters
topSequence.ZeeOnESD.egammaContainer = "ElectronCollection"
topSequence.ZeeOnESD.TrackMatchContainer="egDetailContainer"
topSequence.ZeeOnESD.egammaEtCut  = 20.0*GeV
topSequence.ZeeOnESD.egammaEtaCut  = 2.5


# message level for ZeeOnESD
topSequence.ZeeOnESD.OutputLevel = INFO

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
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='ZeeOnESD.root' OPT='RECREATE'"]
from AnalysisTools.AthAnalysisToolsConf import AANTupleStream
topSequence += AANTupleStream()
topSequence.AANTupleStream.ExtraRefNames = [ "StreamESD","Stream1" ]
topSequence.AANTupleStream.OutputName = 'ZeeOnESD.root'
topSequence.AANTupleStream.WriteInputDataHeader = True
topSequence.AANTupleStream.OutputLevel = WARNING
# Root Ntuple output file and name
#theApp.HistogramPersistency = "ROOT"
#theApp.HistogramPersistencySvc.OutputFile = "VFitZmmOnAOD.root"


