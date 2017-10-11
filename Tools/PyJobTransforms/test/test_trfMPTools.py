#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package test_trfMPTools.py
#  @brief Unittests for trfMPTools.py
#  @author graeme.andrew.stewart@cern.ch
#  @version $Id: test_trfMPTools.py 772406 2016-09-09 12:10:12Z mavogel $

import os
import subprocess
import unittest

import logging
msg = logging.getLogger(__name__)

# Allowable to import * from the package for which we are the test suite
from PyJobTransforms.trfMPTools import *
from PyJobTransforms.trfArgClasses import argList, argSubstepList, argFile

import PyJobTransforms.trfExceptions as trfExceptions


## Unit tests
class AthenaMPProcTests(unittest.TestCase):
    def setUp(self):
        os.environ.pop("ATHENA_PROC_NUMBER", "")
    
    def test_noMP(self):
        self.assertEqual(detectAthenaMPProcs(), 0)
        
    def test_noMPwithArgdict(self):
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['some', 'random', 'values'])}
        self.assertEqual(detectAthenaMPProcs(argdict), 0)
        
    def test_MPfromEnv(self):
        os.environ["ATHENA_PROC_NUMBER"] = "8"
        self.assertEqual(detectAthenaMPProcs(), 8)
        
    def test_MPfromEnvEmpty(self):
        os.environ["ATHENA_PROC_NUMBER"] = "0"
        self.assertEqual(detectAthenaMPProcs(), 0)
        
    def test_MPBadfromEnv(self):
        os.environ["ATHENA_PROC_NUMBER"] = "-1"
        self.assertRaises(trfExceptions.TransformExecutionException, detectAthenaMPProcs)
        os.environ["ATHENA_PROC_NUMBER"] = "notAnInt"
        self.assertRaises(trfExceptions.TransformExecutionException, detectAthenaMPProcs)
        
    def test_MPfromArgdict(self):
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=8', 'random', 'values'])}
        self.assertEqual(detectAthenaMPProcs(argdict), 8)

    def test_MPfromArgdictEmpty(self):
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=0', 'random', 'values'])}
        self.assertEqual(detectAthenaMPProcs(argdict), 0)

    def test_MPfromArgdictBad(self):
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=-4', 'random', 'values'])}
        self.assertRaises(trfExceptions.TransformExecutionException, detectAthenaMPProcs, argdict)
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=notAnInt', 'random', 'values'])}
        self.assertRaises(trfExceptions.TransformExecutionException, detectAthenaMPProcs, argdict)
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=4', '--nprocs=8', 'values'])}
        self.assertRaises(trfExceptions.TransformExecutionException, detectAthenaMPProcs, argdict)

    def test_MPfromBoth(self):
        # Env should have priority
        os.environ["ATHENA_PROC_NUMBER"] = "4"
        argdict={'movealong': argList('nothing to see here'), 'athenaopts': argSubstepList(['--nprocs=2', 'random', 'values'])}
        self.assertEqual(detectAthenaMPProcs(argdict), 4)


class AthenaMPOutputParseTests(unittest.TestCase):
    def setUp(self):
        # Gah, this is a pest to setup! Need to creat stub files for the mother outputs
        # and the worker outputs
        outputStruct = [('.', [], ['data15_13TeV.00267167.physics_Main.merge.RAW._lb0176._SFO-1._0001.1', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'tmp.HIST_ESD_INT', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002']),
                         ('athenaMP-workers-RAWtoESD-r2e', ['worker_3', 'worker_7', 'worker_4', 'worker_5', 'worker_2', 'worker_6', 'evt_counter', 'worker_1', 'worker_0'], []), ('athenaMP-workers-RAWtoESD-r2e/worker_3', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_7', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_4', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_5', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_2', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_6', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/evt_counter', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_1', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out']), ('athenaMP-workers-RAWtoESD-r2e/worker_0', [], ['tmp.HIST_ESD_INT', 'AthenaMP.log', 'data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002', 'eventLoopHeartBeat.txt', 'ntuple_RAWtoESD.pmon.gz', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002', 'FileManagerLog', 'PoolFileCatalog.xml.BAK', 'PoolFileCatalog.xml', 'data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002', 'data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002', 'AtRanluxGenSvc.out'])]
        for delement in outputStruct:
            try:
                os.mkdir(delement[0])
            except OSError:
                pass
            for subdir in delement[1]:
                try:
                    os.mkdir(os.path.join(delement[0], subdir))
                except OSError:
                    pass
            for fname in delement[2]:
                open(os.path.join(delement[0], fname), "w")
        
        with open("athenaMP-outputs-RAWtoESD-r2e", "w") as mpoutput:
            print >>mpoutput, """<?xml version="1.0" encoding="utf-8"?>
<athenaFileReport>
  <Files OriginalName="data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002">
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_0/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_1/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_2/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_3/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_4/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_5/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_6/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
    <File description="POOL" mode="WRITE|CREATE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_7/data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002" shared="True" technology="ROOT"/>
  </Files>
  <Files OriginalName="tmp.HIST_ESD_INT">
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_0/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_1/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_2/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_3/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_4/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_5/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_6/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
    <File description="HIST" mode="WRITE" name="{CWD}/athenaMP-workers-RAWtoESD-r2e/worker_7/tmp.HIST_ESD_INT" shared="False" technology="ROOT"/>
  </Files>
</athenaFileReport>
""".format(CWD=os.getcwd())

    def tearDown(self):
        subprocess.call(['rm -fr athenaMP* data15* tmp.*'], shell=True)
    
    def test_basicMPoutputs(self):
        dataDict = {'BS': argFile("data15_13TeV.00267167.physics_Main.merge.RAW._lb0176._SFO-1._0001.1", io="input"),
                    'ESD': argFile("data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002"),
                    'HIST_ESD_INT': argFile("tmp.HIST_ESD_INT"),
                    'DRAW_EMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002"),
                    'DRAW_EGZ': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002"),
                    'DRAW_TAUMUH': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002"),
                    'DRAW_ZMUMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002"),}
        self.assertEqual(athenaMPOutputHandler("athenaMP-outputs-RAWtoESD-r2e", "athenaMP-workers-RAWtoESD-r2e", dataDict, 8), None)
        
    def test_missingMPoutputs(self):
        dataDict = {'ESD': argFile("data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002"),
                    'HIST_ESD_INT': argFile("tmp.HIST_ESD_INT"),
                    'DRAW_EMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002"),
                    'DRAW_EGZ': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002"),
                    'DRAW_TAUMUH': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002"),
                    'DRAW_NOTHERE': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_NOTHERE.f594._lb0176._SFO-1._0002"),
                    'DRAW_ZMUMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002"),}
        self.assertRaises(trfExceptions.TransformExecutionException, athenaMPOutputHandler, "athenaMP-outputs-RAWtoESD-r2e", "athenaMP-workers-RAWtoESD-r2e", dataDict, 8)

    def test_wrongMPoutputs(self):
        dataDict = {'ESD': argFile("data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002"),
                    'HIST_ESD_INT': argFile("tmp.HIST_ESD_INT"),
                    'DRAW_EMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002"),
                    'DRAW_EGZ': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002"),
                    'DRAW_TAUMUH': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002"),
                    'DRAW_NOTHERE': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_NOTHERE.f594._lb0176._SFO-1._0002"),
                    'DRAW_ZMUMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002"),}
        self.assertRaises(trfExceptions.TransformExecutionException, athenaMPOutputHandler, "athenaMP-outputs-RAWtoESD-r2e", "athenaMP-workers-RAWtoESD-r2e", dataDict, 20)
        
    def test_wrongMPoutputDir(self):
        dataDict = {'ESD': argFile("data15_13TeV.00267167.physics_Main.recon.ESD.f594._lb0176._SFO-1._0002"),
                    'HIST_ESD_INT': argFile("tmp.HIST_ESD_INT"),
                    'DRAW_EMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EMU.f594._lb0176._SFO-1._0002"),
                    'DRAW_EGZ': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_EGZ.f594._lb0176._SFO-1._0002"),
                    'DRAW_TAUMUH': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_TAUMUH.f594._lb0176._SFO-1._0002"),
                    'DRAW_NOTHERE': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_NOTHERE.f594._lb0176._SFO-1._0002"),
                    'DRAW_ZMUMU': argFile("data15_13TeV.00267167.physics_Main.recon.DRAW_ZMUMU.f594._lb0176._SFO-1._0002"),}
        self.assertRaises(trfExceptions.TransformExecutionException, athenaMPOutputHandler, "athenaMP-outputs-RAWtoESD-r2e-missing", "athenaMP-workers-RAWtoESD-r2e", dataDict, 20)
        
        
if __name__ == '__main__':
    unittest.main()
