# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )

from AthenaConfiguration.ComponentFactory import CompFactory
RoRSeqFilter = CompFactory.RoRSeqFilter
PassFilter   = CompFactory.PassFilter


class NoHypoToolCreated(Exception):
    """Exception thrown by HypoTool generators if no HypoTool is needed"""  # see ATR-23920


def algColor(alg):
    """ Set given color to Alg type"""
    if isComboHypoAlg(alg):
        return "darkorange"
    if isHypoBase(alg):
        return "darkorchid1"
    if isInputMakerBase(alg):
        return "cyan3"
    if isFilterAlg(alg):
        return "chartreuse3"
    if isPassFilterAlg(alg):
        return "darkgreen"
    return "cadetblue1"



def isHypoBase(alg):
    if  'HypoInputDecisions'  in alg.__class__.__dict__:
        return True
    prop = alg.__class__.__dict__.get('_properties')
    if type(prop) is dict:
        return  ('HypoInputDecisions'  in prop)
    else:
        return False

def isInputMakerBase(alg):
    return  ('InputMakerInputDecisions'  in alg.__class__.__dict__)

def isFilterAlg(alg):
    return isinstance(alg, RoRSeqFilter)

def isPassFilterAlg(alg):
    return isinstance(alg, PassFilter)

def isComboHypoAlg(alg):
    return  ('MultiplicitiesMap'  in alg.__class__.__dict__)

def isHypoAlg(alg):
    return isHypoBase(alg) and not isComboHypoAlg(alg)

