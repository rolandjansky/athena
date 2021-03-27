excludeList = []
excludeList += ['McEventCollection#TruthEventOLD',
                           'RecoTimingObj#EVNTtoHITS_timingsOLD']

from AthenaCommon.DetFlags import DetFlags
## Inner Detector
if DetFlags.ID_on():
    excludeList += ['SiHitCollection#*OLD',
                               'TRTUncompressedHitCollection#TRTUncompressedHitsOLD',
                               'TrackRecordCollection#CaloEntryLayerOLD']
## Calo
if DetFlags.Calo_on():
    excludeList += ['LArHitContainer#*OLD', 'LArHitContainer#*_G4', 
                               'LArHitContainer#LArHitEMB_FastCaloSim', 'LArHitContainer#LArHitEMEC_FastCaloSim', 
                               'LArHitContainer#LArHitFCAL_FastCaloSim', 'LArHitContainer#LArHitHEC_FastCaloSim',
                               'TileHitVector#*OLD', 'TileHitVector#TileHitVec_G4', 'TileHitVector#TileHitVec_FastCaloSim',
                               'CaloCalibrationHitContainer#*OLD',
                               'TrackRecordCollection#MuonEntryLayerOLD']
                               
## Muon
if DetFlags.Muon_on():
     excludeList += ['CSCSimHitCollection#CSC_HitsOLD',
                                'MDTSimHitCollection#MDT_HitsOLD',
                                'RPCSimHitCollection#RPC_HitsOLD',
                                'TGCSimHitCollection#TGC_HitsOLD',
                                'TrackRecordCollection#MuonExitLayerOLD']

#FIXME Add Renaming for Fwd Detector sim hits
## FwdRegion
if DetFlags.FwdRegion_on():
    excludeList += ["SimulationHitCollection#*OLD"]

## Lucid
if DetFlags.Lucid_on():
    excludeList += ["LUCID_SimHitCollection#*OLD"]

## ZDC
if DetFlags.ZDC_on():
    excludeList += ["ZDC_SimPixelHit_Collection#*OLD",
                               "ZDC_SimStripHit_Collection#*OLD"]
## ALFA
if DetFlags.ALFA_on():
    excludeList += ["ALFA_HitCollection#*","ALFA_ODHitCollection#*OLD"]

## AFP
if DetFlags.AFP_on():
    excludeList += ["AFP_TDSimHitCollection#*","AFP_SIDSimHitCollection#*OLD"]


## configure Athena for POOL persistency
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
streamSeq = AlgSequence("Streams")
sSoutStream = getattr(streamSeq, 'StreamHITS', None)
tSoutStream = None #getattr(topSequence, 'StreamHITS', None)
StreamHITS = None
if tSoutStream is not None:
    StreamHITS = tSoutStream
else:
    if sSoutStream is not None:
        StreamHITS = sSoutStream

if StreamHITS is not None:
    StreamHITS.ExcludeList += excludeList
    StreamHITS.AcceptAlgs = [] # HACK until I understand AcceptAlgs better

#StreamHITS.TakeItemsFromInput=False;
print StreamHITS
