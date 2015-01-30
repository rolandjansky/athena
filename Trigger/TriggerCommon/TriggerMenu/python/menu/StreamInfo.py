# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.StreamInfo' )

monStreams = ['monitoring_random', 
              'CSC',
              ]

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
                  'HLT_IDCosmic',
                  'CosmicID',
                  ]

calibStreams = ['Background', 
                'Tile', 
                'L1CaloCalib', 
                'LArCellsEmpty',
                'PixelNoise', 'PixelBeam', 
                'SCTNoise',
                'CostMonitoring',
                'beamspot',
                'Muon_Calibration',
                'ALFACalib'
                ]

##NOTE: DataScouting_xx_NAME: 
##xx stands for the unique ROB_ID associated with the stream. If you add a new one,
##make sure to use a unique number
dataScoutingStreams = ['DataScouting_01_CosmicMuons',
                       'DataScouting_02_CosmicMuons',
                       'DataScouting_03_CosmicMuons',
                       'DataScouting_04_IDCosmic',                    
                       'DataScouting_05_Jets',
                       'DataScouting_06_Jets',
                       ]

expressStreams = ['express']


def getAllStreams():

    return monStreams + physicsStreams + calibStreams + dataScoutingStreams + expressStreams


def getStreamTag(streams):


    # stream is a tuple (stream, type, obeyLB, prescale)
    streamTags = []

    for stream in streams:
        
        if stream in physicsStreams:
            streamTags += [(stream, 'physics', 'yes', '1')]
        elif stream in calibStreams:
            streamTags += [(stream, 'calibration', 'no', '1')]
        elif stream in expressStreams:
            streamTags += [(stream, 'express', 'yes', '1')]
        elif stream in monStreams:
            streamTags += [(stream, 'monitoring', 'yes', '1')]
        elif stream in dataScoutingStreams:
            streamTags += [(stream, 'calibration', 'yes', '1')]
            
        else:
            log.error('Stream %s not defined, returning dummy stream!' % stream)
            return [ ('jettauetmiss','physics','yes','1') ]#[('','','','')]
            

    log.info('StreamTags '+str(streamTags))
    return streamTags

#[ ('jettauetmiss','physics','yes','1') ]
