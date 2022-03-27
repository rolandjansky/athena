# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from collections import OrderedDict as odict

from AthenaCommon.Logging import logging
log = logging.getLogger(__name__)

from ..Base.ThresholdType import ThrType

# eFEX conversions based on https://indico.cern.ch/event/1026972/contributions/4312070/attachments/2226175/3772176/Copy%20of%20Reta_Threshold_Setting.pdf
# ATR-23596
def eFEXfwToFloatConversion(fw,bitshift):
    decimal = 1/(1+fw/pow(2,bitshift))
    return float("{:.3f}".format(decimal))

def eFEXfwToFloatConversion_wstot(fw,bitshift):
    decimal = pow(2,bitshift)/fw
    return float("{:.3f}".format(decimal))

# jFEX conversion based on ATR-21235
def jFEXfloatToFWConversion(decimal):
    fw = round((1-decimal)/decimal)
    return fw

def cTAUfwToFlowConversion(fw):
    decimal = fw/1024
    return float("{:.2f}".format(decimal))

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
    if ttype == ThrType.gJ:
        return getConfig_gJ()
    if ttype == ThrType.gLJ:
        return getConfig_gLJ()
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
    confObj["roads"]["rpc"] = odict([(0,0), (4,1), (6,2), (8,3), (10,4), (12,5), (14,6)])
    confObj["roads"]["tgc"] = odict([(0,0)] + list(zip([3,4,5,6,7,8,9,10,11,12,13,14,15,18,20],list(range(1,16)))))

    # check that there is a unique assignment between roads and pt values
    for muonDet in ("rpc","tgc"):
        roads = []
        ptValues = []
        for ptValue in confObj["roads"][muonDet]: 
            if ptValue in ptValues:
                raise RuntimeError("Muon roads: pt value %s is duplicated, please fix.", str(ptValue) ) 
            else: 
                ptValues += [ptValue]
            road = confObj["roads"][muonDet][ptValue]
            if road !=0 and road in roads:
                raise RuntimeError("Muon roads: road %s is duplicated, please fix.", str(road) )
            else:
                roads += [road]
    if len(confObj["roads"]["rpc"])!=7 or len(confObj["roads"]["tgc"])!=16:
        raise RuntimeError("Muon roads: number of roads not as expected. TGC=%s (exp 16), RCP=%s (exp 7)", len(confObj["roads"]["tgc"]), len(confObj["roads"]["rpc"]) )

    return confObj


def getConfig_eEM():
    confObj = odict()
    confObj["workingPoints"] = odict()
    bitshift_reta = 3
    bitshift_rhad = 3
    bitshift_wstot = 5   
    reta_fw_loose = 53
    reta_fw_medium = 65
    reta_fw_tight = 72
    rhad_fw_loose = 16
    rhad_fw_medium = 22
    rhad_fw_tight = 23
    wstot_fw_loose = 8
    wstot_fw_medium = 9
    wstot_fw_tight = 25 
    # based on https://indico.cern.ch/event/1035198/contributions/4378014/attachments/2251846/3820098/20210526_l1calo_TGM.pdf
    confObj["workingPoints"]["Loose"] = [
        odict([("reta_fw", reta_fw_loose), ("reta", eFEXfwToFloatConversion(reta_fw_loose,bitshift_reta)), 
               ("wstot_fw", wstot_fw_loose), ("wstot", eFEXfwToFloatConversion_wstot(wstot_fw_loose,bitshift_wstot)), 
               ("rhad_fw", rhad_fw_loose), ("rhad", eFEXfwToFloatConversion(rhad_fw_loose,bitshift_rhad)), 
               ("etamin", -49), ("etamax", 49), ("priority", 1)]),
               # another item can be added with higher priority to set eta-dependent cuts
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("reta_fw", reta_fw_medium), ("reta", eFEXfwToFloatConversion(reta_fw_medium,bitshift_reta)), 
               ("wstot_fw", wstot_fw_medium), ("wstot", eFEXfwToFloatConversion_wstot(wstot_fw_medium,bitshift_wstot)), 
               ("rhad_fw", rhad_fw_medium), ("rhad", eFEXfwToFloatConversion(rhad_fw_medium,bitshift_rhad)), 
               ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("reta_fw", reta_fw_tight), ("reta", eFEXfwToFloatConversion(reta_fw_tight,bitshift_reta)), 
               ("wstot_fw", wstot_fw_tight), ("wstot", eFEXfwToFloatConversion_wstot(wstot_fw_tight,bitshift_wstot)), 
               ("rhad_fw", rhad_fw_tight), ("rhad", eFEXfwToFloatConversion(rhad_fw_tight,bitshift_rhad)), 
               ("etamin", -49), ("etamax", 49), ("priority", 1)]),
    ]
    confObj["ptMinToTopo"] = 3 
    confObj["maxEt"] = 50
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

    iso_loose_float = 0.1 # PLACEHOLDER
    iso_medium_float = 0.1 # PLACEHOLDER
    iso_tight_float = 0.1 # PLACEHOLDER    
    frac_loose_float = 0.2 # PLACEHOLDER
    frac_medium_float = 0.2 # PLACEHOLDER
    frac_tight_float = 0.2 # PLACEHOLDER
    frac2_loose_float = 0.3 # PLACEHOLDER
    frac2_medium_float = 0.3 # PLACEHOLDER
    frac2_tight_float = 0.3 # PLACEHOLDER

    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("iso_fw", jFEXfloatToFWConversion(iso_loose_float)), ("iso", iso_loose_float),
               ("frac_fw", jFEXfloatToFWConversion(frac_loose_float)), ("frac", frac_loose_float), 
               ("frac2_fw", jFEXfloatToFWConversion(frac2_loose_float)), ("frac2", frac2_loose_float), 
               ("etamin", -49), ("etamax", 49), ("priority", 0)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("iso_fw", jFEXfloatToFWConversion(iso_medium_float)), ("iso", iso_medium_float), 
               ("frac_fw", jFEXfloatToFWConversion(frac_medium_float)), ("frac", frac_medium_float), 
               ("frac2_fw", jFEXfloatToFWConversion(frac2_medium_float)), ("frac2", frac2_medium_float), 
               ("etamin", -49), ("etamax", 49), ("priority", 0)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("iso_fw", jFEXfloatToFWConversion(iso_tight_float)), ("iso", iso_tight_float), 
               ("frac_fw", jFEXfloatToFWConversion(frac_tight_float)), ("frac", frac_tight_float), 
               ("frac2_fw", jFEXfloatToFWConversion(frac2_tight_float)), ("frac2", frac2_tight_float), 
               ("etamin", -49), ("etamax", 49), ("priority", 0)]),
    ]
    confObj["ptMinToTopo1"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo2"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo3"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB1"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB2"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB3"] = 5 # PLACEHOLDER
    confObj["maxEt"] = 50 # PLACEHOLDER
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
    bitshift_rCore = 3
    bitshift_rHad = 3
    rCore_fw_loose = 2  # rCore = 1 - (3x2)/(9x2), rCore > threshold -> pass
    rCore_fw_medium = 12 # PLACEHOLDER
    rCore_fw_tight = 32
    rHad_fw_loose = 10 # PLACEHOLDER
    rHad_fw_medium = 10 # PLACEHOLDER
    rHad_fw_tight = 10 # PLACEHOLDER
    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("rCore", eFEXfwToFloatConversion(rCore_fw_loose,bitshift_rCore)), ("rCore_fw", rCore_fw_loose), 
               ("rHad", 1), ("rHad_fw", 1),
              ]), 
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("rCore", eFEXfwToFloatConversion(rCore_fw_medium,bitshift_rCore)), ("rCore_fw", rCore_fw_medium), 
               ("rHad", 1), ("rHad_fw", 1), 
             ]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("rCore", eFEXfwToFloatConversion(rCore_fw_tight,bitshift_rCore)), ("rCore_fw", rCore_fw_tight), 
               ("rHad", 1), ("rHad_fw", 1), 
             ]),
    ]
    confObj["workingPoints"]["HadLoose"] = [
        odict([("rCore", 1), ("rCore_fw", 1), 
               ("rHad", eFEXfwToFloatConversion(rHad_fw_loose,bitshift_rHad)), ("rHad_fw", rHad_fw_loose), 
             ]),
    ]
    confObj["workingPoints"]["HadMedium"] = [
        odict([("rCore", 1), ("rCore_fw", 1),
               ("rHad", eFEXfwToFloatConversion(rHad_fw_medium,bitshift_rHad)), ("rHad_fw", rHad_fw_medium),
             ]),
    ]
    confObj["workingPoints"]["HadTight"] = [
        odict([("rCore", 1), ("rCore_fw", 1),
               ("rHad", eFEXfwToFloatConversion(rHad_fw_tight,bitshift_rHad)), ("rHad_fw", rHad_fw_tight),
             ]),
    ]
    confObj["ptMinToTopo"] = 5 # PLACEHOLDER
    confObj["resolutionMeV"] = 100
    confObj["maxEt"] = 50 # PLACEHOLDER

    # Check that FW values are integers
    for wp in confObj["workingPoints"]:
        for ssthr in confObj["workingPoints"][wp]:
            for ssthr_i in ssthr:
                if "_fw" in ssthr_i:
                     if not isinstance(ssthr[ssthr_i], int):
                          raise RuntimeError("Threshold %s in eTAU configuration is not an integer!", ssthr_i )
    return confObj

def getConfig_cTAU():
    isolation_fw_loose = 410
    isolation_fw_medium = 358
    isolation_fw_tight = 307 # PLACEHOLDER

    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_loose)), ("isolation_fw", isolation_fw_loose)]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_medium)), ("isolation_fw", isolation_fw_medium)]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_tight)), ("isolation_fw", isolation_fw_tight)]),
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
    isolation_fw_loose = 410
    isolation_fw_medium = 358 # PLACEHOLDER
    isolation_fw_tight = 307 # PLACEHOLDER

    confObj = odict()
    confObj["workingPoints"] = odict()
    confObj["workingPoints"]["Loose"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_loose)), ("isolation_fw", isolation_fw_loose), 
              ]),
    ]
    confObj["workingPoints"]["Medium"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_medium)), ("isolation_fw", isolation_fw_medium), 
              ]),
    ]
    confObj["workingPoints"]["Tight"] = [
        odict([("isolation", cTAUfwToFlowConversion(isolation_fw_tight)), ("isolation_fw", isolation_fw_tight), 
              ]),
    ]
    confObj["ptMinToTopo1"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo2"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo3"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB1"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB2"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB3"] = 5 # PLACEHOLDER
    confObj["resolutionMeV"] = 200
    confObj["maxEt"] = 50 # PLACEHOLDER

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
    confObj["ptMinToTopo1"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo2"] = 5 # PLACEHOLDER
    confObj["ptMinToTopo3"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB1"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB2"] = 5 # PLACEHOLDER
    confObj["ptMinxTOB3"] = 5 # PLACEHOLDER
    confObj["resolutionMeV"] = 200  
    return confObj

def getConfig_jLJ():
    confObj = odict()
    confObj["ptMinToTopo1"] = 15 # PLACEHOLDER
    confObj["ptMinToTopo2"] = 15 # PLACEHOLDER
    confObj["ptMinToTopo3"] = 15 # PLACEHOLDER
    confObj["ptMinxTOB1"] = 15 # PLACEHOLDER
    confObj["ptMinxTOB2"] = 15 # PLACEHOLDER
    confObj["ptMinxTOB3"] = 15 # PLACEHOLDER
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_gJ():
    confObj = odict()
    confObj["ptMinToTopo1"] = 0 
    confObj["ptMinToTopo2"] = 0 
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_gLJ():
    confObj = odict()
    confObj["ptMinToTopo1"] = 6 
    confObj["ptMinToTopo2"] = 6 
    confObj["seedThrA"] = 3 
    confObj["seedThrB"] = 3 
    confObj["seedThrC"] = 3 
    confObj["rhoTowerMinA"] = -9.6 
    confObj["rhoTowerMinB"] = -9.6 
    confObj["rhoTowerMinC"] = -9.6 
    confObj["rhoTowerMaxA"] = 0.25 
    confObj["rhoTowerMaxB"] = 0.25 
    confObj["rhoTowerMaxC"] = 0.25 
    confObj["resolutionMeV"] = 200

    # Check that all values are integers in MeV
    for param in confObj:
        if int(confObj[param]*1000) != (confObj[param]*1000):
            raise RuntimeError("Param %s in gLJ configuration is not an integer in MeV! %d", param, confObj[param])
    return confObj

def getConfig_jXE():
    confObj = odict()
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_jTE():

    def convertTowerToEta(tower, module):  #ATR-21235
        boundaries1 = {0:16, 1:17, 2:18, 3:19, 4:20, 5:21, 6:22, 7:23, 8:24, 9:25, 10:27, 11:29, 12:31, 13:32, 14:49}
        boundaries2 = {0:8, 1:9, 2:10, 3:11, 4:12, 5:13, 6:14, 7:15, 8:16}
        boundaries3 = {0:0, 1:1, 2:2, 3:3, 4:4, 5:5, 6:6, 7:7, 8:8}
        if module==1:
            return boundaries1[tower]
        elif module==2:
            return boundaries2[tower]
        elif module==3:
            return boundaries3[tower]
        else:
            raise RuntimeError("getConfig_jTE::convertTowerToEta: module not recognised") 

    module1 = 9
    module2 = 4
    module3 = 4

    confObj = odict()
    confObj["etaBoundary1"] = convertTowerToEta(module1,1)
    confObj["etaBoundary1_fw"] = module1
    confObj["etaBoundary2"] = convertTowerToEta(module2,2)
    confObj["etaBoundary2_fw"] = module2
    confObj["etaBoundary3"] = convertTowerToEta(module3,3)
    confObj["etaBoundary3_fw"] = module3
    confObj["resolutionMeV"] = 200
    return confObj

def getConfig_gXE():
    confObj = odict()
    confObj["seedThrA"] = 1 
    confObj["seedThrB"] = 1 
    confObj["seedThrC"] = 1 
    confObj["XERHO_sigmaPosA"] = 3 
    confObj["XERHO_sigmaPosB"] = 3 
    confObj["XERHO_sigmaPosC"] = 3 
    confObj["XERHO_sigmaNegA"] = 8 
    confObj["XERHO_sigmaNegB"] = 8 
    confObj["XERHO_sigmaNegC"] = 8 
    confObj["XEJWOJ_a_A"] = 48 
    confObj["XEJWOJ_a_B"] = 48 
    confObj["XEJWOJ_a_C"] = 48 
    confObj["XEJWOJ_b_A"] = 52 
    confObj["XEJWOJ_b_B"] = 52 
    confObj["XEJWOJ_b_C"] = 52 
    confObj["XEJWOJ_c_A"] = 0 
    confObj["XEJWOJ_c_B"] = 0 
    confObj["XEJWOJ_c_C"] = 0 
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

