# This job options compares geometry data from two sources
#
# In order run this script you need to define two sessions named Session0 and Session1
# in dblookup.xml and authentication.xml files, and point your runtime to these files
# using CORAL_DBLOOKUP_PATH and CORAL_AUTH_PATH respectively
#
# The SourceCompAlg will compare data corresponding to one global geometry tag specified
# via the SourceCompAlg.GlobalTag property. In case no value is set to this property,
# the algorithm will compare all ATLAS locked supported tags.
#
# Comparison results will be stored into GeometryDBTagDiff.log file in the run directory. 
# If no differences are found, then the log file will be empty
#

# Create an event selector:
import AthenaCommon.AtlasUnixGeneratorJob

# Generate one dummy event
from AthenaCommon.AppMgr import AppMgr as appMgr
appMgr.EvtMax = 1

# Configure the comparator algorithm
from RDBAccessSvc.RDBAccessSvcConf import SourceCompAlg
SourceCompAlg = SourceCompAlg()
#SourceCompAlg.GlobalTag = "ATLAS-R2-2016-01-00-01"

# Add the comparator to the AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += SourceCompAlg



