#!/bin/python
from collections import defaultdict
import csv, os, re

#input_GRL = "Runs_data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns_DESDM_ZMUMU.txt"
#input_GRL = "Runs_data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns_tolerable_IBLSTANDBY-DISABLE_DESDM_ZMUMU.txt"
#input_GRL = "Runs_data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good_25ns_DESDM_EGZ.txt"
#input_GRL = "Runs_data16_13TeV.periodAllYear_DetStatus-v82-pro20-13_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns.xml"
input_GRL = "Runs_data16_13TeV.periodAllYear_DetStatus-v82-pro20-13_DQDefects-00-02-04_PHYS_StandardGRL_All_Good_25ns_DAOD_ZMUMU_period_A-I.txt"
#import runs from periods
runListGRL = []
d = defaultdict(list)
d_all = defaultdict(list)
for file in os.listdir("./"):
    if file.startswith("runs_period"):
#        print(file)

        input = open(file, 'rb')
        reader = csv.reader(input)
        for line in reader:
            period = os.path.splitext(file)[0][11:]
 #           print period
            for run in line:
#                print run
                d.setdefault(str(period),[]).append(run)
                d_all.setdefault(str('allPeriods'),[]).append(run)

inputGRL = open(input_GRL, 'rb')
reader = csv.reader(inputGRL)
for line in reader:
#    print line

    reg = re.search('00(.+?)\.',str(line))
    if reg:
        found = reg.group(1)
#        print found
        runListGRL.append(found)
inputGRL.close()
print  " GRL : \n"+ str(runListGRL)+" entries : "+str(len(runListGRL))+"\n"
for ele in d_all:
    print  "All runs in periods \n"+str(d_all[ele]) + "  entries : "+str(len(d_all[ele]))+"\n"
    overlap  = list(set(d_all[ele]).intersection(set(runListGRL)))
    missing  = list((set(runListGRL)-set(d_all[ele])))
    print "GRL runs in all runs from period  "+str(overlap)+" entries: "+str(len(overlap))+"\n \n"
    print "Missing runs  "+str(missing)+" entries: "+str(len(missing))+"\n \n"


for entry in d:

    print "entry : "+str(entry)+" key : "+str(d[entry]) + "  entries : "+str(len(d[entry]))
    overlap  = list(set(d[entry]).intersection(set(runListGRL)))
    print "Survived in period ("+str(entry)+") using GRL "+str(overlap)+" entries: "+str(len(overlap))+"\n \n"
    output = open(str(os.path.splitext(input_GRL)[0])+"_period_"+str(entry)+".txt", 'w')
    for element in overlap:

 #       print element
        inputGRL = open(input_GRL, 'rb')
        reader = csv.reader(inputGRL)
        for line in reader:
#            print line
            if str(element) in str(line):
                print "SUCCESS : "+str(line)
                output.write(line[0]+"\n")
            else:
                continue
        inputGRL.close()

