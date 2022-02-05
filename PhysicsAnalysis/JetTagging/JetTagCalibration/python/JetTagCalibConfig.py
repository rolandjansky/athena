# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def JetTagCalibCfg(ConfigFlags, scheme="", TaggerList = [], **kwargs):

    CalibrationChannelAliases = [   "myOwnCollection->AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt4Tower->AntiKt4Tower,AntiKt4H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt4Topo->AntiKt4Topo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo",
                                    "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4Topo,AntiKt4H1Topo",
                                    "AntiKt6Tower->AntiKt6Tower,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt6Topo->AntiKt6Topo,AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt6LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt6Topo,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt4TopoEM->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo,AntiKt4LCTopo",
                                    "AntiKt6TopoEM->AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                                    #WOUTER: I added some more aliases here that were previously set up at ./python/BTagging_jobOptions.py. But it cannot
                                    #stay there if we want support for JetRec to setup b-tagging from their end.
                                    "AntiKt4EMTopo->AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                                    "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt4EMTopoOrigin->AntiKt4EMTopoOrigin,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                                    "AntiKt4LCTopoOrigin->AntiKt4LCTopoOrigin,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt10LCTopo->AntiKt10LCTopo,AntiKt6LCTopo,AntiKt6TopoEM,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                                    "AntiKt10Truth->AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt10TruthWZ->AntiKt10TruthWZ,AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt4Truth->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt4TruthWZ->AntiKt4TruthWZ,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt4Track->AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt3Track->AntiKt3Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt2Track->AntiKt2Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                                    "AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                                    "AntiKt4HI->AntiKt4HI,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo"]

    newChannel = kwargs.get("NewChannel")
    if newChannel:
        CalibrationChannelAliases+=newChannel

    #IP2D
    grades= ConfigFlags.BTagging.Grades

    #IP3D
    #Same as IP2D. Revisit JetTagCalibCondAlg.cxx if not.

    #RNNIP
    RNNIPConfig = {'rnnip':''}

    if scheme and scheme != "Trig":
        raise RuntimeError(f"Invalid 'scheme' passed to JetTagCalibCfg: {scheme}")

    result = ComponentAccumulator()
    readkeycalibpath = f"/GLOBAL/Onl/{scheme}BTagCalib/RUN12"

    if ConfigFlags.Input.isMC:
        readkeycalibpath = readkeycalibpath.replace("/Onl/","/")

    result.merge(addFoldersSplitOnline(ConfigFlags, "GLOBAL",
                                       [f"/GLOBAL/Onl/{scheme}BTagCalib/RUN12"],
                                       [f"/GLOBAL/{scheme}BTagCalib/RUN12"],
                                       className="CondAttrListCollection",
                                       splitMC=True))

    JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
    result.addCondAlgo(JetTagCalibCondAlg("JetTagCalibCondAlg",
                                          ReadKeyCalibPath = readkeycalibpath,
                                          HistosKey = f"JetTag{scheme}CalibHistosKey",
                                          taggers = TaggerList,
                                          channelAliases = CalibrationChannelAliases,
                                          IP2D_TrackGradePartitions = grades,
                                          RNNIP_NetworkConfig = RNNIPConfig))
    return result
