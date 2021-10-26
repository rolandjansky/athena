# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#The Bad Channel handling of calibration jobs is sufficently non-standard to
#justify a separate config-file

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
LArBadChannelCondAlg, LArBadFebCondAlg =CompFactory.getComps("LArBadChannelCondAlg","LArBadFebCondAlg")
from IOVDbSvc.IOVDbSvcConfig import addFolders


def LArCalibBadChannelCfg(flags):
    result=ComponentAccumulator()

    foldername="/LAR/BadChannelsOfl/BadChannels"
    foldertag="".join(foldername.split("/"))+flags.LArCalib.BadChannelTag

    result.merge(addFolders(flags,foldername+"<tag>"+foldertag+"</tag>",flags.LArCalib.BadChannelDB,
                            className="CondAttrListCollection"))

    theLArBadChannelCondAlgo=LArBadChannelCondAlg(ReadKey=foldername)
    result.addCondAlgo(theLArBadChannelCondAlgo)
    return result


