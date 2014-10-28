# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.StreamInfo' )



def getStreamTag(streams):

    physicsStreams = ['Main',
                      'Physics',
                      'DISCARD',
                      'Egamma', 
                      'Bphysics',
                      'Combined',
                      'Muon', 
                      'Met', 'Jet', 'Tau', 
                      'MinBias',
                      'Standby', 
                      'L1Muon', 'L1Calo', 'L1MinBias', 
                      'IDCosmic','CosmicCalo',
                      'HLTPassthrough',
                      'JetTauEtmiss', # needed for LS1menu
                      'EnhancedBias',
                      'IDMonitoring',
                      'CosmicMuons',
                      ]

    calibStreams = ['Background', 
                    'IDMonitoring', 
                    'Tile', 
                    'L1CaloCalib', 
                    'LArCellsEmpty',
                    'PixelNoise', 'PixelBeam', 
                    'SCTNoise',
                    'calibration_CSC',
                    'CostMonitoring',
                    ]

    expressStreams = ['express']

    # stream is a tuple (stream, type, obeyLB, prescale)
    streamTags = []

    for stream in streams:
        
        if stream in physicsStreams:
            streamTags += [(stream, 'physics', 'yes', '1')]
        elif stream in calibStreams:
            streamTags += [(stream, 'calibration', 'no', '1')]
        elif stream in expressStreams:
            streamTags += [(stream, 'express', 'yes', '1')]
        else:
            log.error('Stream %s not defined!' % stream)
            return [ ('jettauetmiss','physics','yes','1') ]#[('','','','')]
            

    log.info('StreamTags '+str(streamTags))
    return streamTags

#[ ('jettauetmiss','physics','yes','1') ]
