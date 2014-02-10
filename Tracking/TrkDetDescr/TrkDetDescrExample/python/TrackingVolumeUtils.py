# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math
from ROOT import *

def drawTrackingVolume(g4Tree,tgTree,saveLocation) :
    
    tvname = g4Tree.GetName()
    print '----------------------------------------------------------------'
    print '  TrackingVolume: ', tvname
    
    tvCanvas = TCanvas(tvname,tvname,100,100,800,1000)
    tvCanvas.Divide(2,3)

    tgTree.SetMarkerStyle(8)
    tgTree.SetMarkerSize(0.5)
    tgTree.SetMarkerColor(kRed)
    tgTree.SetLineColor(kRed)
    
    tvsavename = tvname.replace('::',"_")
    
    # x0 vs eta / phi
    tvCanvas.cd(1)
    try :
        g4Tree.Draw('PathInX0:Eta>>'+tvsavename+'G4X0Eta','','prof')
        tgTree.Draw('PathInX0:Eta>>'+tvsavename+'TGX0Eta','','prof')
        x0etaG4 = gDirectory.Get(tvsavename+'G4X0Eta')
        x0etaTG = gDirectory.Get(tvsavename+'TGX0Eta')
        x0etaG4.GetXaxis().SetTitle('#eta')
        x0etaG4.GetYaxis().SetTitle('t [X_{0}]')
        x0etaG4.Draw('hist')
        x0etaTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass
    tvCanvas.cd(2)
    try :
        g4Tree.Draw('PathInX0:Phi>>'+tvsavename+'G4X0Phi','','prof')
        tgTree.Draw('PathInX0:Phi>>'+tvsavename+'TGX0Phi','','prof')
        x0phiG4 = gDirectory.Get(tvsavename+'G4X0Phi')
        x0phiTG = gDirectory.Get(tvsavename+'TGX0Phi')
        x0phiG4.GetXaxis().SetTitle('#phi')
        x0phiG4.GetYaxis().SetTitle('t [X_{0}]')
        x0phiG4.Draw('hist')
        x0phiTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass    
    # l0 vs eta / phi
    tvCanvas.cd(3)
    try :
        g4Tree.Draw('PathInL0:Eta>>'+tvsavename+'G4L0Eta','','prof')
        tgTree.Draw('PathInL0:Eta>>'+tvsavename+'TGL0Eta','','prof')
        l0etaG4 = gDirectory.Get(tvsavename+'G4L0Eta')
        l0etaTG = gDirectory.Get(tvsavename+'TGL0Eta')
        l0etaG4.GetXaxis().SetTitle('#eta')
        l0etaG4.GetYaxis().SetTitle('t [#Lambda_{0}]')    
        l0etaG4.Draw('hist')
        l0etaTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass        
    tvCanvas.cd(4)
    try :
        g4Tree.Draw('PathInL0:Phi>>'+tvsavename+'G4L0Phi','','prof')
        tgTree.Draw('PathInL0:Phi>>'+tvsavename+'TGL0Phi','','prof')
        l0phiG4 = gDirectory.Get(tvsavename+'G4L0Phi')
        l0phiTG = gDirectory.Get(tvsavename+'TGL0Phi')
        l0phiG4.GetXaxis().SetTitle('#phi')
        l0phiG4.GetYaxis().SetTitle('t [#Lambda_{0}]')    
        l0phiG4.Draw('hist')
        l0phiTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass        
    # AZrho vs eta / phi
    tvCanvas.cd(5)
    try :
        g4Tree.Draw('Z/A*Rho*Path/Measure:Eta>>'+tvsavename+'G4ZArhoEta','','prof')
        tgTree.Draw('Z/A*Rho*Path/Measure:Eta>>'+tvsavename+'TGZArhoEta','','prof')
        zarhoetaG4 = gDirectory.Get(tvsavename+'G4ZArhoEta')
        zarhoetaTG = gDirectory.Get(tvsavename+'TGZArhoEta')
        if zarhoetaG4.GetMaximum() < zarhoetaTG.GetMaximum() :
           zarhoetaG4.SetMaximum(zarhoetaTG.GetMaximum()) 
        zarhoetaG4.GetXaxis().SetTitle('#eta')
        zarhoetaG4.GetYaxis().SetTitle('Eff. Z/A* #rho [ g/mm^{3} ]')    
        zarhoetaG4.Draw('hist')
        zarhoetaTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass        
    tvCanvas.cd(6)
    try :
        g4Tree.Draw('Z/A*Rho*Path/Measure:Phi>>'+tvsavename+'G4ZArhoPhi','','prof')
        tgTree.Draw('Z/A*Rho*Path/Measure:Phi>>'+tvsavename+'TGZArhoPhi','','prof')
        zarhophiG4 = gDirectory.Get(tvsavename+'G4ZArhoPhi')
        zarhophiTG = gDirectory.Get(tvsavename+'TGZArhoPhi')
        if zarhophiG4.GetMaximum() < zarhophiTG.GetMaximum() :
           zarhophiG4.SetMaximum(zarhophiTG.GetMaximum())    
        zarhophiG4.GetXaxis().SetTitle('#phi')
        zarhophiG4.GetYaxis().SetTitle('Eff. Z/A * #rho [ g/mm^{3} ]')    
        zarhophiG4.Draw('hist')
        zarhophiTG.Draw('pe,same')
    except (ReferenceError, AttributeError):
        pass
        
    tvCanvas.SaveAs(saveLocation+tvsavename+'.png')