# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

## Job options file for launching Geant4 ATLAS detector simulations
## on the Tile geometry built from a standalone SQLite file
##
## 2022 Jan, Riccardo Maria BIANCHI, <riccardo.maria.bianchi@cern.ch>
##


from __future__ import print_function


from AthenaCommon.Logging import log as msg
msg.info("Job to test the new Tile 'DetectorFactoryLite' with G4")


## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSeq = AlgSequence()

## Output threshold (DEBUG, INFO, WARNING, ERROR, FATAL)
from AthenaCommon.Constants import DEBUG, INFO
ServiceMgr.MessageSvc.OutputLevel = DEBUG #INFO
ServiceMgr.MessageSvc.useColors = True
ServiceMgr.MessageSvc.defaultLimit = 0 # '0' means 'no limit'; otherwise, you can set to a value (e.g., 10000); default is '500'
#print( dir(ServiceMgr.MessageSvc)) # to see all options of MessageSvc



## Detector flags
##
## NOTE: InDetServMat cannot be switched ON/OFF, so it must be present when running on any InDet subsystems. Therefore, you should dump it from the InDetServMat plugin too, when dumping any other InDet subsystem.
from AthenaCommon.DetFlags import DetFlags
#print(dir(DetFlags)) # debug msg, to see all det flags
DetFlags.ID_setOff() # needs to be ON also when using beampipe only
DetFlags.bpipe_setOn() 
DetFlags.pixel_setOff()
DetFlags.SCT_setOff()
DetFlags.TRT_setOff()
DetFlags.BCM_setOff()
DetFlags.Calo_setOff() # LAr
DetFlags.Tile_setOff()  # Tile
DetFlags.Muon_setOff()
DetFlags.Lucid_setOff()
DetFlags.Truth_setOff()
#DetFlags.Print() # WARNING: this enables all Athena steps. I'm not sure what it is used for, actually...


from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()


#from TileGeoModel import TileGeoModelConf
#print( dir(TileGeoModelConf)) # to see all available handles 
from TileGeoModel.TileGeoModelConf import TileDetectorTool
TileDetectorTool.OutputLevel=DEBUG


## Global conditions tag
from AthenaCommon.GlobalFlags import jobproperties
jobproperties.Global.ConditionsTag = "OFLCOND-RUN12-SDR-21"

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolEvgenInput = ['/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/minbias_Inelastic-pythia8-7000.evgen.pool.root']
athenaCommonFlags.PoolHitsOutput = "test.HITS.pool.root"
athenaCommonFlags.EvtMax = 2 # NOTE: there is also a `VP1EventProducer.MaxNumberOfFiles`, defined later

## Simulation flags
from G4AtlasApps.SimFlags import simFlags
simFlags.load_atlas_flags()

## Layout tags: see simFlags.SimLayout for allowed values
## Use the default layout:
simFlags.SimLayout.set_On()
## Set a specific layout tag:
simFlags.SimLayout='ATLAS-R2-2015-03-01-00'
## Set a specific non-officially-supported layout tag using the _VALIDATION suffix:
#simFlags.SimLayout = 'ATLAS-R2-2015-03-01-00_VALIDATION'

## Set a matching RunNumber for run-dependent conditions.
## --> See: https://twiki.cern.ch/twiki/bin/viewauth/AtlasComputing/ConditionsRun1RunNumbers
#simFlags.RunNumber = 222510
#print(dir(simFlags)) # print all methods/variables of simFlags

## Set the EtaPhi, VertexSpread and VertexRange checks on
simFlags.EventFilter.set_On()

# Setup custom log level for the Athena tool 'TrackingAction',
# which is otherwise very verbose and buries the DEBUG messages
# we are interested into
from AthenaCommon.CfgGetter import getPublicTool
getPublicTool('G4UA::AthenaTrackingActionTool').OutputLevel = INFO



## Use single particle generator
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
#athenaCommonFlags.PoolEvgenInput.set_Off()
athenaCommonFlags.SkipEvents.set_Off()

# This should only be used when evgen is run in the simulation step
include('G4AtlasApps/fragment.SimCopyWeights.py')


include("G4AtlasApps/G4Atlas.flat.configuration.py")

#from TruthExamples.TruthExamplesConf import DumpMC
#topSeq += DumpMC()


from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg", tryDefaultConfigurable=True)

## Add the G4 sim to the alg sequence after the generator
from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("G4AtlasAlg",tryDefaultConfigurable=True)
print (topSeq)



### Call and setup the Sequencer, 
### which creates a data file (HITS,ESD,...) for each event
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaServices.AthenaServicesConf import OutputStreamSequencerSvc
outputStreamSequencerSvc = OutputStreamSequencerSvc()
outputStreamSequencerSvc.SequenceIncidentName = "EndEvent"
svcMgr += outputStreamSequencerSvc


### Add the algorithm producing VP1 events
from VP1AlgsEventProd.VP1AlgsEventProdConf import VP1EventProd

### Testing the VP1AlgsEventProd 
VP1EventProd.OutputFileTypeLabel="HITS"
VP1EventProd.RemoveTempInputFiles=True
VP1EventProd.DestinationDirectory="output_singleEvent_%s_files" % VP1EventProd.OutputFileTypeLabel
#Set the output level
MyOutputLevel = DEBUG
if not 'VP1MsgLvl' in dir():
  VP1MsgLvl=MyOutputLevel
VP1EventProd.OutputLevel=VP1MsgLvl
# create the instance
VP1EventProducer = VP1EventProd(InputPoolFile = athenaCommonFlags.PoolHitsOutput())

# Set the max number of events processed by VP1AlgsEventProd,
# which is equal to the single-event data files eventually produced.
# NOTE: this is the max n. of events processed by VP1AlgsEventProd, 
# which can be different from the n. of events processed by Athena 
# that is defined earlier, by setting the `athenaCommonFlags.EvtMax` variable
# If `VP1EventProducer.MaxNumberOfFiles` < `athenaCommonFlags.EvtMax`
# then only the first n events will be processed by VP1EventProducer.
# NOTE: In the OnlineEventDisplays machinery, set the number of files 
# large enough, so the deleting is performed by the dedicated prune script.
VP1EventProducer.MaxNumberOfFiles = 3 #250 


### Finally add this event producer to the main sequencer
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += VP1EventProducer

### Finally print setup in debug mode
if VP1MsgLvl <= DEBUG:
  printfunc ("\n\n\t VP1 setup\n",VP1EventProducer,"\n\n")
