#!/usr/bin/env python

import getopt
import sys
import CalibDbDefaults

calibtype = 'rt'
cool_database_string = CalibDbDefaults.cool_database_string
cool_folder = CalibDbDefaults.cool_folders[calibtype]
cool_tag = CalibDbDefaults.cool_tags[calibtype]
dbname = CalibDbDefaults.cool_dbname

# we may want to modify to take reader password from CalibDbInfo for convenience

def printhelp(argv0):
	sys.stderr.write("Usage: " + argv0 + " [-h] [-s <cool database string>] [-f <cool folder>] [-t <cool tag>] <run number> <calib db>/<head id> <reader password> <root file>\n")
	sys.exit(1)
	
optlist, args = getopt.gnu_getopt(sys.argv, 'hs:f:t:')
if not len(args)==5:
	printhelp(args[0])
	
for opt, oa in optlist:
	if opt=='-s':
		if oa[-2:]=='db':
			cool_database_string = 'sqlite://;schema=' + oa + ';dbname=' + dbname
			cool_tag=''
		else:
			cool_database_string = oa
		continue
	if opt=='-f':
		cool_folder=oa
		continue
	if opt=='-t':
		cool_tag=oa
		continue
	
run_number=int(args[1])
asp=args[2].split("/")
db=asp[0]
head_id=int(asp[1])
reader_password=args[3]
root_filename=args[4]

sys.argv=[ sys.argv[0], '-b' ]
from ROOT import *
from array import *
from MuonCalibDbOperations.ReadCalibDb import *
from MuonCalibDbOperations.ReadCool import *

if not cool_tag:
	cool_tag = GetFolderTag(cool_database_string, cool_folder)
	
	
print "Cool database: " + cool_database_string + " " + cool_folder + " " + cool_tag + " " + str(run_number)
cool_t0s=ReadT0Cool(cool_database_string, cool_folder, cool_tag, run_number)

print "Calibration database: " + db + "/" + str(head_id)
calib_t0s=ReadT0CalibDb(db, reader_password, head_id)
print len(calib_t0s), "chambers in  calibration databse"

outfile=TFile(root_filename, "RECREATE")
tree=TTree("tree", "t0 differences")

station_nm=array('c', "BIL\0")
station_phi=array('i', [0])
station_eta=array('i', [0])
tree.Branch("station_nm", station_nm, "station_nm/C")
tree.Branch("station_phi", station_phi, "station_phi/I")
tree.Branch("station_eta", station_eta, "station_eta/I")
n_tubes=array('i', [0])
n_tubes_calib=array('i', [0])
n_tubes_cool=array('i', [0])
dt0=array('f', 432*[0.0])
t0_cool=array('f', 432*[0.0])
t0_calib=array('f', 432*[0.0])
d_validflag=array('i', 432*[0])
validflag_cool=array('i', 432*[0])
validflag_calib=array('i', 432*[0])
d_adc=array('f', 432*[0])
tree.Branch("n_tubes", n_tubes, "n_tubes/I")
tree.Branch("n_tubes_calib", n_tubes_calib, "n_tubes_calib/I")
tree.Branch("n_tubes_cool", n_tubes_cool, "n_tubes_cool/I")
tree.Branch("dt0", dt0, "dt0[n_tubes]/F")

tree.Branch("t0_cool", t0_cool, "t0_cool[n_tubes]/F")
tree.Branch("t0_calib", t0_calib, "t0_calib[n_tubes]/F")


tree.Branch("d_validflag", d_validflag, "d_validflag[n_tubes]/I")
tree.Branch("validflag_cool", validflag_cool, "validflag_cool[n_tubes]/I")
tree.Branch("validflag_calib", validflag_calib, "validflag_calib[n_tubes]/I")
tree.Branch("d_adc", d_adc, "d_adc[n_tubes]/F")

for chamber_id in cool_t0s:
	if not calib_t0s.has_key(chamber_id):
		n_tubes[0]=0
		continue
	for i in range(0,3):
		station_nm[i]=chamber_id[0][i]
	print station_nm
	station_phi[0]=chamber_id[1]
	station_eta[0]=chamber_id[2]
	cot0s=cool_t0s[chamber_id]
	cat0s=calib_t0s[chamber_id]
	n_tubes[0]=min(len(cot0s), len(cat0s))
	n_tubes_calib[0]=len(cat0s)
	n_tubes_cool[0]=len(cot0s)
	for i in range(0, n_tubes[0]):
		dt0[i]=cot0s[i][0] - cat0s[i][0]
		d_validflag[i]=cot0s[i][1] - cat0s[i][1]
		validflag_cool[i]=cot0s[i][1]
		validflag_calib[i]=cat0s[i][1]
		d_adc[i]=cot0s[i][2] - cat0s[i][2]
		t0_cool[i]=cot0s[i][0]
		t0_calib[i]=cat0s[i][0]
	tree.Fill()

outfile.Write()	
		
