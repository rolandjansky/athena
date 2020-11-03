# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
# Python package containing lists of discriminants that are defined by
# the code here.

# we break this into two classes: those that you can calculate
# trivially from normal b-tagging information, and those that require
# us to more complicated things like lists of vertices or tracks.

complex_jet_discriminants = [
    "IP2D_isDefaults", "IP3D_isDefaults", "JetFitter_isDefaults",
    "SV1_nVtx","SV1_isDefaults",
    "secondaryVtx_isDefaults", "secondaryVtx_nTrks", "secondaryVtx_m",
    "secondaryVtx_E","secondaryVtx_EFrac","secondaryVtx_L3d",
    "secondaryVtx_Lxy",
    "secondaryVtx_min_trk_flightDirRelEta",
    "secondaryVtx_max_trk_flightDirRelEta",
    "secondaryVtx_avg_trk_flightDirRelEta",
    "min_trk_flightDirRelEta",
    "max_trk_flightDirRelEta",
    "avg_trk_flightDirRelEta",
    "rnnip_isDefaults",
    "IP2D_nTrks", "IP3D_nTrks",
]
trivial_jet_discriminants = [
    "IP2D_cu",
    "IP2D_bu",
    "IP2D_bc",
    "IP3D_cu",
    "IP3D_bu",
    "IP3D_bc",
    "JetFitter_deltaR",
]
