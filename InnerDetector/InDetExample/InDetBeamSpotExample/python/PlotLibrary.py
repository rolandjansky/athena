#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
A library with miscellaneous beam spot related utilities and plots.
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id $'


from math import sqrt
from time import mktime, strptime
from numpy import array

import ROOT
from InDetBeamSpotExample import ROOTUtils


#
# Plot beam spot size estimated from emittance and beta*
#
def LHCBeamSpotSize(lhcTimeString,     # E.g. ['Apr 4 14:27:30 2010'] (list)
                    lhcBeta1,          # beta* in m (scalar), beam 1
                    lhcEmit1,          # Norm. emittance in um rad (list), beam 1
                    lhcBeta2,          # dito beam 2
                    lhcEmit2,          # dito beam 2
                    betaGamma = 3730.  # Default value of betaGamma for 7 TeV
                    ):
    lhcTime = array([ mktime(strptime(s,'%b %d %H:%M:%S %Y')) for s in lhcTimeString])
    print (lhcTime)

    # Normalized emittance quoted in um rad; lhcEps arrays are emittance in m rad
    lhcEps1 = lhcEmit1 * 1.0e-6 / betaGamma
    lhcEps2 = lhcEmit2 * 1.0e-6 / betaGamma

    # Beam sizes in mm
    lhcSigmaBeam1 = sqrt(lhcBeta1*lhcEps1) * 1.0e3
    print ('Size of beam 1 (mm): ',lhcSigmaBeam1)
    lhcSigmaBeam2 = sqrt(lhcBeta2*lhcEps2) * 1.0e3
    print ('Size of beam 2 (mm): ',lhcSigmaBeam2)

    # Beam spot size in mm
    lhcSigma = 1. / sqrt(1./lhcSigmaBeam1**2 + 1./lhcSigmaBeam2**2)
    print ('Beam spot size (mm): ',lhcSigma)

    # Draw on existing plot
    gr = ROOTUtils.protect( ROOT.TGraph(len(lhcTime),lhcTime,lhcSigma) )
    gr.SetMarkerColor(2)
    gr.SetLineColor(2)
    #gr.SetMarkerSize(1.0)
    #gr.SetMarkerStyle(25)
    gr.SetMarkerSize(1.5)
    gr.SetMarkerStyle(20)
    gr.Draw('PSAME')
    return [(gr,'Expected from  #sqrt{#epsilon #beta*}','P')]


#
# Plot normalized emittance with new axis on right hand side of plot
#
def LHCEmittance(lhcTimeString,     # E.g. ['Apr 4 14:27:30 2010'] (list)
                 lhcEmit1,          # Norm. emittance in um rad (list), beam 1
                 lhcEmit2,          # dito beam 2
                 minEmittance = 0.,
                 maxEmittance = 8.
                 ):
    lhcTime = array([ mktime(strptime(s,'%b %d %H:%M:%S %Y')) for s in lhcTimeString])
    ROOT.gPad.SetTicky(0)   # Remove tick marks on right side y axis
    ROOT.gPad.SetRightMargin(0.1)
    #maxEmittance = max(max(lhcEmit1),max(lhcEmit2))*1.7
    hmin = ROOT.gPad.GetUymin()
    hmax = ROOT.gPad.GetUymax()

    lhcEmit1Data = (lhcEmit1-minEmittance)/(maxEmittance-minEmittance)*(hmax-hmin)+hmin
    gr1 = ROOTUtils.protect( ROOT.TGraph(len(lhcTime),lhcTime,lhcEmit1Data) )
    gr1.SetMarkerColor(4)
    gr1.SetLineColor(4)
    gr1.SetMarkerStyle(25)
    gr1.Draw('PSAME')
    #gr1.Draw('PLSAME')

    lhcEmit2Data = (lhcEmit2-minEmittance)/(maxEmittance-minEmittance)*(hmax-hmin)+hmin
    gr2 = ROOTUtils.protect( ROOT.TGraph(len(lhcTime),lhcTime,lhcEmit2Data) )
    gr2.SetMarkerColor(4)
    gr2.SetLineColor(4)
    gr2.SetMarkerStyle(26)
    gr2.Draw('PSAME')
    #gr2.Draw('PLSAME')

    raxis = ROOTUtils.protect( ROOT.TGaxis(ROOT.gPad.GetUxmax(),hmin,ROOT.gPad.GetUxmax(),hmax,minEmittance,maxEmittance,510,'+L') )
    raxis.SetLineColor(4)
    raxis.SetTitle('Invariant Emittance (#mum rad)')
    raxis.SetTitleColor(4)
    raxis.SetTitleSize(0.05)
    raxis.SetTextFont(42)
    raxis.SetLabelFont(42)
    raxis.SetLabelSize(0.05)
    raxis.SetLabelColor(4)
    raxis.Draw()
    return [(gr1,'Emittance beam 1','P'),(gr2,'Emittance beam 2','P')]


#
# Plots and data for fill 1022
#
fill1022TimeString = ['Apr 4 14:27:30 2010', 'Apr 4 22:27:30 2010', 'Apr 5 3:57:30 2010','Apr 5 7:57:30 2010' ]
fill1022Emit1X = array([ 1.45, 1.8, 1.92, 2.0 ])
fill1022Emit1Y = array([ 1.6, 2.1, 2.45, 2.55 ])
fill1022Emit2X = array([ 1.8, 2.27, 2.27, 2.5 ])
fill1022Emit2Y = array([ 2.6, 4.6, 5.4, 5.8 ])
fill1022Beta1X = 11.54
fill1022Beta1Y = 12.74
fill1022Beta2X =  9.89
fill1022Beta2Y = 12.70

# Beam spot size errors, based on measured beta* errors and assumed 5% error on emittance
# See LuminousSizeError.nb Mathematica notebook
# Witold - forget errors; machine doesn't have a clue; so we'll omit them from the plot
#lhcTimeErr = array([0.,0.,0.,0.])
#lhcSigmaXErr = array([.0031, .0031, .0031, .0031])
#lhcSigmaYErr = array([.0037, .0041, .0041, .0042])
#if plot=='sigx':
#    gr = ROOTUtils.protect( ROOT.TGraphErrors(len(lhcTime),lhcTime,lhcSigmaX,lhcTimeErr,lhcSigmaXErr) )
#if plot=='sigy':
#    gr = ROOTUtils.protect( ROOT.TGraphErrors(len(lhcTime),lhcTime,lhcSigmaY,lhcTimeErr,lhcSigmaYErr) )

def LHCBeamSpotSizeFill1022(plot='sigx'):
    if plot=='sigx':
        return LHCBeamSpotSize(fill1022TimeString,fill1022Beta1X,fill1022Emit1X,fill1022Beta2X,fill1022Emit2X)
    if plot=='sigy':
        return LHCBeamSpotSize(fill1022TimeString,fill1022Beta1Y,fill1022Emit1Y,fill1022Beta2Y,fill1022Emit2Y)

def LHCEmittanceFill1022(plot='sigx'):
    if plot=='sigx':
        return LHCEmittance(fill1022TimeString,fill1022Emit1X,fill1022Emit2X,0.1,8.5)
    if plot=='sigy':
        return LHCEmittance(fill1022TimeString,fill1022Emit1Y,fill1022Emit2Y,0.1,8.5)



#
# Plots and data for fill 1058
#
fill1058TimeString = ['Apr 25 6:03:00 2010', 'Apr 24 6:57:00 2010']
fill1058Emit1X = array([ 5.05, 2.6])
fill1058Emit1Y = array([ 5.04, 2.6])
fill1058Emit2X = array([ 3.88, 2.5])
fill1058Emit2Y = array([10.64, 4.9])
fill1058Beta1X = 2.28
fill1058Beta1Y = 2.02
fill1058Beta2X = 1.92
fill1058Beta2Y = 2.10

def LHCBeamSpotSizeFill1058(plot='sigx'):
    if plot=='sigx':
        return LHCBeamSpotSize(fill1058TimeString,fill1058Beta1X,fill1058Emit1X,fill1058Beta2X,fill1058Emit2X)
    if plot=='sigy':
        return LHCBeamSpotSize(fill1058TimeString,fill1058Beta1Y,fill1058Emit1Y,fill1058Beta2Y,fill1058Emit2Y)

    

# Test code for modules
if __name__ == '__main__':
    gr = LHCBeamSpotSizeFill1022()
