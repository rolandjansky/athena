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
parser.add_argument('--toJSON', help='Convert to JSON file')
parser.add_argument('--toPKL', help='Convert to python pickle format file')
parser.add_argument('file', nargs=1, help='File to work with')
args = parser.parse_args()


conf = None
if args.file[0].endswith( ".pkl" ):
    input_file = file( args.file[0] )
    conf = []
    while True:
        try:
            conf.append( pickle.load( input_file ) )
        except EOFError:
            break
    print "Red", len(conf), "items"

if args.file[0].endswith( ".json" ):

    def __keepPlainStrings(element):
        if isinstance(element, unicode):
            return str(element)
        if isinstance(element, list):
            return [ __keepPlainStrings(x) for x in element ]
        if isinstance(element, dict):
            return dict( [ (__keepPlainStrings(key), __keepPlainStrings(value)) for key,value in element.iteritems() ] )
        return element

    conf = json.load( file( args.file[0] ), object_hook=__keepPlainStrings )
                
if args.pr:
    for n,c in enumerate(conf):
        print "Item", n, "*"*80
        pprint.pprint(dict(c))

if args.toJSON:
    oFile = open( args.toJSON, "w" )
    json.dump( conf, oFile, indent=2, ensure_ascii=True )
    oFile.close()

if args.toPKL:    
    oFile = open( args.toPKL, "w" )
    pickle.dump( conf, oFile )
    oFile.close()

