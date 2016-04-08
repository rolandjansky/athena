#!/usr/bin/env python

import getopt
import sys
import CalibDbDefaults

calibtype = 'rt'
cool_database_string = CalibDbDefaults.cool_database_string
cool_folder = CalibDbDefaults.cool_folders[calibtype]
cool_tag = CalibDbDefaults.cool_tags[calibtype]
dbname = CalibDbDefaults.cool_dbname

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
from MuonCalibIdentifier.MuonFixedIdUnpack import *


outfile=TFile(root_filename, "RECREATE")

if not cool_tag:
	cool_tag = GetFolderTag(cool_database_string, cool_folder)
	
outfile.mkdir("cool").cd()
print "Cool database: " + cool_database_string + " " + cool_folder + " " + cool_tag + " " + str(run_number)
cool_gr, cool_sp, dummy=ReadRtCool(cool_database_string, cool_folder, cool_tag, run_number)

outfile.mkdir("calibdb").cd()
print "Calibration database: " + db + "/" + str(head_id)
calib_gr, calib_sp=ReadRtCalibDb(db, reader_password, head_id)

outfile.cd()
tree=TTree("tree", "rt differences")
region_id=array('i', [0])
station_nm=array('c', "BIL\0")
station_phi=array('i', [0])
station_eta=array('i', [0])
tree.Branch("region_id", region_id, "region_id/I")
tree.Branch("station_nm", station_nm, "station_nm/C")
tree.Branch("station_phi", station_phi, "station_phi/I")
tree.Branch("station_eta", station_eta, "station_eta/I")
n_points=array('i', [0])
n_points_calib=array('i', [0])
n_points_cool=array('i', [0])
dt13=array('f', [0.0])
dr13=array('f', [0.0])
dr=array('f', 100*[0.0])
dt=array('f', 100*[0.0])
tree.Branch("n_points", n_points, "n_points/I")
tree.Branch("n_points_calib", n_points_calib, "n_points_calib/I")
tree.Branch("n_points_cool", n_points_cool, "n_points_cool/I")
tree.Branch("dt13",  dt13, "dt13/F")
tree.Branch("dr13",  dr13, "dr13/F")

for chamber_id in cool_gr:
	if not calib_gr.has_key(chamber_id):
		continue
	region_id[0]=chamber_id
	up=MuonFixedIdUnpack(chamber_id)
	st_str=up.stationNameString()
	for i in range(0,3):
		station_nm[i]=st_str[i]
	station_phi[0]=up.stationPhi()
	station_eta[0]=up.stationEta()
	co_gr=cool_gr[chamber_id]
	co_sp=cool_sp[chamber_id]
	ca_gr=calib_gr[chamber_id]
	ca_sp=calib_sp[chamber_id]
	n_points[0] = min(co_gr.GetN(), ca_gr.GetN(), 100)
	n_points_calib[0] = ca_gr.GetN()
	n_points_cool[0] = co_gr.GetN()
	for i in range(i, n_points[0]):
		dr[i]=ca_gr.GetX()[i]
		dt[i]=ca_gr.GetY()[i]
	dt13[0] = ca_sp.Eval(13.0) - co_sp.Eval(13.0)
	dr13[0] = dt13[0] / ca_sp.Derivative(13.0)
	tree.Fill()

outfile.Write()	
		
