#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfValidation.py
#  @brief Unittests for trfValidation.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfValidation.py 763940 2016-07-24 13:46:01Z uworlika $

import unittest

import logging
import os

msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfValidation import *

from PyJobTransforms.trfLogger import stdLogLevels
import PyJobTransforms.trfArgClasses as trfArgClasses

from PyJobTransforms.trfValidation import eventMatch, athenaLogFileReport

## Unittests for this module
class eventCheckTests(unittest.TestCase):
    
    def test_basicMatch(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 500},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
        
    def test_matchWithSkipEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 400,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_matchWithMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': None,
                                         'maxEvents': 100,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_matchWithSkipAndMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 100,
                                         'maxEvents': 100,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_matchWithSkipAndMaxEventsOvershoot(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 100},
                                         'skipEvents': 400,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
         
    def test_tooFewEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 499},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithSkipEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 399},
                                         'skipEvents': 100,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 199},
                                         'skipEvents': None,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_tooFewEventsWithSkipAndMaxEvents(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 500},
                                         'outEventDict': {'ESD': 199},
                                         'skipEvents': 100,
                                         'maxEvents': 200,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_minEff(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 991},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
 
    def test_minEffTooFew(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_badConf(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': "unknownAction"}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)

    def test_badConfEfficency(self):
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 10.0}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1000},
                                         'outEventDict': {'Hits': 989},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)
 
    def test_minEffRounding(self):
        # Test the eff * inputEvents is rounded correctly with floor()
        evmatch = eventMatch(executor=None, eventCountConf={'Evgen': {'Hits': 'minEff'}})
        evmatch.configureCheck(override={'inEventDict': {'Evgen': 1},
                                         'outEventDict': {'Hits': 0},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())
         
    def test_globDataTypeMatch(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_multiData(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'ESD': 100, 'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertTrue(evmatch.decide())

    def test_multiDataFail(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'BS': 100},
                                         'outEventDict': {'ESD': 99, 'DESD_SGLMU': 10},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)

    def test_inputGlobWithFail(self):
        evmatch = eventMatch(executor=None)
        evmatch.configureCheck(override={'inEventDict': {'NTUP_ZEE': 100},
                                         'outEventDict': {'NTUP_ZEE_MRG': 98},
                                         'skipEvents': None,
                                         'maxEvents': None,
                                         'evAccEff': 0.99})
        self.assertRaises(trfExceptions.TransformValidationException, evmatch.decide)


## Unittests for this module
class athenaLogFileReportTests(unittest.TestCase):

    def setUp(self):
        testLogExcerpt = '''
16:00:59 Py:Generate          INFO            MetaData attributes: (None)
16:00:59      +-- AthSequencer/AthOutSeq
16:00:59      +-- AthSequencer/AthRegSeq
16:00:59 MetaData: physicsComment = Dijet truth jet slice JZ2W, with the AU2 CT10 tune
16:00:59 MetaData: generatorName = Pythia8
16:00:59 MetaData: generatorTune = AU2 CT10
16:00:59 MetaData: keywords = dijet, qcd
16:00:59 MetaData: specialConfig = NONE
16:00:59 MetaData: contactPhysicist = MC group
16:00:59 srcFilePrefix = /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.0.3/AtlasSimulation/19.0.3/InstallArea/share, dirName =
16:00:59 srcFilePrefix = /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.0.3/AtlasSimulation/19.0.3/InstallArea/share, dirName =        '''

        testErrorExcerpt = '''
09:36:22 Py:Athena            INFO including file "eflowRec/eflowRecESDList.py"
09:36:22 Py:Athena            ERROR too many floobles'''

        testLogExcerptMP = '''
10:43:11 Thu Mar 30 10:43:11 CEST 2017
10:47:28 2017-03-30 10:46:30,037 AthMpEvtLoopMgr.SharedEvtQueueProvider            INFO Logs redirected in the AthenaMP event event counter PID=18296
10:47:29 2017-03-30 10:47:03,885 ToolSvc.InDetImpactPoint3dEstimator               WARNING  DeltaR and MomentumDir are not orthogonal'''

        testErrorExcerptMP = '''
10:43:11 Thu Mar 30 10:43:11 CEST 2017
10:47:28 2017-03-30 10:46:30,037 AthMpEvtLoopMgr.SharedEvtQueueProvider            INFO Logs redirected in the AthenaMP event event counter PID=18296
10:47:29 2017-03-30 10:47:03,885 ToolSvc.InDetImpactPoint3dEstimator               WARNING  DeltaR and MomentumDir are not orthogonal
10:47:28 2017-03-30 10:47:14,698 DRAW_TOPSLMUKernel                                ERROR Incompatible vectors - different length'''

        testBadAlloc = '''
22:41:12 EventCounter                                         INFO EventCounter:EventCounter::execute - seen events: 9800
22:41:14 AthenaEventLoopMgr                                   INFO   ===>>>  start processing event #5328391, run #204158 9800 events processed so far  <<<===
22:41:14 AthenaEventLoopMgr                                   INFO   ===>>>  done processing event #5328391, run #204158 9801 events processed so far  <<<===
22:41:25 src/central_freelist.cc:333] tcmalloc: allocation failed 16384
22:41:25 PerfMonSvc:    INFO [evt: 9836] cpu= 246.000 ms vmem=3990.754 Mb dvmem=   1.000 Mb alloc= +0 -0    0.000 kb  => [/evt/PerfMonSlice]
22:41:25 PerfMonSvc:    INFO [evt: 9838] cpu= 148.000 ms vmem=3994.754 Mb dvmem=   4.000 Mb alloc= +0 -0    0.000 kb  => [/evt/PerfMonSlice]
22:41:25 PerfMonSvc:    INFO [evt: 9841] cpu= 236.000 ms vmem=3996.754 Mb dvmem=   2.000 Mb alloc= +0 -0    0.000 kb  => [/evt/PerfMonSlice]
22:41:25 PerfMonSvc:    INFO [evt: 9866] cpu= 144.000 ms vmem=3998.754 Mb dvmem=   2.000 Mb alloc= +0 -0    0.000 kb  => [/evt/PerfMonSlice]
22:41:25 TrigDecisionCnvAlg                                  FATAL  Standard std::exception is caught
22:41:25 TrigDecisionCnvAlg                                  ERROR std::bad_alloc
22:41:25 ServiceManager                                      FATAL No Service factory for ExceptionSvc available.
22:41:25 TrigDecisionCnvAlg                                  ERROR ServiceLocatorHelper::service: can not locate service ExceptionSvc
22:41:25 AthAlgSeq                                           FATAL  Exception with tag=TrigDecisionCnvAlg is caught
22:41:25 AthAlgSeq                                           ERROR TrigDecisionCnvAlg    Service [ExceptionSvc] not found     StatusCode=FAILURE
22:41:25 ServiceManager                                      FATAL No Service factory for ExceptionSvc available.
22:41:25 AthAlgSeq                                           ERROR ServiceLocatorHelper::service: can not locate service ExceptionSvc
22:41:25 AthMasterSeq                                        FATAL  Exception with tag=AthAlgSeq is caught
22:41:25 AthMasterSeq                                        ERROR AthAlgSeq    Service [ExceptionSvc] not found     StatusCode=FAILURE
22:41:25 ServiceManager                                      FATAL No Service factory for ExceptionSvc available.
22:41:25 AthMasterSeq                                        ERROR ServiceLocatorHelper::service: can not locate service ExceptionSvc
22:41:25 Traceback (most recent call last):
22:41:25   File "/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/jobOptions/AthenaCommon/runbatch.py", line 11, in <module>
22:41:25     theApp.run()     # runs until theApp.EvtMax events reached
22:41:25   File "/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/python/AthenaCommon/AppMgr.py", line 627, in run
22:41:25     sc = self.getHandle()._evtpro.executeRun( nEvt )
22:41:25 RuntimeError: (file "", line 0) Service [ExceptionSvc] not found (C++ exception)
22:41:25 DetectorStore                                        INFO Stop DetectorStore
22:41:25 DsoDb **error**: could not load library [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libEventInfoDict.so] (libEventInfoDict.so: failed to map segment from shared object: Cannot allocate memory)
22:41:25 AthenaPoolConverter                                 ERROR There is no correct dictionary for class (type/key) EventStreamInfo_p3/StreamAOD
22:41:25 AthenaPoolConverter                                 ERROR CreateRep failed, key = StreamAOD
22:41:25 StreamAOD.StreamAODTool                             ERROR Could not create Rep for DataObject (clid/key):167728019 StreamAOD
22:41:25 AthenaPoolCnvSvc                                     INFO problem adding converter for CLID [243004407]
22:41:25 AthenaPoolCnvSvc                                     INFO problem adding converter for CLID [243004407]
22:41:25 AthenaPoolCnvSvc                                     INFO No converter for object SG::DataBucket<xAOD::EventFormat_v1>  CLID= 243004407
22:41:25 StreamAOD.StreamAODTool                             ERROR Could not create Rep for DataObject (clid/key):243004407 EventFormat
22:41:25 src/central_freelist.cc:333] tcmalloc: allocation failed 16384
22:41:25 AthenaPoolConverter                                 ERROR Failed to create persistent object: std::bad_alloc
22:41:25 AthenaPoolConverter                                 ERROR CreateRep failed, key = /LAR/LArAffectedRegionInfo
22:41:25 StreamAOD.StreamAODTool                             ERROR Could not create Rep for DataObject (clid/key):1316383046 /LAR/LArAffectedRegionInfo
22:41:25 src/central_freelist.cc:333] tcmalloc: allocation failed 16384
22:41:25 AthenaPoolConverter                                 ERROR Failed to create persistent object: std::bad_alloc
22:41:25 AthenaPoolConverter                                 ERROR CreateRep failed, key = /TRIGGER/HLT/Menu
22:41:25 StreamAOD.StreamAODTool                             ERROR Could not create Rep for DataObject (clid/key):1316383046 /TRIGGER/HLT/Menu
22:41:25 src/central_freelist.cc:333] tcmalloc: allocation failed 16384
22:41:25 AthenaPoolConverter                                 ERROR Failed to create persistent object: std::bad_alloc
22:41:25 AthenaPoolConverter                                 ERROR CreateRep failed, key = /TRIGGER/HLT/Prescales
22:41:25 StreamAOD.StreamAODTool                             ERROR Could not create Rep for DataObject (clid/key):1316383046 /TRIGGER/HLT/Prescales
22:41:25 AthenaPoolCnvSvc                                     INFO problem adding converter for CLID [243004407]
22:41:25 AthenaPoolCnvSvc                                     INFO problem adding converter for CLID [243004407]
22:41:25 AthenaPoolCnvSvc                                     INFO No converter for object SG::DataBucket<xAOD::EventFormat_v1>  CLID= 243004407
22:41:25 StreamAOD                                           FATAL streamObjects failed.
22:41:25 -------------------------------------------------------------------------------------
22:41:25 Core dump from CoreDumpSvc on td046.pic.es at Tue Dec  9 22:41:25 2014
22:41:25 
22:41:25 Caught signal 11(Segmentation fault). Details:
22:41:25   errno = 0, code = 1 (address not mapped to object)
22:41:25   pid   = 24, uid = 0
22:41:25   value = (0, 0)
22:41:25   vmem = 4000 MB
22:41:25   rss  = 3476.8 MB
22:41:25   total-ram = 64386.6 MB
22:41:25   free-ram  = 9165.63 MB
22:41:25   buffer-ram= 67.5664 MB
22:41:25   total-swap= 64386 MB
22:41:25   free-swap = 63581.7 MB
22:41:25   addr  = 0x18
22:41:25 
22:41:25 Event counter: 9868
22:41:25 EventID: [Run,Evt,Lumi,Time,BunchCross,DetMask] = [204158,5297050,354,1338437641:983625960,699,0xbffffffffff7]
22:41:25 Last incident: AthenaEventLoopMgr:BeginEvent
22:41:25 Current algorithm: <NONE>
22:41:25 Algorithm stack: <EMPTY>
22:41:25 -------------------------------------------------------------------------------------
22:41:25 | AtlasBaseDir : /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1 |
22:41:25 | AtlasVersion :                                                           19.1.1.9 |
22:41:25 | CMTCONFIG    :                                              x86_64-slc6-gcc47-opt |
22:41:25 -------------------------------------------------------------------------------------
22:41:25  Note: to see line numbers in below stacktrace you might consider running following :
22:41:25   atlasAddress2Line --file <logfile>
22:41:25 -------------------------------------------------------------------------------------
22:41:25 
22:41:25 (pid=6608 ppid=6607) received fatal signal 11 (Segmentation fault)
22:41:25 signal context:
22:41:25   signo  = 11, errno = 0, code = 1 (address not mapped to object)
22:41:25   pid    = 24, uid = 0
22:41:25   value  = (0, (nil))
22:41:25   addr   = 0x18
22:41:25   stack  = (2, 0, (nil))
22:41:25 
22:41:25   ffffffff ffffffff 00000000 00000000 00000000 00000000 46020000 00000000
22:41:25   f00726b0 ff7f0000 400826b0 ff7f0000 803f2639 00000000 00000000 00000000
22:41:25   00000000 00000000 18010000 00000000 2052081a 697f0000 803f2639 00000000
22:41:25   18000000 00000000 00000000 00000000 e73bf01f 697f0000 300726b0 ff7f0000
22:41:25   ff15081a 697f0000 06020100 00000000 33000000 00000000 06000000 00000000
22:41:25   0e000000 00000000 00000000 00000000 18000000 00000000 400326b0 ff7f0000
22:41:25   00000000 00000000 100726b0 ff7f0000 020b091a 697f0000 80a28d09 00000000
22:41:25   00000000 ff7f0000 18558a09 00000000 00000000 00000000 00000000 00000000
22:41:25 
22:41:25 stack trace:
22:41:26  0x7f69161eecd6 Athena::DebugAids::stacktrace(int) ??:0   + 0x46 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libCxxUtils.so D[0x3ccd6]]
22:41:26  0x7f69161ea1f2 Athena::Signal::fatalDump(int, siginfo*, void*) ??:0   + 0x142 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libCxxUtils.so D[0x381f2]]
22:41:26  0x7f69161ea4ed Athena::Signal::fatal(int, siginfo*, void*) ??:0   + 0x17d [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libCxxUtils.so D[0x384ed]]
22:41:26  0x7f6915c7cd1a CoreDumpSvcHandler::action(int, siginfo*, void*) ??:0   + 0xfa [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libAthenaServices.so D[0x253d1a]]
22:41:26  0x322240f710 ???  + 0x322240f710 [/lib64/libpthread.so.0 D[0xf710]]
22:41:27  0x7f691a0815ff ROOT::Cintex::Allocate_stub_function(ROOT::Cintex::StubContext_t*, int (*)(ROOT::Cintex::StubContext_t*, G__value*, char const*, G__param*, int)) ??:0   + 0x2f [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCintex.so D[0x115ff]]
22:41:27  0x7f691a080e5a ROOT::Cintex::CINTFunctionBuilder::Setup(Reflex::Member const&) ??:0   + 0x59a [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCintex.so D[0x10e5a]]
22:41:27  0x7f691a07e3bf ROOT::Cintex::CINTClassBuilder::Setup_memfunc() ??:0   + 0xbf [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCintex.so D[0xe3bf]]
22:41:27  0x7f691a07e4d2 ROOT::Cintex::CINTClassBuilder::Setup_memfunc_with_context(void*) ??:0   + 0x22 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCintex.so D[0xe4d2]]
22:41:27  0x7f691b85cfeb G__incsetup_memfunc ??:0   + 0x11b [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCint.so D[0x114feb]]
22:41:28  0x7f691b79c727 Cint::G__ClassInfo::HasMethod(char const*) ??:0   + 0x47 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCint.so D[0x54727]]
22:41:28  0x7f691c55bc5f TCint::ClassInfo_HasMethod(void*, char const*) const ??:0   + 0xf [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x216c5f]]
22:41:28  0x7f691c56c445 TClass::Property() const ??:0   + 0x125 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x227445]]
22:41:28  0x7f691c56d3f9 TClass::IsForeign() const ??:0   + 0x29 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x2283f9]]
22:41:28  0x7f691c56dbe0 TClass::PostLoadCheck() ??:0   + 0xe0 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x228be0]]
22:41:29  0x7f691c5083e3 TROOT::LoadClass(char const*, bool) const ??:0   + 0x73 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x1c33e3]]
22:41:29  0x7f691c572238 TClass::GetClass(char const*, bool, bool) ??:0   + 0x4e8 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x22d238]]
22:41:29  0x7f691d25c22a TStreamerInfo::Build() ??:0   + 0xf7a [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libRIO.so D[0x10322a]]
22:41:29  0x7f691c56bbe2 TClass::GetStreamerInfo(int) const ??:0   + 0x292 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCore.so D[0x226be2]]
22:41:29  0x7f691d363652 int TStreamerInfo::WriteBufferAux<char**>(TBuffer&, char** const&, int, int, int, int) ??:0   + 0x4742 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libRIO.so D[0x20a652]]
22:41:30  0x7f691d262ddb TStreamerInfoActions::GenericWriteAction(TBuffer&, void*, TStreamerInfoActions::TConfiguration const*) ??:0   + 0x3b [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libRIO.so D[0x109ddb]]
22:41:30  0x7f691d20b4d5 TBufferFile::ApplySequence(TStreamerInfoActions::TActionSequence const&, void*) ??:0   + 0x75 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libRIO.so D[0xb24d5]]
22:41:30  0x7f691cdafef3 TBranch::Fill() ??:0   + 0x193 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libTree.so D[0x94ef3]]
22:41:30  0x7f691cdb72c8 TBranchElement::Fill() ??:0   + 0x2d8 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libTree.so D[0x9c2c8]]
22:41:30  0x7f69100518d7 pool::RootTreeContainer::writeObject(pool::DbContainerImp::_Transaction&) ??:0   + 0x147 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libRootStorageSvc.so D[0x2e8d7]]
22:41:31  0x7f69116f8d61 pool::DbContainerImp::commitTransaction(pool::DbTransaction&) ??:0   + 0x221 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x2ed61]]
22:41:31  0x7f69116f8470 pool::DbContainerImp::transAct(pool::DbTransaction&) ??:0   + 0xa0 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x2e470]]
22:41:31  0x7f691170575e pool::DbContainerObj::transAct(pool::DbTransaction&) ??:0   + 0x3e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x3b75e]]
22:41:31  0x7f6911715448 pool::DbDatabaseObj::transAct(pool::DbTransaction&) ??:0   + 0xd8 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x4b448]]
22:41:31  0x7f6911710d73 pool::DbDatabase::transAct(pool::DbTransaction&) ??:0   + 0x73 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x46d73]]
22:41:32  0x7f69116f1891 pool::DbStorageSvc::endTransaction(pool::Transaction*, pool::Transaction::Action) ??:0   + 0xc1 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libStorageSvc.so D[0x27891]]
22:41:32  0x7f690c864047 pool::PersistencySvc::DatabaseHandler::commitAndHoldTransaction() ??:0   + 0x27 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libPersistencySvc.so D[0x13047]]
22:41:32  0x7f690c86f13d pool::PersistencySvc::GlobalTransaction::commitAndHold() ??:0   + 0x13d [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libPersistencySvc.so D[0x1e13d]]
22:41:32  0x7f690c91c90d PoolSvc::commitAndHold(unsigned long) const ??:0   + 0x7d [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libPoolSvc.so D[0x3790d]]
22:41:32  0x7f69117f2450 AthenaPoolCnvSvc::commitOutput(std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&, bool) ??:0   + 0x270 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libAthenaPoolCnvSvcLib.so D[0x58450]]
22:41:33  0x7f68f881a373 AthenaPoolOutputStreamTool::commitOutput() ??:0   + 0x153 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libOutputStreamAthenaPool.so D[0x48373]]
22:41:33  0x7f6915c51535 AthenaOutputStream::write() ??:0   + 0x365 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libAthenaServices.so D[0x228535]]
22:41:33  0x7f6915c4e98d AthenaOutputStream::handle(Incident const&) ??:0   + 0x61d [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libAthenaServices.so D[0x22598d]]
22:41:33  0x7f6916767ab6 IncidentSvc::i_fireIncident(Incident const&, std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) ??:0   + 0x136 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiCoreSvc.so D[0x1bdab6]]
22:41:33  0x7f691676806d IncidentSvc::fireIncident(Incident const&) ??:0   + 0x5d [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiCoreSvc.so D[0x1be06d]]
22:41:33  0x7f6915c01c98 MetaDataSvc::stop() ??:0   + 0x248 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasCore/19.1.1/InstallArea/x86_64-slc6-gcc47-opt/lib/libAthenaServices.so D[0x1d8c98]]
22:41:34  0x7f69197785e1 Service::sysStop() ??:0   + 0x81 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiKernel.so D[0x2c55e1]]
22:41:34  0x7f6916733d48 ServiceManager::stop() ??:0   + 0x298 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiCoreSvc.so D[0x189d48]]
22:41:34  0x7f69166ffe70 ApplicationMgr::stop() ??:0   + 0x260 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiCoreSvc.so D[0x155e70]]
22:41:34  0x7f691705cbf2 (anonymous namespace)::method_4965(void*, void*, std::vector<void*, std::allocator<void*> > const&, void*) ??:0   + 0x7f691705cbf2 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/GAUDI/v24r1p2-lcg67b/InstallArea/x86_64-slc6-gcc47-opt/lib/libGaudiKernelDict.so D[0xe4bf2]]
22:41:35  0x7f691a085621 ROOT::Cintex::Method_stub_with_context(ROOT::Cintex::StubContext_t*, G__value*, char const*, G__param*, int) ??:0   + 0x141 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCintex.so D[0x15621]]
22:41:35  0x7f691b79ae6f Cint::G__CallFunc::Execute(void*) ??:0   + 0xcf [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/AtlasProduction/19.1.1.9/InstallArea/x86_64-slc6-gcc47-opt/lib/libCint.so D[0x52e6f]]
22:41:35  0x7f691d6a6c73 PyROOT::TRootObjectByValueExecutor::Execute(Cint::G__CallFunc*, void*, bool) ??:0   + 0x23 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/atlas/offline/external/root-snapshot/atlas-5.34.13p1/x86_64-slc6-gcc47-opt/lib/libPyROOT.so D[0x48c73]]
22:41:35  0x7f691d6ade37 PyROOT::TMethodHolder<PyROOT::TScopeAdapter, PyROOT::TMemberAdapter>::CallSafe(void*, bool) ??:0   + 0x67 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/atlas/offline/external/root-snapshot/atlas-5.34.13p1/x86_64-slc6-gcc47-opt/lib/libPyROOT.so D[0x4fe37]]
22:41:35  0x7f691d6adff9 PyROOT::TMethodHolder<PyROOT::TScopeAdapter, PyROOT::TMemberAdapter>::Execute(void*, bool) ??:0   + 0x89 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/atlas/offline/external/root-snapshot/atlas-5.34.13p1/x86_64-slc6-gcc47-opt/lib/libPyROOT.so D[0x4fff9]]
22:41:36  0x7f691d6ad214 PyROOT::TMethodHolder<PyROOT::TScopeAdapter, PyROOT::TMemberAdapter>::operator()(PyROOT::ObjectProxy*, _object*, _object*, long, bool) ??:0   + 0x124 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/atlas/offline/external/root-snapshot/atlas-5.34.13p1/x86_64-slc6-gcc47-opt/lib/libPyROOT.so D[0x4f214]]
22:41:36  0x7f691d6b17b0 PyROOT::(anonymous namespace)::mp_call(PyROOT::MethodProxy*, _object*, _object*) ??:0   + 0x7f691d6b17b0 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/atlas/offline/external/root-snapshot/atlas-5.34.13p1/x86_64-slc6-gcc47-opt/lib/libPyROOT.so D[0x537b0]]
22:41:36  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:36  0x00495311 ext_do_call inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:2705 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4334   + 0x1291 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x495311]]
22:41:36  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:37  0x0049867d fast_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4042 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4117   + 0x45fd [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49867d]]
22:41:37  0x00499e79 fast_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4042 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4108   + 0x5df9 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x499e79]]
22:41:37  0x00499e79 fast_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4042 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4108   + 0x5df9 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x499e79]]
22:41:37  0x00499e79 fast_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4042 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4108   + 0x5df9 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x499e79]]
22:41:37  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:38  0x0049867d fast_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4042 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4117   + 0x45fd [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49867d]]
22:41:38  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:38  0x0049ab12 PyEval_EvalCode /build/vdiez/Python-2.7.3/Python/ceval.c:673   + 0x32 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49ab12]]
22:41:38  0x004bc0da run_mod inlined at /build/vdiez/Python-2.7.3/Python/pythonrun.c:1339 in PyRun_FileExFlags /build/vdiez/Python-2.7.3/Python/pythonrun.c:1354   + 0xaa [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4bc0da]]
22:41:38  0x004915a8 builtin_execfile /build/vdiez/Python-2.7.3/Python/bltinmodule.c:795   + 0x4915a8 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4915a8]]
22:41:39  0x00499b82 call_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:2666 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4021   + 0x5b02 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x499b82]]
22:41:39  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:39  0x004fcc36 function_call /build/vdiez/Python-2.7.3/Objects/funcobject.c:536   + 0x4fcc36 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4fcc36]]
22:41:39  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:39  0x00423ae0 instancemethod_call /build/vdiez/Python-2.7.3/Objects/classobject.c:2579   + 0x423ae0 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x423ae0]]
22:41:40  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:40  0x0046cbfc slot_tp_call /build/vdiez/Python-2.7.3/Objects/typeobject.c:5405   + 0x46cbfc [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x46cbfc]]
22:41:40  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:40  0x00495c79 do_call inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4044 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4239   + 0x1bf9 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x495c79]]
22:41:40  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:40  0x0049ab12 PyEval_EvalCode /build/vdiez/Python-2.7.3/Python/ceval.c:673   + 0x32 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49ab12]]
22:41:41  0x004bc0da run_mod inlined at /build/vdiez/Python-2.7.3/Python/pythonrun.c:1339 in PyRun_FileExFlags /build/vdiez/Python-2.7.3/Python/pythonrun.c:1354   + 0xaa [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4bc0da]]
22:41:41  0x004915a8 builtin_execfile /build/vdiez/Python-2.7.3/Python/bltinmodule.c:795   + 0x4915a8 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4915a8]]
22:41:41  0x00499b82 call_function inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:2666 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4021   + 0x5b02 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x499b82]]
22:41:41  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:42  0x004fcc36 function_call /build/vdiez/Python-2.7.3/Objects/funcobject.c:536   + 0x4fcc36 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4fcc36]]
22:41:42  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:42  0x00423ae0 instancemethod_call /build/vdiez/Python-2.7.3/Objects/classobject.c:2579   + 0x423ae0 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x423ae0]]
22:41:42  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:42  0x0046cbfc slot_tp_call /build/vdiez/Python-2.7.3/Objects/typeobject.c:5405   + 0x46cbfc [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x46cbfc]]
22:41:43  0x0041f5ee PyObject_Call /build/vdiez/Python-2.7.3/Objects/abstract.c:2529   + 0x4e [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x41f5ee]]
22:41:43  0x00495c79 do_call inlined at /build/vdiez/Python-2.7.3/Python/ceval.c:4044 in PyEval_EvalFrameEx /build/vdiez/Python-2.7.3/Python/ceval.c:4239   + 0x1bf9 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x495c79]]
22:41:43  0x0049a9d8 PyEval_EvalCodeEx /build/vdiez/Python-2.7.3/Python/ceval.c:3253   + 0x848 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49a9d8]]
22:41:43  0x0049ab12 PyEval_EvalCode /build/vdiez/Python-2.7.3/Python/ceval.c:673   + 0x32 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x49ab12]]
22:41:43  0x004bc0da run_mod inlined at /build/vdiez/Python-2.7.3/Python/pythonrun.c:1339 in PyRun_FileExFlags /build/vdiez/Python-2.7.3/Python/pythonrun.c:1354   + 0xaa [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4bc0da]]
22:41:44  0x004bcb0f PyRun_SimpleFileExFlags /build/vdiez/Python-2.7.3/Python/pythonrun.c:943   + 0xdf [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4bcb0f]]
22:41:44  0x004168a2 Py_Main /build/vdiez/Python-2.7.3/Modules/main.c:639   + 0xc22 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x4168a2]]
22:41:44  0x322201ed5d __libc_start_main  + 0xfd [/lib64/libc.so.6 D[0x1ed5d]]
22:41:44  0x00415941 _start /build/vdiez/Python-2.7.3/Python/Python-ast.c:6576   + 0x415941 [/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/lcg/external/Python/2.7.3/x86_64-slc6-gcc47-opt/bin/python D[0x415941]]
22:41:44 ./runwrapper.AODMerge.sh: line 10:  6608 Segmentation fault      athena.py --preloadlib=/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/IntelSoftware/linux/x86_64/xe2013/composer_xe_2013.3.163/compiler/lib/intel64/libintlc.so.5:/cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc47-opt/19.1.1/sw/IntelSoftware/linux/x86_64/xe2013/composer_xe_2013.3.163/compiler/lib/intel64/libimf.so --drop-and-reload runargs.AODMerge.py RecJobTransforms/skeleton.MergePool_tf.py
'''

        testSuppressExcerpt = ""
        for i in range(100):
            testSuppressExcerpt += "12:13:14 DummyAlg ERROR An error message number {0}\n".format(i)

        testCoreDumpCurAlg = '''
18:16:05 ToolSvc.CombinedMuonTrackBuilder                  WARNING standaloneRefit:: no outer material
18:16:06 -------------------------------------------------------------------------------------
18:16:06 Core dump from CoreDumpSvc on wn029.datagrid.cea.fr at Fri Mar 13 18:16:06 2015
18:16:06 
18:16:06 Caught signal 11(Segmentation fault). Details:
18:16:06   errno = 0, code = 128 (kernel)
18:16:06   pid   = 0, uid = 0
18:16:06   value = (319308181, 0xc093e25f13084195)
18:16:06   vmem = 3197.72 MB
18:16:06   rss  = 2365.55 MB
18:16:06   total-ram = 48257.9 MB
18:16:06   free-ram  = 303.656 MB
18:16:06   buffer-ram= 73.25 MB
18:16:06   total-swap= 73728 MB
18:16:06   free-swap = 73686.9 MB
18:16:06   addr  = 0
18:16:06 
18:16:06 Event counter: 41
18:16:06 EventID: [Run,Evt,Lumi,Time,BunchCross,DetMask] = [204158,70319934,598,1338451622:532780295,2772,0xbffffffffff7,0x0,0x0]
18:16:06 Last incident: AthenaEventLoopMgr:BeginEvent
18:16:06 Current algorithm: MuonCreatorAlg
18:16:06 Algorithm stack:
18:16:06    AthMasterSeq
18:16:06    AthAlgSeq
18:16:06    MuonCreatorAlg
18:16:40 There was a crash.
18:16:40 This is the entire stack trace of all threads:
18:16:40 ===========================================================
18:16:40 
18:16:40 Thread 2 (Thread 0x2ab56efce740 (LWP 9827)):
18:16:40 #0  0x00002ab55a77420a in timer_helper_thread () from /lib64/librt.so.1
18:16:40 #1  0x00002ab550498aa1 in start_thread () from /lib64/libpthread.so.0
18:16:40 #2  0x00002ab550e2293d in clone () from /lib64/libc.so.6
18:16:40 
18:16:40 Thread 1 (Thread 0x2ab551811d00 (LWP 9638)):
18:16:40 #0  0x00002ab550de669d in waitpid () from /lib64/libc.so.6
18:16:40 #1  0x00002ab550d78609 in do_system () from /lib64/libc.so.6
18:16:40 #2  0x00002ab550d78940 in system () from /lib64/libc.so.6
18:16:40 #3  0x00002ab55717bc5a in TUnixSystem::StackTrace() () from /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/LCGCMT/LCGCMT_81c/InstallArea/x86_64-slc6-gcc49-opt/lib/libCore.so
18:16:40 #4  0x00002ab55717dc2c in TUnixSystem::DispatchSignals(ESignals) () from /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/LCGCMT/LCGCMT_81c/InstallArea/x86_64-slc6-gcc49-opt/lib/libCore.so
18:16:40 #5  0x00002ab56e606982 in CoreDumpSvcHandler::action(int, siginfo*, void*) () from /cvmfs/atlas.cern.ch/repo/sw/software/x86_64-slc6-gcc49-opt/20.7.5/AtlasCore/20.7.5/InstallArea/x86_64-slc6-gcc49-opt/lib/libAthenaServices.so
18:16:40 #6  <signal handler called>
18:16:40 #7  0x00002ab56a40110f in G4VoxelNavigation::ComputeStep (this=this
18:16:40 entry=0x21c80ea8, localPoint=..., localDirection=..., currentProposedStepLength=currentProposedStepLength
18:16:40 entry=230844.58122861374, newSafety=
18:16:40 0x7ffc8fbcd1b8: -1, history=..., validExitNormal=
18:16:40 0x21c80d89: false, exitNormal=..., exiting=
18:16:40 0x21c80d5f: false, entering=
18:16:40 0x21c80d5e: false, pBlockedPhysical=0x21c80d60, blockedReplicaNo=
18:16:40 0x21c80d68: 0) at src/G4VoxelNavigation.cc:152
18:16:40 #8  0x00002ab56a3e0f0c in G4Navigator::ComputeStep (this=0x21c80d00, pGlobalpoint=..., pDirection=..., pCurrentProposedStepLength=230844.58122861374, pNewSafety=
18:16:40 0x7ffc8fbcd1b8: -1) at src/G4Navigator.cc:797
        '''
            
        testCoreDumpNoCurAlg = '''
18:16:05 ToolSvc.CombinedMuonTrackBuilder                  WARNING standaloneRefit:: no outer material
18:16:06 -------------------------------------------------------------------------------------
18:16:06 Core dump from CoreDumpSvc on wn029.datagrid.cea.fr at Fri Mar 13 18:16:06 2015
18:16:06 -------------------------------------------------------------------------------------
18:16:06 Caught signal 11(Segmentation fault). Details:
        '''
        
        testMissedBadAlloc = '''
10:18:13 StreamESD                                            INFO Records written: 3642
10:18:13 /cvmfs/atlas.cern.ch/repo/sw/database/DBRelease/28.1.1/poolcond/cond09_mc.000079.gen.COND/cond09_mc.000079.gen.COND._0002.pool.root Info Database being retired...
10:18:13 Domain[ROOT_All] Info ->  Deaccess DbDatabase   READ      [ROOT_All] BC292F26-AE73-9041-BF5C-BCE6C5C651EC
10:18:13 Domain[ROOT_All] Info >   Deaccess DbDomain     READ      [ROOT_All]
10:18:13 tmp.ESD Info Database being retired...
10:18:14 terminate called after throwing an instance of 'std::bad_alloc'
10:18:14   what():  std::bad_alloc
10:18:14
10:18:14 (pid=29034 ppid=29033) received fatal signal 6 (Aborted)
10:18:14 signal context:
10:18:14   signo  = 6, errno = 0, code = -6 (*unknown reason*)
10:18:14   pid    = 29034, uid = 41000
10:18:14   value  = (0, (nil))
10:18:14   stack  = (2, 0, (nil))'''

        testDbMonitor = '''
16:32:37 IOVDbSvc                                             INFO Total payload read from COOL: 123 bytes in ((    4.56 ))s
16:32:39 IOVDbSvc                                             INFO Total payload read from COOL: 456 bytes in ((    7.89 ))s'''

        with open('file1', 'w') as f1:
            print >> f1, 'This is test file 1 w/o meaning'
        with open('file2', 'w') as f2:
            print >> f2, testLogExcerpt
        with open('file3', 'w') as f3:
            print >> f3, testLogExcerpt
            print >> f3, testErrorExcerpt
        with open('file4', 'w') as f4:
            print >> f4, testBadAlloc
        with open('file5', 'w') as f5:
            print >> f5, testSuppressExcerpt
        with open('file6', 'w') as f6:
            print >> f6, testCoreDumpCurAlg
        with open('file7', 'w') as f7:
            print >> f7, testCoreDumpNoCurAlg
        with open('file8', 'w') as f8:
            print >> f8, testMissedBadAlloc
        with open('file9', 'w') as f9:
            print >> f9, testDbMonitor
        with open('file10', 'w') as f10:
            print >> f10, testLogExcerptMP
            print >> f10, testErrorExcerptMP

        self.myFileReport1 = athenaLogFileReport('file1')
        self.myFileReport2 = athenaLogFileReport('file2')
        self.myFileReport3 = athenaLogFileReport('file3')
        self.myFileReport4 = athenaLogFileReport('file4')
        self.myFileReport5 = athenaLogFileReport('file5')
        self.myFileReport6 = athenaLogFileReport('file6')
        self.myFileReport7 = athenaLogFileReport('file7')
        self.myFileReport8 = athenaLogFileReport('file8')
        self.myFileReport9 = athenaLogFileReport('file9')
        self.myFileReport10 = athenaLogFileReport('file10')

    def tearDown(self):
        for f in 'file1', 'file2', 'file3', 'file4', 'file5', 'file6', 'file7', 'file8', 'file9', 'file10',\
                 'logWithSubstepNameSerial', 'logWithSubstepNameMP':
            try:
                os.unlink(f)
            except OSError:
                pass

    def test_emptyScanLogMetaData(self):
        self.assertEqual(self.myFileReport1._metaData, {})

    def test_gatherScanLogMetaData(self):
        resultDict = {'physicsComment': 'Dijet truth jet slice JZ2W, with the AU2 CT10 tune',
                      'generatorName': 'Pythia8',
                      'generatorTune': 'AU2 CT10',
                      'keywords': 'dijet, qcd',
                      'specialConfig': 'NONE',
                      'contactPhysicist': 'MC group'
                      }
        self.assertEqual(self.myFileReport2._metaData, resultDict)
        
    def test_logscanError(self):
        self.assertEqual(self.myFileReport3.worstError(), {'level': 'ERROR', 'nLevel': logging.ERROR, 
                                                           'firstError': {'count': 1, 'firstLine': 15,
                                                                          'message': 'Py:Athena            ERROR too many floobles'},})

    def test_logscanErrorMP(self):
        self.assertEqual(self.myFileReport10.worstError(), {'level': 'ERROR', 'nLevel': logging.ERROR,
                                                           'firstError': {'count': 1, 'firstLine': 9,
                                                                          'message': 'DRAW_TOPSLMUKernel                                ERROR Incompatible vectors - different length'},})

    def test_logscanErrorWithSubstepNameSerial(self):
        testLogERRORwithSubstepNameSerial = '''
RAWtoALL 22:21:40 ToolSvc.TileROD_Decoder   INFO    ROD 540007 has unexpected data size
RAWtoALL 22:21:40 ToolSvc.TileROD_Decoder   WARNING Frag has unexpected size ignoring 6 words to end of ROD frag
RAWtoALL 22:21:40 ToolSvc.TileROD_Decoder   ERROR   ROB 540007 ROD 540007 has unexpected data size
RAWtoALL 22:21:40 AlgErrorAuditor           ERROR   Illegal Return Code: Algorithm ManagedAthenaTileMon reported an \
ERROR, but returned a StatusCode "SUCCESS"'''

        logFileName = 'logWithSubstepNameSerial'
        with open(logFileName, 'w') as logFile:
            print >> logFile, testLogERRORwithSubstepNameSerial

        logFileReportSerial = athenaLogFileReport(logfile=logFileName, substepName='RAWtoALL')
        expectedError = dict(level='ERROR', nLevel=logging.ERROR,
                             firstError=dict(count=1, firstLine=4, message='ToolSvc.TileROD_Decoder   ERROR   ROB 54'
                                                                           '0007 ROD 540007 has unexpected data size'))
        self.assertEqual(logFileReportSerial.worstError(), expectedError)

    def test_logscanErrorWithSubstepNameMP(self):
        testLogERRORwithSubstepNameMP = '''
RAWtoALL 22:21:40 2017-07-13 19:23:38,171 ToolSvc.TileROD_Decoder   INFO    ROD 540007 has unexpected data size
RAWtoALL 22:21:40 2017-07-13 19:23:38,171 ToolSvc.TileROD_Decoder   WARNING Frag 0x40f has unexpected size ignoring \
6 words till the end of ROD frag
RAWtoALL 22:21:40 2017-07-13 19:23:38,171 ToolSvc.TileROD_Decoder   ERROR   ROB 540007 ROD 540007 has unexpected \
data size
RAWtoALL 22:21:40 2017-07-13 19:23:38,184 AlgErrorAuditor           ERROR   Illegal Return Code: Algorithm \
ManagedAthenaTileMon reported an ERROR, but returned a StatusCode "SUCCESS"'''

        logFileName = 'logWithSubstepNameMP'
        with open(logFileName, 'w') as logFile:
            print >> logFile, testLogERRORwithSubstepNameMP

        logFileReportMP = athenaLogFileReport(logfile=logFileName, substepName='RAWtoALL')
        expectedError = dict(level='ERROR', nLevel=logging.ERROR,
                             firstError=dict(count=1, firstLine=4, message='ToolSvc.TileROD_Decoder   ERROR   ROB '
                                                                           '540007 ROD 540007 has unexpected data size'))
        self.assertEqual(logFileReportMP.worstError(), expectedError)

    def test_badAlloc(self):
        self.assertEqual(self.myFileReport4.worstError(), {'level': 'CATASTROPHE', 'nLevel': stdLogLevels['CATASTROPHE'],
                                                           'firstError': {'count': 1, 'firstLine': 11,
                                                                          'message': 'TrigDecisionCnvAlg                                  ERROR std::bad_alloc'}})
        self.assertEqual(self.myFileReport8.worstError(), {'level': 'CATASTROPHE', 'nLevel': stdLogLevels['CATASTROPHE'],
                                                           'firstError': {'count': 1, 'firstLine': 7,
                                                                          'message': 'terminate after \'std::bad_alloc\'.'}})

    def test_suppress(self):
        print self.myFileReport5
        self.assertEqual(self.myFileReport5._levelCounter['ERROR'], 100)
        self.assertEqual(len(self.myFileReport5._errorDetails['ERROR']), 10)
        pass

    def test_coreDumpCurAlg(self):
        self.assertEqual(self.myFileReport6.worstError(), {'level': 'FATAL', 'nLevel': logging.FATAL,
                                                           'firstError': {'count': 1, 'firstLine': 4,
                                                               'message': 'Segmentation fault: Event counter: 41; Run: 204158; Evt: 70319934; Current algorithm: MuonCreatorAlg; Current Function: G4VoxelNavigation::ComputeStep'}})
        self.assertEqual(self.myFileReport7.worstError(), {'level': 'FATAL', 'nLevel': logging.FATAL,
                                                           'firstError': {'count': 1, 'firstLine': 4,
                                                                          'message': 'Segmentation fault: Event counter: unknown; Run: unknown; Evt: unknown; Current algorithm: unknown; Current Function: unknown'}})

    def test_dbMonitor(self):
        print self.myFileReport9 
        self.assertEqual(self.myFileReport9.dbMonitor(), {'bytes': 579, 'time': 12.45})
        self.assertEqual(self.myFileReport8.dbMonitor(), None)

    ## TODO
    # Special tests for G4 errors and core dumps

## Unittests for this module
if __name__ == '__main__':
    unittest.main()
