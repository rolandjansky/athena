# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *
import argparse,sys,os

## read options                                                                                                          
parser = argparse.ArgumentParser()
parser.add_argument('--ifile', help='input ilumicalc file', dest='ifile', default='')
parser.add_argument('--ofile', help='output file', dest='ofile', default='ilumi2histo.root')
parser.add_argument('--relunc', help='luminosity relative uncertainty [%]', dest='relunc', default=3.2)

config = parser.parse_args()

if not config.ifile:
    sys.exit('** You need to provide an iLumiCalc input file!')


lumi_map = {}

try:
    m_file = TFile.Open(config.ifile,'read')

    for k in  m_file.GetListOfKeys():
        if '_intlumi' in k.GetName():

            run  =  m_file.Get(k.GetName()).GetTitle().split()[-1]

            #OLD WAY (look at histogram title)
            #if not 'Delivered' in  m_file.Get(k.GetName()).GetTitle():
            #    print 'WARNING : No lumi provided for run ',run 
            #    continue
            #lumi =  m_file.Get(k.GetName()).GetTitle().split('=')[1]
            #lumi =  float(lumi.split('/')[0])


            #NEW WAY (look at last filled bin) 
            h = m_file.Get(k.GetName())
            lumi = h.GetBinContent( h.GetNbinsX()-9 )

            lumi_map[run] = float(lumi)*1e-6

except IOError as e:
    print('IlumiCalc file not found!')
    print(os.strerror(e.errno))


lumi_histo = TH1F('lumi_histo',';;Int. Luminosity [pb]',len(lumi_map.keys()),0.,float(len(lumi_map.keys())))
lumi_histo.Sumw2()

ib=1
lsrt = lumi_map.keys()
lsrt.sort()
for k in lsrt:
    lumi_histo.SetBinContent(ib, float(lumi_map[k]))
    if config.relunc > 0:
        lumi_histo.SetBinError(ib, float(lumi_map[k])*float(config.relunc)/100.)
    lumi_histo.GetXaxis().SetBinLabel(ib, k)
    ib+=1

lumi_histo.SaveAs(config.ofile)




