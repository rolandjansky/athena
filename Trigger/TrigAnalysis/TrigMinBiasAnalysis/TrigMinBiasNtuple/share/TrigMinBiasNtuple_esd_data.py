#---------------------------------------------------------------
#
# Load the convertes and then run the ntuple
# creation code over data ESD.  This file is based on
# esdtoaod.py from RecExCommon
#
#---------------------------------------------------------------
# Set this to the Tag used for the data file you are running on (from AMI)
DetDescrVersion="ATLAS-GEO-16-00-01"
GeoModelSvc = Service("GeoModelSvc")
GeoModelSvc.AtlasVersion = "ATLAS-GEO-16-00-01"

PoolESDInput = ["rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0001.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0002.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0003.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0004.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0005.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0006.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0007.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0008.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0009.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0010.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0011.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0012.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0013.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0014.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0015.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0016.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0017.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0018.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0019.1","rfio:/castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/physics_MinBias/00180144/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0020.1"]

#poolESDInput = ["/tmp/tamartin/data11_7TeV.00180144.physics_MinBias.hltmerge.ESD.c607_c608_j6._0001.1"]

#from AthenaCommon.GlobalFlags import globalflags
#globalflags.ConditionsTag.set_Value_and_Lock('COMCOND-ES1C-000-00')
# For some reason this must be set to OFLCOND-CSC-00-00-00, despite the file being made with COMCOND-ES1C-000-00
#globalflags.ConditionsTag.set_Value_and_Lock('OFLCOND-CSC-00-00-00')

EvtMax=500

###############################
doCBNT=False

doTrigger=False

readESD=True
doWriteAOD=False
doWriteESD=False
doWriteTAG=False
doAOD=False 
doESD=True
doTAG=False
doTruth=False

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_flags.py")
#-----------------------------------------------------------

#-----------------------------------------------------------
include("RecExCommon/RecExCommon_topOptions.py")
#-----------------------------------------------------------


jobproperties.PerfMonFlags.doMonitoring = False
#jobproperties.PerfMonFlags.OutputFile = "ntuple.root"

MessageSvc.debugLimit = 10000000
MessageSvc.Format = "% F%48W%S%7W%R%T %0W%M"

#get rid of messages and increase speed
Service ("StoreGateSvc" ).ActivateHistory=False

#from AthenaCommon.AlgSequence import AlgSequence
print AlgSequence
print ServiceMgr

# disable printing of histograms in finalize
if hasattr(svcMgr,'THistSvc'):
    svcMgr.THistSvc.OutputLevel = WARNING

#-----------------------------------------------------------
## Make SpacePoints from Clusters
from SiSpacePointFormation.SiSpacePointFormationConf import InDet__SiTrackerSpacePointFinder
InDetSiTrackerSpacePointFinder = InDet__SiTrackerSpacePointFinder()       
topSequence += InDetSiTrackerSpacePointFinder
#-----------------------------------------------------------

from TrigMinBiasNtuple.TrigMinBiasNtupleConf import TrigMinBiasNtupleAlg
TrigMinBiasNtuple = TrigMinBiasNtupleAlg ()

TrigMinBiasNtuple.ntupleFile = "trigmb"

# A string vector enable and configure Ntuple components
#  "NtComponent[,Class:key][,opt:Option]"
# where ',' is used to separate the items and [] are optional.
TrigMinBiasNtuple.ntupleComponents = [#

  "CtpRdo",  
  "CtpDecision",
  "EventInfo,EventInfo:ByteStreamEventInfo",
  "MbtsContainer",
  "MbtsLvl2",
  "PixelClus",
  "SctSp",
  "SpLvl2",
  "TrtLvl2",
  "TrkCntsEf",
  "TrtRaw",
  "Track,TrackCollection:Tracks"

#  "CaloCell"
  ]

## Switch on classes to be dumped,
## by default they are switched off (except doEventInfo)


topSequence += TrigMinBiasNtuple
print          TrigMinBiasNtuple

theApp.HistogramPersistency = "ROOT"

if not hasattr(ServiceMgr, 'THistSvc'):
  from GaudiSvc.GaudiSvcConf import THistSvc
  ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output += [ "trigmb DATAFILE='TrigMinBias_Reprocess.root' OPT='RECREATE'"]

# For debugging
# ServiceMgr.StoreGateSvc.ActivateHistory = True
# ServiceMgr.StoreGateSvc.Dump            = True


