#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''@file AthMonitorCfgHelper.py
@author C. D. Burton
@author P. Onyisi
@date 2019-01-25
@brief Helper classes for Run 3 monitoring algorithm configuration
'''

class AthMonitorCfgHelper(object):
    '''
    This class is for the Run 3-style configuration framework. It is intended to be instantiated once
    per group of related monitoring algorithms.
    '''
    def __init__(self, inputFlags, monName):
        '''
        Create the configuration helper. Needs the global flags and the name of the set of
        monitoring algorithms.

        Arguments:
        inputFlags -- the global configuration flag object
        monName -- the name you want to assign the family of algorithms
        '''
        from AthenaCommon.AlgSequence import AthSequencer
        from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
        self.inputFlags = inputFlags
        self.monName = monName
        self.monSeq = AthSequencer('AthMonSeq_' + monName)
        self.resobj = ComponentAccumulator()

    def addAlgorithm(self, algClassOrObj, name = None, *args, **kwargs):
        '''
        Instantiate/add a monitoring algorithm

        Arguments:
        algClassOrObj -- the Configurable class object of the algorithm to create, or an instance
                         of the algorithm Configurable. The former is recommended.  In the former case,
                         the name argument is required.
        name -- the name of the algorithm to create. Required when passing a Configurable class object
                as algClassOrObj.  No effect if a Configurable instance is passed.
        *args, **kwargs -- additional arguments will be forwarded to the Configurable constructor if
                           a Configurable class object is passed. No effect if a Configurable instance
                           is passed.

        Returns:
        algObj -- an algorithm Configurable object
        '''
        from AthenaCommon.Configurable import Configurable
        if issubclass(algClassOrObj, Configurable):
            if name is None:
                raise TypeError('addAlgorithm with a class argument requires a name for the algorithm')
            algObj = algClassOrObj(name, *args, **kwargs)
        else:
            algObj = algClassOrObj
        
        # configure these properties; users really should have no reason to override them
        algObj.Environment = self.inputFlags.DQ.Environment
        algObj.DataType = self.inputFlags.DQ.DataType

        self.monSeq += algObj
        return algObj

    def addGroup(self, alg, name, topPath=''):
        '''
        Add a "group" (technically, a GenericMonitoringTool instance) to an algorithm. The name given
        here can be used to retrieve the group from within the algorithm when calling the fill()
        function.  (Note this is *not* the same thing as the Monitored::Group class.)

        Arguments:
        alg -- algorithm Configurable object (e.g. one returned from addAlgorithm)
        name -- name of the group
        topPath -- directory name in the output ROOT file under which histograms will be produced

        Returns:
        tool -- a GenericMonitoringTool Configurable object. This can be used to define histograms
                associated with that group (using defineHistogram).
        '''
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
        tool = GenericMonitoringTool(name)
        acc, histsvc = getDQTHistSvc(self.inputFlags)
        self.resobj.merge(acc)
        tool.THistSvc = histsvc
        tool.HistPath = self.inputFlags.DQ.FileKey + ('/%s' % topPath if topPath else '')
        alg.GMTools += [tool]
        return tool

    def result(self):
        '''
        This function should be called to finalize the creation of the set of monitoring algorithms.

        Returns:
        (resobj, monSeq) -- a tuple with a ComponentAccumulator and an AthSequencer
        '''
        self.resobj.addSequence(self.monSeq)
        return self.resobj,self.monSeq

class AthMonitorCfgHelperOld(object):
    ''' 
    This is the version of the AthMonitorCfgHelper for the old-style jobOptions framework
    '''
    def __init__(self, dqflags, monName):
        '''
        Create the configuration helper. Needs the global flags and the name of the set of
        monitoring algorithms.

        Arguments:
        dqflags -- the DQMonFlags object
        monName -- the name you want to assign the family of algorithms
        '''
        from AthenaCommon.AlgSequence import AthSequencer
        self.dqflags = dqflags
        self.monName = monName
        self.monSeq = AthSequencer('AthMonSeq_' + monName)

    def addAlgorithm(self,algClassOrObj, *args, **kwargs):
        '''
        Instantiate/add a monitoring algorithm

        Arguments:
        algClassOrObj -- the Configurable class object of the algorithm to create, or an instance
                         of the algorithm Configurable. The former is recommended.  In the former case,
                         the name argument is required.
        name -- the name of the algorithm to create. Required when passing a Configurable class object
                as algClassOrObj.  No effect if a Configurable instance is passed.
        *args, **kwargs -- additional arguments will be forwarded to the Configurable constructor if
                           a Configurable class object is passed. No effect if a Configurable instance
                           is passed.

        Returns:
        algObj -- an algorithm Configurable object
        '''
        from AthenaCommon.Configurable import Configurable
        if issubclass(algClassOrObj, Configurable):
            algObj = algClassOrObj(*args, **kwargs)
        else:
            algObj = algClassOrObj
        
        # configure these properties; users really should have no reason to override them
        algObj.Environment = self.dqflags.monManEnvironment()
        algObj.DataType = self.dqflags.monManDataType()

        self.monSeq += algObj
        return algObj

    def addGroup(self, alg, name, topPath=''):
        '''
        Add a "group" (technically, a GenericMonitoringTool instance) to an algorithm. The name given
        here can be used to retrieve the group from within the algorithm when calling the fill()
        function.  (Note this is *not* the same thing as the Monitored::Group class.)

        Arguments:
        alg -- algorithm Configurable object (e.g. one returned from addAlgorithm)
        name -- name of the group
        topPath -- directory name in the output ROOT file under which histograms will be produced

        Returns:
        tool -- a GenericMonitoringTool Configurable object. This can be used to define histograms
                associated with that group (using defineHistogram).
        '''
        from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool
        tool = GenericMonitoringTool(name)
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr(svcMgr, 'THistSvc'):
            from GaudiSvc.GaudiSvcConf import THistSvc
            svcMgr += THistSvc()
        tool.THistSvc = svcMgr.THistSvc
        tool.HistPath = self.dqflags.monManFileKey() + ('/%s' % topPath if topPath else '')
        alg.GMTools += [tool]
        return tool

    def result(self):
        '''
        This function should be called to finalize the creation of the set of monitoring algorithms.

        Returns:
        monSeq -- an AthSequencer
        '''
        return self.monSeq

def getDQTHistSvc(inputFlags):
    '''
    This function creates a THistSvc - used for the new-style job configuration
    
    Returns:
    (result, histsvc) -- a tuple of (ComponentAccumulator, THistSvc Configurable object)
    '''
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from GaudiSvc.GaudiSvcConf import THistSvc

    result = ComponentAccumulator()
    histsvc = THistSvc()
    histsvc.Output += ["%s DATAFILE='%s' OPT='RECREATE'" % (inputFlags.DQ.FileKey, 
                                                            inputFlags.Output.HISTFileName)]
    result.addService(histsvc)
    return result, histsvc
