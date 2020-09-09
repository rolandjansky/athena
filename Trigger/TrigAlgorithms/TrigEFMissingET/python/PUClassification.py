#
#  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

""" Python version of the PUClassification enum """

# Neutral/forward components without tracks
NeutralForward = 1 << 0
# Tracks not associated to the PV
ChargedPU = 1 << 1
# Tracks associated to the PV
ChargedHS = 1 << 2

NFcPU = NeutralForward | ChargedPU
NFcHS = NeutralForward | ChargedHS
Charged = ChargedHS | ChargedPU
All = NeutralForward | ChargedPU | ChargedHS
