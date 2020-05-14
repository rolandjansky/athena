#!/usr/bin/env python
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# python script for drawing LArG4Validation ntuples
#

__version__ = '$Revision: 505706 $'
__author__  = 'Radist Morse radist.morse@gmail.com'
__doc__     = 'Script for drawing LArG4Validation N tuples'

#list of accessible variables
def eventext(event,hname) :
	if   (hname == "hadrleak") :
		return event.ethad1/event.et
	elif (hname == "e237oe277") :
		if (event.e277 > 0) :
			return event.e237/event.e277
		else :
			return 0
	elif (hname == "e2tsts1") :
		return event.e2tsts1/(1000. + 0.009 * event.et)
	elif (hname == "e2tsts1demins") :
		return event.e2tsts1 - event.emins1
	elif (hname == "erec") :
		return event.energy
	elif (hname == "erecegen") :
		return event.energy / event.truth_energy
	elif (hname == "eta") :
		return event.eta
	elif (hname == "pt") :
		return event.pt
	elif (hname == "egen") :
		return event.truth_energy
	else :
		return "False"

def varname(hname) :
	if   (hname == "hadrleak") :
		return "Hadron Leak"
	elif (hname == "e237oe277") :
		return "#frac{E_{237}}{E_{277}}"
	elif (hname == "e2tsts1") :
		return "E_{2}tsts_{1}"
	elif (hname == "e2tsts1demins") :
		return "e2tsts1demins"
	elif (hname == "erec") :
		return "E_{rec}"
	elif (hname == "erecegen") :
		return "#frac{E_{rec}}{E_{gen}}"
	elif (hname == "eta") :
		return "#eta"
	elif (hname == "pt") :
		return "P_{t}"
	elif (hname == "egen") :
		return "E_{gen}"
	else :
		return ""

import sys
from os.path import isfile
from optparse import OptionParser

usage = "usage: %prog [options] root1 [root2 ...]"

parser = OptionParser(usage=usage, version="%prog v1.0 $Id: LArG4AODNtuplePlotter.py 505706 2012-06-15 11:48:02Z gsedov $")

parser.add_option("-s","--split",dest="split_canvas", help="Split canvas, format: 'HORIZ:VERT' (default = %default)")
parser.add_option("-p","--plots",dest="plots", help="input file with histogram parameters")
parser.add_option("-f","--files",dest="files", help="input file with files parameters")
parser.add_option("-o","--output",dest="outputfile", help="output ps file")
parser.add_option("-d","--divide",dest="divide",action="store_true", help="Calculate ratio between root files (divide all roots to first")
parser.add_option("-m","--mean",dest="mean",action="store_true",help="Print mean value in the legend")
parser.add_option("-z","--zero",dest="zero",action="store_true",help="Set minimums to zero")

parser.set_defaults(split_canvas="1:1",plots="",files="",outputfile="DISPLAY",divide=False, zero=False)

(options, args) = parser.parse_args()

split_canv = options.split_canvas.split(":")
split_canv = map(int,split_canv)

from LArG4Validation.LArG4PlottingScript import parseRoots, defaultRoots, \
	parsePlots, fillPlots, dividePlots, savePlots, drawPlots, PlotEntry, \
	createPlots

if (len(split_canv) != 2) :
	printfunc ("ERROR: wrong split parameter")
	sys.exit(1)

if (options.divide and len(args) < 2) :
	printfunc ("ERROR: must be at least two input files for ratio calculation")
	sys.exit(1)

if isfile(options.files) :
	printfunc ("Parsing file with root parameters:",options.files)
	parsedRoots = parseRoots(options.files)
else :
	printfunc ("No root parameters provided, using default")
	parsedRoots = defaultRoots(args)

if isfile(options.plots) :
	printfunc ("Parsing file with plots parameters:",options.plots)
	parsedPlots = parsePlots(options.plots,varname)
else :
	printfunc ("No plots parameters provided, using default")
	parsedPlots = []
	parsedRestricts = []
	listvars = ["hadrleak","e237oe277","weta2","f1","e2tsts1","e2tsts1demins","wtots1","fracs1","weta1"]
	capvars = {}
	for var in listvars :
		capvars[var] = varname(var)
	listmax = [0.02,1.0,0.015,0.7,3.0,1500.0,4.0,0.6,0.8]
	listmin = [-0.007,0.84,0.005,0.0,0.0,0.0,1.0,0.0,0.4]
	listlogy = [0,0,0,0,1,1,0,0,0]
	for var,mmax,mmin,logy in zip(listvars,listmax,listmin,listlogy) :
		pe = PlotEntry()
		pe.vars_to_draw.append(var)
		pe.axis_captions[var] = capvars[var]
		pe.display_name = var
		pe.givenmin = mmin
		pe.givenmax = mmax
		pe.logy = logy
		parsedPlots.append(pe)
	split_canv[0] = 3
	split_canv[1] = 3

#number of pads
maxperlist = split_canv[0]*split_canv[1]

#can't make NewPage for display
numplots = len(parsedPlots)

if (options.outputfile == "DISPLAY") and (numplots > maxperlist) :
	printfunc ("ERROR: too many hists to print to display")
	sys.exit(1)

from ROOT import TFile

#opening root files
for rootopt in parsedRoots :
	if not isfile(rootopt.filename) :
		printfunc ("ERROR: unexistent file:",rootopt.filename)
		sys.exit(1)
	root = TFile(rootopt.filename,"read")
	if root.IsOpen() == 0 :
		printfunc ("ERROR: can't open the file:",rootopt.filename)
		sys.exit(1)
	rootopt.rootfile = root
	rootopt.tree = root.Get("AOD")

printfunc ("Creating plots...")
plots = createPlots(parsedPlots,parsedRoots)

printfunc ("Filling plots...")
fillPlots(plots,parsedPlots,parsedRoots,eventext)


if (options.divide) :
	printfunc ("Calculating ratio")
	rootopt1 = parsedRoots.pop(0)
	dividePlots(plots,rootopt1)
	for rootopt in parsedRoots :
		rootopt.legendname += " / " + rootopt1.legendname

printfunc ("Done!")

if options.outputfile.endswith(".root") :
	savePlots(plots,options.outputfile)
else :
	canv,leg = drawPlots(plots,parsedPlots,parsedRoots,options.outputfile,options.zero,options.mean,split_canv[0],split_canv[1])

if (options.outputfile == "DISPLAY") :
	from time import sleep
	while not not(canv) :
		sleep(1)
