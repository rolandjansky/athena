include ("ParticleBuilderOptions/optESDtoAOD.py")

# AOD Flags are set here - comment otu and set appropriately
from ParticleBuilderOptions.AODFlags import AODFlags
#AODFlags.FastSimulation = False
AODFlags.Streaming = False

# DetFlags modifications are best set here (uncomment RecExCommon_flags first)
# include ("RecExCommon/RecExCommon_flags.py")
# switch off ID, calo, or muons
# DetFlags.ID_setOff()
# DetFlags.Calo_setOff()
# DetFlags.Muon_setOff()

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")
