##=============================================================================
## Name:        LongLivedDPD_topOptions.py
##
## Description: job options for all Physics ESD->DPD.
##
## Usage: All neccessary job options for the RPV/LL DPDs can be set here.
##        To run, type:
##             athena LongLivedParticleDPDMaker/LongLivedDPD_topOptions.py
##=============================================================================

## use AutoConfiguration to setup everything


from RecExConfig.RecFlags import rec
rec.AutoConfiguration=['everything']


## Include the job property flags for this package 
from LongLivedParticleDPDMaker.LongLivedDPDFlags import *
## Include the job property flags for the physics DPDs in this package 
from LongLivedParticleDPDMaker.PrimaryDPDFlags_RPVLLStream import primRPVLLDESDM

# Switch on the writing of the physics DPDs
primDPD.WriteRPVLLStream = True


primDPD.OutputDirectoryName=""
primDPD.OutputMiddleName=""
if not vars().has_key('UseMCTruth'): UseMCTruth=False

primDPD.UseMCTruth=UseMCTruth
primDPD.ApplySkimming               = True # Default is True


## configure the RPVLL DESD - turn filters on or off
primRPVLLDESDM.doDV=True
primRPVLLDESDM.doSmpCaloId=True
primRPVLLDESDM.doSmpMs=True
primRPVLLDESDM.doKinkedTrack=True
primRPVLLDESDM.doHips=True
primRPVLLDESDM.doStopped=True
primRPVLLDESDM.doHV=True
primRPVLLDESDM.doVH=True

# Some global flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags

include("/afs/cern.ch/atlas/project/PAT/data/data.physics_Muons.ESD.py")

athenaCommonFlags.FilesInput= testInputFiles


from AthenaCommon.GlobalFlags import globalflags


if not vars().has_key('EvtMax'): EvtMax =300
athenaCommonFlags.EvtMax=EvtMax

# Execute the dpd maker
include("PrimaryDPDMaker/esdtodpd.py")



RPVLL.SetOutputFileName("myTestDESD.root")
