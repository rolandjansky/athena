# setup job fragment to synchronize between Detector Flags and Overlay Flags 
# Piyali Banerjee <piyali.banerjee@cern.sh> 07/06/10

from AthenaCommon.DetFlags import DetFlags

from OverlayCommonAlgs.OverlayFlags import OverlayFlags


if DetFlags.ID_on():
    OverlayFlags.set_InDetOn()
else:
    OverlayFlags.set_InDetOff()
        
if DetFlags.pixel_on():
    OverlayFlags.set_PixelOn()
else:
    OverlayFlags.set_PixelOff()

if DetFlags.TRT_on():
    OverlayFlags.set_TRTOn()
else:
    OverlayFlags.set_TRTOff()

if DetFlags.SCT_on():
    OverlayFlags.set_SCTOn()
else:
    OverlayFlags.set_SCTOff()


#if DetFlags.Beam_on():
#    OverlayFlags.set_BeamOn()
#else:
#    OverlayFlags.set_BeamOff()
                
if DetFlags.BCM_on():
    OverlayFlags.set_BCMOn()
else:
    OverlayFlags.set_BCMOff()
                
    
if DetFlags.Calo_on():
    OverlayFlags.set_CaloOn()
else:
    OverlayFlags.set_CaloOff()
    
if DetFlags.LAr_on():
    OverlayFlags.set_LArOn()
else:
    OverlayFlags.set_LArOff()

if DetFlags.Tile_on():
    OverlayFlags.set_TileOn()
else:
    OverlayFlags.set_TileOff()

                
if DetFlags.Muon_on():
    OverlayFlags.set_MuonOn()
else:
    OverlayFlags.set_MuonOff()

if DetFlags.MDT_on():
    OverlayFlags.set_MDTOn()
else:
    OverlayFlags.set_MDTOff()

if DetFlags.CSC_on():
    OverlayFlags.set_CSCOn()
else:
    OverlayFlags.set_CSCOff()

if DetFlags.TGC_on():
    OverlayFlags.set_TGCOn()
else:
    OverlayFlags.set_TGCOff()

if DetFlags.RPC_on():
    OverlayFlags.set_RPCOn()
else:
    OverlayFlags.set_RPCOff()

if DetFlags.Forward_on():
    OverlayFlags.set_ForwardOn()
#else:
#    OverlayFlags.set_ForwardOff()

if DetFlags.Lucid_on():
    OverlayFlags.set_LucidOn()
#else:
#    OverlayFlags.set_LucidOff()

if DetFlags.ALFA_on():
    OverlayFlags.set_ALFAOn()
#else:
#    OverlayFlags.set_ALFAOff()
                
if DetFlags.ZDC_on():
    OverlayFlags.set_ZDCOn()
#else:
#    OverlayFlags.set_ZDCOff()

                
#if DetFlags.Trigger_on():
#    OverlayFlags.set_TriggerOn()
#else:
#    OverlayFlags.set_TriggerOff()
                
if DetFlags.LVL1_on():
    OverlayFlags.set_LVL1On()
else:
    OverlayFlags.set_LVL1Off()

#Synchronizing of truth flag does not work
#if DetFlags.Truth_on():
#    OverlayFlags.set_TruthOn()
#else:
#    OverlayFlags.set_TruthOff()
