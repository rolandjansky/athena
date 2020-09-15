# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

########################################################################
#                                                                      #
# ConstModUtils: A module for configuring constituent modifiers        #
# Author: TJ Khoo                                                      #
#                                                                      #
########################################################################
import cppyy
try:
    cppyy.load_library('libxAODBaseObjectTypeDict')
except Exception:
    pass
from ROOT import xAODType
xAODType.ObjectType

from AthenaCommon import Logging
constmodlog = Logging.logging.getLogger('ConstModHelpers')

from AthenaConfiguration.ComponentFactory import CompFactory

# Tool types and configs can be expanded if the user
# wishes to add their own custom definitions

# Maybe we need a config class in JetDefinition?
ConstModTools = {
    # Topoclusters
    "Origin": CompFactory.CaloClusterConstituentsOrigin,
    "EM":     CompFactory.ClusterAtEMScaleTool,
    # Particle flow
    "CorrectPFO": 
              CompFactory.CorrectPFOTool,
    "CHS":    CompFactory.ChargedHadronSubtractionTool,
    # Pileup suppression
    "Vor":    CompFactory.VoronoiWeightTool,
    "CS":     CompFactory.ConstituentSubtractorTool,
    "SK":     CompFactory.SoftKillerWeightTool
}

ConstModConfigs = {
    # Topoclusters
    "Origin": {},
    "EM":     {},
    # Particle flow
    "CorrectPFO":
              {"WeightPFOTool": CompFactory.getComp("CP::WeightPFOTool")("weightPFO")},
    "CHS":    {},
    # Pileup suppression
    "Vor":    {"doSpread":False, "nSigma":0},
    "CS":     {"MaxEta":5.},
    "SK":     {}
}

def getConstitModAlg(constit,suffix="",tvaKey="JetTrackVtxAssoc",vtxKey="PrimaryVertices",monTool=None):
    inputtype = constit.basetype

    # Need to extend to TCC
    if inputtype not in [xAODType.CaloCluster, xAODType.ParticleFlow]:
        constmodlog.error("Only ParticleFlow and CaloCluster currently supported!")
        raise TypeError("Unsupported input type {0}".format(inputtype))


    sequence = list(constit.modifiers) # Copy, as we may make some additions
    typename = {xAODType.CaloCluster:  "TopoCluster",
                xAODType.ParticleFlow: "EMPFlow"
                }[inputtype]

    if inputtype == xAODType.ParticleFlow:
        # Always do 4mom corrections first and CHS last
        sequence = ["CorrectPFO"] + sequence + ["CHS"]

    # If no mods are needed, don't give back a tool
    if sequence == []: return None

    modlist = []
    for step in sequence:
        if step == "LC":
            continue # Nothing to do for LC clusters
        tool = None

        toolname = "ConstitMod{0}_{1}{2}".format(typename,step,suffix)
        tool = ConstModTools[step](toolname,**ConstModConfigs[step])

        # May want to set also for cluster origin correction
        # but so far unused
        if step=="CorrectPFO":
            tool.VertexContainerKey = vtxKey
        if step=="CHS":
            tool.TrackVertexAssociation = tvaKey
            tool.VertexContainerKey = vtxKey
        
        if inputtype == xAODType.ParticleFlow and step not in ["CorrectPFO","CHS"]:
            tool.IgnoreChargedPFO=True
            tool.ApplyToChargedPFO=False
        tool.InputType = inputtype
        modlist.append(tool)

    sequenceshort = "".join(sequence)
    seqname = "ConstitMod{0}_{1}{2}".format(sequenceshort,typename,suffix)
    inputcontainer = str(constit.rawname)
    outputcontainer = str(constit.inputname)
    if inputtype==xAODType.ParticleFlow:
        # Tweak PF names because ConstModSequence needs to work with
        # up to 4 containers
        def chopPFO(thestring):
            pfostr = "ParticleFlowObjects"
            if thestring.endswith(pfostr):
                return thestring[:-len(pfostr)]
            return thestring
        inputcontainer = chopPFO(inputcontainer)
        outputcontainer = chopPFO(outputcontainer)

    modseq = CompFactory.JetConstituentModSequence(seqname,
        InputType=inputtype,
        OutputContainer = outputcontainer,
        InputContainer= inputcontainer,
        Modifiers = modlist,
        MonTool = monTool
    )

    constitmodalg = CompFactory.JetAlgorithm("jetalg_{0}".format(modseq.getName()))
    constitmodalg.Tools = [modseq]

    return constitmodalg
