#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#################################################################################################################
# B. Martin - 26/01/2017
# 
# This script checks whether a DAOD was produced with the full statistics of the corresponding xAOD.
# python checkDAODstats.py list.txt
# where list.txt is a list of MC15 DAODs 
#
# Don't forget to setup pyAMI...
#
# export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
# source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
# lsetup pyami
#
#################################################################################################################

# argument for the SQL request
def MakeArg( DSidentifier, InputType):

    arg='-gLiteEntity=\"prodsys_task\" -gLiteQuery=\"SELECT prodsys_task.prodsysIdentifier, prodsys_task.nInputEvents, prodsys_task.nInputEventsUsed, prodsys_task.datasetFK, prodsys_task.identifier , prodsys_task.taskStatus WHERE dataset.identifier=\'%s\'\", -processingStep=\"production\" -project=\"mc15_001\" -sql=\"SELECT prodsys_task.prodsysIdentifier, prodsys_task.nInputEvents, prodsys_task.nInputEventsUsed, prodsys_task.datasetFK, prodsys_task.identifier ,\'mc15_001\' as PROJECT,\'production\' as PROCESS, \'prodsys_task\' as AMIENTITYNAME, prodsys_task.identifier as AMIELEMENTID , FROM dataset,prodsys_task WHERE (dataset.identifier IN (SELECT dataset.identifier FROM dataset WHERE dataset.identifier = \'%s\')) AND (dataset.identifier = prodsys_task.datasetFK ) AND (\'DATA_DELETED\' <> prodsys_task.taskStatus)\" -stats=\"prodsys_task.%s\"' % (DSidentifier, DSidentifier, InputType)

    return arg


from sys import argv
import pyAMI.client

client = pyAMI.client.Client('atlas')

if len(argv) != 2:
    print "Error: please pass the script an input file containing a list of MC15 DAODs:"
    print "CheckDAODCompleteness.py list_of_datasets.txt"
    exit(0)

print "Reading list of DAODs from file %s" % argv[1]

infile = open(argv[1],"r")

import time
outfilenom = 'for_reprocessing_'+time.strftime('%y%m%d_%H%M%S')+'.txt'
outfile = open(outfilenom,'w')
found_a_file = False

for asample in infile.readlines(): 
    # Get rid of comments
    sample = asample.split('#')[0].strip()
    # Skip blank linkes
    if ''==sample: continue

    if 'mc15_13TeV' not in sample or '_p' not in sample or not 'merge.DAOD_' in sample:
        print "unexpected input %s, the script only runs over MC15 DAODs" % sample
        continue

    # strip mc15_13TeV: if present
    sample = sample.replace('mc15_13TeV:','')

    print 'Checking sample',sample

    result_DAOD = client.execute(['GetDatasetInfo', '-logicalDatasetName=%s'%sample], format = 'dict_object')

    identifier=result_DAOD.get_rows('Element_Info')[0]['identifier']

    # SQL query to retrieve nInputEvents and nInputEventsUsed for that DAOD
    nInputEvents_DAOD = int(client.execute(['SearchQuery', MakeArg(identifier,'nInputEvents')], format = 'dict_object').get_rows('Element_Info')[0]['SUM'])
    nInputEventsUsed_DAOD = int(client.execute(['SearchQuery', MakeArg(identifier,'nInputEventsUsed')], format = 'dict_object').get_rows('Element_Info')[0]['SUM'])
    
    # this checks whether some derivation jobs have failed
    if nInputEventsUsed_DAOD < nInputEvents_DAOD:
        print "\nWarning: nInputEvents_DAOD = %i, nInputEventsUsed_DAOD = %i, likely some failed derivation jobs\n"%(nInputEvents_DAOD,nInputEventsUsed_DAOD)
    elif nInputEventsUsed_DAOD > nInputEvents_DAOD:
        print "\nERROR: INCONSISTENT RESULT, nInputEvents_DAOD = %i, nInputEventsUsed_DAOD = %i\n"%(nInputEvents_DAOD,nInputEventsUsed_DAOD)

    # retrieve number of events in xAOD
    result_xAOD = client.execute(['GetDatasetProv', '-logicalDatasetName=%s'%sample], format = 'dict_object')
    nEvents_xAOD = int(result_xAOD.get_rows('node')[1]['events'])

    # this checks whether the derivation job was run over the full xAOD
    if nInputEvents_DAOD < nEvents_xAOD:
        print "\nWARNING !!! NOT ALL xAOD EVENTS WERE PROCESSED: number of events in xAOD = %i, number of input events for DAOD = %i"%(nEvents_xAOD,nInputEvents_DAOD)
        print "This DAOD should be reprocessed (with the same ptag) to pick up the remaining statistics\n"
        outfile.write(sample+'\n')
        found_a_file=True
    elif nInputEvents_DAOD > nEvents_xAOD:
        print "\nERROR: INCONSISTENT RESULT, number of event in xAOD = %i, number of input events for DAOD = %i\n"%(nEvents_xAOD,nInputEvents_DAOD)

infile.close()
outfile.close()

if found_a_file:
    print 'You can find all samples that should be reprocessed in the file',outfilenom
    print 'Please request derivations for these samples at http://its.cern.ch/jira/browse/ATLSUSYDPD'
else:
    import os
    os.remove( outfilenom )

