#--------------------------------------------------------------
# Template jobOptions: SLHC 
#  - Simulation of pileup (minbias) events
# input environment variables:
#   EVTMAX = number of events to generate
#   OUTFILENAME = output data filename
#   LAYOUT = layout tag SLHC-pixel-strip[-dev]
# this script calls on jobOptions_pythia_SLHC.py,
# which uses the environment variable SEEDNUMBER
# to create random number seeds
#--------------------------------------------------------------

import os

from AthenaCommon.GlobalFlags import globalflags
globalflags.ConditionsTag = "OFLCOND-SDR-BS14T-ATLAS-00"

include("InDetSLHC_Example/preInclude.SLHC.py")

include("InDetSLHC_Example/preInclude.SiliconOnly.py")

from AthenaCommon.AthenaCommonFlags import jobproperties
jobproperties.AthenaCommonFlags.PoolHitsOutput=os.environ['OUTFILENAME']
jobproperties.AthenaCommonFlags.EvtMax=int(os.environ['EVTMAX'])

from G4AtlasApps.SimFlags import SimFlags
SimFlags.load_atlas_flags()
SimFlags.SimLayout='ATLAS-SLHC-01-00-00_VALIDATION'
SimFlags.SimLayout.set_On() 
#sets the EtaPhi, VertexSpread and VertexRange checks on
SimFlags.EventFilter.set_On()
#SimFlags.EventFilter.set_Value({'VertexPositioners': True, 'VertexRangeChecker': True})
SimFlags.EventFilter.set_Value({'VertexPositioners': True, 'VertexRangeChecker': True, 'EtaPhiFilters': False})

SimFlags.KinematicsMode='EventGenerator'
SimFlags.GeneratorPath=os.environ["SCRIPTDIR"] + '/jobOptions_pythia_SLHC.py' # path?

MessageSvc = Service( "MessageSvc" )
MessageSvc.OutputLevel = WARNING

from G4AtlasApps import SimKernel

# SLHC Flags -----------------------------------------------------
from InDetSLHC_Example.SLHC_JobProperties import SLHC_Flags
SLHC_Flags.SLHC_Version = os.environ['LAYOUT']

#--------------------------------------------------------------
# SLHC setup
#--------------------------------------------------------------
# set the path variables consistently
from InDetSLHC_Example.SLHC_Setup import SLHC_Setup
SLHC_Setup = SLHC_Setup()


# enter interactive mode
#theApp.initialize()

# this is used to filter generated particles
#from G4AtlasApps import AtlasG4Eng
#FilterMenu = AtlasG4Eng.G4Eng.menu_EventFilter()
#EPFilter = FilterMenu.getFilter('EtaPhiFilters')
#EPFilter.ClearEtaIntervals()
#EPFilter.AddEtaInterval(-3.0, 3.0)
##EPFilter.AddEtaInterval(0.0, 3.0)
###EPFilter.ClearPhiIntervals()
###EPFilter.AddPhiInterval(-0.3,0.3)

#G4Command=AtlasG4Eng.G4Eng.gbl.G4Commands()
#G4Command.tracking.verbose(1)

# start run after the interactive mode
#theApp.nextEvent( theApp.EvtMax )
#theApp.finalize()
#theApp.exit()
