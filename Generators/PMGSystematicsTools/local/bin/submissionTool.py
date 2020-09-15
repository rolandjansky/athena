#!/usr/bin/env python
"""
This module is a wrapper to pathena, which does the following useful things:
- Retrieves the list of matrix-elememt weights stored in a given ATLAS MC samples, either
-- using the DSID_database
-- or by automatically downloading a test sample and checking manually which weights are
available when initialising the Rivet_i instances
- Produces Job options from templates stored in /data/RivetAnalysis*JO*py
- Automatic download of test samples if needed

Author: Louie D. Corpe (UCL)  
Email: l.corpe@cern.ch  
"""

import os, sys
import re
import optparse
import readDatabase as rDB
import rivet

parser = optparse.OptionParser( usage = "%prog [options]" )
parser.add_option("-i","--inputSamples", help="list of samples to submit. One per line", dest="inputSamples", default="example.txt" )
parser.add_option("-l","--label", help="label to keep track of what you submitted", dest="label", default="" )
parser.add_option("--noSyst", help="process the systematics variations if there are any in ME", dest="noSyst", default=False, action="store_true" )
parser.add_option("--testSampleDir", help="where to store the test samples. Must be away from the submission dir otherwise panda will try to send the test samples too...", dest="testSampleDir",default="../testSamples" )
parser.add_option("-a","--rivetAnalysis ", help="If running Rivet, which rivet analysis/analyses to run? The script will include it in the job. Supports comma-separeted list of analyses. If this is a custom plugin please make sure you have compiled it the submission directory using: rivet-buildplugin RivetAnalysis_<myanalysis>.so <myanalysis>.cc. eg ATLAS_2017_I1514251. If you are running AthAnalysis code, you should ignore this option", dest="analysis",default=None )
parser.add_option("--dryRun ", help="do everything except submit the jobs", dest="dryRun", default=False, action="store_true" )
parser.add_option("--ds","--downloadSample", help="if your sample DSID is not in the database, you can download one like this and get the weights from there. ", dest="downloadSample", default=False, action="store_true" )
parser.add_option("--ef","--extraFiles", help="comma separated list of any additional files your jobs need as inputs (eg config files, steering files...). Should be in your submission directory, and will get bundled up and submitted by panda.", dest="extraFiles", default="")
parser.add_option("-N","--nJobs", help="[DEPRECATED] Number of jobs to prepare. This feeds into the pathena nJobs/split option. -1 tells pathena to work out a sensible value itself.", dest="nJobs", default=-1)
parser.add_option("--pathenaOptions", help="Pass any of the usual pathena options inside '', which will be propagated to job submission.", dest="pathenaOptions", default="")
parser.add_option("--nFilesPerJob", help="[DEPRECATED] Number of files to process in each job. This feeds into the pathena nFilesPerJob option. -1 tells pathena to work out a sensible value itself.", dest="nFilesPerJob", default=-1)
parser.add_option("-j","--jo","--templateJobOptions", help="Your template job options! Should contain a line 'systWeights=!SYSTWEIGHTS! which will get populated by this script. If running rivet analyses, you can leave this option blank and just fill --analysis with the list of Rivet analyses you want to run. For AthAnalysis JOs, see this as an example: source/ExampleDAODAnalysis/share/ExampleDAODAnalysisAlgJobOptions.py' ", dest="templateJobOptions", default=None)
(opts, args) = parser.parse_args()


submissionTemplatesDir=os.environ["SYSTTOOLSPATH"]

# by default the template is for no Systsematics
#templateJobOptions="%s/data/RivetAnalysis_JO_noSyst.py" % (submissionTemplatesDir)
#source/ExampleDAODAnalysis/share/ExampleDAODAnalysisAlgJobOptions.py

def findTestFile(testSampleDir,identifier):
 """
 `testSampleDir` String (look in this directory to try to find a matching test file)
 `identifier` String (A string which uniquely defined the sample you are interested in
 for example, the DSID of the sample of interest)
 `result` String or None (String if a matching file name was found, None otherwise)

 check the specified directory for a test EVNT file for a specified DISD
 """
 for dirName in os.listdir(testSampleDir):
  #if identifier in dirName and "EVNT" in dirName: 
  if identifier in dirName : 
     testSamplePath=testSampleDir+"/"+dirName
     for fileName in os.listdir(testSamplePath):
       if not 'root' in fileName: continue
       if not 'EVNT' in fileName: continue
       testSamplePath=testSamplePath+"/"+fileName
       # if so, break: we don't need to download it.
       return testSamplePath
       break

def getTotalNEventsAndNFiles(thisSampleName):
 """
 `thisSampleName` String (name of sample to retrieve info for)
 `result` int, int (nFilesInSample, nEventsInSample)

 download a tst file for the specified dataset name into the specified dir name
 """
 res=None
 commandLine= "rucio list-files %s | tail -n3 > out.tmp"  % (thisSampleName) 
 os.system(commandLine)
 ftmp =open("out.tmp",'r')
 nTotalFiles=-1
 nTotalEvents=-1
 for line in ftmp.readlines():
   line=line.strip().split(":")
   if "Total files" in line[0]:
     nTotalFiles=int(line[1])
   if "Total events" in line[0]:
     nTotalEvents=int(line[1])
 print("[INFO] sample has nFiles=%d, with nEvents=%d"%(nTotalFiles, nTotalEvents))
 return nTotalFiles, nTotalEvents 

 

def downloadTestFile(testSampleDir,thisSampleName):
 """
 `testSampleDir` String (look in this directory to try to find a matching test file)
 `thisSampleName` String (name of sample to download)
 `result` String or None (String if a matching file name was found, None otherwise)

 download a tst file for the specified dataset name into the specified dir name
 """
 res=None
 print("[INFO] do not currently have a test sample for ", thisSampleName)
 commandLine= "rucio download --nrandom 1 %s --dir %s " % (thisSampleName, testSampleDir) 
 print("[INFO] --> downloading one using this command \n ",commandLine)
 os.system(commandLine)
 if ":" in thisSampleName:
   testSamplePath=testSampleDir+"/"+thisSampleName.split(":")[1]
 else:
   testSamplePath=testSampleDir+"/"+thisSampleName
 for fileName in os.listdir(testSamplePath):
  testSamplePath=testSamplePath+"/"+fileName
  return testSamplePath
  break

def findUserNickname():
 """
 return the nickname related to the grid certificate
 """
 try:
    nickname = os.popen("voms-proxy-info -all | grep nickname").read()
    nickname = nickname.split(' ')[4]
 except:
    nickname = os.getlogin()
 return nickname

def main(argv):
  """
  This module can also be run as a standalone executable. 
  For info about the options try:
  submissionTool.py -h

  This tool is used to submit samples to the GRID, when once instance of Rivet_i per Matrix-Element
  weight is initialised and run. Template Job Options are available in /data/RivetAnalysis_*JO*.py
  """
  print("========================================================================================")
  print("[INFO] processing files for ", opts.label," using inputs ", opts.inputSamples)
  print("[INFO] ignore systematics ? " , opts.noSyst)
  print("[INFO] use ME-wights from downloaded test sample ? ", opts.downloadSample)
  print("[INFO] location of downloaded samples =  ", opts.testSampleDir)
  print("========================================================================================")
  
  isRivet = ((opts.templateJobOptions is None) and (opts.analysis is not None))
  isAthAnalysis = ((opts.templateJobOptions is not None) and (opts.analysis is None))

  if (isRivet == isAthAnalysis):
    print("Configuration error." )
    print("If running rivet routines, you should provide arguments for --rivetAnalysis and NOT --templateJobOptions")
    print("If running AthAnalysis code, you should provide arguments for --templateJobOptions and NOT --rivetAnalysis")
    exit(1)

  if opts.nFilesPerJob >0 or opts.nJobs>0:
    print("[WARNING] --nFilesPerJob and --nJobs options for submissionTool.py are deprecated. Advice it to let pAthena work this our by itself. If you really want to use thise options, specify them manually with --pathenaOptions")
    exit(1)
  
  f=open(opts.inputSamples)
  # loop through the samples we want to process
  for line in f.readlines():
    line=line.strip()
    fracString=""
    if len (line.split(" "))>1:
      fracString =line.split(" ")[1]
      line =line.split(" ")[0]
    if len(line)==0 : continue
    if line[0]=="#" : continue
    if line.strip()[-1]=='/': line=line.strip()[0:-1]
    # get sample name and DSID
    thisSampleName=line.split()[0]
    isOfficialProduction=True
    if  thisSampleName in re.findall("user.*",thisSampleName):
      print("[INFO] this sample has been indentified as a user-geneated sample rather than official ATLAS production")
      isOfficialProduction=False
      # this is not an official ATLAS sample
      # so no guarantee to be able to easily find
      # a DSID where we normally expect it.
      # in this case dsid is a dummy anyway.
      # the safest thing to do is use the whole 
      # sample name (after the scope) to identify
      # the sample.
      if not ":" in thisSampleName: dsid=thisSampleName
      else: dsid=thisSampleName.split(":")[1]
    else:
      dsid=thisSampleName.split(".")[1]
    print("\n--- [INFO] processing DSID %s ---"% dsid )
    testSamplePath=None

    frac=1
    nFilesInSample, nEventsInSample=-1,-1
    if "" == fracString or fracString=="all" or fracString=="-1":
      print("[INFO] sample specified with string '%s': processing all events"%fracString )
    elif "%" in fracString:
      nFilesInSample, nEventsInSample = getTotalNEventsAndNFiles(thisSampleName)
      frac= float(fracString.replace("%",""))*0.01
      print("[INFO] sample specified with string '%s': processing %d out of %d files, so %.2f%% of files"%(fracString, int(frac*nFilesInSample),nFilesInSample, 100*(frac*nFilesInSample)/nFilesInSample) )
      opts.pathenaOptions+= " --nFiles=%d "% int(frac*nFilesInSample)
    elif "." in fracString and float(fracString)<1:
      nFilesInSample, nEventsInSample = getTotalNEventsAndNFiles(thisSampleName)
      frac= float(fracString)
      print("[INFO] sample specified with string '%s': processing %d out of %d files, so %.2f%% of files"%(fracString, int(frac*nFilesInSample),nFilesInSample, 100*(frac*nFilesInSample)/nFilesInSample) )
      opts.pathenaOptions+= " --nFiles=%d "% int(frac*nFilesInSample)
    elif ("." not in fracString) and  int(float(fracString))>1:
      events= int(float(fracString))
      nFilesInSample, nEventsInSample = getTotalNEventsAndNFiles(thisSampleName)
      frac= float(events)/nEventsInSample
      print("[INFO] sample specified with string '%s'. processing %d/%d=%.2f%% events, so %d/%d=%.2f%% of files"%(fracString, events, nEventsInSample, 100*frac, int(frac*nFilesInSample),nFilesInSample, 100*(frac*nFilesInSample)/nFilesInSample) )
      opts.pathenaOptions+= " --nFiles=%d "% int(frac*nFilesInSample)
    else:
       print("[ERROR] malformed input string: %s. Should either be an integer number of events, a float between (0,1) for the fraction of files, or a percentage of files, or 'all' (default)")  
       exit(1)

    os.system("mkdir -p %s"% opts.testSampleDir)
    testSamplePath=findTestFile(opts.testSampleDir,dsid)
    systWeights =[]
    if opts.noSyst and isRivet: 
      if float(rivet.version()[0])>=3: 
        opts.templateJobOptions="%s/data/RivetAnalysis_JO_Rivet3noSyst.py" % (submissionTemplatesDir)
      else:
        opts.templateJobOptions="%s/data/RivetAnalysis_JO_noSyst.py" % (submissionTemplatesDir)
    if not opts.noSyst:
      print("[INFO] Including the Systematic Variations stored as ME weights")
      # we are doing systematics. Can we get the ME weight names from DB?
      list_dictionary,list_keys=rDB.getWeights(dsid)
      if len(list_dictionary.keys())>0  and not opts.downloadSample:
        if not isOfficialProduction:
          print("[ERROR] your sample:", thisSampleName)
          print("[ERROR]... appears to be a user-generated dataset")
          print("[ERROR]... and will not be present in the DSID_database")
          print("[ERROR]... try again with option --downloadSamples")
          exit(1)
        print("[INFO] Obtaining ME weights from Database")
        # retrieved ME weight names from DB, use the correct template
        if isRivet: 
          if float(rivet.version()[0])>=3: 
            opts.templateJobOptions="%s/data/RivetAnalysis_JO_Rivet3.py" % (submissionTemplatesDir)
          else:
            opts.templateJobOptions="%s/data/RivetAnalysis_JO.py" % (submissionTemplatesDir)
        systWeights =[]
        for weightType, weightInfo in rDB.getWeights(dsid)[0].items():
          if isinstance(weightInfo['weights'], list):
             weights= weightInfo['weights']
          else : weights=[weightInfo['weights']]
          for iw in weights:
            if not iw in systWeights:
               systWeights.append(iw)
      else:
        print("[INFO] Obtaining ME weights from download of test file")
        # instead, download a test file of each DSID to be submitted, 
        # and manually get the list of ME weights it contains
        if isRivet: 
          if float(rivet.version()[0])>=3: 
            opts.templateJobOptions="%s/data/RivetAnalysis_JO_Rivet3.py" % (submissionTemplatesDir)
          else:
            opts.templateJobOptions="%s/data/RivetAnalysis_JO_MEfromFile.py" % (submissionTemplatesDir)
        # check if we already have a test file for that DSID
        if (testSamplePath==None):
          testSamplePath = downloadTestFile(opts.testSampleDir,thisSampleName)
        print("[SUCCESS] found test file ", testSamplePath)
        # make JOs for this DSID
        if isAthAnalysis:
          print("[INFO] attempting to retrieve weight names from metadata... this will crash if you are trying to submit EVNT files from R21 or DAOD files from R20...")
          from PyUtils import AthFile
          af = AthFile.fopen(testSamplePath) #opens the first file from the InputCollections list
          systWeights=None
          metadata = af.fileinfos['metadata']
          if '/Generation/Parameters' in metadata:
              genpars=metadata['/Generation/Parameters']
              if 'HepMCWeightNames' in genpars:
                  systWeights=genpars['HepMCWeightNames']
                  print("[SUCCESS] we found the following syst weights!", systWeights.keys())
              else:
                  print('HepMCWeightName not found in /Generation/Parameters:')
                  print(genpars)
                  #raise RuntimeError('HepMCWeightName not found in /Generation/Parameters. Exiting...')
          else:
              print('/Generation/Parameters not found in metadata:')
              print(metadata)
          

    thisSampleJobOption = opts.templateJobOptions.replace(".py","_%s_%s.py" % (dsid, opts.label)).split("/")[-1]
    os.system("cp %s %s" % (opts.templateJobOptions,thisSampleJobOption))
    systWeights=str(systWeights).replace('\'','"')
    os.system("sed -i -e 's|!SYSTWEIGHTS!|%s|g' %s" % (systWeights,thisSampleJobOption))
    os.system("sed -i -e 's|!DSID!|%s|g' %s" % (dsid,thisSampleJobOption))
    os.system("sed -i -e 's|!TESTSAMPLE!|%s|g' %s" % (testSamplePath,thisSampleJobOption))
    os.system("sed -i -e 's|!RIVETANALYSIS!|%s|g' %s" % (opts.analysis,thisSampleJobOption))
    if ":" in thisSampleName:
      outputSampleName="user.%s."% (findUserNickname()) +thisSampleName.split(":")[1].replace(".evgen.EVNT","").replace(".merge.EVNT","")+".RIVET.%s" % (opts.label)
    else:
      outputSampleName="user.%s."% (findUserNickname()) +thisSampleName.replace(".evgen.EVNT","").replace(".merge.EVNT","")+".RIVET.%s" % (opts.label)
    #pathenaCommandLine = "pathena --nFilesPerJob=100 --long  --extOutFile \*.yoda --inDS=%s  --outDS=%s --extFile=RivetAnalysis_%s.so,%s.yoda  %s" % (thisSampleName, outputSampleName, opts.analysis, opts.analysis,thisSampleJobOption )
    analysis_files=[]
    if isRivet:
      for an in opts.analysis.split(","):
        analysis_files+=["RivetAnalysis_%s.so"%an]
        analysis_files+=["%s.yoda"%an]
    analysis_files=",".join(analysis_files)
    if len(opts.extraFiles)> 0:
      if len (analysis_files): analysis_files+=","+opts.extraFiles
      else: analysis_files=opts.extraFiles
    if len(analysis_files): analysis_files="--extFile=%s"% analysis_files
    rootOrYoda = "root" if not isRivet else 'yoda'
    pathenaCommandLine ="pathena  --extOutFile \*.%s --inDS=%s  --outDS=%s %s %s %s" % (rootOrYoda, thisSampleName, outputSampleName, analysis_files, thisSampleJobOption , opts.pathenaOptions )
    if opts.dryRun:
       pathenaCommandLine += " --noSubmit"
    else:
      os.system(pathenaCommandLine)
  
if __name__ == "__main__":
  main( sys.argv[1:] )
