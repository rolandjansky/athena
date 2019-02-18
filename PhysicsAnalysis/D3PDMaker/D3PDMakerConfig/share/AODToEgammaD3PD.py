# $Id$
#
# @file D3PDMakerConfig/share/AODToEgammaD3PD.py
# @author scott snyder <snyder@bnl.gov>
# @date Aug, 2009
# @brief Example for building an egamma D3PD from an AOD.
#


###################################################################3
# Define the input file here.
#

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.FilesInput= ["AOD.pool.root"]


###################################################################3
# Define the output file here.
#

if not globals().get('tupleFileOutput'):
    tupleFileOutput = 'egamma.root'

from D3PDMakerConfig.D3PDProdFlags import prodFlags
prodFlags.WriteEgammaD3PD = True
prodFlags.WriteEgammaD3PD.FileName = tupleFileOutput
prodFlags.WriteEgammaD3PD.lock()

###################################################################3
# Define other job options here.
#

athenaCommonFlags.EvtMax = -1

# Example of changing D3PD maker flags.
from D3PDMakerConfig.D3PDMakerFlags import D3PDMakerFlags
#D3PDMakerFlags.DoTrigger = False


###################################################################
# Configure RecExCommon.
#

from RecExConfig.RecFlags import rec
rec.DPDMakerScripts.append( "D3PDMakerConfig/EgammaD3PD_prodJobOFragment.py" )
rec.doDPD.set_Value_and_Lock( True )
rec.doCBNT.set_Value_and_Lock( False )
rec.doWriteTAG.set_Value_and_Lock( False )
rec.doWritexAOD.set_Value_and_Lock( False )
rec.doTau.set_Value_and_Lock( False )
rec.doCaloRinger.set_Value_and_Lock( False )
from MuonRecExample.MuonRecFlags import muonRecFlags
muonRecFlags.doMSVertex.set_Value_and_Lock( False )

#from xAODJetCnv import ParticleJetCompatibility

# Block loading conditions folders we won't need.
blocked_folders = [
    '/CALO/Identifier/CaloTTOnAttrIdMapAtlas',
    '/CALO/Identifier/CaloTTOnOffIdMapAtlas',
    '/CALO/Identifier/CaloTTPpmRxIdMapAtlas',
    '/LAR/ElecCalibMC/HVScaleCorr',
    '/LAR/ElecCalibMC/Pedestal',
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
    '/CALO/H1Weights/H1WeightsKt4Topo',
    '/Indet/PixelDist',
    ]
from IOVDbSvc.CondDB import conddb
for f in blocked_folders:
    conddb.blockFolder (f)


include( "RecExCommon/RecExCommon_topOptions.py" )
