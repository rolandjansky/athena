# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TT(object):
    """
    Trigger type logic
    should also match StreamingDef.py definitions
    """
    rand        = 1 << 0
    zerobs      = 1 << 1  # trigger type for zero bias triggers
    calo        = 1 << 2        
    muon        = 1 << 3
    lardemo     = 1 << 4
    ftk = minb  = 1 << 5  # min-bias TT is needed for TM internal use (L1Seeds.py) 
    nim = alfa  = 1 << 6
    phys        = 1 << 7

    # Calibration requests
    #  From David B - for calibration events TriggerType
    #  will be reset by CTP - values below are placeholders
    calreq0 = 0x30
    calreq1 = 0x32
    calreq2 = 0x34
