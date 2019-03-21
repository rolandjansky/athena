#!/usr/bin/env python

# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import glob
import logging
import multiprocessing
import os
import subprocess
import sys
import uuid

# Setting logging options
fmt = '%(asctime)s :: %(levelname)-8s :: %(message)s'
datefmt = '%m/%d/%Y %H:%M'
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG,
                    format=fmt,
                    datefmt=datefmt,
                    filename='./frozen_derivation_test.log',
                    filemode='w')
console = logging.StreamHandler()
formatter = logging.Formatter(fmt,datefmt=datefmt)
console.setFormatter(formatter)
logger.addHandler(console)

###
# Function to check if the derivation job succeeded or not
###
def check_job_success(logFile):
    ''' Return true/false depending on whether the provided log file has "successful run" pattern. '''

    # Simply tail the log and look for the famous "successful run"
    cmd = ' tail -n 20 ' + logFile + ' | grep "\\"successful run\\"" | wc -l '

    result = subprocess.check_output(['/bin/bash', '-c',cmd]).rstrip()
    logger.debug('Job success check for the log file %s is %s', logFile, result)

    return (result == '1')

####
## Function to run the clean test 
####
def run_clean_test(inputFile,extraArgs,release,cleanDir,uniqId):
    ''' Runs the clean derivation test and checks if the job succeeded or not. '''
   
    logger.info('Starting clean test...')

    # Build the actual command
    cmd = ( ' mkdir -p ' + cleanDir + '/clean_derivation_test_' + uniqId + ';'
            ' cd '       + cleanDir + '/clean_derivation_test_' + uniqId + ';'
            ' source $AtlasSetup/scripts/asetup.sh ' + release + ' >& /dev/null;'
            ' Reco_tf.py --inputAODFile %s %s > derivation_test.log 2>&1;'%(inputFile,extraArgs) )

    # Execute the actual command
    subprocess.call(cmd,shell=True)

    # Check the job status
    logFile = cleanDir + '/clean_derivation_test_' + uniqId + '/log.AODtoDAOD'
    logger.debug('Attempting to read the clean log file from %s', logFile)
    result = check_job_success(logFile)

    logger.info('Finished clean test w/ status %s...', 'SUCCESS' if result else 'FAILURE')
    pass

####
## Function to run the patched test 
####
def run_patched_test(inputFile,extraArgs,pwd,release):
    ''' Runs the patched derivation test and checks if the job succeeded or not. '''

    logger.info('Starting patched test...')

    # Build the actual command
    cmd = ' cd '+pwd+';'
    if not release: # This is the CI escape
        pass
    elif 'WorkDir_DIR' in os.environ:
        cmake_build_dir = (os.environ['WorkDir_DIR'])
        logger.debug('Using the local cmake build directory %s'%(cmake_build_dir))
        cmd += ( ' source $AtlasSetup/scripts/asetup.sh '+release+'  >& /dev/null;'
                 ' source '+cmake_build_dir+'/setup.sh;' )
    else :
        cmd += ( ' source $AtlasSetup/scripts/asetup.sh '+release+'  >& /dev/null;' )
    cmd += ( ' mkdir -p patched_derivation_test; cd patched_derivation_test;'
             ' Reco_tf.py --inputAODFile %s %s > derivation_test.log 2>&1;'%(inputFile,extraArgs) )

    # Execute the actual command
    subprocess.call(cmd,shell=True)

    # Check the job status
    logFile = pwd + '/patched_derivation_test/log.AODtoDAOD'
    logger.debug('Attempting to read the patched log file from %s', logFile )
    result = check_job_success(logFile)

    logger.info('Finished patched test w/ status %s...', 'SUCCESS' if result else 'FAILURE' )
    pass

####
## Function to run the diff-root 
####
def compare_files(cleanHeadDir,uniqId,pwd,isPatchedOnly):
    ''' Compares the clean and patched DAOD files to see if the contents differ. '''

    cleanDir = cleanHeadDir + '/clean_derivation_test_' + uniqId
    if isPatchedOnly:
        cleanDir = cleanHeadDir

    logger.debug('Reading the reference file from location '+cleanDir)

    comparison_command = ( 'acmd.py diff-root ' + cleanDir + '/DAOD_PHYSVAL.pool.root '
                           'patched_derivation_test/DAOD_PHYSVAL.pool.root ' 
                           '--error-mode resilient ' 
                           '--entries 20 > patched_derivation_test/diff-root.DAOD.log 2>&1' )
    output,error = subprocess.Popen(['/bin/bash', '-c', comparison_command], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    # We want to catch/print both container additions/subtractions as well as
    # changes in these containers.  `allGood_return_code` is meant to catch
    # other issues found in the diff (sync issues etc.)
    passed_all_test=True
    allGood_return_code=False

    logFile = pwd + '/patched_derivation_test/diff-root.DAOD.log'
    logger.debug('Attempting to read the diff-log file from %s', logFile )
    with open(logFile) as f:
        for line in f:
            if 'WARNING' in line: # Catches container addition/subtractions
                logger.error(line)
                passed_all_test=False
            if 'leaves differ' in line: # Catches changes in branches
                logger.error(line)
                passed_all_test=False
            if 'INFO all good.' in line:
                allGood_return_code = True

    _decision = passed_all_test and allGood_return_code
    if _decision:
        logger.info('Passed!')
    else:
        logger.error('Your tag breaks changes the AOD content! See patched_derivation_test/diff-root.DAOD.log file for more information.')

    return _decision

####
## Function to get release information
####
def get_release_setup():
    ''' This function tries to read environment variables to figure out the appropriate ATLAS setup '''

    # Read in environment variables
    current_nightly = os.environ['AtlasBuildStamp']
    release_base=os.environ['AtlasBuildBranch']
    release_head=os.environ['AtlasVersion']
    platform=os.environ['AthDerivation_PLATFORM']
    project=os.environ['AtlasProject']

    # Search through the main nightly installation location
    builds_dir_searchStr='/cvmfs/atlas-nightlies.cern.ch/repo/sw/'+release_base+'/[!latest_]*/'+project+'/'+release_head
    sorted_list = sorted(glob.glob(builds_dir_searchStr), key=os.path.getmtime)

    latest_nightly = ''
    for folder in reversed(sorted_list):
        if not glob.glob(folder+'/../../'+release_base+'__'+project+'*-opt*.log') : continue
        latest_nightly = folder.split('/')[-3]
        break

    if current_nightly != latest_nightly:
        logger.info('Please be aware that you are not testing your tags in the latest available nightly, which is %s',latest_nightly)

    setup='%s,%s,%s,AthDerivation'%(release_base,platform.replace('-', ','),current_nightly)

    logger.info('Your tags will be tested in environment %s',setup)

    return setup


####
## Main Function
####
def main():
    ''' Main function that parses user inputs, prepares and runs the tests '''

    from optparse import OptionParser
    parser=OptionParser(usage='%prog [options]',
                        version='%prog 0.1.0')

    parser.add_option('--log-level',
                      type='choice',
                      action='store',
                      dest='logLevel',
                      choices=['critical','error','warning','info','debug'],
                      default='info',
                      help='Logging level (default: info)')

    parser.add_option('-c',
                      '--clean-dir',
                      type='string',
                      dest='cleanDir',
                      default='/tmp/',
                      help='Specify the head directory for running the clean derivation tests '
                           '(default: /tmp/${USER}). If provided w/ -p (--patched) option, '
                           'the reference results are expected to be in this location.')

    parser.add_option('-p',
                      '--patched',
                      action='store_true',
                      dest='patched',
                      default=False,
                      help='Flag to avoid clean run. ' 
                           'In this mode only patched test will run and the '
                           'changes will be compared against predefined '
                           ' reference files (default: false)')

    parser.add_option('--mode',
                      type='choice',
                      action='store',
                      dest='mode',
                      choices=['data','mc'],
                      default='mc',
                      help='Test mode (default: mc)')

    (options,args) = parser.parse_args()

    logger.info(76*'-')

    # Set logging level
    logger.setLevel(options.logLevel.upper())

    # Check if patched only
    isPatchedOnly = options.patched

    # Check if the clean directory exists
    cleanHeadDir = options.cleanDir
    if str(cleanHeadDir) == '/tmp/':
        cleanHeadDir += os.environ['USER']

    if os.path.exists(cleanHeadDir):
        logger.info('The head directory for the output of the clean tests will be %s', cleanHeadDir)
    else:
        logger.error('The head directory for the output of the clean tests doesn\'t exist!')
        logger.error('Please check that %s exists and you have enough permissions to read/write.', cleanHeadDir)
        logger.error('If not please specify a folder that satisfies the requirements via -c or --clean-dir options.')
        sys.exit(-1)    

    # Check if an ATLAS project is setup
    if 'AtlasVersion' not in os.environ:
        logging.error('Please setup the an ATLAS release!')
        sys.exit(-1)

    # Get the release information
    mySetup = ''
    try:
        mySetup = get_release_setup()
    except KeyError as error:
        logger.warning('Cannot find the key %s',error)
        logger.warning('This might be benign if running in CI enviroment')
    cleanSetup = mySetup

    # Get the PWD and create UUID to avoid possible overlap between runs
    myPwd = os.getcwd()
    logger.debug('Current working directory is %s',myPwd)
    uniqId = str(uuid.uuid4())
    logger.debug('Created uuid %s',uniqId)

    # Run the tests
    logger.info('%s %s %s',25*'-',' Run AthDerivation Test ',25*'-')
    inputFile  = ( '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/{}'
                   ''.format('AOD.14795494._005958.pool.root.1' if options.mode == 'mc' else 'data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1') )
    extraArgs  = ( '--outputDAODFile pool.root --imf False --maxEvents 20 --reductionConf PHYSVAL --preExec \'rec.doApplyAODFix.set_Value_and_Lock(True); '
                   'from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "{}"; '
                   'from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); '
                   'topSequence += CfgMgr.xAODMaker__DynVarFixerAlg("InDetTrackParticlesFixer", Containers = ["InDetTrackParticlesAux." ] )\''
                   ''.format('BTagCalibRUN12-08-47' if options.mode == 'mc' else 'BTagCalibRUN12Onl-08-47') )

    myprocesses = {}
    if not isPatchedOnly:
        myprocesses['clean']   = multiprocessing.Process(target=run_clean_test,kwargs={'inputFile':inputFile,'extraArgs' : extraArgs,'release':cleanSetup,'cleanDir':cleanHeadDir,'uniqId':uniqId})
        myprocesses['patched'] = multiprocessing.Process(target=run_patched_test,kwargs={'inputFile':inputFile,'extraArgs' : extraArgs,'pwd':myPwd,'release':mySetup})
        myprocesses['clean'].start()
        myprocesses['patched'].start()
    else:
        myprocesses['patched'] = multiprocessing.Process(target=run_patched_test,kwargs={'inputFile':inputFile,'extraArgs' : extraArgs,'pwd':myPwd,'release':mySetup})
        myprocesses['patched'].start()

    for process in myprocesses:
        myprocesses[process].join()

    # Post processing
    logger.info('%s %s %s',25*'-',' Run xAOD content check ',25*'-')
    
    all_tests_passed = True

    if not compare_files(cleanHeadDir,uniqId,myPwd,isPatchedOnly):
        all_tests_passed = False

    logger.info('%s %s %s',25*'-','  Summary of all tests  ',25*'-')
    if all_tests_passed:
        logger.info(' All Tests: Passed (0)')
        logger.info(76*'-')
        sys.exit(0)
    else:
        logger.info(' All Tests: FAILED (-1)')
        logger.info(76*'-')
        sys.exit(-1)

if __name__ == '__main__':
    main()
