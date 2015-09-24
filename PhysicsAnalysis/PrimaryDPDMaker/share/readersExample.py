##=============================================================================
## Name:        readersExample.py
##
## Author:      David Cote (DESY)
## Created:     December 2008
##=============================================================================

from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

input='/afs/cern.ch/user/c/cote/scratch0/Pool/EGAMTAUCOMM.pool.root'

globalflags.InputFormat.set_Value_and_Lock('pool')
rec.readESD.set_Value_and_Lock( True )
rec.readRDO.set_Value_and_Lock( False )
athenaCommonFlags.PoolESDInput.set_Value_and_Lock([input])
athenaCommonFlags.EvtMax=10

rec.triggerStream="IDCosmic"
rec.AutoConfiguration.append('FieldAndGeo')
rec.AutoConfiguration.append('BeamType')
rec.AutoConfiguration.append('ConditionsTag')

rec.doWriteESD=False
rec.doAOD=False
rec.doWriteAOD=False
rec.doWriteTAG=False	
rec.doWriteTAGCOM=False
rec.doMonitoring=False
rec.doCBNT=False
include("RecExCommission/MinimalCommissioningSetup.py")

#DPD
## rec.doDPD.set_Value_and_Lock( True )
## rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")
## primDPD.WritePrescaledESDStream.set_Value_and_Lock( True )

include("RecExCommon/RecExCommon_topOptions.py")

from EventBookkeeperTools.ReadSkimDecisions import SkimDecisionsReader
sd=SkimDecisionsReader()
sd.SkimDecisionsContainerName="EGamTauComm_SkimDecisionsContainer"
topSequence+=sd

from EventBookkeeperTools.EventBookkeeperToolsConf import ReadEventBookkeepers
BookkeeperReader = ReadEventBookkeepers("BookkeeperReader")
BookkeeperReader.EventBookkeeperCollectionName="EventBookkeepers"
BookkeeperReader.OutputLevel = DEBUG 
topSequence+=BookkeeperReader

