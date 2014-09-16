# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import CoolConvUtilities.AtlCoolLib as AtlCoolLib
from PyCool import cool,coral
import zlib,StringIO
import sys
import re
from MuonCalibDbOperations.MuonCalibResolveTag import ResolveTag
from MuonCalibDbOperations.MuonCalibConvertTimeSlewing import *	
	
sys.argv=[sys.argv[0], '-b']

from ROOT import *
from MuonCalibIdentifier.MuonFixedIdUnpack import *

def UnpackData(data):
	if type(data)==str:
		return data
	if type(data)==coral.Blob:
		blob=StringIO.StringIO()
		for i in range(data.size()):
			blob.write(chr(data[i]))
		unpacked=zlib.decompress(blob.getvalue())
		return unpacked

def iov_keygen(iov):
	return long(iov[0])*10000000 + iov[1]

def DumpFolderSummary(db_string, folder, tag, run=None):
	
	try:
		db=AtlCoolLib.indirectOpen(db_string, oracle=True, readOnly=True, debug=True)
	except Exception,e:
		print 'Problem opening database',e
		sys.exit(-1)

#get folder and tag	
	cool_folder=db.getFolder(folder)
	cool_tag=ResolveTag(cool_folder, tag)
	
	counters={}
	if run:
		objs=cool_folder.browseObjects((run<<32), ((run+1)<<32), cool.ChannelSelection.all(), cool_tag)
	else:
		objs=cool_folder.browseObjects(0, (999999<<32), cool.ChannelSelection.all(), cool_tag)
	for obj in objs:
		file_col=obj.payload()['file']
		file_items=re.split('[A-Z,a-z]*', file_col)
		site="default"
		head_id=-1
		if len(file_items)==2:
			try:
				head_id = int(file_items[1])
				site = re.sub("[0-9]", "", file_col)
			except ValueError:
				site=file_col
		ts="NOTS"
		if TimeSlewingApplied(obj):
			ts="TS"		
		ident=(obj.since()>>32, obj.until()>>32, site, head_id, ts)
		if not ident in counters:
			counters[ident] = 0
		counters[ident] +=1
	for ident in sorted(counters.keys(), key=iov_keygen):
		print "[", ident[0], ",", ident[1], "[", ident[2], ident[3], ident[4], ":" , counters[ident]
			

def ReadRtCool(db_string, folder, tag, run_number):

	try:
		db=AtlCoolLib.indirectOpen(db_string, oracle=True, readOnly=True, debug=True)
	except Exception,e:
		print 'Problem opening database',e
		sys.exit(-1)

#get folder and tag	
	cool_folder=db.getFolder(folder)
	cool_tag=ResolveTag(cool_folder, tag)

#data to be filled
	graphs={}
	splines={}	
	iovs=set([])
	
	myiov=(run_number<<32)
	
	objs=cool_folder.browseObjects(myiov,myiov, cool.ChannelSelection.all(), cool_tag)
	for obj in objs:
		iov=(obj.since(), obj.until())
		iovs.add(iov)
		sp=UnpackData(obj.payload()['data']).split('\n')
		sp1=sp[0].split(',')
		chamber=int(sp1[0])
		n_points=int(sp1[1])	
		sp2=sp[1].split(',')
		graphs[chamber]=TGraphErrors(n_points)
		up=MuonFixedIdUnpack(chamber)
		if up.stationNameString()=='XXX':
			print "Invalid station name in ", obj.payload()['file']
			sys.exit(1)
		nm=up.stationNameString() + "_" + str(up.stationPhi()) + "_" + str(up.stationEta())
#		print nm
		ts_applied=TimeSlewingApplied(obj)
		for i in range(0, n_points):
			r=float(sp2[3*i])
			t=float(sp2[3*i+1])
			s=float(sp2[3*i+2])
			if not ts_applied:
				t_new=NoTs2Ts(r, t)
				t=t_new
			graphs[chamber].SetPoint(i, r, t)
			graphs[chamber].SetPointError(i, 0, s)
		splines[chamber]=TSpline3("sp_" + nm, graphs[chamber])
		if gDirectory.IsWritable():
			splines[chamber].Write("sp_" + nm)
			graphs[chamber].Write("gr_" + nm)
	
	return graphs, splines, iovs


def ReadT0Cool(db_string, folder, tag, run_number):
	
	try:
		db=AtlCoolLib.indirectOpen(db_string, oracle=True, readOnly=True, debug=True)
	except Exception,e:
		print 'Problem opening database',e
		sys.exit(-1)

#get folder and tag	
	cool_folder=db.getFolder(folder)
	cool_tag=ResolveTag(cool_folder, tag)
	myiov=(run_number<<32)
#data
	t0_values={}
	
	objs=cool_folder.browseObjects(myiov,myiov, cool.ChannelSelection.all(), cool_tag)
	
	for obj in objs:
		sp=UnpackData(obj.payload()['data']).split('\n')
		sp1=sp[0].split(',')
		ch_sp=sp1[0][2:].split("_")
		ch_id=(ch_sp[0], int(ch_sp[1]), int(ch_sp[2]))
		ntubes=int(sp1[-1])
		
		t0_items=sp[1].split(",")
		t0_values[ch_id] = []
		ts_applied=TimeSlewingApplied(obj)
		for i in range(0, ntubes):
			t0=float(t0_items[3*i+0])
			if not ts_applied:
				t0+= GasmonDriftTimeOffsetT0
			t0_values[ch_id].append((t0, int(t0_items[3*i+1]), float(t0_items[3*i+2])))
	
	return t0_values
		
	
def GetFolderTag(db_string, folder):
	dbSvc=cool.DatabaseSvcFactory.databaseService()
	try:
		db=dbSvc.openDatabase(db_string)
	except Exception,e:
		print 'Problem opening database',e
		sys.exit(-1)

#get folder and tag	
	cool_folder=db.getFolder(folder)
	for x in cool_folder.listTags():
		return x
	
