# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from ..lib import DCSC_Subdetector, DCSC_Variable

class TDQ(DCSC_Subdetector):
    
    folder_base = "/TDQ/DCS"
    
    mapping = {
        401: list(range( 5, 32)),
        402: list(range(51, 68)),
        403: list(range(32, 45)),
        404: list(range( 1,  5)) + list(range(45, 51)),
    }

    variables = [
        DCSC_Variable("WIENER/LVL1", lambda iov: iov.General_Status_GetPowerOn is True),
    ]
    
    dead_fraction_caution = 0.00000005
    dead_fraction_bad     = 0.0000001
