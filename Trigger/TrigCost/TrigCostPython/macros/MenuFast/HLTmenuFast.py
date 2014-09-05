#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
import getopt
from math import sqrt
import sys, os

#from RootUtils import PyROOTFixes
#PyROOTFixes.enable_tree_speedups()

import ROOT
vars={'EM':    3,
      'EMH':   3,
      'EMHI':  3,
      'TAU':   2,
      'TAUI':  2,
      'MU':    2,
      'XE':    1,
      'JET':   8,
      'g':     2,
      'etight':   2,
      'eitight':  2,
      'emedium':  2,
      'eimedium': 2,
      'eloose':   2,
      'gloose':   2,
      'gloosewovlp': 2,
      'gmedium':     2,
      'gmediumwovlp':2,
      'taumedium': 2,
      'tauloose':  2,
      'mu':        2,
      'mui':       2,
      'mub':       2,
      'mue':       2,
      'muec':      2,
      'xe':        1,
      'EFxe':      1,
      'xeworandom':1,
      'EFjet':    4,
      'fatjet':   2,
      'jet':      8,
      'bjet':     2,
      'fwdjet':   2,
      'fwdejet':  2,
}

class Threshold:
    def __init__(self,thresh):
        var=''
        value=''
        self.mult=1
        if thresh[0].isdigit():
            self.mult=int(thresh[0])
            thresh=thresh[1:]
        for v in thresh:
            if v.isdigit():
                value+=v
            else:
                var+=v
        if var=='J':  var='JET'
        if var=='BJ': var='BJET'
        if var=='MET': var='XE'

        Vthresh=False
        if 'V' in var:
            var=var.replace('V','')
            Vthresh=True
        if not var in vars and var!='HT' and var!='MHT':
            raise NameError('Do not how to interpret : '+thresh)
        self.var=var
        try:
            self.thresh=int(value)*1000
            if 'MU' in var:
                self.thresh-=1000
            if 'mu' in var:
                self.thresh-=1000    
            if 'TAU' in var:
                self.thresh+=1500
            if 'tau' in var:        
                self.thresh+=2000   
        except ValueError:
            raise ValueError('No valid threshold found in: '+thresh)
        if Vthresh:
            if self.thresh<15000:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<0.8||abs(%s_eta[%d])>2.0)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1)
            else:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<0.8||abs(%s_eta[%d])>2.0)+1000*(abs(%s_eta[%d])<1.2||abs(%s_eta[%d])>1.6)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1)
        else:
            self.cut='%s_pt[%d]>%d' % ( self.var,self.mult-1,self.thresh)
        if var=='HT':
            self.cut='Sum$(JET_pt)>%d' % self.thresh
        if var=='MHT':
            self.cut='Sum$(JET_pt*cos(JET_phi))**2+Sum$(JET_pt*sin(JET_phi))**2>%d' % (self.thresh**2)

class Item:
    def __init__(self,item):
        self.thresholds=[]
        for thresh in item.split('_'):
            if thresh=='L1': continue
            self.thresholds.append(Threshold(thresh))

        self._cut='&&'.join([ ('(%s)' % thresh.cut) for thresh in self.thresholds])
    def cut(self):
        return self._cut

hist=ROOT.TH1F("hist","hist",100,0,1)
    
class Tuple:
    def __init__(self,files):
        #expect list of tuples pairs with patterns and weights
        if type(files)!=list:
            files=[files]
        self.tuples=[]
        self.weights=[]
        self.entries=[]
        self.offsets=[]
        offset=0
        for file in files:
            nt=ROOT.TChain("physics")
            nt.Add(file[0])
            self.tuples.append(nt)
            self.weights.append(file[1])
            self.offsets.append(offset)
            entries=nt.GetEntries()
            self.entries.append(entries)
            offset+=entries

    def passes(self,item):
        eventIDs=[]
        for nt,offset in zip(self.tuples,self.offsets):
            num=nt.Project("hist","Entry$",item.cut())
            v=nt.GetV1()
            for idx in range(num):
                eventIDs.append(int(v[idx])+offset)
        return eventIDs
    
    def eventWeights(self):
        weights=[]
        for nt,weight in zip(self.tuples,self.weights):
            nt.SetEstimate(3000000)
            ntweights=[]
            num=nt.Project("hist","weight","")
            v=nt.GetV1()
            for idx in range(num):
                ntweights.append(v[idx])
            sumWeights=sum(ntweights)
            for idx in range(num):
                weights.append(ntweights[idx]*weight/sumWeights)
        return weights
            
def calcRates(menu,tuple,lumi):
    rates={}
    passes={}
    for trigger in menu.keys()+['Total']:
        rates[trigger]=[0,0,0,0] #total rate, error,unique rate, error
    weights=tuple.eventWeights()
    for trigger,item in menu.iteritems():
        passes[trigger]=tuple.passes(item)
        #print "# of passed in : ",trigger, len(passes[trigger])
    total=set()
    for trigger in menu:
        rates[trigger][0]=lumi*sum([weights[event] for event in passes[trigger]])
        if len(passes[trigger]):
            rates[trigger][1]=lumi*sqrt(sum([weights[event]**2 for event in passes[trigger]]))
        total.update(passes[trigger])
        all=set()
        for other in menu:
            if other==trigger: continue
            all.update(passes[other])
        unique=set(passes[trigger]).difference(all)
        rates[trigger][2]=lumi*sum([weights[event] for event in unique])
        if len(unique):
            rates[trigger][3]=lumi*sqrt(sum([weights[event]**2 for event in unique]))
    rates['Total'][0]=lumi*sum([weights[event] for event in total])
    rates['Total'][1]=lumi*sqrt(sum([weights[event]**2 for event in total]))
    return rates
        
def usage():
    print " usage: %s [-s] [-f filename] [-l lumi] <item1> [item2] ... " % sys.argv[0]
    print "     -l Lumonisity i.e. 2e34"
    print "     -s Show the breakdown of MC rates into the MC samples "
    print "     -f Produce rate outputfile with the csv format under home directory. Specify the file name i.e. rates"
    sys.exit(1)

# cross sections are normalized for 1e34
xsec8TeVJF17=1.2450e3*10
eff8TeVJF17=7.4759e-2*(201243+201804+201273)/(5000.*5.*30.)/3. #805416./(5000.*5.*40.)  #(604273)/(5000.*5.*30.)/3.
xsec14TeVJF17=2.7006e3*10
eff14TeVJF17=7.7499E-02*(206615+206627+206620)/(5000.*5.*30.)/3.  #825943./(5000.*5.*40.)   #(206866+206557+206414)/(5000.*5.*30.)/3.

xsec8TeV=7.285e4*10
eff8TeV=1.3984e-3
xsec14TeV=7.9310e4*10
eff14TeV=3.4532e-3*(797613./1000000.)

xsecZee=2.0570e-3*10 
xsecZmumu=2.0580E-3*10 
xsecttbar=8.1226E-4*10 
xsecWplusenu=1.2160E-2*10 
xsecWminusenu=8.9770E-3*10 
xsecWplusmunu=1.2160E-2*10 
xsecWminusmunu=8.9770E-3*10

xsecWplusenu8TeV=6.8912E-3*10
xsecWminusenu8TeV=4.7904E-3*10
xsecWplusmunu8TeV=6.8912E-3*10
xsecWminusmunu8TeV=4.7904E-3*10
xsecttbar8TeV=2.0813E-4*10 
xsecZee8TeV=1.1099E-3*10  
xsecZmumu8TeV=1.1098E-3*10 

effZee=1.0
effZmumu=1.0
effttbar=5.4274E-01
effWplusenu=1.0
effWminusenu=1.0
effWplusmunu=1.0
effWminusmunu=1.0

effttbar8TeV=5.4263E-01 

ebweight=0.05*0.96  # should be (1/pileup)
ebeff=425810./(500.*992.) #(500.*1012.)   #433961./(500.*1012.)

mcfudge=0.80
xsec_ratio=1.

def main():
    lumi=2 
    sampledir='sorted_260813/'
    eb=Tuple([(sampledir+'eb_re1.root',ebweight*xsec8TeV*ebeff)])
    jf17mc14TeV=Tuple([(sampledir+'jf17_14TeV1_bcid_re1.root',mcfudge*xsec14TeVJF17*eff14TeVJF17),
                       (sampledir+'jf17_14TeV2_bcid_re1.root',mcfudge*xsec14TeVJF17*eff14TeVJF17),
                       (sampledir+'jf17_14TeV3_bcid_re1.root',mcfudge*xsec14TeVJF17*eff14TeVJF17),
                       ])
    jf17mc8TeV=Tuple([(sampledir+'jf17_8TeV1_bcid_re1.root',mcfudge*xsec8TeVJF17*eff8TeVJF17),
                      (sampledir+'jf17_8TeV2_bcid_re1.root',mcfudge*xsec8TeVJF17*eff8TeVJF17),
                      (sampledir+'jf17_8TeV3_bcid_re1.root',mcfudge*xsec8TeVJF17*eff8TeVJF17),
                      ])
    Zee_tuple=Tuple([(sampledir+'/Zee_bcid_re1.root',mcfudge*xsecZee*effZee)])
    Zmumu_tuple=Tuple([(sampledir+'/Zmumu_bcid_re1.root',mcfudge*xsecZmumu*effZmumu)])
    Wplusenu_tuple=Tuple([(sampledir+'/Wplusenu_bcid_re1.root',mcfudge*xsecWplusenu*effWplusenu)])
    Wplusmunu_tuple=Tuple([(sampledir+'/Wplusmunu_bcid_re1.root',mcfudge*xsecWplusmunu*effWplusmunu)])
    Wminusenu_tuple=Tuple([(sampledir+'/Wminusenu_bcid_re1.root',mcfudge*xsecWminusenu*effWminusenu)])
    Wminusmunu_tuple=Tuple([(sampledir+'/Wminusmunu_bcid_re1.root',mcfudge*xsecWminusmunu*effWminusmunu)])
    ttbar_tuple=Tuple([(sampledir+'/ttbar_bcid_re1.root',mcfudge*xsecttbar*effttbar)])

    Wplusenu8TeV_tuple=Tuple([(sampledir+'/Wplusenu_8TeV_bcid_re1.root',mcfudge*xsecWplusenu8TeV)])
    Wminusenu8TeV_tuple=Tuple([(sampledir+'/Wminusenu_8TeV_bcid_re1.root',mcfudge*xsecWminusenu8TeV)])
    Wplusmunu8TeV_tuple=Tuple([(sampledir+'/Wplusmunu_8TeV_bcid_re1.root',mcfudge*xsecWplusmunu8TeV)])
    Wminusmunu8TeV_tuple=Tuple([(sampledir+'/Wminusmunu_8TeV_bcid_re1.root',mcfudge*xsecWminusmunu8TeV)])
    ttbar8TeV_tuple=Tuple([(sampledir+'/ttbar_8TeV_bcid_re1.root',mcfudge*xsecttbar8TeV*effttbar8TeV)]) 
    Zee8TeV_tuple=Tuple([(sampledir+'/Zee_8TeV_bcid_re1.root',mcfudge*xsecZee8TeV)])
    Zmumu8TeV_tuple=Tuple([(sampledir+'/Zmumu_8TeV_bcid_re1.root',mcfudge*xsecZmumu8TeV)])  
    try:
        opts, triggers = getopt.getopt(sys.argv[1:], "sf:l:")
        if len(triggers)<1: raise ValueError
    except (getopt.GetoptError, ValueError):
        usage()

    bcidVeto=True 
    doPlot=False
    doShowRateBreakdown=False
    doOutputFile=False
    OutputFilename='rates'
    for opt,arg in opts:
        if opt=='-l':
            lumi=float(arg)
            if lumi>1e30:
                lumi/=1e34
        if opt=='-s': doShowRateBreakdown=True
        if opt=='-f':
            doOutputFile=True
            OutputFilename=os.environ['HOME']+"/"+str(arg)
    print ("Assuming Lumi=%0.fe34 bs=25ns mu=60"% lumi)    
    if bcidVeto==True: print "Veto first 12 BCIDs in each train because these are dominated by pedestal shifts and will be corrected on L1Calo."
    
    menu={}
    for trigger in triggers:
        item=Item(trigger)
        menu[trigger]=item
    
    print 'Calculating EB data rates'
    ratesData=calcRates(menu,eb,lumi)
    
    print 'Calculating 8 TeV rates by Jets, W, Z, and ttbar'
    rates8TeVJF17=calcRates(menu,jf17mc8TeV,lumi)
    Wplusenu_8TeV=calcRates(menu,Wplusenu8TeV_tuple,lumi)
    Wminusenu_8TeV=calcRates(menu,Wminusenu8TeV_tuple,lumi)
    Wplusmunu_8TeV=calcRates(menu,Wplusmunu8TeV_tuple,lumi)
    Wminusmunu_8TeV=calcRates(menu,Wminusmunu8TeV_tuple,lumi)
    Zee_8TeV=calcRates(menu,Zee8TeV_tuple,lumi)
    Zmumu_8TeV=calcRates(menu,Zmumu8TeV_tuple,lumi)
    ttbar_8TeV=calcRates(menu,ttbar8TeV_tuple,lumi)
    
    #print 'Calculating 14 TeV rates by minbias'
    #rates14TeV=calcRates(menu,mc14TeV,lumi)
        
    print 'Calculating 14 TeV rates by Jets, W, Z, and ttbar'
    rates14TeVJF17=calcRates(menu,jf17mc14TeV,lumi)
    Wplusenu_14TeV=calcRates(menu,Wplusenu_tuple,lumi)
    Wplusmunu_14TeV=calcRates(menu,Wplusmunu_tuple,lumi)
    Wminusenu_14TeV=calcRates(menu,Wminusenu_tuple,lumi)
    Wminusmunu_14TeV=calcRates(menu,Wminusmunu_tuple,lumi)
    Zee_14TeV=calcRates(menu,Zee_tuple,lumi)
    Zmumu_14TeV=calcRates(menu,Zmumu_tuple,lumi)
    ttbar_14TeV=calcRates(menu,ttbar_tuple,lumi)
    
    datarate8TeV=ratesData

    rateJF17_8TeV={}
    rateJF17_14TeV={}
    ratesScaledJets={}
    dataScaled={}
    ratesW8TeV={}
    ratesW14TeV={}
    ratesZ8TeV={}
    ratesZ14TeV={}
    ratesttbar8TeV={}
    ratesttbar14TeV={}

    rateWZel8TeV={}
    rateWZmu8TeV={}
    rateWZel14TeV={}
    rateWZmu14TeV={}
    
    Jets_8TeV={}
    Jets_14TeV={}
    
    for trigger in triggers + ['Total']:
        # Calculating rates for W/Z/ttbar at 8/14 TeV based on MCs
        ratesW8TeV[trigger] =[(r1+r2+r3+r4) for (r1,r2,r3,r4) in zip(Wplusenu_8TeV[trigger], Wplusmunu_8TeV[trigger], Wminusenu_8TeV[trigger], Wminusmunu_8TeV[trigger])]
        ratesW14TeV[trigger]=[(r1+r2+r3+r4) for (r1,r2,r3,r4) in zip(Wplusenu_14TeV[trigger], Wplusmunu_14TeV[trigger], Wminusenu_14TeV[trigger], Wminusmunu_14TeV[trigger])]
        
        rateWZel8TeV[trigger] =[(r1+r2+r3) for (r1,r2,r3) in zip(Wplusenu_8TeV[trigger], Wminusenu_8TeV[trigger], Zee_8TeV[trigger])]
        rateWZel14TeV[trigger] =[(r1+r2+r3) for (r1,r2,r3) in zip(Wplusenu_14TeV[trigger], Wminusenu_14TeV[trigger], Zee_14TeV[trigger])] 
        
        ratesZ8TeV[trigger] =[(r1+r2) for (r1,r2) in zip(Zee_8TeV[trigger], Zmumu_8TeV[trigger])]
        ratesZ14TeV[trigger]=[(r1+r2) for (r1,r2) in zip(Zee_14TeV[trigger], Zmumu_14TeV[trigger])]
                
        ratesttbar8TeV[trigger] =[r for r in ttbar_8TeV[trigger]]
        ratesttbar14TeV[trigger]=[r for r in ttbar_14TeV[trigger]]

        # Calculating rates for JF17 at 8/14 TeV, correcting W/Z contibutions 
        rateJF17_8TeV[trigger] =[(r1+(r2)*(1.-xsec_ratio)) for r1,r2 in zip(rates8TeVJF17[trigger], rateWZel8TeV[trigger])]
        rateJF17_14TeV[trigger]=[(r1+(r2)*(1.-xsec_ratio)) for r1,r2 in zip(rates14TeVJF17[trigger], rateWZel14TeV[trigger])] if rates14TeVJF17[trigger][0]>0. else [0,0,0,0]
        
        ratesScaledJets[trigger]=[2.0,0.2,2.0,0.2]
        dataScaled[trigger]=[0,0,0,0]

        Jets_8TeV[trigger]=[0,0,0,0]
        Jets_14TeV[trigger]=[0,0,0,0]

        MinScaled=1.5
        MaxScaledError=0.3
        MaxScaled=6.
        # Scaling from 8 TeV to 14 TeV
        if rateJF17_8TeV[trigger][0]>0. and rateJF17_14TeV[trigger][0]>0.:

            Jets_8TeV[trigger][0]  = rateJF17_8TeV[trigger][0] -rateWZel8TeV[trigger][0]
            Jets_14TeV[trigger][0] = rateJF17_14TeV[trigger][0]-rateWZel14TeV[trigger][0]

            Jets_8TeV[trigger][1]  = sqrt(rateJF17_8TeV[trigger][1]**2+rateWZel8TeV[trigger][1]**2)
            Jets_14TeV[trigger][1] = sqrt(rateJF17_14TeV[trigger][1]**2+rateWZel14TeV[trigger][1]**2)
            
            ratesScaledJets[trigger][0] = Jets_14TeV[trigger][0]/Jets_8TeV[trigger][0]
            ratesScaledJets[trigger][1] = sqrt( (Jets_14TeV[trigger][0]**2) * (Jets_8TeV[trigger][1]**2) + (Jets_14TeV[trigger][1]**2) * (Jets_8TeV[trigger][0]**2) )/ (Jets_8TeV[trigger][0]**2)

            if ratesScaledJets[trigger][0]>MaxScaled:
                ratesScaledJets[trigger][0]=MaxScaled
            if ratesScaledJets[trigger][0]<MinScaled:
                ratesScaledJets[trigger][0]=MinScaled
            if ratesScaledJets[trigger][1]>MaxScaledError*ratesScaledJets[trigger][0]:
                ratesScaledJets[trigger][1]=MaxScaledError*ratesScaledJets[trigger][0]
                
        dataScaled[trigger][0] = ratesScaledJets[trigger][0]*\
                                (datarate8TeV[trigger][0]-ratesW8TeV[trigger][0]-ratesZ8TeV[trigger][0]-ratesttbar8TeV[trigger][0])+\
                                 ratesW14TeV[trigger][0]+ratesZ14TeV[trigger][0]+ratesttbar14TeV[trigger][0]
        dataScaled[trigger][1] = sqrt( (ratesScaledJets[trigger][1]*\
                                       (datarate8TeV[trigger][0]-ratesW8TeV[trigger][0]-ratesZ8TeV[trigger][0]-ratesttbar8TeV[trigger][0]))**2+\
                                        ratesW14TeV[trigger][1]**2 +ratesZ14TeV[trigger][1]**2 +ratesttbar14TeV[trigger][1]**2 )\

        if rateJF17_8TeV[trigger][2]>0. and rateJF17_14TeV[trigger][2]>0.:
            Jets_8TeV[trigger][2]  = rateJF17_8TeV[trigger][2] -rateWZel8TeV[trigger][2]
            Jets_14TeV[trigger][2] = rateJF17_14TeV[trigger][2]-rateWZel14TeV[trigger][2]

            Jets_8TeV[trigger][3]  = sqrt(rateJF17_8TeV[trigger][3]**2 +rateWZel8TeV[trigger][3]**2)
            Jets_14TeV[trigger][3] = sqrt(rateJF17_14TeV[trigger][3]**2+rateWZel14TeV[trigger][3]**2)
            
            ratesScaledJets[trigger][2]= Jets_14TeV[trigger][2]/Jets_8TeV[trigger][2]
            ratesScaledJets[trigger][3] = sqrt( (Jets_14TeV[trigger][2]**2) * (Jets_8TeV[trigger][3]**2)\
                                              + (Jets_14TeV[trigger][3]**2) * (Jets_8TeV[trigger][2]**2) )/ (Jets_8TeV[trigger][2]**2)


            if ratesScaledJets[trigger][2]>MaxScaled:
                ratesScaledJets[trigger][2]=MaxScaled
            if ratesScaledJets[trigger][2]<MinScaled:
                ratesScaledJets[trigger][2]=MinScaled
            if ratesScaledJets[trigger][3]>MaxScaledError*ratesScaledJets[trigger][2]:
                ratesScaledJets[trigger][3]=MaxScaledError*ratesScaledJets[trigger][2]
                
        dataScaled[trigger][2] = ratesScaledJets[trigger][2]*\
                                (datarate8TeV[trigger][2]-ratesW8TeV[trigger][2]-ratesZ8TeV[trigger][2]-ratesttbar8TeV[trigger][2])+\
                                 ratesW14TeV[trigger][2] +ratesZ14TeV[trigger][2] +ratesttbar14TeV[trigger][2]
        dataScaled[trigger][3] = sqrt( (ratesScaledJets[trigger][3]*\
                                       (datarate8TeV[trigger][2]-ratesW8TeV[trigger][2]-ratesZ8TeV[trigger][2]-ratesttbar8TeV[trigger][2]))**2+\
                                        ratesW14TeV[trigger][3]**2 +ratesZ14TeV[trigger][3]**2 +ratesttbar14TeV[trigger][3]**2 )



    if doOutputFile:
        print 'Saving the output to '+OutputFilename+'.csv'
        outfh=open(OutputFilename+'.csv','w')
        outfh.write('%-20s,   %10s,  %10s,  %7s,  %10s,  %10s\n' % ('Trigger [Hz]:', '8 TeV Data', '8 TeV MC '+(', Jets, W, Z, ttbar' if doShowRateBreakdown else ''), 'Scaling Jet','14 TeV MC'+(', Jets, W, Z, ttbar' if doShowRateBreakdown else ''), 'Data Scaled 14 TeV'))

    print '%-35s  %8s %8s%5s %8s %8s\n' % ('Trigger [Hz]:', '8 TeV Data |', '8 TeV MC '+('  [ Jet,          W,            Z,          ttbar ]' if doShowRateBreakdown else ''), '(Scaling Jet)','| 14 TeV MC'+('  [ Jet,         W,             Z,          ttbar ]' if doShowRateBreakdown else ''), ' | Data Scaled 14 TeV')
        
    def printLine(*args):
        #print      '%-35s %4.1f+-%4.1f   %4.1f+-%4.1f  (%4.1f+-%4.1f)  %4.1f+-%4.1f  %4.1f+-%4.1f' % args
        print      ('%-35s %5.1f+-%4.1f   %5.1f+-%4.1f '+(('['+'%5.1f+-%4.1f  '*4+']') if doShowRateBreakdown else '')+ ' (%2.1f+-%2.1f)  %5.1f+-%4.1f '+(('['+'%5.1f+-%4.1f  '*4+']') if doShowRateBreakdown else '')+'  %5.1f+-%4.1f') % args
        if doOutputFile: outfh.write(('%s:, %5.1f+-%4.1f,  %5.1f+-%4.1f, '+((' %5.1f+-%4.1f,'*4) if doShowRateBreakdown else '')+' (%5.1f+-%4.1f), %5.1f+-%4.1f, '+((' %5.1f+-%4.1f, '*4) if doShowRateBreakdown else '')+'%5.1f+-%4.1f\n') % args )

    # Should modify total rates
    for trigger in triggers+['Total']:
        if trigger=='Total':
            print '='*(120+ (90 if doShowRateBreakdown else 0))
            
        if doShowRateBreakdown:
            printLine(
                trigger,
                ratesData[trigger][0]*1000,   ratesData[trigger][1]*1000,
                (Jets_8TeV[trigger][0] if Jets_8TeV[trigger][0]>0. else 0.) *1000+ratesW8TeV[trigger][0]*1000+ratesZ8TeV[trigger][0]*1000+ratesttbar8TeV[trigger][0]*1000,
                sqrt( (Jets_8TeV[trigger][1] if Jets_8TeV[trigger][1]>0. else 0.)**2+ratesW8TeV[trigger][1]**2+ratesZ8TeV[trigger][1]**2+ratesttbar8TeV[trigger][1]**2)*1000,

                Jets_8TeV[trigger][0]*1000,      Jets_8TeV[trigger][1]*1000,
                ratesW8TeV[trigger][0]*1000,     ratesW8TeV[trigger][1]*1000,
                ratesZ8TeV[trigger][0]*1000,     ratesZ8TeV[trigger][1]*1000,
                ratesttbar8TeV[trigger][0]*1000, ratesttbar8TeV[trigger][1]*1000,
                
                ratesScaledJets[trigger][0],  ratesScaledJets[trigger][1],
                (Jets_14TeV[trigger][0] if Jets_14TeV[trigger][0]>0. else 0.)*1000+ratesW14TeV[trigger][0]*1000+ratesZ14TeV[trigger][0]*1000+ratesttbar14TeV[trigger][0]*1000,
                sqrt( (Jets_14TeV[trigger][1] if Jets_14TeV[trigger][1]>0. else 0. )**2+ratesW14TeV[trigger][1]**2+ratesZ14TeV[trigger][1]**2+ratesttbar14TeV[trigger][1]**2)*1000,

                Jets_14TeV[trigger][0]*1000,      Jets_14TeV[trigger][1]*1000,
                ratesW14TeV[trigger][0]*1000,     ratesW14TeV[trigger][1]*1000,
                ratesZ14TeV[trigger][0]*1000,     ratesZ14TeV[trigger][1]*1000,
                ratesttbar14TeV[trigger][0]*1000, ratesttbar14TeV[trigger][1]*1000,
                
                dataScaled[trigger][0]*1000,dataScaled[trigger][1]*1000,
                )
        else:
            printLine(
                trigger,
                ratesData[trigger][0]*1000,   ratesData[trigger][1]*1000,
                (Jets_8TeV[trigger][0] if Jets_8TeV[trigger][0]>0. else 0.) *1000+ratesW8TeV[trigger][0]*1000+ratesZ8TeV[trigger][0]*1000+ratesttbar8TeV[trigger][0]*1000,
                sqrt( (Jets_8TeV[trigger][1] if Jets_8TeV[trigger][1]>0. else 0.)**2+ratesW8TeV[trigger][1]**2+ratesZ8TeV[trigger][1]**2+ratesttbar8TeV[trigger][1]**2)*1000,
                ratesScaledJets[trigger][0],  ratesScaledJets[trigger][1],
                (Jets_14TeV[trigger][0] if Jets_14TeV[trigger][0]>0. else 0.)*1000+ratesW14TeV[trigger][0]*1000+ratesZ14TeV[trigger][0]*1000+ratesttbar14TeV[trigger][0]*1000,
                sqrt( (Jets_14TeV[trigger][1] if Jets_14TeV[trigger][1]>0. else 0. )**2+ratesW14TeV[trigger][1]**2+ratesZ14TeV[trigger][1]**2+ratesttbar14TeV[trigger][1]**2)*1000,
                dataScaled[trigger][0]*1000,dataScaled[trigger][1]*1000,
                )
        if trigger!='Total':
            if doShowRateBreakdown:
                printLine(
                    '  Unique',
                    ratesData[trigger][2]*1000,   ratesData[trigger][3]*1000,
                    (Jets_8TeV[trigger][2] if Jets_8TeV[trigger][2]>0. else 0.)*1000+ratesW8TeV[trigger][2]*1000+ratesZ8TeV[trigger][2]*1000+ratesttbar8TeV[trigger][2]*1000,
                    sqrt( (Jets_8TeV[trigger][3] if Jets_8TeV[trigger][3]>0. else 0.)**2+ratesW8TeV[trigger][3]**2+ratesZ8TeV[trigger][3]**2+ratesttbar8TeV[trigger][3]**2)*1000,
                    Jets_8TeV[trigger][2]*1000,      Jets_8TeV[trigger][3]*1000,
                    ratesW8TeV[trigger][2]*1000,     ratesW8TeV[trigger][3]*1000,
                    ratesZ8TeV[trigger][2]*1000,     ratesZ8TeV[trigger][3]*1000,
                    ratesttbar8TeV[trigger][2]*1000, ratesttbar8TeV[trigger][3]*1000,
                    ratesScaledJets[trigger][2],  ratesScaledJets[trigger][3],
                    (Jets_14TeV[trigger][2] if Jets_14TeV[trigger][2]>0. else 0.)*1000+ratesW14TeV[trigger][2]*1000+ratesZ14TeV[trigger][2]*1000+ratesttbar14TeV[trigger][2]*1000,
                    sqrt( (Jets_14TeV[trigger][3] if Jets_14TeV[trigger][3]>0. else 0.)**2+ratesW14TeV[trigger][3]**2+ratesZ14TeV[trigger][3]**2+ratesttbar14TeV[trigger][3]**2)*1000.,
                    Jets_14TeV[trigger][2]*1000,      Jets_14TeV[trigger][3]*1000,
                    ratesW14TeV[trigger][2]*1000,     ratesW14TeV[trigger][3]*1000,
                    ratesZ14TeV[trigger][2]*1000,     ratesZ14TeV[trigger][3]*1000,
                    ratesttbar14TeV[trigger][2]*1000, ratesttbar14TeV[trigger][3]*1000,
                    dataScaled[trigger][2]*1000, dataScaled[trigger][3]*1000,
                    )
            else:
                printLine(
                    '  Unique',
                    ratesData[trigger][2]*1000,
                    ratesData[trigger][3]*1000,
                    (Jets_8TeV[trigger][2] if Jets_8TeV[trigger][2]>0. else 0.)*1000+ratesW8TeV[trigger][2]*1000+ratesZ8TeV[trigger][2]*1000+ratesttbar8TeV[trigger][2]*1000,
                    sqrt( (Jets_8TeV[trigger][3] if Jets_8TeV[trigger][3]>0. else 0.)**2+ratesW8TeV[trigger][3]**2+ratesZ8TeV[trigger][3]**2+ratesttbar8TeV[trigger][3]**2)*1000,
                    ratesScaledJets[trigger][2],  ratesScaledJets[trigger][3],
                    (Jets_14TeV[trigger][2] if Jets_14TeV[trigger][2]>0. else 0.)*1000+ratesW14TeV[trigger][2]*1000+ratesZ14TeV[trigger][2]*1000+ratesttbar14TeV[trigger][2]*1000,
                    sqrt( (Jets_14TeV[trigger][3] if Jets_14TeV[trigger][3]>0. else 0.)**2+ratesW14TeV[trigger][3]**2+ratesZ14TeV[trigger][3]**2+ratesttbar14TeV[trigger][3]**2)*1000.,
                    dataScaled[trigger][2]*1000,
                    dataScaled[trigger][3]*1000,
                    )

if __name__ == "__main__":
    main()


