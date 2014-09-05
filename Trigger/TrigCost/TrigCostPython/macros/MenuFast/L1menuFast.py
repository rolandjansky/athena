#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import getopt
from math import sqrt
import sys

#from RootUtils import PyROOTFixes
#PyROOTFixes.enable_tree_speedups()

import ROOT
vars={'EM':    2,
      'EMH':   2,
      'EMHI':  2,
      'TAU':   2,
      'TAUI':  2,
      'JET':   5,
      'FJ':    2,
      'FJE':   2,
      'MU':    2,
      'MUB':   2,
      'MUE':   2,
      'MUEC':   2,
      'MET':   1,
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
        if var=='XE': var='MET'
        Vthresh=False
        if 'V' in var:
            var=var.replace('V','')
            Vthresh=True
        if not var in vars and var!='HT' and var!='MHT': raise NameError('Do not how to interpret: '+thresh)
        self.var=var
        try:
            self.thresh=int(value)*1000
            if 'MU' in var:
                self.thresh-=1000
        except ValueError:
            raise ValueError('No valid threshold found in: '+thresh)
        if Vthresh:
            if self.thresh<15000:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<=0.8005||abs(%s_eta[%d])>2.0005)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1)
            else:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<=0.8005||abs(%s_eta[%d])>2.0005)+1000*(abs(%s_eta[%d])<=1.2005||abs(%s_eta[%d])>1.6005)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1)
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
  print " usage: %s [-1|-2|-3] [-p [-b] [-w]] [-l lumi] <item1> [item2] ... " % sys.argv[0]
  print "     -1,-2, -3 selects pileup level of 27, 54 or 81"
  print "     -p  use real pileup simulation for 14 TeV [default is combinatorial only]"
  print "       -b  veto first 12 BCIDs in each train as these are dominated by pedestal shifts"
  print "       -w  use 50ns MC instead 25ns MC for 14 TeV. Note it uses mu=54 for mu=27"
  sys.exit(1)

#cross sections are normalized for 1e34
xsecJF=1.2450e3*10
effJF=7.4759e-2

xsec8TeV=7.285e4*10
eff8TeV=1.3984e-3

xsec14TeV=7.9310e4*10
eff14TeV=3.4532e-3 

ebweight=0.05*0.96  # should be (1/pileup)

mcfudge=0.75

def main():
    lumi=1
    eb=Tuple([('sorted/eb.root',ebweight*xsec8TeV)])
    mc8TeV=Tuple([('sorted/low8TeV.root',mcfudge*xsec8TeV*(1-eff8TeV)),
                  ('sorted/high8TeV.root',mcfudge*xsec8TeV*eff8TeV),
                  ])
#    mc8TeV=Tuple([('sorted/low14TeV.root',mcfudge*xsec14TeV*(1-eff14TeV)),
#                   ('sorted/high14TeV_v2.root',mcfudge*xsec14TeV*eff14TeV),
#                  ])
    mc14TeV=Tuple([('sorted/low14TeV.root',mcfudge*xsec14TeV*(1-eff14TeV)),
                   ('sorted/high14TeV.root',mcfudge*xsec14TeV*eff14TeV),
                  ])
    try:
        opts, triggers = getopt.getopt(sys.argv[1:], "123pbwl:")
        if len(triggers)<1: raise ValueError
    except (getopt.GetoptError, ValueError):
        usage()
    pileup=0
    realMC=False
    bcidVeto=False
    use50ns=False
    for opt,arg in opts:
        if opt=='-l':
            lumi=float(arg)
            if lumi>1e30:
                lumi/=1e34
        if opt=='-1':
            pileup=1
        if opt=='-2':
            pileup=2
        if opt=='-3':
            pileup=3
        if opt=='-p': realMC=True
        if opt=='-w': use50ns=True
        if opt=='-b': bcidVeto=True
    if pileup:
        mu=25*pileup
        muMC=27*pileup  #slight inconsistency in how mu is defined
        print 'Added combinatorial pileup at mu=%d for 8 TeV MC' % muMC
        mc8TeV=Tuple([('sorted/mc8TeV_mu%d.root' % mu,mcfudge*xsec8TeV/mu)])
        if not realMC:
            print 'Added combinatorial pileup at mu=%d for 14 TeV MC' % muMC        
            mc14TeV=Tuple([('sorted/mc14TeV_mu%d.root' % mu,mcfudge*xsec14TeV/mu)])
        else:
            if use50ns and muMC==27: muMC=54 #no mu=27 MC for 50 ns
            bcspace='25ns'
            if use50ns: bcspace='50ns'
            ignorebc=""
            ignorebctext=""
            if bcidVeto:
                ignorebctext="(ignoring first 12 BCIDs)"
                ignorebc="_bcid"
            print 'Use simulated %s pileup at mu=%d for 14 TeV  MC %s' % (bcspace,muMC,ignorebctext)
            mc14TeV=Tuple([('sorted/mu%d_%s%s.root' % (muMC,bcspace,ignorebc),mcfudge*xsec14TeV/muMC)])

    menu={}
    for trigger in triggers:
        item=Item(trigger)
        menu[trigger]=item
    
    
    print 'Calculating data rates'
    ratesData=calcRates(menu,eb,lumi)

    print 'Calculating 8 TeV rates'
    rates8TeV=calcRates(menu,mc8TeV,lumi)

    print 'Calculating 14 TeV rates'
    rates14TeV=calcRates(menu,mc14TeV,lumi)




    ratesScaled={}
    dataScaled={}
    for trigger in triggers+['Total']:
        ratesScaled[trigger]=[0,0,0,0]
        dataScaled[trigger]=[0,0,0,0]
        if rates8TeV[trigger][0]:
            ratesScaled[trigger][0]=rates14TeV[trigger][0]/rates8TeV[trigger][0]
            ratesScaled[trigger][1]=(sqrt((rates14TeV[trigger][1]*rates8TeV[trigger][0])**2+
                                          (rates14TeV[trigger][0]*rates8TeV[trigger][1])**2)/
                                     (rates8TeV[trigger][0]**2))
            dataScaled[trigger][0]=ratesScaled[trigger][0]*ratesData[trigger][0]
            dataScaled[trigger][1]=ratesScaled[trigger][1]*ratesData[trigger][0]
        if rates8TeV[trigger][2]:
            ratesScaled[trigger][2]=rates14TeV[trigger][2]/rates8TeV[trigger][2]
            ratesScaled[trigger][3]=(sqrt((rates14TeV[trigger][3]*rates8TeV[trigger][2])**2+
                                          (rates14TeV[trigger][2]*rates8TeV[trigger][3])**2)/
                                     (rates8TeV[trigger][2]**2))
            dataScaled[trigger][2]=ratesScaled[trigger][2]*ratesData[trigger][2]
            dataScaled[trigger][3]=ratesScaled[trigger][3]*ratesData[trigger][2]
            
            

    print '%-20s  %10s  %10s  %10s  %10s   %10s' % ('Trigger','8 TeV Data','8 TeV MC','14 TeV MC',
                                           'MC Scaling','Data scaled to 14 TeV')

    #outfh=open('rates.csv','w')
    def printLine(*args):
        print '%-20s  %4.1f+-%4.1f  %4.1f+-%4.1f  %4.1f+-%4.1f  (%3.1f+-%3.1f)  %4.1f+-%4.1f' % args
        #outfh.write('%-20s,  %4.1f,+-,%4.1f,  %4.1f,+-,%4.1f,  %4.1f,+-,%4.1f,  %3.1f,+-,%3.1f,  %4.1f,+-,%4.1f\n' % args )
        
    for trigger in triggers+['Total']:
        if trigger=='Total':
            print '='*80
        printLine(
            trigger,
            ratesData[trigger][0],ratesData[trigger][1],
            rates8TeV[trigger][0],rates8TeV[trigger][1],
            rates14TeV[trigger][0],rates14TeV[trigger][1],
            ratesScaled[trigger][0],ratesScaled[trigger][1],
            dataScaled[trigger][0],dataScaled[trigger][1],
            )
        if trigger!='Total':
            printLine(
                '    Unique',
                ratesData[trigger][2],ratesData[trigger][3],
                rates8TeV[trigger][2],rates8TeV[trigger][3],
                rates14TeV[trigger][2],rates14TeV[trigger][3],
                ratesScaled[trigger][2],ratesScaled[trigger][3],
                dataScaled[trigger][2],dataScaled[trigger][3],
                )
        print
    
if __name__ == "__main__":
    main()
