# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from DQConfMakerBase.DQElements import *

_alg = DQAlgorithm(id='BinContentComp', libname='libdqm_algorithms.so')
_algpars = [DQAlgorithmParameter(name='NSigma', value=0.001),
            DQAlgorithmParameter(name='FixedError', value=0.001)]
_thresholds = [{'green': DQThreshold(name='NBins', value=0),
                'red': DQThreshold(name='NBins', value=0)}
               ]

def alg(obj):
    return [_alg, _algpars, _thresholds, None]

_summaryalg = DQAlgorithm(id='WorstCaseSummary', libname='libdqm_summaries.so')
def summaryalg(obj):
    return _summaryalg
