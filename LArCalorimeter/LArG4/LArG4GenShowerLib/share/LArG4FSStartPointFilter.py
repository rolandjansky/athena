#!/usr/bin/env physh

import sys
from os.path import isfile
from optparse import OptionParser
from random import randint

usage = "usage: %prog [options] input1 [input2...]"

parser = OptionParser(usage=usage, version="%prog v0.0.1 $Id: LArG4FSStartPointFilter.py 711210 2015-11-27 15:56:00Z jchapman $")

parser.add_option("-p","--particle", dest="part", type="choice", action="append", choices=["11","22","2112"], help="particle to be filtered out (default - no filter)")
parser.add_option("-t","--truncate", dest="numevents", type=int, help="Truncate the number of events (default - all)")
parser.add_option("-o","--output", dest="outfile", help="Name of output file")

parser.set_defaults(part=[],outfile="genevents.ascii",numevents=0,draw=False,execute=False)

(options, args) = parser.parse_args()

del args[0]

if len(args) == 0 :
    print "ERROR: No input! Aborting"
    sys.exit(1)

stpoints = []
globallinever = ""
#reading input
for infilename in args :
    #opening file
    print "Opening file",infilename
    infile = open(infilename)
    #first line is empty
    infile.readline()
    linever = infile.readline().rstrip("\n")
    if (globallinever == "") :
        globallinever = linever
    linehead = infile.readline().rstrip("\n")
    #print linehead,linever
    #must be FS SP header OR normal HepMC GenEvent header
    if (( not linever.startswith("HepMC::Version")) or (linehead != "HepMC::IO_GenEvent-START_EVENT_LISTING") or (linever != globallinever)) :
        print >> sys.stderr, "ERROR: Wrong input:",infilename,"omitting"
        continue
    #starting the loop
    stpointsloc = []
    line = infile.readline()
    while ((line != "") and ( not line.startswith("HepMC::" ))) :
        line1 = infile.readline()
        line2 = infile.readline()
        line3 = infile.readline()
        #checking every event for being single particle event
        if (line[0] != "E") or (line1[0] != "U") or (line2[0] != "V") or (line3[0] != "P") :
            print "ERROR:",infilename,"is not a valid file!"
            sys.exit(1)
        stpoint = [line,line1,line2,line3]
        stpointsloc.append(stpoint)
        line = infile.readline()
    infile.close()
    stpoints += stpointsloc
    if ("tfile" in dir()) :
        del tinfo
        tfile.close()

if len(stpoints) == 0 :
    print "ERROR: no events found is not a valid file!"
    sys.exit(1)

#creating an output stream
if isfile(options.outfile) :
    print "WARNING: File",options.outfile,"already exists."
outdata = open(options.outfile,"w")
outdata.write("\n")
outdata.write(globallinever)
outdata.write("\nHepMC::IO_GenEvent-START_EVENT_LISTING\n")


stpsize = len(stpoints)
if (options.numevents > stpsize) :
    print "WARNING: requested number of events is bigger then provided in input files"
    options.numevents = 0

if (options.numevents == 0) :
    options.numevents = stpsize #all events

#starting the filter
i = 0
while i < options.numevents :
    if (stpsize == 0) :
        print "INFO: We've run out of starting point.\nIt's okay if you didn't specify -t option, but may mean that you do not have enough otherwise."
        break
    rand = randint(0,stpsize-1)
    stpoint = stpoints.pop(rand) #take a random event
    stpsize-=1
    parsed = stpoint[3].split() #last line - the particle. PDG code and 4-vector is there.
    if (options.part == []) or (parsed[2] in options.part) : #check PDG
        parsed = stpoint[0].split() #first line - event itself. The number of event is there
        parsed[1] = str(i) #Change the number of event...
        stpoint[0] = " ".join(parsed) #...and construct the line back
        if (options.draw) :
            parsed = stpoint[2].split() #third line - the vertex.
            x = float(parsed[3])
            y = float(parsed[4])
            z = float(parsed[5])
            r = sqrt(x*x + y*y)
            hist.Fill(z,r)
        if len(options.outfile) > 0 :
            outdata.write(stpoint[0].rstrip("\n")+"\n")
            outdata.write(stpoint[1].rstrip("\n")+"\n")
            outdata.write(stpoint[2].rstrip("\n")+"\n")
            outdata.write(stpoint[3].rstrip("\n")+"\n")
        i += 1

outdata.write("HepMC::IO_GenEvent-END_EVENT_LISTING")
outdata.close()
print "INFO: Written", i, "starting points"

## Algorithm sequence
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## AthenaCommon flags
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
athenaCommonFlags.EvtMax = i
athenaCommonFlags.PoolHitsOutput.set_Off()

from AthenaCommon.AppMgr import theApp
theApp.EvtMax = i

import AthenaCommon.AtlasUnixGeneratorJob

from ReadEventFromFile.ReadEventFromFileConf import ReadHepMcFromAscii
read = ReadHepMcFromAscii()
read.AsciiFile = options.outfile
topSequence += read


from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = ["EventInfo#*", "McEventCollection#*"]

outfile = options.outfile
if outfile.endswith(".ascii") :
    outfile = outfile[:-6]
outStream.OutputFile = outfile+".pool.root"
