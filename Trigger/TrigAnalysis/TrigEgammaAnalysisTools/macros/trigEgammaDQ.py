#! /usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import sys, optparse, shutil, os, argparse
from sys import argv, exit
import subprocess

__doc__ = """\
Recurvsively search input HIST file starting from HLT/Egamma and generate performance plots 
"""

parser = argparse.ArgumentParser(description = __doc__)
parser.add_argument('--file', type=str,help='input file') 
parser.add_argument('--run', type=str,default="",help='run number')
parser.add_argument('--t0', type=bool,default=False,help='t0 input file')
parser.add_argument('--ref', type=str,default="",help='reference file') 
parser.add_argument('--refrun', type=str,default="",help='reference run number for T0 file') 
args = parser.parse_args()

import ROOT 
from ROOT import TFile, TH1F, TH2,TGaxis
from ROOT import TCanvas, TLegend
from ROOT import SetOwnership

from TrigEgammaAnalysisTools.TrigEgammaDataQuality import TrigEgammaPlotHolder
from AtlasStyle import *

dqlist = {'Efficiency':['eff_et','eff_eta','eff_phi','eff_mu']}
dqlevel = ['HLT','L1Calo']
complist={'L1_EM20VH':'L1_EM20VHI',
        'L1_EM20VHI':'HLT_e24_lhtight_nod0_ivarloose',
        'L1_EM22VHI':'HLT_e26_lhtight_nod0_ivarloose',
        'HLT_e24_lhmedium_L1EM20VH':'HLT_e24_lhmedium_L1EM20VHI',
        'HLT_e24_lhmedium_L1EM20VHI':'HLT_e24_lhmedium_ivarloose',
        'HLT_e24_lhmedium_ivarloose':'HLT_e24_lhmedium_nod0_ivarloose',
        'HLT_e24_lhmedium_nod0_ivarloose':'HLT_e24_lhtight_nod0_ivarloose',
        }

#########################################################

def setCanvas(c):
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)
    c.SetHighLightColor(2)
    c.SetFillColor(0)
    c.SetBorderMode(0)
    c.SetBorderSize(2)
    c.SetTickx(1)
    c.SetTicky(1)
    c.SetLeftMargin(0.16)                                  
    c.SetRightMargin(0.05)                                     
    c.SetTopMargin(0.05)                                        
    c.SetFrameBorderMode(0)                                           
    c.SetFrameBorderMode(0)                                              

def makeCanvas(h1,trigger,name,run="",lumi=""):
    c = TCanvas()
    hname=h1.GetName()
    if('eff' in hname):
        h1.SetMaximum(1.4)
        h1.SetMinimum(0.)
        h1.GetYaxis().SetTitle("Efficiency")
    else:
        TGaxis.SetMaxDigits(2);
    
    h1.Draw()
    leg = TLegend(0.4,0.2,0.70,0.30)
    setLegend1(leg,h1,trigger)
    ATLASLabel(0.2,0.87,"Internal")
    ATLASRunNumberLabel(0.7,0.85,run)
    ATLASLumiLabel(0.2,0.85,lumi)
    oname="plot_Run_"+run+"_"+hname+"_"+trigger+"_"+name+".eps"
    #oname2="plot_Run_"+trigger+"_"+hname+"_"+name+".C"
    gPad.RedrawAxis()
    c.Print(oname)
    #c.SaveAs(oname2)
    return oname

def makeCanvas2(h1,h2,title,name,name2,run="",lumi="",run2=""):
    c = TCanvas()
    hname=h1.GetName()
    if('eff' in hname):
        h1.SetMaximum(1.4)
        h1.SetMinimum(0.)
        h1.GetYaxis().SetTitle("Efficiency")
        h2.SetMaximum(1.4)
        h2.SetMinimum(0.)
    else:
        TGaxis.SetMaxDigits(2);
    
    h1.SetMarkerStyle(24)
    h1.SetMarkerColor(1)
    h1.SetLineColor(1)
    h2.SetMarkerStyle(22)
    h2.SetMarkerColor(4)
    h2.SetLineColor(4)
    
    h1.Draw()
    h2.Draw("SAME")
    leg = TLegend(0.4,0.2,0.7,0.40)
    if not run2:
        setLegend2(leg,h1,h2,name,name2)
    else:
        setLegend2(leg,h1,h2,name,run2)
    ATLASLabel(0.2,0.85,"Internal")
    ATLASRunNumberLabel(0.7,0.85,run)
    ATLASLumiLabel(0.2,0.85,lumi)
    oname="plot_Run_"+run+"_"+hname+"_"+name+"_"+name2+".eps"
    #oname2="plot_Run_"+run+"_"+hname+"_"+name+"_"+name2+".C"
    gPad.RedrawAxis()
    c.Print(oname)
    #c.SaveAs(oname2)
    return oname

def setLegend1(leg,histo1,trigName):
   #TLegend *leg = new TLegend(0.40,0.25,0.75,0.35,NULL,"brNDC");
    leg.SetBorderSize(0)
    leg.SetTextFont(42)
    leg.SetTextSize(0.042)
    leg.SetLineColor(1)
    leg.SetLineStyle(1)
    leg.SetLineWidth(1)
    leg.SetFillColor(0)
    leg.SetFillStyle(0)
    leg.AddEntry(histo1,trigName,"p")
    leg.Draw()

def setLegend2(leg,histo1,histo2,trigName,trigName2):
   #TLegend *leg = new TLegend(0.40,0.25,0.75,0.35,NULL,"brNDC");
    leg.SetBorderSize(0)
    leg.SetTextFont(42)
    leg.SetTextSize(0.042)
    leg.SetLineColor(1)
    leg.SetLineStyle(1)
    leg.SetLineWidth(1)
    leg.SetFillColor(0)
    leg.SetFillStyle(0)
    leg.AddEntry(histo1,trigName,"p")
    leg.AddEntry(histo2,trigName2,"p")
    leg.Draw()

def montage(plotlist,name):
    print name
    plot_names_str = ' '.join(plotlist)
    cmd = 'montage -tile 2x2 -geometry 800x800+3+3 {0} {1}'.format(plot_names_str, name + '.pdf')
    subprocess.call(cmd, shell=True)
    cmd = 'tar -czf {0} *.eps --remove-files'.format(name + '.tar.gz')
    subprocess.call(cmd, shell=True)

def sortPlots(histos,path,holder):
    
    tname, tplot, run = parseName(path)
    holder.setRunNumber(run) 
    # The following is ridiculous 
    for l in dqlevel:
        if(l in tplot):
            holder.updateTriggers(tname)
            for t in dqlist.keys():
                if(t in tplot):
                    toplot=dqlist[t]
                    plotlist=[]
                    hlist=[]
                    for h in histos:
                        if(h.GetName() in toplot):
                            hlist.append(h)
                    holder.updatePlots(tname,tplot,hlist)
    

def parseName(fpath):
    dlist=fpath.split("/")
    tplot=""
    tname=""
    run=""
    flist=dlist[0].split(".")
    if(len(flist)>1):
        run=flist[1]
    #print run
    if("Shifter" in fpath):
        tname=dlist.pop()
    elif("Expert" in fpath):  
        tplot=dlist.pop()
        tplot+="_"+dlist.pop()
    for l in dlist:
        if("HLT_" in l):
            tname = l
        if("L1_" in l):
            tname = l
    return tname, tplot, run    

def objectsInDir(rdir,holder):
    histos=[]
    for key in rdir.GetListOfKeys():
        cl = key.GetClassName(); rcl = ROOT.TClass.GetClass(cl)
        if ' ' in key.GetName():
            print 'WARNING: cannot have spaces in histogram names for han config; not including %s %s' % (cl, key.GetName())
            continue
        if rcl.InheritsFrom('TDirectory'):
            recurse(key.ReadObj(),holder)
        elif rcl.InheritsFrom('TH1'):

            if '/' in key.GetName():
                print 'WARNING: cannot have slashes in histogram names, encountered in directory %s, histogram %s' % (rdir.GetPath(), key.GetName())
                continue
            if key.GetName() == 'summary':
                print 'WARNING: cannot have histogram named summary, encountered in %s' % rdir.GetPath()
                continue
            histos.append(key.ReadObj())
            fpath = rdir.GetPath()
            name = (fpath + '/' + key.GetName()).lstrip('/')
            #print name, fpath
    if(len(histos) > 0):        
        sortPlots(histos,fpath,holder)

def recurse(rdir,holder):
    for key in rdir.GetListOfKeys():
        cl = key.GetClassName(); rcl = ROOT.TClass.GetClass(cl); kname=key.GetName()
        if ' ' in kname:
            print 'WARNING: cannot have spaces in histogram names for han config; not including %s %s' % (cl, key.GetName())
            continue
        elif rcl.InheritsFrom('TDirectory'):
            objectsInDir(key.ReadObj(),holder)

def createTriggerPlots(holder):
    slist=[]
    for key in dqlist:
        for trigger in holder.Triggers:
            h1 = holder.getPlots(trigger,key) 
            print h1
            plist=[]
            for h in h1:
                pname1=makeCanvas(h,trigger,key,holder.RunNumber,holder.Lumi)
                plist.append(pname1)
                slist.append(pname1)
            #if(len(h1)>0):
                #montage(plist,trigger+'_'+key)       
    return slist

def createTriggerComparisonPlots(holder):
    slist=[]
    for key in dqlist:
        for trigger in holder.Triggers:
            h1 = holder.getPlots(trigger,key) 
            print h1
            if(trigger in complist):
                h2 = holder.getPlots(complist[trigger],key)
                print h2
            i=0
            pclist=[]
            for h in h1:
                if(trigger in complist):
                    pname2=makeCanvas2(h,h2[i],h.GetName(),trigger,complist[trigger],holder.RunNumber,holder.Lumi)
                    pclist.append(pname2)
                    slist.append(pname2)
                i+=1
            #if(len(pclist)>0):
                #montage(pclist,trigger+"_"+complist[trigger])   
    return slist           

def createReferencePlots(holder,rholder):
    slist=[]
    for key in dqlist:
        for trigger in holder.Triggers:
            h1 = holder.getPlots(trigger,key)
            h2 = rholder.getPlots(trigger,key) 
            print h1
            plist=[]
            i=0
            if h2:
                for h in h1:
                    pname1=makeCanvas2(h,h2[i],h.GetName(),trigger,"Ref",holder.RunNumber,holder.Lumi,rholder.RunNumber)
                    plist.append(pname1)
                    slist.append(pname1)
                    i+=1
    return slist

def createPlots(holder):
    slist1 = createTriggerPlots(holder)
    slist2 = createTriggerComparisonPlots(holder)
    slist=slist1+slist2
    mname='TrigEgammaDQSummary_Run_'+holder.RunNumber
    montage(slist,mname)


def process(infname, basepath, run="", rinfname="", rbasepath=""):
    lumi=""
    lbset=""
    if run:
        from TrigCostPython import TrigCostCool
        lbset=TrigCostCool.GetLumiblocks(int(run),-1,-1,"")
    
        if(lbset): 
            print "%-40s % 9.2g %-10s" % ("Recorded Luminosity:  ",lbset.GetRecordedLumi()/(1e6),"pb^{-1}")
            lumi=str(round(lbset.GetRecordedLumi()/(1e6),1))

    import re
    f = ROOT.TFile.Open(infname, 'READ')
    if not f.IsOpen():
        print 'ERROR: cannot open %s' % infname
        return
    topindir = f.Get(basepath)
    if not topindir:
        raise ValueError("Path %s doesn't exist in input file" % basepath)
    holder = TrigEgammaPlotHolder('holder')
    holder.setLumi(lumi)
    recurse(topindir,holder)
    #print holder.Efficiency
    print holder.RunNumber
    print holder.Triggers
    createPlots(holder)

    if rinfname:
        f2 = ROOT.TFile.Open(rinfname, 'READ')
        if not f2.IsOpen():
            print 'ERROR: cannot open %s' % rinfname
            return
        rtopindir = f2.Get(rbasepath)
        if not rtopindir:
            raise ValueError("Path %s doesn't exist in input file" % rbasepath)
        rholder = TrigEgammaPlotHolder('rholder')
        rholder.setLumi(lumi)
        #print rholder.Efficiency
        recurse(rtopindir,rholder)
        print rholder.RunNumber
        print rholder.Triggers
        slist=createReferencePlots(holder,rholder)
        mname='TrigEgammaDQSummary_Run_'+holder.RunNumber+'_refRun_'+rholder.RunNumber
        montage(slist,mname)
    
    if lbset:
        print "-------------------------------------------------------------------------"
        print "Lumiblock Info("+str(lbset.lbbeg)+","+str(lbset.lbend)+")"
        print "-------------------------------------------------------------------------"
        print
        print "%-40s % 9.5g %-10s" % ("Integrated Time:      ",lbset.GetTotalTime(),"seconds")
        print "%-40s % 9.5g %-10s" % ("Live Time:            ",lbset.GetLiveTime(),"seconds")
        print "%-40s % 9.2g %-10s" % ("Delivered Luminosity: ",lbset.GetDeliveredLumi()/(1e6),"pb^{-1}")
        print "%-40s % 9.2g %-10s" % ("Recorded Luminosity:  ",lbset.GetRecordedLumi()/(1e6),"pb^{-1}")
        print "%-39s % 9.3e %-10s" % ("Mean Instantaneous Luminosity:",lbset.GetDeliveredLumi()/lbset.GetTotalTime()*1e30,"1/(cm^2 s)")
        print "%-40s % 9.2e %-10s" % ("Live-Time-Averaged # Bunches:",lbset.GetLiveTimeAveragedBunches(),"")
        print "%-40s % 9.2e %-10s" % ("Integrated-Time-Averaged # Bunches:",lbset.GetIntegratedTimeAveragedBunches(),"")

def setDefaults():
    SetAtlasStyle()
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetPalette(1)

def main():
    
    setDefaults()
    
    if(args.t0):
        if not args.run:
            print 'Run number not given'
            sys.exit(2)
    
        run='run_'+args.run
        path=run+"/HLT/Egamma/Expert"
    else:
        path= "HLT/Egamma/Expert"
    
    if(args.ref):
        rpath=""
        if(args.refrun):
            rpath='run_'+args.refrun+'/HLT/Egamma/Expert'
        else:
            rpath='HLT/Egamma/Expert'
        process(args.file,path,args.run,args.ref,rpath)
    else:
        process(args.file,path,args.run)

if __name__=="__main__":
    #os.environ['TDAQ_ERS_NO_SIGNAL_HANDLERS']='1'
    main()

         
    
