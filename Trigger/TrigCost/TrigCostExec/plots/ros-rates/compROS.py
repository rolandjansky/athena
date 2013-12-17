#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Usage: python plotROS.py -s SORT_ROS.txt
# -b baseline file

import os
import re
import sys
import math


from optparse import OptionParser
    
p = OptionParser(usage="usage: <path:ROOT file directory>", version="0.1")

p.add_option('-s',         type='string',  default="SORT_ROS.txt",                     dest='sfile',   help="ROS sorting file")
p.add_option('-i',         type='string',  default='ros_rates_run_167607_std_v3.txt',  dest='ifile',   help="Input ROS rates file")
p.add_option('-b',         type='string',  default='ros_rates_run_182454_lb0195.txt',  dest='bfile',   help="Baseline ROS rates file to compare to")
p.add_option('--req', action='store_true', default=False,                              dest="req",     help="Plot ROS Request rates")
p.add_option('--band',action='store_true', default=False,                              dest="band",    help="Plot ROS Bandwidth")
p.add_option('--err', action='store_true', default=False,                              dest="err",     help="Add errors to plots")
p.add_option('--wait',action='store_true', default=False,                              dest="wait",     help="Wait mode")

(options, args) = p.parse_args()  

#--------------------------------------------------------------------------
# ROS sorting list
#
sortROS = {}

def readSortROS(path):
    global sortROS    
    file = open(path, 'r')

    for line in file.readlines():
        parts = line.split()

        if len(parts) == 2:
            sortROS[parts[1]] = int(parts[0])

#--------------------------------------------------------------------------
# ROS helper class
#
class ROSData:

    def __init__(self):
        self.ros          = None
        self.ret_rate     = None
        self.req_rate     = None
        self.ret_band     = None
        self.req_cach     = None
        self.ret_rate_err = None
        self.req_rate_err = None
        self.ret_band_err = None
        self.req_cach_err = None
        self.subdet       = None
        self.subros       = None
        
    def ReadLine(self, line):
        
        parts = line.split()
        if len(parts) < 1:
            print 'ReadLine - error for line: %s' %(line)
            return

        self.ros = parts[0]
        keys     = self.ros.split('-')
        
        if len(keys) == 4:
            self.subdet = keys[1]
            self.subros = keys[2]
            
            if self.ros.count('FCAL'):
                self.subdet = 'FCAL'
            if self.ros.count('TIL'):
                self.subdet = 'TILE'
            if self.ros.count('HEC'):
                self.subdet = 'HEC'
        elif len(keys)==3:
            if self.ros.count('MET'):
                self.subdet = 'MET'
                
        if len(parts) == 11:
            self.req_rate     = float(parts[5])
            self.ret_rate     = float(parts[6])
            self.ret_band     = float(parts[7])
            self.req_cach     = float(parts[5])-float(parts[6])
            if float(parts[5]) > 0.0:
                self.req_rate_err = math.sqrt(float(parts[5]))
            if float(parts[6]) > 0.0:
                self.ret_rate_err = math.sqrt(float(parts[6]))
            if float(parts[7]) > 0.0:
                self.ret_band_err = math.sqrt(float(parts[7]))
            if self.req_cach > 0.0:
                self.req_cach_err = math.sqrt(float(self.req_cach))

        elif len(parts) == 14:
            self.req_rate     = float(parts[6])
            self.ret_rate     = float(parts[7])
            self.req_cach     = float(parts[8])
            self.ret_band     = float(parts[9])
            if float(parts[6]) > 0.0:
                self.req_rate_err = math.sqrt(float(parts[6]))
            if float(parts[7]) > 0.0:
                self.ret_rate_err = math.sqrt(float(parts[7]))
            if float(parts[8]) > 0.0:
                self.req_cach_err = math.sqrt(float(parts[8]))
            if float(parts[8]) > 0.0:
                self.ret_band_err = math.sqrt(float(parts[9]))
                
        elif len(parts) == 22:
            self.req_rate     = float(parts[10])
            self.ret_rate     = float(parts[13])
            self.ret_band     = float(parts[16])
            self.req_rate_err = float(parts[12])
            self.ret_rate_err = float(parts[15])
            self.ret_band_err = float(parts[18])
            
        else:
            self.subdet   = None
            print 'ROSData - failed to read sub-det: %s' %self.ros

    def GetPixelSubROS(self):
        if self.subros == 'B':    return 'Barrel'
        if self.subros == 'BL':   return 'BLayer'
        if self.subros == 'DISK': return 'Disk'        
            
    def Print(self):
        print 'ROS=%-20s sub-det=%s req_rate=%6.1f Hz ret_rate=%6.1f Hz ret_band=%6.1f Kb/s' %(self.ros, self.subdet, self.req_rate, self.ret_rate, self.ret_band)
        if options.err:
            print 'ROS=%-20s sub-det=%s req_rate_err=%6.1f Hz ret_rate_err=%6.1f Hz ret_band_err=%6.1f Kb/s' %(self.ros, self.subdet, self.req_rate_err, self.ret_rate_err, self.ret_band_err)

#--------------------------------------------------------------------------
# ROS comparison by name
#
def ros_subdet_number(val):

    if type(val) == type(ROSData()):
        subdet = val.subdet
    else:
        subdet = val

    if subdet == 'PIX':  return 1
    if subdet == 'SCT':  return 2
    if subdet == 'TRT':  return 3
    if subdet == 'LAR':  return 4
    if subdet == 'TILE': return 5
    if subdet == 'HEC':  return 6
    if subdet == 'FCAL': return 7   
    if subdet == 'MDT':  return 8
    if subdet == 'RPC':  return 10
    if subdet == 'TGC':  return 9
    if subdet == 'CSC':  return 11
    if subdet == 'MET':  return 12
    if subdet == 'TDQ':  return 13

    print 'Unknown subdet: %s' %subdet
    return 100

#--------------------------------------------------------------------------
# ROS comparison by name
#
def ros_key(ros):

    if ros.ros in sortROS:
        return '%03d' %sortROS[ros.ros]

    print 'SORT_ROS - error: unknown ROS %s' %ros.ros
    return 'subdet_%02d_%s' %(ros_subdet_number(ros), ros.ros)
        
#--------------------------------------------------------------------------
# ROS comparison by name
#
def FillROSHist(file, color, scale = 1.0):
    #
    # Open ROOT files for writing
    #
    tfile = open(file, 'r')

    roses   = []
    subdets = {}

    for tline in tfile.readlines():
        if tline.count('ROS') == 0:
            continue

        rline = tline.splitlines()[0]
        if rline.count('Events') or rline.count('TDQ'):
            continue

        ros = ROSData()
        ros.ReadLine(rline)
        ros.Print()
        
        roses += [ros]

        try:
            subdets[ros.subdet].append(ros)
        except KeyError:
            subdets[ros.subdet] = [ros]

    for subdet in sorted(subdets.keys()):
        data = subdets[subdet]
        print '%s: has %d ROSs' %(subdet, len(data))


    nbin = len(roses)+2*len(subdets)
    hros = ROOT.TH1D('hros', 'hros', nbin, 0.0, nbin)
    hros.SetDirectory(0)
    hros.SetStats(False)
    hros.SetFillColor(color)
    hros.SetLineColor(color)
    hros.SetLineWidth(2)
    if options.req:
        hros.GetYaxis().SetTitle('Data request rate/ROS [Hz]')
    elif options.band:
        hros.GetYaxis().SetTitle('Data bandwidth rate/ROS [Kb/s]')
    else:
        hros.GetYaxis().SetTitle('Data retrieval rate/ROS [Hz]')
    hros.GetYaxis().SetTitleOffset(1.3)
    
    ibin = 1
    idet = None
    nros = 0

    for ros in sorted(roses, key = ros_key):
        #print '%s %s' %(ros.subdet, ros.ros)
        
        if idet != ros.subdet:
            if idet != None:
                hros.GetXaxis().SetBinLabel(ibin-1-nros/2, idet)
                ibin += 2

            nros = 0
            idet = ros.subdet

        if options.req:
            hros.SetBinContent(ibin, scale*ros.req_rate)
            if options.err:
                hros.SetBinError(ibin, scale*ros.req_rate_err)
        elif options.band:
            hros.SetBinContent(ibin, scale*ros.ret_band)
            if options.err:
                hros.SetBinError(ibin, scale*ros.ret_band_err)
        else:    
            hros.SetBinContent(ibin, scale*ros.ret_rate)
            if options.err:
                hros.SetBinError(ibin, scale*ros.ret_rate_err)
        ibin += 1
        nros += 1

    if idet != None:
        hros.GetXaxis().SetBinLabel(ibin-nros/2, idet)
        
    print 'nros=%d ndet=%d nbin=%d ibin=%d ' %(len(roses), len(subdets), nbin, ibin)
    return hros, roses

#--------------------------------------------------------------------------
# ROS comparison by name
#
def FillPIXHist(roses_, color):
    #
    # Plot pixel roses
    #
    roses   = []

    for ros in roses_:
        if ros.ros.count('PIX') != 0:
            roses.append(ros)

    nbin = len(roses)+2
    hpix = ROOT.TH1D('hpix', 'hpix', nbin, 0.0, nbin)
    hpix.SetDirectory(0)
    hpix.SetStats(False)
    hpix.SetFillColor(color)
    hpix.SetLineColor(color)
    hpix.SetLineWidth(2)
    hpix.SetMarkerStyle(0)
    
    hpix.GetXaxis().SetLabelSize(0.08)
    hpix.GetXaxis().SetLabelOffset(0.018)

    if options.req:
        hpix.GetYaxis().SetTitle('Data request rate/ROS [Hz]')
    if options.band:        
        hpix.GetYaxis().SetTitle('Data bandwidth rate/ROS [Kb/s]')
    else:
        hpix.GetYaxis().SetTitle('Data retrieval rate/ROS [Hz]')
    hpix.GetYaxis().SetTitleOffset(1.3)
    
    ibin = 1
    iros = None
    nros = 0

    for ros in sorted(roses, key = ros_key):
        print '%s %s  %f' %(ros.subdet, ros.ros, ros.ret_rate)
        subros = ros.GetPixelSubROS()
        
        if iros != subros:
            if iros != None:
                hpix.GetXaxis().SetBinLabel(ibin-1-nros/2, iros)
                ibin += 1

            nros = 0
            iros = subros
            
        hpix.SetBinContent(ibin, ros.ret_rate)
        ibin += 1
        nros += 1

    if iros != None:
        hpix.GetXaxis().SetBinLabel(ibin-1-nros/2, iros)
        
    print 'nros=%d nbin=%d ibin=%d ' %(len(roses), nbin, ibin)
    return hpix

###########################################################################
# Main function for command line execuation
#
if __name__ == "__main__":

    #
    # Read sorting order
    #
    if options.sfile != None:
        readSortROS(options.sfile)

    #
    # Make histograms
    #
    import ROOT
#    import TrigCostExec.TrigCostExecPython as plots 
#    plots.setPlotDefaults(ROOT)

    can = ROOT.TCanvas('ros', 'ros', 1000, 600)
    can.cd()
    can.SetLogy(False)
    can.SetRightMargin(0.08)
    
    hroses  = []
    icolor  = 2
    drawopt = ''

    hros1, roses1 = FillROSHist(options.ifile, 6)
    hros4, roses4 = FillROSHist(options.bfile, 4)
    #    hros2, roses2 = FillROSHist('ros_rates_run_167607_mod_v3.txt', 8)
    #    hros5, roses5 = FillROSHist('ros_rates_run_1e33_pred_656.txt', 8, scale=1.1)

    #
    # Compare two online runs
    #
    hros1.SetFillColor(8)
    hros1.SetLineColor(8)

    hros4.SetFillColor(0)
    hros4.SetLineColor(1)

    hros1.GetYaxis().SetRangeUser(0.0, 1.1*hros1.GetMaximum())
    
    hros1.Draw()
    hros4.Draw('SAME')
    
    leg4 = ROOT.TLegend(0.20, 0.63, 0.80, 0.90)
    leg4.SetHeader('ATLAS Trigger Operations')
    leg4.SetBorderSize(0)
    leg4.AddEntry(hros1, options.ifile, 'f')
    leg4.AddEntry(hros4, options.bfile, 'l')
    leg4.Draw()
        
    can.Modified()
    can.Update()
    if options.wait:
        can.WaitPrimitive()

    if not can or can.Closed():
        sys.exit(0)

    can.Print(options.sfile+'_and_'+options.bfile+'.pdf')
    can.Print(options.sfile+'_and_'+options.bfile+'.eps')

    #
    # Compare two online runs
    #
    hros1.Divide(hros4)
    
    bin=0
    ymax=0.0
    
    while bin < 142:
        currBin=hros1.GetBinContent(bin)
        print "bin:",currBin
        if currBin> ymax:
            ymax=currBin    
        bin=bin+1
    print "hist max:",hros1.GetMaximumBin()

    hros1.GetYaxis().SetRangeUser(0.0, 1.1*ymax)

    hros1.SetTitle("Ratio of Menu to Baseline")

    hros1.Draw()
    
    can.Modified()
    can.Update()
#    s=raw_input()
    if options.wait:
        can.WaitPrimitive()

    if not can or can.Closed():
        sys.exit(0)

    can.Print(options.sfile+'_over_'+options.bfile+'.pdf')
    can.Print(options.sfile+'_over_'+options.bfile+'.eps')


