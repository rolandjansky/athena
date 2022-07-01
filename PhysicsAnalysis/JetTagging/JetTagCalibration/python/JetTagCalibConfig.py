# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def JetTagCalibCfg(ConfigFlags):

    channelAliases = ConfigFlags.BTagging.calibrationChannelAliases
    TaggerList = ConfigFlags.BTagging.taggerList
    scheme = ConfigFlags.BTagging.databaseScheme

    #IP2D
    grades= ConfigFlags.BTagging.Grades

    #RNNIP
    RNNIPConfig = {'rnnip':''}

    if scheme and scheme != "Trig":
        raise RuntimeError(f"Invalid 'scheme' passed to JetTagCalibCfg: {scheme}")

    result = ComponentAccumulator()
    readkeycalibpath = f"/GLOBAL/Onl/{scheme}BTagCalib/RUN12"

    if ConfigFlags.Input.isMC:
        readkeycalibpath = readkeycalibpath.replace("/Onl/","/")

    calibrationTag = ConfigFlags.BTagging.calibrationTag if ConfigFlags.BTagging.calibrationTag else None
    result.merge(addFoldersSplitOnline(ConfigFlags, "GLOBAL",
                                       [f"/GLOBAL/Onl/{scheme}BTagCalib/RUN12"],
                                       [f"/GLOBAL/{scheme}BTagCalib/RUN12"],
                                       className="CondAttrListCollection",
                                       splitMC=True,
                                       tag=calibrationTag))

    JetTagCalibCondAlg = CompFactory.Analysis.JetTagCalibCondAlg
    result.addCondAlgo(JetTagCalibCondAlg("JetTagCalibCondAlg",
                                          ReadKeyCalibPath = readkeycalibpath,
                                          HistosKey = f"JetTag{scheme}CalibHistosKey",
                                          taggers = TaggerList,
                                          channelAliases = channelAliases,
                                          IP2D_TrackGradePartitions = grades,
                                          RNNIP_NetworkConfig = RNNIPConfig))
    return result
