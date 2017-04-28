# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Script by Kate Pachal, December 2016

import sys
import os
import argparse
import time
import re
import subprocess
import datetime
import random
import glob
import time

from itertools import combinations,combinations_with_replacement,permutations,product
import operator

# This is essentially code to solve the problem of sorting n distinguishable
# balls into m indistinguishable boxes. We want all unique combinations of 
# parameters into 3 (or n) categories. 

# Solution is recursive following the outline given in 
# http://www.elcamino.edu/faculty/gfry/210/DistributeDifBallsDifBoxes.pdf

# For 19 globally reduced nuisance parameters going into 3 strongly reduced NPs,
# number of configs is s2(19,3) = 193448101: yikes!

# For sake of sanity, not going to go this route. Instead, look at all ways to
# choose groups of ~int(n/3) uniquely.
# Using this route, 16 globally reduced parameters give
# 1009008 configs. This is more reasonable.

# Since Steven used to group all but leading stat uncertainty together,
# I will group final 5 together, reducing nPars from 19 to 15.
# This can be done by changing XXX to XXY for all pars you wish to see
# treated as 1 unit in config file.

# Usage: 
# --d,dir yourRelease,           example JES_2016/Moriond2017
# --t,tmp templateFile,          and a path within that release to the template to use
# --n,npars nPars,               the number of reduced parameters to allow (default 3; occasionally 4)
# --r,ref reference config name, example JES2016_AllNuisanceParameters.config 

# A few things which will rarely be changed, and so
# are not set by command line:
templateScript = "scripts/StrongReductions_BatchScript_Template.sh"
ScriptArchive = "NPBashScripts"
# These are two-point systematics to relate via Steven's function system
# (which I still don't understand)
compsToSplit = ["EtaIntercalibration_Modelling_orig","Flavor_Composition_orig","Flavor_Response_orig","BJES_Response"]

# NOTE: THIS CURRENTLY ONLY WORKS FOR m=3!
# Time estimates: for 10 into 3, 0.0205 s
#                 for 12 into 3, 0.1821 s
#                 for 15 into 3, 4.5851 s
#                 for 19 into 3, ?? s (XXX combinations)
def computeCombinations(n,m) :
  
  myCombinations = []

  count = 0
  # First, look at unique ways to choose first 1/3.
  for indices in combinations(range(n), int(n/m)) :
    # We have one set of 7 and a remaining m-int(n/m) to be split in two.
    remaining = sorted([l for l in range(n) if l not in indices])
    for secondSet in combinations((x for x in remaining),int((n-int(n/m))/(m-1))) :
      secondSet = sorted(secondSet)
      final = sorted([l for l in remaining if l not in secondSet])
      newlist = sorted([tuple(indices),tuple(secondSet),tuple(final)])
      count = count+1
      myCombinations.append(tuple(newlist))

  print "Removing duplicates."
  myCombinations = list(set(myCombinations))
  print "Examined",count,"combinations of which",len(myCombinations),"were unique."
  return myCombinations

# This assumes your n parameters are sensibly numbered in the file.
def makeNewConfigFile(templateFile,combinations,index,npars) :

  newConfName = templateFile.replace("template","index{0}".format(index))
  combs = {0: combinations[0], 1 : combinations[1], 2:combinations[2]}
  count = 0
  holdVal = -1
  with open(templateFile) as input, open(newConfName,"write") as output :
    for line in input :
      if "XXX" in line or "XXY" in line :
        if "XXY" in line and holdVal > 0 :
          line = line.replace("XXY","{0}".format(holdVal))
          output.write(line)
          continue
        for val,indices in combs.iteritems() :
          if count in indices :
            addon = 100+val
        count = count+1
        if "XXY" in line :
          holdVal = addon
        line = line.replace("XXX","{0}".format(addon))
        line = line.replace("XXY","{0}".format(addon))
      if count == npars : break
      output.write(line)

  return newConfName

class JESGroup :
  def __init__(self):
    self.name = ""
    self.index = 0
    self.groupID = 0
    self.groupingAssigned = 0

def replace_right(source, target, replacement, replacements=None):
    return replacement.join(source.rsplit(target, replacements))

def makeSplitConfigs(config,redo=True) :

  # Groups are only 3 in version I'm prepared for.
  groups = [100,101,102]
  JESGroups = {}

  # Keep track of number of variations and give
  # unique names
  nCombs = 2**len(compsToSplit)
  if not redo:
    return nCombs
  
  mycombinations = tuple(combinations_with_replacement([0,1], len(compsToSplit)))
  mypermutations = []
  for i in mycombinations :
    mypermutations = mypermutations + list(set(tuple(permutations(i))))

  outfiles = []
  for index in range(len(mypermutations)) :
    newname = config.replace(".config","_splitV{0}.config".format(index))
    newname = newname.replace("generateConfigs","splitsConfigs")
    outfiles.append(open(newname, 'w'))
  
  # Read in config file and write various options
  thisVal = -1
  continuing = False
  addingSplit = False
  splitComp = ""
  group = None
  with open(config,"r") as input :
  
    for line in input :
    
      # Just write comments.
      if not line.isspace() and not line.startswith("JES") :
        for file in outfiles : file.write(line)
        continue
    
      # Blank line can signal end of component.
      if line.isspace() :
        if continuing and addingSplit :
          for index, file in enumerate(outfiles) :
            possible = groups[:]
            newLine = "JESComponent.{0}.Split:              1\n\n".format(thisVal)
            file.write(newLine)
          
            # Now need matching opposite section
            for bonusline in lines :
              bonusline = bonusline.replace(".{0}.".format(thisVal),".{0}.".format(110+splitComp))
              if ".Group" in bonusline :
                # Two possibilities. First is that this is a standalone item
                # and group will be one of 100, 101, 102.
                # Alternately, part of a larger JES group and need to get index from that.
                if group > 99 :
                  groupIndex = group
                else :
                  groupIndex = JESGroups[group].groupingAssigned
                possible.remove(groupIndex)
                useIndex = mypermutations[index][splitComp]
                # Need to replace only last instance below
                bonusline = replace_right(bonusline,"{0}".format(group),"{0}".format(possible[useIndex]),1)
              file.write(bonusline)
  
            newLine = "JESComponent.{0}.Split:              -1\n".format(110+splitComp)
            file.write(newLine)
        
        for file in outfiles : file.write(line)
        continue

      # What component index is this?
      items = filter(None,line.split())
      description = items[0]
      val = eval(description.split(".")[1])
      # Start a new component/group
      if val != thisVal :
        thisVal = val
        continuing = False
        lines = [line]
      else :
        continuing = True
        lines.append(line)
      if line.startswith("JESComponent") :
        # Can I get the name?
        if "Name" in description :
          # Is it one I want?
          if any(ext in line for ext in compsToSplit) :
            print "Found component", items[1]
            addingSplit = True
            splitComp = compsToSplit.index(items[1])
          else :
            addingSplit = False
        if ".Group" in description :
          group = eval(items[1])

      # Groups need to be stored in a look-up format
      # so I can pick up info from them later.
      if line.startswith("JESGroup") :
        continuing = False
        # Can I get the name?
        if "Name" in description :
          newGroup = JESGroup()
          newGroup.index = thisVal
          newGroup.name = items[1]
          JESGroups[thisVal] = newGroup
        if ".Group" in description :
          JESGroups[thisVal].groupID = eval(items[1])
        if ".SubGroup" in description :
          JESGroups[thisVal].groupingAssigned = eval(items[1])
      
      for file in outfiles :
        file.write(line)

  for file in outfiles : file.close()
 
  return len(mypermutations)

def batchSubmit(command, index, phrase) :

  # Perform setLimitsOneMassPoint on batch
  batchcommand = command
  
  # Open batch script as fbatchin
  fbatchin = open(templateScript, 'r')
  fbatchindata = fbatchin.read()
  fbatchin.close()
          
  # open modified batch script (fbatchout) for writing
  batchtempname = '{0}/StrongReductions_BatchScript_{1}_index{2}.sh'.format(ScriptArchive,phrase,index)
  fbatchout = open(batchtempname,'w')
  fbatchoutdata = fbatchindata.replace("ZZZ",batchcommand) # In batch script replace ZZZ for submit command
  fbatchout.write(fbatchoutdata)    
  modcommand = 'chmod 744 {0}'.format(batchtempname)
  subprocess.call(modcommand, shell=True)
  fbatchout.close()
  submitcommand = "qsub {0}".format(batchtempname)
  print submitcommand
  subprocess.call(submitcommand, shell=True)

def runMatrixCommands(nominalFile,config,outPlotName,outRootName,index,batch) :

  # If root file modification time more recent than config modification time,
  # then we don't have to do this.
  fileNew = False
  if os.path.isfile(outRootName):
    file_time = os.path.getmtime(outRootName)
    config_time = os.path.getmtime(config)
    if config_time < file_time : fileNew = True

  if not fileNew :
    command = 'jetDefinition="AntiKt4EMTopo" configFiles="{0};{1}" outFile="{2}" outHistFile="{3}" bash runMakeCorrelationMatrixPlots.sh Flexible FineGridFixedEta false'.format(nominalFile,config,outPlotName,outRootName)
    if batch :
      batchSubmit(command, index, "Matrices")
    else :
      subprocess.call(command,shell=True)

def runComparisonCommands(outFileName,plotsName,file1,file2,file3,file4,string,batch) :

  # If output canvases already exist, then unless
  # root file modification time more recent than canvas modification time,
  # we don't have to do this.
  canvasExists = False
  earliestCanvas = 1E25
  canvases = glob.glob(outFileName.replace(".png","*.png"))
  for canvas in canvases :
    canvasExists = True
    if os.path.getmtime(canvas) < earliestCanvas : earliestCanvas = os.path.getmtime(canvas)

  latestRootFile = 0
  for rootFileName in [file1,file2,file3,file4] :
    if not os.path.isfile(rootFileName) :
      print "ERROR! Root file does not exist."
      return
    if os.path.getmtime(rootFileName) > latestRootFile : latestRootFile = os.path.getmtime(rootFileName)

  if canvasExists and latestRootFile < earliestCanvas  :
    print "Combination already calculated; skip"
    return

  command = 'python ../python_scripts/Make4DCorrelationMatrix.py "AntiKt4EMTopo" {0} {1} false {2} {3} {4} {5}'.format(outFileName,plotsName,file1,file2,file3,file4)
  if batch :
    batchSubmit(command, string, "CompareSets")
  else :
    subprocess.call(command,shell=True)

def main() :

  parser = argparse.ArgumentParser()
  parser.add_argument("-d","--dir")
  parser.add_argument("-t","--tmp")
  parser.add_argument("-n","--npars",default=3)
  parser.add_argument("-r","--ref")
  parser.add_argument("-b","--batch",action='store_true')
  parser.add_argument("-ns","--nosubmit", action='store_true')
  parser.add_argument("-rc","--redo-configs", action='store_true')
  parser.add_argument("-ntm","--ntestsmatrices",default=100)
  parser.add_argument("-ntc","--ntestscomparisons",default=100)
  parser.add_argument("--matrixStage",action='store_true')
  parser.add_argument("--comparisonStage",action='store_true')
  parser.add_argument("--investigateComb",default="",type=str)
  parser.add_argument("--investigateSplits",default="",type=str)
  args = parser.parse_args()
  print args

  shareDir = os.getcwd()+"/../../share/"+args.dir
  nominalFile = shareDir+"/"+args.ref
  templateFile = shareDir+"/"+args.tmp
  newFilesDir = ""
  if "/" in templateFile :
    for item in templateFile.split("/")[:-1] :
      newFilesDir = newFilesDir+"/"+item

  nPars = args.npars

  print "sharedir",shareDir
  print "nominal",nominalFile
  print "template",templateFile
  print "new configs will go in",newFilesDir
  print "using",nPars,"parameters, not counting eta intercalibration non-closure."

  doPlots = False

  # Now we make the new configuration files.
  configFileList = []
  nConfigs = 0
  if args.redo_configs and not args.investigateSplits :
    start_time = time.time()
    fullCombList = computeCombinations(15,3) # 15
    index = -1
    print("--- %s seconds ---" % (time.time() - start_time))
    for comb in fullCombList :
      index = index+1
      thisname = makeNewConfigFile(templateFile,comb,index,args.npars)
      #configFileList.append(thisname)
    nConfigs = index+1
  else :
    nConfigs = len([os.path.join(newFilesDir, f) for f in os.listdir(newFilesDir) if "index" in f])

  # Begin matrix computation stage, if desired.
  outRootDir = os.getcwd()+"/NPRootFiles/"
  outRootFormat = outRootDir+"matrices_{0}.root"
  if args.matrixStage and not args.investigateSplits :

    # Rather than run on all consecutive configs up to ntestsmatrices,
    # we want configs that are not overly similar to each other.
    # So randomly select (non-repeating) indices between 0 and the number
    # of configs available, and run on these until the limit is reached.
    useval = int(args.ntestsmatrices)
    indicesToUse = random.sample(xrange(0,nConfigs), useval)
    for index in indicesToUse :

      # We know what the config matching this index should be.
      config = templateFile.replace("template","index{0}".format(index))
    
      # We have 1 config for every set of NP's we wish to examine. Now
      # make a correlation matrix for each. This is a batch job!
      outRootName = outRootFormat.format(index)
      if doPlots :
        outPlotName = os.getcwd()+"/NPPlots/matrices_{0}.pdf".format(index)
      else :
        outPlotName = "NONE"

      runMatrixCommands(nominalFile,config,outPlotName,outRootName,index,args.batch)

  # Begin matrix comparison stage, if desired.
  if args.comparisonStage and not args.investigateSplits :

    # Now we need to compare sets of 4 and look for one with a low enough difference.
    # Have to be chosen from among existing root files *which may or may not be
    # the same as what was just run* since they get replaced only if needed.
    rootFiles = [os.path.join(outRootDir, f) for f in os.listdir(outRootDir) if "matrices" in f]
    indicesAvailable = []
    for file in rootFiles :
      index = re.findall(r'\d+', file.split("/")[-1].split("_")[-1])
      indicesAvailable.append(eval(index[0]))

    # In special cases you may have a specific combination you know is good and you want to explore
    # variations rather close to it to see if it can be improved upon.
    # Call with the combination and look at others which differ by only 1 config:
    if args.investigateComb :
      goodComb = tuple([eval(item) for item in args.investigateComb.split(",")])
      newList = []
      for index in range(4) :
        for new in random.sample(indicesAvailable,max(1,int(float(args.ntestscomparisons)/4.0))) :
          if not new in goodComb :
            newComb = list(goodComb)
            newComb[index] = new
            newList.append(tuple(newComb))
      pool = tuple(newList)

    # Default option has no restrictions.
    else :
      # There are a crazy number of combinations so just pick a set of those at random
      # if there are more than 15 of them...
      random.shuffle(indicesAvailable)
      if len(indicesAvailable) > 15 :
        indicesAvailable = indicesAvailable[:15]

      # Now run on unique combinations up to a cutoff number of tries.
      # For now this is the same as the number of individual files I created,
      # with the assumption that my patience for this step is about equal ;)
      pool = tuple(combinations(indicesAvailable,4))
      #pool = tuple([tuple([116061,36885,33399,64587])])

    for indices in random.sample(pool,min(len(pool),args.ntestscomparisons)) :
      print "Testing combination",indices

      outFileName = os.getcwd()+"/NPPlots/SR-4D-{0}-{1}-{2}-{3}.png".format(indices[0],indices[1],indices[2],indices[3])
      
      runComparisonCommands(outFileName,"NONE",outRootFormat.format(indices[0]),outRootFormat.format(indices[1],outRootFormat.format(indices[2])),outRootFormat.format(indices[2]),outRootFormat.format(indices[3]),"{0}-{1}-{2}-{3}".format(indices[0],indices[1],indices[2],indices[3]),args.batch)
      
  # Let's say we have a good combination but now we need
  # to check out the various options for splitting configs.
  if args.investigateSplits :
    
    configIndices = tuple([eval(item) for item in args.investigateSplits.split(",")])
    print configIndices
    splitconfigs = [templateFile.replace("template","index{0}".format(index)) for index in configIndices[1:]]
    print splitconfigs
    iterables = []
    for configindex, config in zip(configIndices[1:],splitconfigs) :
      newIndices = makeSplitConfigs(config,args.redo_configs)
      newIterables = []
      
      for index in range(newIndices) :
        outRootName = (outRootFormat.format(configindex)).replace(".root","_splitV{0}.root".format(index))
        newconfig = config.replace(".config","_splitV{0}.config".format(index))
        newconfig = newconfig.replace("generateConfigs","splitsConfigs")
        newIterables.append("{0}_splitV{1}".format(configindex,index))
        
        runMatrixCommands(nominalFile,newconfig,"NONE",outRootName,"{0}-{1}".format(configindex,index),args.batch)

      iterables.append(newIterables)

    # Wait for matrices to finish
    isdone = not subprocess.check_output("qstat",shell=True)
    while not isdone :
      time.sleep(20)
      isdone = not subprocess.check_output("qstat",shell=True)

    #Run comparisons
    pool = tuple(product([configIndices[0]],iterables[0],iterables[1],iterables[2]))
    
    for indices in random.sample(pool,min(len(pool),args.ntestscomparisons)) :
      print "Testing combination",indices

      outFileName = os.getcwd()+"/NPPlots/SR-4D-{0}-{1}-{2}-{3}.png".format(indices[0],indices[1],indices[2],indices[3])

      runComparisonCommands(outFileName,"NONE",outRootFormat.format(indices[0]),outRootFormat.format(indices[1],outRootFormat.format(indices[2])),outRootFormat.format(indices[2]),outRootFormat.format(indices[3]),"{0}-{1}-{2}-{3}".format(indices[0],indices[1],indices[2],indices[3]),args.batch)

if __name__ == "__main__":
  main()
