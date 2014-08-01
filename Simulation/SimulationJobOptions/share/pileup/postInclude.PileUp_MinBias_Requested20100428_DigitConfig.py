#########################################################
#
# SimulationJobOptions/postOptions.PileUp_Minbias_Requested20100428_DigitConfig.py
# John Chapman
#
# Increase the cache refresh rate to 100% for minbias and
# cavern background samples.
# Correct the amount of minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
#
# This job option should be added via the postInclude
# command line argument.
#
# FIXME: DEPRECATED Use
# postInclude=SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py,SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py
# instead DELETE THIS FILE
#
#########################################################

include("SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py")
include("SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py")
