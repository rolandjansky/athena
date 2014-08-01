#########################################################
#
# SimulationJobOptions/postOptions.VeryLowLumiMinbiasPileUpOnMinbiasSignal.py
# John Chapman
#
# Correct the amount of minbias pile-up in BC0 to account for
# the fact that there is already a minbias event (the
# signal event) in that bunch crossing.
#
# This job option should be added via the postInclude
# command line argument.
#
# FIXME: DEPRECATED Use
# postInclude=SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py,SimulationJobOptions/postOptions.Truth.pTmin100MeV.py
#
#########################################################

include("SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py")
include("SimulationJobOptions/postOptions.Truth.pTmin100MeV.py")
