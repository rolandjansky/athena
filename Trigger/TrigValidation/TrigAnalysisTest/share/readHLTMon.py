
from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

import sys,ROOT

from ROOT import TH1F,TH1D,TFile
from ROOT import gDirectory

myfile = TFile('histo.root')

#########  Egamma ###################

## LVL1

h  = TH1F()
h1 = TH1F()
h2 = TH1F()
h3 = TH1F()
h4 = TH1F()

h = myfile.Get("HLT/EgammaMon/L1/EmIsol;1")
h1 = myfile.Get("HLT/EgammaMon/L1/EmClus;1")
h2 = myfile.Get("HLT/EgammaMon/L1/Eta;1")
h3 = myfile.Get("HLT/EgammaMon/L1/HadCore;1")
h4 = myfile.Get("HLT/EgammaMon/L1/HadIsol;1")
h5 = myfile.Get("HLT/EgammaMon/L1/Phi;1")

##  L2Calo

g = TH1F()
g1 = TH1F()
g2 = TH1F()
g3 = TH1F()
g4 = TH1F()
g5 = TH1F()

g = myfile.Get("HLT/EgammaMon/L2Calo/Et;1")
g1 = myfile.Get("HLT/EgammaMon/L2Calo/HadEt;1")
g2 = myfile.Get("HLT/EgammaMon/L2Calo/Rcore;1")
g3 = myfile.Get("HLT/EgammaMon/L2Calo/Rstrip;1")
g4 = myfile.Get("HLT/EgammaMon/L2Calo/eta;1")
g5 = myfile.Get("HLT/EgammaMon/L2Calo/phi;1")

################  Muon  ##############################

f = TH1F()
f1 = TH1F()
f2 = TH1F()
f3 = TH1F()
f4 = TH1F()
f5 = TH1F()

f = myfile.Get("HLT/MuonMon/Common_Counter;1")
f1 = myfile.Get("HLT/MuonMon/Offline_Number_Of_LPT_Muon;1")
f2 = myfile.Get("HLT/MuonMon/Offline_Number_Of_Muon;1")
f3 = myfile.Get("HLT/MuonMon/Offline_Number_Of_Selected_CB_Muon;1")
f4 = myfile.Get("HLT/MuonMon/Offline_Number_Of_CB_Muon;1")
f5 = myfile.Get("HLT/MuonMon/Offline_Number_Of_SA_Muon;1")

#################  ResultsMon   ##########################

m = TH1F()
m1 = TH1F()
m2 = TH1F()


m = myfile.Get("HLT/ResultMon/HLTResultEF;1")
m1 = myfile.Get("HLT/ResultMon/HLTResultL2;1")
m2 = myfile.Get("HLT/ResultMon/L1Events;1")


mynewfile = TFile('histo_new.root','recreate')

mynewfile.mkdir('HLT')
mynewfile.cd('HLT')
gDirectory.pwd()
gDirectory.mkdir('EgammaMon')
mynewfile.cd('HLT/EgammaMon')
gDirectory.pwd()
gDirectory.mkdir('L1')
mynewfile.cd('HLT/EgammaMon/L1')

h.Write()
h1.Write()
h2.Write()
h3.Write()
h4.Write()
h5.Write()

gDirectory.cd('../')
gDirectory.pwd()
gDirectory.mkdir('L2Calo')
mynewfile.cd('HLT/EgammaMon/L2Calo')

g.Write()
g1.Write()
g2.Write()
g3.Write()
g4.Write()
g5.Write()

gDirectory.cd('../../')
gDirectory.pwd()
gDirectory.mkdir('MuonMon')
gDirectory.cd('MuonMon')

f.Write()
f1.Write()
f2.Write()
f3.Write()
f4.Write()
f5.Write()

gDirectory.cd('../')
gDirectory.pwd()
gDirectory.mkdir('ResultMon')
gDirectory.cd('ResultMon')

m.Write();
m1.Write();
m2.Write();

theApp.EvtMax=1
