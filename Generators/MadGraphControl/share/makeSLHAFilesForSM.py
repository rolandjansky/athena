#!/bin/env python

import os,shutil,sys,glob

# Grab the run list
listOfRuns=[]
if len(sys.argv)>0:
    for a in sys.argv[1:]: 
        if not '.py' in a: listOfRuns+=sys.argv[1].split(',')
if len(listOfRuns)==0:
    print 'Usage: makeSLHAFilesForSM.py run1 run2 run3,run4,run5 run6...'
    sys.exit(0)
print 'Getting SLHA files for run(s)',listOfRuns

# Fake some things that we won't need later
from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.randomSeed = 1234
from AthenaCommon import Logging
evgenLog = Logging.logging.getLogger('SLHAGetter')

# only really need this once
from AthenaCommon.Include import IncludeError, include
include.setShowIncludes(False)
include('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/susycontrol/MadGraphControl_SimplifiedModelPreInclude.py')
include.block('MC15JobOptions/MadGraphControl_SimplifiedModelPreInclude.py')
include.block('MC15JobOptions/MadGraphControl_SimplifiedModelPostInclude.py')

for run in listOfRuns:
    loc_l = glob.glob('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID'+run[0:3]+'xxx/MC15.'+run+'.*')
    if 0==len(loc_l):
        print 'Run not found:',run,'in','/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID'+run[0:3]+'xxx/MC15.'+run+'.*'
        continue
    if len(loc_l)>1:
        print 'Multiple runs found:',loc_l,'for run',run,'- Using first.'
    loc = loc_l[0]

    jobO = open(loc,'r')
    main_jobO = ''
    for line in jobO:
        if 'include' in line and 'MadGraphControl' in line: 
            # This is pretty clumsy.  Better to split on () and exec() out the string
            if '"' in line: main_jobO=line.split('"')[1]
            elif "'" in line: main_jobO=line.split("'")[1]
    print 'For run',run,'located main JO',main_jobO
    jobO.close()

    # This is not the prettiest, but it works
    try:
        runArgs.runNumber=int(run)
        runNumber=int(run) # Protection against old code
        include('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/susycontrol/'+(main_jobO.split('/')[-1]))
        SLHAonly=True
        #include('/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/susycontrol/MadGraphControl_SimplifiedModelPostInclude.py')
        include('./MadGraphControl_SimplifiedModelPostInclude.py')
    except: pass

    # Move the modified card to where we want it
    if not os.access('param_card.dat',os.R_OK):
        print 'Failed to get parameter card for run',run
        continue
    shutil.move('param_card.dat','susy_'+run+'.slha')

    # Clean up old cards
    if os.access('LH.dat',os.R_OK):         os.remove('LH.dat')
    for old_card in glob.glob('param_card*dat'): os.remove(old_card)
print 'All done - goodbye.'
