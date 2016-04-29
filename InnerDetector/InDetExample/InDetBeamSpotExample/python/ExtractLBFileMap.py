# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
import user
import sys, traceback
import os, re,commands,fnmatch
from glob import glob
# Turn off X11 requirement in ROOT
sys.argv = sys.argv[:1] + ['-b'] + sys.argv[1:]
from PyUtils.Helpers import ShutUp
import ROOT
rootMsg = ShutUp()
rootMsg.mute()
# Turn off X11 requirement in ROOT
sys.argv.remove('-b')
from PyCool import coral
from time import ctime
import PyCintex
import AthenaROOTAccess.transientTree
rootMsg.unMute()

from InDetBeamSpotExample import DiskUtils


def getLBs(tt, name):
    # Get helper for dumping EventStreamInfo
    import AthenaPython.PyAthena as PyAthena
    esi1 = PyAthena.EventStreamInfo

    # Get EventStreamInfo
    br  = tt.GetBranch( name )
    if br == None: print "Could not find branch for ",name; return None
    try:
        if br.GetEntry( 0 ) <= 0:
            print ":: Could not get meta data for first event for branch", anem
        esi = getattr(tt, name)
        if esi == None:
            print "Could not get object for EventStreamInfo: ", name
        else:
            return esi.lumi_blocks()
    
    except Exception, e:
        print "## Caught exception [%s] !!" % str(e.__class__)
        print "## What:",e
        import sys
        print sys.exc_info()[0]
        print sys.exc_info()[1]
        sc = 1

def extract(dirs, filter, prefix = True):
    # Get list of file names for dir
    fileNames = []
    for arg in dirs:
        # Handle castor
        if os.path.isdir(arg) or "castor" in arg or "eos" in arg:
            if arg.split('/')[1] in ('castor', 'eos'):
                fileNames.extend(DiskUtils.filelist('%s/*%s*' % (arg,filter), prefix=prefix))
            elif filter == 'dpd':
                fileNames.extend(glob('%s/*/*-dpd.root' %(arg)))
            else:
                if os.path.isdir(arg):
                    fileNames.extend(glob('%s/*%s*' % (arg, filter)))
                else:
                    print 'ERROR: arguments must be directories'
                    sys.exit(1)
        else:
            for line in open(arg,'r'):
                fileNames.append('root://eosatlas/'+line.rstrip())

    fileNames = set( fileNames )
    return extractFromFiles(fileNames)

def extractFromFiles(fileNames):
    sc = 0
    lbDict = {}
    # Loop over files
    for fileName in fileNames:
        try:
            f = ROOT.TFile.Open (fileName)
            assert f.IsOpen()

            isNewDataHdr = True
            tree = f.Get ('MetaDataHdr')

            if not isinstance(tree, ROOT.TTree):
                isNewDataHdr = False
                tree = f.Get ('MetaDataHdrDataHeader')
                if not isinstance(tree, ROOT.TTree):
                    print "File does NOT contain meta data Tree"
                    continue

            # get the DataHeader_pX name
            if isNewDataHdr:
                branches = set([ 'DataHeader' ])
            else:    
                branches = set([ br.GetName() for br in tree.GetListOfBranches()
                                 if re.match(r'DataHeader_p.*?', br.GetName()) ])

            if len(branches) <= 0:
                raise RuntimeError("Could not find a persistent DataHeader in MetaDataHdrDataHeader tree")
            if len(branches) > 1:
                print "*** Warning *** Too many DataHeader_pX:",branches

            dh_name = branches.pop()

            # Fill this in if you want to change the names of the transient branches.
            branchNames = {}
            if isNewDataHdr:
                tt = AthenaROOTAccess.transientTree.makeTree(f, persTreeName='MetaData',
                                                             dhTreeName='MetaDataHdr',
                                                             dhBranchName=dh_name,
                                                             branchNames = branchNames)
            else:
                tt = AthenaROOTAccess.transientTree.makeTree(f, persTreeName='MetaData',
                                                             dhTreeName='MetaDataHdrDataHeader',
                                                             dhBranchName=dh_name,
                                                             branchNames = branchNames)

            try:
                brs = tt.GetListOfBranches()
                for br in brs:
                    if not br.GetObjClassName() == 'EventStreamInfo': continue

                    # Find LBs in a given file
                    for lb in getLBs(tt, br.GetName()):
                        fname = fileName.split('/')[-1]                            
                        try:
                            lbDict[fname].append(lb) 
                        except KeyError:
                            lbDict[fname] = [lb]

            except Exception, e:
                print "## Caught exception [%s] !!" % str(e.__class__)
                print "## What:",e
                import sys
                print sys.exc_info()[0]
                print sys.exc_info()[1]
                sc = 1
                pass

            if not "eos" in fileName:
                f.Close()
        
        except Exception, e:
            print "## Caught exception [%s] !!" % str(e.__class__)
            print "## What:",e
            import sys
            print sys.exc_info()[0]
            print sys.exc_info()[1]
            sc = 1
            pass

    return lbDict
