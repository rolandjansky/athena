#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#


from .ConfigHelpers import AlgConfig
from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

                    

def metAthSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.athSequence, conf.inputMaker, conf.outputKey

def metRecoSequence(dummyFlags, **recoDict):
    conf = AlgConfig.fromRecoDict(**recoDict)
    return conf.recoSequence, conf.outputKey
