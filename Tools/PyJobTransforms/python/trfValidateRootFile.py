#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @Package PyJobTransforms.trfValidateRootFile
# @brief Functionality to test a Root file for corruption
# @author atlas-comp-transforms-dev@cern.ch
# @todo The main() CLI should migrate to @c scripts and this module just implement functions



import sys
import logging

from PyUtils import RootUtils
ROOT = RootUtils.import_root()
from ROOT import TFile, TTree, TKey, TDirectory, TClass, TList, TObjArray, TStopwatch, TBasket

msg = logging.getLogger(__name__)

def checkBranch(branch):

    msg.debug('Checking branch %s...' % branch.GetName())

    nBaskets=branch.GetWriteBasket();

    msg.debug('Checking %s baskets...' % nBaskets)

    for iBasket in range(nBaskets):
        basket=branch.GetBasket(iBasket);
        if not basket:
            msg.warning('Basket %s of branch %s is corrupted.' % (iBasket, branch.GetName() ))
            return 1

    listOfSubBranches=branch.GetListOfBranches();
    msg.debug('Checking %s subbranches...' % listOfSubBranches.GetEntries())
    for subBranch in listOfSubBranches:
        if checkBranch(subBranch)==1:
            return 1;

    msg.debug('Branch %s looks ok.' % branch.GetName())
    return 0    


def checkTreeBasketWise(tree):

    listOfBranches=tree.GetListOfBranches()

    msg.debug('Checking %s branches ...' % listOfBranches.GetEntries())

    for branch in listOfBranches:
        if checkBranch(branch)==1:
            msg.warning('Tree %s is corrupted (branch %s ).' % (tree.GetName(), branch.GetName()))
            return 1

    return 0


def checkTreeEventWise(tree):

    nEntries=tree.GetEntries()

    msg.debug('Checking %s entries...' % nEntries)

    for i in range(nEntries):
        if tree.GetEntry(i)<0:
            msg.warning('Event %s of tree %s is corrupted.' % (i, tree.GetName()))
            return 1

    return 0


def checkDirectory(directory, type, requireTree):

    msg.debug('Checking directory %s...' % directory.GetName())

    listOfKeys=directory.GetListOfKeys()

    msg.debug('Checking %s keys... ' % listOfKeys.GetEntries())

    for key in listOfKeys:

        msg.debug('Looking at key %s...' % key.GetName())
        msg.debug('Key is of class %s.' % key.GetClassName())

        object=directory.Get(key.GetName())
        if not object:
            msg.warning("Can't get object of key %s." % key.GetName())
            return 1

        if requireTree and not isinstance(object, TTree):
            msg.warning("Object %s is not of class TTree!" % object.GetName())
            return 1

        if isinstance(object,TTree):

            msg.debug('Checking tree %s ...' % object.GetName())
            
            if type=='event':
                if checkTreeEventWise(object)==1:
                    return 1
            elif type=='basket':    
                if checkTreeBasketWise(object)==1:
                    return 1

            msg.debug('Tree %s looks ok.' % object.GetName())    
            
        if isinstance(object, TDirectory):
            if checkDirectory(object, type, requireTree)==1:
                return 1

    msg.debug('Directory %s looks ok.' % directory.GetName())
    return 0


def checkFile(fileName, type, requireTree):

    msg.info('Checking file %s.' % fileName)

    file=TFile.Open(fileName)

    if not file:
        msg.warning("Can't access file %s." % fileName)
        return 1

    if not file.IsOpen():
        msg.warning("Can't open file %s." % fileName)
        return 1

    if file.IsZombie():
        msg.warning("File %s is a zombie." % fileName)
        file.Close()
        return 1

    if file.TestBit(TFile.kRecovered):
        msg.warning("File %s needed to be recovered." % fileName)
        file.Close()
        return 1

    if checkDirectory(file, type, requireTree)==1:
        msg.warning("File %s is corrupted." % fileName)
        file.Close()
        return 1

    file.Close();
    msg.info("File %s looks ok." % fileName)
    return 0


def usage():
    print "Usage: validate filename type requireTree verbosity"
    print "'type'  must be either 'event' or 'basket'"
    print "'requireTree' must be either 'true' or 'false'"
    print "'verbosity' must be either 'on' or 'off'"

    return 2


def main(argv):

    clock=TStopwatch()
    
    argc=len(argv)

    if (argc!=5):
        return usage()

    fileName=argv[1]
    type=argv[2]
    requireTree=argv[3]
    verbosity=argv[4]


    if type!="event" and type!="basket":
        return usage()

    if requireTree!="true" and requireTree!="false":
        return usage()

    if verbosity=="on":
        msg.setLevel(logging.DEBUG)
    elif verbosity=="off":
        msg.setLevel(logging.INFO)
    else:
        return usage()
  
    rc=checkFile(fileName,type, requireTree)
    msg.debug('Returning %s' % rc)
    
    clock.Stop();
    clock.Print();

    return rc

    
if __name__ == '__main__':                

    ch=logging.StreamHandler(sys.stdout)
#    ch.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    msg.addHandler(ch)

    rc=main(sys.argv)
    sys.exit(rc)
    
