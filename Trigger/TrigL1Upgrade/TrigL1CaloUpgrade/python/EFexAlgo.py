# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

import ROOT
import ROOT.TH1F
from AthenaPython import PyAthena
from AthenaPython.PyAthena import StatusCode

# Class Defined by Denis Oliveira Damazio
# at 2015/11/03 for eFex studies

# class inherits from PyAthena.Alg (so that
# it can be used also in the topSequence

class EFexAlgo (PyAthena.Alg) :

   # constructor
   def __init__(self,name="EFexAlgo"):
     # initialize base class
     PyAthena.Alg.__init__(self,name)
     # prepare some parameters
     self.name = name
     self.sg=None  # storeGate placeholder
     self.ds=None  # detStore (for geometry)
     self.ListOfEt=[] # accumulating histogram info in list (not good!)
     self.ListOfEtVsEta=[]
     self.detaThr=0.07 # delta eta/phi to form a cluster 
     self.dphiThr=0.125
     self.detaTTThr=0.125 # delta eta/phi to form a cluster 
     self.dphiTTThr=0.15
     self.pi = ROOT.TMath.Pi() # just to cache
     return

   # all athena like algorithms need initialize/execute/finalize
   def initialize(self):
     print ("In Initialize") # should change to use msg.log!?
     PyAthena.Alg.initialize(self)
     self.sg = PyAthena.py_svc('StoreGateSvc')
     self.ds = PyAthena.py_svc('DetectorStore')
     # getting local singleton instance good enough to
     # initialize geometry needed for CaloCells
     dd=ROOT.CaloDetDescrManager.instance()  # noqa: F841
     return StatusCode.Success

   # takes from input list only cells with et above Trh
   def findCellsAboveThr(self, list, Thr) :
     output=[]
     for cell in list:
        if (cell.et()> Thr) :
            output.append(cell)
     return output

   # find cells candidate to form a cluster around a seed cell
   def findTTsAround(self, list, one) :
     output=[]
     oneeta=one.eta()
     onephi=one.phi()
     for cell in list:
        deta = ROOT.TMath.Abs( oneeta-cell.eta() )
        if ( deta > self.detaTTThr ) : continue
        dphi = ROOT.TMath.Abs( onephi-cell.phi() )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        if ( dphi > self.dphiTTThr ) : continue
        output.append(cell)
     return output

   # find cells candidate to form a cluster around a seed cell
   def findCellsAround(self, list, one) :
     output=[]
     oneeta=one.eta()
     onephi=one.phi()
     for cell in list:
        deta = ROOT.TMath.Abs( oneeta-cell.eta() )
        if ( deta > self.detaThr ) : continue
        dphi = ROOT.TMath.Abs( onephi-cell.phi() )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        if ( dphi > self.dphiThr ) : continue
        output.append(cell)
     return output

   # find cells candidate to form a cluster around eta/phi point
   def findCellsAroundEtaPhi(self, list, oneeta, onephi) :
     output=[]
     for cell in list:
        deta = ROOT.TMath.Abs( oneeta-cell.eta() )
        if ( deta > self.detaThr ) : continue
        dphi = ROOT.TMath.Abs( onephi-cell.phi() )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        dphi = ROOT.TMath.Abs( dphi - self.pi )
        if ( dphi > self.dphiThr ) : continue
        output.append(cell)
     return output

   #check if a cell is the one with highest energy in list
   # help forming local maximum based cluster
   def isCellEmMaximum(self,list,celltested) :
      if (celltested.caloDDE().getSampling() >=8 ) : return False
      et = celltested.et()
      for cell in list:
         if ( cell.caloDDE().getSampling() >=8 ) : continue
         if ( cell.et() > et ) : return False
      return True

   # sums energy in EM cells in list (FCAL not included)
   def sumEmCells(self,list):
      TotalSum=0.0
      for cell in list:
        if ( cell.caloDDE().getSampling() >=8 ) : continue
        TotalSum+=cell.et()
      return TotalSum

   # sums energy in Had cells in list (FCAL not included)
   def sumHadCells(self,list):
      TotalSum=0.0
      for cell in list:
        samp = cell.caloDDE().getSampling()
        if ( (samp < 8) or (samp>=20) ) : continue
        TotalSum+=cell.et()
      return TotalSum

   # sums hadronic energy in TTs in Tile region (lacking from
   # SuperCell container
   def sumHadTT(self,list):
      TotalSum=0.0
      for TT in list:
        print ( "TT explore",TT.eta() , TT.sampling(), TT.pt())
        if ( ROOT.TMath.Abs( TT.eta() )>1.72 ) : continue
        if ( TT.sampling() == 0 ) : continue
        TotalSum+=(TT.pt()*1e3)
      return TotalSum

   # method executed once per event. Main loop.
   def execute(self):
     print ("In Execute")
     # retrieve SuperCell container (in python this is trivial)
     SCells = self.sg['SCell']
     print ("Size of SCells container : ", SCells.size())
     TTs = self.sg['xAODTriggerTowers']
     print ("Size of TT container : ", TTs.size())
     TPs = self.sg['TruthParticles']
     print ("Size of TP container : ",TPs.size())

     # method to get all cells above a threshold (seeds for clusters)
     listAboveThr=self.findCellsAboveThr(SCells,3e3)

     oneEt = self.ListOfEt
     twoEt = self.ListOfEtVsEta
     for scell in listAboveThr:
         # for each seed build a list of cells around it
         print ('cell aboveThr : ',scell.et())
         cellsOfCluster=self.findCellsAround(SCells,scell)
         for c in cellsOfCluster:
            print ('\t',c.et())
         # check if cell is em and hottested around it
         if ( not self.isCellEmMaximum(cellsOfCluster,scell) ) : continue
         print ('continue')
         TTOfCluster=self.findTTsAround(TTs,scell)
         # sum the et of all cells in EM layer
         clusterEmEnergy = self.sumEmCells(cellsOfCluster)
         # sum the et of all cells in HAD layer
         clusterHadEnergy = self.sumHadCells(cellsOfCluster)
         clusterHadEnergy += self.sumHadTT(TTOfCluster)
         # for now make a list in place of histograms
         oneEt.append(clusterEmEnergy/1e3)
         twoEt.append([scell.eta(),clusterHadEnergy/1e3])
         if ( clusterEmEnergy > 5e2 ) :
             print ("Results : ",scell.eta(),scell.phi(),clusterEmEnergy, clusterHadEnergy)
     for truth in TPs:
         if not ( (truth.absPdgId() == 11) or (truth.pdgId()==22) ) : continue
         if not ( truth.pt() > 1e3 ) : continue
         if not ( ROOT.TMath.Abs(truth.eta()) <2.5 ) : continue
         print ("Truth : ",truth.pt(), truth.eta(), truth.phi())


     print ("End of List")
     return StatusCode.Success

   # algorithm finalize also helps by creating, filling and
   # saving histograms
   def finalize(self):
     OutputFile=self.name+".root"
     f=ROOT.TFile(OutputFile,'RECREATE')
     h1 = ROOT.TH1F("SCellEt","SCellEt",100,0,20)
     h2 = ROOT.TH2F("SCellEtVsEta","SCellEtVsEta",50,-5.0,5.0,100,0,20)
     for x in self.ListOfEt:
        h1.Fill( x )
     for x in self.ListOfEtVsEta:
        h2.Fill( x[0],x[1] )
     h1.Write()
     h2.Write()
     f.Write()
     f.Close()
     PyAthena.Alg.finalize(self)
     print ("In Finalize")
     return StatusCode.Success

