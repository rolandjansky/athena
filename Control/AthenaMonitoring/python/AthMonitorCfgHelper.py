#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

class AthMonitorCfgHelper(object):
    def __init__(self, inputFlags, monName):
        from AthenaCommon.AlgSequence import AthSequencer
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        self.inputFlags = inputFlags
        self.monName = monName
        self.monSeq = AthSequencer('AthMonSeq_' + monName)
        self.resobj = ComponentAccumulator()

    def addAlgorithm(self,algClassOrObj, *args, **kwargs):
        from AthenaCommon.Configurable import Configurable
        if issubclass(algClassOrObj, Configurable):
            algObj = algClassOrObj(*args, **kwargs)
        else:
            algObj = algClassOrObj
        
        # configure these properties; users really should have no reason to override them
        algObj.Environment = self.inputFlags.DQ.Environment
        algObj.DataType = self.inputFlags.DQ.DataType

        self.monSeq += algObj
        return algObj

    def addGroup(self, alg, name, topPath=''):
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
        tool = GenericMonitoringTool(name)
        acc, histsvc = GetDQTHistSvc(self.inputFlags)
        self.resobj.merge(acc)
        tool.THistSvc = histsvc
        tool.HistPath = self.inputFlags.DQ.FileKey + ('/%s' % topPath if topPath else '')
        alg.GMTools += [tool]
        return tool

    def result(self):
        self.resobj.addSequence(self.monSeq)
        return self.resobj,self.monSeq

def GetDQTHistSvc(inputFlags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from GaudiSvc.GaudiSvcConf import THistSvc

    result = ComponentAccumulator()
    histsvc = THistSvc()
    histsvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % (inputFlags.DQ.FileKey, 
                                                            inputFlags.Output.HISTFileName)]
    result.addService(histsvc)
    return result, histsvc
