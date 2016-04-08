#!/usr/bin/env python

import sys
import getopt
import CalibdDbDefaults

cool_database_strings=[CalibDbDefaults.cool_database_string, CalibDbDefaults.cool_database_string]
cool_folders=[CalibDbDefaults.cool_folders[calibtype], CalibDbDefaults.cool_folders[calibtype]]
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

cool_t0s=[]

for i in xrange(2):
	if not cool_tags[i]:
		cool_tags[i] = GetFolderTag(cool_database_strings[i], cool_folders[i])
	print "Cool database: " + cool_database_strings[i] + " " + cool_folders[i] + " " + cool_tags[i] + " " + str(run_numbers[i])
	cool_t0s.append(ReadT0Cool(cool_database_strings[i], cool_folders[i], cool_tags[i], run_numbers[i]))


all_stations=set(cool_t0s[0].keys()).union(set(cool_t0s[1].keys()))


outfile=TFile(outfile_nm, "RECREATE")
tree=TTree("tree", "t0 differences")

station_nm=array('c', "BIL\0")
station_phi=array('i', [0])
station_eta=array('i', [0])
tree.Branch("station_nm", station_nm, "station_nm/C")
tree.Branch("station_phi", station_phi, "station_phi/I")
tree.Branch("station_eta", station_eta, "station_eta/I")

n_tubes=[]
t0=[]
t0_mean=[]
t0_min=[]
t0_max=[]
#validflag=[]
#adc=[]
for i in xrange(2):
	n_tubes.append(array('i', [0]))
	tree.Branch("n_tubes_" + str(i), n_tubes[-1], "n_tubes_" + str(i) + "/I")
	t0.append(array('f', 432*[0.0]))
	tree.Branch("t0_" + str(i), t0[-1], "t0_" + str(i) + "[n_tubes_" + str(i) + "]/F")
	t0_mean.append(array('f', [0.0]))
	tree.Branch("t0_mean_" + str(i), t0_mean[-1], "t0_mean_" + str(i) + "/F")
	t0_min.append(array('f', [0.0]))
	tree.Branch("t0_min_" + str(i), t0_min[-1], "t0_min_" + str(i) + "/F")
	t0_max.append(array('f', [0.0]))
	tree.Branch("t0_max_" + str(i), t0_max[-1], "t0_max_" + str(i) + "/F")
	
	
for ident in all_stations:
	for i in range(0,3):
		station_nm[i]=ident[0][i]
	station_phi[0]=ident[1]
	station_eta[0]=ident[2]
	for i in xrange(2):
		if not ident in cool_t0s[i]:
			n_tubes[i][0]=0
			t0_mean[i][0]=9e9
			t0_min[i][0]=9e9
			t0_max[i][0]=9e9
			continue
		t0_mean[i][0]=0.0
		t0_min[i][0]=9e9
		t0_max[i][0]=-9e9
		theT0s=cool_t0s[i][ident]
		n_tubes[i][0]=len(theT0s)
		for j in xrange(n_tubes[i][0]):
			t0_mean[i][0]+=theT0s[j][0]
			if t0_min[i][0]>theT0s[j][0]:
				t0_min[i][0]=theT0s[j][0]
			if t0_max[i][0]<theT0s[j][0]:
				t0_max[i][0]=theT0s[j][0]
			t0[i][j]=theT0s[j][0]
		t0_mean[i][0]/=float(n_tubes[i][0])
	tree.Fill()
outfile.Write()	

		
		
			

