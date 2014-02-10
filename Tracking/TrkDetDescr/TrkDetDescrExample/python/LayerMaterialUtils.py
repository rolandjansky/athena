# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import math
from ROOT import *

def drawCylinderLayer(g4CylTree,cylLayer,tgCylTree,saveLocation) :
    
    gROOT.cd()     
    # create a canvas first
    clname = cylLayer.GetName()
    
    # now fill it 
    for evt in cylLayer :
        
        radius   = evt.cylRadius
        position = evt.cylPosZ
        halfz    = evt.cylHalfZ
        binsz    = evt.cylMatBinsZ
        binsphi  = evt.cylMatBinsPhi
        
        numbinsz   = 100
        numbinsphi = 100
        if binsz   > numbinsz   : numbinsz = binsz
        if binsphi > numbinsphi : numbinsphi = binsphi 
        
        
        print '----------------------------------------------------------------'
        print '  Layer: ', clname, ' ( LayerIndex : ',evt.cylLayerIndex, ')'
        print '    r, halflength  : ', radius, halfz
        print '    bins (z/phi)   : ', evt.cylMatBinsZ, evt.cylMatBinsPhi
        
        
        # create the x0 histogram
        clmap_x0         = TH2F('clmap_x0_'+clname,clname,binsz,-halfz,halfz,binsphi,-math.pi,math.pi)
        # the steps
        stepZ   = 2.*halfz/numbinsz
        stepPhi = 2*math.pi/numbinsphi
        x0max = 0.
        # loop over bins and fill it
        for ibin in xrange(evt.cylMatBinsTotal) :
            # first get the ibin phi
            ibinz   = ibin/numbinsphi
            ibinphi = ibin - ibinz*numbinsphi      
            x0 = evt.cylMatX0[ibin]
            if x0 > x0max : x0max = x0           
            clmap_x0.SetBinContent(ibinz+1,ibinphi+1,x0)
            # z / phi value for the profile plots
            izval   = -halfz+(ibinz+0.5)*stepZ
            iphival = -math.pi+(ibinphi+0.5)*stepPhi
        pass
    
    # and draw it
    canvasX0map = TCanvas(clname+'_x0',clname,100,100,800,1000)
    canvasX0map.Divide(2,3)

    # ---------------------------------------------------------
    # draw profiles first to get a good estimate for the maximum
    canvasX0map.cd(5)
    x0profZG4 = TProfile(clname+'x0profZG4',clname+'x0profZG4',numbinsz,-halfz+position,halfz+position)
    x0profZTG = TProfile(clname+'x0profZTG',clname+'x0profZTG',numbinsz,-halfz+position,halfz+position)
    g4CylTree.Draw('PathInX0:DensedHitZ>>'+clname+'x0profZG4','','prof')
    tgCylTree.Draw('PathInX0:DensedHitZ>>'+clname+'x0profZTG','','prof')
    x0profG4z = gDirectory.Get(clname+'x0profZG4')
    x0profTGz = gDirectory.Get(clname+'x0profZTG')
    # style 
    x0profTGz.SetMarkerStyle(8)
    x0profTGz.SetMarkerSize(0.5)
    x0profTGz.SetMarkerColor(kRed)
    x0profTGz.SetLineColor(kRed)
    x0profG4z.GetYaxis().SetTitle('t [X_{0}]')
    x0profG4z.GetXaxis().SetTitle('z [mm]')
    x0maxProf = 1.5*x0profG4z.GetMaximum()
    x0profG4z.GetYaxis().SetRangeUser(0.,x0maxProf)
    x0profG4z.Draw('hist')
    x0profTGz.SetMarkerColor(kRed)
    x0profTGz.SetMarkerSize(0.8)
    x0profTGz.Draw('pe,same')
    canvasX0map.cd(6)
    x0profPhiG4 = TProfile(clname+'x0profPhiG4',clname+'x0profPhiG4',numbinsphi,-math.pi,math.pi)
    x0profPhiTG = TProfile(clname+'x0profPhiTG',clname+'x0profPhiTG',numbinsphi,-math.pi,math.pi)
    g4CylTree.Draw('PathInX0:DensedHitPhi>>'+clname+'x0profPhiG4','','prof')
    tgCylTree.Draw('PathInX0:DensedHitPhi>>'+clname+'x0profPhiTG','','prof')
    x0profG4phi = gDirectory.Get(clname+'x0profPhiG4')
    x0profTGphi = gDirectory.Get(clname+'x0profPhiTG')
    # style 
    x0profTGphi.SetMarkerStyle(8)
    x0profTGphi.SetMarkerSize(0.5)
    x0profTGphi.SetMarkerColor(kRed)
    x0profTGphi.SetLineColor(kRed)
    x0profG4phi.GetXaxis().SetTitle('#phi')
    x0profG4phi.GetYaxis().SetTitle('t [X_{0}]')
    if 1.5*x0profG4phi.GetMaximum() > x0maxProf : x0maxProf = 1.5*x0profG4phi.GetMaximum()
    x0profG4phi.GetYaxis().SetRangeUser(0.,1.5*x0profG4phi.GetMaximum())
    x0profG4phi.Draw('hist')
    x0profTGphi.SetMarkerColor(kRed)
    x0profTGphi.SetMarkerSize(0.8)
    x0profTGphi.Draw('pe,same')
    # ---- now the maps
    canvasX0map.cd(1)
    try :
        g4CylTree.Draw('PathInX0>>'+clname+'range')
        x0range = gDirectory.Get(clname+'range')
        x0maxProf = 1.2*x0range.GetXaxis().GetXmax()
    except TypeError :
        print '  -> TTree does not exist in TrackingGeometry '    
    x0mapG4       = TH3F(clname+'x0mapG4',clname+'x0mapG4', numbinsphi,-math.pi,math.pi,numbinsz,-halfz+position,halfz+position,50,0.,x0maxProf)
    try :
        g4CylTree.Draw('PathInX0:DensedHitZ:DensedHitPhi>>'+clname+'x0mapG4','','')
        x0mapG4      = gDirectory.Get(clname+'x0mapG4')
        x0mapG4Prof	= x0mapG4.Project3DProfile('xy') 
        x0mapG4Prof.GetXaxis().SetTitle('z [mm]')
        x0mapG4Prof.GetYaxis().SetTitle('#phi')    
        x0mapG4Prof.GetZaxis().SetTitle('t [X_{0}]')        
        x0mapG4Prof.GetZaxis().SetTitleOffset(1.5*x0mapG4Prof.GetZaxis().GetTitleOffset())
        x0mapG4Prof.GetZaxis().SetRangeUser(0.,x0maxProf)
        x0mapG4Prof.SetBit(kCanDelete)
        x0mapG4Prof.SetBit(kMustCleanup)        
        x0mapG4Prof.Draw('colz')                             
    except TypeError :
        print '  -> TTree does not exist in TrackingGeometry '        
    canvasX0map.cd(2)
    x0mapTG = TH3F(clname+'x0mapTG',clname+'x0mapTG',numbinsphi,-math.pi,math.pi,numbinsz,-halfz+position,halfz+position,50,0.,x0maxProf)
    try :
        tgCylTree.Draw('PathInX0:DensedHitZ:DensedHitPhi>>'+clname+'x0mapTG','','')
        x0mapTG      = gDirectory.Get(clname+'x0mapTG')
        x0mapTGProf	= x0mapTG.Project3DProfile('xy') 
        x0mapTGProf.GetXaxis().SetTitle('z [mm]')
        x0mapTGProf.GetYaxis().SetTitle('#phi')    
        x0mapTGProf.GetZaxis().SetTitle('t [X_{0}]')        
        x0mapTGProf.GetZaxis().SetTitleOffset(1.5*x0mapTGProf.GetZaxis().GetTitleOffset())
        x0mapTGProf.GetZaxis().SetRangeUser(0.,x0maxProf)
        x0mapTGProf.SetBit(kCanDelete)
        x0mapTGProf.SetBit(kMustCleanup)        
        x0mapTGProf.Draw('colz')
    except TypeError :
        print '  -> TTree does not exist in Geant4 '                                     
    canvasX0map.cd(3)
    clmap_x0.GetZaxis().SetRangeUser(0.,x0max)
    clmap_x0.GetXaxis().SetTitle('z [mm]')
    clmap_x0.GetYaxis().SetTitle('#phi')    
    clmap_x0.GetZaxis().SetTitle('t [X_{0}]') 
    clmap_x0.GetZaxis().SetTitleOffset(1.2*clmap_x0.GetZaxis().GetTitleOffset())       
    clmap_x0.Draw('colz')
    # ---------------------------------------------------------    
    canvasX0map.cd(4)
    try :
        x0mapDiff = x0mapTGProf.Clone()
        x0mapDiff.Add(x0mapG4Prof,-1)
        x0mapDiff.GetXaxis().SetTitle('z [mm]')
        x0mapDiff.GetYaxis().SetTitle('#phi')    
        x0mapDiff.GetZaxis().SetTitle('#Delta(t) [X_{0}]')
        x0mapDiff.GetZaxis().SetRangeUser(-1.,1.) 
        x0mapDiff.GetZaxis().SetTitleOffset(1.5*x0mapDiff.GetZaxis().GetTitleOffset())
        x0mapDiff.Divide(x0mapG4Prof)
        x0mapDiff.Draw('colz')
    except UnboundLocalError :
        print '  -> No TrackingGeometry/G4 histograms to divide'


    canvasX0map.SaveAs(saveLocation+'/LayerMaterial_CylinderLayer_'+str(evt.cylLayerIndex)+'.png')

    canvasAnalysis = TCanvas(clname+'_analyse',clname,100,100,600,1200)
    canvasAnalysis.Divide(1,3)
    canvasAnalysis.cd(1)
    cylLayer.Draw('LayerMaterialL0:LayerMaterialX0','','box')    
    canvasAnalysis.cd(2)
    cylLayer.Draw('LayerMaterialZARho:LayerMaterialX0','','box')
    canvasAnalysis.cd(3)
    cylLayer.Draw('LayerMaterialZARho:LayerMaterialL0','','box')
    canvasAnalysis.SaveAs(saveLocation+'/'+clname+'_analysis.png')

    # memory cleanup
    gDirectory.Delete(clname+'x0mapTG')
    gDirectory.Delete(clname+'x0mapG4')
    gDirectory.Delete('clmap_x0_'+clname)
    gDirectory.Delete('clmap_x0_pz_'+clname)
    gDirectory.Delete('clmap_x0_pphi_'+clname)

    #clmap_x0_profZ.Draw('hist')
    #canvasX0map.cd(3)
    #clmap_x0_profPhi.Draw('hist')


def drawDiscLayer(g4DiscTree,discLayer,tgDiscTree,saveLocation) :

    gROOT.cd()    
    # create a canvas first
    dlname = discLayer.GetName()
    # now fill it 
    for evt in discLayer :

        innerR   = evt.discInnerR
        outerR   = evt.discOuterR
        position = evt.discPositionZ
        binsR    = evt.discMatBinsR
        binsPhi  = evt.discMatBinsPhi
        scalor   = math.pi/outerR
        
        numbinsr  = 100
        numbinsphi = 100
        if binsR > numbinsr     : numbinsr = binsR
        if binsPhi > numbinsphi : numbinsphi = binsPhi
        
        print '----------------------------------------------------------------'
        print '  Layer: ', dlname, ' ( LayerIndex : ',evt.discLayerIndex, ')'
        print '    position       : ', position
        print '    innerR/outerR  : ', innerR, outerR
        print '    bins (r/phi)   : ', binsR, binsPhi

        # POLAR plot needs to be in -pi,pi,-pi,pi, NOT yet fixed

        # create the x0 histogram
        dlmap_x0         = TH2F('dlmap_x0_'+dlname,dlname,binsR,-math.pi,math.pi,binsPhi,-math.pi,math.pi)
        # the steps
        stepR   = (outerR-innerR)/binsR
        stepPhi = 2*math.pi/binsPhi
        x0max = 0.
        # loop over bins and fill it
        for ibin in xrange(evt.discMatBinsTotal) :
            ibinphi = ibin/binsR
            ibinr = ibin - ibinphi*binsR
            #ibinz   = ibin/binsphi
            #ibinphi = ibin - ibinz*binsphi            
            x0 = evt.discMatX0[ibin]
            if x0 > x0max : x0max = x0
            iphival   = -math.pi+(ibinphi+0.5)*stepPhi
            irvalscal = (innerR + (ibinr+0.5)*stepR)*scalor          
            dlmap_x0.Fill(irvalscal,iphival,x0)
            # z / phi value for the profile plots
            
        pass

    canvasX0map = TCanvas(dlname,dlname,100,100,800,1100)
    canvasX0map.Divide(2,3)
    # draw profiles tog et good estimate for x0max first
    x0maxProf = x0max
    canvasX0map.cd(5)
    x0profRG4 = TProfile(dlname+'x0profRG4',dlname+'x0profRG4',numbinsr,innerR,outerR)
    x0profRTG = TProfile(dlname+'x0profRTG',dlname+'x0profRTG',numbinsr,innerR,outerR)
    g4DiscTree.Draw('PathInX0:DensedHitR>>'+dlname+'x0profRG4','','prof')
    tgDiscTree.Draw('PathInX0:DensedHitR>>'+dlname+'x0profRTG','','prof')
    x0profG4r = gDirectory.Get(dlname+'x0profRG4')
    x0profTGr = gDirectory.Get(dlname+'x0profRTG')
    # style 
    x0profTGr.SetMarkerStyle(8)
    x0profTGr.SetMarkerSize(0.5)
    x0profTGr.SetMarkerColor(kRed)
    x0profTGr.SetLineColor(kRed)
    x0profG4r.GetXaxis().SetTitle('r [mm]')
    x0profG4r.GetYaxis().SetTitle('t [X_{0}]')
    x0maxProf = 1.5*x0profG4r.GetMaximum()
    x0profG4r.GetYaxis().SetRangeUser(0.,x0maxProf)   
    x0profG4r.Draw('hist')
    x0profTGr.SetMarkerColor(kRed)
    x0profTGr.SetMarkerSize(0.8)
    x0profTGr.Draw('pe,same')
    canvasX0map.cd(6)
    x0profPhiG4 = TProfile(dlname+'x0profPhiG4',dlname+'x0profPhiG4',numbinsphi,-math.pi,math.pi)
    x0profPhiTG = TProfile(dlname+'x0profPhiTG',dlname+'x0profPhiTG',numbinsphi,-math.pi,math.pi)
    g4DiscTree.Draw('PathInX0:DensedHitPhi>>'+dlname+'x0profPhiG4','','prof')
    tgDiscTree.Draw('PathInX0:DensedHitPhi>>'+dlname+'x0profPhiTG','','prof')
    x0profG4phi = gDirectory.Get(dlname+'x0profPhiG4')
    x0profTGphi = gDirectory.Get(dlname+'x0profPhiTG')
    # style 
    x0profTGphi.SetMarkerStyle(8)
    x0profTGphi.SetMarkerSize(0.5)
    x0profTGphi.SetMarkerColor(kRed)
    x0profTGphi.SetLineColor(kRed)
    x0profG4phi.GetXaxis().SetTitle('m [mm]')
    x0profG4phi.GetYaxis().SetTitle('t [X_{0}]')
    if 1.5*x0profG4phi.GetMaximum() > x0maxProf : x0maxProf = 1.5*x0profG4phi.GetMaximum()
    x0profG4phi.GetYaxis().SetRangeUser(0.,1.5*x0profG4phi.GetMaximum())   
    x0profG4phi.Draw('hist')
    x0profTGphi.SetMarkerColor(kRed)
    x0profTGphi.SetMarkerSize(0.8)
    x0profTGphi.Draw('pe,same')
    
    canvasX0map.cd(1)
    try :
        g4DiscTree.Draw('PathInX0>>'+dlname+'range')
        x0range = gDirectory.Get(dlname+'range')
        x0maxProf = 1.2*x0range.GetXaxis().GetXmax()
    except TypeError :
        print '  -> TTree does not exist in TrackingGeometry '    
    x0mapG4 = TH3F(dlname+'x0mapG4',dlname+'x0mapG4',numbinsr,-1.1*outerR,1.1*outerR,numbinsr,-1.1*outerR,1.1*outerR,50,0.,x0maxProf)
    try : 
        g4DiscTree.Draw('PathInX0:DensedHitY:DensedHitX>>'+dlname+'x0mapG4','','')
        x0mapG4      = gDirectory.Get(dlname+'x0mapG4')
        x0mapG4Prof	= x0mapG4.Project3DProfile('xy') 
        x0mapG4Prof.GetXaxis().SetTitle('x [mm]')
        x0mapG4Prof.GetYaxis().SetTitle('y [mm]')    
        x0mapG4Prof.GetZaxis().SetTitle('t [X_{0}]') 
        x0mapG4Prof.GetZaxis().SetRangeUser(0.,x0maxProf)
        x0mapG4Prof.GetZaxis().SetTitleOffset(1.5*x0mapG4Prof.GetZaxis().GetTitleOffset())
        x0mapG4Prof.SetBit(kCanDelete)
        x0mapG4Prof.SetBit(kMustCleanup)        
        x0mapG4Prof.Draw('colz')                            
    except TypeError :
        print '  -> TTree does not exist in TrackingGeometry '
    canvasX0map.cd(2) 
    try : 
        x0mapTG = TH3F(dlname+'x0mapTG',dlname+'x0mapTG',numbinsr,-1.1*outerR,1.1*outerR,numbinsr,-1.1*outerR,1.1*outerR,50,0.,x0maxProf)
        tgDiscTree.Draw('PathInX0:DensedHitY:DensedHitX>>'+dlname+'x0mapTG','','')
        x0mapTG      = gDirectory.Get(dlname+'x0mapTG')
        x0mapTGProf	= x0mapTG.Project3DProfile('xy') 
        x0mapTGProf.GetXaxis().SetTitle('x [mm]')
        x0mapTGProf.GetYaxis().SetTitle('y [mm]')    
        x0mapTGProf.GetZaxis().SetTitle('t [X_{0}]')
        x0mapTGProf.GetZaxis().SetRangeUser(0.,x0maxProf)
        x0mapTGProf.GetZaxis().SetTitleOffset(1.5*x0mapTGProf.GetZaxis().GetTitleOffset())
        x0mapTGProf.SetBit(kCanDelete)
        x0mapTGProf.SetBit(kMustCleanup)        
        x0mapTGProf.Draw('colz')       
    except TypeError :
        print '  -> TTree does not exist in Geant4 '                         
    canvasX0map.cd(3)
    dlmap_x0.SetLineColor(kWhite)
    dlmap_x0.GetZaxis().SetRangeUser(0.,1.1*x0max)
    dlmap_x0.Draw('polcolz')
    canvasX0map.cd(4)
    try : 
        x0mapDiff = x0mapTGProf.Clone()
        x0mapDiff.Add(x0mapG4Prof,-1)
        x0mapDiff.GetXaxis().SetTitle('x [mm]')
        x0mapDiff.GetYaxis().SetTitle('y [mm]')    
        x0mapDiff.GetZaxis().SetTitle('t [X_{0}]')
        x0mapDiff.GetZaxis().SetRangeUser(-1.,1.)
        x0mapDiff.GetZaxis().SetTitleOffset(1.5*x0mapDiff.GetZaxis().GetTitleOffset())
        x0mapDiff.Divide(x0mapG4Prof)
        x0mapDiff.Draw('colz')        
    except UnboundLocalError :
        print '  -> No TrackingGeometry/G4 histograms to divide'           

    canvasX0map.SaveAs(saveLocation+'/LayerMaterial_DiscLayer_'+str(evt.discLayerIndex)+'.png')
    
    # memory cleanup
    gDirectory.Delete(dlname+'x0mapTG')
    gDirectory.Delete(dlname+'x0mapG4')