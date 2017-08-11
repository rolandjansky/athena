#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

##############################################################
#                                                            #
# ExtractEvents.py (written by C. Ohm, 2015)                 #
#                                                            #
# Wrapper script for filtering out a small number of events  #
# from ATLAS event files in the Tier-0 eos area. The script  #
# currently supports RAW, (D)ESD and (D)AOD formats.         #
#                                                            #
##############################################################

import sys, os, argparse, subprocess, fnmatch

# This is a bit ugly, but seems to be the only way for now
# See more info here: https://cern.service-now.com/service-portal/view-request.do?n=RQF0492611
#eospath = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
# Update: https://cern.service-now.com/service-portal/view-incident.do?n=INC1004372
eospath = "eos"

def validEventList(eventList):
    eventStrings = eventList.split(',')
    events = []
    for e in eventStrings:
        try:
            events.append(int(e))
        except ValueError:
            raise argparse.ArgumentTypeError("Can't convert %s to a valid event number" % e)
    return events

def validFileFormat(formatString):
    if not any(x in formatString for x in ['RAW', 'ESD', 'AOD']):
        return False
    else:
        return formatString

def extractEvents(run, events, inputFiles, format, outputFile=""):
    # form the components of the command for the final event extraction
    runEvents = []
    for e in events:
        runEvents.append((run,e))
    suffix = format
    if 'RAW' in format:
       suffix += '.data' 
    else:
        suffix += '.pool.root'
    if outputFile == "":
        outputFile = "r%08d_" % run
        if len(events) > 1:
            outputFile += "multiple_events."
        else:
            outputFile += "e%012d." % events[0]
        outputFile += suffix
    inputFilesString = ""
    for f in inputFiles:
        inputFilesString += " %s" % f
    cmd = "acmd.py filter-files -o %s -s '%s'%s" % (outputFile, str(runEvents), inputFilesString)

    # check that the output file does not already exist
    if os.path.isfile(outputFile):
        print "Output file %s already exists - please remove it or choose a different name for the output file (-o/--outputfile)" % outputFile

    # execute the command
    print "Will run the following command to start the extraction"
    print "   %s" % cmd
    extraction = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    lines_iterator = iter(extraction.stdout.readline, b"")
    for line in lines_iterator:
        print(line.rstrip())

    # if the output file is less than 500 bytes, it's probably empty
    size = os.path.getsize(outputFile)
    if size < 500:
        print "Size of output file only %d bytes, deleting it as most likely empty" % size
        os.remove(outputFile)
    else:
        print "Done. Output saved to %s (%.1f kB)" % (outputFile, size/1024.)
    if len(events) > 1:
        print "NB! You requested %d events, please check the log to see that all were found" % len(events)

def main():

    parser = argparse.ArgumentParser(description='Extract events specified by run, lumi block and event number from ATLAS event files. Written by C. Ohm - feedback is very welcome!')
    parser.add_argument('-r', '--run', required=True, type=int, help='Run number')
    parser.add_argument('-lb', '--lumiblock', required=True, type=int, help='Luminosity block number')
    parser.add_argument('-e', '--events', type=str, required=True, help='Event number(s), separated by commas if several (e.g. 12345 or 123,456,789)')
    parser.add_argument('-p', '--projecttag', type=str, nargs='?', help='Project tag, defaults to "data15_13TeV"', default='data15_13TeV')
    parser.add_argument('-s', '--stream', type=str, nargs='?', help='Stream name, defaults to "physics_Main"', default='physics_Main')
    parser.add_argument('-f', '--fileformat', type=validFileFormat, help='File format: (D)RAW(_XYZ), (D)ESD(_XYZ), (D)AOD(_XYZ)', default='AOD')
    parser.add_argument('-m', '--matchingstring', type=str, nargs='?', default='', help='String for matching the dataset to look in, useful when there are several processings available, or both merged and unmerged datasets, e.g. "*merge.AOD*f620*" will do what you think')
    parser.add_argument('-o', '--outputfile', type=str, nargs='?', help='Name for the output file (appropriate suffix based on input format will be appended automatically)')
    parser.add_argument('-v', '--verbose', action='store_true', default=False, help='Verbose mode, prints out eos paths and commands, file names, etc')
    parser.add_argument('-i', '--inputfile', nargs='+', type=str, default='', help='Local input file(s), to bypass search in EOS disk buffer')

    args = parser.parse_args()
    print args

    eventList = validEventList(args.events)

    # if the user provides input file(s), look for the event(s) in them!
    if args.inputfile != "":
        print "Will look in specified input file(s)"
        extractEvents(args.run, eventList, args.inputfile, args.fileformat)
        sys.exit(0)
        
    if args.fileformat == False:
        print "Unknown argument for -f/--fileformat - please provide a valid string describing the file format, i.e. one containing 'RAW', 'ESD' or 'AOD'"
        sys.exit(1)

    print "Will try to extract event(s) %s in LB %d of run %d (project tag: %s, stream: %s, format: %s, matching string: \"%s\")" % (args.events, args.lumiblock, args.run, args.projecttag, args.stream, args.fileformat, args.matchingstring)

    filePath = "root://eosatlas//eos/atlas/atlastier0/rucio/%s/%s/%08d/" % (args.projecttag, args.stream, args.run)
    #print filePath
    cmd = eospath+" ls "+filePath
    if args.verbose:
        print "Will run the following command to get a list of datasets matching the provided info"
        print "   %s" % cmd
    env = os.environ.copy()
    datasetListing = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, env=env)
    datasetNames = []
    if args.verbose:
        print "Datasets before requiring match with pattern (%s):" % args.matchingstring
    for line in datasetListing.stdout.readlines():
        if args.verbose:
            print "   %s" % line.rstrip()
        # skip log file tar balls
        if "LOGARC.tar" in line:
            continue
        # RAW dataset don't have anything after "..daq.RAW"
        if (".%s." % args.fileformat) in line or (args.fileformat == "RAW" and ".RAW" in line):
            datasetNames.append(line.rstrip())

    # now get rid of all datasets that don't match the matching pattern
    datasetNames = [ds for ds in datasetNames if not ".LOGARC" in ds]
    if args.matchingstring != '':
        if args.verbose:
            print "Removing datasets that don't match %s" % (args.matchingstring)
        datasetNames = [ds for ds in datasetNames if fnmatch.fnmatch(ds, "*"+args.matchingstring+"*")]
    
    if len(datasetNames) > 1:
        print "More than one dataset matching the provided info"
        for ds in datasetNames:
            print "   %s" % ds
        print "Please provide tighter constraints, e.g. by using the -m/--matchingstring option"
        sys.exit(1)

    if len(datasetNames) == 0:
        print "No dataset matching the provided info - please provide looser constraints and or use the -v/--verbose switch to see more info about what datasets are available in EOS"
        sys.exit(1)

    print "Will use the following dataset found in EOS: %s" % datasetNames[0]

    cmd = eospath+" ls "+filePath+datasetNames[0]+'/'
    if args.verbose:
        print "Will run the following command to get a list of files in the selected dataset"
        print "   %s" % cmd

    fileListing = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    fileNames = []
    for line in fileListing.stdout.readlines():
        print line
        substrings = line.split('lb')
        lbs = []
        if line.count('lb') == 2:
            lbs = range(int(substrings[1][0:4]), int(substrings[2][0:4])+1)
        else:
            lbs.append(int(substrings[1][0:4]))
        if args.lumiblock in lbs:
            fileNames.append(filePath+datasetNames[0]+"/"+line.rstrip())
    if args.verbose:
        print "Will look for the sought event(s) in the following %d file(s):" % len(fileNames)
        for file in fileNames:
            print "   %s" % file

    # if no files are found, tell the user and stop
    if len(fileNames) == 0:
	print "No files available in %s - will exit" % (filePath+datasetNames[0]+'/')
	sys.exit(1)

    # actually extract the events
    extractEvents(args.run, eventList, fileNames, args.fileformat)

if __name__ == '__main__':
    main()
