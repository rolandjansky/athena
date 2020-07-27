#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import re
import argparse
import sys
import os 

def main():
    commandLine()
    global status
    status = regtest()
    sys.exit(status)

def progErrorExit(message, exitcode):
    try:
        failkey
    except NameError:
        failkey = 'FAILURE'
    print('regtest.py: %s %s' % (failkey, message))
    sys.exit(exitcode)
        
def commandLine():
    global debug
    global inputfile
    global failkey
    global linematch
    global reffile
    debug = False
    inputfile = ''
    reffile = ''
    failkey = 'FAILURE'
    linematch = 'REGTEST'
    parser = argparse.ArgumentParser(description = 'usage')
   
    parser.add_argument('--debug', 
                        action = 'store_true',
                        default = False,
                        help = 'print debug information, for the script maintainer'       
                        )
    
    parser.add_argument('--failkey',
                        type = str,
                        default = 'FAILURE',
                        help =''' keyword to be printed to indicate to the test framework that a
                               test has failed, default FAILURE'''
                        )
   
    parser.add_argument('--linematch',
                         default = "REGTEST",
                         type = str,
                         help = ' only compare lines which match this string default: REGTEST'
                        )

    parser.add_argument('--inputfile',
                        metavar = '<file>',
                        type = str,
                        help = 'specify (athena) log file to take as input'
                        )

    parser.add_argument('--reffile',
                        metavar = '<file>',
                        type = str,
                        help = 'specifiy reference file, with absolute or reletive path'
                        )
    global args
    args = parser.parse_args()
    if help is True:
        usage()
        progErrorExit('usage', -1)

def usage():
    print('''
 Usage: regtest.py [options] 

  Testing tool for comparing marked lines in a log file against a reference

  Options:

  --help                show this information
  --debug               print debug information, for the script maintainer
  --failkey <string>    keyword to be printed to indicate to the test 
                        framework that a test has failed, default ''', failkey,'''
  --inputfile <string>  specify (athena) log file to take as input
  --reffile <string>    specifiy reference file, with absolute or reletive path
  --linematch <string>  only compare lines which match this string
                        default: REGTEST
                       
 Example to match a particular algorithm:
                       'TrigJetRec_Cone.+REGTEST'

  Return codes: False = success, True = failure of some sort.

  Technical info:

  Lines which match the regular expression
  ''')

def regtest():
    with open(args.inputfile,'r') as inpfile:
        matchline = 0
        for line in inpfile:
            if re.search('REGTEST',line):
                matchline += 1
   
    if matchline == 0:
        print('=== Alert!', failkey, 'no lines matching', linematch, 'in LOG')
        result = True
        exit()

    if debug is True:
       print('regtest.py,: debug: diff -b', args.inputfile, args.reffile)
  
    command = 'diff -b ' +  args.inputfile + ' ' +  args.reffile
    rc = os.system(command)
    if rc is False:
       print('=== Output is the same as reference.')
#       result = False
       result = 0
    else:
        print('''=== Alert!''', failkey, '''input file (<) differs from reference (>)
        If this change is understood, to update the reference file please type:
        cp ''', args.inputfile, args.reffile)
        #        result = True
        result = 1


    if debug is True:
        print('regtest.py  debug: returning result', result)
    return result

if __name__ == '__main__':
    main()
