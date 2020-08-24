#!/usr/bin/env python
#Author: Abhishek Nag (TU Dresden)	#Co-Author: Louie Corpe (UCL)
#email:  abhishek.nag@cern.ch      	            l.corpe@cern.ch

import os, sys
import optparse
import readDatabase

parser = optparse.OptionParser(usage = "%prog [options]")
parser.add_option("-i","--inputSamples", help="list of samples. One per line. Default=Sherpa_samples.txt", dest="inputSamples", default="Sherpa_samples.txt")
parser.add_option("--testSampleDir", help="where to store the test samples. Default=testSamples", dest="testSampleDir",default="testSamples" )
parser.add_option("--weightDir", help="where to store the weight files. Default=Weight_files", dest="weightDir", default="Weight_files")
#parser.add_option("-g","--gen", help="specify the generator of the inputSamples. Options: Sherpa, Powheg, MadGraph. Default = Sherpa", dest="gen", default="Sherpa" )
(opts, args) = parser.parse_args()


from datetime import date
SystToolsPath=""
if 'SYSTTOOLSPATH' in os.environ.keys():
  SystToolsPath=os.environ['SYSTTOOLSPATH']
else:
  print("[ERROR] Environment variable SYSTTOOLSPATH  is not set. It should be set to the systematics-tools directory. Use setupSystematicsToolR21.sh")
  exit(1)
os.system("mkdir -p %s" % opts.testSampleDir)
os.system("mkdir -p %s" % opts.weightDir)

f=open(opts.inputSamples)
for line in f.readlines():
    flag=False
    if line[0]=="#" or line=='' : continue
    thisSampleName=line.split()[0]
    gen=thisSampleName.split(".")[2].split("_")[0]
    if 'Ph' in gen or 'Powheg' in gen:
        gen='Powheg'
    elif 'MG' in gen or 'MadGraph' in gen:
        gen='MadGraph'
    elif 'Sherpa' in gen or 'Sh' in gen:
        gen='Sherpa'
    elif 'aMcAtNlo' in gen:
        gen='aMcAtNlo'
    else:
        print("unrecognized generator")
        continue
    dsid=thisSampleName.split(".")[1]
    print("[INFO] processing DSID", dsid)

    dictionary, keys=readDatabase.getWeights(int(dsid))
    if keys != []:
        print("[INFO] DSID",dsid,"already exists. Skipping")
        continue
    tSample=None
    testSamplePath=None
    weightflag=False
    for dirName in os.listdir(opts.testSampleDir):
        if dsid in dirName:
            testSamplePath=opts.testSampleDir+"/"+dirName
            for fileName in os.listdir(testSamplePath):
                testSamplePath=testSamplePath+"/"+fileName
                print("[INFO] Sample found")
                flag=True
                break
            if not 'root' in testSamplePath or testSamplePath.endswith(".part"): 
                testSamplePath=None
                print("[WARNING] Sample not downloaded properly.")
                flag=False
    for filename in os.listdir(SystToolsPath+"/data/Weight_files/"):
        if dsid in filename:
            weightflag=True
    if (testSamplePath==None and weightflag==False):
        print "[INFO] do not currently have a test sample for ", thisSampleName
        rucioCommand= "rucio download --nrandom 1 %s --dir %s " % (thisSampleName,  opts.testSampleDir)
        print "[INFO] --> downloading one using this command \n ",rucioCommand
        os.system(rucioCommand)
        if ":" in thisSampleName: testSamplePath=opts.testSampleDir+"/"+thisSampleName.split(":")[1]
        else : testSamplePath=opts.testSampleDir+"/"+thisSampleName
        if not os.path.exists(testSamplePath):
            print("[ERROR INFO] No such directory: %s", testSamplePath)
            continue
        for fileName in os.listdir(testSamplePath):
            if fileName.endswith(".part"):
                print("[ERROR INFO] Downloading failed")
                continue
            tSample=testSamplePath
            testSamplePath=testSamplePath+"/"+fileName
            flag=True
            break
    #if not any(dsid in dname for dname in os.listdir(opts.weightDir)) or flag==True:
    if flag == True:
        print("[SUCCESS] found test file ", testSamplePath)
        athenaCommand= "athena --filesInput=%s --evtMax=1 DSIDDatabaseTool/DSIDDatabaseToolAlgJobOptions.py" % (testSamplePath)
        print("[INFO] running athena weight retrieving tool using \n", athenaCommand)
        os.system(athenaCommand)
        rmCommand = "rm -r %s" % (tSample)
        os.system(rmCommand)
        print("[INFO] copying weight file")
        mvCommand="mv weight.txt %s" % ('weight_'+gen+'_'+dsid+'.txt')
        os.system(mvCommand)
        cpCommand="cp %s %s/data/Weight_files/." % ('weight_'+gen+'_'+dsid+'.txt',SystToolsPath)
        os.system(cpCommand)
        commandLine="rm %s" % ('weight_'+gen+'_'+dsid+'.txt')
        os.system(commandLine)
    if flag or weightflag:
        buildCommand="buildDatabase.py %s/data/Weight_files/%s" % (SystToolsPath, 'weight_'+gen+'_'+dsid+'.txt')
        print("[INFO] updating Database for dsid", dsid)
        os.system(buildCommand)
        today = date.today()
        d1=today.strftime("%Y-%m-%d")
        copyData="cp %s/data/DSID_Database.yaml %s/data/DSID_Database_%s.yaml" % (SystToolsPath, SystToolsPath, d1)
        copyDict="cp %s/data/Weight_Database.yaml %s/data/Weight_Database_%s.yaml" % (SystToolsPath, SystToolsPath, d1)
        os.system(copyData)
        os.system(copyDict)
f.close()
