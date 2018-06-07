#!/bin/sh
#
# art-description: Recursive geometry test on ATLAS-R2-2015-03-01-00 (MC15 default). Done only on G4 and GeoModel envelopes
# art-type: grid

athena G4AtlasTests/test_G4AtlasGeo.py

#TODO need to add a test to grep log files.
