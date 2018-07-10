# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
import ROOT.TH1F
from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode

class eFexEratioAlgo (PyAthena.Alg) :

   def __init__(self,name="eFexEratioAlgo",SClusterContName='SCluster'):
     print "Constructor",name
     PyAthena.Alg.__init__(self,name)
     self.name = name
     self.sg=None
     self.ds=None
     self.pi = ROOT.TMath.Pi() # just to cache
     self.SClusterContName=SClusterContName
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
     self.hphinum1 = []
     self.hphinum2 = []
     self.hphinum3 = []
     self.hphinum4 = []
     self.hphinum5 = []
     self.hetaden = []
     self.hphiden = []
     self.trackPt = []
     self.trackEta = []
     return StatusCode.Success

   def deltaPhi(self,phi1,phi2):
      dphi = abs(phi1-phi2);
      dphi = abs(self.pi-dphi);
      dphi = abs(self.pi-dphi);
      return dphi;

   def execute(self):

     print 'TTTT : ',self.SClusterContName
     truth = self.sg['TruthParticles']
     eFex = self.sg[self.SClusterContName]
     lASP = self.sg['LArLayer1Vars']
     tracks = self.sg['InDetTrackParticles']
     scells = self.sg['SCell']
     for t in truth:
       if ( (t.pt() > 15e3) and (abs(t.pdgId())==11) and (t.parent()) and (t.parent().pdgId()==23) and (t.barcode() < 10000) ):
         if ( abs(t.eta()) > 2.47 ):
             continue
         if ( (abs(t.eta())>1.37) and (abs(t.eta()) <1.54) ):
             continue
         #if ( abs(t.phi()) > 2.8 ) : continue
         pt = t.pt()/1000
         self.hden.append(pt)
         if ( pt > 20 ):
           self.hetaden.append(t.eta())
           self.hphiden.append(t.phi())
         for scluster in eFex:
            if ( scluster.et() < 20e3 ): continue;
            if ( abs(t.eta()-scluster.eta()) > 0.06 ): continue;
            if (self.deltaPhi(t.phi(),scluster.phi()) > 0.1 ):
                   continue;
            self.hnum1.append(pt)
            if ( pt > 20 ):
              self.hetanum1.append(t.eta())
              self.hphinum1.append(t.phi())
            rEta=-999.0
            eRatio=-999.0
            if ( scluster.e277() > 0.1 ) :
                   rEta = scluster.e237() / scluster.e277()
            if ( rEta < 0.745 ) :
                   continue
            self.hnum2.append(pt)
            if ( pt > 20 ):
              self.hetanum2.append(t.eta())
              self.hphinum2.append(t.phi())
            for eratio in lASP:
               if ( abs(eratio.eta()-scluster.eta()) > 0.035 ): continue;
               if ( self.deltaPhi(eratio.phi(),scluster.phi()) > 0.08):
                   continue;
               if ( eratio.emaxs1() + eratio.e2tsts1() > 0.1 ):
                   eRatio = ( eratio.emaxs1() - eratio.e2tsts1() ) / (eratio.emaxs1() + eratio.e2tsts1())
            if ( eRatio < 0.41 ):
               continue
            self.hnum3.append(pt)
            if ( pt > 20 ):
              self.hetanum3.append(t.eta())
              self.hphinum3.append(t.phi())
            #print t.pt(), t.eta(), t.phi(), scluster.et(), scluster.eta(), scluster.phi(), rEta, eRatio
            self.hnum4.append(pt)
            if ( pt > 20 ):
              self.hetanum4.append(t.eta())
              self.hphinum4.append(t.phi())
            for track in tracks:
               #print dir(track)
               if ( track.pt() < 2e3 ):
                   continue
               if ( abs(track.eta()-scluster.eta()) > 0.02 ): continue;
               if ( self.deltaPhi(track.phi0(),scluster.phi())> 0.03):
                   continue;
               self.trackPt.append(track.pt()/1e3)
               self.trackEta.append(track.eta())

     print "End of List"
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
     hnumphi1 = ROOT.TH1F("numphi1","numphi1",128,-self.pi,self.pi);
     hnumphi2 = ROOT.TH1F("numphi2","numphi2",128,-self.pi,self.pi);
     hnumphi3 = ROOT.TH1F("numphi3","numphi3",128,-self.pi,self.pi);
     hnumphi4 = ROOT.TH1F("numphi4","numphi4",128,-self.pi,self.pi);
     hdeneta = ROOT.TH1F("deneta","deneta",100,-2.5,2.5);
     hdenphi = ROOT.TH1F("denphi","denphi",128,-self.pi,self.pi);
     trackPt = ROOT.TH1F("trackPt","trackPt",160,0,80);
     trackEta = ROOT.TH1F("trackEta","trackEta",100,-2.5,2.5);
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
     for i in self.trackPt:
         trackPt.Fill(i) 

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

     for i in self.hphinum1:
         hnumphi1.Fill(i)
     for i in self.hphinum2:
         hnumphi2.Fill(i)
     for i in self.hphinum3:
         hnumphi3.Fill(i)
     for i in self.hphinum4:
         hnumphi4.Fill(i)
     for i in self.hphiden:
         hdenphi.Fill(i)

     for i in self.trackEta:
         trackEta.Fill(i)

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
     hnumphi1.Write()
     hnumphi2.Write()
     hnumphi3.Write()
     hnumphi4.Write()
     hdenphi.Write()
     trackPt.Write()
     trackEta.Write()
     f.Write()
     f.Close()
     PyAthena.Alg.finalize(self)
     print "In Finalize"
     return StatusCode.Success

