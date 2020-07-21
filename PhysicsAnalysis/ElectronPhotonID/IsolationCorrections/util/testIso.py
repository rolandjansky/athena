#!/usr/bin/env python
import logging
import array as ar
import ROOT as r
import math

r.xAOD.Init().ignore()

r.PyConfig.IgnoreCommandLineOptions = True
logging.basicConfig(level=logging.INFO)

# This is to allow to create EGamma objects
factory = r.EgammaFactory()
event = r.xAOD.TEvent()

OutputLevel = 3
Save = False
doDraw = False

doOld = True
doNew = True

FileName = 'isolation_ptcorrections_rel20_2_v4.root'
UseLogLog = True

#
import cppyy
cppyy.loadDictionary('xAODCoreRflxDict')
cppyy.loadDictionary('xAODPrimitivesDict')
isoPar = r.xAOD.Iso

# how to I use setProperty within AthDerivation ? This works fine in AnalysisBase <= 21.2.106.
if doOld:
    print 'Now trying to initialize an IsolationCorrectionTool (no LogLog)'
    toolIso = r.CP.IsolationCorrectionTool('isoTest')
    toolIso.setProperty("int")("OutputLevel",OutputLevel).ignore()
    toolIso.setProperty("ForcePartType",True).ignore()
    toolIso.initialize().ignore()

if doNew:
    print 'Now trying to initialize an IsolationCorrectionTool (eventually LogLog)'
    toolIsoLL = r.CP.IsolationCorrectionTool('isoTestLogLog')
    #toolIsoLL.setProperty("CorrFile",FileName).ignore()
    toolIsoLL.setProperty("int")("OutputLevel",OutputLevel).ignore()
    toolIsoLL.setProperty("ForcePartType",True).ignore()
    toolIsoLL.setProperty("LogLogFitForLekage",UseLogLog).ignore()
    toolIsoLL.initialize().ignore()

etaB = [0.05, 0.3, 0.7, 1.0, 1.2, 1.4, 1.7, 1.9, 2.2, 2.4]
etaBN = [ "0.0_0.1", "0.1_0.6", "0.6_0.8", "0.8_1.15", "1.15_1.37", "1.37_1.52", "1.52_1.81", "1.81_2.01", "2.01_2.37", "2.37_2.47" ]

gg = []
ff = []
cc = []

emin = 25
emax = 26
#emax = 6001
#emin = 3500
#emax = 3600
energyScan = range(emin,emax)
nP = len(energyScan)
from array import array
x = array('d',nP*[0])
y = array('d',nP*[0])
yLL = array('d',nP*[0])

isoScan = range(3)
particleScan = range(5)
etaScan = range(10)

fIn = r.TFile.Open("/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/IsolationCorrections/v1/isolation_ptcorrections_rel20_2.root")

ph  = [ '_unconverted', '_converted_ok', '_converted_trouble', '_author_1_electron', '_author_16_electron' ]
iso = [ 'topoetcone20', 'topoetcone30' , 'topoetcone40' ]
par = [isoPar.topoetcone20, isoPar.topoetcone30, isoPar.topoetcone40 ]

# iso type
for ii in isoScan:

    # particle type
    for ip in particleScan:

        if doDraw:
            cN = 'cc'+ph[ip]+iso[ii]
            c = r.TCanvas(cN,cN,1200,800)
            c.Divide(4,3)
            cc.append(c)

        
        # eta bin
        for ie in etaScan:
            if (ip < 3 and ie != 5 and ie != 9) or ip >= 3:
                
                print 'Running on isolation type ',iso[ii],' for particle type ',ph[ip],' and eta bin ',etaBN[ie]

                eta = etaB[ie]
                etmax = energyScan[nP-1]/math.cosh(eta)
                lun = r.TLine(energyScan[0]/math.cosh(eta),0.,energyScan[nP-1]/math.cosh(eta),0.)
                lun.SetLineWidth(2)
                lun.SetLineStyle(2)

                fN = 'mean_f_'+iso[ii]+'_eta_'+etaBN[ie]+ph[ip]
                #print fN
                fu = fIn.Get(fN)
                fu.SetLineColor(2)
                ff.append(fu)
                fu.SetName("fun"+str(ie)+ph[ip]+iso[ii])

                for eGeV in energyScan:
                    e  = eGeV*1000
                    et = e/math.cosh(eta)
                    # use the general create_photon (eta,phi,e0,e1,e2,e3,rconv,zconv)
                    # because the tool use E1+E2+E3, not E, so I put E0 = 0, E = -1 ==> E = E1+E2+E3
                    if ip == 0:
                        rconv = 1000
                    elif ip == 1:
                        rconv = 200
                    else:
                        rconv = 100
                    if ip < 3:
                        p = factory.create_photon(eta,0.1,0,e/3,e/3,e/3.,-1,rconv,10000)
                    else:
                        # this is to differentiate author 1 and author 16 electrons in this easy check
                        phi = 0.1
                        if ip == 4:
                            phi = -0.1
                        p = factory.create_electron(eta,phi,0,e/3,e/3,e/3.,-1)
                    x[eGeV-emin] = et
                    if doOld:
                        y[eGeV-emin]   = toolIso.GetPtCorrection(p,par[ii])
                    if doNew:
                        yLL[eGeV-emin] = toolIsoLL.GetPtCorrection(p,par[ii])
                    #print 'resu = ',et,y[eGeV-emin],yLL[eGeV-emin],fu.Eval(et)
                g = r.TGraph(nP,x,y)
                g.SetName("isoEta"+str(ie)+ph[ip]+iso[ii])
                g.SetMarkerStyle(20)
                g.SetMarkerSize(0.3)
                gg.append(g)

                gLL = r.TGraph(nP,x,yLL)
                gLL.SetName("isoLLEta"+str(ie)+ph[ip]+iso[ii])
                gLL.SetMarkerStyle(20)
                gLL.SetMarkerSize(0.3)
                gLL.SetMarkerColor(4)
                gg.append(gLL)

                if doDraw:
                    c.cd(ie+1)
                    if doOld:
                        g.Draw("AP")
                    if doNew and doOld:
                        gLL.Draw("P")
                    elif doNew and not doOld:
                        gLL.Draw("AP")
                    fu.Draw("same")
                    lun.Draw()

        if doDraw:
            c.SaveAs(cN+'.pdf')

if doOld:
    print ('Will finalize the default tool')
    toolIso.finalize()
    print ('Done')
if doNew:
    #toolIsoLL.Print()
    print ('Will finalize the LL tool')
    toolIsoLL.finalize()
    print ('Done')

if Save:
    fOut = r.TFile("outCompare.root","recreate")
    for g in gg:
        g.Write()
    for f in ff:
        f.Write()
    for c in cc:
        c.Write()
    fOut.Close()
