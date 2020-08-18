# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Hook for the DataQualityConfigurations package
__all__ = ['data09', 'data09_1beam','data09_calocomm', 'data09_calophys',
           'data09_cos', 'data09_idcomm', 'data09_larcomm', 'data09_muoncomm',
           'data09_tilecomm', 'data08', 'data08_1beam', 'data08_1beammag',
           'data08_cos', 'data08_cosmag', 'data08_idcomm', 'TestDisplay']
from .DQCDispatch import getmodule
