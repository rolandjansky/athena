#!/bin/python
# author: steffen henkelmann
# prerequists: setup rucio
import sys,os,re,subprocess
import xml.etree.ElementTree as ET
from collections import defaultdict

#select GRL
GRL = "data16_13TeV.periodAllYear_DetStatus-v82-pro20-13_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"
dirGRL = "/afs/cern.ch/user/a/atlasdqm/grlgen/All_Good/"
datatype = "data16_13TeV"
stream = "physics_Main"
derivation = "DAOD_ZMUMU"
state = "merge"
#tag the output
tag = str(GRL) #"data16_13TeV.periodAllYear_DetStatus-v82-pro20-12_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"


runList = []
filesToRunOnGrid = []
storeEntry = ""
#fill runnumber list using the metadata of the grl
tree = ET.parse(dirGRL+GRL)
root = tree.getroot()
#choose metatag directly with help of inidices
base = root[0][5]
s_meta = base.attrib


#extract runnumber of GRL
for attrib in s_meta:
    if s_meta[attrib] == "RunList":
        print "Retrieved runlist successfully from GRL, tag name :  " + s_meta[attrib]
        runList = str(base.text).split(',')
    else:
        print "ERROR : Definition of GRL might have changed!!!"
        sys.exit(0)
print runList
#Now get DESDM_ZMUMU for each of the extracted runs
OutputFile = "Runs_"+tag+".txt"
if os.path.exists(OutputFile):
    os.remove(OutputFile)
else:
    print("Sorry, I can not remove %s file." % OutputFile)

print 'Obtain derivations from eos\n'
#check if files exist on grid and write into txt file
outputf = open(OutputFile, 'a')
fileAvailableOnGrid = 0

print 'Check if derivations are available on the grid \n'


#runList = ['302872']
#runList = ['302919']

for item in runList:
    item =  datatype + ".00" + str(item) + '.' + str(stream) + '.' +str(state) + '.' +str(derivation) + '*/'
    ds = subprocess.Popen(['rucio','list-dids', '--short', item], stdout=subprocess.PIPE)
    outds, errds = ds.communicate()
    lines = outds.splitlines()
    print lines
    if lines:
        inDS = ""
        ftags = []

        containerCands = defaultdict(dict)
        for line in lines:
            print line
            ftag = line.split('.')[-1].split('_')[0]
            #make sure ton onl check for containers, no datasets
#            if len(line.split('.')[-1].split('_')) == 3:
            ftags.append(int(filter(str.isdigit, str(ftag))))
#                print ftag, ptag
                # fill candidate container with f- and p-tags
            containerCands[int(filter(str.isdigit, str(ftag)))] = line
        if not containerCands:
            print 'ERROR :: Check if something changed in the definition of the datasets ...'
        else:
            print 'INFO :: available ftags > ',ftags
            print max(ftags)
            inDS = containerCands[max(ftags)]
#                elif sorted(set(ptags))[-2] in containerCands[max(ftags)].keys():

#                    print containerCands[max(ftags)][sorted(set(ptags))[-2]]

    else:
        print 'ERROR :: Check if the input ds is correct since no datasets are found ...'
    print 'INFO ::  Check metadata for ', inDS
    process = subprocess.Popen(['rucio','get-metadata',inDS], stdout=subprocess.PIPE)
    out, err = process.communicate()
    if re.search("availability: AVAILABLE",out):
        print "SUCCESS : "+inDS+" is available on the grid!"
        fileAvailableOnGrid +=1

 #       outputf.write("\'%s\',\n" % inDS.split(':')[1])
        outputf.write("%s \n" % inDS.split(':')[1])
        print 'INFO :: write to file ', inDS.split(':')[1]
if len(filesToRunOnGrid) == fileAvailableOnGrid:
    print 'SUCCESS : All files in GRL are available on the GRID'
else:
    print 'WARNING : Not all files available in the GRL are available on the GRID!!!'

outputf.close()
print 'SUCCESS : Wrote outputfile > '+OutputFile

