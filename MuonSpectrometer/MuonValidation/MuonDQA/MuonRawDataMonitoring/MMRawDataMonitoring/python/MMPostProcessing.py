#
#	Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#

from ROOT import TH2F
from .MMMonUtils import getMMLabelX, get_MPV_charge, get_mean_and_sigma, getXYbins, poi, get_time

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
                                test_list.append(f'pcb_{eta}_{side}_phi{phi}_multiplet{mult+1}_gas_gap{gg+1}_vs_hitcut')
                list_of_all_histos.append(test_list)
        histos_sorted = []
        for ihisto in range(len(names)):
                ind = histos_unsorted.index(names[ihisto])
                histos_sorted.append(inputs[ind][1][0])
                
        h_eff_per_PCB = TH2F(f'Efficiency_{side}_{eta}_per_PCB', f'Efficiency {side} {eta} per PCB', bins,0,bins, 16, .5,16.5)
        thisLabelx=getMMLabelX("x_lab_occ_etaminus2")
        if(nPCB==5):
                thisLabelx=getMMLabelX("x_lab_occ_etaminus1")
        for xbin in range(len(thisLabelx)):
                h_eff_per_PCB.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
        h_eff_per_PCB.GetYaxis().SetTitle('Sector')
        
        for ihisto in range(len(histos_sorted)):
                iphi = histos_sorted[ihisto].GetName()[18:20]
                if '_' in iphi:
                        iphi = histos_sorted[ihisto].GetName()[18:19]
                xbin,ybin = getXYbins(nPCB, int(iphi), True, histos_sorted[ihisto].GetName(), list_of_all_histos)
                ybin+=1
                for ipcb in range(nPCB):
                        h_eff_per_PCB.SetBinContent(xbin, ybin, histos_sorted[ihisto].GetEfficiency(ipcb+1))
                        xbin+=1
        return [h_eff_per_PCB]

def residual_map(inputs):
        inputs = list(inputs)
        side = 'A'
        if('CSide' in inputs[0][1][0].GetName()):
                side = 'C'
        eta='eta2'
        if('stationEta1' in inputs[0][1][0].GetName()):
                eta='eta1'
        
        output1 = TH2F(f'h_residual_sigma_{eta}_E{side}',f'EndCap{side}: Sigma of Gaussian Fit {eta} - residuals',8, 0, 8, 16, .5, 16.5)
        output2 = TH2F(f'h_residual_mean_{eta}_E{side}',f'EndCap{side}: Mean of Gaussian Fit {eta} - residuals',8, 0, 8, 16, .5, 16.5)
        thisLabelx=getMMLabelX("x_lab_mpv")
        
        for xbin in range(len(thisLabelx)):
                output1.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
                output2.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
        output1.GetYaxis().SetTitle('Sector')
        output1.GetZaxis().SetTitle('#sigma [mm]')
        output2.GetYaxis().SetTitle('Sector')
        output2.GetZaxis().SetTitle('Mean [mm]')
        
        for ihisto in range(len(inputs)):
                h = inputs[ihisto][1][0]
                sector = inputs[ihisto][1][0].GetName()[19:21]
                ml = inputs[ihisto][1][0].GetName()[43:44]
                gap = inputs[ihisto][1][0].GetName()[52:53]
                if '_' in sector:
                        sector = inputs[ihisto][1][0].GetName()[19:20]
                        ml = inputs[ihisto][1][0].GetName()[42:43]
                        gap = inputs[ihisto][1][0].GetName()[51:52]
                ybin = int(sector)
                xbin = int(gap) + (int(ml) -1)*4
                
                mean,sigma = get_mean_and_sigma(h, -10, 10)
                output1.SetBinContent(xbin,ybin,sigma)
                output2.SetBinContent(xbin,ybin,mean)
                output1.GetZaxis().SetRangeUser(0, 10)
                output2.GetZaxis().SetRangeUser(-5, 5)
        
        return [output1,output2]

def charge_map_perLayer(inputs):
        inputs = list(inputs)
        
        side = 'A'
        if('CSide' in inputs[0][1][0].GetName()):
                side = 'C'
        eta='eta2'
        if('stEta1' in inputs[0][1][0].GetName()):
                eta='eta1'
                
        output = TH2F(f'h_landau_{eta}_E{side}',f'E{side}: MPV of Landau Fit to Cluster charge {eta}',8, 0, 8, 16, .5, 16.5)
        thisLabelx=getMMLabelX("x_lab_mpv")

        for xbin in range(len(thisLabelx)):
                output.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
        output.GetYaxis().SetTitle('Sector')
        output.GetZaxis().SetTitle('MPV')
        
        for ihisto in range(len(inputs)):
                h = inputs[ihisto][1][0]
                sector = inputs[ihisto][1][0].GetName()[18:20]
                ml = inputs[ihisto][1][0].GetName()[37:38]
                gap = inputs[ihisto][1][0].GetName()[42:43]
                if '_' in sector:
                        sector = inputs[ihisto][1][0].GetName()[18:19]
                        ml = inputs[ihisto][1][0].GetName()[36:37]
                        gap = inputs[ihisto][1][0].GetName()[41:42]
                ybin = int(sector)
                xbin = int(gap) + (int(ml) -1)*4
                
                if (h.GetEntries()==0):
                        continue
                else:
                        mpv = get_MPV_charge(h)
                        output.SetBinContent(xbin,ybin,mpv)
        return[output]

def map_per_PCB(inputs, histo_name, start_index, proctype):
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
                                test_list.append(f'{histo_name}_{side}_{eta}_phi{phi+1}_ml{mult+1}_gap{gg+1}')
                list_of_all_histos.append(test_list)

        histos_sorted = []
        for ihisto in range(len(names)):
                ind = histos_unsorted.index(names[ihisto])
                histos_sorted.append(inputs[ind][1][0])
        if proctype=='fermiDirac_tzero':
                h_poi_per_PCB = TH2F(f'{histo_name}_{side}_{eta}_tzero_per_PCB', poi(histo_name,side,eta)+' tzero' , bins, 0, bins, 16, .5, 16.5)
        elif proctype=='fermiDirac_deltat':
                h_poi_per_PCB = TH2F(f'{histo_name}_{side}_{eta}_tzero_per_PCB', poi(histo_name,side,eta)+' deltaT' , bins, 0, bins, 16, .5, 16.5)
        else:
                h_poi_per_PCB = TH2F(f'{histo_name}_{side}_{eta}_per_PCB', poi(histo_name,side,eta), bins, 0, bins, 16, .5, 16.5)
        thisLabelx=getMMLabelX("x_lab_occ_etaminus2")
        if(nPCB==5):
                thisLabelx=getMMLabelX("x_lab_occ_etaminus1")
        for xbin in range(len(thisLabelx)):
                h_poi_per_PCB.GetXaxis().SetBinLabel(xbin+1, thisLabelx[xbin])
        h_poi_per_PCB.GetYaxis().SetTitle('Sector')

        for ihisto in range(len(histos_sorted)):
                histo_name=histos_sorted[ihisto].GetName()
                iphi = histos_sorted[ihisto].GetName()[start_index:start_index+2]
                if '_' in iphi:
                        iphi = histos_sorted[ihisto].GetName()[start_index:start_index+1]
             
                xbin,ybin = getXYbins(nPCB, int(iphi), False, histos_sorted[ihisto].GetName(), list_of_all_histos)
                for ipcb in range(nPCB):
                        histo_tmp = histos_sorted[ihisto].ProjectionY(histos_sorted[ihisto].GetName()+"_py_"+str(ipcb+1),ipcb+1,ipcb+1)
                        if proctype=='mean':
                                if (histo_tmp.GetEntries()==0): 
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, -999)
                                else:
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, histo_tmp.GetMean())
                                h_poi_per_PCB.GetZaxis().SetRangeUser(0, 20)
                        if proctype=='gaus':
                                if (histo_tmp.GetEntries()<100):
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, -999)
                                else:
                                        gmean, gsigma = get_mean_and_sigma(histo_tmp, -100, 700)
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, gmean)
                                h_poi_per_PCB.GetZaxis().SetRangeUser(-100, 700)
                        if proctype=='landau':
                                if (histo_tmp.GetEntries()<100):
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, -999)
                                else:
                                        mpv = get_MPV_charge(histo_tmp)
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, mpv)
                                h_poi_per_PCB.GetZaxis().SetRangeUser(0, 100)
                        if proctype=='fermiDirac_tzero':
                                if (histo_tmp.GetEntries()<100):
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, -999)
                                else:
                                        t0, tmax = get_time(histo_tmp)
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, t0)
                                h_poi_per_PCB.GetZaxis().SetRangeUser(-100, 700)
                        if proctype=='fermiDirac_deltat':
                                if (histo_tmp.GetEntries()<100):
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, -999)
                                else:
                                        t0, tmax = get_time(histo_tmp)
                                        h_poi_per_PCB.SetBinContent(xbin, ybin, tmax-t0)
                                h_poi_per_PCB.GetZaxis().SetRangeUser(0, 300)
                        xbin+=1
        return[h_poi_per_PCB]

