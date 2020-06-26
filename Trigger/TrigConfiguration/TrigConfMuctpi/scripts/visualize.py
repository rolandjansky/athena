#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import argparse
from TrigConfMuctpi.XMLReader import MioctGeometryXMLReader
import math

global box, c, h, leg

colorMap = [  1, 13,
              8,  3,
              5,801,
              9,  4,
              50,  2,
              11,  7,
              30, 51,
              6, 38 ]

colorMap2 = [
    1, 8, 801, 9, 50, 4,
    2, 11, 7, 30, 51, 6, 38 ]
fillStyleMap2 = [ 3004, 3012, 3005, 3010 ]


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


def drawROIGeometry(geometry, is2016):

    outfn = "ROILayout%s.pdf" % ("2016" if is2016 else "2015")

    global box, c, h, leg

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"MuCTPi Geometry %s" % "2016" if is2016 else "2015",1400,950)
    c.Draw()

    #h = TH2F("h","Muon Geometry",10,-2.6,2.6,10,-6.4,6.4)
    h = TH2F("h","Muon Geometry %s" % "2016" if is2016 else "2015",10,-2.6,2.6,10,-0.15,6.4)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw()

    box = TBox()
    box.SetFillStyle(0)
    box.SetLineColor(3)
    box.SetLineColor(3)

    text = TLatex()
    text.SetTextSize(0.005)
    text.SetTextFont(42)
    text.SetTextAlign(22)
    
    secLabel = TLatex()
    secLabel.SetTextSize(0.008)
    secLabel.SetTextFont(42)
    secLabel.SetTextAlign(22)
    
    leg = TLegend(0.7,0.1,0.9,0.4)
    leg.SetEntrySeparation(0.05)
    leg.SetNColumns(2)

    #for MIOCT in geometry.getMIOCTs():
    for colorIndex,MioctID in enumerate(drawOrder):
        MIOCT = geometry.getMIOCT(MioctID)
        firstBox = True
        color = colorMap[colorIndex % len(colorMap)]
        #print "Using color ",color
        box.SetLineColor(color)
        box.SetLineWidth(1)
        
        for Sector in MIOCT.Sectors:
            ymin = 10
            ymax = -10
            for ROI in Sector.ROIs:
                c1_x = float(ROI["etamin"])
                c1_y = float(ROI["phimin"])
                c2_x = float(ROI["etamax"])
                c2_y = float(ROI["phimax"])
                ymin = min(ymin,c1_y)
                ymax = max(ymax,c2_y)
                #print "eta [%f - %f], phi [%f - %f]" % (c1_x,c2_x,c1_y,c2_y)
                b = box.DrawBox(c1_x,c1_y,c2_x,c2_y)
                text.DrawText( (c1_x + c2_x)/2, (c1_y + c2_y)/2 ,ROI["roiid"])
                if firstBox:
                    firstBox = False
                    leg.AddEntry(b,"Slot %s" % MIOCT["slot"],"l")
            if Sector["name"].startswith("B"):
                if int(Sector["name"][1:])<32:
                    xpos = -0.02
                    ypos = (ymin+ymax)/2 - 0.05
                else:
                    xpos = 0.02
                    ypos = (ymin+ymax)/2 + 0.03
                secLabel.DrawText(xpos,ypos,Sector["name"])

    leg.Draw()

    c.Update()
    c.SaveAs(outfn)



def drawTopoGeometry(geometry, is2016):

    outfn = "TopoLayout%s.pdf" % ("2016" if is2016 else "2015")

    global box, c, h, leg

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"MuCTPi to Topo Geometry",1400,950)
    c.Draw()

    h = TH2F("h","Muon Topo Geometry %s" % "2016" if is2016 else "2015",10,-2.6,2.6,10,-0.15,6.4)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw()

    box = TBox()
    box.SetFillStyle(0)
    box.SetLineColor(3)

    circle = TArc()

    for colorIndex,MioctID in enumerate(drawOrder):
        MIOCT = geometry.getMIOCT(MioctID)
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
    c.SaveAs(outfn)
    


ETACODE=0
PHICODE=1
IETA=2
IPHI=3

def drawTopoGeometryEtaPhi(geometry, colorBy, is2016):

    if colorBy<ETACODE or colorBy>IPHI:
        return

    global box, c, h, leg

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"MuCTPi to Topo Geometry",1400,950)
    c.Draw()

    h = TH2F("h","Muon Topo Geometry %i" % (2016 if is2016 else 2015),10,-2.6,2.6,10,-0.15,6.4)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw()

    box = TBox()
    box.SetFillStyle(0)
    box.SetLineColor(3)

    circle = TArc()

    if colorBy==IPHI:
        leg = TLegend(0.9,0.1,0.98,0.9)
    else:
        leg = TLegend(0.8,0.1,0.9,0.35)
    #leg.SetEntrySeparation(0.05)
    #leg.SetNColumns(2)


    codeInLegend = []
    for MioctID in drawOrder:
    #for MioctID in [3,4,5,6,7]:
        MIOCT = geometry.getMIOCT(MioctID)

        fillStyle = 3004
        for cellIdx,TopoCell in enumerate(MIOCT.Decode.TopoCells):

            if colorBy==ETACODE:
                code = int(TopoCell["etacode"],16)
            elif colorBy==PHICODE:
                code = int(TopoCell["phicode"],16)
            elif colorBy==IETA:
                code = abs(int(TopoCell["ieta"]))
            elif colorBy==IPHI:
                code = int(TopoCell["iphi"])
            else:
                raise RuntimeError("Don't know how to color the eta-phi map (%r)" % colorBy)
            color = colorMap2[code % len(colorMap2)]
            fillStyle = fillStyleMap2[code % 4]
            box.SetLineColor(color)
            box.SetFillColor(color)

            circle.SetLineColor(color)
            circle.SetFillColor(color)
        
            # corner 1
            c1_x = float(TopoCell["etamin"])
            c1_y = float(TopoCell["phimin"])
            # corner 2
            c2_x = float(TopoCell["etamax"])
            c2_y = float(TopoCell["phimax"])
            # center 
            c_x = float(TopoCell["ieta"])
            c_y = float(TopoCell["iphi"])

            #if code>63:
            #    continue
            #print "cell %i : eta [%f - %f], phi [%f - %f]" % (cellIdx, c1_x, c2_x, c1_y, c2_y)

            box.SetFillStyle(fillStyle)
            b = box.DrawBox(c1_x,c1_y,c2_x,c2_y)
            box.SetFillStyle(0)
            box.DrawBox(c1_x,c1_y,c2_x,c2_y)

            circle.DrawArc(c_x/10.,c_y/10.,0.02)


            if code not in codeInLegend:
                codeInLegend += [code]
                if colorBy==ETACODE:
                    leg.AddEntry(b,"etacode %i" % code,"lf")
                elif colorBy==PHICODE:
                    leg.AddEntry(b,"phicode %i" % code,"f")
                elif colorBy==IETA:
                    leg.AddEntry(b,"|ieta| %i" % code,"f")
                elif colorBy==IPHI:
                    leg.AddEntry(b,"iphi %i" % code,"f")

    leg.Draw()

    c.Update()
    if colorBy==ETACODE:
        ext = "EtaCode"
    elif colorBy==PHICODE:
        ext = "PhiCode"
    elif colorBy==IETA:
        ext = "Eta"
    elif colorBy==IPHI:
        ext = "Phi"


    c.SaveAs("TopoLayout%s%s.pdf" % ("2016" if is2016 else "2015", ext))
    


def drawCodeValues(geometry, is2016):

    global h,c

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"Topo encoding",1400,950)
    c.Draw()

    h = TH2F("h","Muon Topo encoding %i" % (2016 if is2016 else 2015),8,0,8,8,0,8)
    h.SetXTitle("#eta")
    h.SetYTitle("#phi")
    h.Draw()

    for MioctID in drawOrder:
    #for MioctID in [3,4,5,6,7]:
        MIOCT = geometry.getMIOCT(MioctID)
        for tc in MIOCT.Decode.TopoCells:
            h.Fill(int(tc['etacode'],16),int(tc['phicode'],16))

    h.Draw("colz")
    c.Update()
    c.SaveAs("TopoCodes%s.pdf" % "2016" if is2016 else "2015")
    

def drawIValues(geometry, is2016):

    global h,c

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"Topo encoding",1400,950)
    c.Draw()

    h = TH2F("h","Muon Topo encoding %i" % (2016 if is2016 else 2015),48,-24,24,64,0,64)
    h.SetXTitle("#eta_{index}")
    h.SetYTitle("#phi_{index}")
    h.Draw()

    for MioctID in drawOrder:
        MIOCT = geometry.getMIOCT(MioctID)
        for tc in MIOCT.Decode.TopoCells:
            h.Fill(int(tc['ieta'])+0.5,int(tc['iphi'])+0.5)

    h.Draw("colz")
    c.Update()
    c.SaveAs("TopoCellCenters%s.pdf" % "2016" if is2016 else "2015")
    


def drawRoiDeltaR(geometry, is2016):

    def getTopoCell(mioct, etacode, phicode):
        global mioctTCmap
        mioctTCmap = {}
        mioctid = mioct['id']
        if mioctid not in mioctTCmap:
            d = {}
            for tc in mioct.Decode.TopoCells:
                key = ( int(tc['etacode'],16), int(tc['phicode'],16))
                d[key] = tc
            mioctTCmap[mioctid] = d
        return mioctTCmap[mioctid][(etacode,phicode)]

    global h,c,hB,hEC,hFW

    gROOT.Reset()
    gStyle.SetOptStat(0)
    
    c = TCanvas('c',"Topo encoding",1400,950)
    c.Divide(2,2)
    c.Draw()

    hB = TH2F("hb","Delta R between ROI and TopoCell %i in Barrel" % (2016 if is2016 else 2015),64,0,64,32,0,32)
    hB.SetXTitle("SL")
    hB.SetYTitle("ROI ID")

    hEC = TH2F("hec","Delta R between ROI and TopoCell %i in Endcap" % (2016 if is2016 else 2015),96,0,96,148,0,148)
    hEC.SetXTitle("SL")
    hEC.SetYTitle("ROI ID")

    hFW = TH2F("hfw","Delta R between ROI and TopoCell %i in Forward" % (2016 if is2016 else 2015),48,0,48,64,0,64)
    hFW.SetXTitle("SL")
    hFW.SetYTitle("ROI ID")

    for MioctID in drawOrder:
        mioct = geometry.getMIOCT(MioctID)
        
        for sector in mioct.Sectors:
            for roi in sector.ROIs:
                tc = getTopoCell(mioct, int(roi['etacode'],16), int(roi['phicode'],16))
                deltaEta = float(roi['eta'])-float(tc['ieta'])/10.
                deltaPhi = float(roi['phi'])-float(tc['iphi'])/10.
                if deltaPhi>math.pi:
                    deltaPhi -= 2*math.pi # some topocells are made up from ROIs that are on both sides of 0
                deltaR = math.sqrt(deltaEta*deltaEta+deltaPhi*deltaPhi)
                sectorid = int(sector['name'].lstrip('ABCDEF'))
                roiid = int(roi['roiid'])
                if sector['name'].startswith('B'):
                    hB.Fill(sectorid,roiid,deltaR)
                elif sector['name'].startswith('E'):
                    if sector['name'].startswith('EA'):
                        sectorid += 48
                    hEC.Fill(sectorid,roiid,deltaR)
                elif sector['name'].startswith('F'):
                    if sector['name'].startswith('FA'):
                        sectorid += 24
                    hFW.Fill(sectorid,roiid,deltaR)


    c.cd(1)
    hB.Draw("colz")
    c.cd(2)
    hEC.Draw("colz")
    c.cd(3)
    hFW.Draw("colz")
    c.Update()
    c.SaveAs("ROIDeltaR%s.pdf" % "2016" if is2016 else "2015")



def main():

    parser = argparse.ArgumentParser( description=__doc__, 
                                      formatter_class = argparse.RawTextHelpFormatter)

    parser.add_argument( '-i', dest='filename', default="TrigConfMuctpi/TestMioctGeometry2016.xml", type=str,
                         help='name of muon geometry xml file')

    args = parser.parse_args()

    is2016 = '2016' in args.filename

    print("Using input %s" % args.filename)

    geometry = readXML(args.filename)

    from ROOT import gROOT, gStyle, TH2F, TCanvas, TBox, TLegend, TArc, TLatex
    global gROOT, gStyle, TH2F, TCanvas, TBox, TLegend, TArc, TLatex

    drawROIGeometry(geometry, is2016 = is2016)
    
    drawTopoGeometry(geometry, is2016 = is2016)
    
    drawTopoGeometryEtaPhi(geometry, colorBy=ETACODE, is2016 = is2016)
    
    drawTopoGeometryEtaPhi(geometry, colorBy=PHICODE, is2016 = is2016)
    
    drawTopoGeometryEtaPhi(geometry, colorBy=IPHI, is2016 = is2016)
    
    drawTopoGeometryEtaPhi(geometry, colorBy=IETA, is2016 = is2016)
    
    drawCodeValues(geometry, is2016 = is2016)
    
    drawIValues(geometry, is2016 = is2016)
    
    drawRoiDeltaR(geometry, is2016 = is2016)


    input("Press Enter to continue...")


if __name__=="__main__":
    main()
