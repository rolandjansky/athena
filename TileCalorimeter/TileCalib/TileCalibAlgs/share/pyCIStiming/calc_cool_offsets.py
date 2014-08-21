#!/bin/env python
# Author: Michael Miller (miller@uchicago.edu)

import os, struct, sys, math
import ROOT 
from aux_functions import *


def readCOOLBestPhases(runnumber_lg, runnumber_hg, outdir):

   # reads TileInfoDump log
   
   print "Going to find COOL Best Phases."
   

   infile=open('TileInfoDump_phases.txt', 'r')
   outfile=open('%scool_values_%d_%d.txt' % (outdir, runnumber_lg, runnumber_hg), 'w')

   bestPhases = [[[ checkChannelValid(part, mod, chan) for chan in range(48)] for mod in range(64)] for part in range(4)] 

   for line in infile:
	   a = line.strip().split(' ')
	   if a[0]!="TileInfoDump": # don't need this
               continue
	   else:
	       b = a[10].strip().split('/') # module info part/mod/chan/gain
	       if b[0]=='0': # don't need this
                  continue
	   
	   if b[3]=='0': # two cases for gains - lg
              c = a[14].strip().split('\t')
              outfile.write('Tccis '+str(hex(int(b[0]) * 0x100 + int(b[1])))+' '+str(b[2])+' '+str(c[0]))
	      part = int(b[0])-1
	      mod = int(b[1])
	      chan = int(b[2])
	      if bestPhases[part][mod][chan] == False:
		   pass
	      else:
		   bestPhases[part][mod][chan] = [float(c[0]), 'no_hg']
	           
           elif b[3]=='1': # hg
              c = a[14].strip().split('\t')
              outfile.write(' '+str(c[0])+'\n')
	      part = int(b[0])-1
	      mod = int(b[1])
	      chan = int(b[2])
	      	      
	      if bestPhases[part][mod][chan]==False:
		   pass   
	      elif bestPhases[part][mod][chan][1]=='no_hg':
		   bestPhases[part][mod][chan][1]=float(c[0])
	      else:
		   print "We should never get here."
		   continue
	   
           else:
              print "Something is wrong with the input file. Leaving."
              break

   print "Done finding COOL Best Phases." 
   return bestPhases  



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

def cleanHisto(hist):

   hist.GetXaxis().CenterTitle()
   #hist.GetXaxis().CenterLabels(ROOT.kTRUE)
   hist.GetXaxis().SetTitleOffset(1.3)
   #hist.GetXaxis().SetTitle('T_{fit} [ns]')
   #hist.GetXaxis().SetLabelOffset(0.021)
   #hist.GetXaxis().SetLabelSize(0.06)
   hist.GetYaxis().CenterTitle()
   hist.GetYaxis().SetTitleOffset(1.3)
   #hist.GetYaxis().SetLabelOffset(0.015)
   #hist.GetYaxis().SetLabelSize(0.06)
   hist.SetFillColor(ROOT.kBlue)

   return hist




def calcAvgTimes(runnumber, bestPhases, gain, datadir, outdir):

    print "Going to calculate average times."
    ROOT.gROOT.SetBatch()

    gainnames = ['LG', 'HG']
    h_tfit=ROOT.TH1F("tfit_ev_%s" % gainnames[gain], "Run %d: %s t_{fit} distribution" % (runnumber, gainnames[gain]), 200, -50., 50)    
    h_tfit_cool=ROOT.TH1F("tfit_cool_ev_%s" % gainnames[gain], "Run %d: %s" % (runnumber, gainnames[gain]), 200, -50., 50)
    
    histos = [h_tfit, h_tfit_cool]

    xnames = ['t_{fit} (ns)', 't_{fit} - CoolBestPhase (ns)']
       
    for i in range(2):
	histos[i].GetXaxis().SetTitle(xnames[i])
        histos[i].GetXaxis().CenterTitle()
        histos[i].GetXaxis().SetTitleOffset(1.2)
	histos[i].SetFillColor(ROOT.kGray)
    

    times = [[[ checkPMTValid(part, chan) for chan in range(48)] for mod in range(64)] for part in range(4)]

    dacvals = [120, 7]
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
		       
                       
		       if cap!=100 or dac!=dacvals[gain] or pha!=0:
	                   continue	 
	
                       for pmt in range(48):   
			       
			   tfit = Tfit[pmt]
			   efit = Efit[pmt]

			   h_tfit.Fill(tfit, 1.)
			   
			   bestPhase = bestPhases[partInd][modInd][int(PMTtoChannel(partInd, pmt))]
			   if not isinstance(bestPhase, list):
			       continue
			   bestPhase = float(bestPhase[gain])
			   h_tfit_cool.Fill(tfit - bestPhase, 1.)
				 
                           if times[partInd][modInd][pmt] == 'noninst':
		               pass
		           elif 0.9*injCharge>efit or 1.1*injCharge<efit:
			       #don't use events which differ from injected charge by more than 10%
			       pass
		           elif times[partInd][modInd][pmt] == 'real':
			       times[partInd][modInd][pmt] = [tfit, 1]
		           else:
			       times[partInd][modInd][pmt][0] += tfit
			       times[partInd][modInd][pmt][1] += 1
                
    injection_yes = 0
    injection_no = 0
    for partition in times:
	for module in partition:
            for channel in module:
	        if isinstance(channel, (list, tuple)):
		    injection_yes += 1
		    loc = times.index(partition), partition.index(module), module.index(channel)
		    times[loc[0]][loc[1]][loc[2]] = channel[0]/channel[1]
		elif channel == 'real':
		    injection_no += 1


    print 'Found corrections for ', 100*float(injection_yes) / (injection_yes + injection_no), '% of ', gainnames[gain], 'channels.'


    for h in histos:
	 can = SetupDraw()
	 h.Draw()
	 can.Update()
	 can.SaveAs('%s%s.ps' % (outdir, h.GetName()))

    return times 


def plot(bestPhases, times, runnumber, gain, outdir):

    print "Going to plot."
    ROOT.gROOT.SetBatch()

    partnames = ['LBA', 'LBC', 'EBA', 'EBC']
    gainnames = ['LG', 'HG']    
    h_avgtime=ROOT.TH1F("avgtime_%s" % gainnames[gain], "Run %d: %s <t_{fit}> distribution" % (runnumber, gainnames[gain]), 200, -50., 50)
    h_bestphase=ROOT.TH1F("bestphases_%s" % gainnames[gain], "Run %d: %s COOL best phase distribution" % (runnumber, gainnames[gain]), 200, -50., 50)
    h_diff=ROOT.TH1F("diff_%s" % gainnames[gain], "Run %d: %s Differences from COOL Phase" % (runnumber, gainnames[gain]), 50, -10., 10)
    h_diff_zoom=ROOT.TH1F('diff_zoom_%s' % gainnames[gain], 'Run %d: %s Zoom differences from COOL Phase' % (runnumber, gainnames[gain]), 200, -1., 1)


    h_diff_part = [ROOT.TProfile2D('diff_%s' % partnames[part], 'Run %d: (<t_{fit}>_{chan} - CoolBestPhase) [ns] in %s' % (runnumber, partnames[part]),\
				   64, 1., 65., 48, 0., 48.) for part in range(4)]
    h_diff_part_abs = [ROOT.TProfile2D('diff_%s_abs' % partnames[part], 'Run %d: |<t_{fit}>_{chan} - CoolBestPhase| in %s' % (runnumber, partnames[part]),\
				       64, 1., 65., 48, 0., 48.) for part in range(4)]
    

    histos = [h_avgtime, h_bestphase, h_diff, h_diff_zoom]
    
    xnames = ['<t_{fit}>_{chan} [ns]', 'BestPhase [ns]', \
	      '<t_{fit}>_{chan} - CoolBestPhase [ns]', \
	      '<t_{fit}>_{chan} - CoolBestPhase [ns]']
    
    for i in range(4):
	histos[i].GetXaxis().SetTitle(xnames[i])
        histos[i].GetXaxis().CenterTitle()
        histos[i].GetXaxis().SetTitleOffset(1.2)
	histos[i].SetFillColor(ROOT.kBlue)


    # careful - times is indexed by pmt-1 - bestPhases is indexed by channel number    
    for part in times:
	for mod in part:
	    for chan in mod:
	        loc = times.index(part)+1, part.index(mod), mod.index(chan)	 
                time = times[loc[0]-1][loc[1]][loc[2]]
		phase = bestPhases[loc[0]-1][loc[1]][int(PMTtoChannel(loc[0]-1, loc[2]))][gain]

 		if ((time in ['noninst', 'real']) or (phase in ['noninst', 'real'])):
		    continue 

		h_avgtime.Fill(time, 1.)
                h_bestphase.Fill(phase, 1.)
                h_diff.Fill(time-phase, 1.)
                h_diff_zoom.Fill(time-phase, 1.)
                
                if not checkChannelValid(loc[0]-1, loc[1], loc[2])=='noninst':
                   h_diff_part[loc[0]-1].Fill(loc[1]-1, loc[2], time-phase)
                   h_diff_part_abs[loc[0]-1].Fill(loc[1]-1, loc[2], abs(time-phase))
                else:
                   h_diff_part[loc[0]-1].Fill(loc[1]-1, loc[2], -10)
                   h_diff_part_abs[loc[0]-1].Fill(loc[1]-1, loc[2], -10)
		     		     
    for hist in h_diff_part:
	hist.GetXaxis().SetTitle('Module Number')
	hist.GetYaxis().SetTitle('Channel Number')
	hist.SetMinimum(-1.0)
	hist.SetMaximum(1.0)
	hist = cleanHisto(hist)
	
    for hist in h_diff_part_abs:
	hist.GetXaxis().SetTitle('Module Number')
	hist.GetYaxis().SetTitle('Channel Number')
	#hist.SetMinimum(0.0)
	#hist.SetMaximum(100.0)
	hist = cleanHisto(hist)    
        hist.GetYaxis().SetTitleOffset(1.5)
                     
	
    for h in histos:
	can = SetupDraw()    
	h.Draw()    
	can.Update()
	can.SaveAs('%s%s.ps' % (outdir, h.GetName()))
	can.Clear()

    del can

    pack = '%stime_locations_%d_%s.ps' % (outdir, runnumber, gainnames[gain])

    can = SetupDraw()
    can.Print('%s[' % pack)

    #lat.Draw()
    can.Divide(2,2)
    cnt = 1
    for hist in h_diff_part:
        can.cd(cnt)  
        ROOT.gStyle.SetOptStat(0)
        hist.Draw('COLZ')
	can.Update()
	can.Modified()
	cnt = cnt + 1
    can.Print(pack)
    del can


     
    can = SetupDraw()
    #can.SetLogz()
    for hist in h_diff_part_abs:
        ROOT.gStyle.SetOptStat(0)
        hist.GetZaxis().SetTitle('ns')
	hist.GetZaxis().CenterTitle()
	hist.Draw('LEGO2Z')
	can.Update()
	can.Modified()
	can.Print(pack)                	

    can.Print('%s]' % pack)


	
    print "Done plotting."

def writeChannels(values_low, values_high, runnumber_lg, runnumber_hg):
	
    file = open('Tile.tccis.%d.%d' % (runnumber_lg, runnumber_hg), 'w')
    
    for i in range(4):
       for j in range(0, 64):
          for k in range(48):
             lg = values_low[i][j][k] 
             if lg in ['real', 'noninst']:
                lg = 0
             hg = values_high[i][j][k]
             if hg in ['real', 'noninst']:
                hg = 0
                
             chan = PMTtoChannel(i, k)
				
	     if lg != 0 and hg != 0: # only update if we have data for both adcs 
		file.write('Tccis ')
                file.write(str(hex((i+1) * 0x100 + j)) + ' ')
                file.write(str(chan) + ' ')
                file.write(str(lg) + ' ')
                file.write(str(hg) + '\n')
                                
    file.close()



def main():

   datadir = '/location/of/ntuples/'
   outdir = '/where/output/will/be/written/'
   
   if len(sys.argv)<3:
       print "Please pass a run number for a low gain and high gain run."
       return True

   else:
       runnumber_lg = int(sys.argv[1])
       runnumber_hg = int(sys.argv[2])

   outdir = outdir + str(runnumber_lg) + '_' + str(runnumber_hg) + '/'
   datadir_lg = datadir + str(runnumber_lg) + '/'
   datadir_hg = datadir + str(runnumber_hg) + '/'
   
   print 'Using run', runnumber_lg, 'for LG and', runnumber_hg, 'for HG'
   print 'Output will be sent to', outdir
   
   if not os.path.exists(outdir):
       os.mkdir(outdir)
      	
   CoolBestPhases = readCOOLBestPhases(runnumber_lg, runnumber_hg, outdir)
   TimesLG = calcAvgTimes(runnumber_lg, CoolBestPhases, 0, datadir_lg, outdir)
   TimesHG = calcAvgTimes(runnumber_hg, CoolBestPhases, 1, datadir_hg, outdir)

   plot(CoolBestPhases, TimesLG, runnumber_lg, 0, outdir)
   plot(CoolBestPhases, TimesHG, runnumber_hg, 1, outdir)

   writeChannels(TimesLG, TimesHG, runnumber_lg, runnumber_hg)
    
if __name__ == '__main__':
    main()
