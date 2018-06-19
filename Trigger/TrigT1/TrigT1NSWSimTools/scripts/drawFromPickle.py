#!/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Draw the triggers that have been written to a txt file as a dictionary.
#
# See PadWithHits.h for a description of the 'simplified' pickle mechanism.
# See the 'pickle()' method of each (cpp) class for a list of the attributed being stored.
#
# davide.gerbaudo@gmail.com
# April 2013
# 
# Imported from https://svnweb.cern.ch/trac/atlasusr/browser/ataffard/UpgradeNSW/NSWAna/NSWNtuple/trunk/python/drawFromPickle.py
# By Alaettin Serhan Mete (amete@cern.ch) in April 2016

import math
import os
import sys
import ROOT as r
r.gROOT.SetBatch(1)
r.PyConfig.IgnoreCommandLineOptions = True
r.gStyle.SetPadTickX(1)
r.gStyle.SetPadTickY(1)


#_________________________________________________
# define some units, constants, and aliases
mm = 1.0
cm = 10.
cm2mm = 10.
mm2cm = 0.1
repv = r.Math.RhoEtaPhiVector
xyzv = r.Math.XYZVector
#_________________________________________________
sectorDphi = 2.0*math.pi/16.
sin, cos, tan, atan, exp = math.sin, math.cos, math.tan, math.atan, math.exp
def midSectorPhi(sec) : return (sec-1)*sectorDphi # sector N starts from 1
def average(lst) : return sum(lst)/len(lst)
def eta2theta(eta) : return 2.0*atan(exp(-eta))
def xyFromEtaPhiZ(eta, phi, z) :
    theta = eta2theta(eta)
    r = z*tan(theta)
    return r*cos(phi), r*sin(phi)
def tuple2vec(tpl) : return xyzv(tpl[0], tpl[1], tpl[2])
#_________________________________________________
class BaseClassFromDict(object) :
    """
    Use the kargs trick to set all the attributes except the ones
    that are my classes, and not basic types. See for example:
    http://stackoverflow.com/questions/1098549/proper-way-to-use-kwargs-in-python
    """
    def __init__(self, **kargs) :
        for k,v in kargs.iteritems() : setattr(self, k, v)
#_________________________________________________
class PadWithHits(BaseClassFromDict) :
    def __init__(self, **kargs) :
        super(PadWithHits, self).__init__(**kargs)
        self.cornerXyz    = [tuple2vec(t) for t in self.cornerXyz] # convert (x,y,z) tuple to vector
        self.avgChargeXyz = self.determineAvgChargeXyz()
        self.rotatedCornerXyz    = [self.rotateVecToSec5(v) for v in self.cornerXyz]
        self.rotatedAvgChargeXyz = self.rotateVecToSec5(self.avgChargeXyz)
        self.minX,   self.maxX   = self.determineXminmax()
        self.minY,   self.maxY   = self.determineYminmax()
        self.minZ,   self.maxZ   = self.determineZminmax()
        self.minH,   self.maxH   = self.determineHminmax()
        self.minPhi, self.maxPhi = self.determinePhiminmax()
        def color(layer) : return [r.kBlack, r.kRed, r.kBlue, r.kViolet, r.kGreen][layer]
        def width(isPivot) : return 2 if isPivot else 1
        self.lineColor = color(self.layer)
        self.lineWidth = width(self.isPivot())
    def setFont(self, font) : self.font = font
    def isPivot(self) : return self.multiplet in [0,2] # ASM 28/04/2016 : Check that this is correct
    def rotateVecToSec5(self, vec) :
        "Rotate a vector to sector5 coordinates (note: delta phi depends only on the sector)"
        deltaPhi = midSectorPhi(self.sector) - midSectorPhi(5)
        return repv(vec.rho(), vec.eta(), vec.phi()).SetPhi(vec.phi() - deltaPhi)
    def determineAvgChargeXyz(self) :
        z = average([p.z() for p in self.cornerXyz]) # although the corners should be at the same z...
        x, y = xyFromEtaPhiZ(self.avgEta, self.avgPhi, z)
        return xyzv(x, y, z)
    def determineXminmax(self):
        xs = [p.x() for p in self.cornerXyz]
        return min(xs), max(xs)
    def determineYminmax(self):
        ys = [p.y() for p in self.cornerXyz]
        return min(ys), max(ys)
    def determineZminmax(self):
        zs = [p.z() for p in self.cornerXyz]
        return min(zs), max(zs)
    def determinePhiminmax(self):
        phis = [p.phi() for p in self.cornerXyz]
        return min(phis), max(phis)
    def determineHminmax(self): return self.loHei, self.hiHei
    def draw(self, view='yx') :
        self.drawEdges(self.lineColor, self.lineWidth, view)
        self.drawCharge(self.lineColor, view)
    def drawEdges(self, color, width, view='yx') :
        lineColor = color
        lineWidth = width
        corners = self.rotatedCornerXyz if view=='hz' else self.cornerXyz
        segments, lines = [], []
         # see Pad::fillCornerCoords to remember which corner is where
        if   view=='yx'   : segments = [(corners[i], corners[j]) for i,j in [(0, 1), (1, 3), (2, 3), (0, 2)]]
        elif view=='hz'   : segments = [(corners[i], corners[j]) for i,j in [(0, 1)]]
        elif view=='zphi' : segments = [(corners[i], corners[j]) for i,j in [(0, 2)]]
        l = r.TLine
        if   view=='yx'   : lines = [l(p1.x(),   p1.y(), p2.x(),   p2.y()) for p1,p2 in segments]
        elif view=='hz'   : lines = [l(p1.z(),   p1.y(), p2.z(),   p2.y()) for p1,p2 in segments]
        elif view=='zphi' : lines = [l(p1.phi(), p1.z(), p2.phi(), p2.z()) for p1,p2 in segments]
        for l in lines :
            l.SetLineColor(lineColor)
            l.SetLineWidth(lineWidth)
            l.Draw()
        if hasattr(self, '_lines') : self._lines.append(lines)
        else : self._lines = lines
        self.printPadCoord(view, lines[0])
    def printPadCoord(self, view, line) :
        if view not in ['hz','zphi'] : return
        coord = {'hz':'ieta', 'zphi':'iphi'}[view]
        leftOrRight = self.layer%2
        angle = 90. if view=='hz' else 0.
        xs = sorted([line.GetX1(), line.GetX2()], reverse=leftOrRight)
        ys = sorted([line.GetY1(), line.GetY2()], reverse=leftOrRight)
        def outside(xx) : return xx[1] + 0.75*(xx[1]-xx[0])
        x = average(xs) if view=='hz'   else outside(xs)
        y = average(ys) if view=='zphi' else outside(ys)
        text = r.TLatex(x, y, "%d" % getattr(self, coord))
        text.SetTextAlign(22)
        text.SetTextAngle(angle)
        text.SetTextColor(line.GetLineColor())
        if hasattr(self, 'font') : text.SetTextFont(self.font)
        text.Draw()
        if hasattr(self, '_labels') : self._labels.append(text)
        else : self._labels = [text]
    def drawCharge(self, color, view='yx') :
        v = self.rotatedAvgChargeXyz if view=='hz' else self.avgChargeXyz
        x, y = 0., 0.
        if   view=='yx'   : x, y = v.x(),   v.y()
        elif view=='hz'   : x, y = v.z(),   v.y()
        elif view=='zphi' : x, y = v.phi(), v.z()
        marker = r.TMarker(x, y, r.kMultiply)
        marker.SetMarkerColor(color)
        marker.Draw()
        if hasattr(self, '_markers') : self._markers.append(marker)
        else : self._markers = [marker]
        def drawCloud() : print 'drawCloud not implemented yet'
        if len(self.hitIndices)>1 : drawCloud()
    
#_________________________________________________
class SingleWedgePadTrigger(BaseClassFromDict) :
    def __init__(self, pads=[], **kargs) :
        self.pads = [PadWithHits(**ka) for ka in pads]
        super(SingleWedgePadTrigger, self).__init__(**kargs)
    def setFont(self, font) :
        self.font = font
        for p in self.pads : p.setFont(font)
    def isPivot(self) : return self.pads[0].isPivot()
    def isConfirm(self) : return not self.isPivot()
    def sector(self) : return self.pads[0].sector
    def draw(self, view) :
        for p in self.pads : p.draw(view)
    def minMaxPadPar(self, parNames=['',]) :
        values = [getattr(p, pname) for p in self.pads for pname in parNames]
        return min(values), max(values)
    def minMaxPadX(self) : return self.minMaxPadPar(['minX','maxX'])
    def minMaxPadY(self) : return self.minMaxPadPar(['minY','maxY'])
    def minMaxPadZ(self) : return self.minMaxPadPar(['minZ','maxZ'])
    def minMaxPadH(self) : return self.minMaxPadPar(['minH','maxH'])
    def minMaxPadPhi(self) : return self.minMaxPadPar(['minPhi','maxPhi'])
    def minMaxPadVar(self, var) :
        if   var=='x'   : return self.minMaxPadX()
        elif var=='y'   : return self.minMaxPadY()
        elif var=='z'   : return self.minMaxPadZ()
        elif var=='h'   : return self.minMaxPadH()
        elif var=='phi' : return self.minMaxPadPhi()
    def drawLabelsLeft(self, x, y, dy) :
        text = r.TLatex( x, y, '')
        text.SetNDC()        
        if hasattr(self, 'font') : text.SetTextFont(self.font)        
        if hasattr(self, '_labels') : self._labels.append(text)
        else : self._labels = [text]
        text.DrawLatex(x, y, "Sector %d"%self.sector())
        y = y + dy
        text.DrawLatex(x, y, (('Pivot' if self.isPivot() else '') +
                              ('Confirm' if self.isConfirm() else '') +
                              ", %d/4"%len(self.pads) +
                              ' : '+self.pattern))
        y = y + dy
        ieta, iphi = self.halfPadIndices
        text.DrawLatex(x, y, "Half-pad indices : (%d, %d)"%(ieta, iphi))
        
        return y+dy

#_________________________________________________
class SectorTriggerCandidate(BaseClassFromDict) :
    def __init__(self, wedgeTrigs=[], **kargs) :
        self.wedgeTrigs = [SingleWedgePadTrigger(**ka) for ka in wedgeTrigs]
        super(SectorTriggerCandidate, self).__init__(**kargs)
        self.possibleViews = ['yx', 'hz', 'zphi']
        self.xVars = dict(zip(self.possibleViews, ['x', 'z', 'phi']))
        self.yVars = dict(zip(self.possibleViews, ['y', 'h', 'z']  ))
    def sector(self) : return self.wedgeTrigs[0].sector()
    def setFont(self, font) :
        self.font = font
        for w in self.wedgeTrigs : w.setFont(font)
    def adjustRanges(self, padMaster, view='yx'):
        possibleViews = self.possibleViews
        xVar, yVar = self.xVars[view], self.yVars[view]
        marginX = dict(zip(possibleViews, [20*cm, 20*cm, math.pi/32]))[view]
        marginY = dict(zip(possibleViews, [10*cm, 20*cm, 10*cm]     ))[view]
        xMin, xMax = self.minMaxPadVar(xVar)
        yMin, yMax = self.minMaxPadVar(yVar)
        xMin, xMax =  xMin-marginX, xMax+marginX
        yMin, yMax = yMin-marginY, yMax+marginY
        self.xyRanges = ((xMin, xMax), (yMin, yMax))
        padMaster.GetXaxis().SetRangeUser(xMin, xMax)
        padMaster.GetYaxis().SetRangeUser(yMin, yMax)
        self.setFont(padMaster.GetTitleFont())
    def draw(self, padMaster, view='yx', onlyPivot=False, onlyConfirm=False) :
        possibleViews = self.possibleViews
        assert view in possibleViews, "Invalid view %s"%view
        xVar = dict(zip(possibleViews, ['x', 'z', 'phi']))[view]
        yVar = dict(zip(possibleViews, ['y', 'h', 'z']  ))[view]
        gridX = dict(zip(possibleViews, [0, 0, 1]  ))[view]
        gridY = dict(zip(possibleViews, [0, 1, 0]  ))[view]
        padMaster.Draw()
        r.gPad.SetGridx(gridX)
        r.gPad.SetGridy(gridY)
        wedgeTrigs = [w for w in self.wedgeTrigs if (not (onlyConfirm or onlyPivot) or
                                                     (onlyConfirm and w.isConfirm()) or
                                                     (onlyPivot and w.isPivot()))]
        for w in wedgeTrigs : w.draw(view)
        self.drawLabels(view, padMaster, wedgeTrigs)
        if view=='yx' : self.drawSectorPostmark(self.xyRanges)
    def minMaxPadVar(self, var) :
        minMaxVals = [w.minMaxPadVar(var) for w in self.wedgeTrigs]
        mins, maxs = [m[0] for m in minMaxVals], [m[1] for m in minMaxVals]
        return min(mins), max(maxs)
    def drawLabels(self, view, padMaster, wedgeTriggers) :
        if view!='yx' : return
        xMin, xMax = padMaster.GetXaxis().GetXmin(), padMaster.GetXaxis().GetXmax()
        yMin, yMax = padMaster.GetYaxis().GetXmin(), padMaster.GetYaxis().GetXmax()
        x, y, dy = 0.125, 0.85, -0.05
        for w in wedgeTriggers : y = w.drawLabelsLeft(x, y, dy)
    def drawSectorPostmark(self, xyRanges=((-1.0, +1.0), (-1.0, +1.0))) :
        (xMin, xMax), (yMin, yMax) = xyRanges
        def topTwelwthRange(vMin, vMax) : return vMax-(vMax-vMin)/12.0, vMax 
        xMin, xMax = topTwelwthRange(xMin, xMax)
        yMin, yMax = topTwelwthRange(yMin, yMax)
        hdx, hdy = 0.5*(xMax-xMin), 0.5*(yMax-yMin)
        R = math.sqrt(hdx*hdx +hdy*hdy)
        midPhi = midSectorPhi(self.sector())
        minPhi, maxPhi = math.degrees(midPhi-0.5*sectorDphi), math.degrees(midPhi+0.5*sectorDphi)
        circle = r.TEllipse(0.5*(xMin+xMax), 0.5*(yMin+yMax), R, R)
        circle.SetLineColor(r.kBlack)
        circle.SetFillStyle(0)
        circle.Draw()
        slice = r.TCrown(0.5*(xMin+xMax), 0.5*(yMin+yMax), 0.2*R, 1.0*R, minPhi, maxPhi)
        slice.SetFillColor(r.kBlack)
        slice.SetFillStyle(1001)
        slice.Draw()
        if hasattr(self, '_markers') : self._markers.append(slice)
        else : self._markers = [slice]
        self._markers.append(circle)

#_________________________________________________

if __name__=='__main__' :
    # expect a list of candidates in the file
    filename = sys.argv[1]
    candidates = eval(open(filename).read())
    outputBaseName = os.path.splitext(filename)[0]
    print "found %d sector trigger candidates"%len(candidates)
    xRangeMin, xRangeMax = -500*cm, +500*cm
    yRangeMin, yRangeMax = -500*cm, +500*cm
    zRangeMin, zRangeMax = -800*cm, +800*cm
    hRangeMin, hRangeMax =    0*cm, yRangeMax
    phiRangeMin, phiRangeMax = -2*math.pi, +2*math.pi
    padMasterYx   = r.TH2F('padMasterYx',  ';x [mm];   y [mm]', 200, xRangeMin, xRangeMax,     200, yRangeMin, yRangeMax)
    padMasterHz   = r.TH2F('padMasterHz',  ';z [mm];   h [mm]', 200, zRangeMin, zRangeMax,     200, hRangeMin, hRangeMax)
    padMasterZphi = r.TH2F('padMasterZphi',';phi [rad];z [mm]', 200, phiRangeMin, phiRangeMax, 200, zRangeMin, zRangeMax)
    can = r.TCanvas('can')
    def formatPadMaster(h) :
        h.SetStats(0)
        h.GetXaxis().CenterTitle()
        h.GetYaxis().CenterTitle()
    for h in [padMasterYx, padMasterHz, padMasterZphi] : formatPadMaster(h)
    for i,c in enumerate(candidates) :
        sCan = SectorTriggerCandidate(**c)
        can.Clear()
        can.Divide(2, 2)

        can.cd(1)
        view, padMaster ='zphi', padMasterZphi
        sCan.adjustRanges(padMaster, view)
        sCan.draw(padMaster, view)

        can.cd(2)
        view, padMaster = 'hz', padMasterHz
        sCan.adjustRanges(padMaster, view)
        sCan.draw(padMaster, view)

        view, padMaster = 'yx', padMasterYx
        sCan.adjustRanges(padMaster, view)
        can.cd(3)
        sCan.draw(padMaster, view, onlyPivot=True)
        can.cd(4)
        sCan.draw(padMaster, view, onlyConfirm=True)

        can.SaveAs("%s_%03d.png"%(outputBaseName, i))
