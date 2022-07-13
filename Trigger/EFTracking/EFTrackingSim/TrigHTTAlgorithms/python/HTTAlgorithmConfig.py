# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
@author Riley Xu - rixu@cern.ch
@date Feb 6th 2020
@brief This file declares functions that configure the tools and algorithms in HTTAlgorithms.
'''

from TrigHTTHough.TrigHTTHoughConf import HTTRoadUnionTool, HTTHoughTransformTool, HTTHough1DShiftTool
from TrigHTTLRT.TrigHTTLRTConf import HTTHoughTransform_d0phi0_Tool, HTTLLPDoubletHoughTransformTool

import TrigHTTMaps.HTTMapConfig as HTTMaps

def intList(string):
    return [ int(v) for v in string.split(',') if v != '' ]

def floatList(floatStr):
    '''
    Converts a list of floats in string form to an actual list. This is needed since
    trfArgClasses doesn't have a argFloatList class
    '''
    return [ float(v) for v in floatStr.split(',') if v != '' ]

def applyTag(obj, tag):
    '''
    Applies the parameters in the supplied tag to the given HTTAlgorithm object.
    '''

    params = { # List of configurable parameters for the given object type
        'HTTPatternMatchTool': [
                'max_misses',
        ],
        'HTTSectorMatchTool': [
                'max_misses',
        ],
        'HTTTrackFitterTool': [
                'chi2DofRecoveryMin',
                'chi2DofRecoveryMax',
                'doMajority',
                'nHits_noRecovery',
                'GuessHits',
                'DoMissingHitsChecks',
                'IdealCoordFitType',
                'DoDeltaGPhis'
        ],
    }

    for param in params[obj.getType()]:
        setattr(obj, param, tag[param])


def addHoughTool(map_tag, algo_tag, doHitTracing):
    '''
    Creates and adds the Hough transform tools to the tool svc
    '''

    union = HTTRoadUnionTool()

    if algo_tag['xVar'] == 'phi':
        x_min = algo_tag['phi_min']
        x_max = algo_tag['phi_max']
    else:
        raise NotImplementedError("x != phi")

    x_buffer = (x_max - x_min) / algo_tag['xBins'] * algo_tag['xBufferBins']
    x_min -= x_buffer
    x_max += x_buffer

    if algo_tag['yVar'] == 'q/pt':
        y_min = algo_tag['qpt_min']
        y_max = algo_tag['qpt_max']
    else:
        raise NotImplementedError("y != q/pt")

    y_buffer = (y_max - y_min) / algo_tag['yBins'] * algo_tag['yBufferBins']
    y_min -= y_buffer
    y_max += y_buffer

    tools = []
    nSlice = HTTMaps.getNSubregions(map_tag) if algo_tag['slicing'] else 1

    d0_list = algo_tag['d0_slices'] or [0]

    for d0 in d0_list:
        for iSlice in range(nSlice):
            t = HTTHoughTransformTool("HoughTransform_" + str(d0) + '_' + str(iSlice))

            t.subRegion = iSlice if nSlice > 1 else -1
            t.phi_min = x_min
            t.phi_max = x_max
            t.qpT_min = y_min
            t.qpT_max = y_max
            t.d0_min = d0
            t.d0_max = d0
            t.nBins_x = algo_tag['xBins'] + 2 * algo_tag['xBufferBins']
            t.nBins_y = algo_tag['yBins'] + 2 * algo_tag['yBufferBins']
            t.threshold = algo_tag['threshold']
            t.convolution = algo_tag['convolution']
            t.combine_layers = algo_tag['combine_layers']
            t.scale = algo_tag['scale']
            t.convSize_x = algo_tag['convSize_x']
            t.convSize_y = algo_tag['convSize_y']
            t.traceHits = doHitTracing
            t.localMaxWindowSize = algo_tag['localMaxWindowSize']
            t.fieldCorrection = algo_tag['fieldCorrection']
            if algo_tag['DoDeltaGPhis']:
                t.IdealGeoRoads = True
            try:
                t.hitExtend_x = algo_tag['hitExtend_x']
            except Exception:
                t.hitExtend_x = intList(algo_tag['hitExtend_x']) 

            tools.append(t)

    union.tools = tools # NB don't manipulate union.tools directly; for some reason the attributes get unset. Only set like is done here


    from AthenaCommon.AppMgr import ToolSvc 
    ToolSvc += union

    return union

# This is only for LRT, so draw on the LRT variables.
# That way we don't interfere with the standard Hough for first pass tracking,
# if using it.
def addHough_d0phi0_Tool(map_tag, algo_tag, doHitTracing):
    '''
    Creates and adds the Hough transform tools to the tool svc
    '''

    union = HTTRoadUnionTool("LRTRoadUnionTool")

    if algo_tag['lrt_straighttrack_xVar'] == 'phi':
        x_min = algo_tag['lrt_straighttrack_phi_min']
        x_max = algo_tag['lrt_straighttrack_phi_max']
    else:
        raise NotImplementedError("x != phi")

    x_buffer = (x_max - x_min) / algo_tag['lrt_straighttrack_xBins'] * algo_tag['lrt_straighttrack_xBufferBins']
    x_min -= x_buffer
    x_max += x_buffer

    if algo_tag['lrt_straighttrack_yVar'] == 'd0':
        y_min = algo_tag['lrt_straighttrack_d0_min']
        y_max = algo_tag['lrt_straighttrack_d0_max']
    else:
        raise NotImplementedError("y != d0")

    y_buffer = (y_max - y_min) / algo_tag['lrt_straighttrack_yBins'] * algo_tag['lrt_straighttrack_yBufferBins']
    y_min -= y_buffer
    y_max += y_buffer

    tools = []
    nSlice = HTTMaps.getNSubregions(map_tag) if algo_tag['lrt_straighttrack_slicing'] else 1

    for iSlice in range(nSlice):
        t = HTTHoughTransform_d0phi0_Tool("HoughTransform_d0phi0_" + str(iSlice))

        t.subRegion = iSlice if nSlice > 1 else -1
        t.phi_min = x_min
        t.phi_max = x_max
        t.d0_min = y_min
        t.d0_max = y_max
        t.nBins_x = algo_tag['lrt_straighttrack_xBins'] + 2 * algo_tag['lrt_straighttrack_xBufferBins']
        t.nBins_y = algo_tag['lrt_straighttrack_yBins'] + 2 * algo_tag['lrt_straighttrack_yBufferBins']
        t.threshold = algo_tag['lrt_straighttrack_threshold']
        t.convolution = algo_tag['lrt_straighttrack_convolution']
        t.combine_layers = algo_tag['lrt_straighttrack_combine_layers']
        t.scale = algo_tag['lrt_straighttrack_scale']
        t.convSize_x = algo_tag['lrt_straighttrack_convSize_x']
        t.convSize_y = algo_tag['lrt_straighttrack_convSize_y']
        t.traceHits = doHitTracing
        t.stereo = algo_tag['lrt_straighttrack_stereo']
        t.localMaxWindowSize = algo_tag['lrt_straighttrack_localMaxWindowSize']

        try:
            t.hitExtend_x = algo_tag['lrt_straighttrack_hitExtend_x']
        except Exception:
            t.hitExtend_x = intList(algo_tag['lrt_straighttrack_hitExtend_x']) 


        tools.append(t)

    union.tools = tools # NB don't manipulate union.tools directly; for some reason the attributes get unset. Only set like is done here

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += union

    return union

def addHough1DShiftTool(map_tag, algo_tag):

    union = HTTRoadUnionTool()

    tools = []
    nSlice = HTTMaps.getNSubregions(map_tag) if algo_tag['slicing'] else 1

    splitpt=algo_tag['splitpt']
    for ptstep in range(splitpt):
        qpt_min = algo_tag['qpt_min']
        qpt_max = algo_tag['qpt_max']
        lowpt = qpt_min + (qpt_max-qpt_min)/splitpt*ptstep
        highpt = qpt_min + (qpt_max-qpt_min)/splitpt*(ptstep+1)

        for iSlice in range(nSlice):
            tool = HTTHough1DShiftTool("Hough1DShift_" + str(iSlice)+(("_pt{}".format(ptstep))  if splitpt>1 else ""))
            tool.subRegion = iSlice if nSlice > 1 else -1
            if algo_tag['radiiFile'] is not None:
                tool.radiiFile = algo_tag['radiiFile']
            tool.phi_min = algo_tag['phi_min']
            tool.phi_max = algo_tag['phi_max']
            tool.qpT_min = lowpt
            tool.qpT_max = highpt
            tool.nBins = algo_tag['xBins']
            tool.useDiff = True
            tool.variableExtend = True
            tool.phiRangeCut = algo_tag['phiRangeCut']
            tool.d0spread=-1.0 # mm
            tool.iterStep = 0 
            tool.iterLayer = 7 
            tool.threshold = algo_tag['threshold'][0] 

            try:
                tool.hitExtend = algo_tag['hitExtend_x']
            except Exception:
                tool.hitExtend = floatList(algo_tag['hitExtend_x'])

            tools.append(tool)

    union.tools = tools # NB don't manipulate union.tools directly; for some reason the attributes get unset. Only set like is done here

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += union
    return union

def addLRTDoubletHTTool(algo_tag):
    tool = HTTLLPDoubletHoughTransformTool()
    tool.nBins_x   = algo_tag['lrt_doublet_d0_bins']
    tool.d0_range  = algo_tag['lrt_doublet_d0_range']
    tool.nBins_y   = algo_tag['lrt_doublet_qpt_bins']
    tool.qpT_range = algo_tag['lrt_doublet_qpt_range']
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += tool
    return tool
