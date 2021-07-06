# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def OutputConditionsAlgCfg(flags, name="OutputConditionsAlg",
                           outputFile='condobjs.root',
                           ObjectList=[],IOVTagList=[],WriteIOV=True):

    from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg

    result=PoolWriteCfg(flags)

    
    oca=CompFactory.OutputConditionsAlg(name,ObjectList=ObjectList,
                                        IOVTagList=IOVTagList,WriteIOV=WriteIOV)

    
    # create outputStream tool with given filename and pass to myOCA
    condstream=CompFactory.AthenaOutputStreamTool(name+"Tool")

    #To be fixed: OutputConditionsAlgo works with a string-name of a public tool
    oca.StreamName=name+"Tool"
    condstream.OutputFile=outputFile
    condstream.PoolContainerPrefix="ConditionsContainer"
    result.addPublicTool(condstream)

    result.addEventAlgo(oca)
    
    return result
