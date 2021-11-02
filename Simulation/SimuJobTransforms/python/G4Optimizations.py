# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def enableG4Optimizations(flags):
    """Enable G4 optimizations"""
    # Activate magnetic field switch off in central LAr calorimeter
    #  More info: https://its.cern.ch/jira/browse/ATLPHYSVAL-773
    flags.Sim.MuonFieldOnlyInCalo = True

    # Photon Russian Roulette
    # "Fast simulation" killing low energy photons with some probability.
    #  More info: https://its.cern.ch/jira/browse/ATLASSIM-4096
    flags.Sim.PRRThreshold = 0.5  # MeV
    flags.Sim.PRRWeight = 10.

    # Neutron Russian Roulette
    # "Fast simulation" killing low energy neutrons with some probability. 
    #  More info: its.cern.ch/jira/browse/ATLASSIM-3924
    flags.Sim.NRRThreshold = 2.  # MeV
    flags.Sim.NRRWeight = 10.

    # EM Range Cuts
    # Turn on range cuts for gamma processes (conv, phot, compt)
    # More info: https://its.cern.ch/jira/browse/ATLASSIM-3956
    flags.Sim.G4Commands += ['/process/em/applyCuts true']
