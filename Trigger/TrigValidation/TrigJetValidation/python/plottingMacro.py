#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from time import time
START_TIME = time()

from math    import sqrt
from array   import array

from helperFunctions import *

from ROOT import *

import AtlasStyle

#
# define global variables
#

LATEX = True

FORMAT = 'pdf'

#ADDON = '  work in progress'

CANVAS = None

#
# define helper functions
#

def setDefaults(greyScale = False):
  AtlasStyle.setAtlasDefaults(not LATEX);
  gROOT.SetBatch(True)
  #gStyle.SetPalette(1)
  #gStyle.SetOptTitle(1)
  #gStyle.SetTitleAlign(33)
  global CANVAS
  CANVAS = TCanvas('c', 'global canvas', 800, 600)
  if greyScale:  CANVAS.SetGrayscale()


def plotCurve(chain, hist):
   f1  = TFile('diagnostics.root')
   h1  = f1.Get( 'jets_%s/%s' % (chain, hist) )
   h2  = f1.Get( 'jets_%s_jes/%s' % (chain, hist) )
   h3  = f1.Get( 'jets_%s_lcw/%s' % (chain, hist) )
   h4  = f1.Get( 'jets_%s_lcw_jes/%s' % (chain, hist) )
   h5  = f1.Get( 'jets_%s_lcw_nojcalib/%s' % (chain, hist) )
   h6  = f1.Get( 'jets_%s_nojcalib/%s' % (chain, hist) )


   bins = [ 10 * i for i in range(51) ]
   h1 = h1.Rebin( (len(bins) - 1), str(h1) + '_mod', array('d', bins))
   h2 = h2.Rebin( (len(bins) - 1), str(h2) + '_mod', array('d', bins))
   h3 = h3.Rebin( (len(bins) - 1), str(h3) + '_mod', array('d', bins))
   h4 = h4.Rebin( (len(bins) - 1), str(h4) + '_mod', array('d', bins))
   h5 = h5.Rebin( (len(bins) - 1), str(h5) + '_mod', array('d', bins))
   h6 = h6.Rebin( (len(bins) - 1), str(h6) + '_mod', array('d', bins))

   # set up canvas
   CANVAS.Clear()
   CANVAS.SetLogy(True)

   maxVal = max(h.GetBinContent(h.GetMaximumBin()) for h in [h1, h2, h3, h4, h5, h6])
   h1.SetMaximum(1.2 * maxVal)
   
   h1.SetLineColor(kBlack)
   h1.Draw()
   h2.SetLineColor(kBlue)
   h2.Draw('same')
   h3.SetLineColor(kRed)
   h3.Draw('same')
   h4.SetLineColor(kGreen)
   h4.Draw('same')
   h5.SetLineColor(kOrange)
   h5.Draw('same')
   h6.SetLineColor(kMagenta)
   h6.Draw('same')

   leg = TLegend(0.7, 0.65, 0.9, 0.92)
   leg.AddEntry(h1, 'jets_%s' % chain,  'L')
   leg.AddEntry(h2, 'jets_%s_jes' % chain, 'L')
   leg.AddEntry(h3, 'jets_%s_lcw' % chain, 'L')
   leg.AddEntry(h4, 'jets_%s_lcw_jes' % chain, 'L')
   leg.AddEntry(h5, 'jets_%s_lcw_nojcalib' % chain, 'L')
   leg.AddEntry(h6, 'jets_%s_nojcalib' % chain, 'L')
   leg.Draw('same')

   for fType in [ 'png' ]:#, 'pdf' ]: # 'gif', 'eps' ]:
     CANVAS.SaveAs( 'plots/%s_%s.%s' % (chain, hist, fType) )

   f1.Close()

#
# main function calls
#
chains = [ 'j85', 'j175' ]#, '4j85', '5j85', 'j85_280eta320', 'j175_320eta490', 'j260_320eta490' ]
setDefaults()

for chain in chains:
   plotCurve(chain, 'Et')
   plotCurve(chain, 'Pt')
   plotCurve(chain, 'Energy')
