#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @author: Sebastien Binet <binet@cern.ch>

"""Unit tests for verifying the proper working of trigdectool via PyAthena"""
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.5 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

from pyathena_utest import *

### running trig decision tool ------------------------------------------------
class PyTrigDecisionTool (PyAthenaTestCase):
    """running trigdecision tool from pyathena"""

    def test1_trigdectool_on_release_140100_data (self):
        """trigdectool on release 14.1.0.x data"""

        installRefFiles( ['pyathena_trigdectool_rel140100.ref'] )
        input_pool = [
            'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = 'pyathena_pytrigdectool_rel140100.pool'
        asciilog = 'trig_passed_evts.ascii'
        out = self._run_jobo(
        """
        EVTMAX=10;
        INPUT=%(INPUT)s
        OUTPUT='%(OUTPUT)s'
        ASCIILOG='%(ASCIILOG)s'
        include('PyAthenaTests/pytrigdec_jobOptions.py')
        """ % { 'INPUT' :input_pool,
                'OUTPUT':output_pool_file,
                'ASCIILOG': asciilog
                }
        )
        out_file = open(workDir('pyathena_trigdectool_rel140100.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:TrigDecTestAlg .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_trigdectool_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
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
