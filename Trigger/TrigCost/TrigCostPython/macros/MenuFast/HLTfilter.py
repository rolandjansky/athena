#!/usr/bin/env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

import ROOT
from math import sqrt,atan2,cosh,pi
from array import array

sampledir="/afs/cern.ch/user/n/nakahama/work/susy/wrk_space/LS1/data/"

mu54_25ns_file=sampledir+"user.nakahama.mc12_14TeV.159000.MB.25ns.mu54.e1564_s1499_s1504_r4326_r4371_p1328.susy_filt.all.v6_st1_28July2013.130731020546/all10-14/user.nakahama.000341._0001*.root"

# 40files
jf17_8TeVnew_file1=sampledir+"user.nakahama.mc12_8TeV.129160.JF17.e1130_s1472_s1473_r3542_r3549_p1328.susy_filt.v6_st1_28July2013.130731220837/user.nakahama.000405._0001*.root"
#jf17_8TeVnew_file2=sampledir+"user.nakahama.mc12_8TeV.129160.JF17.e1130_s1472_s1473_r3542_r3549_p1328.susy_filt.v6_st1_28July2013.130731220837/user.nakahama.000405._0002?.output.root"
#jf17_8TeVnew_file3=sampledir+"user.nakahama.mc12_8TeV.129160.JF17.e1130_s1472_s1473_r3542_r3549_p1328.susy_filt.v6_st1_28July2013.130731220837/user.nakahama.000405._0003?.output.root"

# photon ntuples
#sampledir+"user.nakahama.mc12_14TeV.129160.JF17.e1313_s1682_s1691_r4634_r4643.ph_filt.all.v6_st1_28July2013.130731020309/user.nakahama.*root*"
jf17_14TeV_file1=sampledir+"user.nakahama.mc12_14TeV.129160.JF17.e1313_s1682_s1691_r4634_r4643.ph_filt.all.v6_st1_28July2013.130731020309/st0/user.nakahama.000243._00*.root"
#jf17_14TeV_file2=sampledir+"user.nakahama.mc12_14TeV.129160.JF17.e1313_s1682_s1691_r4634_r4643.ph_filt.all.v6_st1_28July2013.130731020309/st1/user.nakahama.000243._00*.root"
#jf17_14TeV_file3=sampledir+"user.nakahama.mc12_14TeV.129160.JF17.e1313_s1682_s1691_r4634_r4643.ph_filt.all.v6_st1_28July2013.130731020309/st2/user.nakahama.000243._00*.root"         

eb_file=sampledir+"eb202798_28july2013_st1_susy_filt/all0-50/susy_filtinputFiles*.txt.root"

ttbar_file=sampledir+"user.nakahama.mc12_14TeV.105200.McAtNlo_ttbar_LFilt.e1565_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731023319/user.nakahama.*root*"
Wplusenu_file=sampledir+"user.nakahama.mc12_14TeV.147800.Wplusenu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731021723/user.nakahama.*root*"
Wplusmunu_file=sampledir+"user.nakahama.mc12_14TeV.147801.Wplusmunu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731021922/user.nakahama.*root*"
Wminusenu_file=sampledir+"user.nakahama.mc12_14TeV.147803.Wminenu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731022315/user.nakahama.*root*"
Wminusmunu_file=sampledir+"user.nakahama.mc12_14TeV.147804.Wminmunu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731022119/user.nakahama.*root*"
Zee_file=sampledir+"user.nakahama.mc12_14TeV.147806.Zee.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731022920/user.nakahama.*root*"
Zmumu_file=sampledir+"user.nakahama.mc12_14TeV.147807.Zmumu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731023123/user.nakahama.*root*"

ttbar_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.105200.McAtNlo_ttbar_LFilt.e1513_s1499_s1504_r3945_r3549_p1328.susy_nofilt.all.v6_st1_28July2013.130731023514/user.nakahama.*root*"
Wplusenu_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147800.Wplusenu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731020819/user.nakahama.*root*"
Wplusmunu_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147801.Wplusmunu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731021102/user.nakahama.*root*"
Wminusenu_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147803.Wminenu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731021520/user.nakahama.*root*"
Wminusmunu_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147804.Wminmunu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731021310/user.nakahama.*root*"
Zee_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147806.Zee.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731022510/user.nakahama.*root*"
Zmumu_8TeV_file=sampledir+"user.nakahama.mc12_8TeV.147807.Zmumu.e1564_s1499_s1504_r4033_r4030_p1181.susy_nofilt.all.v6_st1_28July2013.130731022712/user.nakahama.*root*"


mu54_25ns=ROOT.TChain("susy")
mu54_25ns.Add(mu54_25ns_file)
'''
jf17_8TeVnew1=ROOT.TChain("susy")
jf17_8TeVnew1.Add(jf17_8TeVnew_file1)
#jf17_8TeVnew2=ROOT.TChain("susy")
#jf17_8TeVnew2.Add(jf17_8TeVnew_file2)
#jf17_8TeVnew3=ROOT.TChain("susy")
#jf17_8TeVnew3.Add(jf17_8TeVnew_file3)


jf17_14TeV1=ROOT.TChain("photon")
jf17_14TeV1.Add(jf17_14TeV_file1)

#jf17_14TeV2=ROOT.TChain("photon")
#jf17_14TeV2.Add(jf17_14TeV_file2)
#jf17_14TeV3=ROOT.TChain("photon")
#jf17_14TeV3.Add(jf17_14TeV_file3)      

'''
eb=ROOT.TChain("susy")
eb.Add(eb_file)
'''
ttbar=ROOT.TChain("susy")
ttbar.Add(ttbar_file)

Wplusenu=ROOT.TChain("susy")
Wplusenu.Add(Wplusenu_file)
Wplusmunu=ROOT.TChain("susy")
Wplusmunu.Add(Wplusmunu_file)
Wminusenu=ROOT.TChain("susy")
Wminusenu.Add(Wminusenu_file)
Wminusmunu=ROOT.TChain("susy")
Wminusmunu.Add(Wminusmunu_file)
Zee=ROOT.TChain("susy")
Zee.Add(Zee_file)
Zmumu=ROOT.TChain("susy")
Zmumu.Add(Zmumu_file)

ttbar_8TeV=ROOT.TChain("susy")
ttbar_8TeV.Add(ttbar_8TeV_file)
Wplusenu_8TeV=ROOT.TChain("susy")
Wplusenu_8TeV.Add(Wplusenu_8TeV_file)
Wminusenu_8TeV=ROOT.TChain("susy")
Wminusenu_8TeV.Add(Wminusenu_8TeV_file)
Wplusmunu_8TeV=ROOT.TChain("susy")
Wplusmunu_8TeV.Add(Wplusmunu_8TeV_file)
Wminusmunu_8TeV=ROOT.TChain("susy")
Wminusmunu_8TeV.Add(Wminusmunu_8TeV_file)
Zee_8TeV=ROOT.TChain("susy")
Zee_8TeV.Add(Zee_8TeV_file)
Zmumu_8TeV=ROOT.TChain("susy")
Zmumu_8TeV.Add(Zmumu_8TeV_file)
'''

#https://svnweb.cern.ch/trac/atlasoff/browser/Trigger/TrigHypothesis/TrigEgammaHypo/trunk/src/TrigEFPhotonHypo.cxx#L467
def isPHloose(Et,eta,weta2,emax2,ethad_over_et,Reta37):
    eta_range=[0,0.8,1.5,1.8,2.0,2.5]
    
    Et_cut=[20.,20.,20.,20.,20.,20.]
    Ethad_cut=[0.051825,0.340496,0.203263,0.393187,0.051825]
    weta2_cut=[0.017513,0.012334,0.012813,0.011028,0.012000]
    emax2_cut=[0.420467,0.756967,0.393126,0.002148,0.750000] #energy of 2nd maximum in 1st sampling ?? emax2_emax1Cut
    Reta37_cut=[0.944226,0.846195,0.929666,0.927025,0.929666]
    # emax2 = ((shower->e2tsts1())-(shower->emins1()))/1000 ???
    
    for i in range(len(eta_range)-1):
        if abs(eta)>=eta_range[i] and abs(eta)<eta_range[i+1]:#and (i+1)<len(eta_range)
            if ethad_over_et<=Ethad_cut[i] and weta2<=weta2_cut[i] and Reta37>Reta37_cut[i]:
            #if ethad_over_et<=Ethad_cut[i] and weta2<=weta2_cut[i] and emax2<emax2_cut[i] and Reta37>Reta37_cut[i]:
                #if Et/1000.>=Et_cut[i]:
                #print 'found'
                return True
    return False

def isPHmedium(ET,eta,weta2,emax1,emax2,ethad_over_et,Reta37):
    eta_range=[0.0,0.8,1.5,1.8,2.0,2.5]
    
    Et_cut=[20.,20.,20.,20.,20.,20.]
    weta2_cut=[0.017513,0.012334,0.012813,0.0111028,0.012000]
    DEmaxs1_cut=[0.60,0.76,0.67,0.68,0.68,0.78]
    #What should the cut for DEmaxs1 be? I couldn't find it.
    Ethad_cut=[0.051825,0.340496,0.203263,0.393187,0.051825]
    emax2_cut=[0.420467,0.756967,0.393126,0.002148,0.750000] #energy of 2nd maximum in 1st sampling ?? emax2_emax1Cut
    Reta37_cut=[0.944226,0.846195,0.929666,0.927025,0.929666]        
            
    for i in range(len(eta_range)-1):
        if abs(eta)>=eta_range[i] and abs(eta)<eta_range[i+1]:#and (i+1)<len(eta_range) 
            if (emax1+emax2)>0. and ethad_over_et<=Ethad_cut[i] and weta2<=weta2_cut[i] and Reta37>Reta37_cut[i] and (emax1-emax2)/(emax1+emax2)>DEmaxs1_cut[i]:
                return True
    return False    

weights={}
for line in open('EBweights.txt',"r").readlines():
    words=line.split(' ')
    weights[ (int(words[1]),int(words[2])) ]=float(words[3])
    

goodBCID=range(13,73)+range(93,153)+range(173,233)+range(253,313)
goodLBN=range(446,547)+range(549,558)+range(571,663)+range(671,697)+range(699,727)+range(729,787)+range(789,877)+range(879,920)

exts=['_pt','_eta','_phi']

def filter(name,nt,dataCut=False,ebsum=False,bcid=False,jf=False):
    cnt=0
    nt.SetBranchStatus("*",False)
    nt.SetBranchStatus("timestamp*",True)
    nt.SetBranchStatus("trig_L1*",True)
    nt.SetBranchStatus("MET_Egamma10NoTau_RefFinal_*",True)
    nt.SetBranchStatus("MET_LocHadTopo_*",True)
    nt.SetBranchStatus("mu_muid_n",True)
    nt.SetBranchStatus("mu_muid_phi",True)
    nt.SetBranchStatus("mu_muid_eta",True)
    nt.SetBranchStatus("mu_muid_pt",True)
    nt.SetBranchStatus("mu_muid_loose",True)
    nt.SetBranchStatus("mu_muid_ptcone20",True)
    nt.SetBranchStatus("mu_muid_isCombinedMuon",True)
    #nt.SetBranchStatus("trig_L1_mu_source",True)
    if jf==False:
        nt.SetBranchStatus("jet_AntiKt4LCTopo_n",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopo_pt",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopo_E",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopo_eta",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopo_phi",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopo_flavor_weight_MV1",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_n",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_pt",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_E",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_eta",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_phi",True)
        nt.SetBranchStatus("jet_AntiKt10LCTopo_flavor_weight_MV1",True)
    else:
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_n",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_pt",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_E",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_eta",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_phi",True)
        nt.SetBranchStatus("jet_AntiKt4LCTopoJets_flavor_weight_MV1",True)
    nt.SetBranchStatus("trig_EF_jet_a4tc",True)
    nt.SetBranchStatus("tau_n",True) 
    nt.SetBranchStatus("tau_pt",True)
    nt.SetBranchStatus("tau_eta",True)
    nt.SetBranchStatus("tau_phi",True)
    nt.SetBranchStatus("tau_numTrack",True)
    nt.SetBranchStatus("tau_JetBDTSigTight",True)
    nt.SetBranchStatus("tau_JetBDTSigMedium",True)
    nt.SetBranchStatus("tau_JetBDTSigLoose",True)
    nt.SetBranchStatus("tau_author",True)
    nt.SetBranchStatus("ph_n",True) 
    nt.SetBranchStatus("ph_Et",True)
    nt.SetBranchStatus("ph_eta",True)
    nt.SetBranchStatus("ph_phi",True)
    nt.SetBranchStatus("ph_isEMTight",True)
    nt.SetBranchStatus("ph_isEMMedium",True)
    nt.SetBranchStatus("ph_isEMLoose",True)
    nt.SetBranchStatus("ph_cl_eta",True)
    nt.SetBranchStatus("ph_E277",True)
    nt.SetBranchStatus("ph_Ethad1",True)
    nt.SetBranchStatus("ph_E233",True)
    nt.SetBranchStatus("ph_E237",True)
    nt.SetBranchStatus("ph_weta2",True)
    nt.SetBranchStatus("ph_emaxs1",True)
    nt.SetBranchStatus("ph_Emax2",True)
    nt.SetBranchStatus("el_n",True) 
    nt.SetBranchStatus("el_Et",True)
    nt.SetBranchStatus("el_pt",True) 
    nt.SetBranchStatus("el_ptcone20",True)
    nt.SetBranchStatus("el_eta",True)
    nt.SetBranchStatus("el_phi",True)
    nt.SetBranchStatus("el_tightPP",True)
    nt.SetBranchStatus("el_mediumPP",True)
    nt.SetBranchStatus("el_loosePP",True)
    nt.SetBranchStatus("el_author",True)
    nt.SetBranchStatus("trig_EF_topocl_met_*",True)
    nt.SetBranchStatus("trig_EF_met_*",True)
    nt.SetBranchStatus("trig_EF_jet_*",True)
    nt.SetBranchStatus("EF_*",True)
    nt.SetBranchStatus("EventNumber",True)
    nt.SetBranchStatus("RunNumber",True)
    nt.SetBranchStatus("lbn",True)
    if bcid:
        nt.SetBranchStatus("bcid",True)
        print "Setting bcid skipping"
    outfilename=name
    if bcid: outfilename+="_bcid"    
    file=ROOT.TFile("sorted_260813/"+outfilename+"_re1.root","RECREATE")
    print 'Filtering:',name
    newNT=ROOT.TTree('physics','reduced rate tuple')
    vars={'EM':    3,
          'EMH':   3,
          'EMHI':  3,
          'g':        2,
          'etight':    2,
          'eitight':   2,
          'emedium':   2,
          'eimedium':  2,
          'eloose':   2,
          'gloose':   2,
          'gmedium':  2,
          'TAU':  2,
          'TAUI': 2,
          'taumedium':   2,
          'tauloose':   2,
          'jet':   8,
          'JET':   8,
          'bjet':  2,
          'fwdjet':    2,
          'fwdejet':   2,
          'MU':2,
          'mu':    2,
          'mui':   2,
          'mub':   2,
          'mue':   2,
          'muec':  2,
          'xe':   1,
          'EFxe': 1,
          'XE': 1,
          'xeworandom': 1,
          'EFjet': 4,
          'fatjet':  2,
          #'EFJeightzero':  1,
          #'EFJoneonezero': 1,
          #'EFJtwotwozero': 1,
          #'EFJtwoeightzero':1,
          #'EFXEeightzeroTtclcwloose' :1,
          #'EFMUtwofourI': 1,
          #'EFMUthreesix': 1,
          }
    
    for var in vars:
        for ext in exts:
            name=var+ext
            globals()[name]=array('f',vars[var]*[0])
            newNT.Branch(name,globals()[name],name+('[%d]/F' % vars[var]))
    weight=array('f',[0])
    newNT.Branch('weight',weight,'weight/F')
    Ncnt=array('f',[0])
    newNT.Branch('Ncnt',Ncnt,'Ncnt/F')
    lbn=array('f',[0])
    newNT.Branch('lbn',lbn,'lbn/F')
    newNT.SetDirectory(file)

    while nt.GetEntry(cnt):
        if cnt%10000==0: print cnt
        #if cnt>5000: break
        Ncnt[0]=cnt
        cnt+=1
        weight[0]=1
        lbn[0]=nt.lbn
        if ebsum:
            key=(int(nt.timestamp),int(nt.timestamp_ns))
            if key in weights:
                weight[0]=1./weights[key]
            else:
                print "Failed to find weight for",key
                weight[0]=0
            if nt.RunNumber==202798 and nt.EventNumber==28890558:
                weight[0]=0
            if lbn and not nt.lbn in goodLBN:
                continue  #weight[0]=0
        if bcid and not nt.bcid in goodBCID:  continue
                
        if dataCut:
            if nt.lbn<215: continue
            if nt.lbn>=400: continue
            mbts=False
            for trig in nt.trig_EF_passedPhysics:
                if trig==18: mbts=True
            if not mbts: continue

        xe=1*[(0,0,0)]
        EFxe=1*[(0,0,0)]
        xeworandom=1*[(0,0,0)]
        XE=1*[(0,0,0)]
        if jf==False:
            xe_pt[0]=sqrt(nt.MET_LocHadTopo_etx*nt.MET_LocHadTopo_etx+
                     nt.MET_LocHadTopo_ety*nt.MET_LocHadTopo_ety)
            xe_phi[0]=atan2(nt.MET_LocHadTopo_ety,nt.MET_LocHadTopo_etx)
            EFxe_pt[0]=sqrt(nt.trig_EF_topocl_met_MEx*nt.trig_EF_topocl_met_MEx+
                     nt.trig_EF_topocl_met_MEy*nt.trig_EF_topocl_met_MEy)
            EFxe_phi[0]=atan2(nt.trig_EF_topocl_met_MEy,nt.trig_EF_topocl_met_MEx)
            XE_pt[0]=sqrt(nt.trig_L1_esum_ExMiss*nt.trig_L1_esum_ExMiss+
                               nt.trig_L1_esum_EyMiss*nt.trig_L1_esum_EyMiss)
            XE_phi[0]=atan2(nt.trig_L1_esum_EyMiss,nt.trig_L1_esum_ExMiss) 
            if ebsum and weight[0]<1000:
                xeworandom_pt[0]=xe_pt[0]
                xeworandom_phi[0]=xe_phi[0]
            else:
                xeworandom_pt[0]=0.
                xeworandom_phi[0]=0.
        else: 
            xe_pt[0]=0. 

        TAUs=2*[(0,0,0)]
        TAUIs=2*[(0,0,0)]    
        taumediums=2*[(0,0,0)]
        taulooses=2*[(0,0,0)]
        EMs=3*[(0,0,0)]
        EMHs=3*[(0,0,0)]
        EMHIs=3*[(0,0,0)]
        etights=2*[(0,0,0)]
        eitights=2*[(0,0,0)]
        emediums=2*[(0,0,0)]
        eimediums=2*[(0,0,0)]
        elooses=2*[(0,0,0)]
        gs=2*[(0,0,0)]
        gmediums=2*[(0,0,0)]
        glooses=2*[(0,0,0)]
        for n in range(nt.el_n):
            e=(nt.el_Et[n],nt.el_eta[n],nt.el_phi[n])
            if (nt.el_author[n]==1 or nt.el_author[n]==3) and abs(nt.el_eta[n])<2.47 and nt.el_Et[n]>1000.*5.:
                if nt.el_mediumPP[n]>0:
                    emediums.append(e)
                    if (nt.el_ptcone20[n])/nt.el_pt[n] < 0.1:
                        eimediums.append(e)
                if nt.el_tightPP[n]>0:
                    etights.append(e)
                    if (nt.el_ptcone20[n])/nt.el_pt[n] < 0.1:
                        eitights.append(e)
                if nt.el_loosePP[n]>0:
                    elooses.append(e)

        for n in range(nt.ph_n):
            ph=(nt.ph_Et[n],nt.ph_eta[n],nt.ph_phi[n])
            if nt.ph_Et[n]!=0 and nt.ph_E277[n]!=0 and isPHmedium(nt.ph_Et[n],nt.ph_eta[n],nt.ph_weta2[n],nt.ph_emaxs1[n],nt.ph_Emax2[n],nt.ph_Ethad1[n]/nt.ph_Et[n],nt.ph_E237[n]/nt.ph_E277[n]):
                gs.append(ph)

        for ni in range(nt.ph_n):
            ph=(nt.ph_Et[ni],nt.ph_eta[ni],nt.ph_phi[ni])
            dr=999.
            if nt.ph_Et[ni]!=0. and nt.ph_E277[ni]!=0. and isPHloose(nt.ph_Et[ni],nt.ph_eta[ni],nt.ph_weta2[ni],nt.ph_Emax2[ni],nt.ph_Ethad1[ni]/nt.ph_Et[ni],nt.ph_E237[ni]/nt.ph_E277[ni]):
                glooses.append(ph)
                for nd in range(0, ni):
                    deta = abs(nt.ph_eta[ni]-nt.ph_eta[nd])
                    dphi = abs(nt.ph_phi[ni]-nt.ph_phi[nd])
                    if(dphi>pi): dphi = 2.0*pi-dphi
                    dr=pow((dphi*dphi+deta*deta), 0.5)
                    if nt.ph_Et[nd]!=0. and nt.ph_E277[nd]!=0. and isPHloose(nt.ph_Et[nd],nt.ph_eta[nd],nt.ph_weta2[nd],nt.ph_Emax2[nd],nt.ph_Ethad1[nd]/nt.ph_Et[nd],nt.ph_E237[nd]/nt.ph_E277[nd]) and dr < 0.15:
                        glooses.pop()
                        #print "g overlapped",ni, nd, dr
                        break
            
            if nt.ph_Et[ni]!=0. and (nt.ph_emaxs1[ni]!=0 or nt.ph_Emax2[ni]!=0) and nt.ph_E277[ni]!=0. and isPHmedium(nt.ph_Et[ni],nt.ph_eta[ni],nt.ph_weta2[ni],nt.ph_emaxs1[n],nt.ph_Emax2[ni],nt.ph_Ethad1[ni]/nt.ph_Et[ni],nt.ph_E237[ni]/nt.ph_E277[ni]):
                gmediums.append(ph)
                for nd in range(0, ni):
                    deta = abs(nt.ph_eta[ni]-nt.ph_eta[nd])
                    dphi = abs(nt.ph_phi[ni]-nt.ph_phi[nd])
                    if(dphi>pi): dphi = 2.0*pi-dphi
                    dr=pow((dphi*dphi+deta*deta), 0.5)
                    if nt.ph_Et[nd]!=0. and nt.ph_E277[nd]!=0. and isPHmedium(nt.ph_Et[nd],nt.ph_eta[nd],nt.ph_weta2[nd],nt.ph_emaxs1[nd],nt.ph_Emax2[nd],nt.ph_Ethad1[nd]/nt.ph_Et[nd],nt.ph_E237[nd]/nt.ph_E277[nd]) and dr < 0.15:
                        gmediums.pop()
                        #print "g overlapped",ni, nd, dr
                        break

        for n in range(nt.trig_L1_emtau_n):
            em=(nt.trig_L1_emtau_EMClus[n],nt.trig_L1_emtau_eta[n],nt.trig_L1_emtau_phi[n])
            EMs.append(em)
            if nt.trig_L1_emtau_hadCore[n]<=1000:
                EMHs.append(em)
                if nt.trig_L1_emtau_EMIsol[n]<=2000:
                    EMHIs.append(em)
            l1tau=(nt.trig_L1_emtau_tauClus[n],nt.trig_L1_emtau_eta[n],nt.trig_L1_emtau_phi[n])
            TAUs.append(l1tau)
            if nt.trig_L1_emtau_EMIsol[n]<=4000:
                TAUIs.append(l1tau)
        for n in range(nt.tau_n): 
            tau=(nt.tau_pt[n],nt.tau_eta[n],nt.tau_phi[n])
            if (nt.tau_numTrack[n]>0 and nt.tau_numTrack[n]<7):  #[1-6] by default 2012, [1-3] default 2015
                if nt.tau_JetBDTSigLoose[n]==1:
                    taulooses.append(tau)
                if nt.tau_JetBDTSigMedium[n]==1:
                    taumediums.append(tau)

        TAUs=sorted(TAUs,reverse=True)
        TAUIs=sorted(TAUIs,reverse=True)
        taumediums=sorted(taumediums,reverse=True)
        taulooses=sorted(taulooses,reverse=True)
        EMs=sorted(EMs,reverse=True)
        EMHs=sorted(EMHs,reverse=True)
        EMHIs=sorted(EMHIs,reverse=True)
        elooses=sorted(elooses,reverse=True)
        emediums=sorted(emediums,reverse=True)
        etights=sorted(etights,reverse=True)
        eimediums=sorted(eimediums,reverse=True)
        eitights=sorted(eitights,reverse=True)
        gs=sorted(gs,reverse=True)
        glooses=sorted(glooses,reverse=True)
        gmediums=sorted(gmediums,reverse=True)

        jets=8*[(0,0,0)]
        JETs=8*[(0,0,0)]
        bjets=2*[(0,0,0)]
        fwdjets=2*[(0,0,0)]
        fwdejets=2*[(0,0,0)]
        EFjets=4*[(0,0,0)]
        fatjets=2*[(0,0,0)] 
        if jf==False:
            for E,eta,calib_tag in zip(nt.trig_EF_jet_E,nt.trig_EF_jet_eta,nt.trig_EF_jet_calibtags):
                if calib_tag ==  "AntiKt4_topo_calib_EMJES" and abs(eta)<3.2:
                    jet_Et = E / cosh(eta)
                    jetef=(jet_Et,eta,0.)
                    if abs(eta)<3.2:
                        EFjets.append(jetef)
            for n in range(nt.jet_AntiKt4LCTopo_n):
                jet_Et=nt.jet_AntiKt4LCTopo_E[n]/cosh(nt.jet_AntiKt4LCTopo_eta[n]) 
                jet=(jet_Et,nt.jet_AntiKt4LCTopo_eta[n],nt.jet_AntiKt4LCTopo_phi[n])
                if nt.jet_AntiKt4LCTopo_pt[n]>4*1000:
                    if abs(nt.jet_AntiKt4LCTopo_eta[n])<3.2:
                        jets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopo_flavor_weight_MV1[n]>0.7892) and abs(nt.jet_AntiKt4LCTopo_eta[n])<2.5:
                    # b_medium with 70% working point 
                        bjets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopo_eta[n])>3.2 and abs(nt.jet_AntiKt4LCTopo_eta[n])<4.4:
                        fwdjets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopo_eta[n])>2.4:
                        fwdejets.append(jet)
                        
            for n in range(nt.jet_AntiKt10LCTopo_n):
                fjet_Et=nt.jet_AntiKt10LCTopo_E[n]/cosh(nt.jet_AntiKt10LCTopo_eta[n]) 
                fjet=(fjet_Et,nt.jet_AntiKt10LCTopo_eta[n],nt.jet_AntiKt10LCTopo_phi[n])
                if nt.jet_AntiKt10LCTopo_pt[n]>4*1000:
                    if abs(nt.jet_AntiKt10LCTopo_eta[n])<3.2:
                        fatjets.append(fjet)            
        if jf==True:                
            for n in range(nt.jet_AntiKt4LCTopoJets_n):
                jet_Et=nt.jet_AntiKt4LCTopoJets_E[n]/cosh(nt.jet_AntiKt4LCTopoJets_eta[n]) 
                jet=(jet_Et,nt.jet_AntiKt4LCTopoJets_eta[n],nt.jet_AntiKt4LCTopoJets_phi[n])
                if jet_Et:
                    if abs(nt.jet_AntiKt4LCTopoJets_eta[n])<3.2:
                        jets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopoJets_flavor_weight_MV1[n]>0.7892) and abs(nt.jet_AntiKt4LCTopoJets_eta[n])<2.5:
                    # b_medium with 70% working point 
                        bjets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopoJets_eta[n])>3.2 and abs(nt.jet_AntiKt4LCTopoJets_eta[n])<4.4:
                        fwdjets.append(jet)
                    if abs(nt.jet_AntiKt4LCTopoJets_eta[n])>2.4:
                        fwdejets.append(jet)      
            
        for ni in range(len(nt.trig_L1_jet_et8x8)):
            jet=(nt.trig_L1_jet_et8x8[ni],nt.trig_L1_jet_eta[ni],nt.trig_L1_jet_phi[ni])
            if abs(nt.trig_L1_jet_eta[ni])<3.2:
                JETs.append(jet)

        jets=sorted(jets,reverse=True)
        JETs=sorted(JETs,reverse=True)
        bjets=sorted(bjets,reverse=True)                
        fwdejets=sorted(fwdejets,reverse=True)
        fwdjets=sorted(fwdjets,reverse=True)
        EFjets=sorted(EFjets,reverse=True)
        fatjets=sorted(fatjets,reverse=True)
        MUs=2*[(0,0,0)]
        mus=2*[(0,0,0)]
        muis=2*[(0,0,0)]
        mubs=2*[(0,0,0)]
        mues=2*[(0,0,0)]
        muecs=2*[(0,0,0)]
        if jf==False:
            for  n in range(nt.trig_L1_mu_n):
                #if nt.trig_L1_mu_vetoed[n]: continue
                l1mu=(nt.trig_L1_mu_pt[n],nt.trig_L1_mu_eta[n],nt.trig_L1_mu_phi[n])
                MUs.append(l1mu)    
            for n in range(nt.mu_muid_n):
                #if nt.trig_L1_mu_vetoed or nt.trig_L1_mu_vetoed[n]: continue
                mu=(nt.mu_muid_pt[n],nt.mu_muid_eta[n],nt.mu_muid_phi[n])
                if abs(nt.mu_muid_eta[n])<2.4 and nt.mu_muid_isCombinedMuon[n]>0:
                    mus.append(mu)
                    if nt.mu_muid_pt[n]>0. and nt.mu_muid_ptcone20[n]/nt.mu_muid_pt[n]<0.12:
                     #RelEFOnlyMedium (track-isolation)
                        muis.append(mu) 
                #if nt.trig_L1_mu_muid_source and abs(nt.trig_L1_mu_muid_source[n])==0:
                #    mubs.append(mu)
                #else:
                #    mues.append(mu)
                #if abs(nt.trig_L1_mu_muid_source[n])==1:
                #    muecs.append(mu)
        MUs=sorted(MUs,reverse=True)
        mus=sorted(mus,reverse=True)
        muis=sorted(muis,reverse=True)
        mubs=sorted(mubs,reverse=True)
        mues=sorted(mues,reverse=True)
        muecs=sorted(muecs,reverse=True)
        for var in vars:
            if var=='xe' or var=='EFxe' or var=='xeworandom' or var=='XE': continue # special handling above
            #if var=='EFJeightzero' or var=='EFJoneonezero' or var=='EFJtwotwozero' or var=='EFJtwoeightzero' or var=='EFXEeightzeroTtclcwloose'  or var=='EFMUtwofourI' or var=='EFMUthreesix': continue
            for idx, ext in enumerate(exts):
                name=var+ext
                for n in range(vars[var]):
                    globals()[name][n]=locals()[var+'s'][n][idx]

        newNT.Fill()
    
    newNT.Write("",ROOT.TObject.kOverwrite)   
    file.Close()


#filter("mu54_25ns_5files",mu54_25ns,bcid=True)

filter("eb",eb,bcid=False,ebsum=True)
#filter("jf17_8TeVnew1",jf17_8TeVnew1,bcid=True,jf=False)
#filter("jf17_8TeVnew2",jf17_8TeVnew2,bcid=True,jf=False)
#filter("jf17_8TeVnew3",jf17_8TeVnew3,bcid=True,jf=False)

#filter("jf17_14TeV1",jf17_14TeV1,bcid=True,jf=True)
#filter("jf17_14TeV2",jf17_14TeV2,bcid=True,jf=True)
#filter("jf17_14TeV3",jf17_14TeV3,bcid=True,jf=True)

#filter("Wplusenu",Wplusenu, bcid=True)
#filter("Wplusmunu",Wplusmunu,bcid=True)
#filter("Wminusenu",Wminusenu,bcid=True) 
#filter("Wminusmunu",Wminusmunu,bcid=True)
#filter("ttbar",ttbar,bcid=True)
#filter("Zee",Zee,bcid=True)
#filter("Zmumu",Zmumu,bcid=True)

#filter("Wplusenu_8TeV",Wplusenu_8TeV, bcid=True)
#filter("Wminusenu_8TeV",Wminusenu_8TeV, bcid=True)
#filter("Wplusmunu_8TeV",Wplusmunu_8TeV, bcid=True)
#filter("Wminusmunu_8TeV",Wminusmunu_8TeV, bcid=True)    
#filter("ttbar_8TeV",ttbar_8TeV,bcid=True)
#filter("Zee_8TeV",Zee_8TeV,bcid=True)
#filter("Zmumu_8TeV",Zmumu_8TeV,bcid=True)
