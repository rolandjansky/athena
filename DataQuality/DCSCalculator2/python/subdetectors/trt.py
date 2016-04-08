# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from .lib import DCSC_Subdetector, DCSC_Variable
from DQUtils.sugar import IOVSet

def offset_channelids(iovs, offset):
    return IOVSet(iov._replace(channel=iov.channel+offset) for iov in iovs)

BAR_MAX_ID, ECA_MAX_ID, ECC_MAX_ID = 710, 640, 640

# These six modules do not exist
BAR_REMOVED_IDS = 6

# Strategy here is to remap the different folders onto different input channel 
# id regions. Then we have to construct a modified mapping in TRT to map
# those inputs onto the outputs.

class DCSC_Variable_TRT_HV(DCSC_Variable):
    """
    Different regions are stored in different folders, but they are the same
    variable.
    """
    
    def read(self, query_range, folder_base, folder_name):
        normal_read = super(DCSC_Variable_TRT_HV, self).read
        
        iovs_bar = normal_read(query_range, folder_base, "HV/BARREL")
        iovs_eca = normal_read(query_range, folder_base, "HV/ENDCAPA")
        iovs_ecc = normal_read(query_range, folder_base, "HV/ENDCAPC")
        
        # Filter out unused IDs
        iovs_bar = [i for i in iovs_bar if 0 < i.channel <= BAR_MAX_ID]
        iovs_eca = [i for i in iovs_eca if 0 < i.channel <= ECA_MAX_ID]
        iovs_ecc = [i for i in iovs_ecc if 0 < i.channel <= ECC_MAX_ID]
        
        iovs_eca = offset_channelids(iovs_eca, BAR_MAX_ID)
        iovs_ecc = offset_channelids(iovs_ecc, BAR_MAX_ID + ECA_MAX_ID)
        
        cool_default_sort = lambda i: (i.channel, i.since)
        
        return sorted(iovs_bar + iovs_eca + iovs_ecc, key=cool_default_sort)
        
class TRT(DCSC_Subdetector):

    # Disabled until mismatch in number of channels is handled.    
    #__DISABLED__ = True
    
    folder_base = "/TRT/DCS"
    
    mapping = {        
        121: range(1, BAR_MAX_ID + 1 - BAR_REMOVED_IDS),
        124: range(1 + BAR_MAX_ID, 1 + BAR_MAX_ID + ECA_MAX_ID),
        125: range(1 + BAR_MAX_ID + ECA_MAX_ID, 1 + BAR_MAX_ID + ECA_MAX_ID + ECC_MAX_ID),
    }

    variables = [
        DCSC_Variable_TRT_HV("TRT HV", lambda iov: 1400 < iov.OUTPUTVOLTAGE_VALUE < 3000)
    ]
    
    dead_fraction_caution = 0.05
    dead_fraction_bad = 0.20

