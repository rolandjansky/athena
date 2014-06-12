#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @brief: Trigger transform using PyJobTranforms.
# @details: Code based on PyJobTransforms/share/Athena_tf.py
# @author: Mark Stockton

import argparse
import sys
import time
import traceback

import logging

# Setup core logging here
from PyJobTransforms.trfLogger import msg
msg.info('logging set in %s' % sys.argv[0])

from PyJobTransforms.trfExitCodes import trfExit
from PyJobTransforms.transform import transform
from PyJobTransforms.trfExe import athenaExecutor
from PyJobTransforms.trfArgs import addAthenaArguments
from PyJobTransforms.trfDecorators import stdTrfExceptionHandler, sigUsrStackTrace

import PyJobTransforms.trfExceptions as trfExceptions
import PyJobTransforms.trfArgClasses as trfArgClasses

from TrigTransform.trigPreRun import trigPreRun
from TrigTransform.trigExe import trigExecutor
from TrigTransform.trigPostRun import trigPostRun

@stdTrfExceptionHandler
@sigUsrStackTrace
def main(): 
    msg.info('This is %s' % sys.argv[0])
    
    #note that indata (outdata) can not be the same and must be the same text, ['**'], as in the argument input**File (output**File)  
    trfMT = transform(trfName = 'Trig_trf', executor = trigExecutor(name = 'athena',exe = 'athenaMT.py',exeArgs=['athenaoptsMT'],inData = ['BS'], outData = ['BS_MT']))
    addAthenaArguments(trfMT.parser)
    addTriggerArgs(trfMT.parser)
    trfMT.parseCmdLineArgs(sys.argv[1:])

    #any debug statements will work from here onwards if using --verbose or --loglevel DEBUG
    
    #Convert arg names and carry out operations from any of the options that are to be done before running
    trigPreRun(trfMT)
    
    #Run the transform and generate final report
    trfMT.execute()
    trfMT.generateReport()

    #Carry out operations from any of the options that are to be done after running     
    trigPostRun(trfMT)

    msg.info("%s stopped at %s, trf exit code %d" % (sys.argv[0], time.asctime(), trfMT.exitCode))
    sys.exit(trfMT.exitCode)


def addTriggerArgs(parser):
    ## Notes:
    #     Use group='Trig_trf' to get these arguments in their own sub-section (of --help)
    #     Use runarg=True for commands to be passed on the command line to athenaMT/PT
    
    #Options currently implemented for use within the transform
    parser.defineArgGroup('Trig_trf', 'Trig_trf specific options')
    parser.add_argument('--inputBSFile', type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='input', runarg=True, type='bs'),
                          help='Set the input file name', group='Trig_trf') 
    parser.add_argument('--outputBS_MTFile', '--outputRAWFile', type=trfArgClasses.argFactory(trfArgClasses.argBSFile, io='output', runarg=True, type='bs'),
                          help='Set the output file name', group='Trig_trf')

    parser.add_argument('--athenaoptsMT', group = 'Trig_trf', type=trfArgClasses.argFactory(trfArgClasses.argList, splitter=' '), metavar='OPT1 OPT2 OPT3', 
                        help='Extra options to pass to athenaMT. Options starting with "-" must be given as --athenaopts=\'--opt1 --opt2 ...\'') 
    parser.add_argument('--athenaoptsPT', group = 'Trig_trf', type=trfArgClasses.argFactory(trfArgClasses.argList, splitter=' '), metavar='OPT1 OPT2 OPT3', 
                        help='Extra options to pass to athenaPT. Options starting with "-" must be given as --athenaopts=\'--opt1 --opt2 ...\'') 

    parser.add_argument('--connection', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=False), metavar='STR',
                          help='Set the connection to the configuration database', group='Trig_trf')
    parser.add_argument('--smk', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=False), metavar='SMK',
                          help='Set the super master key for the database configuration', group='Trig_trf')
    parser.add_argument('--lvl1ps', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=False), metavar='L1PS',
                          help='Set the prescales sets for the LVL1', group='Trig_trf')
    parser.add_argument('--hltps', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=False), metavar='HLTPS',
                          help='Set the prescales sets for the HLT', group='Trig_trf')
    parser.add_argument('--run_number', type=trfArgClasses.argFactory(trfArgClasses.argInt, runarg=True), metavar='NUM',
                          help='Set the run number to which the events belong', group='Trig_trf')
    parser.add_argument('--post_commands', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Postcommands to change configuration on top of DB configuration', group='Trig_trf')
    
    #TODO might be useful, see trigPreRun
    #parser.add_argument('--dump_options', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
    #                      help='dumps the current default options to stdout', group='Trig_trf')
    
    #TODO implement other options from existing transforms

    #example log level
    #parser.add_argument('--maxMsgLevel', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True),
    #                      help='highest message level to print in athena', group='Trig_trf')
    
    #TODO: do we need the transform version number?          
    #parser.add_argument('--version', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
    #                      help='show program\'s version number and exit', group='Trig_trf')
    parser.add_argument('--doCTP', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Force the RoIb data to be extracted from the CTP fragment and attached to the event data', group='Trig_trf')
    parser.add_argument('--doMUCTPI', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Fix the Muctpi data', group='Trig_trf')
    parser.add_argument('--doCopyIn', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Force the copying of the input file into the run dir', group='Trig_trf')
    parser.add_argument('--doStreaming', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Stream the events after the trigger reprocessing', group='Trig_trf')
    parser.add_argument('--doDummyStreaming', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Just rename the events out of AthenaPT as they would come from an unknow stream', group='Trig_trf')
    parser.add_argument('--doRecording', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Record the output files on CASTOR', group='Trig_trf')
    parser.add_argument('--doL2', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Reprocess the LVL2 with AthenaMT', group='Trig_trf')
    parser.add_argument('--doNotTruncateL2R', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='raise the limit of the L2 result size to 1200000 words', group='Trig_trf')
    parser.add_argument('--doEF', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Reprocess the EF with AthenaPT', group='Trig_trf')
    parser.add_argument('--doPre', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Reprocess the LVL2 with AthenaMT', group='Trig_trf')
    parser.add_argument('--doPost', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Reprocess the EF with AthenaPT', group='Trig_trf')
    parser.add_argument('--doHLTDebugWrite', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='Set the DEBUG output level for the trigger reprocessing', group='Trig_trf')
    
    #parser.add_argument('--file_stream=STR', type=trfArgClasses.argFactory(trfArgClasses., runarg=True),
    #                      help='Set the input file stream name', group='Trig_trf')
    
    parser.add_argument('--output_dir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the directory name for copying the output files', group='Trig_trf')
    parser.add_argument('--crash_dir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the directory name for copying the events that crashed the trigger', group='Trig_trf')
    parser.add_argument('--histo_dir', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the directory name for copying the output histograms', group='Trig_trf')
    parser.add_argument('--service_class', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the service class for CASTOR IO', group='Trig_trf')
    parser.add_argument('--file_prefix', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the directory name for copying the output files', group='Trig_trf')
    parser.add_argument('--job_id', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the jobId that identifys the processing', group='Trig_trf')
    parser.add_argument('--jobOptions', type=trfArgClasses.argFactory(trfArgClasses.argFile, runarg=True), metavar='FILE',
                          help='Set the jobOptions to be used for the reprocessing', group='Trig_trf')
    parser.add_argument('--options', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the options for the jobOptions', group='Trig_trf')
    parser.add_argument('--conditions', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='List of new \'folder tag\' to be used for the run', group='Trig_trf')
    parser.add_argument('--filters', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='List of filters to be applied on the stream tag of the accept event', group='Trig_trf')
    parser.add_argument('--stream_out', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Specified which event tags have to be streamed out by Streamer', group='Trig_trf')
    parser.add_argument('--proc_session', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='STR',
                          help='Set the processing version', group='Trig_trf')
    parser.add_argument('--doLVL1Remapping', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='MOD',
                          help='Set the module to be used for LVL1 remapping', group='Trig_trf')
    parser.add_argument('--enLVL1prescales', type=trfArgClasses.argFactory(trfArgClasses.argString, runarg=True), metavar='MOD',
                          help='Set the module name to be used for enabling the LVL1 prescales', group='Trig_trf')
    parser.add_argument('-v, --verbose', type=trfArgClasses.argFactory(trfArgClasses.argBool, runarg=True),
                          help='enable the printout verbosity', group='Trig_trf')
    
    

if __name__ == '__main__':
    main()