#########################################################
#
# SimulationJobOptions/postOptions.VeryLowLumi_900ns_DigitConfig.py
# John Chapman
#
# Correct the amount of minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
# Increase the cache refresh rate to 100% for minbias and
# cavern background samples.
#
# This job option should be added via the postInclude
# command line argument.
#
# FIXME: DEPRECATED Use
# postInclude=SimulationJobOptions/postOptions.Truth.pTmin100MeV.py,SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py
#
#########################################################

include("SimulationJobOptions/postOptions.Truth.pTmin100MeV.py")
include("SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py")
