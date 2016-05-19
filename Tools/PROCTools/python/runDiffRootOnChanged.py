#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os
import commands
from optparse import OptionParser

"""
 simple script to run diff-root only on tests which show differences in diff-pool
 either create a diff pool by hand first or just download the ESDTAGCOMM_comparison_log 
 of the current nightly:
 
 wget https://atlas-rtt.cern.ch/prod/tct/rel_3/20.1.X.Y-VAL/build/x86_64-slc6-gcc48-opt/offline/Tier0ChainTests/ESDTAGCOMM_comparison/ESDTAGCOMM_comparison_log

 then execute the script with the comparison log:

 ./runDiffRootOnChanged.py -f ESDTAGCOMM_comparison_log

"""



"""
 first evaluate last 40 lines of the diff-pool log
 and extract tests with changed

"""
def read_diff_pool_log(filename):
    changed_tests = []
    f = open(filename, 'r')
    lines = f.readlines(); f.close()
    for line in lines[-40:]:
        if "CHANGED" in line:
            changed_tests.append(line.split()[0])           
    return changed_tests


"""
  exectute diff-root command
"""
def diff_root(pool_list,test,ftype):
    outfile = "%s_%s.log" %(test, ftype)
    command =  "acmd.py diff-root %s %s --error-mode resilient --ignore-leaves RecoTimingObj_p1_HITStoRDO_timings RecoTimingObj_p1_RAWtoESD_mems RecoTimingObj_p1_RAWtoESD_timings RAWtoESD_mems RAWtoESD_timings ESDtoAOD_mems ESDtoAOD_timings HITStoRDO_timings  --entries 10  >> %s" %(pool_list[0], pool_list[1], outfile)
    if options.dryrun:
        print command
    else:
        os.system(command)



"""
  since some pool files are located on eos others are on afs
  this little hack is needed to get all files for the comparison
"""
def guessSecond(ESD_list):
    ref = ESD_list[0]
    repl = "rel%s" %(ref.split("rel")[1].split("/")[0])
    newversion = "rel_%i" %(int(repl.split("_")[1])+1)
    filename = ref.replace(repl,newversion)
    if os.path.isfile(filename):
        ESD_list.append(filename)
    return ESD_list

"""
  extract path to pool files from diff-pool log 
  skip tests with failing tests
  then execute diff_root for every pair
"""
def get_test_pool_files(logfile, tests):
    for test in tests:
        AOD_list = []
        ESD_list = []
        print 
        print
        print test
        f = open(logfile, 'r')
        for line in f:
            if ".pool.root" in line and "open" in line and test in line:
                if "AOD" in line:
                    AOD_list.append( line.split('[')[1].split(']')[0])
                elif "ESD" in line:
                    ESD_list.append( line.split('[')[1].split(']')[0]) 
        f.close()
        AOD_list = list(set(AOD_list))
        ESD_list = list(set(ESD_list))
        if len(ESD_list) == 1:
            ESD_list = guessSecond(ESD_list)
        if len(AOD_list) == 1:
            AOD_list = guessSecond(AOD_list)

        if len(ESD_list) < 2:
            print "ERROR missing ESD file for diff-root comparison"           
        else:
            print "INFO evaluate ESD diff-root"
            diff_root(ESD_list, test, "ESD")

        if len(AOD_list) < 2:
            print "ERROR missing AOD file for diff-root comparison"
        else:  
            print "INFO evaluate AOD diff-root"
            diff_root(AOD_list, test, "AOD")



def test_pool_files(logfile, tests):
    rels = ["rel_2","rel_3"]
    for test in tests:
        path = "/afs/cern.ch/atlas/project/RTT/prod/Results/tct/REL/20.1.X.Y-VAL/build/x86_64-slc6-gcc48-opt/offline/Tier0ChainTests/%s/" %(test)
        AOD_list = []
        ESD_list = []
        print 
        print
        print test
        for rel in rels:
            aod = path.replace("REL",rel)+"myAOD.pool.root"
            esd = path.replace("REL",rel)+"myESD.pool.root"
            if os.path.isfile(aod):
                AOD_list.append(aod)
            if os.path.isfile(esd):
                ESD_list.append(esd)
        AOD_list = list(set(AOD_list))
        ESD_list = list(set(ESD_list))
        if len(ESD_list) < 2:
            print "ERROR missing ESD file for diff-root comparison"           
        else:
            print "INFO evaluate ESD diff-root"
            diff_root(ESD_list, test, "ESD")

        if len(AOD_list) < 2:
            print "ERROR missing AOD file for diff-root comparison"
        else:  
            print "INFO evaluate AOD diff-root"
            diff_root(AOD_list, test, "AOD")

parser=OptionParser(usage="\n ./runDiffRootOnChanged.py --file <file name with full diff-pool log > \n")
parser.add_option("-f","--file"      ,type="string"        ,dest="filename"    ,default="none"    ,help="diff pool log file")
parser.add_option("-d","--dryRun", action="store_true"  ,dest="dryrun"  ,default=False     ,help="only dumps commands on screen")


(options,args)=parser.parse_args()
logfile = options.filename
tests = read_diff_pool_log(logfile)


print "INFO following tests changed"
for test in tests:
    print "    ",test
if tests:
    get_test_pool_files(logfile, tests)
else:
    print "All Tests are identical no further checks needed"
