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


def plotCurve(hist):
   f1  = TFile('diagnostics.root')
   h1  = f1.Get( 'jets_j400/%s' % hist )
   h2  = f1.Get( 'jets_j400_a4_tc_em_jes/%s' % hist )
   h3  = f1.Get( 'jets_j400_a4_tc_em_sub/%s' % hist )

   bins = [ 10000 * i for i in range(51) ]
   h1 = h1.Rebin( (len(bins) - 1), str(h1) + '_mod', array('d', bins))
   h2 = h2.Rebin( (len(bins) - 1), str(h2) + '_mod', array('d', bins))
   h3 = h3.Rebin( (len(bins) - 1), str(h3) + '_mod', array('d', bins))

   # set up canvas
   CANVAS.Clear()
   CANVAS.SetLogy(True)
   
   h1.SetLineColor(kBlack)
   h1.Draw()
   h2.SetLineColor(kBlue)
   h2.Draw('same')
   h3.SetLineColor(kRed)
   h3.Draw('same')

   leg = TLegend(0.7, 0.65, 0.9, 0.92)
   leg.AddEntry(h1, 'jets_j400',  'L')
   leg.AddEntry(h2, 'jets_j400_a4_tc_em_jes', 'L')
   leg.AddEntry(h3, 'jets_j400_a4_tc_em_sub', 'L')
   leg.Draw('same')

   for fType in [ 'png' ]:#, 'pdf' ]: # 'gif', 'eps' ]:
     CANVAS.SaveAs( 'plots/%s.%s' % (hist, fType) )

   f1.Close()

#
# main function calls
#

setDefaults()
plotCurve('Et')
plotCurve('Pt')
plotCurve('Energy')
