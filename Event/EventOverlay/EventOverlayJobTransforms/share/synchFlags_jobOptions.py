# setup job fragment to synchronize between Detector Flags and Overlay Flags 
# Piyali Banerjee <piyali.banerjee@cern.sh> 07/06/10

from AthenaCommon.DetFlags import DetFlags

if DetFlags.ID_on():
    DetFlags.overlay.ID_setOn()
else:
    DetFlags.overlay.ID_setOff()
        
if DetFlags.pixel_on():
    DetFlags.overlay.pixel_setOn()
else:
    DetFlags.overlay.pixel_setOff()

if DetFlags.TRT_on():
    DetFlags.overlay.TRT_setOn()
else:
    DetFlags.overlay.TRT_setOff()

if DetFlags.SCT_on():
    DetFlags.overlay.SCT_setOn()
else:
    DetFlags.overlay.SCT_setOff()


#if DetFlags.Beam_on():
#    DetFlags.overlay.set_BeamOn()
#else:
#    DetFlags.overlay.set_BeamOff()
                
if DetFlags.BCM_on():
    DetFlags.overlay.BCM_setOn()
else:
    DetFlags.overlay.BCM_setOff()
                
    
if DetFlags.Calo_on():
    DetFlags.overlay.Calo_setOn()
else:
    DetFlags.overlay.Calo_setOff()
    
if DetFlags.LAr_on():
    DetFlags.overlay.LAr_setOn()
else:
    DetFlags.overlay.LAr_setOff()

if DetFlags.Tile_on():
    DetFlags.overlay.Tile_setOn()
else:
    DetFlags.overlay.Tile_setOff()

                
if DetFlags.Muon_on():
    DetFlags.overlay.Muon_setOn()
else:
    DetFlags.overlay.Muon_setOff()

if DetFlags.MDT_on():
    DetFlags.overlay.MDT_setOn()
else:
    DetFlags.overlay.MDT_setOff()

if DetFlags.CSC_on():
    DetFlags.overlay.CSC_setOn()
else:
    DetFlags.overlay.CSC_setOff()

if DetFlags.TGC_on():
    DetFlags.overlay.TGC_setOn()
else:
    DetFlags.overlay.TGC_setOff()

if DetFlags.RPC_on():
    DetFlags.overlay.RPC_setOn()
else:
    DetFlags.overlay.RPC_setOff()

if DetFlags.Forward_on():
    DetFlags.overlay.Forward_setOn()
#else:
#    DetFlags.overlay.set_ForwardOff()

if DetFlags.Lucid_on():
    DetFlags.overlay.Lucid_setOn()
#else:
#    DetFlags.overlay.set_LucidOff()

if DetFlags.ALFA_on():
    DetFlags.overlay.ALFA_setOn()
#else:
#    DetFlags.overlay.set_ALFAOff()
                
if DetFlags.ZDC_on():
    DetFlags.overlay.ZDC_setOn()
#else:
#    DetFlags.overlay.set_ZDCOff()

                
#if DetFlags.Trigger_on():
#    DetFlags.overlay.set_TriggerOn()
#else:
#    DetFlags.overlay.set_TriggerOff()
                
if DetFlags.LVL1_on():
    DetFlags.overlay.LVL1_setOn()
else:
    DetFlags.overlay.LVL1_setOff()

#Synchronizing of truth flag does not work
#if DetFlags.Truth_on():
#    DetFlags.overlay.set_TruthOn()
#else:
#    DetFlags.overlay.set_TruthOff()
