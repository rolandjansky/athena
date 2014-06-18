#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os,sys,glob,re

def confirm(prompt=None, resp=False):
    """prompts for yes or no response from the user. Returns True for yes and
    False for no.

    'resp' should be set to the default value assumed by the caller when
    user simply types ENTER.

    >>> confirm(prompt='Create Directory?', resp=True)
    Create Directory? [y]|n: 
    True
    >>> confirm(prompt='Create Directory?', resp=False)
    Create Directory? [n]|y: 
    False
    >>> confirm(prompt='Create Directory?', resp=False)
    Create Directory? [n]|y: y
    True

    """
    
    if prompt is None:
        prompt = 'Confirm'

    if resp:
        prompt = '%s [%s]|%s: ' % (prompt, 'y', 'n')
    else:
        prompt = '%s [%s]|%s: ' % (prompt, 'n', 'y')
        
    while True:
        ans = raw_input(prompt)
        if not ans:
            return resp
        if ans not in ['y', 'Y', 'n', 'N']:
            print 'please enter y or n.'
            continue
        if ans == 'y' or ans == 'Y':
            return True
        if ans == 'n' or ans == 'N':
            return False


commands=[]

if len(sys.argv)<2:
    print "usage: makeBankContainerAnnovi.py DScommon_name"
    sys.exit()

dsbase=re.sub("Region[0-9]*to[0-9]*_","Region*_",sys.argv[1]);
dsbase=re.sub("/$","",dsbase);
dsbase=re.sub("$","*.*",dsbase);
contDS=re.sub("Region[0-9]*to[0-9]*_","",sys.argv[1]);
contDS=re.sub("\.201.*","",contDS);
contDS=re.sub("$",os.popen("date +.%Y%m%d_%H%M%S/").read(),contDS);
contDS=contDS.rstrip('\n').rstrip('\r');
#contDS=re.sub("^group.trig-daq.","user.annovi.",contDS);

print "dsbase=",dsbase
print "contDS=",contDS

#where contDS is the new container dataset name that I have chosen 
#dsbase is the common part of the output name for the stage I'm trying to make the container DS for.
#dsbase="user.annovi.raw_8Lc_22x16x36_40Mx40loops_Region*_40NLoops_unmerged.2013_09_06_12_*.*"
#contDS="user.annovi.raw_8Lc_22x16x36_40Mx30loops_Region0and16_30NLoops_unmerged.2013_09_06_12_24_00/"

os.system("dq2-ls %s >& ds.txt" %dsbase)
listOfds=open("ds.txt")
for line in listOfds:
    print "adding DS: ", line, #" to CONTAINER: ", contDS,

print
if confirm("Is it ok to proceed?", True):
    os.system("dq2-register-container %s" %contDS)
    listOfds=open("ds.txt")
    for line in listOfds:
        print "adding DS: ", line, # " to CONTAINER: ", contDS,
        os.system("dq2-register-datasets-container %s %s" %(contDS,line))



