# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerMenuMT.HLTMenuConfig.Menu.StreamInfo' )

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
                'AFP',
                'L1TopoMismatches'
                ]

fullEventBuildingStreams = ['Main',
                            'express']

lumiBlockAgnosticStreams = ['BeamSpot',
                            'Background',
                            'IDTracks',
                            'VdM',
                            'PixelBeam']

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


def getStreamTags(streams):
    # Stream is a tuple (name, type, obeysLumiBlock, forceFullEventBuilding)
    streamTags = []

    for stream_name in streams:
        # Determine type of stream
        stream_type = 'UNDEFINED'
        if stream_name in physicsStreams:
            stream_type = 'physics'
        elif stream_name in calibStreams:
            stream_type = 'calibration'
        elif stream_name in expressStreams:
            stream_type = 'express'
        elif stream_name in monStreams:
            stream_type = 'monitoring'
        elif stream_name in dataScoutingStreams:
            stream_type = 'calibration'
        else:
            log.error('Unknown stream name %s, cannot determine the stream type', stream_name)

        # Determine obeysLumiBlock and fullEventBuildingStreams flags
        obeys_lb = stream_name not in lumiBlockAgnosticStreams
        full_ev_build = stream_name in fullEventBuildingStreams

        # Create the stream tag tuple
        streamTags.append( (stream_name, stream_type, obeys_lb, full_ev_build) )

    log.debug('StreamTags: %s', streamTags)
    return streamTags
