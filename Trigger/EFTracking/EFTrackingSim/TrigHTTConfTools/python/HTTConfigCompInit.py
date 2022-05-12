# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
'''
@author Riley Xu - riley.xu@cern.ch
@date Feb 23rd 2021
@brief This file declares functions to configure HTTConfig components
'''

import os

from PyJobTransforms.trfUtils import findFile
#import TrigHTTConfTools.TrigHTTConfToolsConf as Config


def getRegionIndex(map_tag):
    '''
    Note the region member of the tag is a string
    '''
    try:
        return int(map_tag['region'])
    except ValueError:
        return map_tag['regionNames'].index(map_tag['region'])

def getRegionName(map_tag):
    return map_tag['regionNames'][getRegionIndex(map_tag)]

def getSampleType(map_tag):
    return map_tag['sampleType']

def getWithPU(map_tag):
    return map_tag['withPU']

def addEvtSelSvc(map_tag,name=""):
    '''
    Creates and returns a HTTEventSelectionSvc object, configured with the specified tags.
    This function adds the returned map service instance to SvcMgr.
    '''
    from AthenaCommon.Constants import INFO

    import TrigHTTConfTools.TrigHTTConfToolsConf as Config
    if name :
        ES = Config.HTTEventSelectionSvc(name)
    else :
        ES = Config.HTTEventSelectionSvc()
    ES.regionID = getRegionIndex(map_tag)
    ES.regions = findFile(os.environ['DATAPATH'], map_tag['slices'])
    ES.sampleType = getSampleType(map_tag)
    ES.withPU = getWithPU(map_tag)
    ES.OutputLevel=INFO
 
    from AthenaCommon.AppMgr import ServiceMgr 
    #global ServiceMgr
    ServiceMgr += ES
    return ES


if __name__=='__main__':
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    import TrigHTTConfTools.HTTTagConfig as HTTTagConfig
    tags = HTTTagConfig.getTags(stage='bank')
    map_tag = tags['map']
    ES = addEvtSelSvc(map_tag)
    ES.sampleType="singleMuons"

    print(ES)

