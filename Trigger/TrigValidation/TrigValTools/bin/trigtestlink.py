#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os,re,sys

if len(sys.argv)==1:
    print "This creates a soft link to the specified test in the nightly directory\n"
    print "%s <testname>   ... to link a test" % os.path.basename(sys.argv[0])
    print "%s -l           ... to show all tests" % os.path.basename(sys.argv[0])
    sys.exit(1)

testname = sys.argv[1]

path = os.environ['PATH']

projects = '|'.join(["AtlasP1HLT","AtlasCAFHLT","AtlasTestHLT","AtlasTrigger","AtlasAnalysis","AtlasHLT","AtlasProduction"])

for p in path.split(':'):

    #m = re.match('(?P<base>/afs/cern.ch/atlas/software/.*\.X.*/(?:%s)/.*)/InstallArea/share/bin' % projects, p)
    m = re.match('(?P<base>/afs/cern.ch/atlas/software/.*/(?:%s)/.*)/InstallArea/share/bin' % projects, p)
    if not m: continue
    nicosarea = "%s/NICOS_area" % m.groupdict()['base']

    for dirname, subdirs, files in os.walk(nicosarea):

        basename = os.path.basename(dirname)

        if basename == "NICOS_area":
            # pick out the atntest area (only exists for nigthly releases)
            subdirs[:] = [x for x in subdirs if re.match("NICOS_atntest.*",x)]
            if len(subdirs)>0:
                atnarea = subdirs[0] # usually the opt build
            else:
                atnarea = ""
            continue

        if basename == atnarea:
            subdirs[:] = ["trigp1test_testconfiguration_work", "triggertest_testconfiguration_work", "triganalysistest_testconfiguration_work"]
            continue


        if basename in ["trigp1test_testconfiguration_work", "triggertest_testconfiguration_work", "triganalysistest_testconfiguration_work"]:
            if testname == "-l":
                conf = { "trigp1test_testconfiguration_work" : "TrigP1Test.conf",
                         "triggertest_testconfiguration_work" : "TriggerTest.conf",
                         "triganalysistest_testconfiguration_work" : "TrigAnalysisTest.conf" }[basename]
                # list all tests
                print "\n"+basename+":"
                print len(basename)*"=" + "="
                w = max(len(x) for x in subdirs)
                for test in sorted(subdirs):
                    print "%-*s   ==>  trigtest.pl --cleardir --test %s --rundir %s --conf %s" % (w, test,test,test,conf)
                continue

        if basename == testname:
            try:
                os.symlink(dirname,basename)
                print "Linking to",dirname
            except OSError, e:
                print "File can't be linked, because:",e
                sys.exit(1)

            sys.exit(0)
    

if testname != "-l":
    print "Did not find %s in %s/%s/trig{p1,ger,analysis}test_testconfiguration_work" % (testname,nicosarea,atnarea)
