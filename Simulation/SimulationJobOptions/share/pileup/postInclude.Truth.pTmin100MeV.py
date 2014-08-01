#########################################################
#
# SimulationJobOptions/postOptions.Truth.pTmin100MeV.py
# John Chapman
#
# For very low luminosities keep truth information for
# particles with pT>100MeV rather than pT>400MeV.
#
# This job option should be added via the postInclude
# command line argument.
#
#########################################################


from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from AthenaCommon.CfgGetter import getPublicTool
getPublicTool("MergeMcEventCollTool").ptMin = 100.0 #400.0 MeV
