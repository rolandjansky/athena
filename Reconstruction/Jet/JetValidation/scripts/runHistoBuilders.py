# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# External dependencies
import ROOT
import sys
import re
import math
ROOT.gSystem.Load("libGenVector.so")


# Control switch
IsDebug = False



##################################################
# Read and store argument values                 #
##################################################

treeNameJETMET = "qcd"
treeNameCOMMON = "physics"
writeBranchInformation = False

if IsDebug:
  fileNameJETMET="/afs/cern.ch/user/d/delsart/public/COMMON/NTUP_COMMON_ref.root"
  treeNameJETMET="physics"
  fileNameCOMMON="/afs/cern.ch/user/d/delsart/public/COMMON/NTUP_COMMON_new.root"
else: 
  if len(sys.argv) < 3:
    print "Too few arguments.  Expected the following:"
    print "1. JETMET file (required)"
    print "2. COMMON file (required)"
    print "3. JETMET tree name (optional, default=\"qcd\")"
    print "4. COMMON tree name (optional, default=\"physics\")"
    print "5. Whether or not to write branch information to files (optional, default=False)"
    exit(1)
  
  fileNameJETMET=sys.argv[1]
  fileNameCOMMON=sys.argv[2]
  if len(sys.argv) > 3:
    treeNameJETMET = sys.argv[3]
  if len(sys.argv) > 4:
    treeNameCOMMON = sys.argv[4]
  if len(sys.argv) > 5:
    if "True" in sys.argv[5] or "true" in sys.argv[5]:
      writeBranchInformation = True
    elif "False" in sys.argv[5] or "false" in sys.argv[5]:
      writeBranchInformation = False
    else:
      print "Could not interpret the branch information control argument: ",sys.argv[5]
      exit(2)


##################################################
# Read the input files and get the trees         #
##################################################

#fileNameJETMET="/afs/cern.ch/atlas/groups/JetEtmiss/COMMON_validation/JETMET/mc12_8TeV.147914.Pythia8_AU2CT10_jetjet_JZ4W.merge.JETMET.e1126_s1469_s1470_r3542_r3549_r17_2_7_5_3.000212.root"
#fileNameCOMMON="/afs/cern.ch/atlas/groups/JetEtmiss/COMMON_validation/COMMON/test162_mc12.aod.100evt.physics.12_04_2013.root"
#fileNameJETMET="/afs/cern.ch/user/t/toshi/public/NTUP_COMMON/test182_mc12.aod.100evt.qcd.root"
#fileNameJETMET="/afs/cern.ch/user/d/delsart/public/COMMON/NTUP_COMMON_ref.root"

#fileNameCOMMON="/afs/cern.ch/atlas/groups/JetEtmiss/COMMON_validation/COMMON/latesttest_mc12.aod.100evt.physics.root "
#fileNameCOMMON="/afs/cern.ch/user/t/toshi/public/NTUP_COMMON/test173_mc12.aod.100evt.physics.root"
#fileNameCOMMON="/afs/cern.ch/user/t/toshi/public/NTUP_COMMON/test182_mc12.aod.100evt.physics.root"
#fileNameCOMMON="/afs/cern.ch/user/d/delsart/public/COMMON/NTUP_COMMON_new.root"


fileJETMET = ROOT.TFile(fileNameJETMET,"READ")
fileCOMMON = ROOT.TFile(fileNameCOMMON,"READ")

treeJETMET = fileJETMET.Get(treeNameJETMET)
if not isinstance(treeJETMET,ROOT.TTree) :
  print "JETMET file does not contain the specified tree.  Is the tree name correct?"
  print "File: ",fileNameJETMET
  print "Tree: ",treeNameJETMET
  exit(3)

treeCOMMON = fileCOMMON.Get(treeNameCOMMON)
if not isinstance(treeJETMET,ROOT.TTree):
  print "COMMON file does not contain the specified tree.  Is the tree name correct?"
  print "File: ",fileNameCOMMON
  print "Tree: ",treeNameCOMMON
  exit(4)




##################################################
# Run the actual program now                     #
##################################################
from JetValidation.D3PDHistoBuildLib import MultipleHistoBuilderFromD3PD,JetType,JetVar


##################################################
# First, branch name checks                      #
##################################################

branchesJETMET = treeJETMET.GetSetOfTreeBranchNames()
branchesCOMMON = treeCOMMON.GetSetOfTreeBranchNames()
branchesBOTH   = frozenset.intersection(branchesJETMET,branchesCOMMON)
brNotInCOMMON  = branchesJETMET-branchesCOMMON
brNotInJETMET  = branchesCOMMON-branchesJETMET

if writeBranchInformation:
  outFile = open('Branches_JETMET.txt','w')
  outFile.write(str(sorted(branchesJETMET)))
  outFile.close()
  
  outFile = open('Branches_COMMON.txt','w')
  outFile.write(str(sorted(branchesCOMMON)))
  outFile.close()
  
  outFile = open('Branches_BOTH.txt','w')
  outFile.write(str(sorted(branchesBOTH)))
  outFile.close()
  
  outFile = open('Branches_NotIn_COMMON.txt','w')
  outFile.write(str(sorted(brNotInCOMMON)))
  outFile.close()
  
  outFile = open('Branches_NotIn_JETMET.txt','w')
  outFile.write(str(sorted(brNotInJETMET)))
  outFile.close()
  
  # Write out in twiki-syntax
  outFile = open('Twiki_Branches_NotIn_COMMON.txt','w')
  outFile.write('| *Branch name* | *Expected* | *Confirmed by* ||\n')
  for aBranch in sorted(brNotInCOMMON):
    outFile.write('| !%s | ? | ? ||\n'%(aBranch))
  outFile.close()


##################################################
# Next, jet types and variables checks           #
##################################################

if writeBranchInformation:
  jetTypesJETMET = set([])
  jetVarsJETMET  = set([])
  for name in branchesJETMET:
    if "jet_" in name:
      type = JetType(name)
      if type is not None:
        jetTypesJETMET.add(type)
        jetVarsJETMET.add(JetVar(name))
  
  jetTypesCOMMON = set([])
  jetVarsCOMMON  = set([])
  for name in branchesCOMMON:
    if "jet_" in name:
      type = JetType(name)
      if type is not None:
        jetTypesCOMMON.add(type)
        jetVarsCOMMON.add(JetVar(name))
  
  jetTypesBoth = set.intersection(jetTypesJETMET,jetTypesCOMMON)
  jetVarsBoth  = set.intersection(jetVarsJETMET,jetVarsCOMMON)
  
  # For each jet type, ensure that every variable is included
  branchesNotExisting = []
  for aType in jetTypesBoth:
    for aVar in jetVarsBoth:
      toCheck = aType + "_" + aVar
      if toCheck not in branchesBOTH:
        branchesNotExisting.append(toCheck)
  
  outFile = open('Branches_JetTypes_NotExisting.txt','w')
  outFile.write(str(sorted(branchesNotExisting)))
  outFile.close()


##################################################
# Now, do the thorough variable check            #
##################################################

multiBuilder = MultipleHistoBuilderFromD3PD()
multiBuilder.addTrees(treeJETMET,treeCOMMON)
multiBuilder.createBuildersFromTrees()


#multiBuilder.addHistosWithDefaultSelectors(["jet_AntiKt4LCTopo_constscale_pt"], "jet_AntiKt4LCTopo_pt")
#multiBuilder.addHistosWithDefaultSelectors(["jet_AntiKt4LCTopo_constscale_pt",100,0,1000e3], "jet_AntiKt4LCTopo_pt")
#for aBranch in multiBuilder.getBranchIntersection() :
#  if "jet_AntiKt4LC" in aBranch :
#    multiBuilder.addHistosWithDefaultSelectors(aBranch)
#    multiBuilder.addHistosWithHighPtDetscaleSelectors(aBranch)

#multiBuilder.addAllHistosInBranchIntersection()
#multiBuilder.addAllLeadingJetHistosInBranchIntersection()
#multiBuilder.addHistosWithDefaultSelectors(*multiBuilder.getBranchIntersection())
#multiBuilder.addHistosWithHighPtDetscaleSelectors(*multiBuilder.getBranchIntersection())

for aBranch in multiBuilder.getBranchIntersection() :
#for aBranch in ["jet_AntiKt4LCTopo_emscale_pt","jet_AntiKt4LCTopo_eta"]  :
    if "EventNumber" in aBranch:
      multiBuilder.addHistos(aBranch)
    if "jet_AntiKt4" in aBranch and ("LC" in aBranch or "EM" in aBranch) :
        multiBuilder.addHistosWithLeadingJetSelectors(JetType(aBranch), aBranch)

print "\nRunning event loop..."
multiBuilder.eventLoop()
print "Done event loop!"

#multiBuilder.draw(0)
#multiBuilder.canvas.Print("test.pdf")
twikiLog = open('Twiki_ToleranceResults.log','w')
twikiLog.write('| *Variable* | *Passes tolerance* | *Num failed/total bins* | *Largest difference (%)* ||\n')
multiBuilder.writeAllHistoToFile("test.pdf",logFile="failsTolerance.log",twikiLogFile=twikiLog)
twikiLog.close()

multiBuilder.close()
fileJETMET.Close()
fileCOMMON.Close()




