#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @author: Sebastien Binet <binet@cern.ch>

"""Unit tests for verifying the proper working of various FDR jobo examples
via PyAthena"""
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.5 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

from pyathena_utest import *

### basic FDR tests -----------------------------------------------------------
class PyBasicFdrTests (PyAthenaTestCase):
    """Basic tests for py-FDR"""

    def test1_pyfdr_aodtodpd_on_release_140100_data (self):
        """testing aodtodpd jobo on release 14.1.0.x data"""

        installRefFiles( ['pyathena_pyfdr_aodtodpd_rel140100.ref'] )

        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pyfdr_aodtodpd_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=-1;
        OUTPUT='%(OUTPUT)s'
        include('DPDUtils/aodtodpd.py')
        svcMgr.EventSelector.InputCollections = %(INPUT)r
        StreamDPD.OutputFile = OUTPUT
        svcMgr.GeoModelSvc.AtlasVersion = 'ATLAS-CSC-05-01-00'
        """ % { 'INPUT' : input_pool,
                'OUTPUT': output_pool_file,
                }
        )

        self.failUnless(os.path.exists(output_pool_file))

        out_file = open(workDir('pyathena_pyfdr_aodtodpd_rel140100.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if  re.match('^Py:.', l) and
                         not re.match('^Py:ConfigurableDb.',l) and
                         not re.match('^Py:Athena.*? INFO including.', l) ]
            
        ref_file = []
        for l in open(workDir('pyathena_pyfdr_aodtodpd_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    def test2_thin_mctruth_release_140100_data (self):
        """thinning/slimming of mctruth (release 14.1.0.x data)"""

        installRefFiles( ['pyathena_pyfdr_truthslimming_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pyfdr_truthslimming_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/pythin_mctruth_jobOptions.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pyfdr_truthslimming_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:mc_.*?_dumper.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pyfdr_truthslimming_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    pass # PyBasicFdrTests

## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

 # don't want to depend on gaudimodule
   assert( not 'gaudimodule' in sys.modules )

   sys.exit( result )
