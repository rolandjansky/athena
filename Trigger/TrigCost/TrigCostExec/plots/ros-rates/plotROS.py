#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# Usage: python plotROS.py -s SORT_ROS.txt
#

import os
import re
import sys

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
        self.ros      = None
        self.ret_rate = None
        self.req_rate = None
        self.subdet   = None
        self.subros   = None
        
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

        if len(parts) == 14:
            self.req_rate = float(parts[6])
            self.ret_rate = float(parts[7])

        elif len(parts) == 11:
            self.req_rate = float(parts[5])
            self.ret_rate = float(parts[6])
                    
        elif len(parts) == 22:
            self.req_rate = float(parts[10])
            self.ret_rate = float(parts[13])
        else:
            self.subdet   = None
            print 'ROSData - failed to read sub-det: %s' %self.ros

    def GetPixelSubROS(self):
        if self.subros == 'B':    return 'Barrel'
        if self.subros == 'BL':   return 'BLayer'
        if self.subros == 'DISK': return 'Disk'        
            
    def Print(self):
        print 'ROS=%-20s sub-det=%s req_rate=%6.1f Hz ret_rate=%6.1f Hz' %(self.ros, self.subdet, self.ret_rate, self.req_rate)

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
            
        hros.SetBinContent(ibin, scale*ros.ret_rate)
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

    from optparse import OptionParser
    
    p = OptionParser(usage="usage: <path:ROOT file directory>", version="0.1")

    p.add_option('-n',         type='int',    default=0,             dest='nfile')
    p.add_option('-s',         type='string', default=None,          dest='sfile')
    
    (options, args) = p.parse_args()  

    #
    # Read sorting order
    #
    if options.sfile != None:
        readSortROS(options.sfile)

    #
    # Make histograms
    #
    import ROOT
    import TrigCostExec.TrigCostExecPython as plots
    
    plots.setPlotDefaults(ROOT)

    can = ROOT.TCanvas('ros', 'ros', 1000, 600)
    can.cd()
    can.SetLogy(False)
    can.SetRightMargin(0.08)
    
    hroses  = []
    icolor  = 2
    drawopt = ''

    hros1, roses1 = FillROSHist('ros_rates_run_201555_LB70.txt', 6)
    #hros2, roses2 = FillROSHist('ros_rates_run_167607_mod_v3.txt', 8)
    hros2, roses2 = FillROSHist('ros_rates_run_78e32_r3592.txt', 4)
    hros4, roses4 = FillROSHist('ros_rates_run_52e32_pred_r3589.txt', 4)
    #hros5, roses5 = FillROSHist('ros_rates_run_1e33_pred_656.txt', 8, scale=1.1)
    hros5, roses5 =  FillROSHist('ros_rates_run_52e32_pred_r3589.txt', 8)

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
    leg4.AddEntry(hros1, '2012 (run 201555 LB 70) online rates at 5.0 #times 10^{33} cm^{-2}s^{-1}', 'f')
    leg4.AddEntry(hros4, 'Extrapolated rates at 5.0 #times 10^{33} cm^{-2}s^{-1}', 'l')
    leg4.Draw()
        
    can.Modified()
    can.Update()
    can.WaitPrimitive()

    if not can or can.Closed():
        sys.exit(0)

    can.Print('ros_rate_compare_pred_5e33_201555.pdf')
    can.Print('ros_rate_compare_pred_5e33_201555.eps')

    #
    # Compare 1E33 prediction and online run
    #
    hros5.SetFillColor(8)
    hros5.SetLineColor(8)

    hros2.SetFillColor(0)
    hros2.SetLineColor(1)

    hros5.GetYaxis().SetRangeUser(0.0, 1.4*hros5.GetMaximum())
    
    hros5.Draw()
    hros2.Draw('SAME')
    
    leg5 = ROOT.TLegend(0.20, 0.67, 0.80, 0.90)
    leg5.SetHeader('ATLAS Preliminary')
    leg5.SetBorderSize(0)
    leg5.AddEntry(hros2, 'Extrapolated rates at 7.5 #times 10^{33} cm^{-2}s^{-1}', 'l')
    leg5.AddEntry(hros5, 'Extrapolated rates at 5.0 #times 10^{33} cm^{-2}s^{-1}', 'f')
    leg5.Draw()
        
    can.Modified()
    can.Update()
    can.WaitPrimitive()

    if not can or can.Closed():
        sys.exit(0)

    can.Print('ros_rate_compare_pred_7.5e33_and_5e33.pdf')
    can.Print('ros_rate_compare_pred_7.5e33_and_5e33.eps')
    
    #
    # Plot pixel rates
    #

    #can2 = ROOT.TCanvas('ros', 'ros')
    #can2.cd()
    #can2.SetLogy(False)
    
    #hpix1 = FillPIXHist(roses1, 1)
    #hpix2 = FillPIXHist(roses2, 1)
    #
    #hpix1.SetFillColor(0)
    #hpix1.SetLineColor(1)
    #
    #hpix2.SetFillColor(3)
    #hpix2.SetLineColor(3)
    #
    #hpix1.GetYaxis().SetRangeUser(0.0, 2.2e4)
    #hpix2.GetYaxis().SetRangeUser(0.0, 2.2e4)
    #
    #hpix2.Draw()
    #hpix1.Draw('SAME')
    #
    #leg2 = ROOT.TLegend(0.2, 0.69, 0.90, 0.85)
    #leg2.SetHeader('ATLAS Preliminary')
    #leg2.SetBorderSize(0)
    #leg2.SetFillStyle(0)
    #leg2.AddEntry(hpix1, 'Rates measured online with 2010 pixel cable map', 'l')
    #leg2.AddEntry(hpix2, 'Rates computed offline with new pixel cable map', 'f')
    #leg2.Draw()
    #    
    #can2.Modified()
    #can2.Update()
    #can2.WaitPrimitive()
    #
    #if not can2 or can2.Closed():
    #    sys.exit(0)
    #
    #can2.Print('ros_rate_pixel.pdf')
    #can2.Print('ros_rate_pixel.eps')

