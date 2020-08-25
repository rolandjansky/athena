from __future__ import print_function
print("=== JiveXML_fromAOD.py: ")
print("=== Producing XML output for Atlantis event display")
print("=== 25 events processed by default.")
print("=== Make sure you have a file AOD.pool.root connected with ")
print("=== symbolic link in this directory, e.g.")
print("===    ln -sf myAOD_runNumber.pool.root AOD.pool.root  ")
print("=== Make sure you have matching geometry, check with: ")
print("===    os.system('dumpVersionTags.py AOD.pool.root | grep \"GeoAtlas   \"') ")
import os
import sys
from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
if os.path.isfile("AOD.pool.root"):
    athenaCommonFlags.PoolAODInput.set_Value_and_Lock(['AOD.pool.root'])
    print("=== AOD file found ! ")
    #### EDIT geometry version here if necessary: 
    #globalflags.DetDescrVersion.set_Value_and_Lock('ATLAS-GEONF-08-00-00')
    #### EDIT number of events here, default 25:
    athenaCommonFlags.EvtMax.set_Value_and_Lock(25)
    include ("JiveXML/JiveXML_jobOptions_AODRecEx.py")
else:
    print("=== AOD.pool.root not found. Exiting.")
    sys.exit(0)
    
