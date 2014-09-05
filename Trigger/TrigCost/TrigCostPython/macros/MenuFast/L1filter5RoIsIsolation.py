#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
from math import sqrt,atan2
from array import array

highfiles="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_8TeV_119996.NTUP.v1.120724225239/*.root"
high14files="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_14TeV_119996.NTUP.v1/*.root"
high14filesv2="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_14TeV_119996.NTUP.v2/*.root"
low14files="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_14TeV_119995.NTUP.v1/*.root"
lowfiles="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_8TeV_119995.NTUP.v2.120725104911/*.root"
datafiles="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.data12_8TeV_200805.NTUP.v2.120725140208/*.root"
#ebfiles="/afs/cern.ch/user/a/aagaard/eos/atlas/atlasdatadisk/data12_8TeV/NTUP_TRIG/r3837_r3838_p931/data12_8TeV.00202798.physics_EnhancedBias.merge.NTUP_TRIG.r3837_r3838_p931_tid00921217_00/*.root.1"
ebfiles="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.data12_8TeV_202798.NTUP.v3/*.root"
#ebfiles="/tmp/aagaard/EB/*.root.1"
jffiles="/afs/cern.ch/user/a/aagaard/work/14TeV/user.aagaard.mc12_14TeV_129160.NTUP.v0.120801123112/*.root"


mu27_25ns_files1="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu27.NTUP_TRIG.v3.130118172739/user.aagaard.000089.StreamNTUP_L1CaloPhysics*root"
mu27_25ns_files2="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu27.NTUP_TRIG.v3.130119100542/user.aagaard.000089.StreamNTUP_L1CaloPhysics*root"

mu27_25ch=ROOT.TChain("physics")
mu27_25ch.Add(mu27_25ns_files1)
mu27_25ch.Add(mu27_25ns_files2)

mu54_25ns_files="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu54_25ns.NTUP_TRIG.v3.130305171440/user.aagaard.000097.StreamNTUP_L1CaloPhysics*root*"
mu54_25ch=ROOT.TChain("physics")
print mu54_25ch.Add(mu54_25ns_files)

mu81_25ns_files="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu81_25ns.NTUP_TRIG.v3.130305172518/user.aagaard.000099.StreamNTUP_L1CaloPhysics*root*"
mu81_25ch=ROOT.TChain("physics")
print mu81_25ch.Add(mu81_25ns_files)

mu54_50ns_files="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu54_50ns.NTUP_TRIG.v3.130305194037/user.aagaard.000095.StreamNTUP_L1CaloPhysics*root*"
mu54_50ch=ROOT.TChain("physics")
print mu54_50ch.Add(mu54_50ns_files)

mu81_50ns_files="/afs/cern.ch/user/m/mbackes/eos/atlas/atlaslocalgroupdisk/dq2/user/aagaard/mc12_14TeV_159000/user.aagaard.mc12_14TeV_159000.mu81_50ns.NTUP_TRIG.v3.130305195819/user.aagaard.000101.StreamNTUP_L1CaloPhysics*root*"
mu81_50ch=ROOT.TChain("physics")
print mu81_50ch.Add(mu81_50ns_files)


goodBCID=range(13,73)+range(93,153)+range(173,233)+range(253,313)
highch=ROOT.TChain("physics")
highch.Add(highfiles)

high14ch=ROOT.TChain("physics")
high14ch.Add(high14files)

high14chv2=ROOT.TChain("physics")
high14chv2.Add(high14filesv2)

lowch=ROOT.TChain("physics")
lowch.Add(lowfiles)

low14ch=ROOT.TChain("physics")
low14ch.Add(low14files)

datach=ROOT.TChain("physics")
datach.Add(datafiles)

jfch=ROOT.TChain("physics")
jfch.Add(jffiles)

ebch=ROOT.TChain("physics")
ebch.Add(ebfiles)

weights={}
for line in open('EBweights.txt',"r").readlines():
    words=line.split(' ')
    weights[ (int(words[1]),int(words[2])) ]=float(words[3])

exts=['_pt','_eta','_phi']

def filter(name,nt,dataCut=False,ebsum=False,bcid=False):
    cnt=0
    nt.SetBranchStatus("*",False)
    nt.SetBranchStatus("timestamp*",True)
    nt.SetBranchStatus("trig_L1*",True)
    if bcid:
        nt.SetBranchStatus("bcid",True)
    file=ROOT.TFile("sortedWithIso/"+name+".root","RECREATE")
    print 'Filtering:',name
#    newNT=ROOT.TNtuple('physics','reduced rate tuple','EM1:EM2:EMH1:EMH2:TAU1:TAU2:TAU1I:TAU2I:JET1:JET4:MU1:MU2:MUB1:MET:weight')
    newNT=ROOT.TTree('physics','reduced rate tuple')
    vars={'EM':    5,
          'EMH':   5,
          'EMHI':  5,
          'TAU':   5,
          'TAUI':  5,
          'JET':   5,
          'FJ':    2,
          'FJE':   2,
          'MU':    5,
          'MUB':   2,
          'MUE':   2,
          'MUEC':   2,
          'MET':   1,
          }
    for var in vars:
        for ext in exts:
            name=var+ext
            globals()[name]=array('f',vars[var]*[0])
            newNT.Branch(name,globals()[name],name+('[%d]/F' % vars[var]))
        if "EM" in var or "TAU" in var:
            name=var+"_EMIsol"
            globals()[name]=array('f',vars[var]*[0])
            newNT.Branch(name,globals()[name],name+('[%d]/F' % vars[var]))
    weight=array('f',[0])
    newNT.Branch('weight',weight,'weight/F')
    
    newNT.SetDirectory(file)
    while nt.GetEntry(cnt):
        cnt+=1
        weight[0]=1
        if ebsum:
            key=(int(nt.timestamp),int(nt.timestamp_ns))
            if key in weights:
                weight[0]=1./weights[key]
            else:
                print "Failed to find weight for",key
                weight[0]=0
        if cnt%1000==0: print cnt
        if bcid and not nt.bcid in goodBCID: continue
        if dataCut:
            if nt.lbn<215: continue
            if nt.lbn>=400: continue
            mbts=False
            for trig in nt.trig_EF_passedPhysics:
                if trig==18: mbts=True
            if not mbts: continue
        if ebsum and False:
            MET_pt[0]=sqrt(nt.trig_L1_esum_ExMiss*nt.trig_L1_esum_ExMiss+
                     nt.trig_L1_esum_EyMiss*nt.trig_L1_esum_EyMiss)
            MET_phi[0]=atan2(nt.trig_L1_esum_EyMiss,nt.trig_L1_esum_ExMiss)
        else:
            MET_pt[0]=sqrt(nt.trig_L1_energySum_ExMiss*nt.trig_L1_energySum_ExMiss+
                     nt.trig_L1_energySum_EyMiss*nt.trig_L1_energySum_EyMiss)
            MET_phi[0]=atan2(nt.trig_L1_energySum_EyMiss,nt.trig_L1_energySum_ExMiss)

        TAUs=5*[(0,0,0,0)]
        EMs=5*[(0,0,0,0)]
        TAUIs=5*[(0,0,0,0)]
        EMHs=5*[(0,0,0,0)]
        EMHIs=5*[(0,0,0,0)]
        for n in range(nt.trig_L1_emtau_n):
            em=(nt.trig_L1_emtau_EMClus[n],nt.trig_L1_emtau_eta[n],nt.trig_L1_emtau_phi[n],nt.trig_L1_emtau_EMIsol[n])
            EMs.append(em)
            if nt.trig_L1_emtau_hadCore[n]<=1000:
                EMHs.append(em)
                if nt.trig_L1_emtau_EMIsol[n]<=2000:
                    EMHIs.append(em)
            tau=(nt.trig_L1_emtau_tauClus[n],nt.trig_L1_emtau_eta[n],nt.trig_L1_emtau_phi[n],nt.trig_L1_emtau_EMIsol[n])
            TAUs.append(tau)
            if nt.trig_L1_emtau_EMIsol[n]<=4000:
                TAUIs.append(tau)
        TAUs=sorted(TAUs,reverse=True)
        TAUIs=sorted(TAUIs,reverse=True)
        EMs=sorted(EMs,reverse=True)
        EMHs=sorted(EMHs,reverse=True)
        EMHIs=sorted(EMHIs,reverse=True)

        JETs=5*[(0,0,0)]
        FJs=2*[(0,0,0)]
        FJEs=2*[(0,0,0)]
        for  n in range(nt.trig_L1_jet_n):
            jet=(nt.trig_L1_jet_et8x8[n],nt.trig_L1_jet_eta[n],nt.trig_L1_jet_phi[n])
            if abs(nt.trig_L1_jet_eta[n])<3.2:
                JETs.append(jet)
            if abs(nt.trig_L1_jet_eta[n])>3.2:
                FJs.append(jet)
            if abs(nt.trig_L1_jet_eta[n])>2.4:
                FJEs.append(jet)
        JETs=sorted(JETs,reverse=True)
        FJEs=sorted(FJEs,reverse=True)
        FJs=sorted(FJs,reverse=True)
        MUs=5*[(0,0,0)]
        MUBs=3*[(0,0,0)]
        MUEs=3*[(0,0,0)]
        MUECs=3*[(0,0,0)]
        for  n in range(nt.trig_L1_mu_n):
            if nt.trig_L1_mu_vetoed[n]: continue
            mu=(nt.trig_L1_mu_pt[n],nt.trig_L1_mu_eta[n],nt.trig_L1_mu_phi[n])
            MUs.append(mu)
            if abs(nt.trig_L1_mu_source[n])==0:
                MUBs.append(mu)
            else:
                MUEs.append(mu)
                if abs(nt.trig_L1_mu_source[n])==1:
                    MUECs.append(mu)
        MUs=sorted(MUs,reverse=True)
        MUBs=sorted(MUBs,reverse=True)
        MUEs=sorted(MUEs,reverse=True)
        MUECs=sorted(MUECs,reverse=True)

        for var in vars:
            if var=='MET': continue # special handling above
            for idx, ext in enumerate(exts):
                name=var+ext
                for n in range(vars[var]):
                    globals()[name][n]=locals()[var+'s'][n][idx]
            if "EM" in var or "TAU" in var:
                idx = 3
                name=var+"_EMIsol"
                for n in range(vars[var]):
                    globals()[name][n]=locals()[var+'s'][n][idx]

        newNT.Fill()
        
    file.Write()
    file.Close()
    
#filter("high8TeV",highch)
#filter("low8TeV",lowch)
#filter("high14TeV",high14ch)
#filter("low14TeV",low14ch)


#filter("data",datach,True)
#filter("high8TeV",highch)

#filter("ebtest",ebch,ebsum=True)

filter("mu27_25ns",mu27_25ch)
filter("mu27_25ns_bcid",mu27_25ch,bcid=True)
filter("mu54_25ns",mu54_25ch)
filter("mu54_25ns_bcid",mu54_25ch,bcid=True)
filter("mu81_25ns",mu81_25ch)
filter("mu81_25ns_bcid",mu81_25ch,bcid=True)
filter("mu54_50ns",mu54_50ch)
filter("mu54_50ns_bcid",mu54_50ch,bcid=True)
filter("mu81_50ns",mu81_50ch)
filter("mu81_50ns_bcid",mu81_50ch,bcid=True)
filter("high14TeV_v2",high14chv2)

#filter("jf",jfch)
