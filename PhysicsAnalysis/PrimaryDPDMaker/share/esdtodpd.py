##=============================================================================
## Name:        esdtodpd.py
##
## Authors:     Karsten Koeneke & David Cote (DESY)
## Created:     August 2008
##
## Description: Common job options for all ESD->DPD.
##
## Usage: This file should not be executed directly.
##        Instead, do:  athena PrimaryDPDMaker/PerfDPD_topOptions.py
##                      or
##                      athena PrimaryDPDMaker/CommDPD_topOptions.py
##
## Warning: this file is used by the job transform in production!!
##          Do not lock *any* parameter here. Do not specify input/output files, EvtMax, geometry or other global parameters.
##=============================================================================


#==================================
# PrimaryDPDMaker specific options
#==================================
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

#==================================
# RecExCommon specific options
#==================================
from RecExConfig.RecFlags import rec
# Turn off most of RecExCommon... (optional)
rec.doWriteRDO = False 
rec.doESD      = False 
rec.doWriteESD = False 
rec.doAOD      = False
rec.doWriteAOD = False
rec.doWriteTAG = False 
rec.doHist     = False
rec.doCBNT     = False
#DR leave perfmon on by default, needed also for DPD making
#rec.doPerfMon  = False

#varies with the settings
rec.doTruth = primDPD.UseMCTruth()
rec.doAODCaloCells = False

#==================================
#Setting AOD flags. Not much needs to be done if you read an ESD 'cause everything's already built.
#See https://twiki.cern.ch/twiki/bin/view/Atlas/UserAnalysisTest#The_AOD_Production_Flags
#==================================
from ParticleBuilderOptions.AODFlags import AODFlags
AODFlags.Photon             = False
AODFlags.Electron           = False
AODFlags.egammaTrackSlimmer = False
AODFlags.JetTag             = False
AODFlags.Streaming          = False
AODFlags.FastSimulation     = False
AODFlags.MissingEtTruth     = False
AODFlags.FastTrackParticle  = False
AODFlags.Tau                = False
AODFlags.Trigger            = False
AODFlags.ParticleJet        = False

#if primDPD.WritePreselectElectronStreamFromESDInput.writeAODContent :
#    rec.doAOD.set_Value_and_Lock(True)
#    rec.doAODCaloCells.set_Value_and_Lock(True)
#    AODFlags.Photon             = True
#    AODFlags.Electron           = True
#    AODFlags.egammaTrackSlimmer = True
#    pass
    
#These containers do not exists in the ESD. They have to be created on the fly if using AOD-based tools later on.
#AODFlags.Muon        = True
AODFlags.TruthParticleJet = rec.doTruth()
AODFlags.SpclMC           = rec.doTruth()


#==================================
# Mandatory for ESD->DPD
#==================================
rec.doDPD   = True
rec.readESD = True
rec.readAOD = False
rec.readRDO = False
rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")

## Write the used options to the log file
print(AODFlags)
print(rec)

# The job starts here!
include ("RecExCommon/RecExCommon_topOptions.py")
