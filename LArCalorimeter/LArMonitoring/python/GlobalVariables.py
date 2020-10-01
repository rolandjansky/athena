# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from ROOT import TMath
import AthenaCommon.SystemOfUnits as Units

class LArDQGlobals(object):
      __slots__ = ('HVeta_EMB','HVphi_EMB','HVeta_EMEC','HVphi_EMEC','HVeta_HECFcal','HVphi_HECFcal',
                   'LB_Bins','LB_Min','LB_Max','BCID_Bins','BCID_Min','BCID_Max',
                   'colTime_BinWidth','colTime_Bins','colTime_Min','colTime_Max','avgColTime_BinWidth','avgColTime_Bins','avgColTime_Min','avgColTime_Max',
                   'etaCaloNoise_Bins','etaCaloNoise_Max','etaCaloNoise_Min','etaCaloNoise_FcalBins','etaCaloNoise_FcalMax','etaCaloNoise_FcalMin',
                   'FEB_Feedthrough','FEB_Slot','FEB_Crates',
                   'FEB_N_channels','FEB_channels_Min','FEB_channels_Max', 'N_FEBErrors','FEBErrors',
                   'N_FEB_Parttions_Max','N_Cells', 'N_FEB', 'N_Partitions','N_Gains','Gains',
                   'Feedthrough_Slot_Nbins','Feedthrough_Slot_range','Feedthrough_Slot_labels_Barrel','Feedthrough_Slot_labels_Endcap',
                   'ROD_Crates','ROD_Slots','ROS_link',
                   'N_DigitsSummary','DigitsSummary',
                   'N_SubDet','SubDet','Samples_Bins','Samples_Min','Samples_Max',
                   'Energy_Bins','Energy_Min','Energy_Max', 'DSPEnergy_Bins', 'DSPEnergy_Min', 'DSPEnergy_Max',
                   'DSPTime_Bins', 'DSPTime_Min', 'DSPTime_Max', 'DSPQuality_Bins', 'DSPQuality_Min', 'DSPQuality_Max',
                   'DSP1Energy_Bins', 'DSP1Energy_Min', 'DSP1Energy_Max','DSPRanges_Bins', 'DSPRanges_Min', 'DSPRanges_Max', 'DSPRanges',
                   'DSPEonEoff_Bins','DSPEonEoff_Max', 'DSPTonToff_Bins','DSPTonToff_Max', 'DSPQonQoff_Bins','DSPQonQoff_Max', 'DSPThr_Bins',
                   'L1Trig_Bins','L1Trig_Min','L1Trig_Max',
                   'Evt_Bins','Evt_Min','Evt_Max','Evt_labels',
                   'EvtRej_Bins','EvtRej_Min','EvtRej_Max','EvtRej_labels','EvtRejYield_labels','rejBits_Bins',
                   'defaultStreamNames',
                   'TotalNcell',
                   'noisyFEB_Bins','noisyFEB_Max','noisyFEB_Min',
                   'Cell_Time_Min','Cell_Time_Max','Emin','Emax','DEmin','DEmax','Qmin','Qmax','DQmin','DQmax',
                   'Tmin','Tmax','DTmin','DTmax','OnlineOffline','TTriggerTypeMax','MaxCellThresholdADC',
                   'CorruptionSource','Streams',
                   'Partitions','Sides','Variables','Layers','Cell_Variables',
                   'febsBarrelA','febsEndcapA','febsBarrelC','febsEndcapC')
      

lArDQGlobals = LArDQGlobals()

#HV eta-phi bins (used by LArAffectedRegions, bin values copied from there). NOTE: EMEC bins are provided as a list of bin boundaries (variable bin size), all other partitions are in format [Nbins,x_low,x_high]
#barrel
lArDQGlobals.HVeta_EMB = {"EMBAPS":[8, 0.,1.6],"EMBA":[7,0.,1.4]}
lArDQGlobals.HVphi_EMB = {"EMBAPS":[32,-TMath.Pi(),TMath.Pi()],"EMBA":[128,-TMath.Pi(),TMath.Pi()]}
for end in ["PS",""]:
    bins=lArDQGlobals.HVeta_EMB["EMBA"+end]
    lArDQGlobals.HVeta_EMB["EMBC"+end]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_EMB["EMBC"+end]=lArDQGlobals.HVphi_EMB["EMBA"+end]
#endcap
emecbinsA=[1.375,1.50,1.6,1.8,2.0,2.1,2.3,2.5,2.8,3.2]
emecbinsC=[-1*(bi+0.01) for bi in reversed(emecbinsA)]
phibinsPS=64
phibins=256
lArDQGlobals.HVeta_EMEC = {"EMECAPS":emecbinsA,"EMECA":emecbinsA,"EMECCPS":emecbinsC,"EMECC":emecbinsC} 
lArDQGlobals.HVphi_EMEC={}
ps_phirange = range(phibinsPS+1)
lArDQGlobals.HVphi_EMEC["EMECAPS"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibinsPS for x in ps_phirange]
phirange =range(phibins+1) 
lArDQGlobals.HVphi_EMEC["EMECA"]=[-TMath.Pi()+ x*2*TMath.Pi()/phibins for x in phirange]
lArDQGlobals.HVphi_EMEC["EMECCPS"]=lArDQGlobals.HVphi_EMEC["EMECAPS"]
lArDQGlobals.HVphi_EMEC["EMECC"]=lArDQGlobals.HVphi_EMEC["EMECA"]
#hec-fcal
lArDQGlobals.HVeta_HECFcal = {"HECA":[1,1.5, 3.2],"FCalA":[1, 2.9, 4.9]} #same for all layers
lArDQGlobals.HVphi_HECFcal = {"HECA":[32,-TMath.Pi(),TMath.Pi()],"FCalA":[16, -3.2, 3.2]} #same for all layers
for par in ["HEC","FCal"]:
    bins=lArDQGlobals.HVeta_HECFcal[par+"A"]
    lArDQGlobals.HVeta_HECFcal[par+"C"]=[bins[0],-1*(bins[2]+0.01),-1*(bins[1]+0.01)]
    lArDQGlobals.HVphi_HECFcal[par+"C"]=lArDQGlobals.HVphi_HECFcal[par+"A"]
#Lumi block ranges
lArDQGlobals.LB_Bins=3000
lArDQGlobals.LB_Min=0
lArDQGlobals.LB_Max=lArDQGlobals.LB_Bins+lArDQGlobals.LB_Min
#bunch crossing id ranges
lArDQGlobals.BCID_Bins=3564
lArDQGlobals.BCID_Min=0.5
lArDQGlobals.BCID_Max=lArDQGlobals.BCID_Min+lArDQGlobals.BCID_Bins
#LArCollisionTime (A-C) ranges
lArDQGlobals.colTime_BinWidth = 0.25*Units.nanosecond
lArDQGlobals.colTime_Min=-50.5*Units.nanosecond
lArDQGlobals.colTime_Max=50.5*Units.nanosecond
lArDQGlobals.colTime_Bins=(int)((lArDQGlobals.colTime_Max-lArDQGlobals.colTime_Min)//lArDQGlobals.colTime_BinWidth)
lArDQGlobals.avgColTime_BinWidth = 0.25*Units.nanosecond
lArDQGlobals.avgColTime_Min=-40*Units.nanosecond
lArDQGlobals.avgColTime_Max=40*Units.nanosecond
lArDQGlobals.avgColTime_Bins=(int)((lArDQGlobals.avgColTime_Max-lArDQGlobals.avgColTime_Min)//lArDQGlobals.avgColTime_BinWidth)
#eta ranges for caloNoise 
lArDQGlobals.etaCaloNoise_Bins=320
lArDQGlobals.etaCaloNoise_Max=3.2
lArDQGlobals.etaCaloNoise_Min=-3.2
lArDQGlobals.etaCaloNoise_FcalBins=500
lArDQGlobals.etaCaloNoise_FcalMax=5.0
lArDQGlobals.etaCaloNoise_FcalMin=-5.0

#FEB ranges
lArDQGlobals.FEB_Feedthrough={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}
lArDQGlobals.FEB_Slot={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,15],"EMECC":[1,15],"HECA":[1,15],"HECC":[1,15],"FCalA":[1,15],"FCalC":[1,15]}
lArDQGlobals.FEB_Crates={"EMBA":[1,448],"EMBC":[1,448],"EMECA":[1,375],"EMECC":[1,375],"HECA":[1,375],"HECC":[1,375],"FCalA":[1,375],"FCalC":[1,375]}
#Feb N channels
lArDQGlobals.FEB_N_channels=128
lArDQGlobals.FEB_channels_Min=-0.5
lArDQGlobals.FEB_channels_Max=lArDQGlobals.FEB_N_channels-0.5

#total number of FEB
lArDQGlobals.N_FEB=1524
lArDQGlobals.N_FEB_Parttions_Max=500
lArDQGlobals.N_Cells=200000

#feedthrough+slot ranges
lArDQGlobals.Feedthrough_Slot_Nbins={p : (lArDQGlobals.FEB_Feedthrough[p][1]+1)*lArDQGlobals.FEB_Slot[p][1] for p in lArDQGlobals.FEB_Feedthrough }
lArDQGlobals.Feedthrough_Slot_range={p : [lArDQGlobals.FEB_Feedthrough[p][0]*lArDQGlobals.FEB_Slot[p][1]+lArDQGlobals.FEB_Slot[p][0]-0.5,lArDQGlobals.FEB_Feedthrough[p][1]*lArDQGlobals.FEB_Slot[p][1]+lArDQGlobals.FEB_Slot[p][1]+0.5] for p in lArDQGlobals.FEB_Feedthrough }
range_0_32=range(32)
range_1_15=range(1,15)
lArDQGlobals.Feedthrough_Slot_labels_Barrel=[str(If) if Is==1 else '' for If in range_0_32 for Is in range_1_15]
range_0_25=range(25)
range_1_16=range(1,16)
lArDQGlobals.Feedthrough_Slot_labels_Endcap=[str(If) if Is==1 else '' for If in range_0_25 for Is in range_1_16]

#feedthrough+slot ranges for noise correlation plots
lArDQGlobals.Sides=["A","C"]


#numbers from LArCalorimeter/LArIdentifier/LArIdentifier/LArOnlineID_Base.h
feedthroughString="ft"
slotString="slot"
barrelString="Barrel"
endcapString="Endcap"
lArDQGlobals.febsBarrelA=[barrelString+"A"+feedthroughString+str(i_ft).zfill(2)+slotString+str(i_slot).zfill(2) for i_ft in range_0_32 for i_slot in range_1_15]
lArDQGlobals.febsBarrelC=[barrelString+"C"+feedthroughString+str(i_ft).zfill(2)+slotString+str(i_slot).zfill(2) for i_ft in range_0_32 for i_slot in range_1_15]
lArDQGlobals.febsEndcapA=[endcapString+"A"+feedthroughString+str(i_ft).zfill(2)+slotString+str(i_slot).zfill(2) for i_ft in range_0_25 for i_slot in range_1_16]
lArDQGlobals.febsEndcapC=[endcapString+"C"+feedthroughString+str(i_ft).zfill(2)+slotString+str(i_slot).zfill(2) for i_ft in range_0_25 for i_slot in range_1_16]


#ROD ranges
lArDQGlobals.ROD_Crates={"EMBA":[1,4],"EMBC":[1,4],"EMECA":[1,3],"EMECC":[1,3],"HECA":[1],"HECC":[1],"FCalA":[1],"FCalC":[1]}
lArDQGlobals.ROD_Slots={"EMBA":[1,14],"EMBC":[1,14],"EMECA":[1,13],"EMECC":[1,13],"HECA":[1,6],"HECC":[1,6],"FCalA":[1,4],"FCalC":[1,4]}

#ROS ranges
lArDQGlobals.ROS_link={"EMBA":[0,31],"EMBC":[0,31],"EMECA":[0,24],"EMECC":[0,24],"HECA":[0,24],"HECC":[0,24],"FCalA":[0,24],"FCalC":[0,24]}

#FEB errors
lArDQGlobals.N_FEBErrors=13
lArDQGlobals.FEBErrors=["Parity","BCID","Sample Header","EVTID","SCAC status","Sca out of range","Gain mismatch","Type mismatch","# of samples","Empty data block","Checksum / block size","Missing header","Bad gain"]

#number of cells  
lArDQGlobals.TotalNcell=182468

#partitions
lArDQGlobals.N_Partitions=8
lArDQGlobals.Partitions=["EMBC","EMBA","EMECC","EMECA","HECC","HECA","FCalC","FCalA"]
lArDQGlobals.N_SubDet=4
lArDQGlobals.SubDet=["EMB","EMEC","HEC","FCal"]
lArDQGlobals.N_Gains=3
lArDQGlobals.Gains=["HIGH","MEDIUM","LOW"]

#digits summary
lArDQGlobals.N_DigitsSummary=4
lArDQGlobals.DigitsSummary=["OutOfRange","Saturation","Null Digits","Mean Time"]

#samples range
lArDQGlobals.Samples_Bins=32
lArDQGlobals.Samples_Min=-0.5
lArDQGlobals.Samples_Max=lArDQGlobals.Samples_Bins-0.5

#Samples
#check if any algo uses this lArDQGlobals.Sample_N_Min=0
#check if any algo uses this lArDQGlobals.Sample_N_Max=32

#Cell time (in ns) 
lArDQGlobals.Cell_Time_Min=-200
lArDQGlobals.Cell_Time_Max=200

#Energies ranges
lArDQGlobals.Emin=-20000.0
lArDQGlobals.Emax=20000.0

#Difference Energies
lArDQGlobals.DEmin=-512
lArDQGlobals.DEmax=512

#Qfactor range
lArDQGlobals.Qmin=0
lArDQGlobals.Qmax=6553

#Differences Q factor
lArDQGlobals.DQmin=-3000
lArDQGlobals.DQmax=1000

#Time ranges (ps)    
lArDQGlobals.Tmin=-80000
lArDQGlobals.Tmax=80000

#Time ranges (ps)
lArDQGlobals.Tmin=-80000
lArDQGlobals.Tmax=80000

#Diff Time ranges (ps)
lArDQGlobals.DTmin=-800
lArDQGlobals.DTmax=800

#ADC energy range
lArDQGlobals.Energy_Bins=300
lArDQGlobals.Energy_Min=0.
lArDQGlobals.Energy_Max=3000.

#Cell threshold ADC counts
lArDQGlobals.MaxCellThresholdADC=1900

#DSP  diff
lArDQGlobals.DSPEnergy_Bins=400
lArDQGlobals.DSPEnergy_Min=-40.
lArDQGlobals.DSPEnergy_Max=40.
lArDQGlobals.DSPTime_Bins=400
lArDQGlobals.DSPTime_Min=-800.
lArDQGlobals.DSPTime_Max=800.
lArDQGlobals.DSPQuality_Bins=400
lArDQGlobals.DSPQuality_Min=-3000.
lArDQGlobals.DSPQuality_Max=3000.
lArDQGlobals.DSP1Energy_Bins=5000
lArDQGlobals.DSP1Energy_Min=-515.
lArDQGlobals.DSP1Energy_Max=515.
lArDQGlobals.DSPRanges_Bins=4
lArDQGlobals.DSPRanges_Min=0
lArDQGlobals.DSPRanges_Max=4
lArDQGlobals.DSPRanges=["E < 2^{13} MeV","E < 2^{16} MeV","E < 2^{19} MeV","E < 2^{22} MeV"]
lArDQGlobals.DSPEonEoff_Bins=300
lArDQGlobals.DSPEonEoff_Max=20000.
lArDQGlobals.DSPTonToff_Bins=300
lArDQGlobals.DSPTonToff_Max=80000.
lArDQGlobals.DSPQonQoff_Bins=300
lArDQGlobals.DSPQonQoff_Max=66000.
lArDQGlobals.DSPThr_Bins=2000

#trigger word range
lArDQGlobals.L1Trig_Bins=256
lArDQGlobals.L1Trig_Min=-0.5
lArDQGlobals.L1Trig_Max=lArDQGlobals.L1Trig_Bins-0.5

#triggerWord
lArDQGlobals.TTriggerTypeMax=256

#Corruption errors
lArDQGlobals.CorruptionSource=["Parity","BCID Mismatch","Sample header","EVT ID","SCAC status","Sca out of range","Gain mismatch","Type mismatch","# of samples","Empty data block","Checksum /block size","Missing header","Bad gain"]

#Event types histo
lArDQGlobals.Evt_Bins = 15
lArDQGlobals.Evt_Min = -0.5
lArDQGlobals.Evt_Max = 14.5
lArDQGlobals.Evt_labels = ["","Raw data - Transparent","","Result - Physic","","","Calibration","","","Pedestals","","","","Raw data+Result"]
lArDQGlobals.EvtRej_Bins = 7 
lArDQGlobals.EvtRej_Min = 0.5
lArDQGlobals.EvtRej_Max = 7.5
lArDQGlobals.EvtRej_labels = [">=1 FEB in error",">=4 FEBs in error","LArError_DATACORRUPTED","LArError_DATACORRUPTEDVETO","LArError_NOISEBURSTVETO","Accepted","Total"]
lArDQGlobals.EvtRejYield_labels = [">=1 FEB in error",">=4 FEBs in error","LArError_DATACORRUPTED","LArError_DATACORRUPTEDVETO","LArError_NOISEBURSTVETO","Accepted"]

lArDQGlobals.rejBits_Bins=8192

lArDQGlobals.defaultStreamNames = ["express","Main","CosmicCalo","L1Calo","L1Topo","ZeroBias","Standby","LArCells","LArCellsEmpty","Background","others"]

#NoisyRO
lArDQGlobals.noisyFEB_Bins=51
lArDQGlobals.noisyFEB_Min=-0.5
lArDQGlobals.noisyFEB_Max=50.5


#Gains
lArDQGlobals.Gains=["Low Gain","Medium Gain","High Gain"]

#Creation of the dictionnary
lArDQGlobals.Cell_Variables={}
lArDQGlobals.Variables=["etaRange","phiRange","etaNbin", "phiNbin","etasize","etaCellSize","etaMin"]
lArDQGlobals.Layers=["0","1","2","3"]
for Variable in lArDQGlobals.Variables :
    lArDQGlobals.Cell_Variables[Variable]={}
    for sdet in lArDQGlobals.SubDet :
        lArDQGlobals.Cell_Variables[Variable][sdet]={}
        for Side in lArDQGlobals.Sides :
            lArDQGlobals.Cell_Variables[Variable][sdet][Side]={}
            for Layer in lArDQGlobals.Layers:
                lArDQGlobals.Cell_Variables[Variable][sdet][Side][Layer]=list()

#Number of Bins in phi
lArDQGlobals.Cell_Variables["phiNbin"]["EMB"]["A"]= {"0":64,"1":256,"2":256,"3":256}
lArDQGlobals.Cell_Variables["phiNbin"]["EMB"]["C"]= lArDQGlobals.Cell_Variables["phiNbin"]["EMB"]["A"]

lArDQGlobals.Cell_Variables["phiNbin"]["EMEC"]["A"]= {"0":64,"1":64,"2":256,"3":256}
lArDQGlobals.Cell_Variables["phiNbin"]["EMEC"]["C"]= lArDQGlobals.Cell_Variables["phiNbin"]["EMEC"]["A"]

lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]= {"0":64,"1":64,"2":64,"3":64}
lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["C"]= lArDQGlobals.Cell_Variables["phiNbin"]["HEC"]["A"]

lArDQGlobals.Cell_Variables["phiNbin"]["FCal"]["A"]= {"1":16,"2":16,"3":16}
lArDQGlobals.Cell_Variables["phiNbin"]["FCal"]["C"]= lArDQGlobals.Cell_Variables["phiNbin"]["FCal"]["A"]


#Number of bins in eta in each range of cells (defining only for A side since C side is symmetric)
#To be read as follow :
#The EMB Layer 1 have 3 types of cell sizes
#The first bin start at eta=0.0, and have a size of 0.025/4
#The bins [2;446], have a size of 0.025/8
#The bins [447;450, have a size of 0.025

lArDQGlobals.Cell_Variables["etaNbin"]["EMB"]["A"]=    {"0":[0,61],"1":[0,1,446,450],"2":[0,56,57],"3":[0,27]}
lArDQGlobals.Cell_Variables["etaMin" ]["EMB"]["A"]=    {"0":0.0,"1":0.0,"2":0.0,"3":0.0}
lArDQGlobals.Cell_Variables["etaCellSize"]["EMB"]["A"]={"0":[0.025],"1":[0.025/4,0.025/8,0.025],"2":[0.025,0.05],"3":[0.05]}

lArDQGlobals.Cell_Variables["etaNbin"]["EMEC"]["A"]=    {"0":[0,12],"1":[0,1,3,99,147,211,215,223],"2":[0,1,43,51],"3":[0,20]}
lArDQGlobals.Cell_Variables["etaMin" ]["EMEC"]["A"]=    {"0":1.5,"1":1.375,"2":1.375,"3":1.5}
lArDQGlobals.Cell_Variables["etaCellSize"]["EMEC"]["A"]={"0":[0.025],"1":[0.05,0.025,0.025/8,0.025/6,0.025/4,0.025,0.1],"2":[0.05,0.025,0.1],"3":[0.05]}

lArDQGlobals.Cell_Variables["etaNbin"]["HEC"]["A"]=    {"0":[0,10,14],"1":[0,10,13],"2":[0,9,12],"3":[0,8,12]}
lArDQGlobals.Cell_Variables["etaMin" ]["HEC"]["A"]=    {"0":1.5,"1":1.5,"2":1.6,"3":1.7}
lArDQGlobals.Cell_Variables["etaCellSize"]["HEC"]["A"]={"0":[0.1,0.2],"1":[0.1,0.2],"2":[0.1,0.2],"3":[0.1,0.2]}

lArDQGlobals.Cell_Variables["etaNbin"]["FCal"]["A"]=    {"1":[0,63],"2":[0,32],"3":[0,16]}
lArDQGlobals.Cell_Variables["etaMin" ]["FCal"]["A"]=    {"1":-0.5,"2":-0.5,"3":-0.5}
lArDQGlobals.Cell_Variables["etaCellSize"]["FCal"]["A"]={"1":[1],"2":[1],"3":[1]}


###################
#Filling Phi ranges
for Layer in lArDQGlobals.Layers :
    for sdet in lArDQGlobals.SubDet:
        if sdet=="FCal": 
            if Layer=="0": #there's only fcal1, fcal2 and fcal3
                continue
            else:
                phi_range=range(lArDQGlobals.Cell_Variables["phiNbin"][sdet]["A"][Layer]+1)
                lArDQGlobals.Cell_Variables["phiRange"][sdet]["A"][Layer]=[-0.5+x for x in phi_range]
                lArDQGlobals.Cell_Variables["phiRange"][sdet]["C"]=lArDQGlobals.Cell_Variables["phiRange"][sdet]["A"]
        else: #all other partitions
            phi_range=range(lArDQGlobals.Cell_Variables["phiNbin"][sdet]["A"][Layer]+1)
            lArDQGlobals.Cell_Variables["phiRange"][sdet]["A"][Layer]=[-TMath.Pi()+ x*2*TMath.Pi()/lArDQGlobals.Cell_Variables["phiNbin"][sdet]["A"][Layer] for x in phi_range]
            lArDQGlobals.Cell_Variables["phiRange"][sdet]["C"]=lArDQGlobals.Cell_Variables["phiRange"][sdet]["A"]
            pass
        pass #partition loop
    pass #layer loop  
            
###################
#Filling Eta range

for sdet in lArDQGlobals.SubDet:
      for Lay in lArDQGlobals.Layers:
            if sdet=="FCal" and Lay=="0":
                  continue
            Ranges=lArDQGlobals.Cell_Variables["etaNbin"][sdet]["A"][Lay]
            Sizes=lArDQGlobals.Cell_Variables["etaCellSize"][sdet]["A"][Lay]
            if not len(Ranges)-len(Sizes)==1 :
                  print("Bad list of ranges and size please check")
                  continue
                  
            etamin=lArDQGlobals.Cell_Variables["etaMin"][sdet]["A"][Lay]
            currange=[etamin]
            for k in range(len(Ranges)-1) :
                  eta_range = range(1,Ranges[k+1]-Ranges[k]+1)
                  currange+=[round(currange[-1] + x * Sizes[k],5) for x in eta_range]
                  
                  lArDQGlobals.Cell_Variables["etaRange"][sdet]["A"][Lay]=currange
                  #The C side is just the symmeteric of the A side
                  if sdet=="FCal":
                        lArDQGlobals.Cell_Variables["etaRange"][sdet]["C"][Lay] = lArDQGlobals.Cell_Variables["etaRange"][sdet]["A"][Lay]
                  else: #all other partitions
                        lArDQGlobals.Cell_Variables["etaRange"][sdet]["C"][Lay] =list(map(lambda x: x*-1,lArDQGlobals.Cell_Variables["etaRange"][sdet]["A"][Lay]))[::-1]
                              
