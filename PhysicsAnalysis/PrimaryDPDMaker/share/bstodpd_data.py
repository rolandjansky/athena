##=============================================================================
## Name:        bstodpd_data.py
##
## Author:      David Cote (DESY)
## Created:     November 2008
##
## Description: This is the starting script to produce a DPD from RAW.
##=============================================================================

from RecExConfig.RecFlags  import rec
from AthenaCommon.BeamFlags import jobproperties
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.GlobalFlags import globalflags
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD

#Input
globalflags.InputFormat.set_Value_and_Lock('bytestream')
athenaCommonFlags.BSRDOInput.set_Value_and_Lock(['/castor/cern.ch/grid/atlas/DAQ/2008/87863/physics_BPTX/daq.NoTag.0087863.physics.BPTX.LB0000.SFO-1._0001.data'])
athenaCommonFlags.EvtMax=5

#Default configuration
rec.doAOD=False
rec.doWriteAOD=False
rec.doWriteTAG=False	
rec.doWriteTAGCOM=False
rec.doMonitoring=False
rec.doCBNT=False
rec.doESD=True

#auto-configuration
rec.triggerStream="BPTX"
rec.AutoConfiguration.append('FieldAndGeo')
rec.AutoConfiguration.append('BeamType')
rec.AutoConfiguration.append('ConditionsTag')

#data configuration
include( "RecExCommission/RecExCommissionFlags_jobOptions.py" )
#ATLASCosmicFlags.doFilteredESD=False 
include("RecExCommission/MinimalCommissioningSetup.py")

#DPD (skimmed bytestream)
rec.doDPD.set_Value_and_Lock( True )
rec.DPDMakerScripts.append("PrimaryDPDMaker/PrimaryDPDMaker.py")
from PrimaryDPDMaker.PrimaryDPDFlags import primDPD
#skimmed bytestream
primDPD.WriteRAW_IDPROJCOMM=True
primDPD.RAW_IDPROJCOMMName="mySkimmedBS"

#ESD (optional)
rec.doWriteESD=True
athenaCommonFlags.PoolESDOutput="myESD.pool.root"

include("RecExCommon/RecExCommon_topOptions.py")
