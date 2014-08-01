#########################################################
#
# SimulationJobOptions/postOptions.VeryLowLumiMinbiasPileUpOnMinbias_900ns_DigitConfig.py
# John Chapman
#
# Correct the amount of minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
# For very low luminosities keep truth information for
# particles with pT>100MeV rather than pT>400MeV.
# Increase the cache refresh rate to 100% for minbias and
# cavern background samples.
#
# This job option should be added via the postInclude
# command line argument.
#
# FIXME: DEPRECATED Use
# postInclude=SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py,SimulationJobOptions/postOptions.Truth.pTmin100MeV.py,SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py
#
#########################################################

include("SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py")
include("SimulationJobOptions/postOptions.Truth.pTmin100MeV.py")
include("SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py")
