#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
@file TileMonitoringHelper.py
@brief Helper functions for Run 3 Tile monitoring algorithm configuration
'''


_cellNameEB = ['E3', 'E4', 'D4', 'D4', 'C10', 'C10', 'A12', 'A12', 'B11', 'B11', 'A13', 'A13', 
               'E1', 'E2', 'B12', 'B12', 'D5', 'D5', 'E3*', 'E4*', 'A14', 'A14', 'B13', 'B13',
               '',   '',    '',    '',   '',   '', 'B14', 'A15', 'A15',    '',    '', 'B14',
               'B15', 'D6', 'D6', 'B15', 'A16', 'A16',  '',    '',    '',    '',    '', '']


_cellNameLB = ['D0', 'A1', 'B1', 'B1', 'A1', 'A2', 'B2', 'B2', 'A2',  'A3',  'A3', 'B3', 
               'B3', 'D1', 'D1', 'A4', 'B4', 'B4', 'A4', 'A5', 'A5',  'B5',  'B5', 'A6', 
               'D2', 'D2', 'A6', 'B6', 'B6', 'A7',   '',   '', 'A7',  'B7',  'B7', 'A8', 
               'A9', 'A9', 'A8', 'B8', 'B8', 'D3', 'B9',   '', 'D3', 'A10', 'A10', 'B9']


_partitionName = {0: 'AUX', 1 : 'LBA', 2 : 'LBC', 3 : 'EBA', 4 : 'EBC'}


def getCellName(partition, channel):
    '''
    This function returns name of Tile cell for given partition and channel.

    Arguments:
        partition -- Tile partition name (LBA, LBC, EBA, EBC)
        channel   -- Tile channel number ([0..47])
    '''
    return _cellNameLB[channel] if partition.startswith('L') else _cellNameEB[channel]


def getPartitionName(ros):
    '''
    This function returns name of Tile partition for given ROS.

    Arguments:
        ros -- Tile ROS ([0..5])
    '''
    return _partitionName[ros]


def addValueVsModuleAndChannelMaps(group, name, title, path, subDirectory = False, type = 'TH2D', value = '', trigger = '', run = ''):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs module and channel per partion.

    Arguments:
        group   -- Group (technically, a GenericMonitoringTool instance)
        name    -- Name of histogram (actual name is constructed dynamicaly like: name + partition + trigger)
        title   -- Title of histogram (actual title is constructed dynamicaly like: run + trigger + partion + title)
        path    -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type    -- Type of histogram (TH2D, TProfile2D)
        value   -- Name of monitored value (needed for TProfile2D)
        trigger -- Name of trigger (given it will be put into title and name of histogram)
        run     -- Run number (given it will be put into the title)
    '''

    from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

    for ros in range(1, Tile.MAX_ROS):
        partition = getPartitionName(ros)
        labels = []
        for module in range(1, Tile.MAX_DRAWER + 1): # modules start from 1
            label = partition + '0' + str(module) if module < 10 else partition + str(module)
            labels.append(label)
            
        for channel in range(0, Tile.MAX_CHAN):
            cellName = getCellName(partition, channel)
            label = cellName + '_' + 'ch' + str(channel) if cellName else 'ch' + str(channel)
            labels.append(label)

        fullName = 'module' + partition + ',channel' + partition
        if 'Profile' in type: fullName += (',' + value + partition)
        fullName += ';' + name + partition + trigger

        fullPath = path + '/' + partition if subDirectory else path

        fullTitle = 'Partition ' + partition + ': ' + title
        if trigger: fullTitle = 'Trigger ' + trigger + ' ' + fullTitle
        if run:     fullTitle = 'Run ' + run + ' ' + fullTitle

        group.defineHistogram( fullName, path = fullPath, type = type, title = fullTitle,
                               xbins = 64, xmin = 0.5, xmax = 64.5, ybins = 48, ymin = -0.5, ymax = 47.5, 
                               labels = labels )
