# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenu.menu.StreamInfo' )

monStreams = ['monitoring_random', 
              'CSC',
              'IDMonitoring',
              'MetRateStudies',
              ]

physicsStreams = ['Main',
                  'Physics',
                  'Background',
                  'DISCARD',
                  'Egamma', 
                  'Bphysics',
                  'Combined',
                  'Muon', 
                  'Met', 'Jet', 'Tau', 
                  'MinBias',
                  'ZeroBias',
                  'TauOverlay',
                  'Standby', 
                  'L1Muon', 'L1Calo', 'L1MinBias', 'L1Topo',
                  'IDCosmic','CosmicCalo',
                  'HLTPassthrough',
                  'JetTauEtmiss', # needed for LS1menu
                  'EnhancedBias',
                  'CosmicMuons',
                  'HLT_IDCosmic',
                  'CosmicID',
                  'Late',
                  'HardProbes',
                  'MinBiasOverlay',
                  'UPC',
                  'Mistimed',
                  'ExoDelayed',
                  'BphysDelayed',
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
                'beamspot',
                'Muon_Calibration',
                'ALFACalib',
                'IBLLumi',
                'IDTracks',
                'TgcNoiseBurst',
                'VdM',
                'IDFwd',
                'BeamSpot',
                'zdcCalib',
                'AFP'
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
            if 'BeamSpot' in stream or 'Background' in stream or "IDTracks" in stream or 'VdM' in stream or 'PixelBeam' in stream:
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
