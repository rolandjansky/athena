#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#########################################################################
##
## Tier-0 TAG file merging transformation
##
##  - input parameter: file containing a json dictionary consisting of the key/value pairs
##     1) 'inputLArFiles': python list ['datasetname#filename1','datasetname#filename2',...]
##        (input dataset + file names)
##     2) 'outputLArFile': string 'datasetname#filename'
##        (output dataset name + file)
##
##  - generates merged TAG output file
##
## (C) Jack Cranshaw, Luc Goossens, David Malon, Armin Nairz, Nicolas Berger, Jaroslav Guenther
##     (July 2008, April 2009, July 2010, May 2011, May 2017, October 2021)
##
#########################################################################

import sys, string, os, json, time, pprint, subprocess

#########################################################################

# Utility function
def getSubFileMap(fname, nevts=0) :
    if os.path.isfile(fname) :
        sz = os.path.getsize(fname)
        map = { 'name': fname,
                'file_size' : sz,
                'nentries' : nevts,
              }
    else : 
        map = {}
    return map


#########################################################################


def larMerge(dataMap) :

  tstart = time.time()

  print("\n##################################################################")
  print(  "##                 ATLAS Tier0 LAr CAF file Merging             ##")
  print(  "##################################################################\n")

  # initialize exit values
  exitCode = 0
  exitAcronym = 'OK'
  exitMsg = 'trf finished OK'
  dt=0
  nEvents=0
  outFiles = []
  inFiles = []
        
  print("\nFull Tier-0 run options:\n")
  pprint.pprint(dataMap)
  
  inputList = []
  inputDSName = dataMap['inputLArFiles'][0]['dsn']

  for fdict in dataMap['inputLArFiles'] :
    inputList.append(fdict['lfn']) 
    
  print("\ninputLArFiles list:\n")
  pprint.pprint(inputList)

  # output file
  outputDSName = dataMap['outputLArFile'].split('#')[0]
  outputFile = dataMap['outputLArFile'].split('#')[1]

  print('\nOutput file name:', outputFile)

  cmdline=["LArSamplesMerge"]
  for fileName in inputList:
    if os.access(fileName,os.R_OK):
      cmdline.append(fileName)
    else:
      print('ERROR : could not open file', fileName)
      exitCode = 74001
      exitAcronym = 'TRF_LAR_FILE_INPUT_ERROR'
      exitMsg = 'LAr input file not accessible'
      break
    pass
  
  if (exitCode==0):
    writepath=os.path.split(outputFile)[0]
    if writepath=="": writepath="./"
    if (os.access(writepath,os.W_OK)):
      cmdline.append(outputFile)
    else:
      print("ERROR, not allowed to write to oututfile", outputFile)
      exitCode = 74002
      exitAcronym = 'TRF_LAR_MERGE_ERROR'
      exitMsg = 'LAR merging error'
    pass
  
  if (exitCode==0):

    logfile=open("log.LArMerge","w")

    try:
      subprocess.check_call(cmdline,stdout=logfile,stderr=subprocess.STDOUT)
    except Exception as e:
      print("ERROR exectution of subprocess failed")
      print(e)
      exitAcronym = 'TRF_LAR_MERGE_ERROR'
      exitMsg = 'LAR merging error'
      exitCode==1

    logfile.close()
    logfile=open("log.LArMerge","r")

    for line in logfile:
      print(line,)
      if line.startswith("Open file "):
        linetok=line.split(" ")
        inFiles.append(getSubFileMap(linetok[2],nevts=int(linetok[4])))
      if line.startswith("Wrote output file "):
        linetok=line.split(" ")
        nEvents=int(linetok[5])
        outFiles.append(getSubFileMap(linetok[3], nevts=nEvents))
    logfile.close()
  dt = int(time.time() - tstart)
  # assemble job report map, json it
  outMap = { 'exitAcronym' : exitAcronym,
             'exitCode' : exitCode,
             'exitMsg' : exitMsg,
             'files' : { 'output' : [{ 'dataset' : outputDSName,
                                        'subFiles' : outFiles
                                      },],
                         'input' : [{ 'dataset' : inputDSName,
                                       'subFiles' : inFiles
                                     }
                                   ] },
             'resource' : { 'transform' : { 'processedEvents' : int(nEvents), 'wallTime' :  dt} }
            } 

  f = open('jobReport.json', 'w')
  json.dump(outMap, f)
  f.close()

  print("\n##################################################################")
  print(  "## End of job.")
  print(  "##################################################################\n")

  print(outMap)

########################################
## main()
########################################

if __name__ == "__main__":


  if len(sys.argv) == 2 and sys.argv[1].startswith('--argJSON=') :
    jsonfile = sys.argv[1][len('--argJSON='):]
    # extract parameters from json file
    print("Using json file ", jsonfile, " for input parameters")
    f = open(jsonfile, 'r')
    dataMap = json.load(f)
    f.close()
    larMerge(dataMap)
  elif len(sys.argv) == 3 and sys.argv[1].startswith('--inputFiles=') and sys.argv[2].startswith('--outputFile=') :
    inputFiles = sys.argv[1][13:].split(',')
    outputFile = sys.argv[2][13:]
    dataMap = {}
    dataMap['inputLArFiles'] = inputFiles
    dataMap['outputLArFile'] = outputFile
    larMerge(dataMap)
  else :
    print("Input format wrong --- either have: ")
    print("   --argJSON=<json-dictionary containing input info> ")
    print("   with key/value pairs: ")
    print("   or (old way): ")
    print("   --inputFiles=file1,file2,...,fileN --outputFile=file")
    exit(1)
