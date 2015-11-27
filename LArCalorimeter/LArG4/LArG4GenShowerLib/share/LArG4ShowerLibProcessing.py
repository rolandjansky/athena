#!/usr/bin/env python

#
# $Id: LArG4ShowerLibProcessing.py 711210 2015-11-27 15:56:00Z jchapman $
#
# python script for merging libs
#

__version__ = '$Revision: 711210 $'
__author__  = 'Radist Morse radist.morse@gmail.com'
__doc__     = 'Script for merging FS libs'

import sys
from optparse import OptionParser
from LArG4GenShowerLib.LArG4ShowerLibFunctions import *

usage = "usage: %prog [options] lib1 [lib2 ...]"

parser = OptionParser(usage=usage, version="%prog v0.0.1 $Id: LArG4ShowerLibProcessing.py 711210 2015-11-27 15:56:00Z jchapman $")

#parser.add_option("-o","--output", dest="outfile", help="Name of output file (default: %default)")
parser.add_option("-i","--info",dest="info", action="store_true", help="Print info about the lib(s)")
parser.add_option("-a","--add",dest="add", action="store_true", help="Add input libs together")
parser.add_option("-o","--output",dest="output", help="Name of output file")
parser.add_option("-s","--scale",dest="scale", type = float, help="Scale library by energy")
parser.add_option("-d","--moveDist",dest="moveD", action="append", help="move dist bin")
parser.add_option("-e","--moveEta",dest="moveE", action="append", help="move eta bin")
parser.add_option("--removeDist",dest="removeD", action="append", help="remove dist bin")
parser.add_option("--removeEta",dest="removeE", action="append", help="remove eta bin")
parser.add_option("-t","--truncate",dest="truncate", type = int, help="truncate lib")
parser.add_option("-p","--paint",dest="draw", action="store_true", help="paint hits distribution histogram")

parser.set_defaults(info = False, add = False, output = "", scale = 0.0, moveD = [], moveE = [], removeD = [], removeE = [], draw = False, truncate = 0)

(options, args) = parser.parse_args()

if len(args) == 0:
    print "ERROR: No input specified"
    sys.exit(1)

libs = []

for filename in args:
    print "INFO: Reading file",filename
    tmplib = EtaEnergyShowerLib()
    if tmplib.readFromFile(filename) :
        libs.append(tmplib)
    tmplib = TestShowerLib()
    if tmplib.readFromFile(filename) :
        libs.append(tmplib)
    tmplib = FCALDistShowerLib()
    if tmplib.readFromFile(filename) :
        libs.append(tmplib)
    tmplib = FCALDistEtaShowerLib()
    if tmplib.readFromFile(filename) :
        libs.append(tmplib)

if (options.truncate > 0) :
    for lib in libs :
        lib.truncate(options.truncate)

if (options.info) :
    for arg,lib in zip(args,libs) :
        print "LIB",arg
        lib.printInfo()

if (options.add and len(libs) > 1) :
    print "INFO: Adding libs"
    outlib = libs[0].__class__()
    if not outlib.fromLibs(libs) :
        print "ABORTING"
        sys.exit(1)
    del libs
    libs = []
    libs.append(outlib)

if (options.scale > 0) :
    print "Scaling to",options.scale
    libs[0].scaleEnergy(options.scale)

if (len(options.moveD) > 0) :
    for move in options.moveD :
        print "Moving bin:",move
        movl = map(float,move.split(":"))
        if not libs[0].moveDist(*movl) :
            print "WARNING: no",movl[0],"in lib"

if (len(options.moveE) > 0) :
    for move in options.moveE :
        print "Moving bin:",move
        movl = map(float,move.split(":"))
        if not libs[0].moveEta(*movl) :
            print "WARNING: no",movl[0],"in lib"

if (len(options.removeD) > 0) :
    for move in options.removeD :
        print "Removing bin:",move
        movl = float(move)
        if not libs[0].removeDist(movl) :
            print "WARNING: no",movl,"in lib"

if (len(options.removeE) > 0) :
    for move in options.removeE :
        print "Removing bin:",move
        movl = float(move)
        if not libs[0].removeEta(movl) :
            print "WARNING: no",movl,"in lib"

if (len(options.output) > 0) :
    print "INFO: Saving file",options.output
    libs[0].writeToFile(options.output)

if (options.draw) :
    print "INFO: Drawing lib",options.output
    from ROOT import TCanvas
    from ROOT import gROOT, gStyle,gPad
    gROOT.Reset()
    gROOT.SetStyle("Plain")
    gStyle.SetOptStat(0)
    gStyle.SetPalette(1)
    gStyle.SetNumberContours(50)

    c1 = TCanvas( 'c1', "Library Viewer")
    #gStyle.SetOptLogx(1)
    gPad.SetLogx(1)

    hits,containmentZ,containmentR = libs[0].drawHits()

    hits.Draw()
    containmentZ.SetMarkerColor(2)
    containmentZ.Draw("SAME")
    containmentR.SetMarkerColor(4)
    containmentR.Draw("SAME")
    c1.Update()
    from time import sleep
    while not not(c1) :
        sleep(1)
