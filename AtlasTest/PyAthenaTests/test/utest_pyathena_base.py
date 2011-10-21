#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @author: Sebastien Binet <binet@cern.ch>

"""Unit tests for verifying the proper working of basic functionalities of
PyAthena"""
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.10 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

from pyathena_utest import *

### basic functionalities -----------------------------------------------------
class PyAthenaBase (PyAthenaTestCase):
    """basic functionalities"""

    def test1_base_tests (self):
        """base tests"""
        installRefFiles( ['pyathenatests_base.ref'] )
        out = self._run_jobo("""include('AthenaPython/test_pyathena.py')""")
        out_file = open(workDir('pyathenatests_base.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [
            l.strip() for l in out.splitlines()
            if re.match('^Py:.', l) and
            not re.match('^Py:ConfigurableDb.',l) and
            not re.match('^Py:Athena.*? including file',l) and
            not re.match('^Py:Athena.*? INFO using release', l) and
            # disable MyNameAuditor as it captures too many
            # string and component changes
            not re.match('^Py:MyNameAuditor.', l) ]

        ref_file = []
        for l in open(workDir('pyathenatests_base.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(out_file.name)
        return

    def test2_hist_write (self):
        """test writing histos and tuples w/ py-THistSvc"""
        installRefFiles( ['pyathenatests_histwriter.ref'] )
        out = self._run_jobo("""
        ## http://docs.python.org/lib/module-random.html
        ## initialize the pseudo-random numbers generator with a (dummy) seed
        _seed = 1234567890
        from random import seed as _init_seed
        _init_seed(_seed)

        ## real jobo
        include ('AthenaPython/test_pyhistsvc_write.py')
        """)
        out_file = open(workDir('pyathenatests_histwriter.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:PyHistWriter.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathenatests_histwriter.ref'),'r'):
            ref_file.append(l.strip())

        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(out_file.name)
        return

    def test3_hist_read (self):
        """test reading histos and tuples w/ py-THistSvc"""

        installRefFiles( ['pyathenatests_histreader.ref'] )
        out = self._run_jobo("""
        include ('AthenaPython/test_pyhistsvc_read.py')
        """)
        out_file = open(workDir('pyathenatests_histreader.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:PyHistReader.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathenatests_histreader.ref'),'r'):
            ref_file.append(l.strip())

        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(out_file.name)
        safe_remove('tuple1.root')
        safe_remove('tuple2.root')
        safe_remove('tuple3.root')
        return

    def test4_basic_record_retrieve (self):
        """test basic record/retrieve with py-StoreGateSvc"""
        installRefFiles( ['pyathena_basic_record_retrieve.ref'] )
        output_pool_file = workDir('basic_record_retrieve.out.pool')
        out = self._run_jobo("""
        OUTPUT='%(OUTPUT)s'
        include ('PyAthenaTests/pyathena_basic_record_retrieve.py')
        """ % { 'OUTPUT' : output_pool_file }
        )
        self.failUnless(os.path.exists(output_pool_file))
        
        out_file = open(workDir('pyathena_basic_record_retrieve.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:re.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_basic_record_retrieve.ref'),'r'):
            ref_file.append(l.strip())

        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(out_file.name)
        safe_remove(output_pool_file)
        return
        
    def test5_stream_filter (self):
        """test that filtering stream works"""

        input_pool = [
            '/afs/cern.ch/atlas/project/rig/referencefiles/dataStreams_ESD.AOD_50Events/data10_7TeV.00167607.physics_JetTauEtmiss.recon.ESD.f298._lb0087._SFO-4._0001.1_50Events_rel.16.0.3.8_rereco'
            ]
        output_pool_file = workDir('pyathena_basic_filtered_rel140100.pool')
        basic_asciilog = workDir('basic_passed_evts.ascii')
        out = self._run_jobo("""
        EVTMAX=-1;
        INPUT=%(INPUT)s
        OUTPUT='%(OUTPUT)s'
        ASCIILOG='%(ASCIILOG)s'
        include('PyAthenaTests/pyfilter_stream_jobOptions.py')
        """ % { 'INPUT' :input_pool,
                'OUTPUT':output_pool_file,
                'ASCIILOG' : basic_asciilog
                } )

        self.failUnless(os.path.exists(output_pool_file))
        self.failUnless(os.path.exists(basic_asciilog))
        
        # now run the eventinfo dumper alg
        input_pool = [ output_pool_file ]
        asciilog   = workDir('evtlist.ascii')
        out = self._run_jobo("""
        EVTMAX=-1
        INPUT=%(INPUT)s
        ASCIILOG='%(ASCIILOG)s'
        include ('PyAthenaTests/pyevtinfo_jobOptions.py')
        """ % { 'INPUT' : input_pool,
                'ASCIILOG' : asciilog
                })
        self.failUnless(os.path.exists(asciilog))
        
        ref_file = [ l.strip() for l in open(basic_asciilog,'r') ]
        evt_file = [ l.strip() for l in open(asciilog, 'r') ]

        diff = difflib.unified_diff(evt_file, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(basic_asciilog)
        safe_remove(asciilog)
        
        return

## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

 # don't want to depend on gaudimodule
   assert( not 'gaudimodule' in sys.modules )

   sys.exit( result )
