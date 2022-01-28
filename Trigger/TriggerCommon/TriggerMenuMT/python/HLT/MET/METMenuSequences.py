#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from .ConfigHelpers import AlgConfig

def metMenuSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.menuSequence
