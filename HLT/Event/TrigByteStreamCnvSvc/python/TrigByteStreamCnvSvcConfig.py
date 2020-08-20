#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#

from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamInputSvc as _TrigByteStreamInputSvc
from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamCnvSvc as _TrigByteStreamCnvSvc
from AthenaMonitoringKernel.GenericMonitoringTool import GenericMonitoringTool

class TrigByteStreamInputSvc(_TrigByteStreamInputSvc):
    def __init__(self, name='TrigByteStreamInputSvc'):
        super(TrigByteStreamInputSvc, self).__init__(name)
        self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
        self.MonTool.defineHistogram('L1Result_NumROBs', path='EXPERT', type='TH1F',
                                     title='Number of ROBs received in L1 result;Number of ROBs;Events',
                                     xbins=100, xmin=0, xmax=100)
        self.MonTool.defineHistogram('L1Result_FullEvFragSize', path='EXPERT', type='TH1F',
                                     title='Size of L1 result FullEventFragment;Size [kB];Events',
                                     xbins=100, xmin=0, xmax=100)
        self.MonTool.defineHistogram('L1Result_SubDets', path='EXPERT', type='TH1F',
                                     title='Source of ROBs in L1 result passed to HLT;;Total number of ROBs',
                                     xbins=1, xmin=0, xmax=1)
        self.MonTool.defineHistogram('TIME_getNext', path='EXPERT', type='TH1F',
                                     title='Time of DataCollector::getNext() calls;Time [ms];Calls',
                                     xbins=400, xmin=0, xmax=200)
        self.MonTool.defineHistogram('TIME_getNext;TIME_getNext_extRange', path='EXPERT', type='TH1F',
                                     title='Time of DataCollector::getNext() calls;Time [ms];Calls',
                                     xbins=400, xmin=0, xmax=2000)
        self.MonTool.defineHistogram('getNext_LBN,getNext_noEvent;NoEventFraction', path='EXPERT', type='TProfile',
                                     title='Fraction of getNext calls returning NO_EVENT;Lumi Block;Event fraction',
                                     xbins=100, xmin=0, xmax=100, opt='kCanRebin')

class TrigByteStreamCnvSvc(_TrigByteStreamCnvSvc):
    def __init__(self, name='TrigByteStreamCnvSvc'):
        super(TrigByteStreamCnvSvc, self).__init__(name)
        self.MonTool = GenericMonitoringTool('MonTool', HistPath='HLTFramework/'+name)
        self.MonTool.defineHistogram('TIME_eventDone', path='EXPERT', type='TH1F',
                                     title='Time of DataCollector::eventDone() calls;Time [ms];Calls',
                                     xbins=400, xmin=0, xmax=2)
        self.MonTool.defineHistogram('TIME_eventDone;TIME_eventDone_extRange', path='EXPERT', type='TH1F',
                                     title='Time of DataCollector::eventDone() calls;Time [ms];Calls',
                                     xbins=400, xmin=0, xmax=200)
        self.MonTool.defineHistogram('StreamTags', path='EXPERT', type='TH1F',
                                     title='Stream Tags produced by HLT;;Events',
                                     xbins=1, xmin=0, xmax=1)
        self.MonTool.defineHistogram('StreamTagCorrA,StreamTagCorrB;StreamTagsCorr', path='EXPERT', type='TH2F',
                                     title='Stream Tags (produced by HLT) correlation',
                                     xbins=1, xmin=0, xmax=1, ybins=1, ymin=0, ymax=1)
        self.MonTool.defineHistogram('StreamTagsNum', path='EXPERT', type='TH1F',
                                     title='Number of Stream Tags produced by HLT;Number of Stream Tags;Events',
                                     xbins=20, xmin=0, xmax=20)
        self.MonTool.defineHistogram('StreamTagsType', path='EXPERT', type='TH1F',
                                     title='Type of Stream Tags produced by HLT;;Events',
                                     xbins=7, xmin=0, xmax=7,
                                     xlabels=["physics", "calibration", "express", "monitoring", "debug", "reserved", "unknown"])
        self.MonTool.defineHistogram('StreamTagsPebRobsNum', path='EXPERT', type='TH1F',
                                     title='Number of ROBs in PEB stream tags;Number of ROBs;Entries',
                                     xbins=200, xmin=0, xmax=200,
                                     cutmask="StreamTagIsPeb")
        self.MonTool.defineHistogram('StreamTagsPebSubDetsNum', path='EXPERT', type='TH1F',
                                     title='Number of SubDetectors in PEB stream tags;Number of SubDetectors;Entries',
                                     xbins=100, xmin=0, xmax=100,
                                     cutmask="StreamTagIsPeb")
        self.MonTool.defineHistogram('StreamTagsPebSubDetsFromRobList', path='EXPERT', type='TH1F',
                                     title='SubDetectors in PEB stream tags ROB list;;Entries',
                                     xbins=1, xmin=0, xmax=1)
        self.MonTool.defineHistogram('StreamTagsPebSubDetsFromSubDetList', path='EXPERT', type='TH1F',
                                     title='SubDetectors in PEB stream tags SubDetector list;;Entries',
                                     xbins=1, xmin=0, xmax=1)
        self.MonTool.defineHistogram('ResultSizeTotal', path='EXPERT', type='TH1F',
                                     title='HLT result total size (sum of all modules);Size [kB];Events',
                                     xbins=200, xmin=0, xmax=2000)
        self.MonTool.defineHistogram('ResultSizeFullEvFrag', path='EXPERT', type='TH1F',
                                     title='HLT output FullEventFragment size;Size [kB];Events',
                                     xbins=200, xmin=0, xmax=2000)
        self.MonTool.defineHistogram('ResultModuleID,ResultModuleSize;ResultSizeByModule', path='EXPERT', type='TH2F',
                                     title='HLT result size by module;Module ID;Size [kB]',
                                     xbins=10, xmin=0, xmax=10, ybins=200, ymin=0, ymax=2000)
        self.MonTool.defineHistogram('StreamTags,ResultSizeStream;ResultSizeByStream', path='EXPERT', type='TH2F',
                                     title='HLT result size by stream;;Size [kB]',
                                     xbins=1, xmin=0, xmax=1, ybins=200, ymin=0, ymax=2000)
        self.MonTool.defineHistogram('OnlineErrorCode', path='EXPERT', type='TH1F',
                                     title='Online error codes;;Events',
                                     xbins=1, xmin=0, xmax=1)
        self.MonTool.defineHistogram('TIME_monitorRawEvent', path='EXPERT', type='TH1F',
                                     title='Time of monitorRawEvent() calls;Time [us];Calls',
                                     xbins=1000, xmin=0, xmax=1000)
