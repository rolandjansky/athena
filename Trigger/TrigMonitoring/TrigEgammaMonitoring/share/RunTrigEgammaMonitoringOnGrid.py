#!/usr/bin/env python
#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#


import argparse
import os
from AthenaCommon.Logging import logging
mainLogger = logging.getLogger( 'RunTrigEgammaMonitoringOnGrid' )



parser = argparse.ArgumentParser(description = '', add_help = False)
parser = argparse.ArgumentParser()


#
# prun configuration
#

parser.add_argument('--inDS', action='store', dest='inDS', required = True, type=str, help = "Input dataset.")
parser.add_argument('--outDS', action='store', dest='outDS', required = True, type=str, help = "Output dataset.")

parser.add_argument('--dry_run', action='store_true', dest='dry_run', required = False, default = False,
                    help = "Do not launch if this is True.")

parser.add_argument('--nFilesPerJob', action='store', dest='nFilesPerJob', required = False, type=int, default=1,
                    help = "Number of files per job in grid mode.")

parser.add_argument('--job', action='store', dest='job', required = False, type=str, default='RunTrigEgammaMonitoring.py',
                    help = "Job name")

parser.add_argument('-a', '--asetup', action='store', dest='asetup', required = False, type=str, default='asetup Athena,master,latest,here',
                    help = "asetup release")

#
# Job Command
#

parser.add_argument('--preExec', help='Code to execute before locking configs', default=None)
parser.add_argument('--postExec', help='Code to execute after setup', default=None)
parser.add_argument('--emulate', help='emulate the HLT e/g sequence', action='store_true')

parser.add_argument('--pidConfigPath', action='store', required = False, 
                    type=str, default='ElectronPhotonSelectorTools/trigger/rel22_20210611',
                    help = "Electron Likelihood and Photon CB config path to use in emulated precision step.")
parser.add_argument('--dnnConfigPath', action='store', required = False, 
                    type=str, default='ElectronPhotonSelectorTools/offline/mc16_20210430',
                    help = "Electron DNN config path to use in emulated precision step.")
parser.add_argument('--ringerConfigPath', action='store', required = False, 
                    type=str, default='RingerSelectorTools/TrigL2_20210702_r4',
                    help = "Electron ringer config path to use in emulated fast calo step.")


import sys,os
if len(sys.argv)==1:
  parser.print_help()
  sys.exit(1)


args = parser.parse_args()

execute = "{JOB} -i '%IN'".format(JOB=args.job)
if args.emulate:
    execute+= ' --emulate'
    execute+= " --pidConfigPath '{PATH}'".format(PATH=args.pidConfigPath)
    execute+= " --dnnConfigPath '{PATH}'".format(PATH=args.dnnConfigPath)
    execute+= " --ringerConfigPath '{PATH}'".format(PATH=args.ringerConfigPath)

if args.preExec:
    execute+= ' --preExec '+args.preExec
if args.postExec:
    execute+= ' --postExec '+ args.postExec 



command = """ prun --exec \
       "{COMMAND};" \
     --noBuild \
     --inDS={INDS} \
     --outDS={OUTDS} \
     --disableAutoRetry \
     --outputs="*.root" \
     --nFilesPerJob={N_FILES_PER_JOB} \
     --mergeOutput \
     --useAthenaPackage \
    """

command = command.format( 
                            INDS      = args.inDS,
                            OUTDS     = args.outDS,
                            COMMAND   = execute,
                            N_FILES_PER_JOB = args.nFilesPerJob,
                            )


if args.dry_run:
    print(command)
else:
    print(command)
    os.system(command)









