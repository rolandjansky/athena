# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
@author Riley Xu - rixu@cern.ch
@date March 3rd 2020
@brief This file declares functions to make and configure the map service.
'''

import os

from PyJobTransforms.trfUtils import findFile
from PyJobTransforms.trfLogger import msg

from TrigHTTMaps.TrigHTTMapsConf import TrigHTTMappingSvc, HTTHitFilteringTool
import TrigHTTConfTools.HTTConfigCompInit as HTTConfig

def findFileWithTest(datapath,filename):
    retv = findFile(datapath,filename)
    if retv is None:
        msg.info(datapath)
        raise OSError(2, "Couldn't find file", filename)
    return retv

def addMapSvc(tag):
    '''
    Creates and returns a TrigHTTMapSvc object, configured with the specified tag.

    This function adds the returned map service instance to SvcMgr, and ALSO ADDS
    the EventSelectionSvc, which the map svc depends on
    '''
    HTTConfig.addEvtSelSvc(tag)

    HTTMappingSvc = TrigHTTMappingSvc()

    filepaths = [
            'pmap',
            'rmap',
            'modulemap',
            'subrmap',
            'NNmap',
    ]

    formats = {
            'region': HTTConfig.getRegionIndex(tag),
            'regionName': HTTConfig.getRegionName(tag),
    }

    for param in filepaths:
        if tag['formatted']:
            path = tag[param].format(**formats)
        else:
            path = tag[param]
        setattr(HTTMappingSvc, param, findFileWithTest(os.environ['DATAPATH'], path))

    HTTMappingSvc.mappingType = tag['mappingType']
    HTTMappingSvc.layerOverride = tag['layerOverride']
 
    from AthenaCommon.AppMgr import ServiceMgr
    ServiceMgr += HTTMappingSvc

    return HTTMappingSvc



def addHitFilteringTool(tag):
    '''
    Creates and adds the hit filtering tool to the tool svc
    '''

    HitFilteringTool = HTTHitFilteringTool()

    for param in HitFilteringTool.__slots__:
        if param in tag:
            setattr(HitFilteringTool, param, tag[param])

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += HitFilteringTool

    return HitFilteringTool



def getNSubregions(tag):
    formats = {
            'region': HTTConfig.getRegionIndex(tag),
            'regionName': HTTConfig.getRegionName(tag),
    }

    if tag['formatted']:
        path = tag['subrmap'].format(**formats)
    else:
        path = tag['subrmap']
    path = findFile(os.environ['DATAPATH'], path)

    with open(path, 'r') as f:
        fields = f.readline().split()
        assert(fields[0] == 'towers')
        return int(fields[1])


def _applyTag(HTTMappingSvc, tag):
    '''
    Helper function that sets the filepaths of the MapSvc using the supplied tag
    '''
