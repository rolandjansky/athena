#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import getopt
from math import sqrt
import sys

#from RootUtils import PyROOTFixes
#PyROOTFixes.enable_tree_speedups()

import ROOT

ROOT.TFormula.SetMaxima(10000,10000,10000)

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
        self.doEtaCut = False
        threshList = thresh.split(".")
        thresh = threshList[0]
        if len(threshList) > 1:
            etaString = threshList[1]
            if "ETA" in etaString:
                self.doEtaCut = True
                lowerEtaCut = etaString.split("ETA")[0]
                upperEtaCut = etaString.split("ETA")[1]
                self.lowerEtaCut = float(lowerEtaCut[0]+"."+lowerEtaCut[1:]) if lowerEtaCut != '' else -1  #does it need to be self???
                self.upperEtaCut = float(upperEtaCut[0]+"."+upperEtaCut[1:]) if upperEtaCut != '' else -1            
        
        if thresh[0].isdigit():  #multiplicity
            self.mult=int(thresh[0])
            thresh=thresh[1:] # cut away the multiplicity now that is was read in
        for v in thresh:  
            if v.isdigit():  # get the threshold value
                value+=v
            else:            # get the rest of the item name
                var+=v
        if var=='J':   var='JET'
        if var=='XE': var='MET'
        self.Vthresh=False
        if 'V' in var:        # check for V and remove from name
            var=var.replace('V','')
            self.Vthresh=True
        if not var in vars and var!='HT' and var!='HTC' and var!='MHT' and var!='ACCEPTALL': raise NameError('Do not know how to interpret: '+thresh)
        self.var=var #ok assign it
        try:
            self.thresh=int(value)*1000 #make MeV
            if 'MU' in var:
                self.thresh-=1000
        except ValueError:
            raise ValueError('No valid threshold found in: '+thresh)

        self.cutAnyIdx = '' # need this to do combinatorics for topo cuts
        self.cutAnyIdxTree = ''

        etaCutList = []
        etaCutListAnyIdx = []
        etaCutListAnyIdxTree = []
        if self.doEtaCut:
            if self.lowerEtaCut != -1:
                etaCutList += ["fabs(%s_eta[%i]) > %s" % (self.var,self.mult-1,self.lowerEtaCut)]
                etaCutListAnyIdx += ["fabs(%s_eta[X]) > %s" % (self.var,self.lowerEtaCut)]
                etaCutListAnyIdxTree += ["abs(nt.%s_eta[X]) > %s" % (self.var,self.lowerEtaCut)]
            if self.upperEtaCut != -1:
                etaCutList += ["fabs(%s_eta[%i]) < %s" % (self.var,self.mult-1,self.upperEtaCut)]
                etaCutListAnyIdx += ["fabs(%s_eta[X]) < %s" % (self.var,self.upperEtaCut)]
                etaCutListAnyIdxTree += ["abs(nt.%s_eta[X]) < %s" % (self.var,self.upperEtaCut)]

        #self.etaCut = '&&'.join(etaCutList)
        self.etaCutAnyIdx = '&&'.join(etaCutListAnyIdx)
        self.etaCutAnyIdxTree = '&&'.join(etaCutListAnyIdxTree)

        if self.Vthresh: # do the V
            if self.thresh<15000:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<=0.8005||abs(%s_eta[%d])>2.0005)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1)
                self.cutAnyIdx='%s_pt[X]>%d+1000*(abs(%s_eta[X])<=0.8005||abs(%s_eta[X])>2.0005)' % ( self.var,self.thresh,self.var,self.var)
                self.cutAnyIdxTree='nt.%s_pt[X]>%d+1000*(abs(nt.%s_eta[X])<=0.8005||abs(nt.%s_eta[X])>2.0005)' % ( self.var,self.thresh,self.var,self.var)
            else:
                self.cut='%s_pt[%d]>%d+1000*(abs(%s_eta[%d])<=0.8005||abs(%s_eta[%d])>2.0005)+1000*(abs(%s_eta[%d])<=1.2005||abs(%s_eta[%d])>1.6005)' % ( self.var,self.mult-1,self.thresh,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1,self.var,self.mult-1)
                self.cutAnyIdx='%s_pt[X]>%d+1000*(abs(%s_eta[X])<=0.8005||abs(%s_eta[X])>2.0005)+1000*(abs(%s_eta[X])<=1.2005||abs(%s_eta[X])>1.6005)' % ( self.var,self.thresh,self.var,self.var,self.var,self.var)
                self.cutAnyIdxTree='nt.%s_pt[X]>%d+1000*(abs(nt.%s_eta[X])<=0.8005||abs(nt.%s_eta[X])>2.0005)+1000*(abs(nt.%s_eta[X])<=1.2005||abs(nt.%s_eta[X])>1.6005)' % ( self.var,self.thresh,self.var,self.var,self.var,self.var)

        else:
            self.cut='%s_pt[%d]>%d' % ( self.var,self.mult-1,self.thresh)
            self.cutAnyIdx='%s_pt[X]>%d' % ( self.var,self.thresh)
            self.cutAnyIdxTree='nt.%s_pt[X]>%d' % ( self.var,self.thresh)
            if var =='MET':
                self.cutAnyIdxTree='nt.%s_pt>%d' % ( self.var,self.thresh)
            
        if var=='HT':
            self.cut='Sum$(JET_pt)>%d' % self.thresh

        if var=='HTC':
            self.cut='((JET_pt[0]*(JET_pt[0]>20000 && fabs(JET_eta[0]) < 2.5))+(JET_pt[1]*(JET_pt[1]>20000 && fabs(JET_eta[1]) < 2.5))+(JET_pt[2]*(JET_pt[2]>20000 && fabs(JET_eta[2]) < 2.5))+(JET_pt[3]*(JET_pt[3]>20000 && fabs(JET_eta[3]) < 2.5))+(JET_pt[4]*(JET_pt[4]>20000 && fabs(JET_eta[4]) < 2.5)))>%d ' % self.thresh 
            
        if var=='MHT':
            self.cut='Sum$(JET_pt*cos(JET_phi))**2+Sum$(JET_pt*sin(JET_phi))**2>%d' % (self.thresh**2)

        if var =='ACCEPTALL':
            self.cut='1'

    def etaCut(self,nt):
        acceptedObjects = 0
        for idx in range(0,vars[self.var]):
            pt = getattr(nt,self.var+"_pt")[idx] if not self.var=="MET" else getattr(nt,self.var+"_pt")
            eta = getattr(nt,self.var+"_eta")[idx] if not self.var=="MET" else getattr(nt,self.var+"_eta")
            phi = getattr(nt,self.var+"_phi")[idx] if not self.var=="MET" else getattr(nt,self.var+"_phi")
            if pt == 0 and eta == 0 and phi ==0:  # if no entry eta is 0!
                continue
            if not eval(self.cutAnyIdxTree.replace("[X]","[%i]" % (idx))):
                continue
            if self.lowerEtaCut != -1 and not self.upperEtaCut != -1:
                if abs(eta) > float(self.lowerEtaCut):
                    acceptedObjects += 1
            elif not self.lowerEtaCut != -1 and self.upperEtaCut != -1:
                if abs(eta) < float(self.upperEtaCut):
                    acceptedObjects += 1
            elif self.lowerEtaCut != -1 and self.upperEtaCut != -1:
                if abs(eta) > float(self.lowerEtaCut) and abs(eta) < float(self.upperEtaCut):
                    acceptedObjects += 1
            if acceptedObjects >= self.mult:
                return True
        return False

class TopoCut:
    def __init__(self,topo,thresholds):


        self.doLoopCut = False
        self.cut = ""
        self.all = False
        
        # parse topo selection: Example: topo="03DETA04-1EM3VHI-1J30"
        
        topo = topo.split("-")
        topoCut      = topo[0]
        topoElement1 = topo[1]
        topoElement2 = topo[2]

        if "ALL" in topoCut:
            self.all = True
            topoCut = topoCut.replace("ALL","")
            self.doLoopCut = True

        if "DETA" in topoCut:
            self.topoType = "DETA"
            lowerCut = topoCut.split(self.topoType)[0]
            upperCut = topoCut.split(self.topoType)[1]
            self.lowerCut = float(lowerCut[0]+"."+lowerCut[1:]) if lowerCut != '' else -1
            self.upperCut = float(upperCut[0]+"."+upperCut[1:]) if upperCut != '' else -1
      
        elif "DPHI" in topoCut:
            self.topoType = "DPHI"
            lowerCut = topoCut.split(self.topoType)[0]
            upperCut = topoCut.split(self.topoType)[1]
            self.lowerCut = float(lowerCut[0]+"."+lowerCut[1:]) if lowerCut != '' else -1
            self.upperCut = float(upperCut[0]+"."+upperCut[1:]) if upperCut != '' else -1
            
        elif "DR" in topoCut:
            self.topoType = "DR"
            lowerCut = topoCut.split(self.topoType)[0]
            upperCut = topoCut.split(self.topoType)[1]
            self.lowerCut = float(lowerCut[0]+"."+lowerCut[1:]) if lowerCut != '' else -1
            self.upperCut = float(upperCut[0]+"."+upperCut[1:]) if upperCut != '' else -1

        elif "INVM" in topoCut:
            self.topoType = "INVM"
            lowerCut = topoCut.split(self.topoType)[0]
            upperCut = topoCut.split(self.topoType)[1]
            self.lowerCut = 1000*int(lowerCut) if lowerCut != '' else -1
            self.upperCut = 1000*int(upperCut) if upperCut != '' else -1

        else:
            raise NameError('Do not know how to interpret topo selection: '+topoCut)

        topoElement1_doCombinatorix = not topoElement1[0].isdigit()
        topoElement2_doCombinatorix = not topoElement2[0].isdigit()
        
        # first make sure that topocut matches selected triggers
       
        self.element1 = Threshold(topoElement1)
        self.element2 = Threshold(topoElement2)

        topoElement1_Ok = False
        topoElement2_Ok = False

        if self.element1.doEtaCut or self.element2.doEtaCut:
            self.doLoopCut = True
            
        for thresh in thresholds: 
            if thresh.var == self.element1.var and thresh.mult >= self.element1.mult and thresh.thresh == self.element1.thresh and thresh.Vthresh == self.element1.Vthresh:
                topoElement1_Ok = True
            if thresh.var == self.element2.var and thresh.mult >= self.element2.mult and thresh.thresh == self.element2.thresh and thresh.Vthresh == self.element2.Vthresh:
                topoElement2_Ok = True

                
        if not topoElement1_Ok:
            print "WARNING Topo cut does not match selected triggers: "+topoElement1
            #raise NameError('Topo cut does not match selected triggers: '+topoElement1)
        if not topoElement2_Ok:
            print "WARNING Topo cut does not match selected triggers: "+topoElement2
            #raise NameError('Topo cut does not match selected triggers: '+topoElement2)


        #isSameObjectType = 1 if element1.var == element2.var and element1.thresh == element2.thresh and element1.Vthresh == element2.Vthresh else 0 # remove thresh such that EM10_EM15 removes overlap ???
        #self.isSameObjectType = 1 if self.element1.var == self.element2.var and self.element1.Vthresh == self.element2.Vthresh else 0 # remove thresh such that EM10_EM15 removes overlap ???
        self.isSameObjectType = 1 if self.element1.var == self.element2.var else 0 
        self.checkSameRoI = 1 if not self.isSameObjectType and ("EM" in self.element1.var or "EMH" in self.element1.var or "EMHI" in self.element1.var or "TAU" in self.element1.var or "JET" in self.element1.var) and ("EM" in self.element2.var or "EMH" in self.element2.var or "EMHI" in self.element2.var or "TAU" in self.element2.var or "JET" in self.element2.var) else 0


        ##### now create cut string

        # Determine index (range) of objects on which topo selection is to be applied
        
        if topoElement1_doCombinatorix:
            self.topoElement1_indices = range(0,vars[self.element1.var])
        else:
            self.topoElement1_indices = [self.element1.mult-1]

        if topoElement2_doCombinatorix:
            self.topoElement2_indices = range(0,vars[self.element2.var])
        else:
            self.topoElement2_indices = [self.element2.mult-1]


        if not self.doLoopCut:

            cutList = []

            # loop over all objects to be included into the topo selection

        
            for idx1 in self.topoElement1_indices:
                for idx2 in self.topoElement2_indices:

                    topoCutList = []

                    if self.topoType == "DETA":
                        if self.lowerCut != -1:
                            topoCutList += ["(fabs(%s_eta[%i] - %s_eta[%i]) > %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.lowerCut)]
                        if self.upperCut != -1:
                            topoCutList += ["(fabs(%s_eta[%i] - %s_eta[%i]) < %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.upperCut)]
                        topoCutString = '&&'.join(topoCutList)
                   
                    elif self.topoType == "DPHI":
                        if self.lowerCut != -1:
                            topoCutList += ["(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()) > %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.lowerCut)]
                        if self.upperCut != -1:
                            topoCutList += ["(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()) < %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.upperCut)]
                        topoCutString = '&&'.join(topoCutList)

#                    elif self.topoType == "DPHIALL":
#                        if self.lowerCut != -1:
#                            topoCutList += ["(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()) < %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.lowerCut)]
#                        if self.upperCut != -1:
#                            topoCutList += ["(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()) > %s)" % (self.element1.var,idx1,self.element2.var,idx2, self.upperCut)]
#                        topoCutString = '||'.join(topoCutList)
                        
                    elif self.topoType == "DR":
                        if self.lowerCut != -1:
                            topoCutList += ["(TMath::Sqrt(pow(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()),2)+pow(fabs(%s_eta[%i] - %s_eta[%i]),2)) > %s)" % (self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.lowerCut)]
                        if self.upperCut != -1:
                            topoCutList += ["(TMath::Sqrt(pow(fabs(fabs(fabs(%s_phi[%i] - %s_phi[%i])-TMath::Pi())-TMath::Pi()),2)+pow(fabs(%s_eta[%i] - %s_eta[%i]),2)) < %s)" % (self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.upperCut)]
                        topoCutString = '&&'.join(topoCutList)

                    elif self.topoType == "INVM":
                        if self.lowerCut != -1:
                            topoCutList += ["(TMath::Sqrt(2*%s_pt[%i]*%s_pt[%i]*(TMath::CosH(%s_eta[%i] - %s_eta[%i])-TMath::Cos(%s_phi[%i] - %s_phi[%i]))) > %s)" % (self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.lowerCut)]
                        if self.upperCut != -1:
                            topoCutList += ["(TMath::Sqrt(2*%s_pt[%i]*%s_pt[%i]*(TMath::CosH(%s_eta[%i] - %s_eta[%i])-TMath::Cos(%s_phi[%i] - %s_phi[%i]))) < %s)" % (self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2, self.upperCut)]
                        topoCutString = '&&'.join(topoCutList)                    

                    threshold1CutString = self.element1.cutAnyIdx.replace("[X]","[%i]" % (idx1))
                    threshold2CutString = self.element2.cutAnyIdx.replace("[X]","[%i]" % (idx2))

                    overlapRemovalString = "0" if idx1==idx2 and self.isSameObjectType else "1"
                    if self.checkSameRoI:
                        overlapRemovalString = "%s_eta[%i]!=%s_eta[%i] || %s_phi[%i]!=%s_phi[%i]" % (self.element1.var, idx1, self.element2.var, idx2, self.element1.var, idx1, self.element2.var, idx2)
                        

                    # each cutList expression for a given pair of objects in this loop consists the following expressions connected by a logical AND:
                    # 1) the topo cut (including lower and upper cut)
                    # 2) an expression to make sure that 1) is only effective if trigger thresholds for the two involved objects are passed
                    # 3) an expression to make sure that 1) is only effective if objects are not identical (based on index and object type)
                
                    cutList += ["((%s) && ((%s) && (%s)) && (%s) )" % (topoCutString, threshold1CutString, threshold2CutString, overlapRemovalString)]            

            self.cut = "("+'||'.join(cutList)+")"
            
    def loopCut(self,nt):
        accepted = False
        for idx1 in self.topoElement1_indices:
            for idx2 in self.topoElement2_indices:
                
                if idx1==idx2 and self.isSameObjectType:
                    continue
                
                if not eval((self.element1.cutAnyIdxTree.replace("[X]","[%i]" % (idx1)).replace("||"," or ")) + " and " + (self.element2.cutAnyIdxTree.replace("[X]","[%i]" % (idx2)).replace("||"," or "))): # make sure tested objects fulfill trigger requirements
                    continue

                if self.element1.doEtaCut:
                    if not eval((self.element1.etaCutAnyIdxTree.replace("[X]","[%i]" % (idx1)))):
                        continue

                if self.element2.doEtaCut:
                    if not eval((self.element2.etaCutAnyIdxTree.replace("[X]","[%i]" % (idx2)))):
                        continue
                    
                eta1 = getattr(nt,self.element1.var+"_eta")[idx1] if not self.element1.var=="MET" else getattr(nt,self.element1.var+"_eta")
                eta2 = getattr(nt,self.element2.var+"_eta")[idx2] if not self.element2.var=="MET" else getattr(nt,self.element2.var+"_eta")                        
                phi1 = getattr(nt,self.element1.var+"_phi")[idx1] if not self.element1.var=="MET" else getattr(nt,self.element1.var+"_phi")
                phi2 = getattr(nt,self.element2.var+"_phi")[idx2] if not self.element2.var=="MET" else getattr(nt,self.element2.var+"_phi")

                if self.checkSameRoI and eta1==eta2 and phi1==phi2:
                    continue

                #if self.checkSameRoI and self.calcDR(eta1,eta2,phi1,phi2)<0.01:
                #    continue

                if self.topoType == "DETA":
                    var = self.calcDeta(eta1,eta2)
                if self.topoType == "DPHI":
                    var = self.calcDphi(phi1,phi2)
                if self.topoType == "DR":
                    var = self.calcDR(eta1,eta2,phi1,phi2)
                if self.topoType == "INVM":
                    pt1 = getattr(nt,self.element1.var+"_pt")[idx1] if not self.element1.var=="MET" else getattr(nt,self.element1.var+"_pt")
                    pt2 = getattr(nt,self.element2.var+"_pt")[idx2] if not self.element2.var=="MET" else getattr(nt,self.element2.var+"_pt")
                    var = self.calcInvM(pt1,pt2,eta1,eta2,phi1,phi2)
                        
                if self.all:
                    if self.lowerCut != -1 and not self.upperCut != -1:
                        if var < float(self.lowerCut):
                            return False
                        else:
                            accepted = True
                    elif not self.lowerCut != -1 and self.upperCut != -1:
                       if var > float(self.upperCut):
                           return False
                       else:
                            accepted = True
                    elif self.lowerCut != -1 and self.upperCut != -1:
                        if var < float(self.lowerCut) or var > float(self.upperCut):
                            return False
                        else:
                            accepted = True
                            
                else:                      
                    if self.lowerCut != -1 and not self.upperCut != -1:
                        if var > float(self.lowerCut):
                            return True
                    elif not self.lowerCut != -1 and self.upperCut != -1:
                        if var < float(self.upperCut):
                            return True
                    elif self.lowerCut != -1 and self.upperCut != -1:
                        if var > float(self.lowerCut) and var < float(self.upperCut):
                            return True
        return accepted


    def calcDphi(self,phi1,phi2):
        return abs(abs(abs(phi1 - phi2)-ROOT.TMath.Pi())-ROOT.TMath.Pi())

    def calcDeta(self,eta1,eta2):
        return abs(eta1 - eta2)

    def calcDR(self,eta1,eta2,phi1,phi2):
        return sqrt(pow(self.calcDphi(phi1,phi2),2)+pow(self.calcDeta(eta1,eta2),2))

    def calcInvM(self,pt1,pt2,eta1,eta2,phi1,phi2):
        return sqrt(2*pt1*pt2*(ROOT.TMath.CosH(eta1 - eta2)- ROOT.TMath.Cos(phi1 - phi2)))


class Item:
    def __init__(self,item):
        self.name = item
        self.thresholds=[]
        self.topocuts=[]
        self.doLoopCut = False
        self.doEtaCut = False
        for thresh in item.split(":")[0].split("_"):
            if thresh=='L1': continue
            self.thresholds.append(Threshold(thresh))
            if self.thresholds[-1].doEtaCut:
                self.doEtaCut = True

        self._cut='&&'.join([ ('(%s)' % thresh.cut) for thresh in self.thresholds])
                
        for topo in item.split(":")[1:]:
            print topo
            self.topocuts.append(TopoCut(topo,self.thresholds))
            if self.topocuts[-1].doLoopCut:
                self.doLoopCut = True

        self._topocut = ""
        if not self.doLoopCut:
            self._topocut='&&'.join([('%s' % topo.cut) for topo in self.topocuts])
    
        print "------ Item ", item
        print "DEBUG: doLoopCut " , self.doLoopCut
        print "DEBUG: doEtaCut " , self.doEtaCut
        print "DEBUG: Threshold Cut ", self._cut

        if len(self._topocut) > 0:
            self._cut = '&&'.join([self._cut,self._topocut])


        print "DEBUG: Topo Cut ", self._topocut  
        print "DEBUG: Combined Cut ", self._cut, "\n"
        
    def cut(self):
        return self._cut

    def loopCut(self,nt):
        passed = []
        for topo in self.topocuts:
            passed += [topo.loopCut(nt)]
        return False if False in passed else True # dangerous if passed is emty list

    def etaCut(self,nt):
        for thresh in self.thresholds:
            if thresh.doEtaCut:
                if not thresh.etaCut(nt):
                    return False
        return True
    
class Tuple:
    def __init__(self,files):
        #expect list of tuples pairs with patterns and weights
        if type(files)!=list:
            files=[files]
        self.tuples=[]
        self.weights=[]
        self.entries=[]
        self.offsets=[]
        self.fileNames=[]
        offset=0
        for file in files:
            nt=ROOT.TChain("physics")
            self.fileNames += [file[0]]
            nt.Add(file[0])
            self.tuples.append(nt)
            self.weights.append(file[1])
            self.offsets.append(offset)
            entries=nt.GetEntries()
            self.entries.append(entries)
            offset+=entries

    def passes(self,item):
        eventIDs=[]
        ntCounter = 0
        for nt,offset in zip(self.tuples,self.offsets):
            num=nt.Project("hist","Entry$",item.cut())
            v=nt.GetV1()

            if item.doEtaCut or item.doLoopCut:   # run topo selection on accepted events only!
                acceptedEvents = []
                print "DEBUG: Running event loop.."
                for idx in range(num):
                    tmp = nt.GetEntry(int(v[idx]))
                    acceptedEta = True
                    acceptedTopo = True
                    if item.doEtaCut:
                        acceptedEta = item.etaCut(nt)
                    if item.doLoopCut:
                        acceptedTopo = item.loopCut(nt)
                    accepted = acceptedEta and acceptedTopo                       
                    if accepted:
                        acceptedEvents += [v[idx]]
                v = acceptedEvents
                num = len(v)
                
            print "DEBUG: # events after cuts for item %s: %i" %(item.name, num)

            for idx in range(num):
                eventIDs.append(int(v[idx])+offset) # contains event IDs of events that pass the item.cut
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
        passes[trigger]=tuple.passes(item) # passes[trigger] are the event IDs that passed the trigger 
    total=set()
    for trigger in menu:
        rates[trigger][0]=lumi*sum([weights[event] for event in passes[trigger]]) # sum of weights of events that pass the trigger
        if len(passes[trigger]):
            rates[trigger][1]=lumi*sqrt(sum([weights[event]**2 for event in passes[trigger]])) # error
        total.update(passes[trigger]) # put all the events in a set
        all=set()
        for other in menu: # run over other triggers in menu
            if other==trigger: continue
            all.update(passes[other]) # make set of events passing all other triggers (set does not take duplicate!)
        unique=set(passes[trigger]).difference(all) # make list of events that are only passed by this trigger and not by any other in the menu
        rates[trigger][2]=lumi*sum([weights[event] for event in unique]) #unique rate
        if len(unique):
            rates[trigger][3]=lumi*sqrt(sum([weights[event]**2 for event in unique])) #unique error
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
    stringLength = 0
    for trigger in triggers:
        item=Item(trigger)
        menu[trigger]=item
        if  len(trigger) > stringLength:
            stringLength = len(trigger) 
    
    
    print 'Calculating data rates'
    ratesData=calcRates(menu,eb,lumi) # eb is a tuple

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
            
            

    print ('%-'+str(stringLength+10)+'s  %10s  %10s  %10s  %10s   %10s') % ('Trigger','8 TeV Data','8 TeV MC','14 TeV MC',
                                           'MC Scaling','Data scaled to 14 TeV')

    #outfh=open('rates.csv','w')
    def printLine(*args):
        print ('%-'+str(stringLength+10)+'s  %4.1f+-%4.1f  %4.1f+-%4.1f  %4.1f+-%4.1f  (%3.1f+-%3.1f)  %4.1f+-%4.1f') % args
        #outfh.write('%-20s,  %4.1f,+-,%4.1f,  %4.1f,+-,%4.1f,  %4.1f,+-,%4.1f,  %3.1f,+-,%3.1f,  %4.1f,+-,%4.1f\n' % args )
        
    for trigger in triggers+['Total']:
        if trigger=='Total':
            print '='*(stringLength+82)
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
