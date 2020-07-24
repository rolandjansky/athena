##=============================================================================
## Name:        aodtodpd.py
##
## Authors:     Karsten Koeneke & David Cote (DESY)
## Created:     August 2008
##
## Description: Common job options for all AOD->DPD.
##
## Usage: This file should not be executed directly.
##        Instead, do:  athena PrimaryDPDMaker/PhysDPD_topOptions.py
##
## Warning: this file is used by the job transform in production!!
##          Do not lock *any* parameter here. Do not specify input/output files, EvtMax, geometry or other global parameters.
##=============================================================================

from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
rec.doCBNT       = False
rec.doWriteESD   = False
rec.doWriteAOD   = False
rec.doAOD        = False
rec.doWriteTAG   = False
#rec.doPerfMon    = False
rec.doHist       = False
rec.doTruth      = False
#rec.LoadGeometry = False # <-- this one produces many WARNINGS...

# Mandatory for ESD->DPD
rec.doDPD   = True
rec.readAOD = True
rec.readESD = False
rec.readRDO = False
rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")

## Write the used options to the log file
print(rec)

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")
