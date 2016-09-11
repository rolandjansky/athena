#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import os, sys, re
from ROOT import *
# from rootUtil import useAtlasStyle, waitRootCmd, savehistory, get_default_fig_dir 

funlist=[]

def waitNext(savename='temp', pad=gPad):
    while True:
        c = raw_input('Next:')
        if c=='q': sys.exit(0)
        elif c=='s':
            pad.SaveAs(savename+'.pdf')
            pad.SaveAs(savename+'.eps')
            pad.SaveAs(savename+'.png')
        elif c=='n':
            return
        else:
            print "'q' to quit\n's' to save\n'n' for next"


def showCutMap(var, effFun, sTag='test'):
    dir0 = '/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/'
    print dir0
    f1 = TFile(dir0+'IsolationSelection/v2/MC15_Z_Jpsi_cutMap.root','read')
    h1 = f1.Get('Muons/DFCommonGoodMuon/mu_cutValues_'+var)
    h1.GetZaxis().SetRangeUser(99,99)
    h1_xy = h1.Project3D('xy')
    h1_xy.GetXaxis().SetTitle(h1_xy.GetXaxis().GetTitle().replace('eta','#eta'))

#     h1_xy.Draw('colz')

    h1_xy.Reset()
    h1_eff = h1_xy.Clone(h1_xy.GetName()+'_eff')
    fun1 = TF1('fun1', effFun, 0.0, 1000.0)
    for ix in range(h1_xy.GetNbinsX()+2):
        for iy in range(h1_xy.GetNbinsY()+2):
            pt = h1_xy.GetYaxis().GetBinCenter(iy)
            print pt,min(int(fun1.Eval(pt)),99),0.1*pt+90
            h1_xy.SetBinContent(ix,iy,h1.GetBinContent(iy,ix, min(int(fun1.Eval(pt)), 99)))
            h1_eff.SetBinContent(ix,iy, min(int(fun1.Eval(pt)),99))

    h1_eff.Draw('colz')
    gPad.Update()
    waitNext(sTag+'_eff')
#     waitRootCmd(sTag+'_eff')
    h1_xy.Draw('colztext')
    lt = TLatex()
    lt.DrawLatexNDC(0.25,0.95,var+', #epsilon='+effFun.replace('x','p_{T}')+' [%]')

    gPad.Update()
    waitNext(sTag+'_cuts')
#     waitRootCmd(sTag+'_cuts')

def test():
    dir1 = './'
#     dir1 = get_default_fig_dir()

#     showCutMap('ptvarcone30', '99', dir1+'LooseTrackOnlyIso_ptvarcone30')
# 
#     showCutMap('topoetcone20', '99', dir1+'LooseIso_topoetcone20')
    showCutMap('topoetcone20', '96', dir1+'TightIso_topoetcone20')
# 
#     showCutMap('ptvarcone30', '96', dir1+'TightIso_ptvarcone30')
# 
#     showCutMap('topoetcone20', '0.1143*x+92.14', dir1+'GradientIso_topoetcone20')
#     showCutMap('ptvarcone30', '0.1143*x+92.14', dir1+'GradientIso_ptvarcone30')

#     showCutMap('topoetcone20', '0.057*x+95.57', dir1+'GradientLooseIso_topoetcone20')
#     showCutMap('ptvarcone30', '0.057*x+95.57', dir1+'GradientLooseIso_ptvarcone30')

funlist.append(test)

if __name__ == '__main__':
#     savehistory('.')
#     useAtlasStyle()
    gStyle.SetPadRightMargin(0.14)
    gStyle.SetPadTopMargin(0.06)
    gStyle.SetPaintTextFormat('.2f');
    gStyle.SetOptTitle(0)
    gStyle.SetOptStat(0)
    for fun in funlist: print fun()
