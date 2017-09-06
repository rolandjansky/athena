#This is a preInclude file to be used to run the BLS stream in production.
#Author: James Walder
from InDetRecExample.InDetJobProperties import InDetFlags
InDetFlags.KeepFirstParameters.set_Value_and_Lock(True)
InDetFlags.keepAdditionalHitsOnTrackParticle.set_Value_and_Lock(True)
InDetFlags.cutLevel.set_Value_and_Lock(13)

TriggerFlags.AODEDMSet.set_Value_and_Lock("AODBLSSLIM")

