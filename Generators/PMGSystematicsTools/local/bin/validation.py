#!/usr/bin/env python
#Author: Abhishek Nag (TU Dresden)
#email:  abhishek.nag@cern.ch

import os, sys
import optparse
from itertools import chain
sys.path.append('$TestArea/../')
import readDatabase

def file_is_empty(path):
	return os.stat(path).st_size==0

def getDAOD(list_DAOD):
    DAOD_types=[]
    for d in list_DAOD:
        DAOD_types.append(d.split('.')[4])
    short_DAOD=list(set(DAOD_types))
    return short_DAOD

def validate(testSampleDir, thisSampleName, testSamplePath, weight_database, outputSamples):
    for dirName in os.listdir(testSampleDir):
        if thisSampleName.split(":")[1] in dirName:
            testSamplePath=testSampleDir+"/"+dirName
    if (testSamplePath==None):
        print("[INFO] do not currently have a test sample for ", thisSampleName)
        rucioCommand= "rucio download --nrandom 1 %s --dir %s " % (thisSampleName, opts.testSampleDir)
        print("[INFO] --> downloading one using this command \n ",rucioCommand)
        os.system(rucioCommand)
    testSamplePath=testSampleDir+"/"+thisSampleName.split(":")[1]
    if not os.path.exists(testSamplePath):
        print("[ERROR INFO] No such directory: %s OR downloading Failed" % (testSamplePath))
        return
    if not os.listdir(testSamplePath):
        print("[ERROR INFO] Downloading Failed for ", testSamplePath)
        rmcomand="rm -r %s" % (testSamplePath)
        return
    for fileName in os.listdir(testSamplePath):
        if fileName.endswith('part'):
            print("[ERROR INFO] Downloading Failed for ", testSamplePath)
            rmdir="rm -r %s" % (testSamplePath)
            os.system(rmdir)
            return
        testSamplePath=testSamplePath+"/"+fileName
        break
    print("[SUCCESS] found test file ", testSamplePath)
    athenaCommand= "athena --filesInput=%s --evtMax=1 MyPackage/MyPackageAlgJobOptions.py" % (testSamplePath)
    print("[INFO] running athena weight retrieving tool using \n", athenaCommand)
    os.system(athenaCommand)
    if not os.path.exists('weight.txt'):
        print('[INFO] Athena Tool failed to retrieve weights')
        return
    wfile=open('weight.txt', 'r')
    ofile=open(outputSamples, 'a+')
    weight_file=wfile.read().splitlines()
    if set(weight_database) == set(weight_file):
        print('%s is Validated' % (thisSampleName))
        ofile.write(thisSampleName.split(":")[1]+' '+'PASSED'+'\n')
    else:
        print('%s is NOT Validated' % (thisSampleName))
        ofile.write(thisSampleName.split(":")[1]+' '+'FAILED')
    wfile.close()
    ofile.close()
    rmcommand="rm weight.txt"
    os.system(rmcommand)
    rmsample="rm -r %s" % (testSampleDir+"/"+thisSampleName.split(":")[1])
    os.system(rmsample)

parser = optparse.OptionParser(usage = "%prog [options]")
parser.add_option("-i","--inputSamples", help="list of samples. One per line", dest="inputSamples", default="../toValidate_samples.txt")
parser.add_option("--testSampleDir", help="where to store the test samples.", dest="testSampleDir",default="../validSamples" )
parser.add_option("-o","--outputSamples", help="list of validated samples. One per line, with tag as PASSED/FAILED", dest="outputSamples", default="../validated_samples.txt")
(opts, args) = parser.parse_args()

os.system("mkdir -p %s" % opts.testSampleDir)
f=open(opts.inputSamples)
for line in f.readlines():
    weight_database=[]
    if line[0]=="#" or line=='' : continue
    SampleName=line.split()[0]
    dsid=SampleName.split(".")[1]
    dictionary, keys = readDatabase.getWeights(dsid)
    if keys==[]:
        print('[FAIL] DSID %s not found in Database' % (dsid))
        continue
    for key in keys:
        weight_database.append(dictionary[key]['weights'])
    weight_database=list(chain.from_iterable(weight_database))
    testSamplePath=None
    if 'DAOD' in SampleName:
        ofile=open(opts.outputSamples, 'a+')
        if any(SampleName.split(":")[1] in oline for oline in ofile.readlines()):
            print('[INFO] sample already checked')
            continue
        ofile.close()
        validate(opts.testSampleDir, SampleName, testSamplePath, weight_database, opts.outputSamples)
        continue
    ruciolist = "rucio list-dids %s*DAOD* --filter type=CONTAINER --short > list_DAOD.txt" % (SampleName.split("_")[0]+'_'+SampleName.split("_")[1]+'_'+SampleName.split("_")[2])
    ruciolist=ruciolist.replace('mc15','mc16')
    print("[INFO] rucio listing DAODs using this command \n ", ruciolist)
    os.system(ruciolist)
    if file_is_empty('list_DAOD.txt'):
        print('No DAODs found for the given sample: ', SampleName)
        continue
    l=open('list_DAOD.txt', 'r')
    list_DAOD=l.readlines()
    shortDAOD=getDAOD(list_DAOD)
    for DAOD in shortDAOD:
        d_list=list(element for element in list_DAOD if DAOD in element)
        d_list.sort()
        thisSampleName= d_list[-1].split()[0]
        testSamplePath=None
        ofile=open(opts.outputSamples, 'a+')
        if any(thisSampleName.split(":")[1] in oline for oline in ofile.readlines()):
            print('[INFO] sample already checked')
            continue
        ofile.close()
        validate(opts.testSampleDir, thisSampleName, testSamplePath, weight_database, opts.outputSamples)
f.close()
