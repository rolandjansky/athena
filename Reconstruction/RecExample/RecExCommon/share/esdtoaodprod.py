# steering file for ESD->AOD step
# see myTopOptions.py for more info

# jp.AthenaCommonFlags.PoolESDInput=["theesdfile"]

# PoolAODOutput="theoutput.AOD.pool.root"

rec.readESD=True
rec.doWriteESD=False 
rec.doWriteAOD=True 
rec.doAOD=True 
rec.doWriteTAG=False 

from RecExConfig.RecFlags import rec
rec.Production=True


# main jobOption
include ("RecExCommon/RecExCommon_flags.py")

# mof for default production
AODFlags.FastTrackParticle=False

include ("RecExCommon/RecExCommon_topOptions.py")
