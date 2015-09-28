#--------------------------------------------------------------------------
#
# MuonRec_topOptions.py
#
# Primary file to drive muon reconstruction, using RecExCommon
#
# Useage:  athena MuonRec_topOptions.py
#
# It will run the default muon reconstruction.
# If you want to run a customized muon reco job:
# get_joboptions MuonRecExample/MuonRec_myTopOptions.py
# and edit that file (not this one).
#
# To run Muon Combined reconstruction, use:
# MuonCombinedRecExample/MuonCombinedRec_myTopOptions.py
#
#
# Edward.Moyse@cern.ch
# Stephane.Willocq@cern.ch
# Martin.Woudstra@cern.ch
#
#--------------------------------------------------------------------------
from MuonRecExample.MuonRecUtils import logMuon
from MuonRecExample.MuonRecFlags import muonRecFlags
from MuonRecExample.MuonRecUtils import hasJobPropertyBeenSet
from RecExConfig.RecFlags import rec as recFlags
from RecExConfig.RecAlgsFlags import recAlgs as recAlgsFlags
from RecExConfig.RecConfFlags import recConfFlags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags

# configure flags so that only Muon Standalone reco is run
import MuonRecExample.MuonRecStandaloneOnlySetup

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Command line options (-c "EvtMax=10" etc)
#--------------------------------------------------------------

# Specify number of events to be processed
if 'EvtMax' in dir():
    athenaCommonFlags.EvtMax = EvtMax
    del EvtMax
elif not hasJobPropertyBeenSet(athenaCommonFlags.EvtMax):
    athenaCommonFlags.EvtMax = 10

# Number of events to skip
if 'SkipEvents' in dir():
    athenaCommonFlags.SkipEvents = SkipEvents
    del SkipEvents

# Overwrite Geometry tag
if 'DetDescrVersion' in dir():
    globalflags.DetDescrVersion = DetDescrVersion
    del DetDescrVersion

#--------------------------------------------------------------
# no perfmon if VP1
## if muonRecFlags.doVP1():
##     recFlags.doPerfMon = False

# synchronise some flags
recConfFlags.AllowIgnoreConfigError = athenaCommonFlags.AllowIgnoreConfigError()

# event selection. EventList contains the name of the file with one event number per line
if 'EventList' in dir():
    include("MuonRecExample/SelectEvents_jobOptions.py")

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")

# avoid the need for a PoolFileCatalog
ServiceMgr.PoolSvc.AttemptCatalogPatch = True


try:
    del topSequence.Stream2Filter
    logMuon.info("Removing Stream2Filter from topSequence")    
except AttributeError:
    pass


# remove TileMuonFitter from topSequence (CPU)
try:
   del topSequence.TileMuonFitter
   logMuon.info("Removing TileMuonFitter from topSequence")
except AttributeError:
   pass

# move triggeralg to end (calo has run after it)
try:
    # keep link to alg
    MuonCalibExtraTreeTriggerAlg = topSequence.MuonCalibExtraTreeTriggerAlg
    # then remove it from topSequence
    del topSequence.MuonCalibExtraTreeTriggerAlg
except AttributeError:
   pass
else:
   # and add at end (after Tile)
   topSequence += MuonCalibExtraTreeTriggerAlg




# ------------------------------------------------------------
# Event Displays
# ------------------------------------------------------------
if muonRecFlags.doVP1():
    from VP1Algs.VP1AlgsConf import VP1Alg
    topSequence += VP1Alg()

#--------------------------------------------------------------------------
# Dump configuration to file
#--------------------------------------------------------------------------
#from AthenaCommon.ConfigurationShelve import saveToAscii
#saveToAscii("config.txt")    
#--------------------------------------------------------------------------
# Nothing should be added after this line!!!
