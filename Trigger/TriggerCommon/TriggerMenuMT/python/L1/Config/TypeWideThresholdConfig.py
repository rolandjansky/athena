# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger('Menu.L1.Config.TypeWideThresholdConfig')

from ..Base.ThresholdType import ThrType

def getTypeWideThresholdConfig(ttype):
    if isinstance(ttype, str):
        ttype = ThrType[ttype]

    if ttype == ThrType.MU:
        return getConfig_MU()
    if ttype == ThrType.eEM:
        return getConfig_eEM()
    if ttype == ThrType.eTAU:
        return getConfig_eTAU()
    if ttype == ThrType.jJ:
        return getConfig_jJ()
    if ttype == ThrType.EM:
        return getConfig_EM()
    if ttype == ThrType.TAU:
        return getConfig_TAU()
    if ttype == ThrType.JET:
        return getConfig_JET()
    if ttype == ThrType.XS:
        return getConfig_XS()    
    return odict()


def getConfig_MU():
    confObj = odict()
    confObj["exclusionLists"] = odict()
    confObj["exclusionLists"]["rpcFeet"] = []
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B31"), ("rois",[1, 2, 3, 4, 5, 6, 7, 8])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B32"), ("rois",[1, 2, 3, 4, 5, 6, 7, 8])]) )
    
    confObj["roads"] = odict()
    confObj["roads"]["rpc"] = odict([(0,0), (4,0), (6,1), (8,2), (10,3), (15,4), (20,5)])
    confObj["roads"]["tgc"] = odict(zip([0,2,4,6,8,10,12,14,16,18,20,22,24,26,28,30],[0]+list(range(0,15))))
    return confObj


def getConfig_eEM():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("reta", 0.12), ("wstot", 0.79), ("rhad", 0.31), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("reta", 0.09), ("wstot", 0.65), ("rhad", 0.20), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("reta", 0.08), ("wstot", 0.52), ("rhad", 0.19), ("maxEt", 60)]),
    ]
    confObj["ptMinToTopo"] = 3
    confObj["resolutionMeV"] = 100
    return confObj


def getConfig_eTAU():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isolation", 16), ("maxEt", 25)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isolation", 17), ("maxEt", 25)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isolation", 18), ("maxEt", 25)]),
    ]
    confObj["ptMinToTopo"] = 6
    confObj["resolutionMeV"] = 100
    return confObj


def getConfig_jJ():
    confObj = odict()
    confObj["ptMinToTopo"] = [
        odict([("etamin",-49), ("etamax",49), ("small", 12), ("large", 12)])
    ]
    return confObj


def getConfig_EM():
    confObj = odict()
    confObj["isolation"] = odict()
    confObj["isolation"]["HAIsoForEMthr"] = odict([ ( "thrtype", "HAIsoForEMthr" ), ("Parametrization", []) ])
    confObj["isolation"]["HAIsoForEMthr"]["Parametrization"] += [
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 1), ("mincut", 10), ("offset", -2), ("priority", 0), ("slope", 230), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 2), ("mincut",  0), ("offset",  0), ("priority", 0), ("slope",   0), ("upperlimit",  0)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 3), ("mincut", 10), ("offset", -2), ("priority", 0), ("slope", 230), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 4), ("mincut", 10), ("offset", -2), ("priority", 0), ("slope", 230), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 5), ("mincut", 10), ("offset", -2), ("priority", 0), ("slope", 230), ("upperlimit", 50)]),
    ]
    confObj["isolation"]["EMIsoForEMthr"] = odict([ ("thrtype", "EMIsoForEMthr" ), ("Parametrization", []) ])
    confObj["isolation"]["EMIsoForEMthr"]["Parametrization"] += [
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 1), ("mincut",  0), ("offset",   0), ("priority", 0), ("slope",  0), ("upperlimit",  0)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 2), ("mincut", 20), ("offset", -18), ("priority", 0), ("slope", 80), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 3), ("mincut", 20), ("offset", -18), ("priority", 0), ("slope", 80), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 4), ("mincut", 20), ("offset", -18), ("priority", 0), ("slope", 80), ("upperlimit", 50)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 5), ("mincut", 20), ("offset", -18), ("priority", 0), ("slope", 80), ("upperlimit", 50)]),
    ]
    confObj["ptMinToTopo"] = 3
    confObj["resolutionMeV"] = 500
    return confObj


def getConfig_TAU():
    confObj = odict()
    confObj["isolation"] = odict()
    confObj["isolation"]["EMIsoForTAUthr"] =  odict([ ( "thrtype", "EMIsoForTAUthr" ), ("Parametrization", []) ])
    confObj["isolation"]["EMIsoForTAUthr"]["Parametrization"] += [
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 1), ("mincut", 0), ("offset", 30), ("priority", 0), ("slope", 100), ("upperlimit",  60)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 2), ("mincut", 0), ("offset", 20), ("priority", 0), ("slope", 100), ("upperlimit",  60)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 3), ("mincut", 0), ("offset", 15), ("priority", 0), ("slope", 100), ("upperlimit",  60)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 4), ("mincut", 0), ("offset", 40), ("priority", 0), ("slope",   0), ("upperlimit", 124)]),
        odict([ ("etamax", 49), ("etamin", -49), ("isobit", 5), ("mincut", 0), ("offset", 30), ("priority", 0), ("slope", 100), ("upperlimit",  60)])
    ]
    confObj["ptMinToTopo"] = 8
    confObj["resolutionMeV"] = 500
    return confObj


def getConfig_JET():
    confObj = odict()
    confObj["ptMinToTopoLargeWindow"] = 12
    confObj["ptMinToTopoSmallWindow"] = 12
    return confObj


def getConfig_XS():
    confObj = odict()
    confObj["significance"] = odict()
    confObj["significance"]["xeMin"] = 11
    confObj["significance"]["xeMax"] = 63
    confObj["significance"]["teSqrtMin"] = 4
    confObj["significance"]["teSqrtMax"] = 63
    confObj["significance"]["xsSigmaScale"] = 1150
    confObj["significance"]["xsSigmaOffset"] = 1640
    return confObj

