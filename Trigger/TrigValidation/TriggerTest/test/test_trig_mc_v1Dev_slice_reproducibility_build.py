#!/usr/bin/env python

# art-description: Compares results of a slice chains when running in full menu and when running alone
# art-type: build
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.


import logging
from TrigValTools.TrigValSteering import Test, ExecStep, CheckSteps, Common
import six

Common.trigvalsteering_logging_level = logging.DEBUG
ex = ExecStep.ExecStep('FullMenu')
ex.type = 'athena'
ex.job_options = 'TrigUpgradeTest/full_menu.py'
ex.input = 'ttbar'
ex.threads = 1
# LS2_v1 soon to be renamed to Dev_pp_run3_v1
ex.args = '-c "setMenu=\'LS2_v1\';doWriteBS=False;doWriteRDOTrigger=False;"'

def single_slice( name, args ):
    slice_ex = ExecStep.ExecStep( name )    
    for prop,val in six.iteritems(ex.__dict__):
        if prop != 'name':
            setattr( slice_ex, prop, val )
    slice_ex.args += " " + args    
    return slice_ex

from TrigValTools.TrigValSteering.Step import Step
import re
class CompareSlicesToFullMenuStep( Step ):
    def __init__( self, name='CompareSlicesToFullMenuStep' ):
        super( CompareSlicesToFullMenuStep, self ).__init__( name )
        self.log_regex = re.compile(r'TrigSignatureMoniMT.*INFO.HLT.*decisions.*')
        self.full_menu_log = None
        self.slice_logs = None
        self.required = True
        self.auto_report_result = True

    def configure( self, test ):
        self.full_menu_log = test.exec_steps[0].name+'.log'
        self.slice_logs = [  t.name+'.log' for t in test.exec_steps[1:] ]
        pass

    def fetch_lines( self, logname ):
        lines = []
        self.log.info('Scanning %s', logname )
        with open( logname, 'r' ) as logfile:
            for line in logfile:
                if self.log_regex.match( line ):
                    lines.append( line.split()[2:-1] ) #drop component INFO and the "final decision" count
        return lines


    def run( self, dry_run=False ):
        self.log.info( 'Running %s comparing %s with slice logs %s ',
                       self.name, self.full_menu_log, str( self.slice_logs ) )
        if dry_run:
            self.result = 0
            return self.result, '# (internal) {} -> skipped'.format(self.name)

        full_menu_test_lines = self.fetch_lines( self.full_menu_log )
        full_menu_test_lines.sort()
        error = False
        with open('crosscheck.log', 'w') as result_log:

            for slice_log in self.slice_logs:
                slice_test_lines = self.fetch_lines( slice_log )
                slice_test_lines.sort()
                for result_in_slice in slice_test_lines:
                    chain_name = result_in_slice[0]
                    result_in_full_menu = [ l for l in full_menu_test_lines if l[0] == chain_name ] [0] [ :len( result_in_slice ) ] # only first line, and only first few numbers
                    if result_in_slice != result_in_full_menu:
                        error = True
                        result_log.write( 'CompareSlicesToFullMenuStep ERROR Difference found in {} and {}\n'.format( slice_log, self.full_menu_log ) )
                        result_log.write( 'CompareSlicesToFullMenuStep ERROR Slice     {}\n'.format( " ".join( result_in_slice ) ) )
                        result_log.write( 'CompareSlicesToFullMenuStep ERROR Full Menu {}\n'.format( " ".join( result_in_full_menu ) ) )

        # Command to report in commands.json
        cmd = '# (internal) {}'.format(self.name)
        if error:
            self.result = 1
            cmd += ' -> failed'
        else:
            self.result = 0
        self.report_result()
        return self.result, cmd



test            = Test.Test()
test.art_type   = 'build'
test.exec_steps = [ex,                   
                   single_slice( 'Egamma',   '-c "doEmptyMenu=True;doEgammaSlice=True;"' ),
                   single_slice( 'Muon',     '-c "doEmptyMenu=True;doMuonSlice=True;"' ),
                   single_slice( 'Tau',      '-c "doEmptyMenu=True;doTauSlice=True;"' ),
                   single_slice( 'Jet',      '-c "doEmptyMenu=True;doJetSlice=True;"' ),
                   single_slice( 'Bjet',     '-c "doEmptyMenu=True;doBjetSlice=True;"' ),
                   single_slice( 'Bphysics', '-c "doEmptyMenu=True;doBphysicsSlice=True;"' ),
                   single_slice( 'MET',      '-c "doEmptyMenu=True;doMETSlice=True;"' )]


merge_log = CheckSteps.LogMergeStep()
merge_log.merged_name = 'athena.all.log'
merge_log.log_files = [ 'athena.'+x.name+'.log' for x in test.exec_steps ] + [ 'crosscheck.log']
check_log = CheckSteps.CheckLogStep('CheckLog')
check_log.log_file = merge_log.merged_name
cross_check_logs = CompareSlicesToFullMenuStep()
test.check_steps = [ cross_check_logs, merge_log, check_log ]

import sys
sys.exit(test.run())
