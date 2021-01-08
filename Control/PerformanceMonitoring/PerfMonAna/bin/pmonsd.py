#!/usr/bin/env python3

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
__doc__   ='Script for parsing and basic analysis of Semi-Detailed PerfMon (PMonSD) output. More info at https://twiki.cern.ch/twiki/bin/viewauth/Atlas/PerfMonSD'
__author__='Thomas Kittelmann <thomas.kittelmann@cern.ch>'

import sys

def _usage(ec=0):
    import os
    appname=os.path.basename(sys.argv[0])
    print( "Usage, one of the following:" )
    print( )
    print( "1) List most significant performance differences in PMonSD outputs:" )
    print( "   <EXPERIMENTAL FEATURE>" )
    print( "   %s --diff INFILE1 INFILE2"%appname )
    print( )
    print( "2) Parse PMonSD text output and create pickle file:" )
    print( "   %s --parse INFILE [--output=OUTFILE[.psd[.gz]]]"%appname )
    print( )
    print( "3) Print content in ASCII format:" )
    print( "   %s --print INFILE"%appname )
    print( )
    print( "4) Print content as a raw python dictionary:" )
    print( "   %s --print-raw INFILE [KEYS]"%appname )
    print( )
    print( "INFILE  : can be a logfile, a PerfMon tar-ball with PMonSD info inside, or a pickle file" )
    print( "          produced earlier from one of those by this script. Infiles can be gzipped if" )
    print( "          they have the .gz extension. PerfMon tar-balls must have .pmon.gz extension," )
    print( "          and Pickle files must have a .psd or .psd.gz extension." )
    print( "OUTFILE : File for storing parsed information in python pickle format." )
    print( "KEYS : Specify a list of keys to \"dive into\" the extracted dictionary" )
    print( )
    print( "Note that if you prefer to work entirely in python, you can access all of the above" )
    print( "functionality through the module PerfMonComps.PMonSD and the functions therein:" )
    print( "PMonSD.parse(infile,outfile=None), PMonSD.print_ascii(infile) and PMonSD.diff(infile1,infile2)." )
    sys.exit(ec)

def main(args):
    import PerfMonComps.PMonSD
    if '-h' in args or '--help' in args:
        _usage(0)
    n=len(args)
    if n==3 and args[0]=='--diff':
        PerfMonComps.PMonSD.diff(args[1],args[2])
        #todo: diff should return False if failures and we use that as exit code.
        #Differences in the two files is not a failure.
        return 0
    elif n in [2,3] and args[0]=='--parse':
        if n==3: 
            outfile=args[2]
        else: 
            outfile=args[1]
        PerfMonComps.PMonSD.parse(args[1],outfile)
        return 0
    elif n==2 and args[0]=='--print':
        if not PerfMonComps.PMonSD.print_ascii(args[1]):#todo: actually return false in case of problems
            return 1
        return 0
    elif n>=2 and args[0]=='--print-raw':
        keys=args[2:]
        d=PerfMonComps.PMonSD.parse(args[1])
        #1) Select which dictionary to proceed with:
        if keys and keys[0].isdigit():
            #list index 
            keys[0]=int(keys[0])
            if keys[0]>=len(d):
                print( "ERROR: Index out of range: %i (Only found %i PMonSD summaries in input)"%(keys[0],len(d)) )
                return 1
            d=d[int(keys[0])]
            keys=keys[1:]
        elif len(d)>1:
            print( "Parsed list of length %i. Specify index (0,1,...) to pick out specific dictionary"%len(d) )
            return 0
        elif len(d)==1:
            d=d[0]
        else:
            print( "Did not parse any PMonSD info" )
            return 1
        #Dive in, according to keys (all strings we assume):
        while keys:
            k=keys.pop(0)
            if not type(d)==dict:
                print( "ERROR: Can't dive further into dictionary. Remaining objects are:" )
                print( '  '+str(d) )
                return 1
            if k not in d.keys():
                print( "ERROR: Invalid key '%s'. Valid keys are:" )
                print( '  '+str(d.keys()) )
                return 1
            d=d[k]    
        print( d )
        if type(d)==dict:
            print( "Next keys: %s"%str(d.keys()) )
        return 0
    elif n in [2,3] and args[0]=='--validate':
        #hidden feature to use by validation scripts.
        #The idea is that the first file is a text file and the second
        #file is a pickle with result of previous parsings.
        #a) Test that we didn't change results from parsing:
        if n==3:
            if not PerfMonComps.PMonSD._validate_identical(args[1],args[2]):
                print( 'ERROR: Detected differences in information loaded from %s and %s'%(args[1],args[2]) )
                return 1
        #b) Test that we can reproduce the output with the deparsing ability:
        if not PerfMonComps.PMonSD._validate_deparsing(args[1]):
            print( 'ERROR: Errors detected in deparsing of %s'%args[1] )
            return 1
        return 0
    _usage(1)

if __name__=='__main__':
    sys.exit(main(sys.argv[1:]))
