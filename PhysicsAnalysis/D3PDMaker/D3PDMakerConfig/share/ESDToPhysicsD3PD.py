# $Id$
#
# @file D3PDMakerConfig/share/ESDToPhysicsD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building a physics D3PD from an ESD.
#


####################################################################
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["ESD.pool.root"]

####################################################################
# Define the output file here.
#

if not globals().get('inputFileType'):
    inputFileType = 'ESD'

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'physics.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WritePhysicsD3PD = True
prodFlags.WritePhysicsD3PD.FileName = tupleFileOutput
prodFlags.WritePhysicsD3PD.lock()

####################################################################
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.DoTrigger = False


####################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "D3PDMakerConfig/PhysicsD3PD_prodJobOFragment.py" )
rec.doCBNT.set_Value_and_Lock( False )
rec.doAOD.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doWriteAOD.set_Value_and_Lock( False )
rec.doWritexAOD.set_Value_and_Lock( False )
rec.doMuonCombined.set_Value_and_Lock (False)

if not globals().has_key ('block_override'):
    block_override = []
# Block loading conditions folders we won't need.
blocked_folders = [
    '/CALO/Identifier/CaloTTOnAttrIdMapAtlas',
    '/CALO/Identifier/CaloTTOnOffIdMapAtlas',
    '/CALO/Identifier/CaloTTPpmRxIdMapAtlas',
    '/LAR/BadChannels/BadChannels',
    '/LAR/BadChannels/MissingFEBs',
    '/LAR/ElecCalibMC/AutoCorr',
    '/LAR/ElecCalibMC/DAC2uA',
    '/LAR/ElecCalibMC/HVScaleCorr',
    '/LAR/ElecCalibMC/MinBias',
    '/LAR/ElecCalibMC/MinBiasAverage',
    '/LAR/ElecCalibMC/MphysOverMcal',
    '/LAR/ElecCalibMC/Noise',
    '/LAR/ElecCalibMC/Pedestal',
    '/LAR/ElecCalibMC/Ramp',
    '/LAR/ElecCalibMC/Shape',
    '/LAR/ElecCalibMC/fSampl',
    '/LAR/ElecCalibMC/uA2MeV',
    '/LAR/Identifier/FebRodAtlas',
    '/LAR/Identifier/LArTTCellMapAtlas',
    '/LAR/Identifier/OnOffIdAtlas',
    '/MDT/DCS/DROPPEDCH',
    '/MDT/DCS/PSLVCHSTATE',
    '/PIXEL/PixdEdx',
    '/TILE/OFL02/NOISE/AUTOCR',
    '/TILE/OFL02/PULSESHAPE/PHY',
    '/TILE/ONL01/FILTER/OF2/PHY',
    '/TILE/ONL01/NOISE/OFNI',
    '/TRT/Calib/HTCalib',
    '/TRT/Calib/PID',
    '/TRT/Calib/PID_RToT',
    '/TRT/Calib/PID_RToTver_New',
    '/TRT/Calib/PIDver_New',
    '/TRT/Calib/ToTCalib',
    '/TRT/Cond/Status',
    '/TRT/Cond/StatusPermanent',
    ]
from IOVDbSvc.CondDB import conddb
for f in blocked_folders:
    if f not in block_override:
        conddb.blockFolder (f)

include ("RecExCommon/RecExCommon_topOptions.py")
