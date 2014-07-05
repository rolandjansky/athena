# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


def GetMuZTPChains():

    # second entry is the isolated 'type' - 0 for no isolation, 1 for EF only, 2 for EF & L2
    muztpchains = {
        "mu36_tight" : 0,
        "mu24i_tight" : 1,
        "mu50_MSonly_barrel_tight" : 0
        }
    return muztpchains
