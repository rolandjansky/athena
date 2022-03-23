#                                                                
#Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# 

import MMRawDataMonitoring.MMRawMonLabels as labels
from ROOT import TF1

def getMMLabel(x,y):
	labelx = getattr(labels, x)
	labely = getattr(labels, y)
	return labelx,labely

def getMMLabelY(y):
	labely = getattr(labels, y)
	return labely

def getMMLabelX(x):
	labelx = getattr(labels, x)
	return labelx

def get_MPV_charge(histo):
	flandau = TF1("flandau","landau",0,1200)
	histo.Fit("flandau","QRN")
	mpv = flandau.GetParameter(1)
	return mpv

def get_mean_and_sigma(histo):
	fgaus = TF1("fgaus","gaus",-10,10)
	histo.Fit("fgaus","QRN")
	mean = fgaus.GetParameter(1)
	sigma = fgaus.GetParameter(2)
	return mean,sigma
