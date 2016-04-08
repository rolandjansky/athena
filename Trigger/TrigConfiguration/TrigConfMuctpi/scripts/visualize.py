#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys
from ROOT import gROOT, gStyle, TH2F, TCanvas, TBox, TLegend, TArc
from TrigConfMuctpi.XMLReader import MioctGeometryXMLReader
from math import pi as PI

global box, c, h, leg

colorMap = [  1, 13,
              8,  3,
              5,801,
              9,  4,
              50,  2,
              11,  7,
              30, 51,
              6, 38 ]

drawOrder = [ 0,  8,
              7, 15,
              6, 14,
              5, 13,
              4, 12,
              3, 11,
              2, 10,
              1,  9 ]


def readXML(filename):
    geom = MioctGeometryXMLReader(filename)
    #print [str(m) for m in geom.getMIOCTs()]
    return geom


def drawGeometry(geometry):


    global box, c, h, leg

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"MuCTPi Geometry",1100,950)
    c.Draw()

    #h = TH2F("h","Muon Geometry",10,-2.6,2.6,10,-6.4,6.4)
    h = TH2F("h","Muon Geometry",10,-2.6,2.6,10,-0.15,6.4)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw();

    box = TBox()
    box.SetFillStyle(0)
    box.SetLineColor(3)

    leg = TLegend(0.7,0.1,0.9,0.4)
    leg.SetEntrySeparation(0.05)
    leg.SetNColumns(2)

    #for MIOCT in geometry.getMIOCTs():
    for colorIndex,MioctID in enumerate(drawOrder):
        MIOCT = geometry.getMIOCT(MioctID)
        firstBox = True
        color = colorMap[colorIndex % len(colorMap)]
        print "Using color ",color
        box.SetLineColor(color)
        box.SetLineWidth(1)
        
        for Sector in MIOCT.Sectors:
            for ROI in Sector.ROIs:
                c1_x = float(ROI["etamin"])
                c1_y = float(ROI["phimin"])
                c2_x = float(ROI["etamax"])
                c2_y = float(ROI["phimax"])
                #print "eta [%f - %f], phi [%f - %f]" % (c1_x,c2_x,c1_y,c2_y)
                b = box.DrawBox(c1_x,c1_y,c2_x,c2_y)
                if firstBox:
                    firstBox = False
                    leg.AddEntry(b,"Slot %s" % MIOCT["slot"],"l")


    leg.Draw()

    c.Update()
    c.SaveAs("ROILayout.pdf")



def drawTopoGeometry(geometry):

    global box, c, h, leg

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"MuCTPi to Topo Geometry",1100,950)
    c.Draw()

    h = TH2F("h","Muon Topo Geometry",10,-2.6,2.6,10,-0.15,6.4)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw();

    box = TBox()
    box.SetFillStyle(0)
    box.SetLineColor(3)

    circle = TArc()

    drawn=False

    #for MIOCT in geometry.getMIOCTs():
    for colorIndex,MioctID in enumerate(drawOrder):
        MIOCT = geometry.getMIOCT(MioctID)
        firstBox = True
        color = colorMap[colorIndex % len(colorMap)]
        box.SetLineColor(color)
        box.SetFillColor(color)

        circle.SetLineColor(color)
        circle.SetFillColor(color)
        
        fillStyle = 3004
        for cellIdx,TopoCell in enumerate(MIOCT.Decode.TopoCells):
            # corner 1
            c1_x = float(TopoCell["etamin"])
            c1_y = float(TopoCell["phimin"])
            # corner 2
            c2_x = float(TopoCell["etamax"])
            c2_y = float(TopoCell["phimax"])
            # center rounded
            c_x = float(TopoCell["ieta"])
            c_y = float(TopoCell["iphi"])

            #print "cell %i : eta [%f - %f], phi [%f - %f]" % (cellIdx, c1_x, c2_x, c1_y, c2_y)

            if fillStyle==3004:
                fillStyle = 3012
            else:
                fillStyle = 3004
            box.SetFillStyle(fillStyle)
            box.DrawBox(c1_x,c1_y,c2_x,c2_y)
            box.SetFillStyle(0)
            box.DrawBox(c1_x,c1_y,c2_x,c2_y)

            circle.DrawArc(c_x/10.,c_y/10.,0.02)

    c.Update()
    c.SaveAs("TopoLayout.pdf")
    

def main():

    filename = "TestMioctGeometry.xml"

    if len(sys.argv)>=2:
        filename = sys.argv[1]

    print "Using input %s" % filename

    geometry = readXML(filename)

    #drawGeometry(geometry)

    drawTopoGeometry(geometry)


if __name__=="__main__":
    main()
    #sys.exit(main())
