# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
import ROOT.TH1F
from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode

class eFexEratioAlgoOff (PyAthena.Alg) :

   def __init__(self,name="eFexEratioAlgoOff"):
     print "Constructor",name
     PyAthena.Alg.__init__(self,name)
     self.name = name
     self.sg=None
     self.ds=None
     self.pi = ROOT.TMath.Pi() # just to cache
     return

   def initialize(self):
     print "In Initialize"
     PyAthena.Alg.initialize(self)
     self.sg = PyAthena.py_svc('StoreGateSvc')
     self.ds = PyAthena.py_svc('DetectorStore')
     dd=ROOT.CaloDetDescrManager.instance()
     self.hnum1 = []
     self.hnum2 = []
     self.hnum3 = []
     self.hnum4 = []
     self.hnum5 = []
     self.hden = []
     self.hetanum1 = []
     self.hetanum2 = []
     self.hetanum3 = []
     self.hetanum4 = []
     self.hetanum5 = []
     self.hetaden = []
     return StatusCode.Success

   def deltaPhi(self,phi1,phi2):
      dphi = abs(phi1-phi2);
      dphi = abs(self.pi-dphi);
      dphi = abs(self.pi-dphi);
      return dphi;

   def execute(self):

     #print 'execute : ',self.name
     from AthenaPython import PyAthena
     pid = PyAthena.egammaPID
     electrons = self.sg['Electrons']
     eFex = self.sg['SCluster']
     lASP = self.sg['LArLayer1Vars']
     for t in electrons:
       if ( (t.pt() > 15e3) and (t.passSelection(ROOT.xAOD.EgammaParameters.LHMedium) ) ):
         if ( abs(t.eta()) > 2.47 ):
             continue
         if ( (abs(t.eta())>1.37) and (abs(t.eta()) <1.54) ):
             continue
         pt = t.pt()/1000
         #print dir(t)
         #print 'electron : ',pt,t.eta()
         self.hden.append(pt)
         self.hetaden.append(t.eta())
         for scluster in eFex:
            if ( scluster.et() < 20e3 ): continue;
            if ( abs(t.eta()-scluster.eta()) > 0.06 ): continue;
            if (self.deltaPhi(t.phi(),scluster.phi()) > 0.1 ):
                   continue;
            self.hnum1.append(pt)
            self.hetanum1.append(t.eta())
            rEta=-999.0
            eRatio=-999.0
            if ( scluster.e277() > 0.1 ) :
                   rEta = scluster.e237() / scluster.e277()
            if ( rEta < 0.745 ) :
                   continue
            self.hnum2.append(pt)
            self.hetanum2.append(t.eta())
            for eratio in lASP:
               if ( abs(eratio.eta()-scluster.eta()) > 0.035 ): continue;
               if ( self.deltaPhi(eratio.phi(),scluster.phi()) > 0.08):
                   continue;
               if ( eratio.emaxs1() + eratio.e2tsts1() > 0.1 ):
                   eRatio = ( eratio.emaxs1() - eratio.e2tsts1() ) / (eratio.emaxs1() + eratio.e2tsts1())
            if ( eRatio < 0.41 ):
               continue
            self.hnum3.append(pt)
            self.hetanum3.append(t.eta())
            #print t.pt(), t.eta(), t.phi(), scluster.et(), scluster.eta(), scluster.phi(), rEta, eRatio
            self.hnum4.append(pt)
            self.hetanum4.append(t.eta())

     #print 'end of execute : ',self.name
     #print "End of List"
     return StatusCode.Success

   def finalize(self):
     hnum1 = ROOT.TH1F("num1","num1",100,0,100);
     hnum2 = ROOT.TH1F("num2","num2",100,0,100);
     hnum3 = ROOT.TH1F("num3","num3",100,0,100);
     hnum4 = ROOT.TH1F("num4","num4",100,0,100);
     hden = ROOT.TH1F("den","den",100,0,100);
     hnumeta1 = ROOT.TH1F("numeta1","numeta1",100,-2.5,2.5);
     hnumeta2 = ROOT.TH1F("numeta2","numeta2",100,-2.5,2.5);
     hnumeta3 = ROOT.TH1F("numeta3","numeta3",100,-2.5,2.5);
     hnumeta4 = ROOT.TH1F("numeta4","numeta4",100,-2.5,2.5);
     hdeneta = ROOT.TH1F("deneta","deneta",100,-2.5,2.5);
     for i in self.hnum1:
         hnum1.Fill(i) 
     for i in self.hnum2:
         hnum2.Fill(i) 
     for i in self.hnum3:
         hnum3.Fill(i) 
     for i in self.hnum4:
         hnum4.Fill(i) 
     for i in self.hden:
         hden.Fill(i) 

     for i in self.hetanum1:
         hnumeta1.Fill(i)
     for i in self.hetanum2:
         hnumeta2.Fill(i)
     for i in self.hetanum3:
         hnumeta3.Fill(i)
     for i in self.hetanum4:
         hnumeta4.Fill(i)
     for i in self.hetaden:
         hdeneta.Fill(i)

     OutputFile=self.name+".root"
     f=ROOT.TFile(OutputFile,'RECREATE')
     hnum1.Write()
     hnum2.Write()
     hnum3.Write()
     hnum4.Write()
     hden.Write()
     hnumeta1.Write()
     hnumeta2.Write()
     hnumeta3.Write()
     hnumeta4.Write()
     hdeneta.Write()
     f.Write()
     f.Close()
     PyAthena.Alg.finalize(self)
     print "In Finalize"
     return StatusCode.Success

