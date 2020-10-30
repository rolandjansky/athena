#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
'''
@file TileMonitoringHelper.py
@brief Helper functions for Run 3 Tile monitoring algorithm configuration
'''
from TileCalibBlobObjs.Classes import TileCalibUtils as Tile

_cellNameEB = ['E3', 'E4', 'D4', 'D4', 'C10', 'C10', 'A12', 'A12', 'B11', 'B11', 'A13', 'A13',
               'E1', 'E2', 'B12', 'B12', 'D5', 'D5', 'E3*', 'E4*', 'A14', 'A14', 'B13', 'B13',
               '',   '',    '',    '',   '',   '', 'B14', 'A15', 'A15',    '',    '', 'B14',
               'B15', 'D6', 'D6', 'B15', 'A16', 'A16',  '',    '',    '',    '',    '', '']


_cellNameLB = ['D0', 'A1', 'B1', 'B1', 'A1', 'A2', 'B2', 'B2', 'A2',  'A3',  'A3', 'B3',
               'B3', 'D1', 'D1', 'A4', 'B4', 'B4', 'A4', 'A5', 'A5',  'B5',  'B5', 'A6',
               'D2', 'D2', 'A6', 'B6', 'B6', 'A7',   '',   '', 'A7',  'B7',  'B7', 'A8',
               'A9', 'A9', 'A8', 'B8', 'B8', 'D3', 'B9',   '', 'D3', 'A10', 'A10', 'B9']


_partitionName = {0: 'AUX', 1 : 'LBA', 2 : 'LBC', 3 : 'EBA', 4 : 'EBC', 5 : 'AllPart'}
_gainName = {0 : 'LG', 1 : 'HG'}
_sampleName = {0 : 'SampA', 1 : 'SampB', 2 : 'SampD', 3 : 'SampE', 4 : 'AllSamp'}

_cellNameTMDB_LB = ["D0", "D1L", "D1R", "D2R", "D2L", "D3L", "D3R", ""]
_cellNameTMDB_EB = ["D5L", "D5R", "D6L", "D6R"]

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


def getGainName(gain):
    '''
    This function returns name of Tile gain name

    Arguments:
        gain -- Tile gain ([0,1])
    '''
    return _gainName[int(gain)]


def getSampleName(sample):
    '''
    This function returns name of Tile sample name

    Arguments:
        sample -- Tile sample ([0..4])
    '''
    return _sampleName[int(sample)]


def getTileHistogramName(name, separator = '_', **kwargs):
    '''
    This function returns name of Tile histogram.

    Arguments:
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name [+ partition] [+ sample] [+gain] [+ trigger]
        separator -- Separtor between name, partition, sample, gain, and trigger
    '''

    partition = kwargs.get('partition', '')
    trigger = kwargs.get('trigger', '')
    sample = kwargs.get('sample', '')
    gain = kwargs.get('gain', '')

    fullName = name
    fullName += separator + partition if partition else ''
    fullName += separator + sample if sample else ''
    fullName += separator + gain if gain else ''
    fullName += separator + trigger if trigger else ''

    return fullName


def getTileHistogramTitle(title, **kwargs):
    '''
    This function returns title of Tile histogram.

    Arguments:
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        [run +] [trigger +] [partion +] [sample +] [gain +] title
    '''

    partition = kwargs.get('partition', '')
    trigger = kwargs.get('trigger', '')
    sample = kwargs.get('sample', '')
    gain = kwargs.get('gain', '')
    run = kwargs.get('run', '')

    fullTitle = 'Partition ' + partition if partition else ''
    fullTitle += ' Tile Cell ' + sample + ' ' if sample else ''
    fullTitle += ' ' + gain if gain else ''
    fullTitle += ': ' + title

    fullTitle = 'Trigger ' + trigger + ' ' + fullTitle if trigger else fullTitle
    fullTitle = 'Run ' + run + ' ' + fullTitle if run else fullTitle

    return fullTitle


def getTileHistogramPath(path, **kwargs):
    '''
    This function returns path of Tile histogram.

    Arguments:
        path -- Path of histogram, actual path is constructed dynamicaly like:
                    path [+ trigger] [+ partition]
    '''

    partition = kwargs.get('partition', '')
    trigger = kwargs.get('trigger', '')
    subDirectory = kwargs.get('subDirectory', False)

    fullPath = path + '/' + trigger if trigger else path
    fullPath += '/' + partition if partition and subDirectory else ''

    return fullPath



def getModuleLabels(partition):
    '''
    This function returns list of Tile module names for given partition.

    Arguments:
        partition -- Tile partition name (LBA, LBC, EBA, EBC)
    '''

    if partition == 'AllPart':
        labels = [str(module) for module in range(1, Tile.MAX_DRAWER + 1)]
    else:
        ros = {'LBA' : 1, 'LBC' : 2, 'EBA' : 3, 'EBC' : 4}
        labels = [Tile.getDrawerString(ros[partition], module) for module in range(0, Tile.MAX_DRAWER)]

    return labels


def getCellChannelLabels(partition):
    '''
    This function returns list of Tile cell names with channes for given partition.

    Arguments:
        partition -- Tile partition name (LBA, LBC, EBA, EBC)
    '''

    labels = []
    for channel in range(0, Tile.MAX_CHAN):
        cellName = getCellName(partition, channel)
        label = cellName + '_' + 'ch' + str(channel) if cellName else 'ch' + str(channel)
        labels.append(label)
    return labels


def getLabels(labels, partition = ''):
    '''
    This function returns list of labels for Tile histograms.

    This function returns list of Tile module or channel names for given partition
    in the case of input labels is modules or channels. Otherwise it returns given labels.

    Arguments:
        labels    --  List of labels, "modules" and "channels" have special meaning,
                      in this case corresponding labels will be generated dynamicaly
        partition -- Tile partition name (LBA, LBC, EBA, EBC)
    '''

    if 'modules' in labels:
        labels = getModuleLabels(partition)
    elif 'channels' in labels:
        labels = getCellChannelLabels(partition)

    return labels


def getCellNameTMDB(partition, channel):
        return _cellNameTMDB_LB[channel] if partition.startswith('L') else _cellNameTMDB_EB[channel]

def getCellChannelTMDB_Labels(partition):
        return _cellNameTMDB_LB if partition.startswith('L') else _cellNameTMDB_EB


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

    for ros in range(1, Tile.MAX_ROS):
        partition = getPartitionName(ros)
        xlabels = []
        ylabels = []
        for module in range(1, Tile.MAX_DRAWER + 1): # modules start from 1
            label = partition + '0' + str(module) if module < 10 else partition + str(module)
            xlabels.append(label)

        for channel in range(0, Tile.MAX_CHAN):
            cellName = getCellName(partition, channel)
            label = cellName + '_' + 'ch' + str(channel) if cellName else 'ch' + str(channel)
            ylabels.append(label)

        fullName = 'module' + partition + ',channel' + partition
        if 'Profile' in type:
            fullName += (',' + value + partition)
        fullName += ';' + name + partition + trigger

        fullPath = path + '/' + partition if subDirectory else path

        fullTitle = 'Partition ' + partition + ': ' + title
        if trigger:
            fullTitle = 'Trigger ' + trigger + ' ' + fullTitle
        if run:
            fullTitle = 'Run ' + run + ' ' + fullTitle

        group.defineHistogram( fullName, path = fullPath, type = type, title = fullTitle,
                               xbins = 64, xmin = 0.5, xmax = 64.5, ybins = 48, ymin = -0.5, ymax = 47.5,
                               xlabels = xlabels, ylabels = ylabels )


def _getDimensions(triggers = [], perPartition = False, perSample = False, perGain = False, allPartitions = False):

    dimensions = []
    if perPartition:
        dimensions += [int(Tile.MAX_ROS)] if allPartitions else [int(Tile.MAX_ROS - 1)]
    if perSample:
        dimensions += [len(_sampleName)]
    if perGain:
        dimensions += [int(Tile.MAX_GAIN)]
    if triggers:
        dimensions += [len(triggers)]

    return dimensions


def _parsePostfix(postfix, triggers = [], perPartition = False, perSample = False, perGain = False):

    kwargs = {}
    elements = postfix.split('_')

    if triggers:
        kwargs['trigger'] = triggers[int(elements.pop())]
    if perGain:
        kwargs['gain'] = getGainName(elements.pop())
    if perSample:
        kwargs['sample'] = getSampleName(elements.pop())
    if perPartition:
        kwargs['ros'] = int(elements.pop()) + 1
        kwargs['partition'] = getPartitionName(kwargs['ros'])

    return kwargs

def addTile2DHistogramsArray(helper, algorithm, name = '', xvalue = '', yvalue = '', value = '',
                             title = '', path = '', weight = '', xbins = 0, xmin = 0, xmax = 0,
                             ybins = 0, ymin = 0, ymax = 0, type = 'TH2D', run = '', triggers = [],
                             xlabels = (), ylabels = (), subDirectory = False, perPartition = False,
                             perSample = False, perGain = False, allPartitions = False, separator = '_' ):
    '''
    This function configures 2D histograms with Tile monitored value per L1 trigger, partition, sample, gain.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + sample + gain + trigger
        xvalue    -- Name of monitored value for x axis
        yvalue    -- Name of monitored value for y axis
        value     -- Name of monitored value for profile (needed for TProfile2D)
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + sample + title
        path      -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type      -- Type of histogram (TH2D, TProfile2D)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        xlabels   -- List of bin labels for x axis,  "modules" and "channels" have special meaning,
                        in this case corresponding labels will be generated dynamicaly
        ylabels   -- List of bin labels for y axis,  "modules" and "channels" have special meaning,
                        in this case corresponding labels will be generated dynamicaly
        perPartition -- Configure histograms per partition (if True partition name will be put into the title)
        perSample -- Configure histograms per sample (if True sample name will be put into the title)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        allPartitions  -- Configure additional histogram with information from all partitions
        separator -- Given it will be used as separtor between name and trigger
    '''

    dimensions = _getDimensions(triggers = triggers, perPartition = perPartition, perSample = perSample,
                                perGain = perGain, allPartitions = allPartitions)

    array = helper.addArray(dimensions, algorithm, name)
    for postfix, tool in array.Tools.items():

        kwargs = _parsePostfix(postfix, triggers = triggers, perPartition = perPartition,
                               perSample = perSample, perGain = perGain)

        partition = kwargs['partition'] if 'partition' in kwargs else ''
        nxlabels = getLabels(xlabels, partition)
        nylabels = getLabels(ylabels, partition)

        fullName = xvalue + ',' + yvalue + (',' + value if 'Profile' in type else '') + ';'
        fullName += getTileHistogramName(name = name,separator = separator, **kwargs)

        fullPath = getTileHistogramPath(path = path, subDirectory = subDirectory, **kwargs)
        fullTitle = getTileHistogramTitle(title = title, run = run, **kwargs)

        tool.defineHistogram( fullName, path = fullPath, type = type, title = fullTitle,
                              xlabels = nxlabels, ylabels = nylabels, 
                              xbins = xbins, xmin = xmin, xmax = xmax,
                              ybins = ybins, ymin = ymin, ymax = ymax, weight = weight)

    return array


def addTileModuleChannelMapsArray(helper, algorithm, name, title, path, weight = '',
                                  subDirectory = False, type = 'TH2D', value = '',
                                  run = '', triggers = [], perGain = False, separator = ''):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs module and channel per partition.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + gain + trigger
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + gain + title
        path      -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type      -- Type of histogram (TH2D, TProfile2D)
        value     -- Name of monitored value (needed for TProfile2D)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        separator -- Given it will be used as separtor between name, gain, and trigger
    '''

    return addTile2DHistogramsArray(helper, algorithm, name = name, title = title, path = path, weight = weight,
                                    xvalue = 'module', yvalue = 'channel', value = value, type = type,
                                    xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                    ybins = Tile.MAX_CHAN, ymin = -0.5, ymax = Tile.MAX_CHAN - 0.5,
                                    run = run, xlabels = ('modules'), ylabels = ('channels'),
                                    triggers = triggers, subDirectory = subDirectory, perGain = perGain,
                                    perPartition = True, separator = separator)


def addTileModuleCorrelionMapsArray(helper, algorithm, name, title, path, weight = '',
                                    subDirectory = False, type = 'TH2D', value = '', run = '',
                                    triggers = [], perGain = False, allPartitions = False, separator = ''):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs module and channel per partition.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + gain + trigger
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + gain + title
        path      -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type      -- Type of histogram (TH2D, TProfile2D)
        value     -- Name of monitored value (needed for TProfile2D)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        allPartitions  -- Configure additional histogram with information from all partitions
        separator -- Given it will be used as separtor between name, gain, and trigger
    '''

    return addTile2DHistogramsArray(helper, algorithm, name = name, title = title, path = path, weight = weight,
                                    xvalue = 'firstModule', yvalue = 'secondModule', value = value,
                                    xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                    ybins = Tile.MAX_DRAWER, ymin = -0.5, ymax = Tile.MAX_DRAWER - 0.5,
                                    run = run, xlabels = ('modules'), ylabels = ('modules'), type = type,
                                    triggers = triggers, subDirectory = subDirectory, perGain = perGain,
                                    perPartition = True, allPartitions = allPartitions, separator = separator)



def addTileModulePartitionMapsArray(helper, algorithm, name, title, path, weight = '',
                                    type = 'TH2D', value = '', run = '', triggers = [],
                                    perGain = False, separator = ''):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs module and partition.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + gain + trigger
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + gain + title
        path      -- Path in file for histogram (relative to the path of given group)
        type      -- Type of histogram (TH2D, TProfile2D)
        value     -- Name of monitored value (needed for TProfile2D)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        separator -- Given it will be used as separtor between name, gain, and trigger
    '''

    return addTile2DHistogramsArray(helper, algorithm, name = name, title = title + ';Module;Partition',
                                    path = path, weight = weight, run = run, type = type,
                                    xvalue = 'module', yvalue = 'partition', value = value,
                                    xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                    ybins = Tile.MAX_ROS - 1, ymin = -0.5, ymax = Tile.MAX_ROS - 1.5,
                                    xlabels = [str(module) for module in range(1, Tile.MAX_DRAWER + 1)],
                                    ylabels = [getPartitionName(ros) for ros in range(1, Tile.MAX_ROS)],
                                    triggers = triggers, perGain = perGain, separator = separator)



def addTileModuleDigitizerMapsArray(helper, algorithm, name, title, path, weight = '',
                                    subDirectory = False, type = 'TH2D', value = '',
                                    run = '', triggers = [], perGain = False, separator = ''):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs module and digitizer per partition.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + gain + trigger
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + gain + title
        path      -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type      -- Type of histogram (TH2D, TProfile2D)
        value     -- Name of monitored value (needed for TProfile2D)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        separator -- Given it will be used as separtor between name, gain, and trigger
    '''

    return addTile2DHistogramsArray(helper, algorithm, name = name, title = title + ';;Digitizer', path = path,
                                    weight = weight, xvalue = 'module', yvalue = 'digitizer', value = value,
                                    xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                    ybins = 8, ymin = 0.5, ymax = 8.5, run = run, xlabels = ('modules'),
                                    ylabels = (), triggers = triggers, type = type,subDirectory = subDirectory,
                                    perGain = perGain, perPartition = True, separator = separator)



def addTileEtaPhiMapsArray(helper, algorithm, name, title, path, weight = '', type = 'TH2D', value = '',
                           run = '', triggers = [], perSample = True, perGain = False, separator = '',
                           etaTitle= '#eta', etabins = 21, etamin = -2.025, etamax = 2.025,
                           phiTitle = '#phi', phibins = Tile.MAX_DRAWER, phimin = -3.15, phimax = 3.15):
    '''
    This function configures 2D histograms (maps) with Tile monitored value vs eta and phi.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name    -- Name of histogram, actual name is constructed dynamicaly like:
                        name + sample + gain + trigger
        title   -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + sample + gain + title
        path    -- Path in file for histogram (relative to the path of given group)
        type    -- Type of histogram (TH2D, TProfile2D)
        value   -- Name of monitored value (needed for TProfile2D)
        run     -- Run number (given it will be put into the title)
        triggers -- Name of trigger (given it will be put into title and name of histogram)
        perSample -- Configure histograms per sample (if True sample name will be put into the title)
        perGain -- Configure histograms per gain (if True gain name will be put into the title)
        separator -- Given it will be used as separtor between name, gain, and trigger
    '''

    return addTile2DHistogramsArray(helper, algorithm, name = name,
                                    title = title + ';' + etaTitle + ';' + phiTitle,
                                    path = path, weight = weight, type = type,
                                    xvalue = 'eta', yvalue = 'phi', value = value,
                                    xbins = etabins, xmin = etamin, xmax = etamax,
                                    ybins = phibins, ymin = phimin, ymax = phimax,
                                    run = run, triggers = triggers, perGain = perGain,
                                    perSample = perSample, separator = separator)



def addTile1DHistogramsArray(helper, algorithm, name = '', xvalue = '', value = '', title = '', path = '',
                             xbins = 0, xmin = 0, xmax = 0, type = 'TH1D', run = '', triggers = [],
                             subDirectory = False, perPartition = True, perSample = False, opt = '',
                             perGain = False, xlabels = (), allPartitions = False, separator = '_' ):
    '''
    This function configures 1D histograms with Tile monitored value per L1 trigger, partition, sample, gain.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name      -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + sample + trigger
        xvalue    -- Name of monitored value for x axis
        value     -- Name of monitored value (needed for TProfile)
        title     -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + sample + title
        path      -- Path in file for histogram (relative to the path of given group)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        type      -- Type of histogram (TH1D, TProfile)
        run       -- Run number (given it will be put into the title)
        triggers  -- Name of triggers (given it will be put into title and name of histogram)
        perPartition -- Configure histograms per partition (if True partition name will be put into the title)
        perSample -- Configure histograms per sample (if True sample name will be put into the title)
        perGain   -- Configure histograms per gain (if True gain name will be put into the title)
        xlabels    -- List of bin labels
        allPartitions  -- Configure additional histogram with information from all partitions
        separator -- Given it will be used as separtor between name and trigger
    '''

    dimensions = _getDimensions(triggers = triggers, perPartition = perPartition, perSample = perSample,
                                perGain = perGain, allPartitions = allPartitions)

    array = helper.addArray(dimensions, algorithm, name)
    for postfix, tool in array.Tools.items():

        kwargs = _parsePostfix(postfix, triggers = triggers, perPartition = perPartition,
                               perSample = perSample, perGain = perGain)

        partition = kwargs['partition'] if 'partition' in kwargs else ''
        nxlabels = getLabels(xlabels, partition)

        fullName = xvalue + (',' + value if 'Profile' in type else '') + ';'
        fullName += getTileHistogramName(name = name,separator = separator, **kwargs)

        fullPath = getTileHistogramPath(path = path, subDirectory = subDirectory, **kwargs)
        fullTitle = getTileHistogramTitle(title = title, run = run, **kwargs)

        tool.defineHistogram( fullName, path = fullPath, type = type, title = fullTitle,
                              xlabels = nxlabels, xbins = xbins, xmin = xmin, xmax = xmax, opt = opt)

    return array




def addTileModuleArray(helper, algorithm, name, title, path,
                       type = 'TH1D', value = '', subDirectory = False,
                       triggers = [], run = '', separator = '_'):
    '''
    This function configures 1D histograms with Tile monitored value vs module per partition.

    Arguments:
        helper    -- Helper
        algorithm -- Monitoring algorithm
        name    -- Name of histogram, actual name is constructed dynamicaly like:
                        name + partition + trigger
        title   -- Title of histogram, actual title is constructed dynamicaly like:
                        run + trigger + partion + title
        path    -- Path in file for histogram (relative to the path of given group)
        type    -- Type of histogram (TH1D, TProfile)
        value   -- Name of monitored value (needed for TProfile)
        subDirectory -- Put the configured histograms into sub directory named like partion (True, False)
        run     -- Run number (given it will be put into the title)
        separator -- Given it will be used as separtor between name and trigger
    '''

    return addTile1DHistogramsArray(helper = helper, algorithm = algorithm, name = name, type = type,
                                    xvalue = 'module', value = value, title = title, path = path,
                                    xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                    run = run, triggers = triggers, subDirectory = subDirectory,
                                    xlabels = ('modules'), perPartition = True, separator = separator )



def addTileTMDB_2DHistogramsArray(helper, algorithm, name = '', value = '',
                                    title = '', path = '', type = 'TH2D', run = ''):

    array = helper.addArray([int(Tile.MAX_ROS - 1)], algorithm, name)
    for postfix, tool in array.Tools.items():
        ros = int(postfix.split('_').pop()) + 1
        
        partition = getPartitionName(ros)
        nxlabels = getModuleLabels(partition)
        nylabels = getCellChannelTMDB_Labels(partition)
        ybins = len(nylabels)
                    
        fullName = 'module,channel' + (',' + value if 'Profile' in type else '') + ';'
        fullName += getTileHistogramName(name, partition = partition)
        
        fullTitle = getTileHistogramTitle(title, run = run, partition = partition)
        
        tool.defineHistogram( fullName, path = path, type = type, title = fullTitle,
                                xlabels = nxlabels, ylabels = nylabels,
                                xbins = Tile.MAX_DRAWER, xmin = -0.5, xmax = Tile.MAX_DRAWER - 0.5,
                                ybins = ybins, ymin = -0.5, ymax = ybins - 0.5)



def addTileTMDB_1DHistogramsArray(helper, algorithm, name = '', xvalue = '', value = '', title = '',
                                path = '', xbins = 0, xmin = 0, xmax = 0, type = 'TH1D', run = ''):

    for ros in range(1, Tile.MAX_ROS):
        partition = getPartitionName(ros)
        histName = "{}_{}".format(name, partition)
        nChannels = len(_cellNameTMDB_LB) if partition.startswith('L') else len(_cellNameTMDB_EB)
        array = helper.addArray([nChannels], algorithm,  histName)
        for postfix, tool in array.Tools.items():
            channel = int(postfix.split('_').pop())

            fullName = xvalue + (',' + value if 'Profile' in type else '') + ';'
            fullName += histName + '_' + getCellNameTMDB(partition, channel)

            fullTitle = getTileHistogramTitle(title, run = run, partition = partition)

            tool.defineHistogram( fullName, path = path, type = type, title = fullTitle,
                                 xbins = xbins, xmin = xmin, xmax = xmax)