# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from ..Base.ThresholdType import ThrType

def getTypeWideThresholdConfig(ttype):
    if isinstance(ttype, str):
        ttype = ThrType[ttype]

    if ttype == ThrType.MU:
        return getConfig_MU()
    if ttype == ThrType.eEM:
        return getConfig_eEM()
    if ttype == ThrType.jEM:
        return getConfig_jEM()
    if ttype == ThrType.eTAU:
        return getConfig_eTAU()
    if ttype == ThrType.cTAU:
        return getConfig_cTAU()
    if ttype == ThrType.jTAU:
        return getConfig_jTAU()
    if ttype == ThrType.jJ:
        return getConfig_jJ()
    if ttype == ThrType.jLJ:
        return getConfig_jLJ()
    if ttype == ThrType.jXE:
        return getConfig_jXE()
    if ttype == ThrType.jTE:
        return getConfig_jTE()
    if ttype == ThrType.gXE:
        return getConfig_gXE()
    if ttype == ThrType.gTE:
        return getConfig_gTE()
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
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B21"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B22"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B25"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B26"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B53"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B54"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B57"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )
    confObj["exclusionLists"]["rpcFeet"].append( odict([("sectorName", "B58"), ("rois",[8,9,10,11,16,17,18,19,20,21,22,23,28,29,30,31])]) )

    
    # roads from https://indico.cern.ch/event/1011425/contributions/4272884/
    confObj["roads"] = odict()
    confObj["roads"]["rpc"] = odict([(0,0), (4,0), (6,1), (8,2), (10,3), (12,4), (14,5)])
    confObj["roads"]["tgc"] = odict([(0,0)] + list(zip([3,4,5,6,7,8,9,10,11,12,13,14,15,18,20],list(range(0,15)))))
    return confObj


def getConfig_eEM():
    confObj = odict()
    confObj["workingPoints"] = odict()
    # based on https://indico.cern.ch/event/1035198/contributions/4378014/attachments/2251846/3820098/20210526_l1calo_TGM.pdf
    confObj["workingPoints"]["Loose"] = [
        odict([("reta_fw", 58), ("reta", 0.121), ("wstot_fw", 8), ("wstot", 4.0), ("rhad_fw", 16), ("rhad", 0.333), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
        odict([("reta_fw", 53), ("reta", 0.131), ("wstot_fw", 8), ("wstot", 4.0), ("rhad_fw", 16), ("rhad", 0.333), ("maxEt", 60), ("etamin", -25), ("etamax", 25), ("priority", 2)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("reta_fw", 65), ("reta", 0.11), ("wstot_fw", 9), ("wstot", 3.556), ("rhad_fw", 22), ("rhad", 0.267), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("reta_fw", 72), ("reta", 0.1), ("wstot_fw", 25), ("wstot", 1.28), ("rhad_fw", 23), ("rhad", 0.258), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["ptMinToTopo"] = 3
    confObj["resolutionMeV"] = 100

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in eEM configuration is not an integer!", ssthr_i )

    return confObj

def getConfig_jEM():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("iso_fw", 58), ("iso", 0.121), ("frac_fw", 8), ("frac", 4.0), ("frac2_fw", 16), ("frac2", 0.333), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("iso_fw", 65), ("iso", 0.11), ("frac_fw", 9), ("frac", 3.556), ("frac2_fw", 22), ("frac2", 0.267), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("iso_fw", 72), ("iso", 0.1), ("frac_fw", 25), ("frac", 1.28), ("frac2_fw", 23), ("frac2", 0.258), ("maxEt", 60), ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["ptMinToTopo"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 15)])
    ]
    confObj["ptMinxTOB"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 15)])
    ]
    confObj["resolutionMeV"] = 200

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in jEM configuration is not an integer!", ssthr_i )

    return confObj

def getConfig_eTAU():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isoConeRel", 0.150), ("isoConeRel_fw", 40), ("fEM", 1), ("fEM_fw", 1), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isoConeRel", 0.175), ("isoConeRel_fw", 50), ("fEM", 1), ("fEM_fw", 1), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isoConeRel", 0.200), ("isoConeRel_fw", 60), ("fEM", 1), ("fEM_fw", 1), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Had"] = [
        odict([("isoConeRel", 1), ("isoConeRel_fw", 1), ("fEM", 0.2), ("fEM_fw", 10), ("maxEt", 60)]),
    ]
    confObj["ptMinToTopo"] = 5
    confObj["resolutionMeV"] = 100

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in eTAU configuration is not an integer!", ssthr_i )
    return confObj

def getConfig_cTAU():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isolation", 0.40), ("isolation_fw", 410)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isolation", 0.35), ("isolation_fw", 358)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isolation", 0.30), ("isolation_fw", 307)]),
    ]
    confObj["resolutionMeV"] = 100

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in cTAU configuration is not an integer!", ssthr_i )
    return confObj

def getConfig_jTAU():
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isolation", 0.40), ("isolation_fw", 40), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isolation", 0.35), ("isolation_fw", 50), ("maxEt", 60)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isolation", 0.30), ("isolation_fw", 60), ("maxEt", 60)]),
    ]
    confObj["ptMinToTopo"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 5)])
    ]
    confObj["ptMinxTOB"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 5)])
    ]
    confObj["resolutionMeV"] = 200

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in jTAU configuration is not an integer!", ssthr_i )
    return confObj

def getConfig_jJ():
    confObj = odict()
    confObj["ptMinToTopo"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 12)])
    ]
    confObj["ptMinxTOB"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 12)])
    ]
    confObj["resolutionMeV"] = 200  
    return confObj

def getConfig_jLJ():
    confObj = odict()
    confObj["ptMinToTopo"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 80)])
    ]
    confObj["ptMinxTOB"] = [
        odict([("etamin",-49), ("etamax",49), ("value", 80)])
    ]
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_jXE():
    confObj = odict()
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_jTE():
    confObj = odict()
    confObj["etaBoundary"] = 32
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_gXE():
    confObj = odict()
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_gTE():
    confObj = odict()
    confObj["resolutionMeV"] = 200
    return confObj


# LEGACY

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

