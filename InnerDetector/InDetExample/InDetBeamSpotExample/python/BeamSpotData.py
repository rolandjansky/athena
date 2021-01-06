#!/usr/bin/env python

# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

"""
Tools for handling beam spot data in ntuples or in COOL.
For functionality requiring COOL access, you'll probably need to use
Frontier outside of CERN. For example:

setenv FRONTIER_SERVER "(serverurl=http://squid-frontier.usatlas.bnl.gov:23128/frontieratbnl)"
"""
__author__  = 'Juerg Beringer'
__version__ = '$Id: BeamSpotData.py 759522 2016-07-04 12:47:58Z amorley $'

import time
import copy
from array import array
from math import sqrt, atan
import csv

import numpy   # WARNING: must be last import, so numpy can override sqrt etc

import ROOT

import atexit

#
# Definition of generic beam spot ntuple variable attributes
# For definition of LaTeX symbols see beamspot-defs.tex.
#
varDefsGen = {
    'run':   {},
    'fill':  {},
    'bcid':  {},
    'nEvents':   {},
    'nValid':    {'altfmt': '%i', 'latexheader': r'$n_{\mathrm{vtx}}$'},
    'nVtxAll':   {},
    'nVtxPrim':  {},
    'status':    {'fmt': '%4i', 'altfmt': '%4i','latexheader' : 'Status' },
    'posX':      {'units': 'mm', 'fmt': '%10.4f', 'latexheader': '$x$ [mm]',
                  'altfmt': '%10.3f', 'altlatexheader': '\\lumposx [mm]',
                  'title': 'Beam Spot Position x', 'atit': 'Luminous centroid x [mm]', 'min': -2, 'max': 6},
    'posY':      {'units': 'mm', 'fmt': '%10.4f', 'latexheader': '$y$ [mm]',
                  'altfmt': '%10.3f', 'altlatexheader': '\\lumposy [mm]',
                  'title': 'Beam Spot Position y', 'atit': 'Luminous centroid y [mm]', 'min': -2, 'max': 6},
    'posZ':      {'units': 'mm', 'fmt': '%10.2f', 'latexheader': '$z$ [mm]',
                  'altfmt': '%10.1f', 'altlatexheader': '\\lumposz [mm]',
                  'title': 'Beam Spot Position z', 'atit': 'Luminous centroid z [mm]', 'min': -100, 'max': 100},
    'sigmaX':    {'units': 'mm', 'fmt': '%10.4f', 'latexheader': r'$\sigma_x$ [mm]',
                  'altfmt': '%10.0f', 'altfactor': 1000., 'altlatexheader': r'\\lumsigx [$\mu$m]',
                  'title': 'Beam Spot Size #sigma_{x}', 'atit': 'Luminous size #sigma_{x} [mm]', 'min': 0, 'max': 0.6},
    'sigmaY':    {'units': 'mm', 'fmt': '%10.4f', 'latexheader': r'$\sigma_y$ [mm]',
                  'altfmt': '%10.0f','altfactor': 1000., 'altlatexheader': r'\\lumsigy [$\mu$m]',
                  'title': 'Beam Spot Size #sigma_{y}', 'atit': 'Luminous size #sigma_{y} [mm]', 'min': 0, 'max': 0.6},
    'sigmaZ':    {'units': 'mm', 'fmt': '%10.2f', 'latexheader': r'$\sigma_z$ [mm]',
                  'altfmt': '%10.0f', 'altlatexheader': '\\lumsigz [mm]',
                  'title': 'Beam Spot Size #sigma_{z}', 'atit': 'Luminous size #sigma_{z} [mm]', 'min': 0, 'max': 150},
    'tiltX':     {'units': 'rad', 'fmt': '%10.6f', 'latexheader': 'tilt$_{xz}$ [rad]',
                  'altfmt': '%10.0f', 'altfactor': 1.E6, 'altlatexheader': '\\lumtiltx [$\\mu$rad]',
                  'title': 'Beam Spot Tilt x-z', 'atit': 'Luminous region tilt in x-z [mrad]', 'arescale': 1000., 'min': -2., 'max': 4.},
    'tiltY':     {'units': 'rad', 'fmt': '%10.6f', 'latexheader': 'tilt$_{yz}$ [rad]',
                  'altfmt': '%10.0f', 'altfactor': 1.E6, 'altlatexheader': '\\lumtilty [$\\mu$rad]',
                  'title': 'Beam Spot Tilt y-z', 'atit': 'Luminous region tilt in y-z [mrad]', 'arescale': 1000., 'min': -2., 'max': 4.},
    'posXErr':   {'units': 'mm', 'fmt': '%10.4f',
                  'title': 'Uncertainty on Beam Spot Position', 'atit': 'Uncertainty on luminous centroid x [mm]', 'min': 0, 'max': .005},
    'posYErr':   {'units': 'mm', 'fmt': '%10.4f',
                  'title': 'Uncertainty on Beam Spot Position', 'atit': 'Uncertainty on luminous centroid y [mm]', 'min': 0, 'max': .005},
    'posZErr':   {'units': 'mm', 'fmt': '%10.2f',
                  'title': 'Uncertainty on Beam Spot Position', 'atit': 'Uncertainty on luminous centroid z [mm]', 'min': 0, 'max': 5},
    'sigmaXErr': {'units': 'mm', 'fmt': '%10.3f',
                  'title': 'Uncertainty on Beam Spot Size', 'atit': 'Uncertainty on luminous size #sigma_{x} [mm]', 'min': 0, 'max': 0.005},
    'sigmaYErr': {'units': 'mm', 'fmt': '%10.3f',
                  'title': 'Uncertainty on Beam Spot Size', 'atit': 'Uncertainty on luminous size #sigma_{y} [mm]', 'min': 0, 'max': 0.005},
    'sigmaZErr': {'units': 'mm', 'fmt': '%10.2f',
                  'title': 'Uncertainty on Beam Spot Size', 'atit': 'Uncertainty on luminous size #sigma_{z} [mm]', 'min': 0, 'max': 5},
    'tiltXErr':  {'units': 'rad', 'fmt': '%10.6f',
                  'title': 'Uncertainty on Beam Spot Tilt', 'atit': 'Uncertainty on luminous region tilt in x-z [rad]', 'min': 0, 'max': .0001},
    'tiltYErr':  {'units': 'rad', 'fmt': '%10.6f',
                  'title': 'Uncertainty on Beam Spot Tilt', 'atit': 'Uncertainty on luminous region tilt in y-z [rad]', 'min': 0, 'max': .0001},
    'rhoXY':     {'fmt': '%10.3f', 'latexheader': '$\\rho_{xy}$',
                  'altfmt': '%10.2f', 'altfactor': 1., 'altlatexheader': '\\lumrhoxy',
                  'title': 'Beam Spot #rho_{xy}', 'atit': '#rho', 'min': -1., 'max': 1.5},
    'rhoXYErr':  {'fmt': '%10.3f',
                  'title': 'Uncertainty on Beam Spot #rho_{xy}', 'atit': 'Uncertainty on #rho', 'min': 0, 'max': .1},
    'covSxSy':   {'fmt': '%10.3f',
                  'title': 'Covariance of Beam Spot #sigma_{x}#sigma_{y}', 'atit': 'Covariance of #sigma_{x}#sigma_{y}', 'min': -.1, 'max': .1},
    'covSxRhoXY':{'fmt': '%10.3f',
                  'title': 'Covariance of Beam Spot #sigma_{x}#rho_{xy}', 'atit': 'Covariance of #sigma_{x}#rho_{xy}', 'min': -.1, 'max': .1},
    'covSyRhoXY':{'fmt': '%10.3f',
                  'title': 'Covariance of Beam Spot #sigma_{y}#rho_{xy}', 'atit': 'Covariance of #sigma_{y}#rho_{xy}', 'min': -.1, 'max': .1},
    'k':         {'fmt': '%10.3f', 'latexheader': 'k', 'altlatexheader': 'k',
                  'title': 'Error Scale Factor k', 'atit': 'k', 'min': 0.5, 'max': 2.0},
    'kErr':      {'fmt': '%10.3f',
                  'title': 'Uncertainty on Error Scale Factor k', 'atit': 'Uncertainty on k', 'min': 0, 'max': 0.2},
    'sigmaXY':   {'fmt': '%10.6f', 'latexheader': r'$\sigma_{xy}$',
                  'title': 'Beam Spot Size #sigma_{xy}', 'atit': '#sigma_{xy}', 'min': -0.0005, 'max': 0.0005},
}

# Version for making LaTeX table of beam spot results
varDefsTable = copy.deepcopy(varDefsGen)
varDefsTable['posX']['altfmt'] = '%10.4f'
varDefsTable['posY']['altfmt'] = '%10.4f'
varDefsTable['posZ']['altfmt'] = '%10.4f'
varDefsTable['sigmaX']['altfmt'] = '%10.1f'
varDefsTable['sigmaY']['altfmt'] = '%10.1f'
varDefsTable['sigmaZ']['altfmt'] = '%10.1f'
varDefsTable['tiltX']['altfmt'] = '%10.1f'
varDefsTable['tiltY']['altfmt'] = '%10.1f'
varDefsTable['rhoXY']['altfmt'] = '%10.3f'

# Version where default values are tailored for Run 1
varDefsRun1 = copy.deepcopy(varDefsGen)
varDefsRun1['posX']['min'] = -1.
varDefsRun1['posX']['max'] = +1.
varDefsRun1['posY']['min'] = 0.
varDefsRun1['posY']['max'] = 2.
varDefsRun1['posZ']['min'] = -60.
varDefsRun1['posZ']['max'] = +60.
varDefsRun1['sigmaX']['max'] = 0.1
varDefsRun1['sigmaY']['max'] = 0.1
varDefsRun1['sigmaZ']['max'] = 80.
varDefsRun1['tiltX']['min'] = -0.4
varDefsRun1['tiltX']['max'] = +0.4
varDefsRun1['tiltY']['min'] = -0.4
varDefsRun1['tiltY']['max'] = +0.4

# Version where default values are tailored for 
varDefsRun1VtxPaper = copy.deepcopy(varDefsGen)
varDefsRun1VtxPaper['posX']['atit'] = 'x [mm]'
varDefsRun1VtxPaper['posY']['atit'] = 'y [mm]'
varDefsRun1VtxPaper['posZ']['atit'] = 'z [mm]'
varDefsRun1VtxPaper['sigmaX']['atit'] = '#sigma_{x} [mm]'
varDefsRun1VtxPaper['sigmaY']['atit'] = '#sigma_{y} [mm]'
varDefsRun1VtxPaper['sigmaZ']['atit'] = '#sigma_{z} [mm]'

# Version where default values are tailored for MC14 validation plots
varDefsMC14 = copy.deepcopy(varDefsGen)
varDefsMC14['posX']['min'] = -0.297
varDefsMC14['posX']['max'] = -0.287
varDefsMC14['posX']['ndivs'] = 502
varDefsMC14['posY']['min'] = 0.698
varDefsMC14['posY']['max'] = 0.708
varDefsMC14['posY']['ndivs'] = 502
varDefsMC14['posZ']['min'] = -14
varDefsMC14['posZ']['max'] = -4
varDefsMC14['posZ']['ndivs'] = 502
varDefsMC14['sigmaX']['min'] = 0.00
varDefsMC14['sigmaX']['max'] = 0.02
varDefsMC14['sigmaX']['ndivs'] = 502
varDefsMC14['sigmaY']['min'] = 0.00
varDefsMC14['sigmaY']['max'] = 0.02
varDefsMC14['sigmaY']['ndivs'] = 502
varDefsMC14['sigmaZ']['min'] = 40
varDefsMC14['sigmaZ']['max'] = 50
varDefsMC14['sigmaZ']['ndivs'] = 502
varDefsMC14['tiltX']['min'] = -0.1 #mrad
varDefsMC14['tiltX']['max'] = -0.0
varDefsMC14['tiltX']['ndivs'] = 502
varDefsMC14['tiltY']['min'] = -0.1 #mrad
varDefsMC14['tiltY']['max'] = -0.0
varDefsMC14['tiltY']['ndivs'] = 502
varDefsMC14['k']['min'] = 0.6
varDefsMC14['k']['max'] = 1.6
varDefsMC14['k']['ndivs'] = 510

# Version where default values are tailored for MC14 validation plots vs pileup profile plots
varDefsMC14Profile = copy.deepcopy(varDefsGen)
varDefsMC14Profile['posX']['min'] = -0.297
varDefsMC14Profile['posX']['max'] = -0.287
varDefsMC14Profile['posX']['ndivs'] = 502
varDefsMC14Profile['posY']['min'] = 0.698
varDefsMC14Profile['posY']['max'] = 0.708
varDefsMC14Profile['posY']['ndivs'] = 502
varDefsMC14Profile['posZ']['min'] = -14
varDefsMC14Profile['posZ']['max'] = -4
varDefsMC14Profile['posZ']['ndivs'] = 502
varDefsMC14Profile['sigmaX']['min'] = 0.013
varDefsMC14Profile['sigmaX']['max'] = 0.018
varDefsMC14Profile['sigmaX']['ndivs'] = 505
varDefsMC14Profile['sigmaY']['min'] = 0.013
varDefsMC14Profile['sigmaY']['max'] = 0.018
varDefsMC14Profile['sigmaY']['ndivs'] = 505
varDefsMC14Profile['sigmaZ']['min'] = 45
varDefsMC14Profile['sigmaZ']['max'] = 50
varDefsMC14Profile['sigmaZ']['ndivs'] = 505
varDefsMC14Profile['tiltX']['min'] = -0.1 #mrad
varDefsMC14Profile['tiltX']['max'] = -0.0
varDefsMC14Profile['tiltX']['ndivs'] = 502
varDefsMC14Profile['tiltY']['min'] = -0.1 #mrad
varDefsMC14Profile['tiltY']['max'] = -0.0
varDefsMC14Profile['tiltY']['ndivs'] = 502
varDefsMC14Profile['k']['min'] = 0.9
varDefsMC14Profile['k']['max'] = 1.2
varDefsMC14Profile['k']['ndivs'] = 503

# Version where default values are tailored for truth-subtracted validation plots
varDefsTruthCorr = copy.deepcopy(varDefsGen)
varDefsTruthCorr['posX']['atit'] = 'Luminous centroid x - generated value [mm]'
varDefsTruthCorr['posX']['min'] = -0.005
varDefsTruthCorr['posX']['max'] =  0.005
varDefsTruthCorr['posX']['ndivs'] = 502
varDefsTruthCorr['posY']['atit'] = 'Luminous centroid y - generated value [mm]'
varDefsTruthCorr['posY']['min'] = -0.005
varDefsTruthCorr['posY']['max'] =  0.005
varDefsTruthCorr['posY']['ndivs'] = 502
varDefsTruthCorr['posZ']['atit'] = 'Luminous centroid z - generated value [mm]'
varDefsTruthCorr['posZ']['min'] = -5.
varDefsTruthCorr['posZ']['max'] =  5.
varDefsTruthCorr['posZ']['ndivs'] = 502
varDefsTruthCorr['sigmaX']['atit'] = 'Luminous size #sigma_{x} - generated value [mm]'
varDefsTruthCorr['sigmaX']['min'] = -0.005
varDefsTruthCorr['sigmaX']['max'] =  0.005
varDefsTruthCorr['sigmaX']['ndivs'] = 502
varDefsTruthCorr['sigmaY']['atit'] = 'Luminous size #sigma_{y} - generated value [mm]'
varDefsTruthCorr['sigmaY']['min'] = -0.005
varDefsTruthCorr['sigmaY']['max'] =  0.005
varDefsTruthCorr['sigmaY']['ndivs'] = 502
varDefsTruthCorr['sigmaZ']['atit'] = 'Luminous size #sigma_{z} - generated value [mm]'
varDefsTruthCorr['sigmaZ']['min'] = -5.
varDefsTruthCorr['sigmaZ']['max'] =  5.
varDefsTruthCorr['sigmaZ']['ndivs'] = 502
varDefsTruthCorr['tiltX']['atit'] = 'Luminous region tilt in x-z - generated value [mrad]'
varDefsTruthCorr['tiltX']['min'] = -0.05
varDefsTruthCorr['tiltX']['max'] =  0.05
varDefsTruthCorr['tiltX']['ndivs'] = 502
varDefsTruthCorr['tiltY']['atit'] = 'Luminous region tilt in y-z - generated value [mrad]'
varDefsTruthCorr['tiltY']['min'] = -0.05
varDefsTruthCorr['tiltY']['max'] =  0.05
varDefsTruthCorr['tiltY']['ndivs'] = 502
varDefsTruthCorr['sigmaXY']['atit'] = '#sigma_{xy} - generated value'
varDefsTruthCorr['sigmaXY']['min'] = -0.05e-3
varDefsTruthCorr['sigmaXY']['max'] =  0.05e-3
varDefsTruthCorr['sigmaXY']['ndivs'] = 502


varDefs = varDefsGen # generic settings are default
def varDef(var,property,default='',useAlternate=False,override=None):
    if override:
        return override
    try:
        v = varDefs[var].get(property,default)
        if useAlternate:
            v = varDefs[var].get('alt'+property,v)
        return v
    except Exception:
        return default

def fmtVal(var,value,strip=False,useAlternate=False):
    fmt = varDef(var,'fmt','%s')
    if useAlternate:
        value *= varDef(var,'altfactor',1.)
        fmt = varDef(var,'altfmt',fmt)
    s = fmt % value
    if strip:
        return s.strip()
    else:
        return s



class BeamSpotValue:
    """Class to hold information about a single set of beam spot parameters.
       The beam spot parametrization follows the COOL convention (see
       https://twiki.cern.ch/twiki/bin/view/Atlas/CoolBeamSpotParameters)."""

    # Class variables
    coolQuery   = None
    propertyList = ['sigmaXY', 'sigmaXYErr','thetaXY','thetaXYErr', 'defects', 'fullCorrelations']
    pseudoLbDict = {} 
    defectData = None

    def __init__(self, fullCorrelations=False):
        self.fullCorrelations = fullCorrelations

        # Run info etc
        self.run = 0
        self.fill = 0
        self.bcid = 0
        self.lbStart = 0
        self.lbEnd = 0      # exclusive, like COOL ranges
        self.timeStart = 0  # Beware - must be int (seconds since epoch), float not precise enough!
        self.timeEnd = 0    # Beware - must be int (seconds since epoch), float not precise enough!
        self.nEvents = 0    # Number of vertices input to fit
        self.nValid = 0     # Number of vertices used by fit
        self.nVtxAll = 0    # Total number of vertices (no cuts)
        self.nVtxPrim = 0   # Total number of primary vertices (no cuts)

        # Parameters as stored in COOL
        
        self.fitID=0
        self.status = 0
        self.posX = 0.
        self.posY = 0.
        self.posZ = 0.
        self.sigmaX = 0.
        self.sigmaY = 0.
        self.sigmaZ = 0.
        self.tiltX = 0.
        self.tiltY = 0.

        # Errors for the parameters above
        self.posXErr = 0.
        self.posYErr = 0.
        self.posZErr = 0.
        self.sigmaXErr = 0.
        self.sigmaYErr = 0.
        self.sigmaZErr = 0.
        self.tiltXErr = 0.
        self.tiltYErr = 0.

        # Offdiagonal covariance matrix parameters
        self.covSxSy = 0.
        self.covSxRhoXY = 0.
        self.covSyRhoXY = 0.

        if self.fullCorrelations :
            self.covXY = 0.
            self.covXZ = 0.
            self.covXTiltX = 0.
            self.covXTiltY = 0.
            self.covXSx = 0.
            self.covXSy = 0.
            self.covXSz = 0.
            self.covXRhoXY = 0.
            self.covXk = 0.
            
            self.covYZ = 0.
            self.covYTiltX = 0.
            self.covYTiltY = 0.
            self.covYSx = 0.
            self.covYSy = 0.
            self.covYSz = 0.
            self.covYRhoXY = 0.
            self.covYk = 0.

            self.covZTiltX = 0.
            self.covZTiltY = 0.
            self.covZSx = 0.
            self.covZSy = 0.
            self.covZSz = 0.
            self.covZRhoXY = 0.
            self.covZk = 0.

            self.covTiltXTiltY = 0.
            self.covTiltXSx = 0.
            self.covTiltXSy = 0.
            self.covTiltXSz = 0.
            self.covTiltXRhoXY = 0.
            self.covTiltXk = 0.

            self.covTiltYSx = 0.
            self.covTiltYSy = 0.
            self.covTiltYSz = 0.
            self.covTiltYRhoXY = 0.
            self.covTiltYk = 0.

            self.covSxSz = 0.
            self.covSxk = 0.

            self.covSySz = 0.
            self.covSyk = 0.
            
            self.covSzRhoXY = 0.
            self.covSzk = 0.

            self.covRhoXYk = 0.

        
        # Additional fit parameters
        self.rhoXY = 0.
        self.rhoXYErr = 0.
        self.k = 1.
        self.kErr = 0.

        # Data quality info
        self.defectWord = 0

        # Pileup
        self.pileup = 0

        # Scan info
        self.separation = 0.
        self.acquisitionFlag = 0.

    @property
    def sigmaXY(self):
        return self.rhoXY*self.sigmaX*self.sigmaY

    @property
    def sigmaXYErr(self):
        # NOTE: error calculation below neglects covariance matrix
        ss  = self.rhoXYErr*self.rhoXYErr*self.sigmaX*self.sigmaX*self.sigmaY*self.sigmaY
        ss += self.rhoXY*self.rhoXY*self.sigmaXErr*self.sigmaXErr*self.sigmaY*self.sigmaY
        ss += self.rhoXY*self.rhoXY*self.sigmaX*self.sigmaX*self.sigmaYErr*self.sigmaYErr
        return sqrt(ss)

    @property
    def thetaXY(self):
        try:
            txy = .5*atan((2*self.sigmaX*self.sigmaY*self.rhoXY)/(self.sigmaY**2-self.sigmaX**2))
        except Exception:
            txy = 0
        return txy

    @property
    def thetaXYErr(self):
        try:
            tpx = (self.rhoXY*self.sigmaY*(self.sigmaX**2+self.sigmaY**2))/(self.sigmaX**4+2*(2*self.rhoXY**2-1)*self.sigmaX**2*self.sigmaY**2+self.sigmaY**4)
            tpy = -1*(self.rhoXY*self.sigmaX*(self.sigmaX**2+self.sigmaY**2))/(self.sigmaX**4+2*(2*self.rhoXY**2-1)*self.sigmaX**2*self.sigmaY**2+self.sigmaY**4)
            tpr = (self.sigmaX*self.sigmaY**3-self.sigmaY*self.sigmaX**3)/(self.sigmaX**4+2*(2*self.rhoXY**2-1)*self.sigmaX**2*self.sigmaY**2+self.sigmaY**4)
            txye = sqrt(tpx*tpx*self.sigmaXErr**2 + tpy*tpy*self.sigmaYErr**2 + tpr*tpr*self.rhoXYErr**2 + 2*(tpx*tpy*self.covSxSy + tpx*tpr*self.covSxRhoXY + tpy*tpr*self.covSyRhoXY))
        except Exception:
            txye = 0
        return txye

    @property
    def defects(self):

        # Will raise an error if unknown defect encountered
        from InDetBeamSpotExample.DQUtilities import IDBSDefectEncoding
        return IDBSDefectEncoding.intToDefectList(self.defectWord)

    def fillDataFromCOOL(self):
        if not BeamSpotValue.coolQuery:
            from InDetBeamSpotExample.COOLUtils import COOLQuery
            BeamSpotValue.coolQuery = COOLQuery()
        try:
            self.timeStart = BeamSpotValue.coolQuery.lbTime(self.run,self.lbStart)[0]
        except Exception:
            pass
        try:
            self.timeEnd = BeamSpotValue.coolQuery.lbTime(self.run,self.lbEnd-1)[1]
        except Exception:
            pass
        try:
            self.fill = BeamSpotValue.coolQuery.getLHCInfo(self.timeStart).get('FillNumber',0)
        except Exception:
            pass
    
    def fillDataFromPseudoLb(self, pseudoLbFile, timeUnit = 1.):
        if not BeamSpotValue.pseudoLbDict:
            f = open(pseudoLbFile)
            for pLb, line in enumerate(f):
                tokens = line.split()
                if len(tokens) < 5: tokens.append(0.0)
                point, start, end, sep, acq = tokens
                print ("point %s %s %s %s" % (point,start,sep,acq))
                BeamSpotValue.pseudoLbDict[int(point)] = (int(int(start)*timeUnit), int(int(end)*timeUnit), float(sep), float(acq))

        if self.lbStart not in self.pseudoLbDict:
            print ("Missing %s in pseudoLbDict" % self.lbStart)
            return

        self.timeStart = self.pseudoLbDict[self.lbStart][0]
        self.timeEnd = self.pseudoLbDict[self.lbStart][1]
        self.separation = self.pseudoLbDict[self.lbStart][2]
        self.acquisitionFlag = self.pseudoLbDict[self.lbStart][3]

        #print (self.lbStart, self.timeStart, self.timeEnd, time.strftime('%a %b %d %X %Z %Y',time.localtime(self.timeStart)))

        if not BeamSpotValue.coolQuery:
            from InDetBeamSpotExample.COOLUtils import COOLQuery
            BeamSpotValue.coolQuery = COOLQuery()

        try:
            self.fill = BeamSpotValue.coolQuery.getLHCInfo(self.timeStart).get('FillNumber',0)
        except Exception:
            pass
        
        return
        
    def fillDataFromDQ(self):
        """
        Fill DQ defects assuming that any defect is valid for the full fit range
        """
        from InDetBeamSpotExample.DQUtilities import IDBSDefectEncoding

        if not BeamSpotValue.defectData:
            from InDetBeamSpotExample.DQUtilities import IDBSDefectData
            BeamSpotValue.defectData = IDBSDefectData()

        defects = BeamSpotValue.defectData.defectsRange(self.run, self.lbStart, self.lbEnd)
        self.defectWord = IDBSDefectEncoding.defectListToInt(defects)
        return
    

    def __str__(self):
        s = self.summary()+'\n'
        if self.timeStart or self.timeEnd:
            s += '%s  -  %s\n' % (time.strftime('%a %b %d %X %Z %Y',time.localtime(self.timeStart)),
                                  time.strftime('%a %b %d %X %Z %Y',time.localtime(self.timeEnd)))
        for v in ['posX','posY','posZ',
                  'sigmaX','sigmaY','sigmaZ',
                  'tiltX','tiltY','sigmaXY','k']:
            s += '... %6s: %s +- %s %s\n' % (v,
                                             fmtVal(v,getattr(self,v)),
                                             fmtVal(v,getattr(self,v+'Err')),
                                             varDef(v,'units'))
        return s

    def summary(self):
        """Get one-line summary info."""
        return '[%i, %i - %i), fill %i, BCID %i: %i events, %i selected, status %i' % (self.run,self.lbStart,self.lbEnd,
                                                                                       self.fill,self.bcid,
                                                                                       self.nEvents,self.nValid,self.status)

    def dump(self,verbose=False):
        """Standard printout of beam spot parameters."""
        if verbose:
            print (self)
        else:
            print (self.summary())

    def varList(self):
        """Get list of variable names in BeamSpotValue object."""
        l = []
        for name in dir(self):
            # can we test for type(...) == property?
            if name in BeamSpotValue.propertyList:
                continue
            o = getattr(self,name)
            if isinstance(o,int) or isinstance(o,float):
                l.append(name)
        return l

    def getROOTType(self,var):
        """Get ROOT type of variable (either /I or /F)."""
        o = getattr(self,var)
        if isinstance(o,int):
            return '/I'
        else:
            return '/F'

    def getROOTStruct(self):
        """Return a string with a C struct describing all data members of the instance.
           Intended for creating ROOT tree buffer objects via ROOT.gROOT.ProcessLine()."""
        s = 'struct BeamSpotNtBuf {'
        for name in self.varList():
            o = getattr(self,name)
            if isinstance(o,int):
                s += ' Int_t  %s;' % name
            if isinstance(o,float):
                s += ' Float_t  %s;' % name
        s += '};'
        return s

    def __cmp__(self, other):
      if self.run != other.run:
        return self.run.__cmp__(other.run) 
      if self.bcid != other.bcid:
        return self.bcid.__cmp__(other.bcid) 
      
      return self.lbStart.__cmp__(other.lbStart) 


class BeamSpotAverage:
    """A utility class for averaging beam spot data."""

    def __init__(self,varList=None,weightedAverage=True):
        if varList:
            self.varList = varList
        else:
            self.varList = ['posX','posY','posZ','sigmaX','sigmaY','sigmaZ','tiltX','tiltY','rhoXY','k']
        self.weightedAverage = weightedAverage
        self.sumw = numpy.zeros(len(self.varList))
        self.sumwx = numpy.zeros(len(self.varList))
        self.sumwxx = numpy.zeros(len(self.varList))
        self.sumwwee = numpy.zeros(len(self.varList))
        # Unweighted quantities always needed for RMS
        self.sum = numpy.zeros(len(self.varList))
        self.sumx = numpy.zeros(len(self.varList))
        self.sumxx = numpy.zeros(len(self.varList))
        self.nWarnings = 0
        self.lumiData = None   # luminosity data in ubarn, indexed by [run][LB]

    def readLumiData(self,lumiCalcNtupleName):
        """Read iLumiCalc.exe ntuple with luminosity data, store data in self.lumiData dict."""
        lumiFile = ROOT.TFile(lumiCalcNtupleName)
        lumiNt = lumiFile.Get('LumiMetaData')
        print ('Reading',lumiNt.GetEntries(), 'entries from luminosity ntuple',lumiCalcNtupleName)
        self.lumiData = {}
        for j in range(lumiNt.GetEntries()):
            lumiNt.GetEntry(j)
            run = lumiNt.RunNbr
            lb = lumiNt.LBStart
            lumi = lumiNt.IntLumi
            try:
                self.lumiData[run][lb] = lumi
            except Exception:
                self.lumiData[run] = {}
                self.lumiData[run][lb] = lumi
        lumiSum = 0.
        for (run,lbdict) in self.lumiData.items():
            for (lb,lumi) in lbdict.items():
                lumiSum += lumi
        print ('... total luminosity = %6.1f / pb' % (lumiSum/1.E6))
        return self.lumiData

    def add(self,b):
        """Add elements of BeamSpotValue b to average."""
        if self.lumiData is not None:
            lumi = 0
            if b.lbEnd <= b.lbStart:
                print ('ERROR: Illegal luminosity block range: [%i,%i]' % (b.lbStart,b.lbEnd))
                self.nWarnings += 1
            for lb in range(b.lbStart,b.lbEnd):
                try:
                    lumi += self.lumiData[b.run][lb]
                except Exception:
                    print ('ERROR: missing luminosity information for run %i LB %i (in [%i,%i]) - weight set to zero!!' % (b.run,lb,b.lbStart,b.lbEnd))
                    self.nWarnings += 1
        for i in range(len(self.varList)):
            parName = self.varList[i]
            val = getattr(b,parName)
            valErr = getattr(b,parName+'Err')
            self.sum[i] += 1.
            self.sumx[i] += val
            self.sumxx[i] += val*val
            if self.lumiData is None:
                if valErr != 0. :
                    w = 1./valErr/valErr
                else:
                    w = 0.
                    print ('WARNING: Divison by zero for parameter %s   (val = %f  valErr = %f)\n' % (parName,val,valErr))
                    self.nWarnings += 1
            else:
                w = lumi
            self.sumw[i] += w
            self.sumwx[i] += w*val
            self.sumwxx[i] += w*val*val
            self.sumwwee[i] += w*w*valErr*valErr

    def average(self):
        self.rms = numpy.sqrt(self.sumxx/self.sum - self.sumx*self.sumx/self.sum/self.sum)   # factor n/n-1 ?
        if self.weightedAverage:
            if self.lumiData is None:
                self.ave = self.sumwx/self.sumw
                self.err = 1./numpy.sqrt(self.sumw)
            else:
                self.ave = self.sumwx/self.sumw
                self.err = numpy.sqrt(self.sumwwee)/self.sumw
        else:
            self.ave = self.sumx/self.sum
            self.err = self.rms/numpy.sqrt(self.sum)

    def getIndex(self, varName):
        for i in range(len(self.varList)):
            if self.varList[i]==varName:
                return i
        return None



class BeamSpotGraph:
    """A utility class for accumulating beam spot data into TGraphErrors."""

    def __init__(self, timeAxis = False, bcidAxis = False, separationAxis = False):
        self.timeAxis = timeAxis
        self.bcidAxis = bcidAxis
        self.separationAxis = separationAxis
        self.x = array('d')
        self.y = array('d')
        self.ex = array('d')
        self.ey = array('d')
        self.xmin = 1E10
        self.xmax = -1E10
        self.ymin = 1E10
        self.ymax = -1E10
        self.xoffset = 0
        self.what = None

    def add(self,bs,what,arescale=1.):
        """Add element what of BeamSpotValue bs to the graph."""
        self.what = what
        y = arescale*getattr(bs,what)   # Raise exception if not attribute what
        ey = arescale*getattr(bs,what+'Err',0)
        ex = (bs.lbEnd - bs.lbStart)/2.
        x = self.xoffset + bs.lbStart + ex

        if self.timeAxis:
            ex = (bs.timeEnd - bs.timeStart)/2.
            x = self.xoffset + bs.timeStart + ex
        if self.bcidAxis:
            ex = 0
            x = bs.bcid
        if self.separationAxis:
            ex = 0
            x = bs.separation
            
        self.x.append(x)
        self.ex.append(ex)
        self.y.append(y)
        self.ey.append(ey)
        self.xmin = min(x-ex,self.xmin)
        self.xmax = max(x+ex,self.xmax)
        self.ymin = min(y-ey,self.ymin)
        self.ymax = max(y+ey,self.ymax)

    def getTGraph(self,name=''):
        if len(self.x)>0:
            gr = ROOT.TGraphErrors(len(self.x),self.x,self.y,self.ex,self.ey)
            if not name:
                name = self.what
            gr.SetName(name)
            gr.SetTitle(name)
        else:
            gr = None
        return gr


class BeamSpotContainer:
    """Base class for containers of beam spot data such as ntuples or
       information stored in COOL. Derived classes must implement method
       allData() to iterate over all elements in the container."""

    def __init__(self):
        # Cuts for selecting entries in the container
        self.runMin = 0
        self.runMax = 9999999
        self.runList = []
        self.runListExclude = []        
        self.fillMin = 0
        self.fillMax = 9999999
        self.bcidMin = 0
        self.bcidMax = 9999999
        self.lbMin = 0
        self.lbMax = 9999999999
        self.timeMin = 0
        self.timeMax = 2000000000
        self.statusList = []
        self.acqFlag = None
        self.grl = ''
        self.grlIOVs = None
        self.previousGRLIndex = 0 # Cache last GRL index look up time
        
        # Iterator
        self.iter = None

        # Statistics on selected data
        self.initStatistics()

    def initStatistics(self):
        self.nTot = 0
        self.nSel = 0
        self.selRunMin = 9999999
        self.selRunMax = 0
        self.selFillMin = 9999999
        self.selFillMax = 0
        self.selBcidMin = 9999999
        self.selBcidMax = 0
        self.selTimeMin = 2000000000
        self.selTimeMax = -1

    #def __call__(self):
    #    """Iterator to iterate over selected elements in the container."""
    #    return self.allData()

    def __iter__(self):
        """Iterator to iterate over selected elements in the container."""
        self.iter = self.selectedData()
        return self

    def next(self):
        """Return next selected element in the container."""
        return self.iter.next()

    def __next__(self):
        """Return next selected element in the container."""
        return self.iter.next()

    def allData(self):
        """Default generator to iterate over all data. Must be overridden by derived classes."""
        raise StopIteration

    def selectedData(self):
        """Generator to iterate over selected elements in the container."""
        self.initStatistics()

        self.previousGRLIndex = 0

        for b in self.allData():
            self.nTot +=1
            if b.run<self.runMin: continue
            if b.run>self.runMax: continue
            if self.runList and b.run not in self.runList: continue
            if self.runListExclude and b.run in self.runListExclude: continue
            if b.fill<self.fillMin: continue
            if b.fill>self.fillMax: continue
            if b.bcid<self.bcidMin: continue
            if b.bcid>self.bcidMax: continue
            if b.lbStart<self.lbMin: continue
            if b.lbEnd-1>self.lbMax: continue   # lbEnd is exclusive
            if b.timeStart<self.timeMin: continue
            if b.timeEnd>self.timeMax: continue
            if self.statusList and b.status not in self.statusList: continue            
            if self.acqFlag is not None and b.acquisitionFlag != self.acqFlag: continue
            
            if self.grl:
                # Check if in GRL
                from DQUtils.sugar import RANGEIOV_VAL, RunLumi
                from DQUtils import IOVSet

                # Get IoVs from GRL
                if self.grlIOVs is None:
                    self.grlIOVs = IOVSet.from_grl(self.grl)

                idx = self.previousGRLIndex

                # Get current IoV
                test_iov = RANGEIOV_VAL(RunLumi(b.run, b.lbStart), RunLumi(b.run, b.lbEnd)) # COOL convention, ie lbEnd is exclusive

                # Search for IoV in GRL IoVs
                for i, iov in enumerate(self.grlIOVs[idx:]):
                    if (test_iov.since >= iov.since and test_iov.until <= iov.until):
                        self.previousGRLIndex = idx + i
                        break
                else:
                    # Only if don't break ->  IOV not found so skip
                    continue


            self.nSel += 1
            self.selRunMin = min(self.selRunMin,b.run)
            self.selRunMax = max(self.selRunMax,b.run)
            self.selFillMin = min(self.selFillMin,b.fill)
            self.selFillMax = max(self.selFillMax,b.fill)
            self.selBcidMin = min(self.selBcidMin,b.bcid)
            self.selBcidMax = max(self.selBcidMax,b.bcid)
            self.selTimeMin = min(self.selTimeMin,b.timeStart)
            self.selTimeMax = max(self.selTimeMax,b.timeEnd)
            yield b
        print ('\n%i entries selected out of total of %i entries in ntuple:' % (self.nSel,self.nTot))
        print ('... runs   %6i - %6i' % (self.selRunMin,self.selRunMax))
        print ('... fills  %6i - %6i' % (self.selFillMin,self.selFillMax))
        print ('... bcids  %6i - %6i' % (self.selBcidMin,self.selBcidMax))
        print ('... %s - %s' % (time.strftime('%a %b %d %X %Z %Y',time.localtime(self.selTimeMin)),
                               time.strftime('%a %b %d %X %Z %Y',time.localtime(self.selTimeMax))))
        print()

    def getDataCache(self):
        """Get a cache of all data in the form of a dict of runs, where each element
           is a dict with a BeamSpotValue for each individual lumi blocks."""
        cache = {}
        for b in self:
            r = b.run
            if r not in cache:
                cache[r] = {}
            if b.lbEnd-b.lbStart > 500:
                print ('WARNING: Cannot cache LB range %i ... %i for run %i' % (b.lbStart,b.lbEnd,r))
            else:
                for i in range(b.lbStart,b.lbEnd+1):
                    if b.status in self.statusList or not self.statusList:
                        cache[r][i] = b
        return cache

    def summary(self):
        """Get one-line info of Ntuple. Should be overridden by derived classes."""
        return "BeamSpotContainer base class"

    def cutSummary(self):
        """Get summary of cuts made when looping over selected data."""
        s = 'Cuts:\n'
        s += '    run                 %7i ... %7i\n' % (self.runMin,self.runMax)
        s += '    runList             %7s\n' % str(self.runList)
        s += '    runListExcluded     %7s\n' % str(self.runListExclude)        
        s += '    fill                %7i ... %7i\n' % (self.fillMin,self.fillMax)
        s += '    bcid                %7i ... %7i\n' % (self.bcidMin,self.bcidMax)
        s += '    LB                  %7i ... %7i\n' % (self.lbMin,self.lbMax)
        s += '    fit status          %7s\n' % str(self.statusList)
        s += '    %s - %s' % (time.strftime('%a %b %d %X %Z %Y',time.localtime(self.timeMin)),
                               time.strftime('%a %b %d %X %Z %Y',time.localtime(self.timeMax)))
        s += '   acquisition flag     %7s' % self.acqFlag 
        if self.grl:
            s += '    GRL                  %s\n' % self.grl
        return s



ROOT.gROOT.ProcessLine(BeamSpotValue(fullCorrelations=True).getROOTStruct())
from ROOT import BeamSpotNtBuf

class BeamSpotNt(BeamSpotContainer):
    """BeamSpotContainer for master beam spot ntuple."""

    def __init__(self,fileName,update=False,fullCorrelations=False):
        BeamSpotContainer.__init__(self)
        self.treeName = 'BeamSpotNt'
        self.fileName = fileName
        self.update = update
        self.fullCorrelations = fullCorrelations

        if update:
            bs = BeamSpotValue(self.fullCorrelations)
            self.rootFile = ROOT.TFile(fileName,'UPDATE')
            self.ntbuf = BeamSpotNtBuf()   # Not intialized
            self.nt = self.rootFile.Get(self.treeName)
            if not self.nt:
                self.nt = ROOT.TTree(self.treeName,'Master beam spot ntuple')
                for v in bs.varList():
                    varType = bs.getROOTType(v)
                    self.nt.Branch(v,ROOT.AddressOf(self.ntbuf,v),v+varType)
            else:
                for v in bs.varList():
                    self.nt.SetBranchAddress(v,ROOT.AddressOf(self.ntbuf,v))
        else:
            self.rootFile = ROOT.TFile(fileName)
            self.nt = self.rootFile.Get(self.treeName)
            if not self.nt:
                raise ValueError ('Tree %s not found in ntuple file %s' % (self.treeName,self.fileName))

        # Register instance's __del__() with exit handler
        # This is needed for ROOT >= 5.28 to prevent ROOTs exit handler cleaning up the file
        # before we have closed/written to it. Only needed if use TFile in __del__
        atexit.register(self.__del__)

    def __del__(self):
        # Prevent caling more than one (atexit/explicit del)
        if self.rootFile is not None:
            if self.update:
                self.rootFile.Write('',2)   # don't keep old versions
            self.rootFile.Close()
            self.rootFile = None

    def allData(self):
        varList = BeamSpotValue(self.fullCorrelations).varList()
        for j in range(self.nt.GetEntries()):
            self.nt.GetEntry(j)
            bs = BeamSpotValue(self.fullCorrelations)
            for v in varList:
                setattr(bs,v,getattr(self.nt,v,0))
            yield bs

    def fill(self,bs):
        for v in bs.varList():
            setattr(self.ntbuf,v,getattr(bs,v))
        self.nt.Fill()

    def summary(self):
        s = '%s:\n' % (self.fileName)
        if self.update:
            s += '    UPDATE MODE enabled\n'
        s += '    %s ntuple in tree %s\n' % (self.__class__.__name__,self.treeName)
        s += '    %i entries\n' % (self.nt.GetEntries())
        return s


class BeamSpotFinderNt(BeamSpotContainer):
    """BeamSpotContainer for ntuples created by InDetBeamSpotFinder."""

    # Status word conversion maps
    # For status word definition see https://twiki.cern.ch/twiki/bin/view/Atlas/CoolBeamSpotParameters#Definition_of_the_Status_Word
    # InDetBeamSpotFinder uses (see InDetBeamSpotFinder/IInDetBeamSpotTool.h):
    # enum FitResult {unsolved=0, successful, failed, problems};
    # enum FitID {vertexLL=1, vertexChi2=2, trackChi2=3, userDefined=4,unknown = 99};
    fitResultToStatusMap = {0: 0, 1: 3, 2: 0, 3: 0}
    fitIdToStatusMap = {1: 0x38, 2: 0x40, 3: 0x10}

    def __init__(self,fileName,treeName = 'BeamSpotNt',fullCorrelations=True):
        BeamSpotContainer.__init__(self)
        self.fileName = fileName
        self.treeName = treeName
        self.rootFile = ROOT.TFile(fileName)
        self.nt = self.rootFile.Get(treeName)
        self.fullCorrelations = fullCorrelations
        if not self.nt:
            raise ValueError ('Tree %s not found in ntuple file %s' % (treeName,fileName))

        # Register instance's __del__() with exit handler
        # This is needed for ROOT >= 5.28 to prevent ROOTs exit handler cleaning up the file
        # before we have closed/written to it. Only needed if use TFile in __del__
        atexit.register(self.__del__)

    def __del__(self):
        # Prevent caling more than one (atexit/explicit del)
        if self.rootFile is not None:
            self.rootFile.Close()
            self.rootFile = None

    def allData(self):
        for j in range(self.nt.GetEntries()):
            self.nt.GetEntry(j)
            bs = BeamSpotValue(self.fullCorrelations)
            try:
                bs.status = BeamSpotFinderNt.fitIdToStatusMap[self.nt.fitID]+BeamSpotFinderNt.fitResultToStatusMap[self.nt.fitStatus]
            except Exception:
                bs.status = 0
                print ("ERROR: can't translate (fitID,fitStatus) = (%i,%i) into status word" % (self.nt.fitID,self.nt.fitStatus))
            bs.run = self.nt.run
            try:
                bs.bcid = self.nt.bcid
            except Exception:
                pass
            bs.lbStart = self.nt.lumiStart
            bs.lbEnd = self.nt.lumiStart+self.nt.lumiRange
            bs.nEvents = self.nt.nEvents
            try:
                bs.nValid = self.nt.nValid
            except Exception:
                pass
            try:
                bs.nVtxAll = self.nt.nVtxAll
            except Exception:
                pass
            try:
                bs.nVtxPrim = self.nt.nVtxPrim
            except Exception:
                pass                        
            bs.posX = self.nt.xc
            bs.posY = self.nt.yc
            bs.posZ = self.nt.z
            bs.sigmaX = self.nt.sx
            bs.sigmaY = self.nt.sy
            bs.sigmaZ = self.nt.sz
            bs.tiltX = self.nt.ax
            bs.tiltY = self.nt.ay
            bs.rhoXY = self.nt.rhoxy
            bs.k = self.nt.k
            bs.posXErr = sqrt(self.nt.xcxc)
            bs.posYErr = sqrt(self.nt.ycyc)
            bs.posZErr = sqrt(self.nt.zz)
            bs.sigmaXErr = sqrt(self.nt.sxsx)
            bs.sigmaYErr = sqrt(self.nt.sysy)
            bs.sigmaZErr = sqrt(self.nt.szsz)
            bs.tiltXErr = sqrt(self.nt.axax)
            bs.tiltYErr = sqrt(self.nt.ayay)
            bs.rhoXYErr = sqrt(self.nt.rhoxyrhoxy)
            bs.kErr = sqrt(self.nt.kk)
            bs.covSxSy = self.nt.sxsy
            bs.covSxRhoXY = self.nt.sxrhoxy
            bs.covSyRhoXY = self.nt.syrhoxy

            bs.covXY = self.nt.x0y0
            bs.covXZ = self.nt.x0z
            bs.covXSx = self.nt.x0sx
            bs.covXSy = self.nt.x0sy
            bs.covXSz = self.nt.x0sz
            bs.covXTiltX = self.nt.x0ax
            bs.covXTiltY = self.nt.x0ay
            bs.covXRhoXY = self.nt.x0rhoxy
            bs.covXk = self.nt.x0k

            bs.covYZ = self.nt.y0z
            bs.covYSx = self.nt.y0sx
            bs.covYSy = self.nt.y0sy
            bs.covYSz = self.nt.y0sz
            bs.covYTiltX = self.nt.y0ax
            bs.covYTiltY = self.nt.y0ay
            bs.covYRhoXY = self.nt.y0rhoxy
            bs.covYk = self.nt.y0k

            bs.covZSx = self.nt.zsx
            bs.covZSy = self.nt.zsy
            bs.covZSz = self.nt.zsz
            bs.covZTiltX = self.nt.zax
            bs.covZTiltY = self.nt.zay
            bs.covZRhoXY = self.nt.zrhoxy
            bs.covZk = self.nt.zk

            bs.covTiltXTiltY = self.nt.axay
            bs.covTiltXSx = self.nt.axsx
            bs.covTiltXSy = self.nt.axsy
            bs.covTiltXSz = self.nt.axsz
            bs.covTiltXRhoXY = self.nt.axrhoxy
            bs.covTiltXk = self.nt.axk
            
            bs.covTiltYSx = self.nt.aysx
            bs.covTiltYSy = self.nt.aysy
            bs.covTiltYSz = self.nt.aysz
            bs.covTiltYRhoXY = self.nt.ayrhoxy
            bs.covTiltYk = self.nt.ayk
            
            bs.covSxSz = self.nt.sxsz
            bs.covSxk = self.nt.sxk
            
            bs.covSySz = self.nt.sysz
            bs.covSyk = self.nt.syk
            
            bs.covSzRhoXY = self.nt.szrhoxy
            bs.covSzk = self.nt.szk
            
            bs.covRhoXYk = self.nt.rhoxyk
            
            yield bs

    def summary(self):
        s = '%s:\n' % (self.fileName)
        s += '    %s ntuple in tree %s\n' % (self.__class__.__name__,self.treeName)
        s += '    %i entries\n' % (self.nt.GetEntries())
        return s


class BeamSpotCOOL(BeamSpotContainer):
    """BeamSpotContainer for beam spot information stored in COOL."""

    def __init__(self, tag, database='COOLOFL_INDET/CONDBR2', folder='/Indet/Beampos', fullCorrelations=False):
        BeamSpotContainer.__init__(self)
        self.database = database
        self.tag = tag
        self.folder = folder
        from CoolConvUtilities import AtlCoolLib
        self.cooldb = AtlCoolLib.indirectOpen(database, True, True, True)

    def __del__(self):
        self.cooldb.closeDatabase()

    def allData(self):
        from PyCool import cool
        
        folder = self.cooldb.getFolder(self.folder)

        iov1 = self.runMin << 32
        iov2 = (self.runMax+1) << 32
        if (iov2>cool.ValidityKeyMax):
            iov2=cool.ValidityKeyMax

        itr = folder.browseObjects(iov1, iov2, cool.ChannelSelection.all(), self.tag)

        while itr.goToNext():

            obj = itr.currentRef()
            
            since = obj.since()
            runBegin = since >> 32
            lumiBegin = since & 0xFFFFFFFF

            until = obj.until()
            lumiUntil = until & 0xFFFFFFFF

            bs            = BeamSpotValue()                            
            bs.run        = int(runBegin)
            bs.lbStart    = int(lumiBegin)
            bs.lbEnd      = int(lumiUntil)
            bs.posX       = float(obj.payloadValue('posX'))
            bs.posY       = float(obj.payloadValue('posY'))
            bs.posZ       = float(obj.payloadValue('posZ'))
            bs.sigmaX     = float(obj.payloadValue('sigmaX'))
            bs.sigmaY     = float(obj.payloadValue('sigmaY'))
            bs.sigmaZ     = float(obj.payloadValue('sigmaZ'))
            bs.tiltX      = float(obj.payloadValue('tiltX'))
            bs.tiltY      = float(obj.payloadValue('tiltY'))
            bs.status     = int(obj.payloadValue('status'))
            bs.posXErr    = float(obj.payloadValue('posXErr'))
            bs.posYErr    = float(obj.payloadValue('posYErr'))
            bs.posZErr    = float(obj.payloadValue('posZErr'))
            bs.sigmaXErr  = float(obj.payloadValue('sigmaXErr'))
            bs.sigmaYErr  = float(obj.payloadValue('sigmaYErr'))
            bs.sigmaZErr  = float(obj.payloadValue('sigmaZErr'))
            bs.tiltXErr   = float(obj.payloadValue('tiltXErr'))
            bs.tiltYErr   = float(obj.payloadValue('tiltYErr'))

            # COOL stores sigmaXY but BeamSpotData wants rho
            sigmaXYtmp    = float(obj.payloadValue('sigmaXY'))
            sigmaXtmp     = float(obj.payloadValue('sigmaX'))
            sigmaYtmp     = float(obj.payloadValue('sigmaY'))
            sigmaXYErrtmp = float(obj.payloadValue('sigmaXYErr'))
            sigmaXErrtmp  = float(obj.payloadValue('sigmaXErr'))
            sigmaYErrtmp  = float(obj.payloadValue('sigmaYErr'))

            try:
                rhoXYtmp = sigmaXYtmp / sigmaXtmp / sigmaYtmp
            except Exception:
                rhoXYtmp = 0
            bs.rhoXY      = rhoXYtmp

            try:
                sumtmp = sigmaXYErrtmp * sigmaXYErrtmp / sigmaXYtmp / sigmaXYtmp
                sumtmp += sigmaXErrtmp * sigmaXErrtmp / sigmaXtmp / sigmaXtmp
                sumtmp += sigmaYErrtmp * sigmaYErrtmp / sigmaYtmp / sigmaYtmp
                rhoXYErrtmp = sqrt(rhoXYtmp * rhoXYtmp * sumtmp)
            except Exception:
                rhoXYErrtmp = 0

            bs.rhoXYErr = rhoXYErrtmp

            yield bs

    def summary(self):
        s = 'COOL database %s, tag %s:\n' % (self.database,self.tag)
        s += '    %s ntuple, COOL folder %s\n' % (self.__class__.__name__,self.folder)
        return s

class BeamSpotCSV(BeamSpotContainer):
    """BeamSpotContainer for beam spot information stored in online CSV files"""

    def __init__(self, filename='/afs/cern.ch/user/a/atlidbs/data/69OnlineBeamspots.csv', delim=','):
        BeamSpotContainer.__init__(self)
        self.csvFile = open(filename, 'rb')
        self.csvReader = csv.DictReader(self.csvFile, delimiter=delim)

    def __del__(self):
        self.csvFile.close()

    def allData(self):

        for row in self.csvReader:

            bs = BeamSpotValue()

            bs.run         = int(row['run'])
            bs.lbStart     = int(row['firstLBN'])
            bs.lbEnd       = int(row['lastLBN'])
            bs.posX        = float(row['posX'])
            bs.posY        = float(row['posY'])
            bs.posZ        = float(row['posZ'])           
            bs.sigmaX      = float(row['sigmaX'])
            bs.sigmaY      = float(row['sigmaY'])
            bs.sigmaZ      = float(row['sigmaZ'])
            bs.tiltX       = float(row['tiltX'])
            bs.tiltY       = float(row['tiltY'])           
            bs.status      = int(row['status'])

            bs.posXErr     = float(row['posXE'])
            bs.posYErr     = float(row['posYE'])
            bs.posZErr     = float(row['posZE'])           
            bs.sigmaXErr   = float(row['sigmaXE'])
            bs.sigmaYErr   = float(row['sigmaYE'])
            bs.sigmaZErr   = float(row['sigmaZE'])                        
            bs.tiltXErr    = float(row['tiltXE'])
            bs.tiltYErr    = float(row['tiltYE'])
        
            yield bs


    def summary(self):
        s = 'CSV file %s:\n' % self.filename
        s += '    %s ntuple\n' % (self.__class__.__name__)
        return s

# Test code for modules
if __name__ == '__main__':

    #Test for ntuple reading
    #data = BeamSpotFinderNt('/afs/cern.ch/user/a/atlidbs/jobs/data10_7TeV.00167776.express_express/DB_BEAMSPOT.x50_c323/data10_7TeV.00167776.express_express-DB_BEAMSPOT.x50_c323.MergeNt-nt.root')
    data = BeamSpotNt('/afs/cern.ch/user/a/atlidbs/nt/beamspotnt-IndetBeampos-Oct10-Collision_7T_2010_07-v1.root')

    #Test for COOL reading
    #data = BeamSpotCOOL('IndetBeampos-ES1-UPD2')
    #data.runMin = 167844
    #data.runMax = 167844

    #Test for CSV reading
    #data = BeamSpotCSV("/afs/cern.ch/user/a/atlidbs/data/69OnlineBeamspots.csv")
    for b in data:
        print (b)
    pass
