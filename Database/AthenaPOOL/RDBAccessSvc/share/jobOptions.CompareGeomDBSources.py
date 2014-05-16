# This job options compares geometry data corresponding to a given ATLAS tag and read from two sources
#
# It is important therefore to have two sessions named Session0 and Session1
# in the dblookup.xml, both of them must correspond to geometry database instances
#
# Comparison results are printed out into a separate log file in the run directory: GeomDBDiff_<GlobalTag>.log
#

# Create an event selector:
import AthenaCommon.AtlasUnixGeneratorJob

# Generate one dummy event
from AthenaCommon.AppMgr import AppMgr as appMgr
appMgr.EvtMax = 1

# Configure the comparator algorithm -> Set ATLAS tag
from RDBAccessSvc.RDBAccessSvcConf import SourceCompAlg
SourceCompAlg = SourceCompAlg()
SourceCompAlg.GlobalTag = "ATLAS-GEO-18-00-00"

# Add the comparator to the AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += SourceCompAlg



