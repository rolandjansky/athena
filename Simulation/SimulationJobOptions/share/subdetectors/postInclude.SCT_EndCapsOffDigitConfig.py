#########################################################
#
# SimulationJobOptions/postOptions.SCT_EndCapsOffDigitConfig.py
# John Chapman
#
# Switches off Digitization of the SCT EndCaps.
#
# This job option should be added via the postInclude
# command line argument. 
#
#########################################################

from AthenaCommon.DetFlags import DetFlags
if DetFlags.digitize.SCT_on():
    from AthenaCommon.AlgSequence import AlgSequence
    job = AlgSequence()
    from SCT_Digitization.SCT_DigitizationConf import SCT_Digitization
    sct = job.SCT_Digitization
    sct.BarrelOnly=True #only RDOs from Barrel Modules will be created
else:
    logDigitization_flags.warning('Attempted to switch off SCT EndCaps when the whole SCT is already switched off..')
