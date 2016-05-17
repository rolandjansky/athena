# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file OutputStreamAthenaRoot.py
## @brief Helper methods to create output streams
## @author Sebastien Binet <binet@cern.ch>, Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: OutputStreamAthenaRoot.py 723505 2016-02-11 22:29:40Z gemmeren $
###############################################################

import AthenaCommon.CfgMgr as CfgMgr
from AthenaCommon.AppMgr import theApp
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaRootCompsConf import Athena__RootOutputStreamTool as AthenaRootOutputStreamTool

def createNtupleOutputStream(streamName, fileName, tupleName="physics", asAlg = False):
    if asAlg:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()
        pass
    else:
        theApp.OutStreamType = "Athena::RootNtupleOutputStream"
        theApp.OutStreamType = "AthenaOutputStream"
        pass
    
    # Now do standard output stream
    writingTool1 = AthenaRootOutputStreamTool( streamName + "Tool" )
    writingTool1.TreeName = tupleName
    #writingTool1.DataHeaderSatellites = [ "basic/:EventInfo#*" ]
    writingTool1.OutputFile = fileName
    #writingTool1.
    cls = AthenaRootNtupleOutputStreamProtect
    #cls = CfgMgr.Athena__RootNtupleOutputStream
    outputStream = cls(
        streamName,
        WritingTool = writingTool1,
        ItemList = ["RunNumber",
                    "EventNumber",],
        #StrictClidMatching = False,
        )
    if asAlg:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence += outputStream
    else:
        theApp.addOutputStream( outputStream )

    if fileName != "":
        outputStream.OutputFile = fileName
    return outputStream
        
def createOutputStream( streamName, fileName = "", asAlg = False ):
    from AthenaServices.AthenaServicesConf import AthenaOutputStream
    theApp.OutStreamType = "AthenaOutputStream"
    if asAlg:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence = AlgSequence()

    # Now do standard output stream
    writingTool1 = AthenaRootOutputStreamTool( streamName + "Tool" )
    writingTool1.TreeName = "CollectionTree"
    outputStream = AthenaOutputStream(
       streamName,
       WritingTool = writingTool1,
       ItemList    = [ "EventInfo#*" ]
       )
    if asAlg:
        from AthenaCommon.AlgSequence import AlgSequence
        topSequence += outputStream
    else:
        theApp.addOutputStream( outputStream )

    if fileName != "":
        outputStream.OutputFile = fileName

    return outputStream

def createOutputConditionStream( streamName, fileName = "" ):
    from RegistrationServices.OutputConditionsAlg import OutputConditionsAlg
    conditionStream = OutputConditionsAlg(
        streamName,
        outputFile = fileName,
        WriteIOV = False
        )
    return conditionStream

## backward compat
#AthenaRootOutputStream          = createOutputStream
#AthenaRootOutputConditionStream = createOutputConditionStream

"""
from AthenaCommon import CfgMgr
class AthenaRootOutputStreamProtect(CfgMgr.AthenaOutputStream):
    def __init__(self, name='Stream1', **kw):
        kw['name'] = kw.get('name', name)
        super(AthenaRootOutputStreamProtect, self).__init__(**kw)
        return

    def _set_output_file(self, fname):
        self._properties['OutputFile'].__set__(self, fname)
        from AthenaServices.AthenaServicesConf import AthenaOutputStream
        AthenaOutputStream("%s_FH" % (self._name,)).OutputFile = fname
        return

    def _get_output_file(self):
        return self._properties['OutputFile'].__get__(self)

    OutputFile = property(_get_output_file, _set_output_file, "fwd doc...")
    pass
"""

from AthenaCommon import CfgMgr
class AthenaRootNtupleOutputStreamProtect(CfgMgr.Athena__RootNtupleOutputStream):
    def __init__(self, name='Stream1', **kw):
        kw['name'] = kw.get('name', name)
        super(AthenaRootNtupleOutputStreamProtect, self).__init__(**kw)
        return

    def _set_output_file(self, fname):
        self._properties['OutputFile'].__set__(self, fname)
        CfgMgr.Athena__RootNtupleOutputStream("%s_FH" % (self._name,)).OutputFile = fname
        return

    def _get_output_file(self):
        return self._properties['OutputFile'].__get__(self)

    OutputFile = property(_get_output_file, _set_output_file, "fwd doc...")
    pass
