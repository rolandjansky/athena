# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from BTagging.BTaggingFlags import BTaggingFlags

def JetTagCalibCfg(ConfigFlags, scheme="", TaggerList = [], ChannelAlias = ""):

    grades= [ "0HitIn0HitNInExp2","0HitIn0HitNInExpIn","0HitIn0HitNInExpNIn","0HitIn0HitNIn",
            "0HitInExp", "0HitIn",
            "0HitNInExp", "0HitNIn",
            "InANDNInShared", "PixShared", "SctShared",
            "InANDNInSplit", "PixSplit",
            "Good"]

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

    CalibrationChannelAliases += ChannelAlias

    #JetTagCalibCondAlg,=CompFactory.getComps("Analysis__JetTagCalibCondAlg",)
    #JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
    #jettagcalibcondalg = "JetTagCalibCondAlg"

    if scheme == "Trig":
        JetTagCalibCondAlg,=CompFactory.getComps("Analysis__JetTagCalibCondAlg",)
        jettagcalibcondalg = "JetTagCalibCondAlg"
        readkeycalibpath = "/GLOBAL/Onl/TrigBTagCalib/RUN12"
        histoskey = "JetTagTrigCalibHistosKey"
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/TrigBTagCalib/RUN12", className='CondAttrListCollection')
        #if conddb.dbdata == 'COMP200':
        #  conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/BTagCalib/RUN12", className='CondAttrListCollection')
        #  if globalflags.DataSource()!='data':
        #    conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/TrigBTagCalib/RUN12", className='CondAttrListCollection')
        #elif conddb.isMC:
        #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/BTagCalib/RUN12", className='CondAttrListCollection')
        #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/TrigBTagCalib/RUN12", className='CondAttrListCollection')

        JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = ConfigFlags.BTagging.TrigTaggersList, channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = BTaggingFlags.RNNIPConfig)
        return JetTagCalib

    else:
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        result=ComponentAccumulator()
        JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
        jettagcalibcondalg = "JetTagCalibCondAlg"
        readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
        histoskey = "JetTagCalibHistosKey"
        connSchema = "GLOBAL_OFL"
        if not ConfigFlags.Input.isMC:
            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
            connSchema = "GLOBAL"
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))

        JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = BTaggingFlags.CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = BTaggingFlags.RNNIPConfig)
        result.addCondAlgo(JetTagCalib)
        return result


