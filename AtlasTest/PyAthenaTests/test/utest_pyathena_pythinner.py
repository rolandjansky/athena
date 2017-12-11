#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
# @author: Sebastien Binet <binet@cern.ch>

"""Unit tests for verifying the proper working of thinning via PyAthena"""
import unittest, sys, atexit, os, commands, tempfile, re, difflib

### data ----------------------------------------------------------------------
__version__ = "$Revision: 1.13 $"
__author__  = "Sebastien Binet <binet@cern.cn>"

from pyathena_utest import *

### basic thinning of data ----------------------------------------------------
class PyBasicThinning (PyAthenaTestCase):
    """Thinning of basic data"""

    def test1_basic_thinning (self):
        """test basic thinning of basic data"""

        installRefFiles( ['pyathena_pythinner_basic_thinning.ref'] )

        ## prepare input file
        output_pool_file = workDir('basic_athexparticles.pool')
        out = self._run_jobo("""
        EVTMAX=5;
        OUTPUT='%(OUTPUT)s'
        include('AthExThinning/AthExThinning_makeData.py')
        job = AlgSequence()
        job.CreateData.TestNames = ['basic']
        """ % {'OUTPUT':output_pool_file})

        self.failUnless(os.path.exists(output_pool_file))

        ## run basic thinning
        input_pool_file  = workDir('basic_athexparticles.pool')
        output_pool_file = workDir('thinned.basic_athexparticles.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/pythin_basic_jobOptions.py')
        job = AlgSequence()
        job.thinner.coll_name = 'Particles_basic'

        ## and append an alg-reader to make sure data and links are not
        ## corrupted by the ThinningSvc
        job += CfgMgr.AthExThinning__ReadThinnedData(
               'ReadThinnedData',
               Particles   = 'Particles',
               Decay       = 'TwoBodyDecay',
               Elephantino = 'PinkElephantino',
               TestNames   = ['basic'],
               OutputLevel = DEBUG
               )
        """ % {
                'INPUT' :[input_pool_file],
                'OUTPUT':output_pool_file
                }
            )

        self.failUnless(os.path.exists(output_pool_file))

        out_file = open('pyathena_pythinner_basic_thinning.log', 'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thinner .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_basic_thinning.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(input_pool_file)
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return
        
    def test2_multistream_thinning (self):
        """test multistream thinning of basic data"""

        installRefFiles( ['pyathena_pythinner_multistream_thinning.ref',
                          'pyathena_pythinner_multistream_check_thin0.ref',
                          'pyathena_pythinner_multistream_check_thin1.ref',
                          'pyathena_pythinner_multistream_check_thin2.ref'] )

        ## prepare input file
        output_pool_file = workDir('multistream_athexparticles.pool')
        out = self._run_jobo("""
        EVTMAX=5;
        OUTPUT='%(OUTPUT)s'
        include('AthExThinning/AthExThinning_makeData.py')
        job = AlgSequence()
        job.CreateData.TestNames = ['0', '1']

        ## import my algorithm and add it to the list of algorithms to be run
        from PyAthenaTests.Lib import ThinChecker
        ## checking containers content
        job += ThinChecker(name='thin_checker_0', OutputLevel = INFO)
        job.thin_checker_0.coll_type = 'AthExParticles'
        job.thin_checker_0.coll_name = 'Particles_0'

        job += ThinChecker(name='thin_checker_1', OutputLevel = INFO)
        job.thin_checker_1.coll_type = 'AthExParticles'
        job.thin_checker_1.coll_name = 'Particles_1'
        """ % {'OUTPUT':output_pool_file})

        self.failUnless(os.path.exists(output_pool_file))

        ## validating output
        prod_file = open(workDir('pyathena_pythinner_multistream_makedata.log'),
                         'w')
        prod_file.writelines( [l for l in out] )
        prod_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_checker_[01] .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_multistream_check_thin2.ref'),
                      'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## run multistream thinning
        input_pool_file  = workDir('multistream_athexparticles.pool')
        output0_pool_file = workDir('thinned_0.multistream_athexparticles.pool')
        output1_pool_file = workDir('thinned_1.multistream_athexparticles.pool')
        output2_pool_file = workDir('not_thinned.multistream_athexparticles.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT_0='%(OUTPUT_0)s'
        OUTPUT_1='%(OUTPUT_1)s'
        OUTPUT_NOTHINNING='%(OUTPUT_NOTHINNING)s'
        include('PyAthenaTests/pythin_multistream_jobOptions.py')
        """ % {
                'INPUT' :[input_pool_file],
                'OUTPUT_0':output0_pool_file,
                'OUTPUT_1':output1_pool_file,
                'OUTPUT_NOTHINNING':output2_pool_file,
                }
            )

        self.failUnless(os.path.exists(output0_pool_file))
        self.failUnless(os.path.exists(output1_pool_file))
        self.failUnless(os.path.exists(output2_pool_file))

        out_file = open(workDir('pyathena_pythinner_multistream_thinning.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thinner_[01] .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_multistream_thinning.ref'),
                      'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## run checking algorithms

        # - validating output-0
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        include('PyAthenaTests/pythin_multistream_checker_jobOptions.py')
        """ % {
                'INPUT' :[output0_pool_file],
                }
            )
        
        chk0_file = open(
            workDir('pyathena_pythinner_multistream_check_thin0.log'), 'w')
        chk0_file.writelines( [l for l in out] )
        chk0_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_checker_[01] .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_multistream_check_thin0.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        # - validating output-1
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        include('PyAthenaTests/pythin_multistream_checker_jobOptions.py')
        """ % {
                'INPUT' :[output1_pool_file],
                }
            )
        
        chk1_file = open(
            workDir('pyathena_pythinner_multistream_check_thin1.log'), 'w')
        chk1_file.writelines( [l for l in out] )
        chk1_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_checker_[01] .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_multistream_check_thin1.ref'),
                      'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        # - validating output-2
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        include('PyAthenaTests/pythin_multistream_checker_jobOptions.py')
        """ % {
                'INPUT' :[output2_pool_file],
                }
            )
        
        chk2_file = open(
            workDir('pyathena_pythinner_multistream_check_thin2.log'), 'w')
        chk2_file.writelines( [l for l in out] )
        chk2_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_checker_[01] .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_multistream_check_thin2.ref'),
                      'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(input_pool_file)
        safe_remove(output0_pool_file)
        safe_remove(output1_pool_file)
        safe_remove(output2_pool_file)
        safe_remove(prod_file.name)
        safe_remove(out_file.name)
        safe_remove(chk0_file.name)
        safe_remove(chk1_file.name)
        safe_remove(chk2_file.name)
        return
        
### thinning of atlfast-aod data ----------------------------------------------
class PyThinAtlfastAod (PyAthenaTestCase):
    """Thinning of atlfast-aod data"""

    def test1_thin_release_140100_data (self):
        """atlfast thinning of release 14.1.0.x data"""

        installRefFiles( ['pyathena_pythinner_atlfast_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_atlfast_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=10;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/pythin_jobOptions.py')
        job.Thinner.coll_type = 'JetCollection'
        job.Thinner.coll_name = 'AtlfastJetContainer'
        job.OutStream.TakeItemsFromInput = False
        job.OutStream.ForceRead = False
        job.OutStream.ItemList = [ 'EventInfo#*', 'JetCollection#*' ]
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_atlfast_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:Thinner .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_atlfast_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return
    pass # class PyThinAtlfastAod

### thinning of reco-aod data -------------------------------------------------
class PyThinRecAod (PyAthenaTestCase):
    """Thinning of reco-aod data"""

    def test1_thin_jets_release_140100_data (self):
        """reco-aod thinning of jets (release 14.1.0.x data)"""


        ##FIXME ### DOES NOT WORK AS OF 2008-05-27
        return
    
        installRefFiles( ['pyathena_pythinner_reco-aod_jets_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-aod_jets_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/utest_thin_jets_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_reco-aod_jets_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_.', l) 
                         or re.match('^Py:jet_slimmer.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-aod_jets_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    def test2_thin_eles_release_140100_data (self):
        """reco-aod thinning of electrons (release 14.1.0.x data)"""

        ##FIXME ### DOES NOT WORK AS OF 2010-08-26
        return

        installRefFiles( ['pyathena_pythinner_reco-aod_eles_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-aod_eles_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/utest_thin_eles_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_reco-aod_eles_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-aod_eles_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    def test3_thin_photons_release_140100_data (self):
        """reco-aod thinning of photons (release 14.1.0.x data)"""

        ##FIXME ### DOES NOT WORK AS OF 2010-08-26
        return

        installRefFiles( ['pyathena_pythinner_reco-aod_photons_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-aod_photons_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/utest_thin_photons_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_reco-aod_photons_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-aod_photons_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    def test4_thin_taus_release_140100_data (self):
        """reco-aod thinning of taus (release 14.1.0.x data)"""

        ##FIXME ### DOES NOT WORK AS OF 2010-08-26
        return

        
        installRefFiles( ['pyathena_pythinner_reco-aod_taus_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-aod_taus_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/utest_thin_taus_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_reco-aod_taus_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-aod_taus_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    def test5_thin_muons_release_140100_data (self):
        """reco-aod thinning of muons (release 14.1.0.x data)"""

        ##FIXME ### DOES NOT WORK AS OF 2010-08-26
        return

        installRefFiles( ['pyathena_pythinner_reco-aod_muons_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.AOD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-aod_muons_rel140100.pool')
        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'
        include('PyAthenaTests/utest_thin_muons_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT':output_pool_file})
        out_file = open(workDir('pyathena_pythinner_reco-aod_muons_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:thin_.', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-aod_muons_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    pass # class PyThinRecAod

### thinning of reco-esd data -------------------------------------------------
class PyThinRecEsd (PyAthenaTestCase):
    """Thinning of reco-esd data"""

    def test1_thin_cells_release_140100_data (self):
        """reco-esd thinning of calocells (release 14.1.0.x data)"""

        ##FIXME ### DOES NOT WORK AS OF 2010-08-26
        return

        installRefFiles( ['pyathena_pythinner_reco-esd_calocells_rel140100.ref'] )
        input_pool = [
            'root://eosatlas.cern.ch//eos/atlas/user/b/binet/reffiles/14.1.0.x'\
            '/AllBasicSamples.ESD.pool.root'
            ]
        output_pool_file = workDir('pyathena_pythinner_reco-esd_calocells_rel140100.pool')

        out = self._run_jobo(
        """
        EVTMAX=5;
        INPUT=%(INPUT)r
        OUTPUT='%(OUTPUT)s'

        include('PyAthenaTests/utest_thin_cells_release_140100.py')
        """ % { 'INPUT' : input_pool,
                'OUTPUT': output_pool_file,})
        out_file = open(workDir('pyathena_pythinner_reco-esd_calocells_rel140100.log'),
                        'w')
        out_file.writelines( [l for l in out] )
        out_file.flush()

        out_for_diff = [ l.strip() for l in out.splitlines()
                         if re.match('^Py:Thinner .', l) ]
        
        ref_file = []
        for l in open(workDir('pyathena_pythinner_reco-esd_calocells_rel140100.ref'),'r'):
            ref_file.append(l.strip())
            
        diff = difflib.unified_diff(out_for_diff, ref_file)
        diff = os.linesep.join( [d for d in diff] )
        self.failUnless(diff=='', diff)

        ## make sure the StreamESD from reprocessing hasn't been run...
        self.failUnless(not os.path.exists('dummy.esd.pool'))
        
        ## delete files if all good
        safe_remove(output_pool_file)
        safe_remove(out_file.name)
        return

    pass # class PyThinRecEsd


## actual test run
if __name__ == '__main__':
   loader = unittest.TestLoader()
   testSuite = loader.loadTestsFromModule( sys.modules[ __name__ ] )

   runner = unittest.TextTestRunner( verbosity = 2 )
   result = not runner.run( testSuite ).wasSuccessful()

 # don't want to depend on gaudimodule
   assert( not 'gaudimodule' in sys.modules )

   sys.exit( result )
