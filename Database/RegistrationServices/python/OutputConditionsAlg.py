# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# OutputConditionsAlg.py - configuration class for OutputConditionsAlg
# usage:
#  from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
#  myOCA=OutputConditionsAlg("myAlgName","outputfile.root")
#  myOCA.ObjectList=["objects to write out"]
#  myOCA.IOVTagList=["tag name"]
#  ... parameters can also be given directly to OutputConditionsAlg ctor
#  myOCA is automatically added to topSequence, and associated output stream
#  is created

def OutputConditionsAlg(name='OutputConditionsAlg',outputFile='condobjs.root',
                        ObjectList=[],IOVTagList=[],WriteIOV=True):
    import AthenaCommon.AtlasUnixStandardJob  # noqa: F401

    # get a handle on the top-level Algorithm sequence
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()

    # Load POOL support
    import AthenaPoolCnvSvc.WriteAthenaPool  # noqa: F401

    from RegistrationServices.RegistrationServicesConf import OutputConditionsAlg as OPCAlg
    myAlg=OPCAlg(name,
                 ObjectList=ObjectList,IOVTagList=IOVTagList,WriteIOV=WriteIOV)
    topSequence += myAlg

    # create outputStream tool with given filename and pass to myOCA
    from AthenaServices.AthenaServicesConf import AthenaOutputStreamTool
    toolname=name+"Tool"
    myAlg.StreamName=toolname
    condstream=AthenaOutputStreamTool(toolname)
    condstream.OutputFile=outputFile
    condstream.PoolContainerPrefix="ConditionsContainer"
    condstream.TopLevelContainerName = "<type>"
    condstream.SubLevelBranchName = "<key>"

    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc+=condstream
    # return reference to algorithm so can configure further parameters
    return myAlg
