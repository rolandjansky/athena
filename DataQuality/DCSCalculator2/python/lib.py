# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging; log = logging.getLogger("DCSCalculator2.lib")

from DQUtils.iov_arrangement import flatten_channels
#from DQUtils.sugar import RunLumi, IOVSet

from .subdetector import DCSC_Subdetector, DCSC_DefectTranslate_Subdetector, DCSC_Subdetector_DefectsOnly

from .variable import (DCSC_Variable, DCSC_Variable_With_Mapping,
                       DCSC_Global_Variable, DCSC_Defect_Global_Variable)

from .subdetector import GoodIOV, OUT_OF_CONFIG

from .libcore import (make_multi_mapping,
                      map_channels, connect_adjacent_iovs_defect)

__all__ = ['flatten_channels',
           'DCSC_Subdetector', 'DCSC_DefectTranslate_Subdetector',
           'DCSC_Subdetector_DefectsOnly', 'DCSC_Variable',
           'DCSC_Variable_With_Mapping', 'DCSC_Global_Variable',
           'DCSC_Defect_Global_Variable',
           'GoodIOV', 'OUT_OF_CONFIG',
           'make_multi_mapping', 'map_channels',
           'connect_adjacent_iovs_defect']
