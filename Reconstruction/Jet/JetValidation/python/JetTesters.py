# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file:    JetExamples/python/JetTesters.py
## @purpose: Algs to quickly dump jet collection content intot txt files and/or ntuples
## @author:  P-A Delsart <delsart in2p3 fr>
##
##
##

__doc__     = '''Algs to quickly dump jet variables intot txt files and/or ntuples.

Used in Run 1. Could be also useful in Run 2 ??


This module implements a mini D3PD framework but
   - allows to dump txt files 
   - written mostly in python so it is very quickly hackable
This allows  immediate tests and comparisons and is mostly intended for testing jet devellopment
and debugging.

There is a main python Algorithm JetTester : it is dedicated to one JetCollection and holds a list
of tools inheriting JetTestToolBase.
Each of these tools act on the JetCollection by inspecting and umping some variables in a common txt
file and/or root ntuple.

There is also a tool written in plain c++ (ROOT.JetTesterCpp) used to test jet feature from the
c++ side. It is adapted below to be usable in the python side.

Some configuration helper functions in python/JetTestConfig.


'''
__version__ = '$Revision: 1.5 $'
__author__  = 'P-A Delsart <delsart  in2p3 fr>'

import AthenaCommon.SystemOfUnits as Units
import AthenaPython.PyAthena as PyAthena
from AthenaPython.PyAthena import StatusCode

import ROOT 
import array


class JetTestToolBase(object):
    def initialize(self, testerAlg):
        self.tree = testerAlg.outTree
        self.testerAlg = testerAlg
        self.vectors = []
        self.prefix = self.testerAlg.CollectionName.replace('Jets','')
        self.writetxt  = self.testerAlg.outTxtFile.write

    def setupEvent(self, jetColl):
        size = jetColl.size()
        for v in self.vectors:
            v.clear()
            v.resize(size)

    def processJetCollection(self, jetColl):
        pass
    def processJet(self,i, jet):
        pass
    def finalizeEvent(self):
        pass

    def buildPrefix(self):
        return 
    
    def makeBranch(self,name,btype, prefix = None):
        if prefix is None:
            prefix = self.prefix+'_'
        bname = prefix+name
        if isinstance(btype, str):
            # assume it is a simple type.
            obj = array.array(btype,[0])
            branch = self.tree.Branch(bname, obj, bname+'/'+btype.upper() )
        else:
            # assume it is a vector
            obj = btype()
            branch = self.tree.Branch(bname, obj )
            self.vectors.append(obj)

        return obj

    

class JetCollectionTestTool(JetTestToolBase):
    def dump(self, txt):
        self.writetxt( self.prefix+'         %s\n'%(txt,) )
    
class JetTestTool(JetTestToolBase):
    def blockDump(self,doBlock):
        if doBlock:
            self.dump = self.dump_dummy
        else:
            self.dump = self.dump_real
    def dump(self, i, txt):
        pass
    def dump_dummy(self,i, txt):
        pass
    def dump_real(self, i, txt):
        self.writetxt( self.prefix+' %2d     %s\n'%(i,txt) )
    

class JetCollN(JetCollectionTestTool):
    def initialize(self, testerAlg):
        super(JetCollN,self).initialize(testerAlg)
        self.n = self.makeBranch('N','i')
    def processJetCollection(self, jetColl):
        n = jetColl.size()
        self.n[0] = n
        self.dump('Num Jet = %d'%(n,))
    

class JetKinematics(JetTestTool):
    def __init__(self, sigstate = 1, dopxpypz=False, dorap=False):
        self.sigstate = sigstate
        self.dopxpypz = dopxpypz
        self.dorap = dorap

    def initialize(self, testerAlg):
        super(JetKinematics,self).initialize(testerAlg)
        vect_f = ROOT.vector(float)
        self.e = self.makeBranch("e",vect_f)
        self.pt = self.makeBranch("pt",vect_f)
        self.eta = self.makeBranch("eta",vect_f)
        self.phi = self.makeBranch("phi",vect_f)
        if self.dopxpypz:
            self.px = self.makeBranch("px",vect_f)
            self.py = self.makeBranch("py",vect_f)
            self.pz = self.makeBranch("pz",vect_f)

    def processJet(self, i, jet):
        e, pt , eta, phi = jet.e(), jet.pt(), jet.eta(), jet.phi()
        self.e[i]   = e
        self.pt[i]  = pt
        self.eta[i] = eta
        self.phi[i] = phi
        self.dump(i, 'final      e, pt, (eta,phi) = %10.2f ,  %10.2f (% 1.3f,% 1.3f)'%(e,pt,eta,phi) )
        if self.dopxpypz:
            px, py, pz = jet.px(), jet.py(), jet.pz()
            self.dump(i, ' px, py, pz = %10.2f , %10.2f , %10.2f'%(px,py,pz) )
            self.px[i] = px
            self.py[i] = py
            self.pz[i] = pz

class JetSigState(JetTestTool):
    def initialize(self, testerAlg):
        super(JetSigState,self).initialize(testerAlg)
        vect_f = ROOT.vector(float)
        self.em_pt = self.makeBranch("emscale_pt",vect_f)
        self.em_eta = self.makeBranch("emscale_eta",vect_f)
        self.c_pt = self.makeBranch("constscale_pt",vect_f)
        self.c_eta = self.makeBranch("constscale_eta",vect_f)
    
    def processJet(self, i, jet):
        for s,n in [ (0,'emscale   '), (2,'constscale')]:
            e, pt , eta, phi = jet.e(s), jet.pt(s), jet.eta(s), jet.phi(s)
            ptV, etaV = self.vectors[s], self.vectors[s+1]
            ptV[i]   = pt
            etaV[i]  = eta
            self.dump(i, n+' e, pt, (eta,phi) = %10.2f ,  %10.2f (% 1.3f,% 1.3f)'%(e,pt,eta,phi) )

class JetAuthor(JetTestTool):
    def processJet(self, i, jet):
        self.dump(i, 'author=%s .  author+calibtag=%s'%(jet.jetAuthor(), jet.jetAuthorAndCalibTags()) )

class JetNConstituents(JetTestTool):
    def initialize(self, testerAlg):
        super(JetNConstituents,self).initialize(testerAlg)
        vect_i = ROOT.vector(int)
        self.size = self.makeBranch("size", vect_i)
        self.constN = self.makeBranch("constN", vect_i)        

    def processJet(self, i, jet):
        s , n = jet.size(), jet.constituentsN()
        self.size[i] = s
        self.constN[i] = n
        self.dump(i, "size =%d   constitN=%d"%(s,n))

        

class JetMoments(JetTestTool):        
    def __init__(self, moments=['LArQuality', 'Timing', 'HECQuality', 'NegativeE', 'energy_EMB2', 'JVF', 'nTrk', 'sumPtTrk', 'NumTowers','WIDTH']):
        self.moments = moments
    def initialize(self, testerAlg):
        super(JetMoments,self).initialize(testerAlg)
        vect_f = ROOT.vector(float)
        for m in self.moments:
            self.makeBranch(m,vect_f) 

    def processJet(self, i, jet):
        getM = jet.getMoment
        for m,v in zip(self.moments,self.vectors):
            mV = getM(m)
            v[i] = mV
            self.dump(i, '%s %f'%(m,mV) )

class JetConstituents(JetTestTool):
    def initialize(self, testerAlg):
        super(JetConstituents,self).initialize(testerAlg)
        vect = ROOT.vector(float)
        self.c_emscale_e = self.makeBranch("const0_emscale_e", vect)
        self.c_default_e = self.makeBranch("const0_default_e", vect)        

    def processJet(self, i, jet):
        print 'ERROR  JetTesters.py, JetConstituents : does not weork with Run2 software !!'
        # this is run 1 : this won't work
        ## it0 = ROOT.JetConstituentIterator.first(jet,0)
        ## itD = ROOT.JetConstituentIterator.first(jet)
        ## itE = ROOT.JetConstituentIterator.last(jet)
        ## count = 0
        ## list_e0 =[]
        ## list_ed = []
        ## while (it0 != itE) and count<3:
        ##     e0, eD = it0.e(), itD.e()
        ##     if count == 0:
        ##         self.c_emscale_e[i] = e0
        ##         self.c_default_e[i] = eD
        ##     list_e0.append(e0)
        ##     list_ed.append(eD)
        ##     it0.inc()
        ##     itD.inc()
        ##     count +=1
        ## self.dump(i, 'constit emscale e =' + ', '.join('%10.2f'%(e,) for e in list_e0) )
        ## self.dump(i, 'constit default e =' + ', '.join('%10.2f'%(e,) for e in list_ed) )        


    




## class JetCppTester(ROOT.JetTesterCpp, JetTestTool, JetCollectionTestTool):

##     # recopy the dumping functions... 
##     dump_jcoll = JetCollectionTestTool.dump
    
##     def __init__(self, **args):
##         ROOT.JetTesterCpp.__init__(self)

##         # set the c++ flags. False by default.
##         for f in ["CellNavig","SigStateHelper"]:
##             setattr(self, 'm_do'+f, args.get(f,False) )
        
                   
##     def initialize(self, testerAlg):
##         JetTestTool.initialize(self,testerAlg)
##         self.m_prefix = testerAlg.CollectionName.replace('Jets','')+'_'
##         ROOT.JetTesterCpp.initialize(self, testerAlg.outTree)

##     def processJetCollection(self, jcoll):
##         self.m_outputStrings.clear()
##         ROOT.JetTesterCpp.processJetCollection(self,jcoll)
##         for s in self.m_outputStrings:
##             self.dump_jcoll(s)

##     def processJet(self, i ,jet):
##         self.m_outputStrings.clear()
##         ROOT.JetTesterCpp.processJet(self,jet)
##         for s in self.m_outputStrings:
##             self.dump(i,s)
        

# #########################################################################
# The Athena Algorith
# #########################################################################
class JetTester (PyAthena.Alg):
    outFileName = "jettest"
    outRootFile = None
    outTxtFile = None
    outTree = None
    enum = -1

    fillingInstance = False
    lastFiller = None
    @classmethod
    def setFillingInstance(cls, inst):
        if cls.lastFiller :
            cls.lastFiller.fillingInstance = False
        cls.lastFiller = inst
        inst.fillingInstance = True
    
    def __init__(self, name='JetTester', **kw):
        ## init base class
        kw['name'] = name
        super(JetTester, self).__init__(**kw)

        ## properties and data members
        self.CollectionType = kw.get('CollectionType', 'JetCollection') # default value
        self.CollectionName = kw.get('CollectionName', '') # default value
        self.JetTestTools = kw.get('JetTestTools', []) # default value        
        self.NJetTxt = kw.get('NJetTxt', 5) # default value
        self.NoTxt = kw.get('NoTxt', False)
        return

    def initialize(self):
        self.msg.info('==> initialize...')
        if not self.outRootFile:
            JetTester.outRootFile = ROOT.TFile(self.outFileName+".root","recreate")
            JetTester.outTxtFile = open(self.outFileName+".txt","w")
            JetTester.outRootFile.cd()
            JetTester.outTree = ROOT.TTree("JetTestTree","jet tests")


        if not isinstance(self.JetTestTools[0], JetCollN):
            self.JetTestTools = [JetCollN()] + self.JetTestTools

        for t in self.JetTestTools:
            t.initialize(self)


        singleT =[]
        collecT = []
        for t in self.JetTestTools:
            if isinstance(t, JetCppTester) :
                singleT.append(t)
                collecT.append(t)
                continue
            elif isinstance(t, JetTestTool) : singleT.append(t)
            elif isinstance(t, JetCollectionTestTool) : collecT.append(t)
            
        self.singleJetTesters = singleT
        self.collectionJetTesters = collecT
        
        
        self.sg = PyAthena.py_svc ('StoreGateSvc')
        return StatusCode.Success

    

    def execute(self):
        e=self.sg.retrieve('EventInfo')
        enum = e.event_ID().event_number()
        if enum != JetTester.enum:
            JetTester.enum = enum
            self.outTxtFile.write('\n')
            self.outTxtFile.write(' ##################################################################### \n')
            self.outTxtFile.write(' Event %d\n'%enum)
            self.outTxtFile.write(' ##################################################################### \n')            
        jetColl = self.sg.retrieve(self.CollectionType, self.CollectionName)

        self.outTxtFile.write('  ------------------------------------------ \n')
        for t in self.JetTestTools:
            t.setupEvent(jetColl)

        for t in self.collectionJetTesters:
            t.processJetCollection(jetColl)

        # reset Txt dumping
        for t in self.singleJetTesters:
            t.blockDump(self.NoTxt)

        for i,jet in enumerate(jetColl):
            if (i>= self.NJetTxt):
                for t in self.singleJetTesters:
                    t.blockDump(True)
            else:
                self.outTxtFile.write('\n')
            for t in self.singleJetTesters:
                t.processJet(i,jet)

        for t in self.JetTestTools:
            t.finalizeEvent()

        # only one alg should call TTree::Fill (which alg is set by config)
        if self.fillingInstance:

            self.outTree.Fill()
        return StatusCode.Success

    def finalize(self):
        self.msg.info('==> finalize...  ')
        if self.outRootFile:
            self.outRootFile.cd()            
            JetTester.outTree.Write()
            JetTester.outRootFile.Close()
            JetTester.outTxtFile.close()
            JetTester.outRootFile = None
        return StatusCode.Success
    
    # class JetTester
