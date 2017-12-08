#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @author: Sebastien Binet <binet@cern.ch>

"""Unit tests for verifying the proper working of inspecting clusters in
PyAthena"""
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.5 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

from pyathena_utest import *

### running over clusters -----------------------------------------------------
class PyClustersTest (PyAthenaTestCase):
    """Running over clusters"""

    def test1_batch_clusters_in_release_140100_data (self):
        """batch clusters in release 14.1.0.x data"""
        installRefFiles(['pyathena_pyclusters_rel140100.ref'])
        
        input_pool = [
            'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_root_file = workDir('pyathena_pyclusters_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)s
        TUPLEOUT='%(OUTPUT)s'
        include('PyAthenaTests/pyclusters_jobOptions.py')
        """ % { 'INPUT' :input_pool,
                'OUTPUT':output_root_file,
                }
        )
        out_file = open(workDir('pyathena_pyclusters_rel140100.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:ClusterExAlg .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pyclusters_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_root_file)
        safe_remove(out_file.name)
        return
        
    def test2_ares_batch_clusters_in_release_140100_data (self):
        """batch clusters in release 14.1.0.x data (via Ares)"""
        return
    
        ## note: same ref-file than for test1
        installRefFiles(['pyathena_pyclusters_rel140100.ref'])

        input_pool = [
            'root://eosatlas//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_root_file = workDir('ares_pyclusters_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)s
        TUPLEOUT='%(OUTPUT)s'
        include('PyAthenaTests/ares_clusters_jobOptions.py')
        """ % { 'INPUT' :input_pool,
                'OUTPUT':output_root_file,
                }
        )
        out_file = open(workDir('ares_pyclusters_rel140100.log'), 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:ClusterExAlg .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pyclusters_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_root_file)
        safe_remove(out_file.name)
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
