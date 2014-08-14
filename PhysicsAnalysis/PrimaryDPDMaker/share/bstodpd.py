##=============================================================================
## Name:        bstodpd.py
##
## Authors:     Karsten Koeneke (DESY)
## Created:     March 2009
##
## Description: Common job options for all RAW->DPD.
##
## Usage: This file should not be executed directly.
##        Instead, do:  athena PrimaryDPDMaker/RAWPerfDPD_topOptions.py
##
## Warning: this file is used by the job transform in production!!
##          Do not lock *any* parameter here. Do not specify input/output files, EvtMax, geometry or other global parameters.
##=============================================================================


#==================================
# PrimaryDPDMaker specific options
#==================================
from AthenaCommon.GlobalFlags import globalflags
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD


#==================================
# RecExCommon specific options
#==================================
from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
rec.doWriteRDO = False 
rec.doESD      = True 
rec.doWriteESD = False 
rec.doAOD      = False 
rec.doWriteAOD = False
rec.doWriteTAG = False 
rec.doHist     = False
rec.doCBNT     = False
#DR rec.doPerfMon  = False

#varies with the settings
rec.doTruth = primDPD.UseMCTruth()


#==================================
# Mandatory for BS->DPD
#==================================
globalflags.InputFormat.set_Value_and_Lock('bytestream')
rec.doDPD   = True
rec.readRDO = True
rec.readESD = False
rec.readAOD = False
rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")

