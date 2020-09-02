# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
from RecExConfig.RecFlags import rec
from TriggerJobOpts.TriggerFlags import TriggerFlags
from AthenaCommon.GlobalFlags import jobproperties

trtEnabled=DetFlags.detdescr.TRT_on()
# Set the appropriate DetFlags
if (not rec.readESD()) and jobproperties.Global.InputFormat() == "pool" and DetFlags.detdescr.ID_on():   
    DetFlags.makeRIO.ID_setOn()   
    DetFlags.readRIOBS.ID_setOn()   
    DetFlags.writeBS.ID_setOn()
    if not trtEnabled:
        DetFlags.makeRIO.TRT_setOff()   
        DetFlags.readRIOBS.TRT_setOff()   
        DetFlags.writeBS.TRT_setOff()
        

if (not rec.readESD()) and jobproperties.Global.InputFormat() == "pool" and DetFlags.detdescr.Calo_on(): 
    DetFlags.detdescr.ID_setOn() # needed for RegionSelectorLUT
    if not trtEnabled:
        DetFlags.detdescr.TRT_setOff() 
    DetFlags.makeRIO.Calo_setOn()
    DetFlags.readRIOBS.Calo_setOn()
    DetFlags.writeBS.Calo_setOn()

#if (not rec.readESD()) and GlobalFlags.InputFormat.is_pool() and DetFlags.detdescr.Muon_on():
#    DetFlags.makeRIO.Muon_setOn()
#    DetFlags.readRIOBS.Muon_setOn()
#    DetFlags.writeBS.Muon_setOn()

#
# Switch off direct formation of Cells from hits
#    
if not rec.readESD() and TriggerFlags.doCalo():
    from AthenaCommon.Include import include
    include ("CaloRec/CaloCellMaker_config.py")
    CaloCellMakerFlags.doLArHitToCellDirect=False  # noqa: F821 old job options, flags from include above
    CaloCellMakerFlags.doTileHitToRawChannelDirect=False  # noqa: F821 old job options, flags from include above
