################################################################
## Minimal RecExCommon-based athena options to run AODSelect  ##
################################################################

# ==============================================================================
# Load your input file that you want to process
# ==============================================================================
from AthenaCommon.AthenaCommonFlags import jobproperties as jp
if 'inputFiles' in dir():
    jp.AthenaCommonFlags.FilesInput = inputFiles.split(',')
    del inputFiles

# ==============================================================================
# Set the number of events that you want to process
# ==============================================================================
jp.AthenaCommonFlags.EvtMax = 0

# ==============================================================================
# Configure RecExCommon (the mother of all job options in Athena) and schedule your DPD making
# ==============================================================================
from RecExConfig.RecFlags import rec
rec.doHist      = False
rec.doWriteTAG  = False
rec.doCBNT      = False
rec.doDPD       = True
rec.doAODSelect = True

# Configure the AODSelectFlags properly
from AODSelect.AODSelectFlags import AODSelectFlags
AODSelectFlags.doAll                            = True
AODSelectFlags.printAODSelect                   = True
AODSelectFlags.printAODSelect.ExitAfterPrintout = True

# ----------------------------------------------------------------------------------------------------
# If you have your own DPD Maker scripts
# (for examples, see in svn: PhysicsAnalysis/D2PDMaker/share/D2PD_ExampleSimple*.py ),
# then just append your script (wherever it is) to this list:
#       rec.DPDMakerScripts.append("MyPackage/MyScript")
# An example scripts is appended below, so you can see how it works!
# ----------------------------------------------------------------------------------------------------
rec.DPDMakerScripts.append("AODSelect/AODSelect_topOptions.py")


# ==============================================================================
# Now, include the master top options from RecExCommon.
# This automatically ensures that your Athena job will be set up correctly, i.e., if RecExCommon 
# doesn't work, the release is broken!
# ==============================================================================
include ("RecExCommon/RecExCommon_topOptions.py")
