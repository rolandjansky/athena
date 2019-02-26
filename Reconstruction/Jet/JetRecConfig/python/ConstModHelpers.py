# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# ConstModUtils: A module for configuring constituent modifiers        #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################
import cppyy
try:
    cppyy.loadDictionary('xAODBaseObjectTypeDict')
except:
    pass
from ROOT import xAODType
xAODType.ObjectType


from AthenaCommon import Logging
constmodlog = Logging.logging.getLogger('ConstModHelpers')

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

from JetRec import JetRecConf
from JetRecTools import JetRecToolsConf
from PFlowUtils import PFlowUtilsConf

# Tool types and configs can be expanded if the user
# wishes to add their own custom definitions

# Maybe we need a config class in JetDefinition?
ConstModTools = {
    "Origin": JetRecToolsConf.CaloClusterConstituentsOrigin,
    "EM":     JetRecToolsConf.ClusterAtEMScaleTool,
    "Vor":    JetRecToolsConf.VoronoiWeightTool,
    "CS":     JetRecToolsConf.ConstituentSubtractorTool,
    "SK":     JetRecToolsConf.SoftKillerWeightTool
}

ConstModConfigs = {
    "Origin": {},
    "EM":     {},
    "Vor":    {"doSpread":False, "nSigma":0},
    "CS":     {"MaxEta":5.},
    "SK":     {}
}

def ConstitModCfg(inputtype,sequence,suffix=""):
    components = ComponentAccumulator()

    # May wish to pass an empty sequence for regular PFlow
    modlist = []
    if inputtype == xAODType.ParticleFlow:
        weightPFO = PFlowUtilsConf.CP__WeightPFOTool("weightPFO")
        correctPFO = JetRecToolsConf.CorrectPFOTool("correctPFO",
            InputType = inputtype,
            WeightPFOTool = weightPFO
        )
        modlist.append(correctPFO)
    inputname = {xAODType.CaloCluster:  "TopoCluster",
                 xAODType.ParticleFlow: "EMPFlow"
                 }[inputtype]

    for step in sequence:
        if step == "LC":
            continue # Nothing to do for LC clusters
        tool = None

        toolname = "ConstitMod{0}_{1}{2}".format(inputname,step,suffix)
        tool = ConstModTools[step](toolname,**ConstModConfigs[step])
        if inputtype == xAODType.ParticleFlow:
            tool.IgnoreChargedPFO=True
            tool.ApplyToChargedPFO=False
        tool.InputType = inputtype
        modlist.append(tool)

    sequenceshort = "".join(sequence)
    seqname = "ConstitMod{0}_{1}{2}".format(sequenceshort,inputname,suffix)
    inputcontainer = ""
    outputcontainer = ""
    if inputtype==xAODType.ParticleFlow:
        inputcontainer = "JetETMiss"
        outputcontainer = sequenceshort if sequenceshort else "CHS"
        chstool = JetRecToolsConf.ChargedHadronSubtractionTool("chsPFO")
        chstool.InputType = inputtype
        modlist.append(chstool)
    elif inputtype==xAODType.CaloCluster:
        inputcontainer = "CaloCalTopoClusters"
        outputcontainer = sequenceshort+"TopoClusters"
    else:
        constmodlog.error("Only ParticleFlow and CaloCluster currently supported!")
        raise TypeError("Unsupported input type {0}".format(inputtype))

    # If no mods are needed, don't give back a tool
    if not modlist: return components

    modseq = JetRecToolsConf.JetConstituentModSequence(seqname,
        InputType=inputtype,
        OutputContainer = outputcontainer,
        InputContainer= inputcontainer,
        Modifiers = [ mod for mod in modlist]
    )

    constitmodalg = JetRecConf.JetAlgorithm("jetalg_{0}".format(modseq.getName()))
    constitmodalg.Tools = [modseq]
    components.addEventAlgo( constitmodalg )

    return components
