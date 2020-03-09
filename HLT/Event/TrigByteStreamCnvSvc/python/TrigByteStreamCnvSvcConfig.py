#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamInputSvc as _TrigByteStreamInputSvc
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigByteStreamInputSvc(_TrigByteStreamInputSvc):
    def __init__(self, name='TrigByteStreamInputSvc'):
        super(TrigByteStreamInputSvc, self).__init__(name)
        self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
        self.MonTool.defineHistogram('TIME_getNext', path='EXPERT', type='TH1F',
                                     title='Time of DataCollector::getNext() calls;Time [us];N calls',
                                     xbins=100, xmin=0, xmax=50000)
        self.MonTool.defineHistogram('L1Result_NumROBs', path='EXPERT', type='TH1F',
                                     title='Number of ROBs received in L1 result;Number of ROBs;N events',
                                     xbins=100, xmin=0, xmax=100)
        self.MonTool.defineHistogram('L1Result_FullEvFragSize', path='EXPERT', type='TH1F',
                                     title='Size of L1 result FullEventFragment;Size [kB];N events',
                                     xbins=100, xmin=0, xmax=100)
        self.MonTool.defineHistogram('L1Result_SubDets', path='EXPERT', type='TH1F',
                                     title='Source of ROBs in L1 result passed to HLT;;Total number of ROBs',
                                     xbins=1, xmin=0, xmax=1)
