##=============================================================================
## Name:        RAWPerfDPD_topOptions.py
##
## Authors:     Karsten Koeneke (DESY)
## Created:     March 2009
##
## Description: job options for all RAW->DPD.
##
## Usage: athena PrimaryDPDMaker/RAWPerfDPD_topOptions.py 2>&1 | tee log.txt
##        Edit this file to disable DPDs or change the global setup
##
##=============================================================================

# Import the needed modules
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from RecExConfig.RecFlags  import rec
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

# Turn on all RAW Performance DPD
rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Zee.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Wenu.py")
rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Zmumu.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWPerfDPD_Wmunu.py")
#rec.DPDMakerScripts.append("PrimaryDPDMaker/RAWCommDPD_IDProj.py")
#primDPD.WriteRAWPerfDPD_ZEE    = True
#primDPD.WriteRAWPerfDPD_WENU   = True
#primDPD.WriteRAWPerfDPD_ZMUMU  = True
#primDPD.WriteRAWPerfDPD_WMUNU  = True
#primDPD.WriteRAWCommDPD_IDPROJ = True

if not vars().has_key('UseMCTruth'): UseMCTruth=False
primDPD.UseMCTruth=UseMCTruth

# Input
import os
inFile=os.getenv("DataMC")+"data/RAW/data11_7TeV.00191920.physics_JetTauEtmiss.merge.RAW._lb0257._SFO-9._0001.1.10evts"
#inFile=os.getenv("DataMC")+"data/RAW/user.jboyd.DiLeptonicTopSkim.RAW.Run183347.10events.dat"
globalflags.InputFormat.set_Value_and_Lock('bytestream')
# athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/afs/cern.ch/atlas/offline/test/data11_7TeV.00182796.physics_JetTauEtmiss.merge.RAW._lb0300._SFO-10._0001.1.10evts.data'])
athenaCommonFlags.BSRDOInput.set_Value_and_Lock([inFile])

#globalflags.DetDescrVersion.set_Value_and_Lock("ATLAS-GEO-02-01-00")
#globalflags.DataSource.set_Value_and_Lock('geant4')

#auto-configuration
rec.triggerStream="BPTX"
rec.AutoConfiguration.append('FieldAndGeo')
rec.AutoConfiguration.append('BeamType')
rec.AutoConfiguration.append('ConditionsTag')

#data configuration
#include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
#ATLASCosmicFlags.doFilteredESD=False
#include("RecExCommission/MinimalCommissioningSetup.py")


#athenaCommonFlags.SkipEvents.set_Value_and_Lock(0)
if not vars().has_key('EvtMax'): EvtMax = -1
athenaCommonFlags.EvtMax=EvtMax

# Turn off most of RecExCommon... (optional)
rec.doWriteRDO     = False
rec.doESD          = True
rec.doWriteESD     = False
rec.doAOD          = False
rec.doWriteAOD     = False
rec.doWriteTAG     = False
rec.doHist         = False
rec.doCBNT         = False
rec.doFileMetaData = False

globalflags.InputFormat.set_Value_and_Lock('bytestream')
rec.doDPD   = True
rec.readRDO = True
rec.readESD = False
rec.readAOD = False

#the job starts here!
#include("PrimaryDPDMaker/bstodpd.py")
include ("RecExCommon/RecExCommon_topOptions.py")
