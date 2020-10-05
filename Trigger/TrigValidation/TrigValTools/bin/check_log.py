#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import re
import argparse
import sys
import os

desc = 'Tool to check for error messages in a log file. By default ERROR, FATAL \
  and CRITICAL messages are considered. The config file may be used to \
provide patterns of lines to exclude from this check - known problems or false positives. \
If no config file is provided, all errors will be shown.'

# Error keywords
errorRegex = [
    r'^ERROR ', ' ERROR ', ' FATAL ', 'CRITICAL ', 'ABORT_CHAIN',
    r'^Exception\:',
    r'^Caught signal',
    r'^Core dump',
    r'inconsistent use of tabs and spaces in indentation',
    r'glibc detected',
    r'tcmalloc\: allocation failed',
    r'athenaHLT.py\: error',
    r'HLTMPPU.*Child Issue',
    r'HLTMPPU.*Configuration Issue',
    r'There was a crash',
    r'illegal instruction',
    r'failure loading library',
    r'Cannot allocate memory',
    r'in state: CONTROLREADY$',
    r'^\s*missing data: ',
    r'^\s*can be produced by alg(s): ',
    r'pure virtual method called'
]

# Add list of all builtin Python errors
builtins = dir(locals()['__builtins__'])
builtinErrors = [b for b in builtins if 'Error' in b]
errorRegex.extend(builtinErrors)

# Traceback keywords
traceback = [
    r'Traceback',
    r'Shortened traceback',
    r'stack trace',
    r'^Algorithm stack',
    r'^#\d+\s*0x\w+ in '
]
errorRegex.extend(traceback)

# Warning keywords
warningRegex = ['WARNING ']

def main():
    parseOptions()
    parseConfig()
    scanLogfile()
    printResults()

def parseOptions():
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('logfile',metavar='<logfile>')
    parser.add_argument('--config',
    metavar='<file>',
    type = str,
    help ='specify config file',
    )
    parser.add_argument('--showexcludestats',
    action = 'store_true',
    default = False,
    help ='print a summary table of the number of times each of the exclude patterns was matched (default False)'
    )
    parser.add_argument('--printpatterns',
    action = 'store_true',
    default = False,
    help ='print the list of warning/error patterns being searched for (default False)'
    )
    parser.add_argument('--warnings',
    action = 'store_true',
    default = False,
    help ='check for WARNING messages'
    )
    parser.add_argument('--errors',
    action = 'store_true',
    default = False,
    help = 'check errors'
    )
    global args
    args = parser.parse_args()
    if not (args.errors or args.warnings):
        print('error: at least one of errors and warnings must be enabled')
        sys.exit(4)

# Parses the config file provided into a list (ignorePattern)
# If no config file is used, all results will be printed to the terminal
def parseConfig():
    global ignorePattern
    global noConfig
    ignorePattern = []
    # check for conf file locally, otherwise download using get_files
    if args.config:
      if not os.path.isfile(args.config):
          os.system("get_files -data -symlink " + args.config)
    try:
        with open(args.config,'r') as configFile:
            print('Ignoring warnings/error patterns defined in ' + args.config)
            for aline in configFile:
                if 'ignore' in aline:
                  line = aline.strip('ignore').strip()
                  if line.startswith('\'') and line.endswith('\''):
                      line = line[1:-1]
                  ignorePattern.append(line)
        noConfig = False
    except Exception:
      print('No config file, all warnings/errors will be printed')
      noConfig = True

def scanLogfile():
    resultsA =[]
    global pattern
    pattern = []
    tPattern = re.compile('|'.join(traceback))
    global msgLevels
    global logFileAddress
    if args.warnings is True:
        pattern = warningRegex
    if args.errors is True:
        pattern = errorRegex
    msgLevels = re.compile('|'.join(pattern))
    igLevels = re.compile('|'.join(ignorePattern))
    logFileAddress = args.logfile
    with open(logFileAddress,'r', encoding='utf-8') as logFile:
        tracing = False
        for line in logFile:
            #Tracing only makes sense for errors
            if args.errors is True and re.search(tPattern,line):
                tracing = True
            elif line =='\n':
                tracing = False
            if re.search(msgLevels,line):
                resultsA.append(line)
            elif tracing:
                # This currently prints all lines after a traceback even if they don't belong to traceback
                resultsA.append(line)

    if args.showexcludestats and not noConfig:
        separateIgnoreRegex = [re.compile(line) for line in ignorePattern]
        global ignoreDict
        ignoreDict = {line:0 for line in ignorePattern} # stores counts of ignored errors/warnings
    global results
    results = []
    if noConfig:
        results = resultsA
        return
    for res in resultsA:
        if not re.search(igLevels,res):
            results.append(res)
        elif args.showexcludestats:
            for i in range(len(separateIgnoreRegex)):
                if re.search(separateIgnoreRegex[i],res):
                    ignoreDict[ignorePattern[i]] += 1


def printResults():
    global pattern
    if args.printpatterns:
        print('check_log.py - Checking for: '+ str(pattern) +' in '+logFileAddress+'\n')
    if args.showexcludestats and not noConfig:
        print('Ignored:')
        for s in ignoreDict:
            if ignoreDict[s] > 0:
                print(str(ignoreDict[s]) + "x " + s)
        print('\n')
    print('Found messages in ' + logFileAddress + ' (' + str(len(results)) + '):')
    if len(results) > 0:
        for msg in results: print(msg.strip('\n'))
        print("FAILURE : error/fatal found in " + logFileAddress)
        sys.exit(10)

if __name__ == "__main__":
    main()

