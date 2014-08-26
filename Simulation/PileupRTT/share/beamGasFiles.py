#######################################################################################################################
## This is a list of existing collections of beam gas events with geometry 01-02-00. The proportions
## (90% H (2470), 7% C (192), 3% O (82)) have been derived from cross-section and gas-density calculations
## (LHC Report 783, A. Rossi) for the anticipated beam gas in the ATLAS experiment.
##
## There is a structure to these files - the simulation was run in chunks of 200 events from a 5000 event
## primary file. Alternating even and odd generation files were run in alternating beam directions - so
## balance in chunks of 25 files is important. As of now, I'm not sure which beam Hijing thinks of as default,
## (It seems to be beam 1), so I designate unflipped events as "+" and flipped ones as "-"
##
## As of this date (30 Nov 2007), there are missing and/or probably corrupt files in the datasets. These
## are double-commented. (## ##)
## Files excluded to provide the correct proportions of H, O and C are single-commented. (##)
##
## Alden Stradling, 30 Nov 2007
##
## Modified heavily, 20 Dec 2007 - Randomization and corrected proportions, and auto-update from CASTOR
######################################################################################################################

import os, commands
from random import randint

def randomize(l):
    ''' Randomizes a list by swapping its members '''
    for i in range(len(l)*10):
        r,rr=randint(0,len(l)-1),randint(0,len(l)-1)
        l[r],l[rr]=l[rr],l[r]
    return l

def setMaker(fdir):
    cmd='nsls -l %s |grep -v " 0 "|awk \'{print $9}\'' % (fdir)
    base = ['rfio:%s/%s' % (fdir,i) for i in commands.getoutput(cmd).split('\n')]
    a,c=sideChooser(base)
    return base,a,c
    

def sideChooser(l):
    ''' Selects the appropriate cavern side for each file '''
    A=[]
    C=[]
    for i in l:
        if int(i.split('._')[1].split('.')[0]) % 100 / 25 % 2: 
            A.append(i)
        else:
            C.append(i)

    return A,C

path='/eos/user/s/stradlin/BeamGas_1.1/simul/'

bgBaseH,bgCollH_A,bgCollH_C = setMaker(path+'hijing.H.simul')
bgBaseC,bgCollC_A,bgCollC_C = setMaker(path+'hijing.C.simul')
bgBaseO,bgCollO_A,bgCollO_C = setMaker(path+'hijing.O.simul')

#Basic file proportions / 2, allowing for both sides
propH,propC,propO=1235,96,41

beamGasCollections=randomize(bgCollH_A)[:propH]+randomize(bgCollH_C)[:propH]
beamGasCollections+=randomize(bgCollC_A)[:propC]+randomize(bgCollC_C)[:propC]
beamGasCollections+=randomize(bgCollO_A)[:propO]+randomize(bgCollO_C)[:propO]
beamGasCollections=randomize(beamGasCollections)


