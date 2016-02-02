include.block("HIRecExample/hip_flagsAOD.py")

# put here HeavyIon-proton specific AOD configuration 

# MC
from TriggerJobOpts.TriggerFlags import TriggerFlags
TriggerFlags.AODEDMSet.set_Value_and_Lock("AODFULL")
