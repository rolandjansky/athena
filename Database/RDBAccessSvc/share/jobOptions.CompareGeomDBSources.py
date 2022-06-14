# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

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

from AthenaCommon import Logging

# Create an event selector:
import AthenaCommon.AtlasUnixGeneratorJob

# Generate one dummy event
from AthenaCommon.AppMgr import AppMgr as appMgr
appMgr.EvtMax = 1

# Set the SupportedGeometry flag
from AthenaCommon.AppMgr import release_metadata
rel_metadata = release_metadata()
relversion = rel_metadata['release'].split('.')
if len(relversion) < 3:
    relversion = rel_metadata['base release'].split('.')

# Configure the comparator algorithm
from RDBAccessSvc.RDBAccessSvcConf import SourceCompAlg
SourceCompAlg = SourceCompAlg()
Logging.log.info("CompareGeomDBSource.py obtained major release version %s", relversion[0])
SourceCompAlg.SupportedGeometry = relversion[0]

# Add the comparator to the AlgSequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
topSequence += SourceCompAlg



