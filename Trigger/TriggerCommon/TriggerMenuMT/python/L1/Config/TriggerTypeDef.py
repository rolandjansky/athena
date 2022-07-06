# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

class TT(object):
    """
    Trigger type logic
    should also match StreamingDef.py definitions
    """
    rand        = 1 << 0  # Defined for all RDX items, but not used by any FEs
    zerobs      = 1 << 1  # Used by LAr, Tile (and others?) -> data overlay
    calo        = 1 << 2  # Used by LAr/LATOME for saturated pulse
    muon        = 1 << 3  # Used by TGC
    lardigital  = 1 << 4  # Used by LAr/LATOME
    nsw         = 1 << 5  # Used on NSW FEs
    nim = alfa  = 1 << 6  # Used by TGC Burst, and by ALFA in special runs
    phys        = 1 << 7  # Used in conjunction with the other bits

    # Calibration requests
    #  From David B - for calibration events TriggerType
    #  will be reset by CTP - values below are placeholders
    calreq0 = 0x30
    calreq1 = 0x32
    calreq2 = 0x34
