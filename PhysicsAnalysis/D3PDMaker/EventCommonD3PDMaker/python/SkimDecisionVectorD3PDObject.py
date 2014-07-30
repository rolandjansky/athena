# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from D3PDMakerCoreComps.D3PDObject         import make_SG_D3PDObject
import EventCommonD3PDMaker


def _streamsName():
    try:
        from RecExConfig.InputFilePeeker import inputFileSummary
        streams=inputFileSummary['stream_names'][0]
        return streams + '_SkimDecisionsContainer'
    except:
        from AthenaCommon.Logging import logging
        log = logging.getLogger ('SkimDecisionD3PDObject')
        log.warn ('Unable to get stream_names from inputFileSummary')
        return ''
        
    
    

SkimDecisionVectorD3PDObject = \
                       make_SG_D3PDObject ('SkimDecisionCollection',
                                           _streamsName(),
                                           'SkimDecision',
                                           'SkimDecisionVectorD3PDObject')

SkimDecisionVectorD3PDObject.defineBlock \
                                   (0, 'SkimDecision',
                                    EventCommonD3PDMaker.SkimDecisionVectorFillerTool,
                                    DoAllSkimDecisions = True,
                                    SkimDecisionNames = [])
