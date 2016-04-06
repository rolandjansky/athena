#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
from optparse import OptionParser    


"""
  after a list of tests is extracted from the infile
  add differences to the dicts
"""
def getResults(infile, test_dict):
    global debugmode
    for test in test_dict.keys():
        for ftype in ['ESD','AOD']:
            linenr = 0
            f = open(infile, 'r')
            doRead = False
            for line in f:
                linenr += 1
                if line.startswith('Py:diff-root         INFO  old:') and test in line and ftype in line:
                    doRead = True
                if line.startswith('Py:Acmd'):
                    doRead = False
                if doRead and ("leaves differ" in line or "WARNING" in line):
                    if line.split('\n')[0] not in test_dict[test][ftype]:
                        test_dict[test][ftype].append(line.split('\n')[0])
                ## Print line with line number for debug
                if debugmode and doRead:
                    print "DEBUG: %s: %s" %(linenr,line.split('\n')[0])

    return  test_dict

"""                
  create initial dict including all tests which have been probed
"""
def getTests(infile, selected_test):
    test_dict = {}
    f = open(infile, 'r')
    for line in f:
        if "CHANGED" in line or "IDENTICAL" in line:
            test = line.split()[0]
            if selected_test and test not in selected_test:
                continue
            test_dict[test] = { 'ESD' : [],
                                'AOD' : [],
                                'status' : line.split()[1]}
    f.close()
    return  test_dict
    

"""
 prints sorted summary of tests
"""
def reportOverview(test_dict): 
    list_sorted = sorted(test_dict.keys())
    for test in list_sorted:
        #print "%20s %s" %(test, test_dict[test]['status'])
        print '{0:50}  {1:10}'.format(test, test_dict[test]['status'])


"""
  prints detailed diff on screen
"""
def reportDiffs(test_dict):
    list_sorted = sorted(test_dict.keys())
    for test in list_sorted:
        if test_dict[test]['status'] == 'IDENTICAL':
            continue      
        print test, test_dict[test]['status']
        for item in ['ESD','AOD']:
            print item
            for line in test_dict[test][item]:
                print line
            print
        print


"""
  print all
"""
def printDict(test_dict):
    print "Overview:"
    print "=========="
    reportOverview(test_dict)
    print
    print
    print
    print "Details"
    print "========"
    reportDiffs(test_dict)




"""
  Main function here
"""
if __name__ == "__main__":
    parser=OptionParser(usage="\n ./sstat \n")
    parser.add_option("-f","--file",type="string"        ,dest="infile"    ,default="ESDTAGCOMM_comparison_diffroot_log" ,help="inputfile diff-root")
    parser.add_option("-t","--test"  ,type="string"        ,dest="test"   ,default=None                        ,help="condensed output file")
    #parser.add_option("-o","--outfile"  ,type="string"        ,dest="outfile"   ,default="summary.txt"                        ,help="condensed output file")
    parser.add_option("-d", "--debug", action="store_true", dest="verbose", default=False, help="print extra DEBUG messages")
    (options,args)=parser.parse_args()
    
    debugmode = options.verbose

    infile_name = 'ESDTAGCOMM_comparison_diffroot_log'
    
    selected_tests = None
    if options.test:
        selected_tests = options.test.split(",")

    # first create the dict of tests
    test_dict = getTests(options.infile,  selected_tests)
    # then overwrite the format types with diff info
    test_dict = getResults(options.infile,test_dict )
    
    
    ## finally print the results
    printDict(test_dict)
