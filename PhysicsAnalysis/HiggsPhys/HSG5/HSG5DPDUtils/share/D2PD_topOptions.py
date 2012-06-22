##=============================================================================
## Name:        D2PD_topOptions.py
##
## Author:      Karsten Koeneke (DESY)
## Created:     April 2009

## Description: job options for all AOD->D2PD and D1PD->D2PD.
##
## Usage: Here, all neccessary job options for the D2PDs can be set.
##        To run, type:
##             athena D2PDMaker/D2PD_topOptions.py 2>&1 | tee log.txt
##=============================================================================


#==============================================================================
## Include the job property flags for this package 
#==============================================================================
from D2PDMaker.D2PDFlags import D2PDFlags

# A temporary fix for an unchecked StatusCode
from RecExConfig.RecFlags import rec
rec.abortOnUncheckedStatusCode=False


#==============================================================================
# If you have your own DPD Maker scripts
# (see:
#                    share/D2PD_ExampleSimple*.py
# for examples),
# then just append your script (wherever it is) to this list:
#
#       D2PDFlags.DPDMakerScripts.append("MyPackage/MyScript")
#
# The example scripts are appended below, so you can see how it works!
#==============================================================================

D2PDFlags.DPDMakerScripts.append("D2PDMaker/D2PD_ExampleSimpleTest.py")


#==============================================================================
# Load your input file
#==============================================================================
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.PoolAODInput=["AOD.079660._000094.pool.root.1"]



#==============================================================================
# Set the geometry for your input file
#==============================================================================
from AthenaCommon.GlobalFlags import globalflags
globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-08-00-01")


#==============================================================================
# You can change the location and name of your output file with
# these three flags:
#==============================================================================
#D2PDFlags.OutputDirectoryName          = "/my/directory/with/enough/space/"
#D2PDFlags.OutputPoolRootFileNamePrefix = "MyFirstD2PD"
#D2PDFlags.OutputMiddleName             = "test1"


#==============================================================================
# Set the number of events that you want to process
#==============================================================================
#athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)
if not vars().has_key('EvtMax'): EvtMax = 50#250
athenaCommonFlags.EvtMax=EvtMax


#==============================================================================
# Execute the dpd maker
#==============================================================================
include("D2PDMaker/aodtodpd.py")

