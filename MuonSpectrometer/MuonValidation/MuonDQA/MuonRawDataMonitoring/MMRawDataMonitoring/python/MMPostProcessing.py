#
#	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH2F
from .MMMonUtils import getMMLabelX

def make_eff_histo_per_PCB(inputs):
	inputs = list(inputs)
	names = []
	histos_unsorted = []
	for i in range(len(inputs)):
		h=inputs[i][1][0].Clone()
		histos_unsorted.append(h.GetName())
		names.append(h.GetName())
	names.sort()

	nPCB = 3 # for SM2 and LM2
	eta='eta2'
	bins=24
	if('eta1' in names[0]):
		nPCB = 5 # for SM1 and LM1
		eta='eta1'
		bins = 40

	side='ASide'
	if('CSide' in names[0]):
		side='CSide'
	list_of_all_histos=[]
	for phi in range(16):
		test_list = []
		for mult in range(2):
			for gg in range(4):
				test_list.append('pcb_'+eta+'_'+side+'_phi'+str(phi)+'_multiplet'+str(mult+1)+'_gas_gap'+str(gg+1)+'_vs_hitcut')
		list_of_all_histos.append(test_list)

	histos_sorted = []
	for ihisto in range(len(names)):
		ind = histos_unsorted.index(names[ihisto])
		histos_sorted.append(inputs[ind][1][0])

	h_eff_per_PCB = TH2F("Efficiency_"+side+"_"+eta+"_per_PCB", "Efficiency "+side+" "+eta+" per PCB", bins,0,bins, 16,1,17)
	thisLabelx=getMMLabelX("x_lab_occ_etaminus2")
	if(nPCB==5):
		thisLabelx=getMMLabelX("x_lab_occ_etaminus1")
	for xbin in range(len(thisLabelx)):
		h_eff_per_PCB.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
	h_eff_per_PCB.GetYaxis().SetTitle('Sector')
	iphi = 0 # phi0 reflects to the first small sector (i.e in ATLAS convention is sector with phi=1)
	ybin = 1
	for ihisto in range(len(histos_sorted)):
		if 'phi'+str(iphi)+'_' not in histos_sorted[ihisto].GetName():
			ybin+=1
			iphi+=1
			if 'phi'+str(10)+'_' in histos_sorted[ihisto].GetName():
				iphi=10
				ybin=11
			if 'phi'+str(1)+'_' in histos_sorted[ihisto].GetName():
				iphi=1
				ybin=2
		ind = list_of_all_histos[iphi].index(histos_sorted[ihisto].GetName())
		xbin = (ind+1) +(nPCB-1)*ind
		for ipcb in range(nPCB):
			h_eff_per_PCB.SetBinContent(xbin,ybin, histos_sorted[ihisto].GetEfficiency(ipcb+1))
			xbin+=1

	return [h_eff_per_PCB]

def test(inputs):
	""" HitsPerEventInXXPerChamber_[onSegm]_ADCCut """
	print("hello!")
	print(inputs)
	print(len(inputs))
	return[]
