# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import os, sys, string
from math import sqrt
import ROOT
from array import array
from math import sqrt


def getXYInPolygon(s,nev0=1500):
  xset,yset,nset = getXYSetInPolygon(nev0)
  ss = s%(len(xset))
  return xset[ss], yset[ss], nset[ss]



def drawXYInPolygon(nev0=1500):
  nevtot = 0
  nevinwin = 0
  xset,yset,nset = getXYSetInPolygon(nev0)
  print "Number of points ", len(xset), len(yset)
  #histXY = ROOT.TH2F( 'clusE',   'clusE',   500, -500.0, 500.0, 500, -500., 500. )
  histXY = ROOT.TH2F( 'clusE',   'clusE',   2000, -800.0, 800.0, 2000, -800., 800. )
  histXY.SetStats(0)
  histXY.SetTitle("")
  for i in range(0,len(xset)):
    #print "nrun:",i+1,"    xCryo:",xset[i], "yTable:",yset[i]
    print 'nrun %3d   xCryo %5d   yTable %5d  nev %5d' % (i+1,xset[i], yset[i], nset[i])
    nevtot = nevtot + nset[i]
    dist = sqrt(xset[i]*xset[i] + (70-yset[i])*(70-yset[i]))
    if dist < 300:
      nevinwin = nevinwin + nset[i]

    histXY.Fill(xset[i], yset[i])
  c0 = ROOT.TCanvas( 'c0', 'c0', 800, 800 )
  c0.SetFillColor(10)
  c0.SetFillStyle(0)
  c0.cd()
  ROOT.gPad.SetGrid();
  ROOT.gPad.SetLeftMargin(0.12);
  ROOT.gPad.SetRightMargin(0.12);
  ROOT.gPad.SetTopMargin(0.12);
  ROOT.gPad.SetBottomMargin(0.12);
  tc = ROOT.TEllipse(0,70,300);
  tc.SetFillStyle(0)
  tc.SetLineWidth(2)
  histXY.SetMarkerStyle(20)
  histXY.SetLineColor(2)
  histXY.SetMarkerColor(2)
  histXY.Draw()
  tc.Draw()
  #poly_np = 9
  #poly_x = array('d',[0,   300,  150,  250,    0, -250, -150, -300, 0])
  #poly_y = array('d',[320, 220, -80, -130, -280, -130, -80,  220, 320])
  poly_np = 8
  poly_x = array('d',[0,   500,  700,  60,   -60, -700, -500, 0])
  poly_y = array('d',[700, 600,  400, -400, -400,  400,  600, 700])
  myPoly = ROOT.TPolyLine(poly_np, poly_x, poly_y)
  myPoly.SetLineColor(4)
  myPoly.SetLineWidth(2)
  myPoly.Draw()

  #for i in range(0,500):
    #x,y = getXYInPolygon(i)
    #print "nrun:",i+1,"    xCryo:",x, "yTable:",y

  print "nevtot:",nevtot, "in win:",nevinwin
  return c0, histXY, tc, myPoly


#def getXYInPolygon(s):


###############################################################################
# Function returns two arrays with xCryo, yTable positions inside user defined shape (polygon)
# Points are placed in a rectangular grid with center in x0,y0 and grid step dx,dy
###############################################################################
def getXYSetInPolygon(nev0=1500):
  # this is our polygon drawn on xCryo, yTable plane in which we are going to place our cryostat and table
  #poly_np = 9
  #poly_x = array('i',[0,   300,  150,  250,    0, -250, -150, -300, 0])
  #poly_y = array('i',[320, 220, -80, -130, -280, -130, -80,  220, 320])

  poly_np = 8
  poly_x = array('i',[0,   500,  700,  60,   -60, -700, -500, 0])
  poly_y = array('i',[700, 600,  400, -400, -400,  400,  600, 700])

  # this is starting point of grid
  x0 = 0
  y0 = 0
  # this is grid step
  dx = 25
  dy = 25
  # this is number of events to simulate in points Y=-400 and Y=+400
  #n_Ytop = 2000
  #n_Ybot = 4000
  n_Ytop = nev0
  y_Ytop = 700
  n_Ybot = nev0*2
  y_Ybot = -400
  print "Initial number of events:", nev0, "y_Ytop:",y_Ytop, "n_Ytop:",n_Ytop, "y_Ybot:",y_Ybot, "n_Ybot:",n_Ybot
  # - - - - - -
  # - 2 3 4 - -
  # - 1 + 5 - -
  # - 0 7 6 - -
  # - - - - - -
  #
  xset = []
  yset = []
  nset = []
  xset.append(x0)
  yset.append(y0)
  #nset.append((n_Ytop+(y0-400)*(n_Ytop-n_Ybot)/800)/100*100);
  nset.append( (n_Ybot+(y0-y_Ybot)*(n_Ytop-n_Ybot)/(y_Ytop-y_Ybot))/10*10 );
  for iRound in range(1,1000):
    boxSide = 1+iRound*2
    nPoints = (boxSide-1)*4
    #print "----------------------------------------"
    #print "iRound:", iRound, " boxSide ", boxSide, "nPoints ", nPoints
    nGoodPoints = 0
    for iPoint in range(0, nPoints):
      boxSideNum = iPoint/(boxSide-1)
      boxSideStep = iPoint%(boxSide-1)
      if boxSideNum == 0:
        iX = x0 - iRound*dx
        iY = y0 - iRound*dy + dy*boxSideStep
      if boxSideNum == 1:
        iX = x0 - iRound*dx + dx*boxSideStep
        iY = y0 + iRound*dy
      if boxSideNum == 2:
        iX = x0 + iRound*dx
        iY = y0 + iRound*dy - dy*boxSideStep
      if boxSideNum == 3:
        iX = x0 + iRound*dx - dx*boxSideStep
        iY = y0 - iRound*dy
      mt = ROOT.TMath
      result = mt.IsInside(iX, iY, poly_np, poly_x, poly_y)
      #print "iPoint ", iPoint," from nPoints ",nPoints, " boxSideNum:",boxSideNum, " mod:", boxSideStep, " iX:", iX, " iY:", iY, result
      if result == 1:
        nGoodPoints += 1
        xset.append(iX)
        yset.append(iY)
        nset.append( (n_Ybot+(iY-y_Ybot)*(n_Ytop-n_Ybot)/(y_Ytop-y_Ybot))/10*10 );
    #print "nGoodPoints:", nGoodPoints, " nPoints:", nPoints
    if nGoodPoints == 0:
      break;

  return xset, yset, nset


#def getXYInRectangle(s):
  #xmin=-240
  #xmax=240
  #ymin=-170
  #ymax=181
  #dx=100
  #dy=100
  #nx=(xmax-xmin)/dx + 1
  #ny=(ymax-ymin)/dy + 1
  #ss = s%(nx*ny)
  #iy=ss/nx
  #ix=ss%nx
  ##print seed,ix,iy
  #x=xmin+ix*dx
  #y=ymin+iy*dy
  ##print "s:",s,"nx:", nx, "ny:",ny, "nx*ny:",nx*ny,"ix:",ix,"iy:",iy,"x:",x,"y:",y, "ss",ss
  #return x,y


#def getXYInCircle(s):
  ## steps
  #dx=50
  #dy=50
  ## basic rectangle
  #xmin=-400
  #xmax=400
  #ymin=-330.
  #ymax=470.
  ## circle size
  #x0 = 0
  #y0 = 70
  #r0 = 350

  #nx=(xmax-xmin)/dx + 1
  #ny=(ymax-ymin)/dy + 1
  #ntot = nx*ny
  #xset = []
  #yset = []
  #for ix in range(0, int(nx)):
    #for iy in range(0,int(ny)):
      #x=xmin+ix*dx
      #y=ymin+iy*dy
      #dist = sqrt((x-x0)*(x-x0)+(y-y0)*(y-y0))
      ##print "ix, iy",ix, iy, "x,y", x,y, "dist", dist
      #if dist <= r0:
        #xset.append(x)
        #yset.append(y)
  #ss = s%(len(xset))
  ##print "s:",s+1,"ntot:",ntot,"len_xyset;",len(xset), "ss:",ss, "xy:",xset[ss], yset[ss]
  #print "nrun:",s+1,"    xCryo:",xset[ss], "yTable:",yset[ss]
  #return xset[ss], yset[ss]

