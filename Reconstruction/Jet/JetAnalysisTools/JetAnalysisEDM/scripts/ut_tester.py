# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
from JetTestCheck import VarPairVector, VarComparator
import JetTestCheck
import sys
import glob

print sys.argv
MAX_REL_DIFF = float(0.001)

testFiles =sys.argv[1]



refTree = ROOT.TChain("physics")
refTree.AddFile("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root")
refTree.AddFile("/home/delsart/analysis/topAnalisis/files/topBoosted_only5.root")




class VarPairVector_limit2(JetTestCheck.VarPairVector):
    def getEntry(self, evt1,evt2):
        a1, a2 =VarPairVector.getEntry(self,evt1,evt2)
        return a1 , a2[:3:2] # get only entry 0 and 2 of a2

class VarPairVector_ignoreFirst(JetTestCheck.VarPairVector):
    def getEntry(self, evt1,evt2):
        a1, a2 =VarPairVector.getEntry(self,evt1,evt2)
        return a1[20:] , a2[20:] 

class VarPairVector_limitToN(JetTestCheck.VarPairVector):
    Nmax = 2
    def getEntry(self, evt1,evt2):
        a1, a2 =VarPairVector.getEntry(self,evt1,evt2)
        return a1[:self.Nmax] , a2[:self.Nmax] 

class VarPairVector_scale(JetTestCheck.VarPairVector):
    def getEntry(self, evt1,evt2):
        a1, a2 =VarPairVector.getEntry(self,evt1,evt2)
        #print ' VarPairVector_scale getEntry ', self.name
        if self.name == "_pt":        
            a2 *= 1.3
        elif self.name == "_constscale_pt":
            a2 *=0.5
        elif self.name == "_AntiKt10LCTopo_OutputV":
                a2[:] = 23.4
        elif self.name == "_OutputV":
                a2[:] = 43.4


        return a1, a2


def myjet_replacer_outputv(name):
    if 'OutputV' in name : return 'jet_AntiKt10LCTopo_pt'
    return name.replace('myjet_', 'jet_AntiKt10LCTopo_')

def myjet_replacer(name):
    return name.replace('myjet_', 'jet_AntiKt10LCTopo_')

def constscale_replacer(name):
    return name+'_constscale_'


contentTesterList = []
class ContentTester(object):
    def __init__(self, filename, prefix, vlist, func, branchL=None):
        self.filename = filename
        contentTesterList.append( self )
        self.branchL = [prefix+v for v in vlist]
        if branchL is not None:
            self.branchL += branchL
        self.repFunc = func
        self.errorMsg =''

    def presetup(self):
        input = self.filename
        if self.filename == "ViewContainerTest_out.root":
            JetTestCheck.VarPairVector = VarPairVector_limit2
        elif self.filename == "ContainerModifTest_out.root":
            JetTestCheck.VarPairVector = VarPairVector_scale
        elif self.filename == "PtEtaPhiMTest_out.root":
            JetTestCheck.VarPairVector = VarPairVector_ignoreFirst
        elif self.filename == "EventManagerTest_out.root":
            VarPairVector_limitToN.Nmax = 2
            JetTestCheck.VarPairVector = VarPairVector_limitToN
        else:
            JetTestCheck.VarPairVector = VarPairVector

    
    def treeChecks(self, varComp):
        res = True
        if self.filename == "ContainerVsVarHTest_out.root":
            if varComp.t1.GetListOfBranches().GetSize() >= 20:
                self.errorMsg = self.filename+"  :  too high number of branches : "+str(varComp.t1.GetListOfBranches().GetSize())
                res = False
        return res

    def test(self, debug=False):
        self.presetup()
        varComp = VarComparator( self.filename, refTree, MAX_REL_DIFF , treename='bla') 
        varComp.init_trees(branch_list = self.branchL, branch_replace= self.repFunc)
        res = self.treeChecks(varComp)
        if res:
            res = varComp.full_compare(debug=debug)
        varComp.close()
        if res : print " Test ", self.filename, " OK "
        return res

class ContentTesterVoid(ContentTester):
    def test(self):
        return True

import numpy
class ContentTesterScalToVec(ContentTester):
    def test(self, debug=False):
        tref = refTree
        f = ROOT.TFile(self.filename)
        t = f.Get("bla")
        print t
        t.AddFriend(refTree)
        n=t.Draw("singlejet_pt-jet_AntiKt10LCTopo_pt[0]:singlejet_eta-jet_AntiKt10LCTopo_eta[0]","","goff")
        a1=numpy.ndarray(n,dtype=numpy.float64,buffer=t.GetV1())
        res = abs(a1.max()) < 0.01
        a2=numpy.ndarray(n,dtype=numpy.float64,buffer=t.GetV2())
        res = res and abs(a2.max()) < 0.01
        # self.t = t
        # self.f = f
        #print a1,a2
        if res : print " Test ", self.filename, " OK "
        return res

class ContentTesterVect3(ContentTester):
    def test(self,debug=False):
        res = super(ContentTesterVect3,self).test(debug)
        if not res: return False
        f = ROOT.TFile(self.filename)
        t = f.Get("bla")
        n=t.Draw("myjet_Tau2 - myjet_vect3[][0]","","goff")
        a1=numpy.ndarray(n,dtype=numpy.float64,buffer=t.GetV1())
        res = abs(a1.max()) < 0.01
        if res : print " Test ", self.filename, " OK "
        return res

class ContentTesterFixedValue(ContentTester):
    def __init__(self, filename, prefix, varvallist ):
        self.filename = filename
        self.prefix = prefix
        self.vv = varvallist
        contentTesterList.append( self )
                
    def test(self, debug=False):
        vv = self.vv
        # pack the var list by 4        
        nvar = len(vv)
        vvpacked = [ vv[4*i+4*i+3] for i in range(nvar/4)] +[ vv[-(nvar%4):] ]

        f = ROOT.TFile(self.filename)
        t = f.Get("bla")

        # for each group of 4 variables, TTree::Draw and compare to value
        for vv in vvpacked:
            # vv is in the form [(varname, value), (varname, value),...]
            formula = ':'.join(self.prefix+var for (var,val) in vv)
            n=t.Draw(formula,"","goff")
            for i in range(len(vv)):
                a=numpy.ndarray(n,dtype=numpy.float64,buffer=t.GetVal(i))
                print abs(a - vv[i][1]).max() # (a-vv[i][1]).max()
                res = abs(a-vv[i][1]).max() < 0.01
                if not res:
                    print "Error with " , vv[i]
                    print " values from tree = ",a
                    return res
        print " Test ", self.filename, " OK "
        return True
        
    pass



ContentTester("TwoEventInfoTest_out.root" ,  "", [] , lambda s:s, branchL= ["cl_n", "actualIntPerXing", "EventNumber"])
ContentTester("ContainerVsVarHTest_out.root" ,  "jet_AntiKt10LCTopo_", ["pt", "constscale_pt"] , lambda s:s)
ContentTester("TwoVarHandleWriteTest_out.root" ,  "myjet_", ["pt"] , myjet_replacer)
ContentTesterVect3("ViewContainerTest_out.root" ,  "myjet_", ["pt", "emscale_pt", "constscale_eta" ] , myjet_replacer)
ContentTester("ViewContainerCloneTest_out.root" ,  "myjet_", ["pt", "constscale_pt" ] , myjet_replacer)
ContentTester("ContainerModifTest_out.root" , "myjet_", ["pt", "constscale_pt" , "OutputV"] , myjet_replacer_outputv, branchL=['jet_AntiKt10LCTopo_OutputV'])
ContentTester("VecVecVarHandleTest_out.root" , "jet_AntiKt10LCTopo_", ["pt",  ] , lambda s:s)
ContentTester("KinematicVarTest_out.root" , "jet_AntiKt10LCTopo_constscale_", ["pt",  ] , lambda s:s)
ContentTester("PtEtaPhiMTest_out.root" , "mc_", ["pt", "eta", "m" ] , lambda s:s)
ContentTester("EventManagerTest_out.root" , "myjet_", ["pt", "eta",  ] , myjet_replacer)
ContentTesterFixedValue("EventManagerTest_out.root" , "myjet_", [ ("myVar", 22.1), ("constscale_eta",10) ] )
ContentTesterScalToVec("SinglePartTest_out.root" , "singlejet_", ["pt", "eta" ] , myjet_replacer)


if testFiles == "all":
    testFiles = contentTesterList#glob.glob("*out.root")
else:
    testFiles = [t for t in contentTesterList if t.filename == testFiles]

for tt in testFiles:
    
    tf=tt.filename
    print "**************************************************"
    print "Testing ",tf
    if not tt.test(debug=True):
        print '************* error in ',tf, " msg= ", tt.errorMsg
        break


# for interactive tests :
if len(testFiles) == 1:
    probeF = ROOT.TFile(testFiles[0].filename)
    probeTree = probeF.Get("bla")
