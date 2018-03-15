# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration


import ROOT
import ROOT.TH1F
from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode

class eFexEratioAlgoRate (PyAthena.Alg) :

   def __init__(self,name="eFexEratioAlgoRate"):
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
     self.trackPt = []
     self.trackEta = []
     self.pdgTruth1 =[]
     self.pdgTruth2 =[]
     self.pdgTruth3 =[]
     self.pdgTruth4 =[]
     self.pdgTruth1_2GeV =[]
     self.pdgTruth2_2GeV =[]
     self.pdgTruth3_2GeV =[]
     self.pdgTruth4_2GeV =[]
     self.pdgTruth1_4GeV =[]
     self.pdgTruth2_4GeV =[]
     self.pdgTruth3_4GeV =[]
     self.pdgTruth4_4GeV =[]
     return StatusCode.Success

   def deltaPhi(self,phi1,phi2):
      dphi = abs(phi1-phi2);
      dphi = abs(self.pi-dphi);
      dphi = abs(self.pi-dphi);
      return dphi;

   def execute(self):

     truths = self.sg['TruthParticles']
     eFex = self.sg['SCluster']
     lASP = self.sg['LArLayer1Vars']
     tracks = self.sg['InDetTrackParticles']
     self.hden.append(1)
     for scluster in eFex:
         pt = scluster.et()*1e-3
         teta = scluster.eta()
         if ( scluster.et() < 20e3 ): continue;
         list_of_part = []
         for t in truths:
           if ( (t.pt() > 1e3) and (t.barcode() < 10000) and (t.status()==1) ):
               continue
           if ( abs(t.eta()-scluster.eta()) > 0.15 ) :
               continue;
           if ( self.deltaPhi(t.phi(),scluster.phi()) > 0.15 ) :
               continue;
           list_of_part.append( t )
           if ( t.pt() > 2e3 ) :
                self.pdgTruth1_2GeV.append( t.pdgId() )
           if ( t.pt() > 4e3 ) :
                self.pdgTruth1_4GeV.append( t.pdgId() )
           self.pdgTruth1.append( t.pdgId() )
         self.hnum1.append(pt)
         self.hetanum1.append(teta)
         rEta=-999.0
         eRatio=-999.0
         if ( scluster.e277() > 0.1 ) :
                rEta = scluster.e237() / scluster.e277()
         if ( rEta < 0.745 ) :
                continue
         self.hnum2.append(pt)
         self.hetanum2.append(teta)
         for t in list_of_part:
            self.pdgTruth2.append( t.pdgId() )
            if ( t.pt() > 2e3 ):
               self.pdgTruth2_2GeV.append( t.pdgId() )
            if ( t.pt() > 4e3 ):
               self.pdgTruth2_4GeV.append( t.pdgId() )
         for eratio in lASP:
            if ( abs(eratio.eta()-scluster.eta()) > 0.035 ): continue;
            if ( self.deltaPhi(eratio.phi(),scluster.phi()) > 0.08):
                continue;
            if ( eratio.emaxs1() + eratio.e2tsts1() > 0.1 ):
                eRatio = ( eratio.emaxs1() - eratio.e2tsts1() ) / (eratio.emaxs1() + eratio.e2tsts1())
         if ( eRatio < 0.41 ):
            continue
         self.hnum3.append(pt)
         self.hetanum3.append(teta)
         for t in list_of_part:
            self.pdgTruth3.append( t.pdgId() )
            if ( t.pt() > 2e3 ):
               self.pdgTruth3_2GeV.append( t.pdgId() )
            if ( t.pt() > 4e3 ):
               self.pdgTruth3_4GeV.append( t.pdgId() )
         self.hnum4.append(pt)
         self.hetanum4.append(teta)
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
     hdeneta = ROOT.TH1F("deneta","deneta",100,-2.5,2.5);
     trackPt = ROOT.TH1F("trackPt","trackPt",160,0,80);
     trackEta = ROOT.TH1F("trackEta","trackEta",100,-2.5,2.5);
     pdgTruth1 = ROOT.TH1F("pdgTruth1","pdgTruth1",1001,-500.5,500.5)
     pdgTruth2 = ROOT.TH1F("pdgTruth2","pdgTruth2",1001,-500.5,500.5)
     pdgTruth3 = ROOT.TH1F("pdgTruth3","pdgTruth3",1001,-500.5,500.5)
     pdgTruth1_4GeV = ROOT.TH1F("pdgTruth1_4GeV","pdgTruth1_4GeV",1001,-500.5,500.5)
     pdgTruth2_4GeV = ROOT.TH1F("pdgTruth2_4GeV","pdgTruth2_4GeV",1001,-500.5,500.5)
     pdgTruth3_4GeV = ROOT.TH1F("pdgTruth3_4GeV","pdgTruth3_4GeV",1001,-500.5,500.5)
     pdgTruth1_2GeV = ROOT.TH1F("pdgTruth1_2GeV","pdgTruth1_2GeV",1001,-500.5,500.5)
     pdgTruth2_2GeV = ROOT.TH1F("pdgTruth2_2GeV","pdgTruth2_2GeV",1001,-500.5,500.5)
     pdgTruth3_2GeV = ROOT.TH1F("pdgTruth3_2GeV","pdgTruth3_2GeV",1001,-500.5,500.5)
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
     for i in self.trackEta:
         trackEta.Fill(i)

     for i in self.pdgTruth1:
        pdgTruth1.Fill( i )
     for i in self.pdgTruth2:
        pdgTruth2.Fill( i )
     for i in self.pdgTruth3:
        pdgTruth3.Fill( i )

     for i in self.pdgTruth1_2GeV:
        pdgTruth1_2GeV.Fill( i )
     for i in self.pdgTruth2_2GeV:
        pdgTruth2_2GeV.Fill( i )
     for i in self.pdgTruth3_2GeV:
        pdgTruth3_2GeV.Fill( i )

     for i in self.pdgTruth1_4GeV:
        pdgTruth1_4GeV.Fill( i )
     for i in self.pdgTruth2_4GeV:
        pdgTruth2_4GeV.Fill( i )
     for i in self.pdgTruth3_4GeV:
        pdgTruth3_4GeV.Fill( i )

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
     trackPt.Write()
     trackEta.Write()
     pdgTruth1.Write()
     pdgTruth2.Write()
     pdgTruth3.Write()
     pdgTruth1_2GeV.Write()
     pdgTruth2_2GeV.Write()
     pdgTruth3_2GeV.Write()
     pdgTruth1_4GeV.Write()
     pdgTruth2_4GeV.Write()
     pdgTruth3_4GeV.Write()
     f.Write()
     f.Close()
     PyAthena.Alg.finalize(self)
     print "In Finalize"
     return StatusCode.Success

