# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from collections import Counter
log = logging.getLogger( 'TriggerMenu.menu.StreamInfo' )

monStreams = ['monitoring_random',
              'CSC',
              'IDMonitoring'
              ]

physicsStreams = ['Main',
                  'BphysLS',
                  'Background',
                  'DISCARD',
                  'MinBias',
                  'ZeroBias',
                  'TauOverlay',
                  'Standby', 
                  'L1Calo', 'L1Topo',
                  'IDCosmic','CosmicCalo',
                  'HLTPassthrough',
                  'JetTauEtmiss', # needed for LS1menu
                  'EnhancedBias',
                  'CosmicMuons',
                  'HLT_IDCosmic',
                  'Late',
                  'HardProbes',
                  'MinBiasOverlay',
                  'UPC',
                  'Mistimed',
                  'ExoDelayed'
                  ]

calibStreams = [
                'Tile', 
                'L1CaloCalib',
                'LArNoiseBurst',
                'LArCellsEmpty',
                'LArCells',
                'PixelNoise', 'PixelBeam', 
                'SCTNoise',
                'CostMonitoring',
                'Muon_Calibration',
                'ALFACalib',
                'IBLLumi',
                'IDTracks',
                'TgcNoiseBurst',
                'VdM',
                'IDFwd',
                'BeamSpot',
                'zdcCalib',
                'AFP',
                'BphysPEB',
                'RPCNoise',
                'IDprescaledL1',
                'LArPEB',
                'L1TopoMismatches'
                ]

##NOTE: DataScouting_xx_NAME: 
##xx stands for the unique ROB_ID associated with the stream. If you add a new one,
##make sure to use a unique number
dataScoutingStreams = ['DataScouting_03_CosmicMuons',
                       'DataScouting_05_Jets'
                       ]

expressStreams = ['express']


def getAllStreams():

    allStreams = monStreams + physicsStreams + calibStreams + dataScoutingStreams + expressStreams

    duplicates = [stream for stream,count in Counter(allStreams).items() if count>1]

    if duplicates:
        log.error('Stream %s is duplicated' % duplicates)


    return monStreams + physicsStreams + calibStreams + dataScoutingStreams + expressStreams


def getStreamTag(streams):


    # stream is a tuple (stream, type, obeyLB, prescale)
    streamTags = []

    for stream in streams:
        
        if stream in physicsStreams:
            streamTags += [(stream, 'physics', 'yes', '1')]
        elif stream in calibStreams:
            if 'BeamSpot' in stream or 'Background' in stream or "IDTracks" in stream or 'VdM' in stream or 'PixelBeam' in stream or 'BphysPEB' in stream or 'IDprescaledL1' in stream:
               streamTags += [(stream, 'calibration', 'yes', '1')]
            else: 
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
            

    log.debug('StreamTags '+str(streamTags))
    return streamTags

#[ ('jettauetmiss','physics','yes','1') ]
