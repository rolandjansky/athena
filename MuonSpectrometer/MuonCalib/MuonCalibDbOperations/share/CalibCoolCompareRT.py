#!/usr/bin/env python

import sys
import getopt

cool_database_strings=['COOLOFL_MDT/COMP200', 'COOLOFL_MDT/COMP200']
cool_folders=['/MDT/RT', '/MDT/RT']
cool_tags=['COMCOND-BLKPST-004-07', 'COMCOND-BLKPST-004-07']
run_numbers=[None, None]

def printhelp(argv0):
	sys.stderr.write("Usage: " + argv0 + " [-h] [-s <cool database string>] [-f <cool folder>] [-t <cool tag>] <run number 1> [-s <cool database string>] [-f <cool folder>] [-t <cool tag>] <run number 2> <root file>\n")
	sys.exit(1)
	
theArgs=sys.argv[1:]

for i in xrange(2):
	optlist, args = getopt.getopt(theArgs, 'hs:f:t:')
	
	for opt, oa in optlist:
		if opt=='-s':
			if oa[-2:]=='db':
				cool_database_strings[i] = 'sqlite://;schema=' + oa + ';dbname=COMP200'
				cool_tags[i]=''
			else:
				cool_database_strings[i] = oa
			continue
		if opt=='-f':
			cool_folders[i]=oa
			continue
		if opt=='-t':
			cool_tags[i]=oa
			continue
		if opt=='-h':
			 printhelp(sys.argv[0])
			 continue
	if len(args)<2:
		sys.stderr.write("Error in commandline!\n")
		printhelp(sys.argv[0])
	try:
		run_numbers[i]=int(args[0])
	except ValueError:
		sys.stderr.write("Error in commandline!\n")
		printhelp(sys.argv[0])
	theArgs=args[1:]
	
if len(theArgs)!=1:
	sys.stderr.write("Error in commandline!\n")
	printhelp(sys.argv[0])
	
outfile_nm=theArgs[0]

print cool_database_strings
print cool_folders
print cool_tags
print run_numbers
print outfile_nm

sys.argv=[sys.argv[0], '-b']

from ROOT import *
from array import *

from MuonCalibDbOperations.ReadCool import *
from MuonCalibIdentifier.MuonFixedIdUnpack import *

cool_rts=[]

outfile=TFile(outfile_nm, "RECREATE")

for i in xrange(2):
	outfile.mkdir("graphs_"+str(i)).cd()
	if not cool_tags[i]:
		cool_tags[i] = GetFolderTag(cool_database_strings[i], cool_folders[i])
	print "Cool database: " + cool_database_strings[i] + " " + cool_folders[i] + " " + cool_tags[i] + " " + str(run_numbers[i])
	cool_rts.append(ReadRtCool(cool_database_strings[i], cool_folders[i], cool_tags[i], run_numbers[i])[1])


all_stations=set(cool_rts[0].keys()).union(set(cool_rts[1].keys()))

outfile.cd()
tree=TTree("tree", "t0 differences")

station_nm=array('c', "BIL\0")
station_phi=array('i', [0])
station_eta=array('i', [0])
tree.Branch("station_nm", station_nm, "station_nm/C")
tree.Branch("station_phi", station_phi, "station_phi/I")
tree.Branch("station_eta", station_eta, "station_eta/I")

n_tubes=[]
t=[]
v=[]
r=array('f', 13*[0.0])
for i in xrange(13):
	r[i]=float(i+1)
tree.Branch("r", r, "r[13]/F")
#validflag=[]
#adc=[]
for i in xrange(2):
	t.append(array('f', 13*[0.0]))
	v.append(array('f', 13*[0.0]))
	tree.Branch("t_" + str(i), t[-1], "t_" + str(i) + "[13]/F")
	tree.Branch("v_" + str(i), v[-1], "v_" + str(i) + "[13]/F")
	
for ident in all_stations:
	up=MuonFixedIdUnpack(ident)
	nm=up.stationNameString()
	for i in range(0,3):
		station_nm[i]=nm[i]
	station_phi[0]=up.stationPhi()
	station_eta[0]=up.stationEta()
	for i in xrange(2):
		if not ident in cool_rts[i]:
			for j in xrange(13):
				t[i][j]=-9e9
				v[i][j]=-9e9
			continue
		theRt=cool_rts[i][ident]
		for j in xrange(13):
			t[i][j]=theRt.Eval(float(j+1))
			v[i][j]=1/theRt.Derivative(float(j+1))
			
			
	tree.Fill()
outfile.Write()	

		
		
			

