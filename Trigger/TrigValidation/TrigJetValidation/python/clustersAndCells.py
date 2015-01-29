#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from ROOT import *

def printClustersAndCells():
   fin = TFile('diagnostics.root')
   fclusters = open('clusters.txt', 'w')
   fcells = open('cells.txt', 'w')
   fclusters.write('chain name\t mean number of clusters \t mean Et \n')
   fcells.write('chain name\t mean number of cells \t mean energy \n')
   hists = [k.GetName() for k in fin.GetListOfKeys()]
   for hist in hists:
     if "clusters" in hist:
       hNclusters = fin.Get('%s/nClusters' % hist)
       hEt = fin.Get('%s/Et' % hist)
       fclusters.write(hist + '\t' + str(hNclusters.GetMean()) + '\t' + str(hEt.GetMean()) + '\n')
     if "cells" in hist:
       hNcells = fin.Get('%s/nCells' % hist)
       hEnergy = fin.Get('%s/Energy' % hist)
       fcells.write(hist + '\t' + str(hNcells.GetMean()) + '\t' + str(hEnergy.GetMean()) + '\n')

   fin.Close()
   fclusters.close()
   fcells.close()

printClustersAndCells()
