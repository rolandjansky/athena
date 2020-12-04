# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


from TriggerJobOpts.TriggerFlags import TriggerFlags as TF

class TT:
    """
    Trigger type logic
    should also match StreamingDef.py definitions
    """
    rand        = 1 << 0
    zerobs      = 1 << 1  # trigger type for zero bias triggers
    calo        = 1 << 2        
    rpcin = tgc = 1 << 3  # Assign TGC to RPCIN to free one bit for new and CALOEM trigger type
    caloem      = 1 << 4  # to stream separately
    minb        = 1 << 5
    rpcout      = 1 << 6
    phys        = 1 << 7

    # Calibration requests
    #  From David B - for calibration events TriggerType
    #  will be reset by CTP - values below are placeholders
    calreq0 = 0x30
    calreq1 = 0x32
    calreq2 = 0x34

    if any([ x in TF.triggerMenuSetup() for x in
             [ 'InitialBeam_HI',
               'Cosmic_v3',
               'L1_pp',
               'L1_alfa',
               'MC_pp',
               'MC_HI',
               'MC_loose',
               'MC_tight',
               'Physics_pp',
               'Physics_default',
               'Physics_HI',
               ]]):
        caloem = 0

