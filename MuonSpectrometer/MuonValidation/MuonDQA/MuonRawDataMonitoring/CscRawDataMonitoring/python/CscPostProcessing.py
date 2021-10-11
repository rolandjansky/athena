#
#	Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH1F, TF1
from .CscMonUtils import getCSCLabelx, getCSCEfficiency


def CSCLandauFit(inputs):
	h_etaClusQSumHist = inputs[0][1][0].Clone()
	side = 'A'
	nbins = 90
	minbin = 0
	maxbin = 18
	if '_EA' not in h_etaClusQSumHist.GetName():
		side = 'C'
		nbins = 85
		minbin = -17
		maxbin = 0
	h_lfit = TH1F("h1csc_clus_qsum_signal_E"+side+"_lfitmean","EndCap"+side+": MPV of Landau Fit to Cluster charge (Qsum)", nbins, minbin, maxbin)
	xlabels = getCSCLabelx("labels_occupancy_signal_E"+side)
	for ibin in range(len(xlabels)):
		h_lfit.GetXaxis().SetBinLabel(ibin+1, xlabels[ibin])
	h_lfit.GetYaxis().SetTitle("counts / layer")

	for j in range(5, 86):
		if( j % 5 != 0):
			mpv = 0
			mpv_err = 0
			h_proj = h_etaClusQSumHist.ProjectionX("_px", j, j)
			if h_proj:
				fit_status = 0
				flmin = 50
				flmax = 700
				entries = h_proj.GetEntries()
				if(entries > 10):
					flandau = TF1("fland", "landau", flmin, flmax)
					fit_status = h_proj.Fit(flandau, "LQRN", "goff")
					if(fit_status == 0):
						mpv = flandau.GetParameter(1)
						mpv_err = flandau.GetParError(1)
			h_lfit.SetBinContent(j, mpv)
			h_lfit.SetBinError(j, mpv_err)

	return[h_lfit]

def CscChamberEfficiency(inputs):
	h_NumNClusSegm = inputs[0][1][0].Clone()
	side = 'A'
	nbins = 90
	minbin = 0
	maxbin = 18
	if '_EA' not in h_NumNClusSegm.GetName():
		side = 'C'
		nbins = 85
		minbin = -17
		maxbin = 0
	h_eff = TH1F("Muon_Segm_Efficiency_E"+side,"EndCap"+side+": #eta-segment efficiency per sector", nbins, minbin, maxbin)
	xlabels = getCSCLabelx("labels_occupancy_signal_E"+side)

	for ibin in range(len(xlabels)):
		h_eff.GetXaxis().SetBinLabel(ibin+1, xlabels[ibin])
	h_eff.GetYaxis().SetTitle("efficiency")

	for layer in range(1,5):
		if(side == 'A'):
			for ichamber in range(1,17): #to-do: check how the histograms are filled. https://gitlab.cern.ch/atlas/athena/-/blob/master/DataQuality/DataQualityUtils/src/MonitoringFile_CSCChamberEfficiency.cxx
				num = h_NumNClusSegm.GetBinContent(46,1+5*ichamber)
				num_err = h_NumNClusSegm.GetBinError(46,1+5*ichamber)
				den = h_NumNClusSegm.GetBinContent(10*layer-4,1+5*ichamber)
				den_err = h_NumNClusSegm.GetBinError(10*layer-4,1+5*ichamber)
				eff, err = getCSCEfficiency(num, num_err, den, den_err)
				h_eff.SetBinContent(5*ichamber+1+(layer-1),eff)
				h_eff.SetBinError(5*ichamber+1+(layer-1),err) 

		elif(side == 'C'):
			for ichamber in range(1,17):
				num = h_NumNClusSegm.GetBinContent(46, 86-5*ichamber)
				num_err = h_NumNClusSegm.GetBinError(46, 86-5*ichamber)
				den = h_NumNClusSegm.GetBinContent(10*layer-4, 86-5*ichamber)
				den_err = h_NumNClusSegm.GetBinError(10*layer-4, 86-5*ichamber)
				eff, err = getCSCEfficiency(num, num_err, den, den_err)
				h_eff.SetBinContent(90-layer-5*ichamber,eff)
				h_eff.SetBinError(90-layer-5*ichamber,err)	

	return [h_eff]
