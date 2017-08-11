#!/bin/env python
# Author: Michael Miller <miller@uchicago.edu>  

import os, struct, sys
import ROOT
from aux_functions import *


def SetupDraw():

    ROOT.gROOT.SetStyle("Plain")
    ROOT.gStyle.SetCanvasBorderMode(0)
    ROOT.gStyle.SetPadBorderMode(0)
    #ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetLabelFont(42,"XYZ")
    ROOT.gStyle.SetTextFont(42)
    ROOT.gStyle.SetOptStat(111110)
    ROOT.gStyle.SetPalette(1)
    ROOT.gStyle.SetTitleFont(42,"XYZ")
    ROOT.gStyle.SetTitleBorderSize(0)
    ROOT.gStyle.SetPadColor(0)
    ROOT.gStyle.SetCanvasColor(0)
    ROOT.gStyle.SetOptFit()
    
    c1 = ROOT.TCanvas()
    c1.SetFrameBorderMode(0)
    c1.SetBorderSize(0)
    c1.SetBorderMode(0)
    c1.SetFillColor(0)
	
    return c1

																	    
def doModules(runnumber, datadir):
 
    #calculate average channel times   

    ROOT.gROOT.SetBatch()
    
    values = [[[checkPMTValid(part, channel) for channel in range(48)] for module in range(64)] for part in range(4)]
    
    partnames = ['LBA', 'LBC', 'EBA', 'EBC']
    partmap = {'LBA': 'A', 'LBC': 'C', 'EBA': 'D', 'EBC': 'E'}
    
    
    for part in partnames:

        for mod in range(1,65):

            print 'Checking', part, mod 
            partInd = partnames.index(part)    
            modInd = mod - 1

	    filename = '%stiletb_%d_MonoCis.%s%02d.0.aan.root' % (datadir, runnumber, part, mod)
            print filename 
	    if os.path.exists(filename):
                print 'Using', filename 
		file = ROOT.TFile.Open(filename)
		if file and not file.IsZombie():
                    tree = file.Get('h1000')
		    nevents = tree.GetEntries()
              
	    
                    for ev in range(1, nevents):

                       tree.GetEntry(ev)
 
                       pha = tree.m_cispar[5] 
                       dac = tree.m_cispar[6]
                       cap = tree.m_cispar[7]

		       injCharge = (2*4.096*dac*cap)/1023

                       Tfit = getattr(tree, 'tfit%s%02d' % (partmap[part], mod))
		       Efit = getattr(tree, 'efit%s%02d' % (partmap[part], mod))
		       
                       
		       if cap!=100 or dac!=120 or pha!=0:
	                   continue	 
	
                       for pmt in range(48):   
			       
			   tfit = Tfit[pmt]
			   efit = Efit[pmt]

				 
                           if values[partInd][modInd][pmt] == 'noninst':
		               pass
		           elif 0.9*injCharge>efit or 1.1*injCharge<efit:
			       #don't use events which differ from injected charge by more than 10%
			       pass
		           elif values[partInd][modInd][pmt] == 'real':
			       values[partInd][modInd][pmt] = [tfit, 1]
		           else:
			       values[partInd][modInd][pmt][0] += tfit
			       values[partInd][modInd][pmt][1] += 1
                
    injection_yes = 0
    injection_no = 0
    for partition in values:
	for module in partition:
            for channel in module:
	        if isinstance(channel, (list, tuple)):
		    injection_yes += 1
		    loc = values.index(partition), partition.index(module), module.index(channel)
		    values[loc[0]][loc[1]][loc[2]] = channel[0]/channel[1]
		elif channel == 'real':
		    injection_no += 1

    print 'Found corrections for ', 100*float(injection_yes) / (injection_yes + injection_no), '% of detector'

    return values



    

def write_corr(run, values, outdir):

    ROOT.gROOT.SetBatch()

    c1 = SetupDraw()

    mb_corr = open('%smb_corr_deltas_%d.txt' % (outdir, run), 'w')

    h1dt = ROOT.TH1F('h1dt','', 100, -50, 50)
    hmb = ROOT.TProfile('hmb', '', 4, -0.5, 3.5, -50, 50)
    hpar = ROOT.TProfile('hpar', '', 4, -0.5, 3.5, -50, 50)
    hpart1d = [ROOT.TH1F('hpart1d%d' % i, '', 100, -50, 50) for i in range(4)]
    hpartmod = [ROOT.TProfile('hpartmod%d' % i, '', 64, 0.5, 64.5, -50, 50) for i in range(4)]
    hpartmodchan = [[ROOT.TProfile('hpartmodchan_%d_%d' % (i,j), '%02d'%j, 48, -0.5, 47.5, -50, 50) for j in range(1, 65)] for i in range(4)]
    hpartmodchancolor = [ROOT.TProfile2D('hpartmodchancolor_%d_%02d' % (i,j), '', 64, 0.5, 64.5, 48, -0.5, 47.5, -50, 50) for i in range(4)]

    hpar.SetErrorOption('s')
    hmb.SetErrorOption('s')
    for h in hpartmod:
            h.SetErrorOption('s')
    for array in hpartmodchan:
        for h in array:
            h.SetErrorOption('s')


    for partition in values:
        for module in partition:
	    for channel in module:
		npart, nmod, npmt = values.index(partition), partition.index(module), module.index(channel)
                
                tfit = channel
                if tfit in ['noninst', 'real']:
                    continue		    
                h1dt.Fill(channel)
                hmb.Fill(int(npmt)/12, tfit)
                hpar.Fill(npart, tfit)
                hpart1d[npart].Fill(tfit)
                
                hpartmod[npart].Fill(nmod+1, tfit) 
                hpartmodchan[npart][nmod].Fill(npmt, tfit)
                hpartmodchancolor[npart].Fill(nmod+1, npmt, tfit)
                

    timestr = '<t_{fit}>_{chan} (ns)'
    
    c1.Print('%scis_timing_%d.ps[' % (outdir, run))

    ROOT.gStyle.SetOptStat('emrou')
    h1dt.GetXaxis().SetTitle(timestr)
    h1dt.GetYaxis().SetTitle('Number of Channels')
    h1dt.GetXaxis().SetTitleOffset(1.14)
    h1dt.GetYaxis().SetTitleOffset(1.34)
    h1dt.Draw()
    c1.Print('%scis_timing_%d.ps' % (outdir, run))
    ROOT.gStyle.SetOptStat(0)
    
    hmb.GetXaxis().SetBinLabel(1, 'MB1')
    hmb.GetXaxis().SetBinLabel(2, 'MB2')
    hmb.GetXaxis().SetBinLabel(3, 'MB3')
    hmb.GetXaxis().SetBinLabel(4, 'MB4')
    hmb.GetYaxis().SetTitle(timestr)
    hmb.Draw()
    c1.Print('%scis_timing_%d.ps' % (outdir, run))
    

    name = ['LBA', 'LBC', 'EBA', 'EBC']

    hpar.GetYaxis().SetTitle(timestr)
    hpar.GetXaxis().SetTitle('Partition')
    for i in range(4):
	    hpar.GetXaxis().SetBinLabel(i+1, name[i])

    hpar.Draw()
    c1.Print('%scis_timing_%d.ps' % (outdir, run))

    c1.Print('%scis_timing_%d.ps]' % (outdir, run))

    for i in range(4):
	    ROOT.gStyle.SetOptStat('emrou')
	    c1.Print('%scis_timing_%d_%s.ps[' % (outdir, run, name[i]))
	    hpart1d[i].GetXaxis().SetTitle('%s %s' % (name[i], timestr))
	    hpart1d[i].GetYaxis().SetTitle('Number of Channels')
	    hpart1d[i].GetXaxis().SetTitleOffset(1.14)
	    hpart1d[i].GetYaxis().SetTitleOffset(1.34)
	    hpart1d[i].Draw()
	    c1.Modified()
	    c1.Update()
	    c1.Print('%scis_timing_%d_%s.ps' % (outdir, run, name[i]))
	    ROOT.gStyle.SetOptStat(0)
    
	    hpartmodchancolor[i].SetTitle('%s' % timestr)
	    hpartmodchancolor[i].GetXaxis().SetTitle('%s Module Number' % name[i])
	    hpartmodchancolor[i].GetYaxis().SetTitle('PMT Number')
	    hpartmodchancolor[i].SetMaximum(22)
	    hpartmodchancolor[i].SetMinimum(-32)
	    hpartmodchancolor[i].Draw('COLZ')

	    c1.Modified()
	    c1.Update()
	    c1.Print('%scis_timing_%d_%s.ps' % (outdir, run, name[i]))
    
	    hpartmod[i].GetXaxis().SetTitle('%s Module Number' % name[i])
	    hpartmod[i].GetYaxis().SetTitle(timestr)
	    hpartmod[i].Draw()
	    c1.Modified()
	    c1.Update()
	    c1.Print('%scis_timing_%d_%s.ps' % (outdir, run, name[i]))
	    
	    for h in hpartmodchan[i]:
		    if not h.GetEntries():
			    mb_corr.write('%s%s' % (name[i], h.GetTitle()))
			    for ind in range(1,5):
				    mb_corr.write(' %d' % 999) 
			    mb_corr.write('\n')
			    continue
		    
		    h.GetXaxis().SetTitle('%s%s PMT Number' % (name[i], h.GetTitle()))
		    h.GetYaxis().SetTitle(timestr)
		    h.Draw()
		    c1.Modified()
		    c1.Update()
		    c1.Print('%scis_timing_%d_%s.ps' % (outdir, run, name[i]))
		    
		    h.Rebin(48/4)
		    mb_corr.write('%s%s' % (name[i], h.GetTitle()))
		    

		    values = [0, 115, 70, 60, 20]
		    
		    
		    backwardRange = [4,3,2,1] # mb 1 is last channels in a drawer - mb 4 is first channels
		    for k in backwardRange:
                        mb_corr.write(' %d' % (-1 * h.GetBinContent(k)*2))
			    
		    mb_corr.write('\n')
		    print
    
	    c1.Print('%scis_timing_%d_%s.ps]' % (outdir, run, name[i]))
    mb_corr.close()


def main():
   
   datadir = '/location/of/ntuples/'
   outdir = '/where/output/will/be/written/'

   if len(sys.argv)!=2:
	 
	print 'Please pass a run number.'   
        return True

   else:
	runnumber = int(sys.argv[1])  

	datadir = datadir + str(runnumber) + '/'
	outdir = outdir + str(runnumber) + '/'

	if not os.path.exists(outdir):
	    os.mkdir(outdir)	

        print 'Calculating mother board corrections from run', runnumber
	print 'Using data from', datadir
	print 'Ouput will be sent to', outdir
         
        values = doModules(runnumber, datadir)
	write_corr(runnumber, values, outdir)
	

if __name__ == "__main__":
   main()
		
	
	
