#!/usr/bin/env python3

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import logging
import subprocess
import sys

# Setting logging options
fmt = '%(asctime)s :: %(levelname)-8s :: %(message)s'
datefmt = '%m/%d/%Y %H:%M'
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG,
                    format=fmt,
                    datefmt=datefmt,
                    filename='./vtune_athena.log',
                    filemode='w')
console = logging.StreamHandler()
formatter = logging.Formatter(fmt,datefmt=datefmt)
console.setFormatter(formatter)
logger.addHandler(console)

####
## Check Athena Setup
####
def checkAthenaSetup():
    try:
      a = subprocess.check_output(['athena','--version'])
      logger.debug('Athena version information \n %s',a)
    except Exception:
      logger.fatal('Athena is not setup!')
      sys.exit(-1)

####
## Check VTune Setup
####
def checkVTuneSetup():
    try:
      a = subprocess.check_output(['vtune','--version'])
      logger.debug('VTune version information \n %s',a)
    except Exception:
      logger.fatal('VTune is not setup!')
      sys.exit(-1)

####
## AutoGen a jobOptions fragment
####
def generateJOFragment(fileName,firstEvent,lastEvent):
    logger.info('Creating jOptions fragment %s', fileName)
    with open('{}'.format(fileName),'w') as f:
       f.write('# Auto generated jobOptions fragment to setup Athena VTune profiler')
       f.write('\ninclude(\'PerfMonVTune/VTuneProfileEventLoop_preInclude.py\')')
       f.write('\n\nfrom AthenaCommon.AppMgr import ServiceMgr')
       f.write('\nServiceMgr.VTuneProfilerService.ResumeEvent = {}'.format(firstEvent))
       f.write('\nServiceMgr.VTuneProfilerService.PauseEvent  = {}'.format(lastEvent))

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

    parser.add_option('--collect',
                      type='choice',
                      action='store',
                      dest='collect',
                      choices=['hotspots','threading','memory-consumption',
                               'hpc-performance','memory-access','cpugpu-concurrency',
                               'gpu-hotspots','gpu-profiling','graphics-rendering',
                               'fpga-interaction','io','system-overview'],
                      default='hotspots',
                      help='Run the specified analysis type and collect data into a result (default: hotspots)')

    parser.add_option('--strategy',
                      type='string',
                      dest='strategy',
                      default=':trace:trace',
                      help='Run the specified strategy and collect data into a result. (default: :trace:trace)')

    parser.add_option('--tf',
                      type='string',
                      dest='tf',
                      default='',
                      help='Run the specified transformation command and collect data into a result.')

    parser.add_option('--resumeEvent',
                      type='int',
                      dest='start',
                      default=0,
                      help='Start the profiling from the specified event (default: 0).')

    parser.add_option('--pauseEvent',
                      type='int',
                      dest='stop',
                      default=-1,
                      help='Start the profiling at the specified event (default: -1).')

    (options,args) = parser.parse_args()

    logger.info(76*'-')

    # Set logging level
    logger.setLevel(options.logLevel.upper())

    # Check the Athena and VTune setups
    checkAthenaSetup()
    checkVTuneSetup()

    # Perpare the JO fragment
    joFragment = 'PerfMonVTune_autoSetup.py'
    generateJOFragment(joFragment, options.start, options.stop)

    # Prepare the transformation command to execute
    if not options.tf:
        logger.fatal('The transformation command is empty, quitting...')
        sys.exit(-1)

    args = options.tf.split()
    if 'preInclude' in args:
        index = args.index('--preInclude')
        args.insert(index+1,joFragment)
    else:
        args.extend(['--preInclude',joFragment])

    # Run the command
    cmd = ( 'vtune' +
            ' -collect '  + options.collect  +
            ' -strategy ' + options.strategy +
            ' -start-paused -- ' )
    cmd += ' '.join(args)

    logger.info('Running the full command "{}"'.format(cmd))
    subprocess.call(cmd,shell=True)

    logger.info('All done...')
    logger.info(76*'-')

if __name__ == '__main__':
    main()
