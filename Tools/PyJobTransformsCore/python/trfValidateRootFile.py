#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# @Package PyJobTransforms.trfValidateRootFile
# @brief Functionality to test a Root file for corruption
# @author bjorn.sarrazin@cern.ch

from __future__ import print_function



import sys


from PyUtils import RootUtils
ROOT = RootUtils.import_root()
from ROOT import TFile, TTree, TDirectory, TStopwatch, TDirectoryFile



def checkBranch(branch, msg):

    msg.debug('Checking branch %s...', branch.GetName())

    nBaskets=branch.GetWriteBasket()

    msg.debug('Checking %s baskets...', nBaskets)

    for iBasket in range(nBaskets):
        basket=branch.GetBasket(iBasket)
        if not basket:
            msg.warning('Basket %s of branch %s is corrupted.', iBasket, branch.GetName())
            return 1

    listOfSubBranches=branch.GetListOfBranches()
    msg.debug('Checking %s subbranches...', listOfSubBranches.GetEntries())
    for subBranch in listOfSubBranches:
        if checkBranch(subBranch,msg)==1:
            return 1

    msg.debug('Branch %s looks ok.', branch.GetName())
    return 0    


def checkTreeBasketWise(tree, msg):

    listOfBranches=tree.GetListOfBranches()

    msg.debug('Checking %s branches ...', listOfBranches.GetEntries())

    for branch in listOfBranches:
        if checkBranch(branch,msg)==1:
            msg.warning('Tree %s is corrupted (branch %s ).', tree.GetName(), branch.GetName())
            return 1

    return 0


def checkTreeEventWise(tree, msg):

    nEntries=tree.GetEntries()

    msg.debug('Checking %s entries...', nEntries)

    for i in range(nEntries):
        if tree.GetEntry(i)<0:
            msg.warning('Event %s of tree %s is corrupted.', i, tree.GetName())
            return 1

    return 0


def checkDirectory(directory, type, requireTree, msg):

    msg.debug('Checking directory %s...', directory.GetName())

    listOfKeys=directory.GetListOfKeys()

    msg.debug('Checking %s keys... ', listOfKeys.GetEntries())

    for key in listOfKeys:

        msg.debug('Looking at key %s...', key.GetName())
        msg.debug('Key is of class %s.', key.GetClassName())

        object=directory.Get(key.GetName())
        if not object:
            msg.warning("Can't get object of key %s.", key.GetName())
            return 1

        if ( object.GetName().find('Meta') > -1 ) and isinstance(object,TDirectoryFile):
            msg.warning("Will ignore Meta TDirectoryFile %s!", object.GetName() )
            continue

        if requireTree and not isinstance(object, TTree):
            msg.warning("Object %s is not of class TTree!", object.GetName())
            return 1

        if isinstance(object,TTree):

            msg.debug('Checking tree %s ...', object.GetName())
            
            if type=='event':
                if checkTreeEventWise(object, msg)==1:
                    return 1
            elif type=='basket':    
                if checkTreeBasketWise(object, msg)==1:
                    return 1

            msg.debug('Tree %s looks ok.', object.GetName())
            
        if isinstance(object, TDirectory):
            if checkDirectory(object, type, requireTree, msg)==1:
                return 1

    msg.debug('Directory %s looks ok.', directory.GetName())
    return 0


def checkFile(fileName, type, requireTree, msg):

    msg.info('Checking file %s.', fileName)

    file=TFile.Open(fileName)

    if not file:
        msg.warning("Can't access file %s.", fileName)
        return 1

    if not file.IsOpen():
        msg.warning("Can't open file %s.", fileName)
        return 1

    if file.IsZombie():
        msg.warning("File %s is a zombie.", fileName)
        file.Close()
        return 1

    if file.TestBit(TFile.kRecovered):
        msg.warning("File %s needed to be recovered.", fileName)
        file.Close()
        return 1

    if checkDirectory(file, type, requireTree, msg)==1:
        msg.warning("File %s is corrupted.", fileName)
        file.Close()
        return 1

    file.Close()
    msg.info("File %s looks ok.", fileName)
    return 0


def usage():
    print ("Usage: validate filename type requireTree verbosity")
    print ("'type'  must be either 'event' or 'basket'")
    print ("'requireTree' must be either 'true' or 'false'")
    print ("'verbosity' must be either 'on' or 'off'")

    return 2


def main(argv):

    import logging
    msg = logging.getLogger(__name__)
    ch=logging.StreamHandler(sys.stdout)
    #    ch.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    msg.addHandler(ch)


    clock=TStopwatch()
    
    argc=len(argv)

    if (argc!=5):
        return usage()

    fileName=argv[1]
    type=argv[2]
    tree=argv[3]
    verbosity=argv[4]


    if type!="event" and type!="basket":
        return usage()

    if tree=="true":
        requireTree=True
    elif tree=="false":
        requireTree=False
    else:    
        return usage()

    if verbosity=="on":
        msg.setLevel(logging.DEBUG)
    elif verbosity=="off":
        msg.setLevel(logging.INFO)
    else:
        return usage()
  
    rc=checkFile(fileName,type, requireTree, msg)
    msg.debug('Returning %s', rc)
    
    clock.Stop()
    clock.Print()

    return rc

    
if __name__ == '__main__':                


    rc=main(sys.argv)
    sys.exit(rc)
    
