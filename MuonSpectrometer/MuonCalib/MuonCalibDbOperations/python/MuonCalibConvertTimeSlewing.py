# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math

GasmonDriftTimeOffsetsVsR_total = [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ]

GasmonDriftTimeOffsetT0=GasmonDriftTimeOffsetsVsR_total[0]

GasmonDriftTimeOffsetsVsR=[]
for x in GasmonDriftTimeOffsetsVsR_total:
	GasmonDriftTimeOffsetsVsR.append(x - GasmonDriftTimeOffsetT0)

def GetRBin(r):
	if r<=0:
		return 0
	if r>=14.6:
		return len(GasmonDriftTimeOffsetsVsR) -1
	return int(math.floor(r/14.6 * len(GasmonDriftTimeOffsetsVsR)))
	

def Ts2NoTs(r, t):
	return t - GasmonDriftTimeOffsetsVsR[GetRBin(r)]

def NoTs2Ts(r, t):
	return t + GasmonDriftTimeOffsetsVsR[GetRBin(r)]

def TimeSlewingApplied(obj):
	return bool(obj.payload()['tech'] & 0x10)
	
	

	
