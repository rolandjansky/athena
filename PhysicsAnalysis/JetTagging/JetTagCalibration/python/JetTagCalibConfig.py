# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

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

    if scheme == "Trig":
        JetTagCalibCondAlg,=CompFactory.getComps("Analysis__JetTagCalibCondAlg",)
        jettagcalibcondalg = "JetTagCalibCondAlg"
        histoskey = "JetTagTrigCalibHistosKey"
        readkeycalibpath = "/GLOBAL/TrigBTagCalib/RUN12"
        connSchema = "GLOBAL_OFL"
        if not ConfigFlags.Input.isMC:
            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/","/GLOBAL/Onl/")
            connSchema = connSchema.replace("OFL","ONL")
        from IOVDbSvc.CondDB import conddb
        conddb.addFolder(connSchema, readkeycalibpath, className='CondAttrListCollection')

        JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)
        return JetTagCalib

    else:
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        result=ComponentAccumulator()
        JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
        jettagcalibcondalg = "JetTagCalibCondAlg"
        histoskey = "JetTagCalibHistosKey"
        readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
        connSchema = "GLOBAL_OFL"
        if not ConfigFlags.Input.isMC:
            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/","/GLOBAL/Onl/")
            connSchema = connSchema.replace("_OFL","")
        from IOVDbSvc.IOVDbSvcConfig import addFolders
        result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))

        JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)
        result.addCondAlgo(JetTagCalib)
        return result


#    #conf2toConfigurable is not working for this at the moment, thats why we need old-config style for trigger
#    #if this option would be working, need the following line in runHLT_standalone.py
#    #CAtoGlobalWrapper(JetTagCalibCfg, ConfigFlags, scheme="Trig", TaggerList=ConfigFlags.BTagging.TrigTaggersList, ChannelAlias = alias)
#
#    from IOVDbSvc.IOVDbSvcConfig import addFolders
#    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#    result=ComponentAccumulator()
#    JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
#    jettagcalibcondalg = "JetTagCalibCondAlg"
#    connSchema = "GLOBAL_OFL"
#    if scheme == "Trig":
#        histoskey = "JetTagTrigCalibHistosKey"
#        readkeycalibpath = "/GLOBAL/TrigBTagCalib/RUN12"
#        if not ConfigFlags.Input.isMC:
#            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/","/GLOBAL/Onl/")
#            connSchema = connSchema.replace("OFL","ONL")
#    else:
#        histoskey = "JetTagCalibHistosKey"
#        readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
#        if not ConfigFlags.Input.isMC:
#            readkeycalibpath = readkeycalibpath.replace("/GLOBAL/","/GLOBAL/Onl/")
#            connSchema = connSchema.replace("_OFL","")
#
#    result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))
#    JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)
#    result.addCondAlgo(JetTagCalib)
#    return result
