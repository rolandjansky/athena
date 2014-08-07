#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import sys
from PATJobTransforms.ArgDicTools import BuildDicFromCommandLine

def HandleDBRelease(DBRel):
    print "***Note: using DBRelease is not mandatory for tests on afs and copies large files on your run directory, you might consider NOT using it."
    if DBRel.count('.')>2:
        DBRel2 = DBRel[0]+DBRel[1]+DBRel[2]+DBRel[3]+DBRel[4]
        print "WARNING original DB version: %s is not available on afs! Version %s is available."%(DBRel,DBRel2)
        DBRel = DBRel2
    print '   If you still want to use it, you can modify your command to use:   DBRelease=/afs/cern.ch/atlas/www/GROUPS/DATABASE/pacman4/DBRelease/DBRelease-'+DBRel+'.tar.gz\n'



def GetKeysAndValues(outDic):
    keyAndValue=''
    for key, value in outDic.iteritems():
        # Put quotes around all values to make them shell safe
        keyAndValue += key + '=' + "'" + str(value).replace("'", "'\\''") + "'" + ' '
    return keyAndValue


def GetCommand(sysArgv):
    cmdList=BuildDicFromCommandLine(sysArgv,returnList=True)
    print cmdList
    if cmdList==[]:
        print "\nGetCommand finds nothing, probably because you didn't specify an AMI tag."
        print "Please use syntax:  GetCommand.py AMI=<tag>"
        return

    i=1
    for cmd in cmdList:
        trf=cmd['info']['amiTransform']
        if trf=='Reco_trf' or trf=='/afs/cern.ch/atlas/project/tzero/prod1/projects/data08_cos/trfs/Recon.v5.py, based on Reco_trf.py':
            trf='Reco_trf.py'

        if trf=='csc_digi_trf.py':
            trf='Digi_trf.py'

        if trf=='csc_atlasG4_trf.py':
            trf='AtlasG4_trf.py'

        outDic=cmd['outDic']
        keysAndValues=GetKeysAndValues(outDic)
        print "\n\n###################################################################"
        print "This is command #%i:\n"%i
        print trf,keysAndValues
        print "\n"
        if outDic.has_key('DBRelease'): HandleDBRelease(outDic['DBRelease'])            
        i+=1
        

    print "***Note: input and output file names are just suggestions, you're expected to change them as you need.***\n"
    if trf == 'DigiMReco_trf.py':
        print "***DigiMReco_trf note: The minbias HITS files used must match the conditions ***"
        print "*** for the main HITS file. Here the minbias 'files' refer to the datasets   ***"
        print "*** used by this tag, not to specific files.                                 ***\n"

if __name__ == '__main__':
    GetCommand(sys.argv)

        
