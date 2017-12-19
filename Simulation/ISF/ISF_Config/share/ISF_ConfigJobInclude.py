"""
Common configurations for ISF
KG Tan, 17/06/2012
"""

include.block('ISF_Config/ISF_ConfigJobInclude.py')
from AthenaCommon.CfgGetter import getPrivateTool,getPrivateToolClone,getPublicTool,getPublicToolClone,\
        getService,getServiceClone,getAlgorithm,getAlgorithmClone

#--------------------------------------------------------------
# Set to monte carlo
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob
from AthenaCommon import AthenaCommonFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# TODO: ELLI: remove this once the envelopes are stored in the DDDB
#             -> currently a fallback python definition is used
#import shutil
#import os.path
#if os.path.exists('geomDB') and os.path.islink('geomDB'):
    #os.unlink('geomDB')
#os.symlink('/afs/cern.ch/atlas/groups/Simulation/ISF/Envelopes_geomDB', 'geomDB')


#--------------------------------------------------------------
# Set the flags automatically here
# (move it to the job options if not meant to be automatic!)
#--------------------------------------------------------------
import AthenaCommon.SystemOfUnits as Units

from ISF_Config.ISF_jobProperties import ISF_Flags # IMPORTANT: Flags must be finalised before these functons are called

from AthenaCommon.GlobalFlags import globalflags
# --- set globalflags
globalflags.DataSource.set_Value_and_Lock('geant4')
globalflags.InputFormat.set_Value_and_Lock('pool')
globalflags.DetGeo.set_Value_and_Lock('atlas')
globalflags.Luminosity.set_Off()

# --- set SimLayout (synchronised to globalflags)
from G4AtlasApps.SimFlags import simFlags
if globalflags.DetDescrVersion() not in simFlags.SimLayout.get_Value():
    print "ERROR globalFlags.DetDescrVersion and simFlags.SimLayout do not match!"
    print "Please correct your job options."
    # TODO: theApp.exit(1)?
    import sys
    sys.exit(1)
simFlags.load_atlas_flags()
simFlags.EventFilter.set_Off()

#--------------------------------------------------------------
# Job setup
#--------------------------------------------------------------
theApp.EvtMax = athenaCommonFlags.EvtMax()

# all det description
include('ISF_Config/AllDet_detDescr.py')
DetFlags.Print()

if len(globalflags.ConditionsTag()):
    from IOVDbSvc.CondDB import conddb
    conddb.setGlobalTag(globalflags.ConditionsTag())

# Temporary work-around - see ATLASSIM-2351
if ISF_Flags.UsingGeant4():
    include("G4AtlasApps/G4Atlas.flat.configuration.py") #HACK

#--------------------------------------------------------------
# Setup the ISF Services
#--------------------------------------------------------------
# --- load AuditorSvc
from AthenaCommon.ConfigurableDb import getConfigurable
# --- write out summary of the memory usage
#   | number of events to be skip to detect memory leak
#   | 20 is default. May need to be made larger for complete jobs.
ServiceMgr.AuditorSvc += getConfigurable("ChronoAuditor")()
# --- write out a short message upon entering or leaving each algorithm
# ServiceMgr.AuditorSvc += getConfigurable("NameAuditor")()
#
theApp.AuditAlgorithms = True
theApp.AuditServices   = True
#
# --- Display detailed size and timing statistics for writing and reading
ServiceMgr.AthenaPoolCnvSvc.UseDetailChronoStat = True

#--------------------------------------------------------------
# ISF input
#--------------------------------------------------------------

# Note: automatically adds generator to TopSequence if applicable
from AthenaCommon.BeamFlags import jobproperties
# if an input sting identifier was given, use ISF input definitions
if ISF_Flags.Input()!="NONE":
    getAlgorithm('ISF_Input_' + ISF_Flags.Input())
# cosmics input
elif jobproperties.Beam.beamType.get_Value() == 'cosmics':
    from G4AtlasApps.SimFlags import simFlags
    simFlags.load_cosmics_flags()
    if simFlags.ReadTR.statusOn:
        getAlgorithm('TrackRecordCosmicGenerator')
    else:
        getAlgorithm('EvgenCosmicGenerator')
# non of the above

elif not athenaCommonFlags.PoolEvgenInput.statusOn:
    print "ISF Input Configuration: PoolEvgenInput off, likely running with a particle gun preInclude"
# non of the above -> input via ISF_Flags
else :
    if ISF_Flags.OverrideInputFiles():
        athenaCommonFlags.PoolEvgenInput = ISF_Flags.OverrideInputFiles()
    getAlgorithm('ISF_Input_GenericFiles')

from ISF_Example.ISF_Input import ISF_Input

from AthenaCommon.CfgGetter import getAlgorithm
topSeq += getAlgorithm("BeamEffectsAlg")

#--------------------------------------------------------------
# ISF kernel configuration
#--------------------------------------------------------------

# keep reference to collection merger algorithm to guarantee that
# any subsequent simulator configuration gets a reference to the same
# instance when calling confgetter's getAlgorithm
collection_merger_alg = getAlgorithm('ISF_CollectionMerger')

SimKernel = getAlgorithm(ISF_Flags.Simulator.KernelName())

if ISF_Flags.ValidationMode():
    topSeq += getAlgorithm("ISF_SimHitTreeCreator")

#--------------------------------------------------------------
# Setup the random number streams
#--------------------------------------------------------------
from G4AtlasApps.SimFlags import SimFlags,simFlags;
if not athenaCommonFlags.PoolEvgenInput.statusOn:
    if not simFlags.RandomSeedList.checkForExistingSeed('SINGLE'):
        simFlags.RandomSeedList.addSeed( "SINGLE", 43657613, 78935670 )
if not simFlags.RandomSeedList.checkForExistingSeed('VERTEX'):
  simFlags.RandomSeedList.addSeed( 'VERTEX', 9123448, 829143 )
simFlags.RandomSeedList.addtoService()
simFlags.RandomSeedList.printSeeds()

#--------------------------------------------------------------
# Setup the ISF Output
#--------------------------------------------------------------
from ISF_Example.ISF_Output import ISF_HITSStream
from ISF_Example.ISF_Metadata import createSimulationParametersMetadata, configureRunNumberOverrides
createSimulationParametersMetadata()
configureRunNumberOverrides()

if ISF_Flags.HITSMergingRequired():
    topSequence += collection_merger_alg

#--------------------------------------------------------------
# Post kernel configuration
#--------------------------------------------------------------

# proper GeoModel initialization
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if ( DetFlags.detdescr.LAr_on() ):
    GeoModelSvc.DetectorTools["LArDetectorToolNV"].GeometryConfig = "FULL"
if ( DetFlags.detdescr.Tile_on() ):
    GeoModelSvc.DetectorTools["TileDetectorTool"].GeometryConfig = "FULL"


if ISF_Flags.DumpStoreGate() :
    StoreGateSvc.Dump = True


if ISF_Flags.RunVP1() :
    # VP1 part (better switch off PerMon when using VP1)
    from VP1Algs.VP1AlgsConf import VP1Alg
    topSequence += VP1Alg()

elif ISF_Flags.DoPerfMonStats() :
    # Performance Monitoring (VP1 does not like this)
    # https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD
    from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
    pmon_properties.PerfMonFlags.doMonitoring=True
    pmon_properties.PerfMonFlags.doSemiDetailedMonitoring=True


if ISF_Flags.DumpMcEvent() :
    # McEventCollection Dumper
    DumpMC = CfgMgr.DumpMC("DumpMC")
    DumpMC.McEventKey = "TruthEvent"
    topSequence += DumpMC


if ISF_Flags.RunValgrind() :
    # code profiling with valgrind
    from Valkyrie.JobOptCfg import ValgrindSvc
    valgrindSvc = ValgrindSvc ( OutputLevel = DEBUG )
    valgrindSvc.ProfiledAlgs +=  SimKernel
    #valgrindSvc.IgnoreFirstNEvents
    ServiceMgr += valgrindSvc

# useful for debugging:
print topSequence
