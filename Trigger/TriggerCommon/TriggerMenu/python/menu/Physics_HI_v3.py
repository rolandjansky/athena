# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#------------------------------------------------------------------------#
#------------------------------------------------------------------------#
def setupMenu():
    
    from TriggerMenu.menu.TriggerPythonConfig import TriggerPythonConfig
    from TriggerJobOpts.TriggerFlags          import TriggerFlags
    from AthenaCommon.Logging                 import logging
    log = logging.getLogger( 'TriggerMenu.menu.Physics_HI_v3.py' )

    from TriggerMenu.TriggerConfigLVL1 import TriggerConfigLVL1 as tcl1
    
    if tcl1.current:
        log.info("L1 items: %s " % tcl1.current.menu.items.itemNames())
    else:
        log.info("ERROR L1 menu has not yet been defined")


    # INPUT FORMAT FOR CHAINS:
    # ['chainName', chainCounter, 'L1itemforchain', [L1 items for chainParts], [stream], [groups], EBstep]

    TriggerFlags.Slices_all_setOff()

    TriggerFlags.MuonSlice.signatures = [
        ]

    TriggerFlags.JetSlice.signatures = [   
        ]

    TriggerFlags.BjetSlice.signatures = [
        ]

    TriggerFlags.METSlice.signatures = [
        ]

    TriggerFlags.TauSlice.signatures = [
        ]

    TriggerFlags.EgammaSlice.signatures = [
        ]

    TriggerFlags.BphysicsSlice.signatures = [
        ]

    TriggerFlags.CombinedSlice.signatures = [
        ]
   
    TriggerFlags.MinBiasSlice.signatures = [
        ]

    TriggerFlags.CalibSlice.signatures   = []
    TriggerFlags.CosmicSlice.signatures  = [ ]
    TriggerFlags.StreamingSlice.signatures = [
        ]

    TriggerFlags.MonitorSlice.signatures = []
    #TriggerFlags.GenericSlice.signatures = []
    
class Prescales:
    #   Item name             | Prescale
    #----------------------------------------------------------
    L1Prescales = {

        }
    HLTPrescales = {
        }


StreamConfig = {
    'Egamma':      [ 'egamma',
                     'comb__ej', 'comb__emet', 'comb__ejmet', 'comb__eg',
                     'comb__gmet', 
                     'comb__emu', 'comb__gmu',
                     'comb_tauej', 
                     ], 
    'MinBias':     [ 'minbias' ], 
    'Background':  [ 'background' ], 

    'Muons':       [ 'muons', 'bphysics',
                     'comb__muj', 'comb__mumet', 'comb__mujmet',
                     #'comb__emu', 'comb__gmu',
                     'comb_taumuj','comb__mue', 'comb__mug',
                     'comb__multimu',
                     ], 
    'JetTauEtmiss':   [ 'jets', 'tauEtmiss',
                        'comb__tauj', 'comb__taumet', 'comb__jmet',
                        'comb__taujmet', 'comb__taub', 'comb__taue', 
                        'comb__taue', 'comb__taumu', 'comb__fjj', 
                        'comb__tauej', 'comb__taumuj', ],
    # New delayed streams for Jet triggers
    'HadDelayed':  [ 'haddelayed' ],
    'JetCalibDelayed':  [ 'jetcalibdelayed' ],
    'Standby': [ 'standby' ], 
    'calibration': [ 'calibration' ], 
    'express':     [ 'express' ],

    }

