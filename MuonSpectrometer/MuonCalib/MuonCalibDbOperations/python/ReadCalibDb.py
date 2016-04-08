# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import cx_Oracle
from MuonCalibIdentifier.MuonFixedIdUnpack import *
import CalibDbInfo
import sys

sys.argv=[sys.argv[0], '-b']

from ROOT import *


def get_data_schema(cursor, db, head_id):
	meta_schema = {'MP': 'ATLAS_MUONCALIBR_MPI', 'UM' : 'ATLAS_MUONCALIBR_UM', 'RM' : 'ATLAS_MUONCALIBR_RM'}
	cursor.execute("select data_schema from " + meta_schema[db] + ".MDT_HEAD where head_id=:hid", {'hid': head_id})
	return cursor.fetchone()[0]
	
def build_rt_relation(r,t, rt_id):
	gr=TGraph(len(r))
	gr.SetName("gr_" + str(rt_id))
	for i in range(0, len(r)):
		gr.SetPoint(i, r[i], t[i])
	sp=TSpline3("sp_" + str(rt_id), gr)
	up = MuonFixedIdUnpack(rt_id)
	id_str=up.stationNameString() + "_" + str(up.stationPhi()) + "_" + str(up.stationEta())
	gr.Write("gr_" + id_str)
	sp.Write("sp_" + id_str)
	return gr, sp
	

	
def ReadRtCalibDb(db, reader_passwd, head_id):
	ra = CalibDbInfo.dbr[CalibDbInfo.calibdb]
	tns = CalibDbInfo.tns[CalibDbInfo.calibdb]
	if reader_passwd == None:
		reader_passwd = CalibDbInfo.dbr_password[CalibDbInfo.calibdb]
	connection=cx_Oracle.connect(ra, reader_passwd, tns)
	cursor=connection.cursor()
	data_schema=get_data_schema(cursor, db, head_id)
	
	query="select rt.validflag, rt.region_id,  mp.r, mp.t from " + data_schema + ".mdt_rt rt, " + data_schema + ".mdt_rt_map mp  where mp.head_id=:hid and rt.head_id=:hid and rt.region_id= mp.region_id and mp.calibflag= rt.calibflag  and mp.point_nr<rt.bins and rt.validflag>2 order by rt.region_id, mp.point_nr"
	cursor.arraysize = 1000*100
	
	cursor.execute(query, {'hid': head_id})
	rt_gr={}
	rt_sp={}
	r=[]
	t=[]
	prev_id=None
	for validflag_in, region_id, r_p, t_p in cursor.fetchall():
		this_id=region_id
		if this_id!=prev_id:
			if len(r):
				rt_gr[prev_id], rt_sp[prev_id] = build_rt_relation(r,t, prev_id)
			r=[]
			t=[]
			prev_id = this_id	
		r.append(r_p)
		t.append(t_p)
	if len(r)>0:
		rt_gr[prev_id], rt_sp[prev_id] = build_rt_relation(r,t, prev_id)
	return rt_gr, rt_sp
	

def build_t0_vec(values):
	ret=[]
	keys=values.keys()
	keys.sort()
	for key in keys:
		ret.append(values[key])
	return ret

def ReadT0CalibDb(db, reader_passwd, head_id):
	ra = CalibDbInfo.dbr[CalibDbInfo.calibdb]
	tns = CalibDbInfo.tns[CalibDbInfo.calibdb]
	connection=cx_Oracle.connect(ra, reader_passwd, tns)
	cursor=connection.cursor()
	data_schema=get_data_schema(cursor, db, head_id)
	
	query="select CHAMBER, TUBE_ID, TO_NUMBER(P4), VALIDFLAG, TO_NUMBER(ADC_0) from " + data_schema + ".MDT_TUBE where head_id=:hid and validflag>2 order by chamber"
	cursor.arraysize = 1000*400	
	cursor.execute(query, {'hid': head_id})
	t0s={}
	current_values={}
	prev_chamber=None
	filled_chambers=set([])
	for chamber, tube, t0, validflag, adc in cursor.fetchall():
		if prev_chamber != chamber:
			if prev_chamber:
				up=MuonFixedIdUnpack(prev_chamber)
				t0s[(up.stationNameString(), up.stationPhi(), up.stationEta())] = build_t0_vec(current_values)
				filled_chambers.add(prev_chamber)
			prev_chamber = chamber
			current_values={}
		up=MuonFixedIdUnpack(tube)
		tid=288 * (up.mdtMultilayer() - 1) + 72 * (up.mdtTubeLayer() - 1) + (up.mdtTube() - 1 )
		current_values[tid] = (t0, validflag, adc)
	up=MuonFixedIdUnpack(prev_chamber)
	t0s[(up.stationNameString(), up.stationPhi(), up.stationEta())] = build_t0_vec(current_values)
	return t0s
	
	
	
	
