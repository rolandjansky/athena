#!/usr/bin/env python

#
#  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
import pickle
import pprint
import json
import sys
import argparse

parser = argparse.ArgumentParser(description='Utility to transform/display athena configurations')
parser.add_argument('--print', dest="pr", action='store_true', help='Prints')
parser.add_argument('--diff', dest="diff", action='store_true', help='Diffs two files')
parser.add_argument('--toJSON', help='Convert to JSON file')
parser.add_argument('file', nargs='+', help='Files to work with')
args = parser.parse_args()



def __loadSingleFile( fname ):    
    conf = []
    if fname.endswith( ".pkl" ):
        input_file = file( fname )
        conf = []
        while True:
            try:
                conf.append( pickle.load( input_file ) )
            except EOFError:
                break
        print "... Read", len(conf), "items from python pickle file: ", fname

    if fname.endswith( ".json" ):

        def __keepPlainStrings(element):
            if isinstance(element, unicode):
                return str(element)
            if isinstance(element, list):
                return [ __keepPlainStrings(x) for x in element ]
            if isinstance(element, dict):
                return dict( [ (__keepPlainStrings(key), __keepPlainStrings(value)) for key,value in element.iteritems() ] )
            return element

        conf = json.load( file( args.file[0] ), object_hook=__keepPlainStrings )
        print "... Read", len(conf), "items from JSON file: ", fname
    return conf

def __print( fname ):
    conf = __loadSingleFile( fname )
    print "... ", fname, "content"
    for n,c in enumerate(conf):
        print "Item", n, "*"*80
        pprint.pprint(dict(c))
    print "... EOF", fname

def __diff():
    def __merge( c ):
        confdict = {}
        [ confdict.update( el ) for el in c  ]
        return confdict

    confs = [ __merge( __loadSingleFile( f )) for f in args.file ] 
    
    allkeys = set( confs[0].keys() + confs[1].keys())

    for comp in allkeys:
        if comp not in confs[0]:
            print "... component %-54s" % comp, "absent in", args.file[0]
        elif comp not in confs[1]:
            print "... component %-54s" % comp, "absent in", args.file[1]
        elif confs[0][comp] != confs[1][comp]:
            print "... component %-54s" % comp, "present in both files but with different settings"
            print "... in file: ", args.file[0]
            pprint.pprint( confs[0][comp])
            print "... in file: ", args.file[1]
            pprint.pprint( confs[1][comp])
        else:
            print "... component %-54s" % comp, "identical"


if args.pr:
    for fileName in args.file:
        __print( fileName )

if args.toJSON:
    if len( args.file ) != 1:
        print "ERROR, can convert single file at a time, got: ", args.file
        sys.exit( -1 )
    conf = __loadSingleFile( args.file[0] )
    oFile = open( args.toJSON, "w" )
    json.dump( conf, oFile, indent=2, ensure_ascii=True )
    oFile.close()


if args.diff:
    if len( args.file ) != 2:
        print "ERROR, can only diff two files at a time, got: ", args.file
        sys.exit( -1 )
    __diff()
