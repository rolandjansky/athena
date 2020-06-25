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
	if (hname == "erec") :
		return event.DepositedEnergy
	elif (hname == "egen") :
		return event.TrackEnergy
	elif (hname == "cpu") :
		return event.CPU
	elif (hname == "eta") :
		return event.Eta
	elif (hname == "phi") :
		return event.Phi
	elif (hname == "pt") :
		return event.Pt
	elif (hname == "FC1_DeltaR") :
		return sqrt(event.FC1_DeltaX*event.FC1_DeltaX+event.FC1_DeltaY*event.FC1_DeltaY)
	if (hname == "erecegen") :
		return event.DepositedEnergy / event.TrackEnergy
	else :
		return "False"

def varname(hname) :
	if (hname == "erec") :
		return "E_{rec}"
	elif (hname == "egen") :
		return "E_{gen}"
	elif (hname == "cpu") :
		return "CPU Time"
	elif (hname == "eta") :
		return "#eta"
	elif (hname == "phi") :
		return "#phi"
	elif (hname == "pt") :
		return "P_{t}"
	elif (hname == "FC1_DeltaR") :
		return "FC1_DeltaR"
	elif (hname == "erecegen") :
		return "#frac{E_{rec}}{E_{gen}}"
	else :
		return ""

import sys
from os.path import isfile
from optparse import OptionParser
from math import sqrt

usage = "usage: %prog [options] root1 [root2 ...]"

parser = OptionParser(usage=usage, version="%prog v1.0 $Id: LArG4ValidationPlotter.py 505706 2012-06-15 11:48:02Z gsedov $")

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
	for var in ["erec","cpu"] :
		pe = PlotEntry()
		pe.vars_to_draw = [var]
		pe.axis_captions[var] = varname(var)
		pe.display_name = var
		parsedPlots.append(pe)
	pe = PlotEntry()
	pe.vars_to_draw = ["eta","erec"]
	pe.axis_captions["eta"] = varname("eta")
	pe.axis_captions["erec"] = varname("erec")
	pe.display_name = "Energy vs Eta"
	pe.profile = True
	parsedPlots.append(pe)
	pe = PlotEntry()
	pe.vars_to_draw = ["egen","erec"]
	pe.axis_captions["egen"] = varname("egen")
	pe.axis_captions["erec"] = varname("erec")
	pe.display_name = "Energy vs Truth Energy"
	pe.profile = True
	parsedPlots.append(pe)
	split_canv[0] = 2
	split_canv[1] = 2

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
	rootopt.tree = root.Get("COL/1")

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
