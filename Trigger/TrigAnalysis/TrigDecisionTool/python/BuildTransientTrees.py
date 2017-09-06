# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
import AthenaROOTAccess.transientTree


__PersistentTrees_for_TRigDecisionTool = []


# to keep transient trees always in scope

def __printDatHandle(dhe):
    return dhe.token().strip('()/')+'#'+dhe.key()

def filtBasic(dhe):
    """ Filter to load EDM sufficeient to answer ONLY questions about chains/items passed state
    """
    if 'HLTResult' in dhe.token() or 'TrigDecision' in dhe.token() or 'EventInfo' in dhe.token():
        #print '+ ', __printDatHandle(dhe)
        return dhe
    #print '- ', __printDatHandle(dhe)
    return False

def filtEDMAccess(dhe):
    """ Filter to load EDM sufficient to answer all questions (get objects) related to HLT and L1
    """
    if 'HLT' in dhe.key() \
           or 'TrigDecision' in dhe.token()\
           or 'EventInfo' in dhe.token()\
           or 'LVL1_ROI' in dhe.token():
        #print '+ ', __printDatHandle(dhe)
        return dhe
    #print '- ', __printDatHandle(dhe)
    return False

# builds the transient event collection and metadata trees
# input is a list of athena pool files
def BuildTransientTrees(PoolFileList, dhfilt=None):


    CollectionTree = ROOT.AthenaROOTAccess.TChainROOTAccess('CollectionTree')
    MetaDataTree   = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')

    # remember them so they do not go out of scope
    global __PersistentTrees_for_TRigDecisionTool
    __PersistentTrees_for_TRigDecisionTool += [CollectionTree]
    __PersistentTrees_for_TRigDecisionTool += [MetaDataTree]
    
    for f in PoolFileList:
        CollectionTree.Add(f)
        MetaDataTree.Add(f)
    transientCollectionTree = AthenaROOTAccess.transientTree.makeTree(CollectionTree, dhfilt=dhfilt)
    try:
        transientMetaDataTree   = AthenaROOTAccess.transientTree.makeTree(MetaDataTree,
                                                                          persTreeName = 'MetaData',
                                                                          dhTreeName = 'MetaDataHdr')
        # backward compatibility for old data:
    except AttributeError:
        transientMetaDataTree   = AthenaROOTAccess.transientTree.makeTree(MetaDataTree,
                                                                          persTreeName = 'MetaData',
                                                                          dhTreeName = 'MetaDataHdrDataHeader')

    return (transientCollectionTree, transientMetaDataTree)


class cleaner:
    def __init__(self, tcoll, tmd, pcoll, pmd):
        self.tcoll = tcoll
        self.tmd = tmd
        self.pcoll = pcoll
        self.pmd = pmd

    def deleteTransient(self):
        self.tcoll.Delete()
        self.tcoll = None
        self.tmd.Delete()
        self.tmd = None

    # calling this is not needed, the last files of a chain are closed when CollectionTree and MetaDataTree go out of scope
    #def closeFiles(self):
    #    fn = set([f.GetTitle() for f in self.pcoll.GetListOfFiles()] + [f.GetTitle() for f in self.pmd.GetListOfFiles()])
    #    for f in fn:
    #        for fh in ROOT.gROOT.GetListOfFiles():
    #            if fh.GetName()==f:
    #                fh.Close() # don't break here since multiple instances of the same file are open


# builds the transient event collection and metadata trees
# input is a list of athena pool files
def BuildTransientTrees2(PoolFileList, dhfilt=None):

    if not hasattr(PoolFileList,"__iter__"):
        PoolFileList = [PoolFileList]

    CollectionTree = ROOT.AthenaROOTAccess.TChainROOTAccess('CollectionTree')
    MetaDataTree   = ROOT.AthenaROOTAccess.TChainROOTAccess('MetaData')

    # remember them so they do not go out of scope (not needed anymore, since we keep an instance in the cleaner object)
    #global __PersistentTrees_for_TRigDecisionTool
    #__PersistentTrees_for_TRigDecisionTool += [CollectionTree]
    #__PersistentTrees_for_TRigDecisionTool += [MetaDataTree]
    
    for f in PoolFileList:
        CollectionTree.Add(f)
        MetaDataTree.Add(f)
    transientCollectionTree = AthenaROOTAccess.transientTree.makeTree(CollectionTree, dhfilt=dhfilt)
    transientMetaDataTree   = AthenaROOTAccess.transientTree.makeTree(MetaDataTree,
                                                                      persTreeName = 'MetaData',
                                                                      dhTreeName = 'MetaDataHdrDataHeader')

    cl = cleaner(transientCollectionTree, transientMetaDataTree,
                 CollectionTree, MetaDataTree)

    return (transientCollectionTree, transientMetaDataTree, cl)
