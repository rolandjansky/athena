#!/usr/bin/env python

import sys, string, commands, os, pickle, time, pprint, subprocess

from LArCafJobs.GetLBsToIgnore import getLBsToIgnore


#########################################################################

# Utility function

def getFileMap(fname, dsname, nevts=0, guid='') :
  if os.path.isfile(fname) :
    dataMap = { 'lfn': fname,
                'dataset' : dsname,
                'events' : nevts
          }
    if guid : 
      dataMap['GUID'] = guid      
  else : 
    dataMap = {}
  return dataMap

#########################################################################


def getPFN(filespec):
   if isinstance(filespec,dict):
     if filespec.has_key("svcclass"):
       os.environ["STAGE_SVCCLASS"]=filespec["svcclass"]
       pass
     
     if filespec.has_key('pfn'):
       return filespec['pfn']
     else:
       print "ERROR: Unknown file specification (dict):"
       print filespec
       sys.exit(-1)
       
     
   if isinstance(filespec,str):
     names=filespec.split('#')
     if len(names)!=2:
       print "ERROR: Unexpected file specification (string):"
       print filespec
       sys.exit(-1)
     else:
       return names[1]
     
     


def getRunLB(fname):
   filename=fname.split("/")[-1] #Strip off path
   elem=filename.split(".")
   #File name example:
   #data11_7TeV.00190618.physics_CosmicCalo.recon.HIST.f411._lb0826._SFO-ALL._0001.1
   if (len(elem) <6):
     print "ERROR: Invalid file name format. Expected at least six subfileds, got",filename
     sys.exit(-1)
     pass

   step=elem[3]
   if step != 'recon':
     return (-1,-1)

   rnStr=elem[1]

   if not rnStr.isdigit():
     print "ERROR: Can't get run number, got",rnStr
     sys.exit(-1)
     pass
   run=int(rnStr,10)

   LBStr=elem[6]
   if (len(LBStr)<4):
     print "ERROR: Can't get LB number, got",LBStr
     sys.exit(-1)
     pass

   LBStr=LBStr[3:]
   if not LBStr.isdigit():
     print "ERROR: Can't get LB number, got",LBStr
     sys.exit(-1)
     pass

   LB=int(LBStr,10)
   return (run,LB)


def larMerge(dataMap) :

  tstart = time.time()

  print "\n##################################################################"
  print   "##                 ATLAS Tier0 LAr CAF file Merging             ##"
  print   "##################################################################\n"

  print "\nFull Tier-0 run options:\n"
  pprint.pprint(dataMap)
  
  inputList = []
  inputDSName = None

  badLBs=set()

  inFiles=dataMap['inputHistFiles']


  if len(inFiles)>0:
    firstFile=getPFN(inFiles[0])
    runnumber=getRunLB(firstFile)[0]
    if runnumber==-1:
      print "Encountered pre-merged file, no bad-LB checking done" 
    else:
      print "Found run number",runnumber
      badLBs=getLBsToIgnore(runnumber)
     
    
  for val in inFiles:
    pfn=getPFN(val)
    if len(badLBs)>0:
      LB=getRunLB(pfn)[1]
      if LB in badLBs:
        print "Ignoring bad LumiBlock",LB
        continue
    inputList.append(pfn)
  sys.stdout.flush()  
  #print "\ninputLArFiles list:\n"
  #pprint.pprint(inputList)

  #Write input file list to temporary file:
  templist=open("inputfiles.txt","w")
  for infile in inputList:
    templist.write(infile+"\n")
    pass
  templist.close()


  # output file
  outputDSName = dataMap['outputLArHistFile'].split('#')[0]
  outputFile = dataMap['outputLArHistFile'].split('#')[1]

  print '\nOutput file name:', outputFile

  retcode = 0

  cmd="LArQuickHistMerge.exe -i inputfiles.txt " + outputFile
  
  try:
    retcode = subprocess.call(cmd, shell=True)
    print 'retcode =',retcode
    if retcode != 0 : 
      retcode = 62601
      acronym =  "LARQUICKHISTMEGE PROBLEM"
  except OSError, e :
    retcode = 62600
    print e
    acronym = "SUBPROCESS EXECUTION PROBLEM"
    pass

  if retcode==0:
    cmd ="DQWebDisplay.py " + outputFile +" LArDisplay 111"
    print "Attempt to run",cmd
    try:
      retcodeDQM = subprocess.call(cmd, shell=True)
      print 'retcode =',retcodeDQM
    except Exception,e:
      print "Attempt failed with exception"
      print e
    
 
  # get info for report gpickle file 
  if retcode == 0 :
    outputMap = getFileMap(outputFile, outputDSName, nevts=0)
    outFiles = [ outputMap ]
    acronym = 'OK'
    txt = 'trf finished OK'
  else:
    outFiles = []
    outputMap = {}
    print "ERROR: problem in LAr Histogram merging!"
    if retcode == 62600 :
      acronym = 'TRF_LAR_FILE_INPUT_ERROR'
      txt = 'LAr input file not accessible'
    elif retcode == 62601 :  
      acronym = 'TRF_LAR_MERGE_ERROR'



  # assemble job report map, pickle it
  outMap = { 'prodsys': { 'trfCode': retcode,
                          'trfAcronym': acronym,  
                          'jobOutputs': outFiles,
                          'jobInputs': inFiles,
                        }
           }              
  f = open('jobReport.gpickle', 'w')
  pickle.dump(outMap, f)
  f.close()

  print "\n##################################################################"
  print   "## End of job."
  print   "##################################################################\n"



########################################
## main()
########################################

if __name__ == "__main__":

  if len(sys.argv) == 2 and sys.argv[1].startswith('--argdict=') :
    picklefile = sys.argv[1][len('--argdict='):]
    # extract parameters from pickle file
    print "Using pickled file ", picklefile, " for input parameters"
    f = open(picklefile, 'r')
    dataMap = pickle.load(f)
    f.close()
    larMerge(dataMap)
  elif len(sys.argv) == 3 and sys.argv[1].startswith('--inputFiles=') and sys.argv[2].startswith('--outputFile=') :
    inputFiles = sys.argv[1][13:].split(',')
    outputFile = sys.argv[2][13:]
    dataMap = {}
    dataMap['inputHistFiles'] = inputFiles
    dataMap['outputLArHistFile'] = outputFile
    larMerge(dataMap)
  else :
    print "Input format wrong --- either have: "
    print "   --argdict=<pickled-dictionary containing input info> "
    print "   with key/value pairs: "
    print "   or (old way): "
    print "   --inputFiles=file1,file2,...,fileN --outputFile=file"
    exit(1)
