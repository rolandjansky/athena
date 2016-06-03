#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##################################################
### CompareJetTestFileExec
###
###  *** BEING DEPRECATED *** 
###   the code in this file has been recopied in JetTestCheck.py in the __main__ part of it.
###
###  *** BEING DEPRECATED *** 
### An executable to compare the jet content of 2 xAOD files.
###
### Usage :
### python CompareJetTestFilesExec.py xaod1.root xaod2.root
###
### python CompareJetTestFilesExec.py -h to get the list of options
###
###
### Currently the files compare only the AntiKt4LCTopoJets and AntiKt10LCTopoJets
### containers.
###
#################################################

import sys
import argparse

testedContainers = ["AntiKt4LCTopoJets","AntiKt10LCTopoJets"]
##################################################
# Read and store argument values                 #
##################################################

parser = argparse.ArgumentParser(description='Create distributions for many variables and save them in a ROT file')

parser.add_argument('inFile1', type=str, help='1st input root file')
parser.add_argument('inFile2', type=str, default="", nargs='?', help='2nd input root file. If none, then container "XYZ" in inFile1 will be compared against "XYZSuffix" where Suffix is set by --suffix')
parser.add_argument('--treeName1', type=str, default="CollectionTree", help="Name of TTree (default: %(default)s)", metavar='name1')
parser.add_argument('--treeName2', type=str, default="CollectionTree", help="Name of TTree", metavar='name2')
parser.add_argument( '--containers', type=str, default= testedContainers, nargs='*',help="list of containers (ex: --containers cont1 cont2 cont3)", metavar='jetcontname')
parser.add_argument('--details', action='store_true', help="Display status for each variable, even if no error")
parser.add_argument('--ESD', action='store_true', help="Use this flag if both input files are ESD")
parser.add_argument('--AODvsESD', action='store_true', help="Use this flag if file1 is AOD and file2 is  ESD")
parser.add_argument('--suffix', type=str, default='Test', help="In case using only 1 input files, uses this suffix to relate containers to be matched. Default to 'Test'")
parser.add_argument('--clusters', action='store_true', help="Use this flag if file1 is AOD and file2 is  ESD")


#parser.add_argument('-exV', '--excludedVar', type=str, default=[], nargs='*')
#parser.add_argument('-t1', '--type1', type=str, default="xAOD", nargs='?')
#parser.add_argument('--', type=str, default="")
#parser.add_argument('-b' , '--saveBinning',  action='store_true') 
#parser.add_argument('-pdf' , '--savePDF',  action='store_true')
#parser.add_argument('-cal' , '--calibScale',  action='store_false')


args = parser.parse_args()

# reset testedContainers :
testedContainers = args.containers
print "Will test ", testedContainers

from JetValidation.JetTestCheck import *

# if we have only 1 input file, we compare "XXXJets" against "XXXJetsTest" containers
if args.inFile2 == '':
    # define a renamer function
    branchRenamer = lambda n : n.replace('Jets','Jets'+args.suffix)    
    # testing only the 2 first entries
    VarPairVector.maxVecSize = 2
    # do this as long as area calculation is not reproducible :
    # in this case calib differs and misordering can happen for low pt jets
else:
    # we compare containers with same name in 2 different files.
    # No need to rename, renamer function does nothing :
    branchRenamer = lambda n : n


# Filter --------------
def branchFilter(s ):
    """Return True if branch named s needs to be compared """
    return any( s.startswith( b ) for b in testedContainers ) #and "Area" not in s

# Name splitting ----------
# In order to nicely categorize results, the system needs to know to which container
# a variable "AntiKt4LCTopoJets.Var" belongs. The function below returns ("AntiKt4LCTopoJets","Var")
def splitVarName_xAOD( fullname ):
    try:
        base, varname = fullname.split('.')
        return base, varname
    except:
        return "none", None

# act if reading ESD -----------
if args.ESD:
    testedContainers = ["xAOD::JetAuxContainer_v1_"+b for b in testedContainers ]
elif args.AODvsESD:
    branchRenamer = lambda n : "xAOD::JetAuxContainer_v1_"+n

    
    
# Static aux container branches -----
additionalBranches = []
for c in testedContainers:
    bname = c+'Aux.'
    additionalBranches += [ bname+v for v in ("pt","eta","phi","m") ]

# add basic clusters comp
if args.clusters:
    bname = "CaloCalTopoClusterAux."
    if args.ESD:
        bname = "xAOD::CaloClusterAuxContainer_v1_"+bname
        branchRenamer = lambda n : n.replace("_v1_CaloCalTopoCluster", "_v2_CaloCalTopoClusters")
    additionalBranches +=  [bname+v for v in ("rawE","rawEta","rawPhi") ]

# Build the main Comparator
vc = VarComparator( args.inFile1 , args.inFile2 if args.inFile2 else None , treename = args.treeName1, treename2=args.treeName2 )# , treename2="susy")
vc.splitVarName  = splitVarName_xAOD


vc.init_trees( base_vars = [],
               branch_filter = branchFilter,
               branch_replace = branchRenamer,
               checkMissing = False,
               additionalBranches = additionalBranches,
               )

#numErr = vc.full_compare(debug=True, maxEvent=1)
numErr = vc.full_compare(debug=False, maxEvent=100, detailedSummary=args.details)

exit(numErr) # RTT: success if exit code is 0, failure otherwise

