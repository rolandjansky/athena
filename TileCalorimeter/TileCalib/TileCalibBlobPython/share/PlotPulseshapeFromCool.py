#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
#
# PlotPulseshapeFromCool.py
# Nils Gollub <nils.gollub@cern.ch>, 2008-06-05

from TileCalibBlobPython import TileCalibTools
from TileCalibBlobObjs.Classes import *
import ROOT

pointInTime = (999999999,0)

#=== get a logger
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger
log = getLogger("ps_readDb")
    
#=== open the database
db = TileCalibTools.openDb('ORACLE', 'CONDBR2', 'READONLY', 'COOLOFL_TILE')

#=== specify folder and tag
folder = "/TILE/OFL02/PULSESHAPE/PHY"
folderTag = TileCalibUtils.getFullTag(folder, "RUN2-HLT-UPD1-00")
#folder = "/TILE/OFL01/PULSESHAPE/LAS"
#folderTag = TileCalibUtils.getFullTag(folder, "RUN2-HLT-UPD1-01")


#=== get a blob reader
print folder
print folderTag
blobReader = TileCalibTools.TileBlobReader(db,folder,folderTag)

#=== write out the comment
comment = blobReader.getComment(pointInTime)
log.info("Comment: \"%s\"" % comment)

#=== write out all values
cd = blobReader.getDrawer(0,0,pointInTime)

x = -200.;
xarr = []
yarrLG = []
darrLG = []
yarrHG = []
darrHG = []
while x<200.:
    x+=0.1
    yLG  = cd.getY( 0,0,x);
    dyLG = cd.getDY(0,0,x);
    yHG  = cd.getY( 0,1,x);
    dyHG = cd.getDY(0,1,x);
    #if y<-100.: continue 
    print("x=%5.1f  \tyLG=%5.4f\tdyLG=%5.4f  \tyHG=%5.4f\tdyHG=%5.4f" % (x,yLG,dyLG,yHG,dyHG) )
    xarr.append(x)
    yarrLG.append(yLG)
    darrLG.append(dyLG)
    yarrHG.append(yHG)
    darrHG.append(dyHG)

#=== close the database connection
db.closeDatabase()

np=len(xarr)
psLG = ROOT.TGraph(np)
dsLG = ROOT.TGraph(np)
psHG = ROOT.TGraph(np)
dsHG = ROOT.TGraph(np)
for i in xrange(np):
    psLG.SetPoint(i,xarr[i],yarrLG[i])
    dsLG.SetPoint(i,xarr[i],darrLG[i])
    psHG.SetPoint(i,xarr[i],yarrHG[i])
    dsHG.SetPoint(i,xarr[i],darrHG[i])
psLG.SetMarkerColor(ROOT.kBlue)
dsLG.SetMarkerColor(ROOT.kBlue)
psHG.SetMarkerColor(ROOT.kBlue)
dsHG.SetMarkerColor(ROOT.kBlue)
psLG.SetMinimum(-0.1)
psLG.SetMaximum(1.1)
psHG.SetMinimum(-0.1)
psHG.SetMaximum(1.1)
dsLG.SetMinimum(-0.03)
dsLG.SetMaximum(0.037)
dsHG.SetMinimum(-0.03)
dsHG.SetMaximum(0.037)
psLG.SetTitle("Low gain");
dsLG.SetTitle("Low gain derivative");
psHG.SetTitle("High gain");
dsHG.SetTitle("High gain derivative");

can = ROOT.TCanvas("can","Pulse Shapes")
can.Divide(2,2)
can.cd(1)
psLG.Draw("AP")
can.cd(2)
dsLG.Draw("AP")
can.cd(3)
psHG.Draw("AP")
can.cd(4)
dsHG.Draw("AP")

c = raw_input('please enter a character: ')
