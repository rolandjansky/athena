#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#******************************************
#A script to show the status of samples by James Catmore with some additional modifications by Francesco Guescini.
#Before running the script do:
#  voms-proxy-init -voms atlas
#  lsetup pyami
#Example: python listStatus.py -t p3372 -s data17_13TeV.%DAOD_EXOT%

#This will print out the statuses of the matching datasets.
# - ALL EVENTS AVAILABLE: everything was processed and the jobs have finished. These are ready for full-statistics analysis.
# - EVENTS AVAILABLE: WAIT FOR MORE: tasks are still running.
# - NO EVENTS AVAILABLE: tasks didn't start yet.
# - EVENTS PARTIALLY AVAILABLE: jobs had to be stopped due to errors and not all data was read. We will follow up this final category.

#******************************************
__version__ = '0.0'

#******************************************
#import stuff
import pyAMI.atlas.api as atlasAPI
import pyAMI.client, argparse

#******************************************
def listStatus(args):

    #setup AMI
    client = pyAMI.client.Client('atlas')
    atlasAPI.init()

    #variables
    datasets = []
    tagCounter = 0

    #loop over tags
    for tag in args.tags:
        tagCounter = tagCounter+1
        searchPatterns = []
        searchPatterns.append(args.string+tag)
        print "\nrunning query for string",args.string+tag,"(",tagCounter,"of",len(args.tags),")"
        for x in range(0,10):
            theseDatasets = atlasAPI.list_datasets(
                client,
                patterns=searchPatterns,
                fields=['prodsys_status','type'],
                limit=[x*10000,10000])
            if len(theseDatasets)==0:
                break
            else:
                print 'adding', len(theseDatasets), ' datasets from rowNum=', x*10000,  ' to global result'
            datasets = datasets + theseDatasets

        nDatasets = len(datasets)
        print "total number of datasets to process = ",nDatasets

    #loop over datasets
    i=0
    formatPTagDict = {}
    for dataset in datasets:
        formatType = dataset['type']
        name = dataset['ldn']
        ptag = name[-5:]
        prodsysStatus = dataset['prodsys_status']
        formatPTag = formatType,ptag
        if formatPTag in formatPTagDict.keys():
            prodsysStatusDict = formatPTagDict[formatPTag]
            if prodsysStatus in prodsysStatusDict.keys():
                prodsysStatusDict[prodsysStatus] += 1
            else:
                prodsysStatusDict[prodsysStatus] = 1
        if formatPTag not in formatPTagDict.keys():
            formatPTagDict[formatPTag] = {prodsysStatus:1}

    #print results
    print
    for key in formatPTagDict.keys():
        print "***********"
        print key[0],key[1]
        for statKey in formatPTagDict[key].keys():
            print "\t",statKey,(formatPTagDict[key])[statKey]

#******************************************
if __name__ == '__main__':

    #parse input arguments
    parser = argparse.ArgumentParser(epilog='example: python listStatus.py -t p3372 -s data17_13TeV.%DAOD_EXOT%')
    parser.add_argument('-t', '--tags', dest='tags', type=str, nargs='+', default='', required=True, help='AMI tag') #default='p3372'
    parser.add_argument('-s', '--string', dest='string', type=str, default='', required=True, help='base string (use %% for wildcards)') #default='data17_13TeV.%DAOD_%'
    args = parser.parse_args()
    
    listStatus(args)
    print
